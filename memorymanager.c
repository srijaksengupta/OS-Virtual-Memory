#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "kernel.h"
#include "ram.h"
#include "pcb.h"

int pageCount = 1;

int launcher(FILE *p)
{

//    Copying the entire file into the backing store begins here
    char fileName[100];
    char fileNumber[10];
    sprintf(fileNumber, "%d", pageCount);
    strcpy(fileName, "file");
    strcat(fileName, fileNumber);
    strcat(fileName, ".txt");
    FILE *newFile = fopen(fileName, "w");

    char buffer[1000];
    while (!feof(p))
    {
        if (fgets(buffer, 999, p) != NULL)
        {
            fprintf(newFile, buffer, 999);
        }
    }

    char c[100];
    strcpy(c, "chmod +rwx ");
    strcat(c, fileName);
    system(c);

    char command[100];
    strcpy(command, "mv ");
    strcat(command, fileName);
    strcat(command, " BackingStore/");
    system(command);
//    Copying the entire file into the backing store ends here

//    Here, we close the file pointer pointing to the original file
    fclose(p);
    fclose(newFile);

//    Opening the file in the BackingStore
    char fileToOpen[100];
    strcpy(fileToOpen, "BackingStore/");
    strcat(fileToOpen, fileName);
    FILE *current_file = fopen(fileToOpen, "r");

//    If the launch of the program is not successful, we return an error code 0
    if(!current_file)
    {
        return 0;
    }

    //    Here, we call countTotalPages to count the number of pages with respect to this program
    int pages = countTotalPages(current_file);

//    Here, we create a pcb for this program and add it to the Ready Queue
    struct PCB *pcb = makePCB(pages, pageCount);
    addToReady(pcb);
    if(!pcb)
    {
        return 0;
    }

    for(int x = 0; x<2; x++)
    {
        if(x == pages)
        {
            break;
        }

        int ramFrameIdx = findFrame();
//        printf("%d\n", ramFrameIdx);
        int victimFrame = -1;
        if(ramFrameIdx == -1)
        {
            victimFrame = findVictim(getTail());
            loadPage(x, current_file, victimFrame);
            if(x == 0)
            {
                getTail()->PC = victimFrame*4;
            }
            updatePageTable(pcb, x, victimFrame, victimFrame);
        }
        else
        {
            loadPage(x, current_file, ramFrameIdx);
            if(x == 0)
            {
                getTail()->PC = ramFrameIdx*4;
            }
            updatePageTable(pcb, x, ramFrameIdx, victimFrame);
        }
    }

    pageCount = pageCount + 1;
    return 1;
}

int countTotalPages(FILE *f)
{
    int totalPages = 0;
    int lines = 0;

    char buffer[1000];
    while (!feof(f))
    {
        if (fgets(buffer, 999, f) != NULL)
        {
            lines = lines + 1;
        }
    }

//    Setting the file position to beginning of the file of the stream f
    rewind(f);

    if(lines <=4)
    {
        totalPages = 1;
    }
    else if(lines%4 != 0)
    {
        totalPages = (lines / 4) + 1;
    }
    else
    {
        totalPages = lines / 4;
    }
    return totalPages;
}

int loadPage(int pageNumber, FILE *f, int frameNumber)
{
    rewind(f);
    char buffer[1000];
//    printf("%d\n", frameNumber);
    for(int x = 0; x<(pageNumber)*4; x++)
    {
        fgets(buffer, 999, f);
    }

    for(int i=frameNumber*4 ; i<((frameNumber*4)+4) ; i++)
    {
        if (fgets(buffer, 999, f) != NULL)
        {
            addToRAM(i, buffer);
//            ram[i] = strdup(buffer);
        }
    }
    rewind(f);
    return 0;
}

int findFrame()
{
    for(int x = 0; x<40; x=x+4)
    {
        if (ram[x] == NULL)
        {
//            printf("%d\n", x);
            return x/4;
        }
    }
    return -1;
}

int pageTableCheck(int pageNumber, struct PCB *p)
{
    if(p->pageTable[pageNumber] != -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int findVictim(struct PCB *p)
{
//    Using current time as seed for random generator
    srand(time(0));
//    Here, we use a random number generator to pick a random frame number
    int num = rand() % 10;
    if(pageTableCheck(num, p))
    {
        return num;
    }
    else
    {
        while(!(pageTableCheck((++num)%10, p)))
        {
            continue;
        }
    }
    return num;
}

int updatePageTable(struct PCB *p, int pageNumber, int frameNumber, int victimFrame)
{
    if(victimFrame != -1)
    {
        updateVictimPageTable(victimFrame);
    }
    else
    {
        p->pageTable[pageNumber] = frameNumber;
    }
    return frameNumber;
}

