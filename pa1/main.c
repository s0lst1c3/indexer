#include <stdio.h>
#include <stdlib.h>
#include "dictstat.h"

void errorHandler(char* filename, int lineNo) {

	fprintf(stderr, "Error in %s line %d: \n", filename, lineNo);
	exit(1);
}
FILE* openFile(char* inFile) {

	if (inFile == NULL) {
		errorHandler(__FILE__, __LINE__);
	}

	FILE* inHandle = fopen(inFile, "r");
	if (inHandle == NULL) {
		errorHandler(__FILE__, __LINE__-2);
	}
	return inHandle;
}

int main(int argc, char** argv) {

	if (argc != 3) {

		fprintf(stderr,"invalid input\n");
		exit(1);
	}
	
	initialize();

	FILE* dictHandle = openFile(argv[1]);
	FILE* datHandle = openFile(argv[2]);

	readDict(dictHandle, argv[1]);
	scanData(datHandle);
	storeData();

	printAllData();

	fclose(dictHandle);
	fclose(datHandle);


	destroy();	

	return 0;
}


