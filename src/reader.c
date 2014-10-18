#include "reader.h"
#include <ctype.h>


char*
getWord(FILE *inHandle)
{
	St4xC *s = STXCSpawn();
	// clear preceeding whitespace and nonletter chars
	char c = alphanumeric(fgetc(inHandle));
	while (c != EOF && c == '&')
		c = alphanumeric(fgetc(inHandle));

	// if we've reached the end of the file, break out of the loop
	if (c == EOF)
		return NULL;

	// while letters are alphanumeric and not EOF, push them onto the stack
	while (c != '&' && c != EOF) {
		STXCPush(c, s);
		c = alphanumeric(fgetc(inHandle));
	}

	STXCPrint(s);
	// create a string
	int len = STXCLen(s);
	char* word = (char*)malloc(sizeof(char) * len + 1);
	if (word == NULL) {
		fprintf(stderr, "In %s on line %d:\n", __FILE__, __LINE__);
		perror("Malloc faled.\n");
		exit(1);
	}
	word[len] = '\0';
	int i;
	for (i = len-1; i >= 0; --i) {

		word[i] = STXCPop(s);
	}
	STXCDestroy(s);
	return word;
}

char
alphanumeric(char c)
{
	if (c >= 'a' && c <= 'z' || c >= '0' && c <= '9')
		return c;
	if (c >= 'A' && c <= 'Z')
		return c + 0x20;
	if (c == EOF)
		return EOF;
	return '&';
}
