#define TRUE 1
#define FALSE 0

#include "proc.h"
#include "string.h"

int main()
{
    int berhasil;
    char curdir;
    char argc;
    char argv[1][128];
    int pid, i;
    char temp;
    enableInterrupts();
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    interrupt(0x21, 0x23, 0, argv[0], 0);
    pid = argv[0][0] - '0';
    for(i = 0; i < MAX_SEGMENTS; i++)
    {

    }
    //Keluar dari sini
    interrupt(0x21, 0x07, &temp, 0, 0);
}