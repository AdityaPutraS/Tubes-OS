#ifndef STRING_H
#define STRING_H

#define TRUE 1
#define FALSE 0

int mod(int a, int b);
int div(int a, int b);
void printString(char *string, int newLine);
void printInt(int i, int newLine);
void printChar(char c, int newLine);
void split(char *string, char separator, char splitted[32][15]);
void len(char *string, int *length);
void count(char *string, char c, int *banyak);
void isSame(char *s1, char *s2, char *result);
void copy(char *string, char *copied, int start, int length);
void replace(char *s, char cari, char tukar);

#endif