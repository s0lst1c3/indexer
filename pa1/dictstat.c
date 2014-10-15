#include "dictstat.h"

#ifndef ALPHALEN
#define ALPHALEN 26
#endif

TrieT base;

struct Occurrences {
	char *file;
	int count;
};
typedef struct Occurrences Occurrences;

void TRIEdestroyOccurrences(void *p) {
	
	Occurrences *oc = (Occurrences*)p;
	free(oc->file);
	free(oc);
}

Occurrences *OccurrencesFactory(char *file, int count) {
	
	Occurrences *oc = (Occurrences*)malloc(sizeof(Occurrences));
	int len = strlen(file);
	int i = 0;
	oc->file = (char*)malloc(sizeof(char) * len + 1);
	while ( *(file + i) != '\0' ) {
		oc->file[i] = file[i];
		i++;
	}
	
	
	oc->count = count;
	return oc;
}
int TRIEcompareOccurrences(void *p1, void *p2) {

	Occurrences *o1 = p1;
	Occurrences *o2 = p2;
	return o1->count - o2->count;
}

Node* nodeFactory() {

	// instantiate new Node
	Node* next = (Node*)malloc(sizeof(Node));
	if (next == NULL) {

		fprintf(stderr, "in %s on line %d\n", __FILE__, __LINE__);
		exit(1);
	}

	// initialize children
	for(int i = 0; i < ALPHALEN; ++i) {

		next->children[i] = NULL;
	}
	
	next->isDictWord = 0;
	next->superwords = 0;
	next->matches = 0;
	next->sl = SLCreate(TRIEcompareOccurrences, TRIEdestroyOccurrences);

	return next;
}


void initialize() {
	
	base.root = nodeFactory();
}

void destroyHelper(Node* n) {

	if (n == NULL) {

		return;
	}	
	for(int i = 0; i < ALPHALEN; ++i) {

		if (n->children[i] != NULL) {
	
			destroyHelper(n->children[i]);
			SLDestroy(n->children[i]->sl);
			free(n->children[i]);
		}
	}
	return;
}
void destroy() {
	
	destroyHelper(base.root);
	free(base.root);
}



int ascii2index(char c) {

	if (c >= 'a' && c <= 'z') {

		return (int)c - 'a';
	}
	if (c >= 'A' && c <= 'Z') {

		return (int)c - 'A';
	}

	return -1;
}

char index2ascii(int index) {

	return 'a' + (char)index;
}


int TrieInsert(FILE *dict_file, Node *current) {

	// get the next char in the file
	char c = fgetc(dict_file);

	// if we've reached the end if the file,
	// we return false to indicate that we should
	// break out of the while look in readDict()
	if (c == EOF) {
		
		return 0;
	}

	// attempt to convert c into an array index
	int index = ascii2index(c);

	// if i is negative 1, then c isn't a letter.
	// we return i to indicate that we should
	// keep reading fro mthe file
	if ( index < 0 ) {

		return -1;
	}

	if (current->children[index] == NULL) {

		current->children[index] = nodeFactory(c);
	}
	int leaf = TrieInsert(dict_file, current->children[index]);
	if ( leaf == -1 ) {
		current->children[index]->isDictWord = 1;
		leaf = 1;
	}
	return leaf;
}

void readDict(FILE *dict_file, char* filename) {
	
	base.file = filename;
	while ( TrieInsert(dict_file, base.root) );
}

