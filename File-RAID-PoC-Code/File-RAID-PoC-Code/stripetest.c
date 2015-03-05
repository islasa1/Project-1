#include <stdio.h>
#include <stdlib.h>

#include "raidlib.h"

int main(int argc, char *argv[])
{
    int bytesWritten, bytesRestored;

    if(argc < 3)
    {
        printf("useage: stripetest inputfile outputfile\n");
        exit(-1);
    }

    bytesWritten=stripeFile(argv[1], 0); 
    bytesRestored=restoreFile(argv[2], 0, bytesWritten); 
    printf("FINISHED\n");
        
}
