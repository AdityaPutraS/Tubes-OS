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
// void printLogo();
void printStringXY(char *string, int color, int x, int y);
void printInt(int i, int newLine);
void printChar(char c, int newLine);
void split(char *string, char separator, char splitted[32][15]);
void len(char *string, int *length);
void isSame(char *s1, char *s2, char *result);
void copy(char *string, char *copied, int start, int length);
void search(char *sector, char awal, char sisanya[15], char *index, char *success);
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex);
void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex);
void splitDirFilePath(char *path, char *dirPath, char *fileName);
void terminateProgram(int *result);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex);
void putArgs(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char index, char *argv);
void readAllSector(char *dirs, char *file, char *map, char *sector);
void writeAllSector(char *dirs, char *file, char *map, char *sector);
void replace(char *s, char cari, char tukar);

int main() {
	char buffer[SECTOR_SIZE];
	char suc;
	int i;
	char pathSplitted[MAX_DIRS][MAX_DIRSNAME];
	char temp[30], temp2[30];
	makeInterrupt21();
	// printLogo();
	putArgs(0xFF, 0, 0);
	interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, &suc);
	while (1) {
	}
}

/**
 * Mengganti setiap karakter cari pada string s dengan tukar
 */
void replace(char *s, char cari, char tukar) {
	int length, i;
	len(s, &length);
	for (i = 0; i < length; i++) {
		if (s[i] == cari) {
			s[i] = tukar;
		}
	}
	s[length] = '\0';
}

/**
 * Menampilkan kalimat string dengan warna color pada lokasi (x, y)
 */
void printStringXY(char *string, int color, int x, int y) {
	if (x >= 0 && x <= 80 && y >= 0 && y <= 25) {
		int i = 0;
		while (string[i] != '\0' && string[i] != '\n') {
			int offset = 0x8000 + ((80 * y) + x) * 2;
			putInMemory(0xB000, offset, string[i]);
			putInMemory(0xB000, offset + 1, color);
			i++;
			x++;
		}
	}
}

/**
 * Implementasi interupt nomor 0x21 pada kernel
 * Fungsi menerima 5 argumen AL, AH, BX, CX, dan DX
 * dengan AX = (AH << 8) | AL
 * interupt 0x21 dapat dipanggil dengan code:
 * interrupt(0x21, (AH << 8) | AL, BX, CX, DX);
 */
void handleInterrupt21(int AX, int BX, int CX, int DX) {
	char AL, AH;
	AL = (char)(AX);
	AH = (char)(AX >> 8);

	switch (AL) {
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
		case 0x24:
			printInt(BX, CX);
			break;
		case 0x25:
			printChar(BX, CX);
			break;
		default:
			printString("Invalid interrupt", TRUE);
	}
}

/**
 * Mencetak string ke layar, jika newLine == TRUE maka akan mencetak \n
 */
void printString(char *string, int newLine) {
	int i = 0;
	while (string[i] != '\0') {
		interrupt(0x10, 0xE00 + string[i], 0, 0, 0);
		i = i + 1;
	}
	if (newLine) {
		interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
		interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	}
}

//////FOR DEBUGGING PURPOSEs
/**
 * Mencetak char ke layar, jika newLine == TRUE maka akan mencetak \n
 */
void printChar(char c, int newLine) {
	interrupt(0x10, 0xE00 + c, 0, 0, 0);
	if (newLine) {
		interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
		interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	}
}

/**
 * Mencetak integer 9 digit ke layar, jika newLine == TRUE maka akan mencetak \n
 */
void printInt(int i, int newLine) {
	int j;
	char digit[10]; //9 digit max
	digit[0] = '0';
	digit[1] = '0';
	digit[2] = '0';
	digit[3] = '0';
	digit[4] = '0';
	digit[5] = '0';
	digit[6] = '0';
	digit[7] = '0';
	digit[8] = '0';
	digit[9] = '\0';
	j = 8;
	while (i != 0 && j >= 0) {
		digit[j] = '0' + mod(i, 10);
		i = div(i, 10);
		j -= 1;
	}
	printString(digit, newLine);
}
//////////////////////////

/**
 * Mencetak string dari user, akan berhenti membaca ketika enter ditekan
 */
