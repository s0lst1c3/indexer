#include <stdio.h>
#include <stdlib.h>
#include "St4xC.h"

struct SNode {

	struct SNode *prev;
	char c;
};
typedef struct SNode SNode;

struct St4xC {
	
	SNode *top;
	int size;
};

char STXCPop(St4xC *s) {

	if (s == NULL) {
		return 0x00;
	}

	if (s->top == NULL) {

		return 0x00;
	}

	char c = s->top->c;	
	SNode *tmp = s->top;
	s->top = s->top->prev;
	free(tmp);
	s->size--;
	return c;
}

void STXCDestroy(St4xC *s) {

	if (s == NULL) {

		return;
	}

	while(s->size > 0) {
		
		STXCPop(s);
	}
	free(s);
}

static void Gutenb0rg(SNode *current) {

	if (current == NULL) {

		return;
	}

	Gutenb0rg(current->prev);
}
void STXCPrint(St4xC *s) {

	if (s == NULL) {
		return;
	}
	Gutenb0rg(s->top);
}

int STXCPush(char c, St4xC *s) {

	if (s == NULL) {

		return 0;
	}
	
	
	SNode *n = (SNode*)malloc(sizeof(SNode));
	if (n == NULL) {

		return 0;
	}

	n->c = c;
	n->prev = NULL;
	
	if (s->size == 0) {

		s->top = n;
	}
	else {

		n->prev = s->top;
		s->top = n;
	}

	s->size++;
	return 1;
}

St4xC *STXCSpawn() {

	St4xC *s = (St4xC*)malloc(sizeof(St4xC));
	if (s == NULL) {

		return 0;
	}

	s->top = NULL;
	s->size = 0;
	return s;
}

int  STXCLen(St4xC *s) {

	return s->size;
}

