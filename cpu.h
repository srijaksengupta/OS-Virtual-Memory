#ifndef CPU_H
#define CPU_H

struct CPU
{
	int IP;
	int offset;
	char *IR;
	int quanta;
};

int run2(struct CPU *cpu);

#endif