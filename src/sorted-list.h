/* File: sorted-list.h
 * Project: Assignment 2 - Sorted List
 * Authors: John Ryan
 * Netid(s): jgr68
 * Date Created: 30 Sep 2014
 * Last Modified: 01 Sep 2014
 * Email: john.ryan@rutgers.edu
 * Description: The header file for the sorted-list.h data type
 */

#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
 * sorted-list.h
 */

#include <stdlib.h>

struct SortedList;
struct SortedListIterator;

typedef struct SortedList* SortedListPtr;
typedef struct SortedListIterator* SortedListIteratorPtr;
typedef int    (*CompareFuncT)( void *, void * );
typedef void   (*DestructFuncT)( void * );

SortedListPtr         SLCreate ( CompareFuncT cf, DestructFuncT df );
void                  SLDestroy( SortedListPtr list );
int                   SLInsert ( SortedListPtr list, void *newObj );
int                   SLRemove ( SortedListPtr list, void *newObj );
SortedListIteratorPtr SLCreateIterator ( SortedListPtr list );
void                  SLDestroyIterator( SortedListIteratorPtr iter);
void                 *SLGetItem ( SortedListIteratorPtr iter );
void                 *SLNextItem( SortedListIteratorPtr iter );
void                 SLDebugPrint(SortedListPtr sl);

#endif
