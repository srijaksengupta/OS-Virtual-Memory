#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

char *ram[40];

char *getFromRAM(int cell)
{
	return ram[cell];
}

void addToRAM(int cell, char* buffer)
{
    ram[cell] = strdup(buffer);
}

void clearRAM(int frameNumber)
{
    for (int i = frameNumber*4; i < (frameNumber*4)+4; i++)
    {
        ram[i] = NULL;
    }
}