#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>
#include "tfs.h"
#include "io.h"

#define WHITESPACE " \t\r\n"

int main() {

  int done = 0;
  char* tfs_file = NULL;

  do {
    // prompt is current open file, or none
    char* prompt = tfs_file ? tfs_file : "(no file)";

    // print the prompt, then get a line of input from the user
    prints(prompt);
    prints("> ");
    char line[IN_SIZE] = " ";
    inputs(line, IN_SIZE);
    // flag denoting we changed the open TFS, and need to save it to the file
    // after this command has been processed
    int altered = 0;
    char* command = strtok(line, WHITESPACE); // user's command
    char* arg1 = strtok(NULL, WHITESPACE); // first argument
		char* arg2 = strtok(NULL, WHITESPACE);
    // TODO: some commands need one more argument

    if (!strcmp("exit", command)) {
      done = 1;
    }
    else if (!strcmp("display", command)) {
      // most commands don't make sense if there's no open file
     if(noTFSOpen(tfs_file))
				continue;
      display(); // display the TFS as columns of hex digits
    }
    else if (!strcmp("create", command)) {
      if(noArg(arg1,"Need a filepath."))
				continue;
      int temp = open(arg1, O_RDONLY);
      if (temp != -1) {
        prints("File already exists: ");
        prints(arg1);
        println();
        close(temp); // close inadvertently opened file
        continue;
      }
      free(tfs_file); // free previous string (if any)
      tfs_file = strdup(arg1); // save path for prompt and writing
      init(); // initialize new TFS
      altered = 1; // flag for saving to file
    }
    else if (!strcmp("open", command)) {
      if(noArg(arg1,"Need a filepath."))
				continue;
      int temp = open(arg1, O_RDONLY);
      if (temp == -1) {
        prints("Error opening file: ");
        prints(arg1);
        println();
        continue;
      }
      // load TFS from new file (previous file already saved)
      read(temp, (char*)tfs, 256);
      close(temp);
      free(tfs_file); // free previous string
      tfs_file = strdup(arg1); // save path for prompt and writing
    }else if(!strcmp("import",command)){
			if(noTFSOpen(tfs_file))
				continue;
			if(noArg(arg1,"Need a Linux path.") || noArg(arg2,"Need a path to TFS."))
				continue;
			if(isFull()){
				prints("TFS Full");
        println();
				continue;
			}
			int temp = open(arg1, O_RDONLY);
      if (temp == -1) {
        prints("Error reading file: ");
        prints(arg1);
        println();
				close(temp);
        continue;
      }
			struct stat st;
			stat(arg1, &st);
			int size = st.st_size;
			int maxSize = ((16-tfs[0][0]) * 16) - 8;
			if(size > maxSize){
				prints("File exceeded size limit");
				println();
				continue;
			}
			if(import(arg1,arg2) != -1)
				altered = 1;
		}else if(!strcmp("export",command)){
			if(noTFSOpen(tfs_file))
				continue;
			if(noArg(arg1,"Need a path to TFS.") || noArg(arg2,"Need a Linux path."))
				continue;
			export(arg1,arg2);
		}else if(!strcmp("makedir",command)){
			if(noTFSOpen(tfs_file))
				continue;
			if(noArg(arg1,"Need a path in TFS."))
				continue;
			if(isFull()){
				prints("TFS Full");
        println();
				continue;
			}
			if(makeDir(arg1) != -1)
				altered =1;
		}else if(!strcmp("ls",command)){
			if(noTFSOpen(tfs_file))
				continue;
			if(noArg(arg1, "Need a directory"))
				continue;
			ls(arg1);
		}
    // TODO: more commands
    else {
      prints("Command not recognized: ");
      prints(command);
      println();
    }
		
    if (altered == 1) {
      // save changes to file
      // TODO: your code here
			saveTFS(tfs_file);
    }
  } while (!done);
	
  return 0;
}