void readString(char *string) {
	int i = 0;
	char c = 0;
	while (c != '\r') {
		c = interrupt(0x16, 0, 0, 0, 0);
		if (c == '\r') {
			string[i] = '\0';
		} else {
			if (c == '\b') {
				if (i > 0) {
					interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
					interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
					interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
					string[i] = '\0';
					i = i - 1;
				}
			} else {
				string[i] = c;
				i = i + 1;
				interrupt(0x10, 0xE00 + c, 0, 0, 0);
			}
		}
	}
	interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
	interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
}

/**
 * Mencari sisa bagi dari 2 bilangan
 */
int mod(int a, int b) {
	while (a >= b) {
		a = a - b;
	}
	return a;
}

/**
 * Mencari hasil bagi 2 bilangan, rounded down
 */
int div(int a, int b) {
	int q = 0;
	while (q * b <= a) {
		q = q + 1;
	}
	return q - 1;
}

/**
 * Membaca sektor ke (int) sector lalu memasukkan isinya kedalam (char*) buffer,
 * implementasi memanfaatkan interrupt nomor 0x13
 */
void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, 
	div(sector, 36) * 0x100 + mod(sector, 18) + 1, 
	mod(div(sector, 18), 2) * 0x100);
}

/**
 * Menuliskan isi dari (char*) buffer kedalam sektor nomor (int) sector,
 * implementasi memanfaatkan interrupt nomor 0x13
 */
void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, 
	div(sector, 36) * 0x100 + mod(sector, 18) + 1, 
	mod(div(sector, 18), 2) * 0x100);
}

/**
 * Memisahkan suatu string menjadi sekumpulan string yang pada awalnya
 * dipisahkan oleh karakter separator
 */
void split(char *string, char separator, char splitted[32][15]) {
	int i, j, k;
	i = 0;
	j = 0;
	k = 0;
	clear(splitted[0], 15);
	while (string[i] != '\0') {
		if (string[i] == separator) {
			splitted[j][k] = '\0';
			j += 1;
			k = 0;
			clear(splitted[j], 15);
		} else {
			splitted[j][k] = string[i];
			k += 1;
		}
		i += 1;
	}
	splitted[j][k] = '\0';
}

/** Menghitung panjang dari sebuah string */
void len(char *string, int *length) {
	*length = 0;
	while (string[*length] != '\0') {
		*length += 1;
	}
}

/** Menghitung banyaknya kemunculan karakter c pada sebuah string */
void count(char *string, char c, int *banyak) {
	int i = 0;
	*banyak = 0;
	while (string[i] != '\0') {
		if (string[i] == c) {
			*banyak += 1;
		}
		i += 1;
	}
}

/** Menentukan apakah dua buah string sama atau tidak */
void isSame(char *s1, char *s2, char *result) {
	int i;
	char tempResult;
	i = 0;
	tempResult = TRUE;
	while (s1[i] != '\0' && s2[i] != '\0' && tempResult) {
		if (s1[i] != s2[i]) {
			tempResult = FALSE;
		}
		i += 1;
	}
	if (s1[i] != '\0' || s2[i] != '\0') {
		tempResult = FALSE;
	}
	*result = tempResult;
}

/** Menyalin isi dari sebuah (string) ke (string) copied */
void copy(char *string, char *copied, int start, int length) {
	int lenS, lenC, i;
	len(string, &lenS);
	len(copied, &lenC);
	clear(copied, lenC);
	if (lenS < length) {
		clear(copied, lenS);
	} else {
		clear(copied, length);
	}
	//Validasi start
	if (start < lenS) {
		if (length > 0) {
			i = start;
			while (length > 0 && string[i] != '\0') {
				copied[i - start] = string[i];
				i += 1;
				length -= 1;
			}
		}
	}
}

/** 
 * Mencari keberadaan sebuah(string) pada suatu sector
 * Biasanya digunakan untuk searching pada sector, dirs (0x101) dan files (0x102)
 */
void search(char *sector, char awal, char sisanya[15], char *index, char *success) {
	int i, j;
	char copied[15];
	clear(copied, 15);
	*success = FALSE;
	*index = 0;
	// printString("Masuk search , cari : ", FALSE);
	// printString(sisanya, TRUE);
	for (i = 0; (i < 32) && !(*success); i++) {
		if (sector[i * 16] == awal) {
			//Kopi sebagian dari sektor
			clear(copied, 15);
			for (j = 0; j < 15; j++) {
				copied[j] = sector[i * 16 + j + 1];
			}
			// printInt(i, TRUE);
			// printString("V", TRUE);
			// printString(copied, TRUE);
			isSame(copied, sisanya, success);
		}
	}
	// printString("SELESAI", TRUE);
	*index = i - 1;
}

