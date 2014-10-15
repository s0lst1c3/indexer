#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "hashtable.h"


struct Config {

	char *basedir;
	char *outputFile;
	int flag_fileonly;
	int flag_errmsg;
	DankHash *index;
};
typedef struct Config Config;

// functions in main
Config *setup(int argc, char** argv);
void direntL0rd(Config *config);
void endOfJob(Config *config);


// functions in direntL0rd
void dirGinder();
