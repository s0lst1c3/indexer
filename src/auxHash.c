struct token {
	char* id;
	char* file;
	int count;
	UT_hash_handle hh;
};
typedef struct token token;

void addToken(int user_id
