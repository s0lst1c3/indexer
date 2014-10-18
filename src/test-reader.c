#include<stdio.h>
#include "reader.h"


int
main(int argc, char **argv)
{
	FILE *inHandle = fopen(argv[1], "r");
	if (inHandle == NULL)
		return 0;

	while(1) {
		char *nextWord = getWord(inHandle);
		if (nextWord == NULL)
	 		break;
		free(nextWord);
	}
	fclose(inHandle);
	return 0;
}
