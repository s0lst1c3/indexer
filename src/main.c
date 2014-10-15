#include "main.h"

int
main(int argc, char **argv)
{
	Config *config = setup(argc, argv);

	direntL0rd(config);

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

	int flag_fileonly = 0;
	struct stat statbuf;

	// we will not even consider reading from the output file. 
	int cmp = strcmp(outputFile, inputFile);
	if (cmp == 0) {
		// mildly scold user
		perror("Refusing to read from output file. Try again\n");
		exit(1);
	}

	// run stat on the input file
	if ( stat(inputFile, &statbuf) == -1) {

		if (ENOENT == errno) {
			fprintf(stderr, "Error: %s does not exist\n", inputFile);
			
		}
		fprintf(stderr, "Error: Unable to stat %s\n", inputFile);
		exit(1);
	}

	// regular files and directories are ok. anything else is not
	if ( S_ISREG(statbuf.st_mode) ) {
		flag_fileonly = 1;
	}
	else if ( S_ISDIR(statbuf.st_mode) ) {
		flag_fileonly = 0;
	}
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

	// allocate space for config
	Config *config = (Config*)malloc(sizeof(Config));
	if (config == NULL) {
		fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
		exit(1);
	}

	// we append ./ to the beginning of our path
	char* basedir = (char*)malloc(sizeof(char) * strlen(inputFile) + 3);	
	if (basedir == NULL) {
		fprintf(stderr, "Malloc failed: %s on line %d", __FILE__, __LINE__-2);
		exit(1);
	}
	basedir[0] = '.';
	basedir[1] = '/';
	int i = 0;
	while ( *(inputFile + i) != '\0' ) {

		basedir[i+2] = *(inputFile + i);
		i++;
	}
	basedir[i+2] = '\0';

	// set configs
	config->basedir = basedir;
	config->outputFile = strdup(outputFile);
	config->flag_errmsg = 0;
	config->flag_fileonly = flag_fileonly;

	// initialize the tree
	initialize();

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
	if (path == NULL) {
		perror("oh no\n");
		exit(1);
	}
	PSPush(config->basedir, path);
	if (config->flag_fileonly) {

		fileJockie(path, config);
	}
	else {

		dirGrinder(path, config);
	}

	PSPop(path);
	PSDestroy(path);
	return;
}

void fileJockie(PathStack *path, Config *config) {
	char *name = PSGet(path);
	FILE* inFile = fopen(name, "r");
	if (inFile == NULL) {
		perror("Oh well that sucks\n");
		exit(1);
	}
	readDict(inFile, name);
	scanData(inFile);
	storeData();

	fclose(inFile);
	printf("%s\n", name);	
}

void
endOfJob(Config *config)
{
	printAllData();
	destroy();
	//void printOutput(config);
	if (config->flag_errmsg) {

		perror("Errors occurred during the program execution\n");
		perror("Please check error_log for details\n");
	}
	//HTDestroy(config->index);
	free(config->outputFile);
	free(config->basedir);
	free(config);
	

	return;
}

void
//dirGrinder(PathStack* path, Config *config)
dirGrinder(PathStack* path, Config *config)
{
	// declare locals
	char *dirname;
	DIR *dir;
	struct dirent *entry;
	struct stat statbuf;
	int flag_isdir = 0;
	char *name;
	
	// sanity check for troll args
	if (path == NULL) {

		return;
	}

	// get the directory name
	dirname = PSGet(path);
	printf("dirname is: %s", dirname);
	//  open the directory
	dir = opendir(dirname);
	if (dir == NULL)
		return;

	// while there are still entries left in the directory . . . 
	while (1) {

		entry = readdir(dir);
		if (entry == NULL)
			break;

		// ignore . and ..
		if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
			continue;
		}

		PSPush(entry->d_name, path);
		name = PSGet(path);
		
		
		// run stat on the entry to get file type
		if ( stat(name, &statbuf) == -1) {
	
			fprintf(stderr, "Error: Unable to stat %s\n", name);
			// TODO - log file should output here
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		// regular files and directories are ok. anything else is not
		if ( S_ISREG(statbuf.st_mode) ) {

			flag_isdir = 0;
		}
		else if(S_ISDIR(statbuf.st_mode)) {

			flag_isdir = 1;
		}
		else {

			fprintf(stderr, "File invalid: %s\n", name);
			// TODO - log file should output here
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}
	
		// check for valid permissions
		if ( flag_isdir && access(name, F_OK|X_OK|R_OK) == -1 ) {
			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			// TODO - log file should output here
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}
		if ( !flag_isdir && access(name, F_OK|R_OK) == -1 ) {
			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			// TODO - log file should output here
			config->flag_errmsg = 1;
			PSPop(path);
			continue;
		}

		if (flag_isdir) {
			
			// call dirGrinder if entry is a directory
			dirGrinder(path, config);
		}
		else {
		
			fileJockie(path, config);
		}
		// remove entry from path
		PSPop(path);
	}

}