/**
 * Mencari indeks direktori xyz dengan kemungkinan path relatif aaa/bbb/xyz yang terdapat pada
 * sektor dirs dengan index direktory parent dari aaa adalah parentIndex
 */
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex) {
	int i, pathLength, countSlash;
	char pathSplitted[MAX_DIRS][MAX_DIRSNAME];
	char cari[MAX_DIRSNAME];
	if (relPath[0] == '\0') {
		*success = TRUE;
		*index = parentIndex;
	} else {
		//Split path
		len(relPath, &pathLength);
		count(relPath, '/', &countSlash);
		split(relPath, '/', pathSplitted);
		// printString("isi 0 : ", FALSE);
		// printString(pathSplitted[0], TRUE);
		// printString("isi 1 : ", FALSE);
		// printString(pathSplitted[1], TRUE);
		// printString("count slash : ", FALSE);
		// printInt(countSlash, TRUE);
		*success = TRUE;
		for (i = 0; (i < (countSlash + 1)) && *success; i++) {
			// printString("pathSplitted skrng : ", FALSE);
			// printString(pathSplitted[i], TRUE);
			clear(cari, 15);
			copy(pathSplitted[i], cari, 0, 15);
			search(dirs, parentIndex, cari, index, success);
			// if (!success)
			// {
				// printString("Direktori tidak ditemukan", TRUE);
			// }
			parentIndex = *index;
		}
	}
}

/** Memisahkan nama file dengan pathnya */
void splitDirFilePath(char *path, char *dirPath, char *fileName) {
	int i, lenCopy;
	len(path, &i);
	//abc/def/gg
	//0123456789
	//len = 10
	//dirPath = abc/def
	//fileName = gg
	i -= 1;
	lenCopy = 0;
	while (path[i] != '/' && i >= 0) {
		lenCopy += 1;
		i -= 1;
	}
	i += 1;
	copy(path, dirPath, 0, i - 1);
	copy(path, fileName, i, lenCopy);
}

/**
 * Mencari indeks file xyz dengan kemungkinan path relatif aaa/bbb/xyz yang terdapat pada
 * sektor dirs dengan index direktory parent dari aaa adalah parentIndex
 */
void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex) {
	char relPathWithoutFile[MAX_DIRS * (MAX_DIRSNAME + 1)];
	char fileName[MAX_FILENAME];
	char idxDir;
	splitDirFilePath(relPath, relPathWithoutFile, fileName);
	// printString("rPWF : ", FALSE);
	// printString(relPathWithoutFile, TRUE);
	// printString("fN : ", FALSE);
	// printString(fileName, TRUE);
	searchDir(dirs, relPathWithoutFile, &idxDir, success, parentIndex);
	if (*success)
	{
		search(file, idxDir, fileName, index, success);
	}
}

/** Membaca sektor dirs, file, map, dan sector lalu memasukkannya ke buffer yang sesuai */
void readAllSector(char *dirs, char *file, char *map, char *sector)  {
	readSector(dirs, DIRS_SECTOR);
	readSector(file, FILES_SECTOR);
	readSector(map, MAP_SECTOR);
	readSector(sector, SECTORS_SECTOR);
}

/** Menuliskan isi dari semua buffer yang terdefinisi kedalam sector-sector yang sesuai */
void writeAllSector(char *dirs, char *file, char *map, char *sector) {
	writeSector(dirs, DIRS_SECTOR);
	writeSector(file, FILES_SECTOR);
	writeSector(map, MAP_SECTOR);
	writeSector(sector, SECTORS_SECTOR);
}

