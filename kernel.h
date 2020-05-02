#ifndef KERNEL_H
#define KERNEL_H

struct PCB *tp;
struct PCB *getTail();
void addToReady(struct PCB *pcb);
void updateVictimPageTable(int victimFrame);
void scheduler();
int pageFault();

#endif