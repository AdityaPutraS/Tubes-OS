#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_SECTORS 16		 //Ditambah agar sesuai spek
#define MAX_DIRS 32			 //Ditambah agar sesuai spek
#define MAX_DIRSNAME 15		 //Ditambah agar sesuai spek
#define MAP_SECTOR 0x100	 //Diubah agar sesuai spek
#define DIRS_SECTOR 0x101	//Diubah agar sesuai spek
#define FILES_SECTOR 0x102   //Ditambah agar sesuai spek
#define SECTORS_SECTOR 0x103 //Ditambah agar sesuai spek
#define MAX_FILENAME 15		 //Diubah agar sesuai spek
#define MAX_FILES 32		 //Diubah agar sesuai spek
#define ROOT 0xFF			 //Ditambah agar sesuai spek
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define INSUFFICIENT_ENTRIES -3 //Ditambah agar sesuai spek
#define ALREADY_EXISTS -2		//Ditambah agar sesuai spek
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF
#define LOGO_COLUMN 72
#define LOGO_ROW 8
#define ARGS_SECTOR 512 //Dari spek

void handleInterrupt21(int AX, int BX, int CX, int DX);
void printString(char *string, int newLine);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void executeProgram(char *path, int segment, int *result, char parentIndex);
void printLogo();
void printStringXY(char *string, int color, int x, int y);
void printInt(int i);
void split(char *string, char separator, char **splitted, char *success);
void len(char *string, int *length);
void isSame(char *s1, char *s2, char *result);
void copy(char *string, int start, int length, char *copied, char *success);
void search(char *sector, char awal, char *sisanya, char *index, char *success);
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex);
void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex);
void splitDirFilePath(char *path, char *dirPath, char *fileName, char *succ);
int max(int a, int b);
void terminateProgram(int *result);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex);
void putArgs(char curdir, char argc, char **argv);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char index, char *argv);
void readAllSector(char *dirs, char *file, char *map, char *sector);
void writeAllSector(char *dirs, char *file, char *map, char *sector);

int main()
{
	char buffer[SECTOR_SIZE * MAX_SECTORS];
	int suc;
	char input[100];
	makeInterrupt21();
	printLogo();
	//interrupt(0x21, (0 << 8) | 0, "$ ", 0, 0);
	interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, &suc);
	// interrupt(0x21, (0 << 8) | 0, "$ LOLLLZ", 0, 0);
	// interrupt(0x21, (0 << 8) | 1, &input, 0, 0);
	// interrupt(0x21, (0 << 8) | 0, " > ", 0, 0);
	// interrupt(0x21, (0 << 8) | 0, input, 0, 0);
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

void handleInterrupt21(int AX, int BX, int CX, int DX)
{
	char AL, AH;
	AL = (char)(AX);
	AH = (char)(AX >> 8);

	switch (AL)
	{
	case 0x00:
		printString(BX, CX);
		break;
	case 0x01:
		readString(BX);
		break;
	case 0x02:
		readSector(BX, CX);
		break;
	case 0x03:
		writeSector(BX, CX);
		break;
	case 0x04:
		readFile(BX, CX, DX, AH);
		break;
	case 0x05:
		writeFile(BX, CX, DX, AH);
		break;
	case 0x06:
		executeProgram(BX, CX, DX, AH);
		break;
	case 0x07:
		terminateProgram(BX);
		break;
	case 0x08:
		makeDirectory(BX, CX, AH);
		break;
	case 0x09:
		deleteFile(BX, CX, AH);
		break;
	case 0x0A:
		deleteDirectory(BX, CX, AH);
		break;
	case 0x20:
		putArgs(BX, CX, DX);
		break;
	case 0x21:
		getCurdir(BX);
		break;
	case 0x22:
		getArgc(BX);
		break;
	case 0x23:
		getArgv(BX, CX);
		break;
	default:
		printString("Invalid interrupt", TRUE);
	}
}

void printString(char *string, int newLine)
{
	int i = 0;
	while (string[i] != '\0')
	{
		interrupt(0x10, 0xE00 + string[i], 0, 0, 0);
		i = i + 1;
	}
	if(newLine){
	interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
	interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	}
}

//////FOR DEBUGGING PURPOSE
// void printInt(int i)
// {
// 	int j;
// 	char digit[5]; //4 digit max
// 	digit[0] = '0';
// 	digit[1] = '0';
// 	digit[2] = '0';
// 	digit[3] = '0';
// 	digit[4] = '\0';
// 	j = 3;
// 	while (i != 0 && j >= 0)
// 	{
// 		digit[j] = '0' + mod(i, 10);
// 		i = div(i, 10);
// 		j -= 1;
// 	}
// 	printString(digit);
// }
//////////////////////////

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

