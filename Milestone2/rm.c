#define TRUE 1
#define FALSE 0

void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
//void interrupt(int type, int ax, int bx, int cx, int dx);

int main()
{
    int b1, b2;
    char curdir;
    char argc;
    char argv[1][128];
    char temp;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    interrupt(0x21, 0x23, 0, argv[0], 0);   // nama folder / path relatif folder
    // pS("Mencoba hapus : ", FALSE);
    // pS(argv[0], TRUE);
    interrupt(0x21, curdir << 8 | 0x0A, argv[0], &b1, 0);
    interrupt(0x21, curdir << 8 | 0x09, argv[0], &b2, 0);
    if(b1 == -1 && b2 == -1)
    {
        interrupt(0x21, 0x0, "PATH NOT_FOUND", 1, 0);
    }else if(b1 == 0 || b2 == 0){
        interrupt(0x21, 0x0, "SUCCESS", 1, 0);
    }else{
         interrupt(0x21, 0x0, "FAILED, UNKNOWN REASON", 1, 0);
    }
    
    //Keluar dari sini
    interrupt(0x21, 0x20, curdir, 0, 0);
	interrupt(0x21, curdir << 8 | 0x6, "shell", 0x2000, &temp);
}

void pS(char *string, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x0, string, newLine, 0);
}

void pI(int i, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x24, i, newLine, 0);
}

void pC(char c, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x25, c, newLine, 0);
}