/* File: sorted-list.c
 * Project: Assignment 2 - Sorted List
 * Authors: John Ryan, Lucas Rivera
 * Netid(s): jgr68, lr411
 * Date Created: 30 Sep 2014
 * Last Modified: 01 Sep 2014
 * Email: john.ryan@rutgers.edu, lr411@rutgers.edu
 * Description:
 * Run time Analysis:
 */

#include <stdlib.h>
#include <stdio.h>
#include "sorted-list.h"

typedef struct _Node {

	void *data;
	struct _Node  *next;
	int refCount;
} Node;

struct SortedList {

	Node *front;
	CompareFuncT comparator;
	DestructFuncT destroyer;
};

struct SortedListIterator {

	Node *current;
	DestructFuncT destroyer;
};

static Node *nodeFactory(void *newObj);

SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df, Update up) {

	// sanity check
	if (!(cf && df))
		return NULL;

	SortedListPtr sl = (SortedListPtr)malloc(sizeof(struct SortedList));
	if (!sl) {
		fprintf(stderr,"Malloc failed in %s on line %d\n",__FILE__,__LINE__-2);
		return NULL;
	}

	sl->comparator = cf;
	sl->destroyer = df;
	sl->front = NULL;

	return sl;
}

void SLDestroy(SortedListPtr list) {
	if (!list)
		return;
	while (list->front) {
		Node *tmp = list->front;
		list->front = list->front->next;
		list->destroyer(tmp->data);
		free(tmp);
	}
	free(list);
}


int SLInsert(SortedListPtr list, void *newObj) {
	
	// validate parameters
	if (!(list && newObj))
		return 0;
	
	// create a new node
	Node *newNode = nodeFactory(newObj);

	// if the list is empty, then set front to new node
	if (!list->front) {
		list->front = newNode;
		list->front->next = NULL;
		return 1;
	}
	int result = list->comparator(list->front->data, newObj);
	if (!result) {
		perror("Cowardly refusing to insert a duplicate entry\n");
		return 0;
	}
	else if (result < 0) {
		newNode->next = list->front;
		list->front = newNode;
		return 1;
	}

	Node *current = list->front->next;
	Node *prev = list->front;
	while(current) {

		result = list->comparator(current->data, newObj);
		if (!result) {
			perror("Cowardly refusing to insert a duplicate entry\n");
			return 0;
		}
		if (result < 0) {
			newNode->next = current;
			prev->next = newNode;
			return 1;
		}
		prev = current;
		current = current->next;
	}
	prev->next = newNode;
	return 1;
}

static Node *nodeFactory(void *newObj) {

	if (!newObj)
		return NULL;
	
	Node *tmp = (Node*)malloc(sizeof(Node));
	if (!tmp)
		return NULL;

	tmp->data = newObj;
	tmp->next = NULL;
	tmp->refCount = 1;
	return tmp;
}

int SLRemove(SortedListPtr list, void *delMe) {

	if (!(list && delMe) || !list->front)
		return 0;
	
	// check if delMe is at the front of the list
	if (list->comparator(list->front->data, delMe) == 0) {

		Node* tmp = list->front;
		list->front = list->front->next;

		if (--tmp->refCount) {

			list->front->refCount++;
		}
		else {
			free(tmp->data);
			free(tmp);
		}
		return 1;
	}

	Node *prev = list->front;
	Node *current = prev->next;
	while(current) {

		if(!list->comparator(current->data, delMe)) {

			prev->next = current->next;
			if (--current->refCount) {

				if (current->next) {

					current->next->refCount++;
				}
			}
			else {

				list->destroyer(current->data);
				free(current);
			}
			return 1;
		}
		prev = current;
		current = current->next;
	}
	return 0;
}

SortedListIteratorPtr SLCreateIterator(SortedListPtr list) {

	if (!list) {

		return NULL;
	}

	SortedListIteratorPtr iter;
	iter = (SortedListIteratorPtr)malloc(sizeof(struct SortedListIterator));

	if (!iter) {

		fprintf(stderr,"Malloc failed in %s on line %d\n",__FILE__,__LINE__-2);
		return NULL;
	}

	iter->current = list->front;
	iter->current->refCount++;
	iter->destroyer = list->destroyer;
	return iter;
}


void SLDestroyIterator(SortedListIteratorPtr iter) {

	// refuse to entertain what doesn't exist
	if (!iter) {

		return;
	}

	if (!iter->current) {

		free(iter);	
		return;
	}

	iter->current->refCount--;
	if (iter->current->refCount == 0) {

		Node* tmp = iter->current;
		iter->current = iter->current->next;
		free(tmp);
		iter->current->refCount--;
	}
	free(iter);
}


void *SLGetItem( SortedListIteratorPtr iter ) {

	return !iter || !iter->current ? NULL : iter->current->data;
}

void *SLNextItem(SortedListIteratorPtr iter) {

	if (!iter || !iter->current || !iter->current) {

		return NULL;
	}
	
	if (!iter->current->next) {

		if (!iter->current->refCount) {

			free(iter->current->data);
			free(iter->current);
			return NULL;
		}
		return NULL;
	}

	Node* tmp = iter->current;
	iter->current = iter->current->next;
	if (--tmp->refCount) {

		iter->current->refCount++;
	}
	else {
		iter->destroyer(tmp->data);
		free(tmp);
		iter->current->refCount++;
	}

	return iter->current->data;
}
