#include <stdio.h>


unsigned char buffer[65536];


int main(int argc, char *argv[])
{
	FILE *lb;
	FILE *ipl;
	FILE *out;
	int i;
	int blocks, size;

	if(argc<4) {
		printf("%s <docipl> <linuxbios.strip> <target> [<size in K 64,128,256,512>]\n",argv[0]);
		printf("Where docipl is the output from ipl.S\n");
		printf("linuxbios.strip is a linuxbios stripped but not run through mkrom\n");
		printf("target is the result. It should be flashed to the rom.\n");
		return(-1);
	}

	if(!(ipl = fopen(argv[1],"rb"))) {
		fprintf(stderr,"Cannot open %s for reading\n",argv[1]);
		return(-2);
	}

	if(!(lb = fopen(argv[2],"rb"))) {
		fprintf(stderr,"Cannot open %s for reading\n",argv[2]);
		return(-2);
	}

	if(!(out = fopen(argv[3],"wb"))) {
		fprintf(stderr,"Cannot open %s for write\n",argv[3]);
		return(-3);
	}

	if(argc == 5)
		size = atoi(argv[4]);
	else
		size = 256;

	blocks = ((size + 64)/64)-1;

	blocks -=1;	// We always write the payload block!

	for(i=0; i<blocks; i++)
		fwrite(buffer,1,sizeof(buffer), out);

	fseek(ipl,-512, SEEK_END);

	i = fread(buffer,1,sizeof(buffer),lb);
	i = fread(buffer + ( sizeof(buffer) - 512),1, 512, ipl);

	
	fwrite(buffer,1,sizeof(buffer), out);

	return(0);
}

