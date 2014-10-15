#ifndef __HASH_TABLA__
#define __HASH_TABLA__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HashMap;
typedef struct HashMap HashMap;
typedef void (*DestructFuncT)( void * );

HashNode *HMNodeFactory(char *key);
int HMUpsert(char *token, char* filename, HashMap *hm);
int HMDeleteToken(char *token, HashMap *hm);
int HMGet(char *token, HashMap *hm);
HashMap *HMCreate();
void HMDestroy(HashMap *hm);
void HMPrintSorted();
int hashFunction(char* token);
#endif
