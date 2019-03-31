#define TRUE 1
#define FALSE 0
#define MAX_DIRS 32
#define MAX_NAME 15
#define DIRS_SECTOR 0x101
#define FILES_SECTOR 0x102
#define SECTOR_SIZE 512
#define EMPTY 0x00

void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
void clear(char *buffer, int length);

int main()
{
    int i, j;
    char curdir;
    char dirs[SECTOR_SIZE], file[SECTOR_SIZE];
    char fileName[15];
    char temp;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
    interrupt(0x21, 0x02, file, FILES_SECTOR, 0);
    pS("Isi : ", TRUE);
    clear(fileName, 15);
    pS("   ", FALSE);
    pS("Folder : ", TRUE);
    for (i = 0; i < SECTOR_SIZE; i+= 16) {
        if (dirs[i] == curdir && dirs[i+1] != '\0') {
            clear(fileName, 15);
            for (j = 0; j < 15; j++) {
                fileName[j] = dirs[i + j + 1];
            }
            pS("      ", FALSE);
            pS(fileName, TRUE);
        }
    }
    pS("   ", FALSE);
    pS("File : ", TRUE);
    for (i = 0; i < SECTOR_SIZE; i+= 16) {
        if (file[i] == curdir && file[i+1] != '\0') {
            clear(fileName, 15);
            for (j = 0; j < 15; j++) {
                fileName[j] = file[i + j + 1];
            }
            pS("      ", FALSE);
            pS(fileName, TRUE);
        }
    }
    //keluar dari sini
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

void clear(char *buffer, int length)
{
    int i;
    for (i = 0; i < length; ++i)
    {
        buffer[i] = EMPTY;
    }
}