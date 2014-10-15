#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "PathStack.h"

void
//dirGrinder(PathStack* path, Config *config)
dirGrinder(PathStack* path)
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
			PSPop(path);
			continue;
		}
	
		// check for valid permissions
		if ( flag_isdir && access(name, F_OK|X_OK|R_OK) == -1 ) {
			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			PSPop(path);
			continue;
		}
		if ( !flag_isdir && access(name, F_OK|R_OK) == -1 ) {
			fprintf(stderr, "Bad permissions on %s - skipping\n", name);
			PSPop(path);
			continue;
		}

		if (flag_isdir) {
			
			// call dirGrinder if entry is a directory
			dirGrinder(path);
			//dirGrinder(path, Config *config);
		}
		else {
		
			printf("%s\n", name);	
			// call dirGrinder if entry is a regular file
			//fileSlave(path, Config *config);
		}
		// remove entry from path
		PSPop(path);
	}

}

int
main(int argc, char **argv)
{
	char* basedir = argv[1];
	PathStack *path = PSCreate();
	PSPush(basedir, path);

	dirGrinder(path);
	
	PSDestroy(path);
	return 0;
}
