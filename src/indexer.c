#include <stdio.h>
#include <stdlib.h>
#include "indexer.h"

#ifndef ALPHALEN
#define ALPHALEN 36
#endif

TrieT base;

// occurrences of a token in a particlar file
struct Occurrences {
	char *file;
	int count;
};
typedef struct Occurrences Occurrences;

// destroys an Occurrences struct
void 
destroyOccurrences(void *p)
{
	Occurrences *oc = (Occurrences*)p;
	free(oc->file);
	free(oc);
}

// updates an Occurrences struct
void update(void *p)
{
	Occurrences *oc = (Occurrences*)p;
	oc->count++;
}

// creates an Occurrences struct with a particular file name and count
Occurrences *
OccurrencesFactory(char *file, int count)
{
	Occurrences *oc = (Occurrences*)malloc(sizeof(Occurrences));
	int len = strlen(file);
	int i = 0;
	oc->file = (char*)malloc(sizeof(char) * len + 1);
	while ( *(file + i) != '\0' ) {
		oc->file[i] = file[i];
		i++;
	}
	oc->file[i] = '\0';
	oc->count = count;
	return oc;
}

// compares two Occurrences struct
int
compareOccurrences(void *p1, void *p2)
{
	Occurrences *o1 = p1;
	Occurrences *o2 = p2;
	return o1->count - o2->count;
}

// creates a Node
Node*
nodeFactory()
{
	// instantiate new Node
	Node* next = (Node*)malloc(sizeof(Node));
	if (next == NULL) {

		fprintf(stderr, "in %s on line %d\n", __FILE__, __LINE__);
		exit(1);
	}

	int i = 0;

	// initialize children
	for(i = 0; i < ALPHALEN; ++i)
		next->children[i] = NULL;
	
	next->isDictWord = 0;
	next->superwords = 0;
	next->matches = 0;
	next->sl = SLCreate(compareOccurrences, destroyOccurrences);

	return next;
}


// creates the dictionary Trie
void initialize() { base.root = nodeFactory(); }

// called by the destroy() method
void
destroyHelper(Node* n)
{
	if (n == NULL)
		return;

	int i;
	for(i = 0; i < ALPHALEN; ++i) {

		if (n->children[i] != NULL) {
	
			destroyHelper(n->children[i]);
			SLDestroy(n->children[i]->sl);
			free(n->children[i]);
		}
	}
	return;
}
void
destroy()
{
	destroyHelper(base.root);
	free(base.root);
}



// maps a char to its lexographical position in an alphanumeric alphabet
int
ascii2index(char c)
{
	if (c >= 'a' && c <= 'z')
		return (int)c - 'a';
	else if (c >= 'A' && c <= 'Z')
		return (int)c - 'A';
	else if (c >= '0' && c <= '9')
		return (int)c - '0' + 26;

	return -1;
}

char
index2ascii(int index)
{
	if (index <= 25)
		return 'a' + (char)index;

	return index + '0' - 26;
}


int
TrieInsert(FILE *dict_file, Node *current)
{
	// get the next char in the file
	char c = fgetc(dict_file);

	// if we've reached the end if the file,
	// we return false to indicate that we should
	// break out of the while look in readDict()
	if (c == EOF)
		return 0;

	// attempt to convert c into an array index
	int index = ascii2index(c);

	// if i is negative 1, then c isn't a letter.
	// we return i to indicate that we should
	// keep reading fro mthe file
	if ( index < 0 )
		return -1;

	if (current->children[index] == NULL) 
		current->children[index] = nodeFactory(c);

	int leaf = TrieInsert(dict_file, current->children[index]);
	if ( leaf == -1 ) {
		current->children[index]->isDictWord = 1;
		leaf = 1;
	}

	return leaf;
}

void
readDict(FILE *dict_file, char* filename)
{
	base.file = filename;
	while ( TrieInsert(dict_file, base.root) );

	rewind(dict_file);
}