/** Membaca file dengan path tertentu lalu memasukka isinya kealam buffer */
void readFile(char *buffer, char *path, int *result, char parentIndex) {
	//Baca sector dirs
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], sector[SECTOR_SIZE];
	int iterSector, idxFile;
	char succ;
	interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
	interrupt(0x21, 0x02, file, FILES_SECTOR, 0);
	interrupt(0x21, 0x02, sector, SECTORS_SECTOR, 0);
	//Cari index file yang sesuai dengan path
	searchFile(dirs, file, path, &idxFile, &succ, parentIndex);
	if (succ) {
		//Baca daftar sektor file pada entri file
		for (iterSector = 0; iterSector < 16; iterSector++) {
			if (sector[idxFile * 16 + iterSector] != 0x00) {
				//interrupt(0x21, 0x02, buffer + iterSector * SECTOR_SIZE, sector[idxFile * 16 + iterSector], 0);
				readSector(buffer + iterSector * SECTOR_SIZE, sector[idxFile * 16 + iterSector]);
			} else {
				break;
			}
		}
		*result = 0; //Success
	} else {
		*result = NOT_FOUND;
	}
}

/** Menghapus isi buffer sepanjang length */
void clear(char *buffer, int length) {
	int i;
	for (i = 0; i < length; ++i) {
		buffer[i] = EMPTY;
	}
}