void addWord(char* word) {


	if (word == NULL || *word == '\0' || base.root->children == NULL)
		return;

	char *iter = word;
	Node *current = base.root;

	// if current is null, then the word isn't in the tree in the first place
	if (current == NULL) {

		return;
	}

	int i = 0;
	while (iter[i] != '\0' && current->children[ascii2index(iter[i])] != NULL) {

		current->superwords++;
		current = current->children[ascii2index(iter[i])];
		i++;
	}
	if (iter[i] == '\0') {

				
		current->matches++;
	}
	else {
		current->superwords++;
	}
	/*
	if (iter[i] == '\0') {

				
		current->matches++;
		current->superwords--;
	}
	else {
		current->superwords++;
		current->superwords++;
	}
	*/
}
void scanData(FILE *datHandle) {

	if (datHandle == NULL)
		return;

	St4x *s = STXSpawn();

	while (1) {

		// clear preceeding whitespace and nonletter chars
		char c = fgetc(datHandle);
		while (c != EOF && ascii2index(c) < 0) {
			c = fgetc(datHandle);
		}
		if (c == EOF) {
			break;
		}
		while (ascii2index(c) >= 0 && c != EOF) {

			STXPush(c, s);
			c = fgetc(datHandle);
		}
		int len = STXLen(s);
		char* word = (char*)malloc(sizeof(char) * len + 1);
		if (word == NULL) {
			fprintf(stderr, "In %s on line %d:\n", __FILE__, __LINE__);
			perror("Malloc faled.\n");
			exit(1);
		}

		word[len] = '\0';
		for (int i = len-1; i >= 0; --i) {

			word[i] = STXPop(s);
		}
	
		addWord(word);


		free(word);
	}
	
	STXDestroy(s);

}
int resetDataHelper(Node *current, St4x *s, char c, int prefixes) {

	if (current == NULL) {

		return 0;
	}
	STXPush(c, s);

	current->matches = 0;
	current->superwords = 0;

	for(int i = 0; i < ALPHALEN; ++i) {
	
		resetDataHelper(current->children[i], s, index2ascii(i),
								 current->matches + prefixes);
	}
	STXPop(s);
	return 0;
}

void resetData() {
	
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	for(int i = 0; i < ALPHALEN; ++i) {
	
		resetDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}
//------------------------------------------------------------------
int storeDataHelper(Node *current, St4x *s, char c, int prefixes) {

	if (current == NULL) {

		return 0;
	}
	STXPush(c, s);

	if (current->isDictWord == 1) {
		Occurrences *oc = OccurrencesFactory(base.file, current->matches);
		SLInsert(current->sl, oc);
		/*
		STXPrint(s);
		printf(" %d", current->matches);
		printf(" %d", prefixes);
		printf(" %d", current->superwords);
		printf("\n");
		*/
	}

	for(int i = 0; i < ALPHALEN; ++i) {
	
		storeDataHelper(current->children[i], s, index2ascii(i),
								 current->matches + prefixes);
	}
	STXPop(s);
	return 0;
}

void storeData() {
	
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	for(int i = 0; i < ALPHALEN; ++i) {
	
		storeDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}

//------------------------------------------------------------------
int printAllDataHelper(Node *current, St4x *s, char c, int prefixes) {

	if (current == NULL) {

		return 0;
	}
	STXPush(c, s);

	if (current->isDictWord == 1) {

		SortedListIteratorPtr iter = SLCreateIterator(current->sl);
		printf("(\' ");
		STXPrint(s);
		printf(" \') =====> ");

		Occurrences *oc = (Occurrences*)SLGetItem(iter);
		printf("( \'%s : %d\' )-->", oc->file, oc->count);
		while ( (oc = SLNextItem(iter)) ) {
			printf("( \'%s : %d\' )-->", oc->file, oc->count);
		}
		printf("\n");
	}

	for(int i = 0; i < ALPHALEN; ++i) {
	
		printAllDataHelper(current->children[i], s, index2ascii(i),
								 current->matches + prefixes);
	}
	STXPop(s);
	return 0;
}

void printAllData() {
	
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	for(int i = 0; i < ALPHALEN; ++i) {
	
		printAllDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}
/*
int printDataHelper(Node *current, St4x *s, char c, int prefixes) {

	if (current == NULL) {

		return 0;
	}
	STXPush(c, s);
	if (current->isDictWord == 1) {

		STXPrint(s);
		printf(" %d", current->matches);
		printf(" %d", prefixes);
		printf(" %d", current->superwords);
		printf("\n");
	}

	for(int i = 0; i < ALPHALEN; ++i) {
	
		printDataHelper(current->children[i], s, index2ascii(i),
								 current->matches + prefixes);
	}
	STXPop(s);
	return 0;
}

void printData() {
	
	St4x *s = STXSpawn();
	if (s == NULL) {

		fprintf(stderr, "%s on line %d\n",__FILE__,__LINE__ - 3);
		return;
	}
	for(int i = 0; i < ALPHALEN; ++i) {
	
		printDataHelper(base.root->children[i], s, index2ascii(i), 0);
	}
	STXDestroy(s);
}
*/
