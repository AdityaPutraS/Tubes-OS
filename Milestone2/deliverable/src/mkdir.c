#define TRUE 1
#define FALSE 0

void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);

int main() {
    int berhasil;
    char curdir;
    char argc;
    char argv[1][128];
    char temp;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    interrupt(0x21, 0x23, 0, argv[0], 0);   // nama folder / path relatif folder
    // pS("Mencoba buat : ", FALSE);
    // pS(argv[0], TRUE);
    interrupt(0x21, curdir << 8 | 0x08, argv[0], &berhasil, 0);
    // interrupt(0x21, 0x0, "berhasil : ", 0, 0);
    // interrupt(0x21, 0x0, berhasil+'A', 1, 0);
    if(berhasil == -3)
    {
        interrupt(0x21, 0x0, "INSUFFICIENT_ENTRIES", 1, 0);
    }else if(berhasil == -2)
    {
        interrupt(0x21, 0x0, "FOLDER ALREADY_EXISTS", 1, 0);
    }else if(berhasil == -1)
    {
        interrupt(0x21, 0x0, "PATH NOT_FOUND", 1, 0);
    }else if(berhasil == 0)
    {
        interrupt(0x21, 0x0, "SUCCESS", 1, 0);
    }
    //Keluar dari sini
    interrupt(0x21, 0x20, curdir, 0, 0);
	interrupt(0x21, 0x7, &temp, 0, 0);
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