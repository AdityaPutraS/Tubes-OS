#define TRUE 1
#define FALSE 0

void pS(char *string, int newLine);
void pI(int i, int newLine);

int main()
{
    int i;
    char curdir;
    char argc;
    char argv[4][255];
    char temp;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    pI(argc, TRUE);
    // for (i = 0; i < argc; ++i)
    // {
    //     interrupt(0x21, 0x23, i, argv[i], 0);
    //     interrupt(0x21, (0 << 8) | 0x0, argv[i], 0, 0);
    // }
    //Keluar dari sini
    interrupt(0x21, 0x07, &temp, 0, 0);
}

void pS(char *string, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x0, string, newLine, 0);
}

void pI(int i, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x24, i, newLine, 0);
}