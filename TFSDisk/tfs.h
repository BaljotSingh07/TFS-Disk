#include <math.h>

// defines the "byte" type as an unsigned 8-bit integer, valued 0 to 255
typedef unsigned char byte;

// this is the TFS currently in memory, globally available to all functions
// the first index is that of the 16 blocks of the TFS
// the second index is that of the 16 bytes within a block
byte tfs[16][16];

void init();

void display();

// TODO: many more functions

void saveTFS(char* file);

int import(char* linuxFile, char* TFSPath);

int makeDir(char* TFSPath);

void export(char* linuxPath, char* TFSPath);

void ls(char* TFSPath);

int getDR(char* path);

int isFull();

int nameToBlock(char name, int row);

void setFile(char name, int row);
