#include "St4x.h"


struct CQNode {

	char c;
	CQNode *next;
};

struct CQueue {
	CQNode *front, *rear;
}

void
CQEnqueue(char c, CQueue *q)
{
	CQNode *n = (CQNode*)malloc(sizeof(CQNode));
	if (n == NULL)
		return;

	// case 1: queue is empty
	if (q->front == NULL) {
		
		q->front = n;
		q->rear = n;
		return;
	}

	// case 2: queue is not empty -- push rear
	n->next = q->rear;
	q->rear = n->next;
}

char
CQDequeue(CQueue *q)
{
	char c = q->front->c;
	q-	
}
