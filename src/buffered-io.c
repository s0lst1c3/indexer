#include "buffered-io.h"

struct Input {

	int fd; // file descriptor
	int length;
	int used;
	char buffer[2048];
}

void
resetInput(struct Input *iptr, int fd)
{
	iptr->fd = fd;
	iptr->used = 0;
	iptr->length = 0;
}

int
readInput(struct Input *iptr)
{
	char c;

	// if there are bytes still in buffer, return those bytes
	if (iptr->used < iptr->length) {

		c = iptr->BUFFER[iptr->used];
		iptr->used += 1;
		return c;
	}
	// attempt to copy bytes from the file
	iptr->length = read(iptr->df, iptr->buffer, sizeof(iptr->buffer));

	// if we've reached the end of the file, return EOF
	if (iptr->length < 1)
		return EOF;

	// otherwise return the next character in the new buffer
	c = iptr->buffer[0];
	iptr->used = 1;
	return c;
}