void split(char *string, char separator, char **splitted, char *success)
{
	int i, j, k;
	i = 0;
	j = 0;
	k = 0;
	*success = FALSE;
	while (string[i] != '\0')
	{
		if (string[i] == separator)
		{
			splitted[j][k] = '\0';
			j += 1;
			k = 0;
		}
		else
		{
			splitted[j][k] = string[i];
			k += 1;
		}
		i += 1;
	}
	*success = TRUE;
}

void len(char *string, int *length)
{
	*length = 0;
	while (string[*length] != '\0')
	{
		*length += 1;
	}
}

void count(char *string, char c, int *banyak)
{
	int i = 0;
	*banyak = 0;
	while (string[i] != '\0')
	{
		if (string[i] == c)
		{
			*banyak += 1;
		}
		i += 1;
	}
}

void isSame(char *s1, char *s2, char *result)
{
	int len1, len2, i;
	char tempResult;
	len(s1, &len1);
	len(s2, &len2);
	if (len1 != len2)
	{
		tempResult = FALSE;
	}
	else
	{
		i = 0;
		tempResult = TRUE;
		while (s1[i] != '\0' && tempResult)
		{
			if (s1[i] != s2[i])
			{
				tempResult = FALSE;
			}
			i += 1;
		}
	}
	*result = tempResult;
}

void copy(char *string, int start, int length, char *copied, char *success)
{
	int lenS, i;
	*success = FALSE;
	len(string, &lenS);
	copied[0] = '\0';
	//Validasi start
	if (start < lenS)
	{
		if (length > 0)
		{
			i = start;
			while (length > 0 && string[i] != '\0')
			{
				copied[i - start] = string[i];
				i += 1;
				length -= 1;
			}
			copied[i - start] = '\0';
			*success = TRUE;
		}
	}
}

void search(char *sector, char awal, char *sisanya, char *index, char *success)
{
	int i;
	char copied[15], temp, ketemu;
	*success = FALSE;
	ketemu = FALSE;
	i = 0;
	*index = 0;
	while (*index < 32)
	{
		if (sector[*index * 16] == awal)
		{
			copy(sector + (*index * 16), 1, 15, copied, &temp);
			isSame(copied, sisanya, success);
			if (*success)
			{
				break;
			}
		}
		*index = *index + 1;
	}
}

void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex)
{
	int i, pathLength, countSlash;
	char pathSplitted[MAX_DIRS][MAX_DIRSNAME];
	if (relPath[0] == '\0')
	{
		*success = TRUE;
		*index = parentIndex;
	}
	else
	{
		//Split path
		len(relPath, &pathLength);
		count(relPath, "/", &countSlash);
		split(relPath, "/", pathSplitted, success);
		*success = TRUE;
		for (i = 0; (i < (countSlash + 1)) && *success; i++)
		{
			search(dirs, parentIndex, pathSplitted[i], index, success);
			parentIndex = pathSplitted[i];
		}
	}
}

void splitDirFilePath(char *path, char *dirPath, char *fileName, char *succ)
{
	int i, lenCopy;
	len(path, &i);
	//abc/def/gg
	//0123456789
	//len = 10
	//shell
	//01234
	//len = 5
	i -= 1;
	lenCopy = 0;
	while (path[i] != '/' && i >= 0)
	{
		lenCopy += 1;
		i -= 1;
	}
	i += 1;
	copy(path, 0, i - 1, dirPath, succ);
	copy(path, i, lenCopy, fileName, succ);
}

void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex)
{
	char relPathWithoutFile[MAX_DIRS * (MAX_DIRSNAME + 1)];
	char fileName[MAX_FILENAME];
	char idxDir;
	splitDirFilePath(relPath, relPathWithoutFile, fileName, success);
	searchDir(dirs, relPathWithoutFile, &idxDir, success, parentIndex);
	if (*success)
	{
		search(file, idxDir, fileName, index, success);
	}
}

