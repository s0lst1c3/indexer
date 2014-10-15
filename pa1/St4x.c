#include <stdio.h>
#include <stdlib.h>
#include "St4x.h"

struct SNode {

	struct SNode *prev;
	char c;
};
typedef struct SNode SNode;

struct St4x {
	
	SNode *top;
	int size;
};

char STXPop(St4x *s) {

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

void STXDestroy(St4x *s) {

	if (s == NULL) {

		return;
	}

	while(s->size > 0) {
		
		STXPop(s);
	}
	free(s);
}

static void Gutenb0rg(SNode *current) {

	if (current == NULL) {

		return;
	}

	Gutenb0rg(current->prev);
	printf("%c", current->c);
}
void STXPrint(St4x *s) {

	if (s == NULL) {
		return;
	}
	Gutenb0rg(s->top);
}


int STXPush(char c, St4x *s) {

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

St4x *STXSpawn() {

	St4x *s = (St4x*)malloc(sizeof(St4x));
	if (s == NULL) {

		return 0;
	}

	s->top = NULL;
	s->size = 0;
	return s;
}

int  STXLen(St4x *s) {

	return s->size;
}

