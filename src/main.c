#include "main.h"

int
main(int argc, char **argv)
{
	Config *config = setup(argc, argv);

	if (config->flag_fileonly) {
	
		direntL0rd(config);
	}
	else {
		// temp?
		fileJockie(config);
	}

	endOfJob(config);

	return 0;
}

Config *
setup(int argc, char **argv)
{
	// sanity check against arg count
	if (argc != 3) {
		perror("Usage: indexer <output file> <input file>");
		exit(1);
	}
	// VALIDATE USER INPUT
	char *outputFile = argv[1];
	char *inputFile = argv[2];

	// we will not even consider reading from the output file. 
	int cmp = strcmp(outputFile, inputFile);
	if (cmp == 0) {
		// mildly scold user
		perror("Refusing to read from output file. Try again\n");
		exit(1);
	}

	// TODO
	// make sure input dirent actually exists
	/*      some
	        code
  			here

    */
	if (file does not exist) {

		perror("Nonexistent input source.");
		exit(1);
	}

	/* check to see if we have write access to current directory
		and output file
		TODO
	*/
	if (no write access to output) {
		
		perror("Error: we do not have permission to write to output file\n");
		exit(1);
	}

	// check to see if output file already exists them,
	while (outFileExists) {

		printf("The file %s already exists!\n", outputFile);
		printf("Enter yes or no: ");
		// yes or no code here TODO
	}

	// allocate space for config
	Config *config = (Config*)malloc(sizeof(Config));
	if (config == NULL) {

		errHandler(errno, __FILE__, __LINE__);
	}

	// allocate space for hash table
	config->index = (DankHash*)malloc(sizeof(DankHash));
	if (config->index == NULL) {
		
		errHandler(errno, __FILE__, __LINE__);
	}

	// TODO -- a ./ should be appended to the start of all basedirs
	// . and .. should be considered invalid -- will resolve ff.1, ff.2, ff.3
	config->basedir = strdup(inputFile);
	config->outputFile = strdup(outputFile);
	config->flag_errmsg = 0;

	// TODO -- check if inputFile is file or directory	
	if (input file is directory) {
	
		config->flag_fileonly = 0;
	}
	else if (input file is regular file) {

		config->flag_fileonly = 1;
	}
	else {

		fprintf(stderr, "%s is not a valid file or directory", inputFile);
		exit(1);
	}

	return config;
}

void
direntL0rd(Config *config)
{

	if (config == NULL) {

		fprintf(stderr, "Passed a null argument in %s on line %d\n", __FILE__, __LINE__);
		exit(1);
	}
	PathStack* path = PSCreate();
	PSPush(config->basedir);
	// call to dirGrinder()
	// TODO dirGrinder()

	PSPop(path);
	PSDestroy(path);
	return;
}

void
endOfJob(Config *config)
{
	void printOutput(config);
	if (config->flag_errmsg) {

		perror("Errors occurred during the program execution\n");
		perror("Please check error_log for details\n");
	}
	HTDestroy(config->index);
	free(config->outputFile);
	free(config->basedir);

	return;
}

// TODO
void dirGrinder(PathStack *path) {
	
	int isDir = 0;

	// sanity check
	if (path == NULL) {

		return;
	}

	// get a path string
	char *pathstr = PSGet(path);
	struct stat stat_buf;
	if ( !stat(path, &stat_buf) ) {

		fprintf(stderr, "Stat failed in main.c on line %d\n", __LINE__);
		exit(1);
	}
	// check if file or directory
	
	
}

// TODO
void printOutput(Config* config);