/** Mengisi file dengan path tertentu dengan isi dalam buffer */
void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	char dirPath[MAX_DIRS * (MAX_DIRSNAME + 1)];
	char fileName[MAX_FILENAME];
	char sectorBuffer[16];
	int i, cntMapKosong, lenBuffer, lenFileName;
	int iterMap;
	int idxFile;
	char succ;
	char parentFile;
	int divBuffSec;
	readAllSector(dirs, file, map, sector);

	//Cek jumlah sektor kosong di map
	cntMapKosong = 0;
	for (i = 0; i < SECTOR_SIZE; i++) {
		if (map[i] == 0x00) {
			cntMapKosong += 1;
		}
	}
	len(buffer, lenBuffer);
	divBuffSec = div(lenBuffer, SECTOR_SIZE);
	if (divBuffSec <= 0) {
		divBuffSec = 1;
	} if (cntMapKosong >= divBuffSec) {
		//Cek apakah ada entri kosong di files
		succ = FALSE;
		for (i = 0; (i < SECTOR_SIZE) && !succ; i += (MAX_FILENAME + 1)) {
			if (file[i + 1] == '\0') {
				succ = TRUE;
				idxFile = div(i, 16);
			}
		} if (succ) {
			splitDirFilePath(path, dirPath, fileName);
			// printString("dirPath : ", FALSE);
			// printString(dirPath, TRUE);
			// printString("fileName : ", FALSE);
			// printString(fileName, TRUE);
			//Cek apakah dir ada / tidak
			searchDir(dirs, dirPath, &parentFile, &succ, parentIndex);
			if (succ) {
				//Cek apakah filenya sudah ada / tidak
				searchFile(dirs, file, path, &i, &succ, parentIndex);
				if (succ) {
					*sectors = ALREADY_EXISTS;
				} else {
					//printString("Tulis parent file", TRUE);
					file[idxFile * (MAX_FILENAME + 1)] = parentFile;
					len(fileName, &lenFileName);
					for (i = 0; i < lenFileName; i++)
					{
						//printChar(fileName[i], FALSE);
						file[idxFile * (MAX_FILENAME + 1) + i + 1] = fileName[i];
					}
					for (i = 0; i < divBuffSec; i++)
					{
						//Untuk semua sektor dari buffer
						//Cari sektor kosong pertama di map
						for (iterMap = 0; (iterMap < SECTOR_SIZE) && (map[iterMap] != 0x00); iterMap++)
						{
						}
						map[iterMap] = 0xFF;
						sector[idxFile * (MAX_FILENAME + 1) + i] = iterMap;
						//clear(buffer+lenBuffer, 512-lenBuffer);
						copy(buffer, sectorBuffer, i * SECTOR_SIZE, SECTOR_SIZE);
						//interrupt(0x21, 0x03, sectorBuffer, iterMap, 0);
						writeSector(sectorBuffer, iterMap);
					}
					interrupt(0x21, 0x03, dirs, DIRS_SECTOR, 0);
					interrupt(0x21, 0x03, file, FILES_SECTOR, 0);
					interrupt(0x21, 0x03, map, MAP_SECTOR, 0);
					interrupt(0x21, 0x03, sector, SECTORS_SECTOR, 0);
					*sectors = 1;
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

/** Invoke program yang terdapat dalam path tertentu yang dijalankan pada segment tertentu */
void executeProgram(char *path, int segment, int *result, char parentIndex) {
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

/** Mematikan program yang sedang berjalan dan menjalankan shell */
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

/** Membuat directory sesuai path tertentu */
void makeDirectory(char *path, int *result, char parentIndex)
{
	char dirs[SECTOR_SIZE];
	char dirPath[MAX_DIRS * (MAX_DIRSNAME + 1)];
	char folderName[MAX_DIRSNAME];
	int lenFolderName;
	int iterDirs, temp, idxParent;
	char succ;
	interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
	// readSector(dirs, DIRS_SECTOR);
	for (iterDirs = 0; (iterDirs < MAX_DIRS) && (dirs[iterDirs * (MAX_DIRSNAME + 1) + 1] != 0x00); iterDirs++)
	{
	}
	if (iterDirs == SECTOR_SIZE)
	{
		*result = INSUFFICIENT_ENTRIES;
		// printString("INSUFFICIENT_ENTRIES", TRUE);
	}
	else
	{
		clear(dirPath, MAX_DIRS * (MAX_DIRSNAME + 1));
		clear(folderName, MAX_DIRSNAME);
		splitDirFilePath(path, dirPath, folderName);
		//cek apakah parent dari foldernya ada
		// printString("cari parent yang path : ", FALSE);
		// printString(dirPath, TRUE);
		searchDir(dirs, dirPath, &idxParent, &succ, parentIndex);
		if (succ)
		{
			//cek apakah sudah ada foldernya
			// printString("cari folder yang path : ", FALSE);
			// printString(path, TRUE);
			searchDir(dirs, path, &temp, &succ, parentIndex);
			if (succ)
			{
				*result = ALREADY_EXISTS;
				// printString("ALREADY_EXISTS", TRUE);
			}
			else
			{
				dirs[iterDirs * (MAX_DIRSNAME + 1)] = idxParent;
				len(folderName, &lenFolderName);
				for (temp = 0; temp < lenFolderName; temp++)
				{
					dirs[iterDirs * (MAX_DIRSNAME + 1) + temp + 1] = folderName[temp];
				}
				if (lenFolderName < 15)
				{
					dirs[iterDirs * (MAX_DIRSNAME + 1) + temp + 1] = '\0';
				}
				interrupt(0x21, 0x03, dirs, DIRS_SECTOR, 0);
				// writeSector(dirs, DIRS_SECTOR);
				*result = 0; //Success
			}
		}
		else
		{
			*result = NOT_FOUND;
			// printString("NOT_FOUND", TRUE);
		}
	}
}

/** Menghapus file pada path tertentu */
void deleteFile(char *path, int *result, char parentIndex)
{
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	int idxFile, iterSector;
	char succ;
	interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
	interrupt(0x21, 0x02, file, FILES_SECTOR, 0);
	interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
	interrupt(0x21, 0x02, sector, SECTORS_SECTOR, 0);
	searchFile(dirs, file, path, &idxFile, &succ, parentIndex);
	if (succ)
	{
		file[idxFile * (MAX_FILENAME + 1)] = 0x00;
		file[(idxFile * (MAX_FILENAME + 1)) + 1] = 0x00;
		for (iterSector = 0; (iterSector < MAX_SECTORS) && (sector[idxFile * MAX_SECTORS + iterSector] != 0x00); iterSector++)
		{
			map[sector[idxFile * MAX_SECTORS + iterSector]] = 0x00;
			sector[idxFile * MAX_SECTORS + iterSector] = 0x00;
		}
		interrupt(0x21, 0x03, dirs, DIRS_SECTOR, 0);
		interrupt(0x21, 0x03, file, FILES_SECTOR, 0);
		interrupt(0x21, 0x03, map, MAP_SECTOR, 0);
		interrupt(0x21, 0x03, sector, SECTORS_SECTOR, 0);
		*result = 0; //Success
	}
	else
	{
		*result = NOT_FOUND;
	}
}

//Logo di komen agar kernel muat di sektor
// void printLogo()
// {
// 	printStringXY("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *", 0xD, 0, 0);
// 	printStringXY("* ______  _                     _   _                 _____        _  *", 0xD, 0, 1);
// 	printStringXY("* | ___ \\(_)                   | \\ | |               |  _  |      (_) *", 0xD, 0, 2);
// 	printStringXY("* | |_/ / _  _ __ ___    ___   |  \\| |  ___   _ __   | | | | _ __  _  *", 0xD, 0, 3);
// 	printStringXY("* | ___ \\| || '_ ` _ \\  / _ \\  | . ` | / _ \\ | '_ \\  | | | || '__|| | *", 0xD, 0, 4);
// 	printStringXY("* | |_/ /| || | | | | || (_) | | |\\  || (_) || | | | \\ \\_/ /| |   | | *", 0xD, 0, 5);
// 	printStringXY("* \\____/ |_||_| |_| |_| \\___/  \\_| \\_/ \\___/ |_| |_|  \\___/ |_|   |_| *", 0xD, 0, 6);
// 	printStringXY("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *", 0xD, 0, 7);
// }

/** Menghapus suatu dirrectory pada path tertentu */
void deleteDirectory(char *path, int *success, char parentIndex)
{
	char dirs[SECTOR_SIZE], file[SECTOR_SIZE], map[SECTOR_SIZE], sector[SECTOR_SIZE];
	char copied[15];
	int idxFolder, iterFile, iterSector, iterDirs, j;
	char succ;
	// printString("Menghapus : ", FALSE);
	// printString(path, TRUE);
	interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
	interrupt(0x21, 0x02, file, FILES_SECTOR, 0);
	interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
	interrupt(0x21, 0x02, sector, SECTORS_SECTOR, 0);
	searchDir(dirs, path, &idxFolder, &succ, parentIndex);
	if (succ)
	{
		//Hapus semua dirs yang ada di folder ini secara rekursif
		// printString("Hapus semua folder di folder ", FALSE);
		// printString(path, TRUE);
		for (iterDirs = 0; iterDirs < SECTOR_SIZE; iterDirs += (MAX_DIRSNAME + 1))
		{
			if (dirs[iterDirs] == idxFolder && iterDirs != idxFolder && dirs[iterDirs + 1] != '\0')
			{
				//HAPUS
				//copy dulu namanya, buat bikin path
				clear(copied, 15);
				for (j = 0; j < 15; j++)
				{
					copied[j] = dirs[iterDirs + j + 1];
				}
				deleteDirectory(copied, success, idxFolder);
				interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
				interrupt(0x21, 0x02, file, FILES_SECTOR, 0);
				interrupt(0x21, 0x02, map, MAP_SECTOR, 0);
				interrupt(0x21, 0x02, sector, SECTORS_SECTOR, 0);
			}
		}
		//////////////////////////////////////////////////////////
		//Hapus semua file yang ada di folder ini
		// printString("Hapus semua file di folder ", FALSE);
		// printString(path, TRUE);
		for (iterFile = 0; iterFile < SECTOR_SIZE; iterFile += (MAX_FILENAME + 1))
		{
			//printInt(iterFile, TRUE);
			if (file[iterFile] == idxFolder && file[iterFile + 1] != 0x00)
			{
				// printString("Ada file disini, idxnya : ", FALSE);
				// printInt(div(iterFile,16), TRUE);
				file[iterFile] = 0x00;
				file[iterFile + 1] = 0x00;
				for (iterSector = 0; (iterSector < MAX_SECTORS) && (sector[iterFile * MAX_SECTORS + iterSector] != 0x00); iterSector++)
				{
					map[sector[iterFile * MAX_SECTORS + iterSector]] = 0x00;
					sector[iterFile * MAX_SECTORS + iterSector] = 0x00;
				}
			}
		}
		// printString("mengosongkan idx : ", FALSE);
		// printInt(idxFolder, TRUE);
		dirs[(idxFolder * 16)] = 0x00;
		dirs[(idxFolder * 16) + 1] = 0x00;
		interrupt(0x21, 0x03, dirs, DIRS_SECTOR, 0);
		interrupt(0x21, 0x03, file, FILES_SECTOR, 0);
		interrupt(0x21, 0x03, map, MAP_SECTOR, 0);
		interrupt(0x21, 0x03, sector, SECTORS_SECTOR, 0);
		*success = 0; //Sucess
	}
	else
	{
		*success = NOT_FOUND;
	}
}

void putArgs(char curdir, char argc, char argv[64][128])
{
	char args[SECTOR_SIZE];
	int i, j, p;
	clear(args, SECTOR_SIZE);

	args[0] = curdir;
	args[1] = argc;
	i = 0;
	j = 0;

	for (p = 2; p < ARGS_SECTOR && i < argc; ++p)
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
	for (p = 2; p < ARGS_SECTOR; ++p)
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
