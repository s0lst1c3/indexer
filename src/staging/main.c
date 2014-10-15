#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char** argv) {


	// input validation
	errorHandler(argc, argv);

	
	
	

	/*
	HashTable *h = HTInitialize();
	if (h == NULL) {
		// todo: error handler
		exit(1);
	}
	*/

	// make sure that we have write access to the current directory

	
	return 0;
}

int setup(int argc, char** argv) {


	// ensure we have the correct number of arguments
	if (argc != 3) {
		fprintf(stderr, "Usage: <output file> <input file>\n");
		errorHandler(__LINE__-2, __FILE__);
	}

	// make sure we can actually write to the directory with filename
	FILE *invIndex = fopen(argv[1], "w");
	if (invIndex == NULL) {

		errorHandler(__LINE__, __FILE__);
	}
	fclose(invIndex);

	// figure out if our second argument is a directory or file
	struct stat statbuf;
	if ( stat(argv[2], &statbuf) == -1) {

		errorHandler(__LINE__, __FILE__);
	}

	if (S_ISREG(statbuf)) {

		addIndexFromFilename(
	}
	
	FILE *searchBase = argv[2];

	
	
	
	return 0;
}

int errorHandler(int lineNumber, const char *filename) {

	fprintf(stderr, "In %s on line %d:\n", filename, lineNumber);
	if (errno) {
		char *errMessage = strerror(errno);
		perror(errMessage);
	}
	exit(1);
}
