/*
 * bin2hex - binary to hexadecimal converter
 */

#include <stdio.h>

/* 8 values per line */
#define SPLIT 8

int main(int argc, char **argv)
{
	int c, i;
	
	i = 0;
	while ((c = getchar()) != EOF) {
		if ((i % SPLIT) != 0) {
			putchar(' ');
		}
		printf("0x%02x,", c);
		i++;
		if ((i % SPLIT) == 0) {
			putchar('\n');
		}
	}
	putchar('\n');
	return 0;
}

// tag: bin2hex 
