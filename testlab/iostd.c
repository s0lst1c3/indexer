#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "iostd.h"

static FYLE* initFyle(int bufferSize, char mode);

struct FYLE {

	char *buffer;
	int fileDescriptor;
	int bufferMaxSize;
	int bufferSize;
	int bytesUsed;
	char mode;
};

FYLE*
openf(char *filename, int bufferSize, char mode)
{
	// local declarations
	FYLE *pHandle = NULL;
	int flags    = 0;
	mode_t fmode  = 0;

	pHandle = initFyle(bufferSize, mode);
	if (pHandle == NULL)
		return NULL;

	// pass different arguments to open depending on mode then initialize
	if (mode == 'w') {

		pHandle->fileDescriptor = open(filename, O_WRONLY | O_CREAT);
		printf("oh my lawd\n");

	}
	else if (mode == 'r') {

		pHandle->fileDescriptor = open(filename, O_RDONLY);
	}
	else {

		// return null if passed unsupported mode
		closef(pHandle);
		return NULL;
	}
	
	// return NULL if unable to get file descriptor
	if (pHandle->fileDescriptor == 0) {

		closef(pHandle);
		return NULL;
	}

	return pHandle;
}
static FYLE*
initFyle(int bufferSize, char mode)
{
	FYLE *pHandle = NULL;


	pHandle = (FYLE*)malloc(sizeof(FYLE));
	if (pHandle == NULL)
		return NULL;
	
	pHandle->buffer = (char*)malloc(sizeof(char) * bufferSize);
	if (pHandle->buffer == NULL)
		return NULL;

	memset(pHandle->buffer, ' ', bufferSize);
	pHandle->bytesUsed = 0;
	pHandle->bufferMaxSize = bufferSize;
	pHandle->bufferSize = 0;
	pHandle->fileDescriptor = 0;
	pHandle->mode = mode;
	return pHandle;
}

void
closef(FYLE *pHandle)
{
	// sanity check
	if (pHandle == NULL)
		return;

	// if mode is 'w' and there are still bytes left in the buffer, then
	// we must write them to the file before freeing
	if (pHandle->bytesUsed > 0) {

		write(pHandle->fileDescriptor,
				pHandle->buffer,(size_t)pHandle->bytesUsed);	
	}

	free(pHandle->buffer);	
	free(pHandle);
	return;
}

char
fgetachar(FYLE *pHandle)
{
	char c;

	if (pHandle == NULL)
		return EOF;

	if (pHandle->mode != 'r') {
		return EOF;
	}

	if (pHandle->bytesUsed < pHandle->bufferSize) {
		c = pHandle->buffer[pHandle->bytesUsed++];
		return c;
	}
	pHandle->bufferSize = read(pHandle->fileDescriptor, pHandle->buffer,
							(size_t)pHandle->bufferMaxSize);
	if (pHandle->bufferSize < 1)
		return EOF;
	
	c = pHandle->buffer[0];
	pHandle->bytesUsed = 1;
	return c;
}

void
fputachar(FYLE *pHandle, char c)
{
	if (pHandle == NULL)
		return;

	if (pHandle->mode != 'w')
		return;

	if (pHandle->bytesUsed >= pHandle->bufferMaxSize) {
		write(pHandle->fileDescriptor,
				pHandle->buffer,(size_t)pHandle->bufferMaxSize);	
		pHandle->bytesUsed = 0;
	}
	pHandle->buffer[pHandle->bytesUsed++] = c;
}



