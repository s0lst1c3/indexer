#include <stdio.h>

int ascii2index(char c) {

	if (c >= 'a' && c <= 'z') {

		return (int)c - 'a';
	}
	if (c >= 'A' && c <= 'Z') {

		return (int)c - 'A';
	}
	if (c >= '0' && c <= '9') {

		return (int)c - '0' + 26;
	}

	return -1;
}

char index2ascii(int index) {

	if (index <= 25) {
		return 'a' + (char)index;
	}
	return index + '0' - 26;
}


int main() {

	char arr[36] = "abcdefghijklmnopqrstuvwxyz0123456789";
	
	int i = 0;
	while ( i < 36 ) {

		printf("%c", index2ascii(i));
		printf("%d", ascii2index(arr[i]));
		printf("\n");
		i++;
	}

}
