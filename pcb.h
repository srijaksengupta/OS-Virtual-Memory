#ifndef PCB_H
#define PCB_H

struct PCB
{
	int PC;
	int pageTable[10];
	int PC_page, PC_offset, pages_max;
	int PID;
	struct PCB *next;
};

struct PCB *makePCB(int pages, int PID);

#endif