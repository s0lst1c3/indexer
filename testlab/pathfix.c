#include <stdio.h>
#include <stdlib.h>
#include "PathStack.h"

int main(int argc, char **argv) {

	PathStack *path = PSCreate();
	int i = 1;
	char *yes;
	while ( i < argc) {

		printf("pushing %s\n", argv[i]);
		yes = PSGet(path);		
		printf("%s\n", yes);
		PSPush(argv[i], path);
		i++;
	}
	yes = PSGet(path);		
	printf("%s\n", yes);
	i = 0;
	while ( i < argc) {

		printf("%s\n", yes);
		yes = PSGet(path);		
		PSPop(path);
		i++;
	}
	PSDestroy(path);
	return 0;
}
