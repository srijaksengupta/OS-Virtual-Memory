#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"
#include "interpreter.h"


int shellUI(int argc, const char *argv[])
{
    printf("Kernel 1.0 loaded!\n"
            "Welcome to the Srijak shell!\n"
            "Version 3.0 Created April 2020\n");

    shell_memory_initialize();

    while (!feof(stdin))
    {
        printf("$ ");
        fflush(stdout);

        char *line = NULL;
        size_t linecap = 0;
        if (getline(&line, &linecap, stdin) == -1)
            break;

        (void)interpret(line);
        free(line);
    }

    shell_memory_destory();

    return 0;
}