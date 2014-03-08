#include <stdio.h>
main(int argc, char *argv[])
{
	int i, j;
	for(i = 0; i < 1024; i += 8){
		printf("\t.long ");
		for(j = 0; j < 8; j++)
			printf("0x%08x,", ((i+j)<<22) + 0x85);
		printf("\n");
	}
}
