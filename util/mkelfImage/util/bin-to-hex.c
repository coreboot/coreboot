#include <stdio.h>

int main(int argc, char **argv)
{
	int c;
	int i;
	i = 0;
	while((c = getchar()) != EOF) {
		if ((i % 16) != 0) {
			putchar(' ');
		}
		printf("0x%02x,", c);
		i++;
		if ((i %16) == 0) {
			putchar('\n');
		}
	}
	putchar('\n');
	return 0;
}
