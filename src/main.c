/* Main interpreter file - draft */
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

int main(int argc, char *argv[])
{
    int ec = IFJ_OK;

    if(argc >= 2) {
        printf("Source file: %s\n", argv[1]);
    } else {
        printf("No source file specified.\n");
        ec = IFJ_INTERNAL_ERR;
    }

   return ec;
}
