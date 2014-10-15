#include <stdio.h>
#include "St4x.h"

#ifndef __STACK__STRING__
#define __STACK__STRING__

struct PathStack {
	char *arr;
	St4x *lenstack; // a stack of the previous len values
	int len; // logical length of the string
	size_t size; // physical length of the string
	int levels;
};
typedef struct PathStack PathStack;

void PSGrow(StackString *ss);
char *PSGet(StackString *ss);
void PSPop(StackString *ss);
PathStack * PSCreate();
void PSDestroy(StackString *ss);
void PSPush(char *str, StackString *ss);


#endif
