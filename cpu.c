#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "shell.h"
#include "interpreter.h"
#include "kernel.h"
#include "memorymanager.h"
#include "ram.h"
#include "cpu.h"

int run2(struct CPU *cpu)
{
    for(int i = 0; i<cpu->quanta;i++)
    {
        if(getFromRAM(cpu->IP+cpu->offset)!=NULL)
        {
            cpu->IR = getFromRAM(cpu->IP+cpu->offset);
//            printf("%d\n", cpu->IP+cpu->offset);
            interpret(cpu->IR);
            cpu->offset++;
        }
        else
        {
            return -1;
        }
        if(cpu->offset == 4)
        {
            return pageFault();
        }
    }

    if(cpu->offset == 4)
    {
        return pageFault();
    }
    else
    {
        return 0;
    }
}