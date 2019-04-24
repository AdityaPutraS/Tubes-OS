#ifndef DIR_H
#define DIR_H

#define TRUE 1
#define FALSE 0
#define ARGS_SECTOR 512
#define SECTOR_SIZE 512
#define EMPTY 0x00

void clear(char *buffer, int length);
void search(char *sector, char awal, char sisanya[15], char *index, char *success);
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex);
void searchFile(char *dirs, char *file, char *relPath, char *index, char *success, char parentIndex);
void splitDirFilePath(char *path, char *dirPath, char *fileName);
void putArgs(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char index, char *argv);

#endif