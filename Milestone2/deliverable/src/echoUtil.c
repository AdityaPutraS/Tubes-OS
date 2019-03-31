#define TRUE 1
#define FALSE 0

int main()
{
    int i;
    char curdir;
    char argc;
    char argv[64][128];
    char temp;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    for (i = 0; i < argc; ++i)
    {
        interrupt(0x21, 0x23, i, argv[i], 0);
        interrupt(0x21, (0 << 8) | 0x0, argv[i], 0, 0);
        interrupt(0x21, (0 << 8) | 0x0, " ", 0, 0);
    }
    interrupt(0x21, (0 << 8) | 0x0, "", 1, 0);
    //Keluar dari sini
    interrupt(0x21, 0x07, &temp, 0, 0);
}