#define TRUE 1
#define FALSE 0
#define EMPTY 0x00

#define SECTOR_SIZE 512
#define MAX_SECTORS 16	

void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
void clear(char *buffer, int length);

int main()
{
    char curdir;
    char argc;
    char argv[2][128];
    char isiFile[MAX_SECTORS * SECTOR_SIZE];
    char buffer[SECTOR_SIZE];
    char temp;
    int berhasil;
    interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    interrupt(0x21, 0x23, 0, argv[0], 0);   // nama folder / path relatif folder
    if(argc == 2)
    {
        interrupt(0x21, 0x23, 1, argv[1], 0); //parameter -w
        //Cek apakah -w
        if(argv[1][0] == '-' && argv[1][1] == 'w' && argv[1][2] == '\0')
        {
            //Write mode
            clear(isiFile, MAX_SECTORS * SECTOR_SIZE);
            interrupt(0x21, 0x0, "Isi : ", 1, 0);
            interrupt(0x21, 0x1, isiFile, 0, 0);
            //Tulis input ke file
            interrupt(0x21, curdir << 8 | 0x05, isiFile, argv[0], &berhasil);
            if(berhasil == 1){
                interrupt(0x21, 0x0, "FILE_WRITTEN", 1, 0);
            }else if(berhasil == 0)
            {
                interrupt(0x21, 0x0, "INSUFFICIENT_SECTORS", 1, 0);
            }else if(berhasil == -1)
            {
                interrupt(0x21, 0x0, "NOT_FOUND", 1, 0);
            }else if(berhasil == -2)
            {
                interrupt(0x21, 0x0, "ALREADY_EXISTS", 1, 0);
            }else if(berhasil == -3)
            {
                interrupt(0x21, 0x0, "INSUFFICIENT_ENTRIES", 1, 0);
            }
        }else{
            interrupt(0x21, 0x0, "INVALID FLAG", 1, 0);
        }
    }else{
        //Read mode
        clear(buffer, SECTOR_SIZE);
        interrupt(0x21, curdir << 8 | 0x04, buffer, argv[0], &berhasil);
        if(berhasil == 0)
        {
            interrupt(0x21, 0x0, buffer, 1, 0);
        }else if(berhasil == -1){
            interrupt(0x21, 0x0, "FILE NOT FOUND", 1, 0);
        }else{
            interrupt(0x21, 0x0, "ERROR UNKNOWN", 1, 0);
        }
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

void clear(char *buffer, int length)
{
	int i;
	for (i = 0; i < length; ++i)
	{
		buffer[i] = EMPTY;
	}
}