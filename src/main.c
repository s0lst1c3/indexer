#include "main.h"
#include "indexer.h"

#define UNCERTAINTY ' '

int
main(int argc, char **argv)
{
	// set things up
	Config *config = setup(argc, argv);
	if (config == NULL) {
		perror("Setup failed\n");
		exit(1);
	}

	// create a PathStack
	PathStack* path = PSCreate();
	if (path == NULL) {
		perror("oh no\n");
		exit(1);
	}
	// push the basedir to path
	PSPush(config->basedir, path);

	// call fileParser if we're only dealing with files,
	// dirTraverserHelper if we have a directory
	if (config->flag_fileonly)
		fileParser(path, config);
	else
		dirTraverserHelper(path, config);

	// remove the last entry from the path and destroy
	PSPop(path);
	PSDestroy(path);

	// print report, tear things down, and quit
	endOfJob(config);

	return 0;
}

Config *
setup(int argc, char **argv)
{
	char  *inputFile, *outputFile;

	// sanity check against arg count
	if (argc != 3) {
		perror("Usage: indexer <output file> <input file>");
		exit(1);
	}

	inputFile = prepareFilename(argv[2]);
	outputFile = prepareFilename(argv[1]);



	// allocate space for config
	Config *config = (Config*)malloc(sizeof(Config));
	if (config == NULL) {
		fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
		exit(1);
	}

	validateInputs(inputFile, outputFile, config);

	// set configs
	config->basedir = inputFile;
	config->outputFile = outputFile;
	config->flag_errmsg = 0;

	initialize();	
	return config;
}

void
dirTraverser(Config *config)
{

	// do not accept null pointers -- these are bad
	if (config == NULL) {
		fprintf(stderr,"Passed null ptr in %s on line %d\n",__FILE__,__LINE__);
		exit(1);
	}

	// create a PathStack
	PathStack* path = PSCreate();
	if (path == NULL) {
		perror("oh no\n");
		exit(1);
	}
	// push the basedir to path
	PSPush(config->basedir, path);

	// call fileParser if we're only dealing with files, dirTraverserHelper if we
	// have a directory
	if (config->flag_fileonly)
		fileParser(path, config);
	else
		dirTraverserHelper(path, config);

	// remove the last entry from the path and destroy
	PSPop(path);
	PSDestroy(path);
	return;
}

void
dirTraverserHelper(PathStack* path, Config *config)
{
	// declare locals
	char   *dirname;
	DIR    *dir;
	struct dirent *entry;
	struct stat statbuf;
	int    flag_isdir = 0;
	char   *name;
	
	// sanity check for troll args
	if (path == NULL) {

		return;
	}

	// get the directory name
	dirname = PSGet(path);

	//  open the directory
	dir = opendir(dirname);
	if (dir == NULL)
		return;

	// enter indefinite loop
	while (1) {
	
		// get the next entry from the directory	
		entry = readdir(dir);

		// if no entries left, then bail
		if (entry == NULL)
			break;

		// ignore . and ..
		if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			continue;

		// append direct name to path
		PSPush(entry->d_name, path);
		name = PSGet(path);
		
		
		// run stat on the entry to get file type
		if ( stat(name, &statbuf) == -1) {
	
			fprintf(stderr, "Error: Unable to stat %s\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// regular files and directories are ok. anything else is not
		if ( S_ISREG(statbuf.st_mode) )
			flag_isdir = 0;
		else if(S_ISDIR(statbuf.st_mode))
			flag_isdir = 1;
		else {

			fprintf(stderr, "File invalid: %s\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}
	
		// if dirent is a directory, make sure we have read and execute privs
		if ( flag_isdir && access(name, F_OK|X_OK|R_OK) == -1 ) {

			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// if dirent is regular file, make sure we have read privs
		if ( !flag_isdir && access(name, F_OK|R_OK) == -1 ) {

			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// call dirGrinder if directory, fileJockie if file
		if (flag_isdir)
			dirTraverserHelper(path, config);
		else
			fileParser(path, config);

		// remove entry from path
		PSPop(path);
	}
}

void
fileParser(PathStack *path, Config *config)
{

	// convert path to string
	char *name = PSGet(path);
	//printf("%s\n", name);

	// open the file	
	FILE* inFile = fopen(name, "r");
	if (inFile == NULL) {
		perror("Madness??\n");
		exit(1);
	}
	
	// forage fresh datums from file then close
	readDict(inFile, name);
	scanData(inFile);
	storeData();
	resetData();
	fclose(inFile);
}

void
endOfJob(Config *config)
{
	printAllData(config->outputFile);
	
	// freedumz	
	free(config->outputFile);
	free(config->basedir);
	free(config);
}


char*
prepareFilename(char* filename)
{
	int i;
	char *newFile = (char*)malloc(sizeof(char) * strlen(filename) + 3);
	if (newFile == NULL) {
		fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
		exit(1);
	}
	newFile[0] = '.';
	newFile[1] = '/';
	i = 0;
	while ( *(filename + i) != '\0' ) {

		newFile[i+2] = *(filename + i);
		i++;
	}
	newFile[i+2] = '\0';
	return newFile;
}

void
validateInputs(char *inputFile, char *outputFile, Config *config)
{
	struct stat statbuf;

	// we will not even consider reading from the output file
	int cmp = strcmp(outputFile, inputFile);
	if (cmp == 0) {
		// mildly scold user
		perror("Refusing to read from output file. Try again\n");
		exit(1);
	}

	// run stat on the input source
	if ( stat(inputFile, &statbuf) == -1) {

		if (ENOENT == errno) {

			fprintf(stderr, "Error: %s does not exist\n", inputFile);
		}
		fprintf(stderr, "Error: Unable to stat %s\n", inputFile);
		exit(1);
	}

	// regular files and directories are ok. anything else is not
	if ( S_ISREG(statbuf.st_mode) )
		config->flag_fileonly = 1;
	else if ( S_ISDIR(statbuf.st_mode) )
		config->flag_fileonly = 0;
	else {
		perror("That's an interesting file you've got there. Aborting\n");
		exit(1);
	}

	// make sure we have write access to working directory
	if ( access(".", F_OK|X_OK|R_OK|W_OK) == -1 ) {

		perror("Error: Need write access to working directory to proceed\n");
		exit(1);
	}
	// check to see if output file exists
	if ( access(outputFile, F_OK) != -1 ) {

		// if the output file exists, make sure user
		// acknowledges overwrite	
		char response;
		printf("It appears your output file already exists.\n"); 
		printf("Would you like to overwrite it?\n");
		while (1) {
			printf("Please enter y or n : ");
			response = getchar();
			fflush(stdin);
			printf("\n");	
			if (response == 'y' || response == 'Y') {

				break;
			}
			if (response == 'n' || response == 'N') {

				printf("Aborting.\n");
				exit(1);
			}
			printf("That's not a valid option.\n");
		}

		// make sure we have write permission to the output file
		if ( access(outputFile, F_OK|W_OK|R_OK) == -1 ) {
			perror("Bad permissions on output file. Aborting.\n");
			exit(1);
		}
	}
}
