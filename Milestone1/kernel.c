#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_SECTORS 20
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 1
#define DIR_SECTOR 2
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF
#define LOGO_COLUMN 72
#define LOGO_ROW 8

void handleInterrupt21(int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length);
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);
void printLogo();
void printStringXY(char *string, int color, int x, int y);

int main()
{
	char buffer[SECTOR_SIZE * MAX_SECTORS];
	int suc;
	makeInterrupt21();
	printLogo();
	//Cari key.txt kalo udah ada
	//Key.txt akan digenerate setelah manggil keyproc
	//Jadi awalnya panggil keyproc dulu karena key.txt ga ada
	//Lalu run lagi programnya, karena sudah ada key.txt, langsung keprint key nya
	interrupt(0x21, 0x4, buffer, "key.txt", &suc);
	if (suc)
	{
		interrupt(0x21,0x0, "Key : ", 0, 0);
	 	interrupt(0x21,0x0, buffer, 0, 0);
	}
	else
	{
		interrupt(0x21, 0x6, "keyproc", 0x2000, &suc);
	}
	while (1)
	{

	}
}

void printStringXY(char *string, int color, int x, int y)
{
	if (x >= 0 && x <= 80 && y >= 0 && y <= 25)
	{
		int i = 0;
		while (string[i] != '\0' && string[i] != '\n')
		{
			int offset = 0x8000 + ((80 * y) + x) * 2;
			putInMemory(0xB000, offset, string[i]);
			putInMemory(0xB000, offset + 1, color);
			i++;
			x++;
		}
	}
}

void printLogo()
{
	printStringXY("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *", 0xD, 0, 0);
	printStringXY("* ______  _                     _   _                 _____        _  *", 0xD, 0, 1);
	printStringXY("* | ___ \\(_)                   | \\ | |               |  _  |      (_) *", 0xD, 0, 2);
	printStringXY("* | |_/ / _  _ __ ___    ___   |  \\| |  ___   _ __   | | | | _ __  _  *", 0xD, 0, 3);
	printStringXY("* | ___ \\| || '_ ` _ \\  / _ \\  | . ` | / _ \\ | '_ \\  | | | || '__|| | *", 0xD, 0, 4);
	printStringXY("* | |_/ /| || | | | | || (_) | | |\\  || (_) || | | | \\ \\_/ /| |   | | *", 0xD, 0, 5);
	printStringXY("* \\____/ |_||_| |_| |_| \\___/  \\_| \\_/ \\___/ |_| |_|  \\___/ |_|   |_| *", 0xD, 0, 6);
	printStringXY("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *", 0xD, 0, 7);
}

void handleInterrupt21(int AX, int BX, int CX, int DX)
{
	switch (AX)
	{
	case 0x0:
		printString(BX);
		break;
	case 0x1:
		readString(BX);
		break;
	case 0x2:
		readSector(BX, CX);
		break;
	case 0x3:
		writeSector(BX, CX);
		break;
	case 0x4:
		readFile(BX, CX, DX);
		break;
	case 0x5:
		writeFile(BX, CX, DX);
		break;
	case 0x6:
		executeProgram(BX, CX, DX);
		break;
	default:
		printString("Invalid interrupt");
	}
}

void printString(char *string)
{
	int i = 0;
	while (string[i] != '\0')
	{
		interrupt(0x10, 0xE00 + string[i], 0, 0, 0);
		i = i + 1;
	}
	interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
	interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
}

void readString(char *string)
{
	int i = 0;
	char c = 0;
	while (c != '\r')
	{
		c = interrupt(0x16, 0, 0, 0, 0);
		if (c == '\r')
		{
			string[i] = '\0';
		}
		else
		{
			if (c == '\b')
			{
				interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
				interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
				interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
				if (i > 0)
				{
					string[i] = '\0';
					i = i - 1;
				}
			}
			else
			{
				string[i] = c;
				i = i + 1;
				interrupt(0x10, 0xE00 + c, 0, 0, 0);
			}
		}
	}
	interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
	interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
}

