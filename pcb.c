#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "pcb.h"


struct PCB *makePCB(int pages, int PID)
{
//    Here, we create a PCB instance using malloc
	struct PCB *pcb = (struct PCB *)malloc(sizeof(struct PCB));
	pcb->PC = 0;
    pcb->pages_max = pages;
    pcb->PC_page = 0;
    pcb->PC_offset = 0;
    pcb->PID = PID;
    for(int i = 0; i<10; i++)
    {
        pcb->pageTable[i] = -1;
    }
	return pcb;
}