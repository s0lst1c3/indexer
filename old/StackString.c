#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "St4x.h"
#include "StackString.h"

#define DEFAULT_START_SIZE 100

struct StackString {
	char *arr;
	St4x *lenstack; // a stack of the previous len values
	int len; // logical length of the string
	size_t size; // physical length of the string
};

void SSPop(StackString *ss) {

	if (ss->size == 0 || ss->len == 0)
		return;

	
	int prevlen = STXPop(ss->lenstack);
	ss->len = prevlen;
	ss->arr[ss->len] = '\0';
}
void SSPush(char *str, StackString *ss) {

	int inLen = strlen(str);
	while (ss->len + inLen >= ss->size) {

		SSGrow(ss);
	}
	int oldLen = ss->len;
	int i = 0;
	int j = oldLen;
	while (i < inLen) {

		ss->arr[j++] = str[i++];
	}
	ss->len = j;
	ss->arr[j] = '\0';
	STXPush(oldLen, ss->lenstack);
}
void SSDestroy(StackString *ss) {

	free(ss->arr);
	free(ss->lenstack);
	free(ss);

}
StackString * SSCreate() {

	// allocate memory for the StackString
	StackString *ss = (StackString*)malloc(sizeof(StackString) * DEFAULT_START_SIZE);
	if ( ss == NULL )
		return NULL;

	// initalize size and len
	ss->len = 0;
	ss->size = DEFAULT_START_SIZE;

	// allocate space for the string
	ss->arr = (char*)malloc(sizeof(char) * DEFAULT_START_SIZE);
	if ( ss == NULL )
		return NULL;

	// create the len stack
	ss->lenstack = STXSpawn();
	if (ss->lenstack == NULL)
		return NULL;

	// initialize string to null
	int i;
	for (i = 0; i < ss->size; ++i) {

		ss->arr[i] = ' ';
	}
	ss->arr[0] = '\0';
	
	return ss;
}
void SSSet(StackString* ss, char *str) {

	// get the input size
	int i;
	for(i = 0; *(str + i) != '\0'; ++i);
	int newLen = i;

	// increase the size of the string if necessary
	while (newLen >= ss->size) {
		
		SSGrow(ss);
	}

	// set ss to the new string
	for(i = 0; i < newLen; ++i) {

		ss->arr[i] = str[i];
	}
	ss->arr[i] = '\0';
	ss->len = i;
}
char *SSGet(StackString *ss) {

	return ss->arr;
}
void SSGrow(StackString *ss) {

	ss->size *= 2;
	char* next = (char*)malloc(sizeof(char) * ss->size);
	int i = 0;
	while ( ss->arr[i] != '\0' ) {

		next[i] = ss->arr[i];
		i++;
	}
	next[i] = '\0';
	free(ss->arr);
	ss->arr = next;
}