int mod(int a, int b)
{
	while (a >= b)
	{
		a = a - b;
	}
	return a;
}

int div(int a, int b)
{
	int q = 0;
	while (q * b <= a)
	{
		q = q + 1;
	}
	return q - 1;
}

void readSector(char *buffer, int sector)
{
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}
void writeSector(char *buffer, int sector)
{
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void readFile(char *buffer, char *filename, int *success)
{
	char dir[SECTOR_SIZE];
	int iterDir = 0;
	int iterFileName;
	char ketemu = FALSE;
	char sama;
	int iterLastByte, i;
	//Isi dir dengan list of semua filename
	readSector(dir, DIR_SECTOR);
	//Traversal dir
	for (iterDir = 0; iterDir < SECTOR_SIZE; iterDir += DIR_ENTRY_LENGTH)
	{
		sama = TRUE;
		for (iterFileName = 0; iterFileName < MAX_FILENAME; iterFileName++)
		{
			if (filename[iterFileName] == '\0')
			{
				break;
			}
			else
			{
				if (filename[iterFileName] != dir[iterDir + iterFileName])
				{
					sama = FALSE;
					break;
				}
			}
		}
		if (sama)
		{
			ketemu = TRUE;
			break;
		}
	}
	//Cek apakah sudah ketemu
	if (!ketemu)
	{
		*success = FALSE;
		return;
	}
	else
	{
		//Traversal 20 byte terakhir dari dir[iterDir] - dir[iterDir+32]
		iterLastByte = iterDir + MAX_FILENAME;
		for (i = 0; i < MAX_SECTORS; i++)
		{
			if (dir[iterLastByte + i] == 0)
			{
				break;
			}
			else
			{
				readSector(buffer + i * SECTOR_SIZE, dir[iterLastByte + i]);
			}
		}
		*success = TRUE;
		return;
	}
}

void clear(char *buffer, int length)
{
	int i;
	for (i = 0; i < length; ++i)
	{
		buffer[i] = EMPTY;
	}
}

void writeFile(char *buffer, char *filename, int *sectors)
{
	char map[SECTOR_SIZE];
	char dir[SECTOR_SIZE];
	char sectorBuffer[SECTOR_SIZE];
	int dirIndex;

	readSector(map, MAP_SECTOR);
	readSector(dir, DIR_SECTOR);

	for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex)
	{
		if (dir[dirIndex * DIR_ENTRY_LENGTH] == '\0')
		{
			break;
		}
	}

	if (dirIndex < MAX_FILES)
	{
		int i, j, sectorCount;
		for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i)
		{
			if (map[i] == EMPTY)
			{
				++sectorCount;
			}
		}

		if (sectorCount < *sectors)
		{
			*sectors = INSUFFICIENT_SECTORS;
			return;
		}
		else
		{
			clear(dir + dirIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
			for (i = 0; i < MAX_FILENAME; ++i)
			{
				if (filename[i] != '\0')
				{
					dir[dirIndex * DIR_ENTRY_LENGTH + i] = filename[i];
				}
				else
				{
					break;
				}
			}

			for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i)
			{
				if (map[i] == EMPTY)
				{
					map[i] = USED;
					dir[dirIndex * DIR_ENTRY_LENGTH + MAX_FILENAME + sectorCount] = i;
					clear(sectorBuffer, SECTOR_SIZE);
					for (j = 0; j < SECTOR_SIZE; ++j)
					{
						sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j];
					}
					writeSector(sectorBuffer, i);
					++sectorCount;
				}
			}
		}
	}
	else
	{
		*sectors = INSUFFICIENT_DIR_ENTRIES;
		return;
	}

	writeSector(map, MAP_SECTOR);
	writeSector(dir, DIR_SECTOR);
}

void executeProgram(char *filename, int segment, int *success)
{
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	int i;
	readFile(buffer, filename, success);
	if (*success)
	{
		for (i = 0; i < MAX_SECTORS * SECTOR_SIZE; i++)
		{
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
}
