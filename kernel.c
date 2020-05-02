#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "shell.h"
#include "interpreter.h"
#include "memorymanager.h"
#include "ram.h"
#include "cpu.h"
#include "pcb.h"

struct CPU *cpu;
struct PCB *head, *tail;

struct PCB *tp;

struct PCB *getTail()
{
    return tail;
}

void addToReady(struct PCB *pcb)
{
//    Here, we add the PCB to the tail of the Ready Queue
	pcb->next = NULL;
	if (tail != NULL)
	{
	    tail->next = pcb;
	    tail = pcb;
	}
	else
	{
	    head = pcb;
		tail = pcb;
	}
}

void removeFromReady() {
//    Here, we remove the PCB from the Ready Queue
	if (head != NULL)
	{
	    struct PCB *t = head;
	    head = head->next;
	    if (head == NULL)
		    tail = NULL;
	}
	else
	{
	    head = NULL;
		tail = NULL;
	}
}

void updateVictimPageTable(int victimFrame)
{
    struct PCB *p = head;
    while (p!=NULL)
    {
        int index = -1;
        for(int i = 0; i < 10; i++)
        {
            if(p->pageTable[i] == victimFrame)
            {
                index = i;
            }
        }
        if (index != -1)
        {
            p->pageTable[index] = -1;
            break;
        }
        p = p->next;
    }
}

void scheduler()
{
//    Here, the scheduler function checks if the ready queue is empty.
//    If it is not empty, then control moves into the while loop
    while (head != NULL)
    {
		tp = head;
		removeFromReady();
		tp->next = NULL;

		if (tp->pageTable[tp->PC_page] == -1)
        {
            char fileName[100];
            char fileNumber[10];
            sprintf(fileNumber, "%d", tp->PID);
            strcpy(fileName, "file");
            strcat(fileName, fileNumber);
            strcat(fileName, ".txt");

//            Opening the file in the BackingStore
            char fileToOpen[100];
            strcpy(fileToOpen, "BackingStore/");
            strcat(fileToOpen, fileName);
            FILE *cur = fopen(fileToOpen, "r");

//            If the launch of the program is not successful, we return
            if(!cur)
            {
                return;
            }

            int ramFrameIdx = findFrame();
            int victimFrame = -1;
            if(ramFrameIdx == -1)
            {
                victimFrame = findVictim(tp);
                loadPage(tp->PC_page, cur, victimFrame);
                tp->PC = victimFrame*4;
                updatePageTable(tp, tp->PC_page, victimFrame, victimFrame);
            }
            else
            {
                loadPage(tp->PC_page, cur, ramFrameIdx);
                tp->PC = ramFrameIdx*4;
                updatePageTable(tp, tp->PC_page, ramFrameIdx, victimFrame);
            }
            fclose(cur);
            addToReady(tp);
        }
        else
        {
            cpu->IP = tp->pageTable[tp->PC_page] * 4;
            cpu->offset = tp->PC_offset;
//            printf("\n");
//            printf("%d\n", cpu->IP);
//            printf("%d\n", cpu->quanta);
//            printf("%d\n", cpu->offset);
//            printf("%s\n", cpu->IR);
            int err = -1;
            if(getFromRAM(cpu->IP+cpu->offset)!=NULL)
            {
                err = run2(cpu);
            }
            if(err == -1)
            {
//                Here, the program terminates and we clear the ram cells occupied by lines of this program
                for (int i = 0; i < 10; i++)
                {
                    int frameNumber = tp->pageTable[i];
                    if (frameNumber != -1)
                    {
                        clearRAM(frameNumber);
                    }
                }
                char fileName[100];
                char fileNumber[10];
                sprintf(fileNumber, "%d", tp->PID);
                strcpy(fileName, "file");
                strcat(fileName, fileNumber);
                strcat(fileName, ".txt");

                char fileToRemove[100];
                strcpy(fileToRemove, "BackingStore/");
                strcat(fileToRemove, fileName);

//                Here, we handle the extra addition of deleting the file in the backing store
                char command[100] = "rm ";
                strcat(command, fileToRemove);
                system(command);
                free(tp);
            }
            else
            {
//                Since the PCB was interrupted, it has lost it quanta, even when there were some quanta left
//                Thus, we store everything back into the PCB and place the PCB at the back of the Ready queue
                if(err == 0)
                {
                    tp->PC = cpu->IP;
                    tp->PC_offset = cpu->offset;
                }
                addToReady(tp);
            }
        }
    }
}

int pageFault()
{
//    Determining the next page: PC_page++
    tp->PC_page++;
//    If PC_page is beyond pages_max then the program terminates
    if(tp->PC_page >= tp->pages_max)
    {
        return -1;
    }
    else
    {
        if(!pageTableCheck(tp->PC_page, tp))
        {
//            If pageTable[PC_page] is valid then we have the frame number and can do PC=ram[frame] and reset PC_offset to zero
            tp->PC = tp->pageTable[tp->PC_page] * 4;
            tp->PC_offset = 0;
        }
        else
        {
//            Here, the pageTable[PC_page] is NOT valid and thus, we need to find the page on disk
//            Thus, we need to  update the PCB page table and possibly the victim’s page table
            char fileName[100];
            char fileNumber[10];
            sprintf(fileNumber, "%d", tp->PID);
            strcpy(fileName, "file");
            strcat(fileName, fileNumber);
            strcat(fileName, ".txt");

//            Opening the file in the BackingStore
            char fileToOpen[100];
            strcpy(fileToOpen, "BackingStore/");
            strcat(fileToOpen, fileName);
            FILE *cur = fopen(fileToOpen, "r");

//            If the launch of the program is not successful, we return an error code 0
            if(!cur)
            {
                return 0;
            }

            int ramFrameIdx = findFrame();
            int victimFrame = -1;
            if(ramFrameIdx == -1)
            {
                victimFrame = findVictim(tp);
                loadPage(tp->PC_page, cur, victimFrame);
                tp->PC = victimFrame*4;
                updatePageTable(tp, tp->PC_page, victimFrame, victimFrame);
                tp->PC_offset = 0;
            }
            else
            {
                loadPage(tp->PC_page, cur, ramFrameIdx);
                tp->PC = ramFrameIdx;
                updatePageTable(tp, tp->PC_page, ramFrameIdx, victimFrame);
                tp->PC_offset = 0;
            }
            fclose(cur);
            tp->PC = tp->pageTable[tp->PC_page] * 4;
            tp->PC_offset = 0;
        }
        return 1;
    }
}

void boot()
{
    for(int i=0; i<40; i++)
    {
        ram[i] = NULL;
    }

    DIR* d ;
    d = opendir("BackingStore");
    if(d)
    {
        system("rm -rf BackingStore/ && mkdir BackingStore");
    }
    else
    {
        system("mkdir BackingStore");
    }
    system("chmod +rwx BackingStore");
}

int kernel()
{
    head = tail = NULL;
	cpu = (struct CPU*)malloc(sizeof(struct CPU));
//	We need to set the quanta equal to 2 as described in the assignment description
	cpu->quanta = 2;
	return shellUI();
}

int main()
{
    int error=0;
    // First : actions performed by boot
    boot();
    // Second: actions performed by kernel
    error = kernel();
    return error;
}