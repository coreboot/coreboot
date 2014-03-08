#include <stdio.h>
main(int argc, char *argv[])
{
	int i, j;
	printf("\t.long 1\n\t.align 4096\npml0:");
	for(i = 0; i < 1024; i += 8){
		printf("\t.long ");
		for(j = 0; j < 8; j++){
			if (!i && !j)
				printf("0x%08x,", 0);
			else
				printf("0x%08x%s", ((i+j)<<12) + 0x5, j < 7 ? ",":"");
		}
		printf("\n");
	}
	printf("pml1:\n");
	for(i = 0; i < 1024; i += 8){
		printf("\t.long ");
		for(j = 0; j < 8; j++)
			if (!i && !j)
				printf("pml0+5,");
			else
				printf("0x%08x%s", ((i+j)<<22) + 0x85,j < 7 ? ",":"");
		printf("\n");
	}
}
