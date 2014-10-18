#ifndef __ST4X__
#define __ST4X__

struct St4xC;
typedef struct St4xC St4xC;

char STXCPop(St4xC *s);
void STXCDestroy(St4xC *s);
void STXCPrint(St4xC *s);
int  STXCPush(char c, St4xC *s);
int  STXCLen(St4xC *s);
St4xC *STXCSpawn();

#endif
