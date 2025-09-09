/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>

static const char *optstring  = "i:o:h";

static struct option long_options[] = {
	{"input",              required_argument, 0, 'i' },
	{"output",             required_argument, 0, 'o' },
	{"help",                     no_argument, 0, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void usage(void)
{
	printf("rbutool: Create RBU image for Gigabyte BMC BIOS updates\n");
	printf("Usage: rbutool -i coreboot.rom -o output.rom\n");
	printf("-i | --input  <FILE>             The input coreboot ROM file\n");
	printf("-o | --output <FILE>             The output RBU ROM file\n");
	printf("-h | --help                      Print this help\n");
}

int main(int argc, char **argv)
{
	int c;
	int ret = 1;
	char *inputfilename = NULL;
	char *outputfilename = NULL;
	uint8_t b;
	uint16_t checksum = 0;

	while (1) {
		int optindex = 0;

		c = getopt_long(argc, argv, optstring, long_options, &optindex);

		if (c == -1)
			break;

		switch (c) {
		case 'i':
			inputfilename = strdup(optarg);
			break;
		case 'o':
			outputfilename = strdup(optarg);
			break;
		case 'h':
			ret = 0; /* fallthrough */
		case '?':
			usage();
			goto out;
		default:
			break;
		}
	}

	if (!inputfilename) {
		fprintf(stderr, "E: Must specify input ROM filename\n");
		goto out;
	}

	if (!outputfilename) {
		fprintf(stderr, "E: Must specify output ROM filename\n");
		goto out;
	}

	FILE *fdin = fopen(inputfilename, "r");
	if (!fdin) {
		fprintf(stderr, "E: %s open failed: %s\n", inputfilename, strerror(errno));
		goto out;
	}

	FILE *fdout = fopen(outputfilename, "w");
	if (!fdout) {
		fprintf(stderr, "E: %s open failed: %s\n", outputfilename, strerror(errno));
		fclose(fdin);
		goto out;
	}

	// Copy input to output and calculate checksum
	while (fread(&b, 1, 1, fdin) == 1) {
		checksum += b;
		if (fwrite(&b, 1, 1, fdout) != 1) {
			fprintf(stderr, "E: write to %s failed: %s\n", outputfilename, strerror(errno));
			fclose(fdin);
			fclose(fdout);
			goto out;
		}
	}

	// Write magic string
	fputs("#GBT#ROM", fdout);
	// Write checksum in BE
	fputc(checksum >> 8, fdout);
	fputc(checksum & 0xff, fdout);
	// Pad with zeros to 16 bytes
	fputc(0, fdout);
	fputc(0, fdout);
	fputc(0, fdout);
	fputc(0, fdout);
	fputc(0, fdout);
	fputc(0, fdout);

	if (fclose(fdin)) {
		fprintf(stderr, "E: %s close failed: %s\n", inputfilename, strerror(errno));
		fclose(fdout);
		goto out;
	}

	if (fclose(fdout)) {
		fprintf(stderr, "E: %s close failed: %s\n", outputfilename, strerror(errno));
		goto out;
	}

	ret = 0;
out:
	if (ret > 0)
		fprintf(stderr, "E: Error creating '%s'\n", outputfilename);

	if (inputfilename)
		free(inputfilename);

	if (outputfilename)
		free(outputfilename);

	exit(ret);

	return 0;
}