void
addWord(char* word)
{
	/// return if word is null, ends in a null terminator, or trie is empty
	if (word == NULL || *word == '\0' || base.root->children == NULL)
		return;

	// create a word iterator
	char *iter = word;
	Node *current = base.root;

	// if current is null, then the word isn't in the tree in the first place
	if (current == NULL)
		return;

	// iterate through the word until we reach the last character
	int i = 0;
	while (iter[i] != '\0' && current->children[ascii2index(iter[i])] != NULL) {

		current->superwords++;
		current = current->children[ascii2index(iter[i])];
		i++;
	}
	if (iter[i] == '\0')
		current->matches++;
	else
		current->superwords++;
}
void
scanData(FILE *datHandle)
{
	if (datHandle == NULL)
		return;

	// create a char stack to hold the letters
	St4x *s = STXSpawn();

	// enter indefinite loop
	while (1) {

		// clear preceeding whitespace and nonletter chars
		char c = fgetc(datHandle);
		while (c != EOF && ascii2index(c) < 0)
			c = fgetc(datHandle);

		// if we've reached the end of the file, break out of the loop
		if (c == EOF)
			break;

		// while letters are alphanumeric and not EOF, push them onto the stack
		while (ascii2index(c) >= 0 && c != EOF) {

			STXPush(c, s);
			c = fgetc(datHandle);
		}
		// add the word to the trie
		int len = STXLen(s);
		char* word = (char*)malloc(sizeof(char) * len + 1);
		if (word == NULL) {
			fprintf(stderr, "In %s on line %d:\n", __FILE__, __LINE__);
			perror("Malloc faled.\n");
			exit(1);
		}
		word[len] = '\0';
		int i;
		for (i = len-1; i >= 0; --i) {

			word[i] = STXPop(s);
		}
		addWord(word);

		// free the word
		free(word);
	}
		
	// destroy the stack	
	STXDestroy(s);

	return;
}

// traverses through trie using tail recursion to efficiently reset
// the nodes for next file
int
resetDataHelper(Node *current, St4x *s, char c, int prefixes)
{

	if (current != NULL) {

		STXPush(c, s);

		current->matches = 0;
		current->superwords = 0;

		int i;
		for(i = 0; i < ALPHALEN; ++i) {
		
			resetDataHelper(current->children[i], s, index2ascii(i),
									 current->matches + prefixes);
		}
		STXPop(s);
	}
	return 0;
}

// resets the trie for use with next file by calling resetDataHelper on each
// child of the root node
void
resetData()
{
	
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	int i;
	for(i = 0; i < ALPHALEN; ++i) {
	
		resetDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}

// efficiently traverses the trie using tail recursion to store the indexing data
// at every node
int
storeDataHelper(Node *current, St4x *s, char c, int prefixes)
{
	if (current != NULL) {

		STXPush(c, s);

		if (current->isDictWord == 1 && current->matches > 0) {
			Occurrences *oc = OccurrencesFactory(base.file, current->matches);
			//printf("Storing { %s : %d }\n", base.file, current->matches);
			SLInsert(current->sl, oc);
		}

		int i;	
		for(i = 0; i < ALPHALEN; ++i) {
		
			storeDataHelper(current->children[i], s, index2ascii(i),
									 current->matches + prefixes);
		}
		STXPop(s);
	}
	return 0;
}

void
storeData()
{
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	int i;
	for(i = 0; i < ALPHALEN; ++i) {
	
		storeDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}

// uses tail recursion to efficiently print the entire trie to the file
int
printAllDataHelper(Node *current, St4x *s, char c, int prefixes, FILE *f)
{
	if (current != NULL) {

		STXPush(c, s);

		if (current->isDictWord == 1) {

			SortedListIteratorPtr iter = SLCreateIterator(current->sl);

			int len = STXLen(s);
			char* word = (char*)malloc(sizeof(char) * len + 1);
			if (word == NULL) {
				fprintf(stderr, "In %s on line %d:\n", __FILE__, __LINE__);
				perror("Malloc faled.\n");
				exit(1);
			}
			word[len] = '\0';
			int i;
			for (i = len-1; i >= 0; --i) {

				word[i] = STXPop(s);
			}
			i = 0;
			while (word[i] != '\0')
				STXPush(word[i++], s);
			fprintf(f, "<list> %s\n", word);
			//printf("<list> %s\n", word);
			free(word);
			
			Occurrences *oc = (Occurrences*)SLGetItem(iter);
			fprintf(f, "%s %d ", oc->file, oc->count);
			//printf("%s %d", oc->file, oc->count);
			int m = 1;
			while ( (oc = SLNextItem(iter)) ) {
				if (m % 5 == 0) {
					fprintf(f,"\n");
					//printf("\n");
				}
				fprintf(f, "%s %d ", oc->file, oc->count);
				//printf("%s %d", oc->file, oc->count);
			}
			fprintf(f, "\n</list>\n");
			//printf("\n</list>");
		}

		int i;
		for(i = 0; i < ALPHALEN; ++i) {
		
			printAllDataHelper(current->children[i], s, index2ascii(i),
									 current->matches + prefixes, f);
		}
		STXPop(s);
	}
	return 0;
}
void
printAllData(char* outputFile)
{
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	FILE *oHandle = fopen(outputFile,"w");
	if (oHandle == NULL) {
		perror("Unable to open the output file.\n");
		exit(1);
	}
	int i;
	for(i = 0; i < ALPHALEN; ++i) {
	
		printAllDataHelper(base.root->children[i], s, index2ascii(i),0,oHandle);
	}
	STXDestroy(s);
}
