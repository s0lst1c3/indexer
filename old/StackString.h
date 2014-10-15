#include <stdio.h>
#include "St4x.h"

#ifndef __STACK__STRING__
#define __STACK__STRING__

struct StackString;
typedef struct StackString StackString;

void SSGrow(StackString *ss);
char *SSGet(StackString *ss);
void SSPop(StackString *ss);
void SSSet(StackString *ss, char *str);
StackString * SSCreate();
void SSDestroy(StackString *ss);
void SSPush(char *str, StackString *ss);


#endif
