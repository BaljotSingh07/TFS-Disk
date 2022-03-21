#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>
#include "tfs.h"
#include "io.h"

// initializes the TFS in memory
void init() {

  // first block is root directory
  // - there is free space, and first free block is 01
  tfs[0][0] = 1;
  // - root dir contents and their locations are all 0
  for (int i = 1; i < 16; i++)
    tfs[0][i] = 0;

  // remaining blocks are free
  // each free block points to the next one,
  // except the last block which stores 255 in first byte
  for (int i = 1; i < 16; i++)
    tfs[i][0] = i == 15 ? 0xFF : i + 1;
    // free block data (beyond byte 0) is undefined (don't need to set)
}

// display the TFS as columns of hex digits
void display() {
  // your code here
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 16; j++){
			printc(intToHex(tfs[i][j] >> 4));
			printc(intToHex(tfs[i][j] & 0x0F));
			printc(' ');
		}
		println();
	}
}

// TODO: many more functions, to handle other commands
void saveTFS(char* file){
	int f = open(file, O_WRONLY | O_CREAT, 0777);
	write(f, tfs, 256);
	close(f);
}

int import(char* linuxFile, char* TFSPath){
	char fileName = TFSPath[strlen(TFSPath)-1];
	if(isupper(fileName)){
		prints("All files is TFS must be lowercase");
		println();
		return -1;
	}
	int DR = getDR(TFSPath);
	if(nameToBlock(fileName, DR) != -1){
		prints("File already exist");
		println();
		return -1;
	}
	if(DR == -1){
		prints("Invalid Path");
		println();
		return -1;
	}
	struct stat st;
	stat(linuxFile, &st);
	int size = st.st_size;
	int f = open(linuxFile, O_RDONLY);
	int freeSpace = tfs[0][0];
	int blocksNeeded = 0;
	
	if(size <= 15){
		read(f,&tfs[freeSpace][1],16); 
	}else{
		blocksNeeded = ceil((size-8)/16.0);
		//set block pointers
		for(double i = 1; i <= (blocksNeeded+1)/2.0;i=i+0.5){
			freeSpace++;
			if((int)i == i){
				setHigh(tfs[0][0],(int)i,freeSpace);
			}else{
				setLow(tfs[0][0],(int)i,freeSpace);
			}
		}
		
		freeSpace = tfs[0][0];
		read(f,&tfs[freeSpace][8],256);
	}
	tfs[tfs[0][0]][0] = size;
	setFile(fileName, DR);
	tfs[0][0] = tfs[0][0]+blocksNeeded;
	return 0;
}

int makeDir(char* TFSPath){
	char dirName = TFSPath[strlen(TFSPath)-1];
	int DR = getDR(TFSPath);
	if(nameToBlock(dirName, DR) != -1){
		prints("Directory already exist");
		println();
		return -1;
	}
	if(islower(dirName)){
		prints("All directory is TFS must be uppercase");
		println();
		return -1;
	}
	if(DR == -1){
		prints("Invalid TFS Path");
		println();
		return -1;
	}
	setHigh(tfs[0][0],0,DR);
	setLow(tfs[0][0],0,tfs[0][0]);
	setFile(dirName,DR);
	for(int i = 1; i <= 15; i++){
		tfs[tfs[0][0]][i] = 0;
	}
	return 0;
}

void export(char* TFSPath, char* linuxPath){
	char fileName = TFSPath[strlen(TFSPath)-1];
	int DR = getDR(TFSPath);
	DR = nameToBlock(fileName,DR);
	if(DR == -1 || isupper(fileName)){
		prints("Invalid TFS Path");
		println();
		return;
	}
	int f = open(linuxPath, O_WRONLY | O_CREAT |  O_TRUNC, 0777);
	if(f == -1){
		prints("Error writing to file: ");
		prints(linuxPath);
		println();
		return;
	}
	if(tfs[DR][0] <= 15)
		write(f,&tfs[DR][1],tfs[DR][0]);
	else
		write(f,&tfs[DR][8],tfs[DR][0]);
	close(f);
}

void ls(char* TFSPath){
	char last = TFSPath[strlen(TFSPath)-1];
	int DR = getDR(TFSPath);
	if(last != '/')
		DR = nameToBlock(last,DR);
	if(DR == -1){
		prints("Invalid TFS Path");
		println();
		return;
	}
	for(int i = 1; i < 11 && tfs[DR][i]; i++){
		printc(tfs[DR][i]);
		printc(' ');
	}
	println();
}

int getDR(char* path){
	int DR = 0;
	if(strlen(path) <= 2)
		return 0;
	path[strlen(path)-1] = '\0';
	char* token = strtok(path, "/");

  while( token != NULL ) {
		if(strlen(token) != 1)
			return -1;
		DR = nameToBlock(token[0],DR);
		token = strtok(NULL, "/");
		//No direcotry found
		if(DR == -1){
			return -1;
		}
  }
	return DR;
}

int isFull(){
	if((tfs[0][0] & 0x0F) == 0){
		return 1;
	}
	return 0;
};

int nameToBlock(char name, int row){
	for(int i =1; i <= 10; i++){
		if(tfs[row][i] == name){
			int col = ceil(10+(i/2.0));
			if(i % 2 == 0){
				return tfs[row][col] & 0x0F;
			}else{
				return tfs[row][col] >> 4;
			}
		}
	}
	return -1;
}

void setFile(char name, int row){
	for(int i =1; i <= 10; i++){
		if(tfs[row][i] == 0){
			tfs[row][i] = name;
			int col = ceil(10+(i/2.0));
			if(i % 2 == 0){
				setLow(row, col, tfs[0][0]);
			}else{
				setHigh(row, col, tfs[0][0]);
			}
			tfs[0][0] = tfs[0][0] + 1; 
			return;
		}
	}
}
