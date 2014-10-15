
#ifndef ALPHALEN
#define ALPHALEN 26
#endif

#ifndef __TRIE_OV_LIFE__
#define __TRIE_OV_LIFE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"
#include "St4x.h"


struct Node {

	struct Node* children[ALPHALEN];
	int isDictWord;
	int superwords;
	int matches;
	SortedListPtr sl;
};
typedef struct Node Node;
// this is going ot have to become a global at some point
struct TrieT {
	Node* root;	
	char* file;
};
typedef struct TrieT TrieT;


// we need to turn THIS one in
void readDict(FILE *dict_file, char* filename);
void initialize();
void destroy();
void scanData(FILE *dict_file);
void printAllData();
void storeData();

//void printData();

// auxiliary functions
Node* nodeFactory();
void destroyHelper(Node* n);
int ascii2index(char c);
char index2ascii(int index);
int TrieInsert(FILE *dict_file, Node *current);
void addWord(char* word);
int printAllDataHelper(Node*, St4x *s, char c, int prefixes);
//int printDataHelper(Node *current, St4x *s, char c, int prefixes);
#endif
