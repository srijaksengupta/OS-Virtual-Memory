#ifndef RAM_H
#define RAM_H

char *ram[40];
char *getFromRAM(int cell);
void addToRAM(int cell, char* buffer);
void clearRAM(int frameNumber);

#endif