void readAllSector(char *dirs, char *file, char *map, char *sector)
{
	readSector(dirs, DIRS_SECTOR);
	readSector(file, FILES_SECTOR);
	readSector(map, MAP_SECTOR);
	readSector(sector, SECTORS_SECTOR);
}
void writeAllSector(char *dirs, char *file, char *map, char *sector)
{
	writeSector(dirs, DIRS_SECTOR);
	writeSector(file, FILES_SECTOR);
	writeSector(map, MAP_SECTOR);
	writeSector(sector, SECTORS_SECTOR);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
	//Baca sector dirs
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	int iterSector, idxFile;
	char succ;
	readAllSector(dirs, file, map, sector);
	//Cari index file yang sesuai dengan path
	searchFile(dirs, file, path, &idxFile, &succ, parentIndex);
	if (succ)
	{
		//Baca daftar sektor file pada entri file
		for (iterSector = 0; iterSector < 16; iterSector++)
		{
			if (sector[idxFile * 16 + iterSector] != 0x00)
			{
				readSector(buffer + iterSector * SECTOR_SIZE, sector[idxFile * 16 + iterSector]);
			}else{
				break;
			}
		}
		*result = 0; //Success
	}
	else
	{
		*result = NOT_FOUND;
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

int max(int a, int b)
{
	if (a >= b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex)
{
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	char dirPath[MAX_DIRS * (MAX_DIRSNAME + 1)];
	char fileName[MAX_FILENAME];
	char sectorBuffer[16];
	int i, cntMapKosong, lenBuffer, lenFileName;
	int iterMap;
	int idxFile;
	char succ;
	char parentFile;
	readAllSector(dirs, file, map, sector);
	//Cek jumlah sektor kosong di map
	cntMapKosong = 0;
	for (i = 0; i < SECTOR_SIZE; i++)
	{
		if (map[i] == 0x00)
		{
			cntMapKosong += 1;
		}
	}
	len(buffer, lenBuffer);
	if (cntMapKosong >= max(1, div(lenBuffer, SECTOR_SIZE)))
	{
		//Cek apakah ada entri kosong di files
		succ = FALSE;
		for (i = 0; (i < SECTOR_SIZE) && !succ; i += (MAX_FILENAME + 1))
		{
			if (file[i] == '\0')
			{
				succ = TRUE;
				idxFile = i;
			}
		}
		if (succ)
		{
			splitDirFilePath(path, dirPath, fileName, &succ);
			//Cek apakah dir ada / tidak
			searchDir(dirs, dirPath, &parentFile, &succ, parentIndex);
			if (succ)
			{
				//Cek apakah filenya sudah ada / tidak
				searchFile(dirs, file, path, &i, &succ, parentIndex);
				if (succ)
				{
					*sectors = ALREADY_EXISTS;
				}
				else
				{
					file[idxFile * (MAX_FILENAME + 1)] = parentFile;
					len(fileName, lenFileName);
					for (i = 0; i < lenFileName; i++)
					{
						file[idxFile * (MAX_FILENAME + 1) + i + 1] = fileName[i];
					}
					for (i = 0; i < max(1, div(lenBuffer, SECTOR_SIZE)); i++)
					{
						//Untuk semua sektor dari buffer
						//Cari sektor kosong pertama di map
						for (iterMap = 0; (iterMap < SECTOR_SIZE) && (map[iterMap] != 0x00); iterMap++)
						{
						}
						map[iterMap] = 0xFF;
						sector[idxFile * (MAX_FILENAME + 1) + i] = iterMap;
						copy(buffer, i * SECTOR_SIZE, SECTOR_SIZE, sectorBuffer, &succ);
						writeSector(sectorBuffer, iterMap);
					}
					writeAllSector(dirs, file, map, sector);
				}
			}
			else
			{
				*sectors = NOT_FOUND;
			}
		}
		else
		{
			*sectors = INSUFFICIENT_ENTRIES;
		}
	}
	else
	{
		*sectors = INSUFFICIENT_SECTORS;
	}
}

void executeProgram(char *path, int segment, int *result, char parentIndex)
{
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	int i;
	readFile(buffer, path, result, parentIndex);
	if (*result == 0)
	{
		for (i = 0; i < MAX_SECTORS * SECTOR_SIZE; i++)
		{
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
}

void terminateProgram(int *result)
{
	char shell[6];
	shell[0] = 's';
	shell[1] = 'h';
	shell[2] = 'e';
	shell[3] = 'l';
	shell[4] = 'l';
	shell[5] = '\0';
	executeProgram(shell, 0x2000, result, 0xFF);
}

void makeDirectory(char *path, int *result, char parentIndex)
{
	char dirs[SECTOR_SIZE];
	char dirPath[MAX_DIRS * (MAX_DIRSNAME + 1)];
	char folderName[MAX_DIRSNAME];
	int lenFolderName;
	int iterDirs, temp, idxParent;
	char succ;
	readSector(dirs, DIRS_SECTOR);
	for (iterDirs = 0; (iterDirs < SECTOR_SIZE) && (dirs[iterDirs] != '\0'); iterDirs++)
	{
	}
	if (iterDirs == SECTOR_SIZE)
	{
		*result = INSUFFICIENT_ENTRIES;
	}
	else
	{
		splitDirFilePath(path, dirPath, folderName, &succ);
		//cek apakah parent dari foldernya ada
		searchDir(dirs, dirPath, &idxParent, &succ, parentIndex);
		if (succ)
		{
			//cek apakah sudah ada foldernya
			searchDir(dirs, path, &temp, &succ, parentIndex);
			if (succ)
			{
				*result = ALREADY_EXISTS;
			}
			else
			{
				dirs[iterDirs] = idxParent;
				len(folderName, lenFolderName);
				for (temp = 0; temp < lenFolderName; temp++)
				{
					dirs[iterDirs * (MAX_DIRSNAME + 1) + temp + 1] = folderName[temp];
				}
				*result = 0; //Success
			}
		}
		else
		{
			*result = NOT_FOUND;
		}
	}
}

void deleteFile(char *path, int *result, char parentIndex)
{
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	int idxFile, iterSector;
	char succ;
	readAllSector(dirs, file, map, sector);
	searchFile(dirs, file, path, &idxFile, &succ, parentIndex);
	if (succ)
	{
		file[idxFile * MAX_FILENAME] = '\0';
		for (iterSector = 0; (iterSector < MAX_SECTORS) && (sector[idxFile * MAX_SECTORS + iterSector] != 0x00); iterSector++)
		{
			map[sector[idxFile * MAX_SECTORS + iterSector]] = 0x00;
			sector[idxFile * MAX_SECTORS + iterSector] = 0x00;
		}
		writeAllSector(dirs, file, map, sector);
		*result = 0; //Success
	}
	else
	{
		*result = NOT_FOUND;
	}
}

void printLogo()
{
	// printStringXY("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *", 0xD, 0, 0);
	// printStringXY("* ______  _                     _   _                 _____        _  *", 0xD, 0, 1);
	// printStringXY("* | ___ \\(_)                   | \\ | |               |  _  |      (_) *", 0xD, 0, 2);
	// printStringXY("* | |_/ / _  _ __ ___    ___   |  \\| |  ___   _ __   | | | | _ __  _  *", 0xD, 0, 3);
	// printStringXY("* | ___ \\| || '_ ` _ \\  / _ \\  | . ` | / _ \\ | '_ \\  | | | || '__|| | *", 0xD, 0, 4);
	// printStringXY("* | |_/ /| || | | | | || (_) | | |\\  || (_) || | | | \\ \\_/ /| |   | | *", 0xD, 0, 5);
	// printStringXY("* \\____/ |_||_| |_| |_| \\___/  \\_| \\_/ \\___/ |_| |_|  \\___/ |_|   |_| *", 0xD, 0, 6);
	// printStringXY("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *", 0xD, 0, 7);
}

void deleteDirectory(char *path, int *success, char parentIndex)
{
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	int idxFolder, iterFile, iterSector;
	char succ;
	readAllSector(dirs, file, map, sector);
	searchDir(dirs, path, &idxFolder, &succ, parentIndex);
	if (succ)
	{
		dirs[idxFolder * MAX_DIRSNAME] = '\0';
		//Hapus semua file yang ada di folder ini
		for (iterFile = 0; iterFile < SECTOR_SIZE; iterFile += (MAX_FILENAME + 1))
		{
			if (file[iterFile] == idxFolder)
			{
				file[iterFile * MAX_FILENAME] = '\0';
				for (iterSector = 0; (iterSector < MAX_SECTORS) && (sector[iterFile * MAX_SECTORS + iterSector] != 0x00); iterSector++)
				{
					map[sector[iterFile * MAX_SECTORS + iterSector]] = 0x00;
					sector[iterFile * MAX_SECTORS + iterSector] = 0x00;
				}
			}
		}
		writeAllSector(dirs, file, map, sector);
		*success = 0; //Sucess
	}
	else
	{
		*success = NOT_FOUND;
	}
}

void putArgs(char curdir, char argc, char **argv)
{
	char args[SECTOR_SIZE];
	int i, j, p;
	clear(args, SECTOR_SIZE);

	args[0] = curdir;
	args[1] = argc;
	i = 0;
	j = 0;
	for (p = 1; p < ARGS_SECTOR && i < argc; ++p)
	{
		args[p] = argv[i][j];
		if (argv[i][j] == '\0')
		{
			++i;
			j = 0;
		}
		else
		{
			++j;
		}
	}

	writeSector(args, ARGS_SECTOR);
}

void getCurdir(char *curdir)
{
	char args[SECTOR_SIZE];
	readSector(args, ARGS_SECTOR);
	*curdir = args[0];
}

void getArgc(char *argc)
{
	char args[SECTOR_SIZE];
	readSector(args, ARGS_SECTOR);
	*argc = args[1];
}

void getArgv(char index, char *argv)
{
	char args[SECTOR_SIZE];
	int i, j, p;
	readSector(args, ARGS_SECTOR);

	i = 0;
	j = 0;
	for (p = 1; p < ARGS_SECTOR; ++p)
	{
		if (i == index)
		{
			argv[j] = args[p];
			++j;
		}

		if (args[p] == '\0')
		{
			if (i == index)
			{
				break;
			}
			else
			{
				++i;
			}
		}
	}
}
