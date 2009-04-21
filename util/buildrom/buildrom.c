#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/* this is the beginning of a tool which will eventually 
 * be able to build rom images with both fallback and 
 * normal. For now it just builds a single image
 * into a rom iamge
 */
/* one switch we already need: -zero allowing you to tell what 
 * to do with numbers that are "zero": make them 0xff or whatever
 * for flash
 * For now we assume "zero" is 0xff
 */

void usage()
{
	fprintf(stderr, "Usage: buildrom <input> <output> <payload> ");
	fprintf(stderr, " <coreboot-size> <total-size>\n");
	exit(1);
}

void __attribute__((noreturn)) fatal(char *s)
{
	perror(s);
	exit(2);
}

int main(int argc, char *argv[])
{
	int infd, payloadfd, outfd, size, readlen, writelen, i;
	int romsize;
	unsigned char *cp;
	struct stat inbuf, payloadbuf;
	char zero = 0xff;

	if (argc != 6)
		usage();

	infd = open(argv[1], O_RDONLY);
	if (infd < 0)
		fatal(argv[1]);
	outfd = open(argv[2], O_RDWR | O_CREAT, 0666);
	if (outfd < 0)
		fatal(argv[2]);
	payloadfd = open(argv[3], O_RDONLY);
	if (payloadfd < 0)
		fatal(argv[3]);

	size = strtol(argv[4], 0, 0);
	romsize = strtol(argv[5], 0, 0);

	if (fstat(infd, &inbuf) < 0)
		fatal("stat of infile");
	if (inbuf.st_size > size) {
		fprintf(stderr, "coreboot image is %d bytes; only %d allowed\n",
			(int)inbuf.st_size, size);
		fatal("Coreboot input file larger than allowed size!\n");
	}

	if (fstat(payloadfd, &payloadbuf) < 0)
		fatal("stat of infile");
	if (payloadbuf.st_size > (romsize - size)){
		fprintf(stderr, "ERROR: payload (%d) + coreboot (%d) - Size is %d bytes larger than ROM size (%d).\n", 
				(int)payloadbuf.st_size, size, 
				(int)(payloadbuf.st_size+size-romsize),
				romsize);
		exit(1);
	}

	printf("Payload: %d coreboot: %d ROM size: %d Left space: %d\n",
			(int)payloadbuf.st_size, size, romsize,
			(int)(romsize-payloadbuf.st_size-size));

	cp = malloc(romsize);
	if (!cp)
		fatal("malloc buffer");
	for (i = 0; i < romsize; i++) {
		cp[i] = zero;
	}

	/* read the input file in at the END of the array */
	readlen = read(infd, &cp[romsize - inbuf.st_size], inbuf.st_size);
	if (readlen < inbuf.st_size) {
		fprintf(stderr, "Wanted %d, got %d\n", (int)inbuf.st_size, readlen);
		fatal("Read input file");
	}

	/* read the payload file in at the START of the array */
	readlen = read(payloadfd, cp, payloadbuf.st_size);
	if (readlen < payloadbuf.st_size) {
		fprintf(stderr, "Wanted %d, got %d\n",
			(int)payloadbuf.st_size, readlen);
		fatal("Read payload file");
	}
	writelen = write(outfd, cp, romsize);
	if (writelen < size) {
		fprintf(stderr, "Wanted %d, got %d\n", size, writelen);
		fatal("Write output file");
	}

	return 0;
}
