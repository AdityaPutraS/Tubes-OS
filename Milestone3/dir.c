#include "dir.h"

#define MAX_DIRS 32			 //Ditambah agar sesuai spek
#define MAX_DIRSNAME 15		 //Ditambah agar sesuai spek
#define MAX_FILENAME 15		 //Diubah agar sesuai spek

/** 
 * Mencari keberadaan sebuah(string) pada suatu sector
 * Biasanya digunakan untuk searching pada sector, dirs (0x101) dan files (0x102)
 */
void search(char *sector, char awal, char sisanya[15], char *index, char *success)
{
	int i, j;
	char copied[15];
	clear(copied, 15);
	*success = FALSE;
	*index = 0;
	// printString("Masuk search , cari : ", FALSE);
	// printString(sisanya, TRUE);
	for (i = 0; (i < 32) && !(*success); i++)
	{
		if (sector[i * 16] == awal)
		{
			//Kopi sebagian dari sektor
			clear(copied, 15);
			for (j = 0; j < 15; j++)
			{
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
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex)
{
	int i, pathLength, countSlash;
	char pathSplitted[MAX_DIRS][MAX_DIRSNAME];
	char cari[MAX_DIRSNAME];
	if (relPath[0] == '\0')
	{
		*success = TRUE;
		*index = parentIndex;
	}
	else
	{
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
		for (i = 0; (i < (countSlash + 1)) && *success; i++)
		{
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
void splitDirFilePath(char *path, char *dirPath, char *fileName)
{
	int i, lenCopy;
	len(path, &i);
	//abc/def/gg
	//0123456789
	//len = 10
	//dirPath = abc/def
	//fileName = gg
	i -= 1;
	lenCopy = 0;
	while (path[i] != '/' && i >= 0)
	{
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
void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex)
{
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