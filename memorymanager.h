#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include "pcb.h"

int launcher(FILE *p);
int countTotalPages(FILE *f);
int loadPage(int pageNumber, FILE *f, int frameNumber);
int findFrame();
int pageTableCheck(int pageNumber, struct PCB *p);
int findVictim(struct PCB *p);
int updatePageTable(struct PCB *p, int pageNumber, int frameNumber, int victimFrame);

#endif