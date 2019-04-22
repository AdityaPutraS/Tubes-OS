#ifndef STRING_H
#define STRING_H

#define TRUE 1
#define FALSE 0

void split(char *string, char separator, char splitted[32][15]);
void len(char *string, int *length);
void isSame(char *s1, char *s2, char *result);
void copy(char *string, char *copied, int start, int length);
void splitDirFilePath(char *path, char *dirPath, char *fileName);
void count(char *string, char c, int *banyak);

#endif