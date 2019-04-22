#include "stringUtil.h"


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