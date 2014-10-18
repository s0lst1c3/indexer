#include "uthash/src/uthash.h"
struct Token {

	char *name;
	char *file;
	int count;

	UT_hash_handle hh;
};
typedef struct Token Token;

InitializeToken(
