#ifndef DIR_H
#define DIR_H

#define TRUE 1
#define FALSE 0

void search(char *sector, char awal, char sisanya[15], char *index, char *success);
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex);
void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex);
void splitDirFilePath(char *path, char *dirPath, char *fileName);

#endif