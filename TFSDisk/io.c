#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "io.h"


void printc(char c) {
  // your code here
	write(STDOUT_FILENO, &c, sizeof(char));
}

void prints(char* s) {
  write(STDOUT_FILENO, s, strlen(s));
}

void println() {
  printc('\n');
}

int noTFSOpen(char* tfs_file){
	if(!tfs_file){
			prints("No file open.");
			println();
			return -1;
	}
	return 0;
}

int noArg(char* arg, char message[]){
	if(!arg){
		prints(message);
		println();
		return -1;
	}
	return 0;
}

char intToHex(int i){
	if(i <= 9){
		return i+'0';
	}
	if(i == 10) return 'a';
	if(i == 11) return 'b';
	if(i == 12) return 'c';
	if(i == 13) return 'd';
	if(i == 14) return 'e';
	if(i == 15) return 'f';
	return '0';
}

void inputs(char dst[], int size) {
  // your code here
	read(STDIN_FILENO, dst, size);
}


void setLow(int row, int col, char nibble) {
    tfs[row][col] &= 0xF0; 
    tfs[row][col] |= (nibble & 0x0F);
}

void setHigh(int row, int col, char nibble) {
    tfs[row][col] &= 0x0F;
    tfs[row][col] |= ((nibble << 4) & 0xF0);
}
