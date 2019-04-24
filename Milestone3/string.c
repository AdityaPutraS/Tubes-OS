#include "string.h"

/**
 * Mencari sisa bagi dari 2 bilangan
 */
int mod(int a, int b)
{
	while (a >= b)
	{
		a = a - b;
	}
	return a;
}

/**
 * Mencari hasil bagi 2 bilangan, rounded down
 */
int div(int a, int b)
{
	int q = 0;
	while (q * b <= a)
	{
		q = q + 1;
	}
	return q - 1;
}

/**
 * Mencetak string ke layar, jika newLine == TRUE maka akan mencetak \n
 */
void printString(char *string, int newLine)
{
	int i = 0;
	while (string[i] != '\0')
	{
		interrupt(0x10, 0xE00 + string[i], 0, 0, 0);
		i = i + 1;
	}
	if (newLine)
	{
		interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
		interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	}
}

//////FOR DEBUGGING PURPOSEs
/**
 * Mencetak char ke layar, jika newLine == TRUE maka akan mencetak \n
 */
void printChar(char c, int newLine)
{
	interrupt(0x10, 0xE00 + c, 0, 0, 0);
	if (newLine)
	{
		interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
		interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
	}
}

/**
 * Mencetak integer 9 digit ke layar, jika newLine == TRUE maka akan mencetak \n
 */
void printInt(int i, int newLine)
{
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
	while (i != 0 && j >= 0)
	{
		digit[j] = '0' + mod(i, 10);
		i = div(i, 10);
		j -= 1;
	}
	// interrupt(0x21, 0x00, digit, newLine, 0);
	printString(digit, newLine);
}
//////////////////////////

/**
 * Memisahkan suatu string menjadi sekumpulan string yang pada awalnya
 * dipisahkan oleh karakter separator
 */
void split(char *string, char separator, char splitted[32][15])
{
	int i, j, k;
	i = 0;
	j = 0;
	k = 0;
	clear(splitted[0], 15);
	while (string[i] != '\0')
	{
		if (string[i] == separator)
		{
			splitted[j][k] = '\0';
			j += 1;
			k = 0;
			clear(splitted[j], 15);
		}
		else
		{
			splitted[j][k] = string[i];
			k += 1;
		}
		i += 1;
	}
	splitted[j][k] = '\0';
}

/** Menghitung panjang dari sebuah string */
void len(char *string, int *length)
{
	*length = 0;
	while (string[*length] != '\0')
	{
		*length += 1;
	}
}

/** Menghitung banyaknya kemunculan karakter c pada sebuah string */
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

/** Menentukan apakah dua buah string sama atau tidak */
void isSame(char *s1, char *s2, char *result)
{
	int i;
	char tempResult;
	i = 0;
	tempResult = TRUE;
	while (s1[i] != '\0' && s2[i] != '\0' && tempResult)
	{
		if (s1[i] != s2[i])
		{
			tempResult = FALSE;
		}
		i += 1;
	}
	if (s1[i] != '\0' || s2[i] != '\0')
	{
		tempResult = FALSE;
	}
	*result = tempResult;
}

/** Menyalin isi dari sebuah (string) ke (string) copied */
void copy(char *string, char *copied, int start, int length)
{
	int lenS, lenC, i;
	len(string, &lenS);
	len(copied, &lenC);
	clear(copied, lenC);
	if (lenS < length)
	{
		clear(copied, lenS);
	}
	else
	{
		clear(copied, length);
	}
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
		}
	}
}

/**
 * Mengganti setiap karakter cari pada string s dengan tukar
 */
void replace(char *s, char cari, char tukar)
{
	int length, i;
	len(s, &length);
	for (i = 0; i < length; i++)
	{
		if (s[i] == cari)
		{
			s[i] = tukar;
		}
	}
	s[length] = '\0';
}