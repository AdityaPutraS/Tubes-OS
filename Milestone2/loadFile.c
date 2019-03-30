//loadFile.c
//Michael Black, 2007
//
//Loads a file into the file system
//This should be compiled with gcc and run outside of the OS

#include <stdio.h>

#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_ENTRIES 32
#define NAME_OFFSET 1
#define MAX_NAME 15
#define MAX_SECTORS 16
#define ENTRY_LENGTH 16
#define MAP_SECTOR 256
#define FILES_SECTOR 258
#define SECTORS_SECTOR 259
#define NOT_FOUND -1

void readSector (char *buffer, FILE *file, int sector) {
  fseek(file, sector * SECTOR_SIZE, SEEK_SET);
  int i;
	for (i = 0; i < SECTOR_SIZE; ++i) {
		buffer[i] = fgetc(file);
  }
}

void writeSector (char *buffer, FILE *file, int sector) {
  fseek(file, sector * SECTOR_SIZE, SEEK_SET);
  int i;
  for (i = 0; i < SECTOR_SIZE; ++i) {
		fputc(buffer[i], file);
  }
}

void copySector (FILE *src, int srcSector, FILE *dest, int destSector) {
  fseek(src, srcSector * SECTOR_SIZE, SEEK_SET);
  fseek(dest, destSector * SECTOR_SIZE, SEEK_SET);
  int i;
  for (i = 0; i < SECTOR_SIZE; ++i) {
    if (feof(src)) {
      fputc(0x00, dest);
      return;
    }
    else {
      fputc(fgetc(src), dest);
    }
  }
}

void writeName (char *entries, int index, char *name) {
  int i;
  for (i = 0; name[i] != '\0'; ++i) {
    entries[index * ENTRY_LENGTH + NAME_OFFSET + i] = name[i];
  }
  for (; i < MAX_NAME; ++i) {
    entries[index * ENTRY_LENGTH + NAME_OFFSET + i] = '\0';
  }
}

int findUnusedEntry (char *entries) {
  int i;
  for (i = 0; i < MAX_ENTRIES; ++i) {
    if (entries[i * ENTRY_LENGTH + NAME_OFFSET] == '\0') {
      return i;
    }
  }
  return NOT_FOUND;
}

int findUnusedSector (char *map) {
  int i;
  for (i = 0; i < MAX_BYTE; ++i) {
    printf("%c ", map[i]);
    if (map[i] == 0x00) {
      return i;
    }
  }
  return NOT_FOUND;
}

int main (int argc, char* argv[]) {
	if (argc < 2) {
		printf("Specify file name to load\n");
		return;
	}

	//open the source file
	FILE* loadFile;
	loadFile = fopen(argv[1], "r");
	if (loadFile == 0) {
		printf("File not found\n");
		return;
	}

	//open the floppy image
	FILE* floppy;
	floppy= fopen("floppya.img","r+");
	if (floppy==0) {
		printf("floppya.img not found\n");
		return;
	}

	//load map sector
	char map[SECTOR_SIZE];
	readSector(map, floppy, MAP_SECTOR);
    
  //load files sector
	char files[SECTOR_SIZE];
	readSector(files, floppy, FILES_SECTOR);
    
  //load sectors sector
	char sectors[SECTOR_SIZE];
	readSector(sectors, floppy, SECTORS_SECTOR);

	//find a free entry
  int index = findUnusedEntry(files);
  if (index != NOT_FOUND) {
    int sectorCount = 0;
    while (!feof(loadFile)) {
      int sector = findUnusedSector(map);
      if (sector != NOT_FOUND) {
        copySector(loadFile, sectorCount, floppy, sector);
        printf("Loaded %s to sector %d\n", argv[1], sector);
        map[sector] = 0xFF;
        sectors[index * ENTRY_LENGTH + sectorCount] = sector;
        ++sectorCount;
      }
      else {
        printf("Cannot load more files: sectors full\n");
        return;
      }
    }
    files[index * ENTRY_LENGTH] = 0xFF;
    writeName(files, index, argv[1]);
    writeSector(map, floppy, MAP_SECTOR);
    writeSector(files, floppy, FILES_SECTOR);
    writeSector(sectors, floppy, SECTORS_SECTOR);
  }
  else {
    printf("Cannot load more files: reached max files\n");
  }

	fclose(floppy);
	fclose(loadFile);
}
