#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "PathStack.h"
#include "sorted-list.h"
#include "indexer.h"


struct Config {

	char *basedir;
	char *outputFile;
	int flag_fileonly;
	int flag_errmsg;
	SortedList *tokens;
};
typedef struct Config Config;

struct TokenCount {

	char *basedir;
	char *outputFile;
	int flag_fileonly;
	int flag_errmsg;
	SortedList *tokens;
};
typedef struct Config Config;

Config *setup(int argc, char** argv);
char* prepareFilename(char* filename);
void validateInputs(char *inputFile, char *outputFile, Config *config);

void dirTraverser(Config *config);
void dirTraverserHelper(PathStack*, Config*);
void fileParser(PathStack *path, Config *config);

void endOfJob(Config *config);
