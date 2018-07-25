/*
 * cbfs-compression-tool, CLI utility for dealing with CBFS compressed data
 *
 * Copyright (C) 2017 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include "common.h"

const char *usage_text = "cbfs-compression-tool benchmark\n"
	"  runs benchmarks for all implemented algorithms\n"
	"cbfs-compression-tool compress inFile outFile algo\n"
	"  compresses inFile with algo and stores in outFile\n"
	"\n"
	"'compress' file format:\n"
	" 4 bytes little endian: algorithm ID (as used in CBFS)\n"
	" 4 bytes little endian: uncompressed size\n"
	" ...: compressed data stream\n";

static void usage(void)
{
	puts(usage_text);
}

static int benchmark(void)
{
	const int bufsize = 10*1024*1024;
	char *data = malloc(bufsize);
	if (!data) {
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	char *compressed_data = malloc(bufsize);
	if (!compressed_data) {
		free(data);
		fprintf(stderr, "out of memory\n");
		return 1;
	}
	int i, l = strlen(usage_text) + 1;
	for (i = 0; i + l < bufsize; i += l) {
		memcpy(data + i, usage_text, l);
	}
	memset(data + i, 0, bufsize - i);
	const struct typedesc_t *algo;
	for (algo = &types_cbfs_compression[0]; algo->name != NULL; algo++) {
		int outsize = bufsize;
		printf("measuring '%s'\n", algo->name);
		comp_func_ptr comp = compression_function(algo->type);
		if (comp == NULL) {
			printf("no handler associated with algorithm\n");
			free(data);
			free(compressed_data);
			return 1;
		}

		struct timespec t_s, t_e;
		clock_gettime(CLOCK_MONOTONIC, &t_s);

		if (comp(data, bufsize, compressed_data, &outsize)) {
			printf("compression failed");
			return 1;
		}

		clock_gettime(CLOCK_MONOTONIC, &t_e);
		printf("compressing %d bytes to %d took %ld seconds\n",
			bufsize, outsize,
			(long)(t_e.tv_sec - t_s.tv_sec));
	}
	free(data);
	free(compressed_data);
	return 0;
}

static int compress(char *infile, char *outfile, char *algoname,
		    int write_header)
{
	int err = 1;
	FILE *fin = NULL;
	FILE *fout = NULL;
	void *indata = NULL;

	const struct typedesc_t *algo = &types_cbfs_compression[0];
	while (algo->name != NULL) {
		if (strcasecmp(algo->name, algoname) == 0) break;
		algo++;
	}
	if (algo->name == NULL) {
		fprintf(stderr, "algo '%s' is not supported.\n", algoname);
		return 1;
	}

	comp_func_ptr comp = compression_function(algo->type);
	if (comp == NULL) {
		printf("no handler associated with algorithm\n");
		return 1;
	}

	fin = fopen(infile, "rb");
	if (!fin) {
		fprintf(stderr, "could not open '%s'\n", infile);
		return 1;
	}
	fout = fopen(outfile, "wb");
	if (!fout) {
		fprintf(stderr, "could not open '%s' for writing\n", outfile);
		goto out;
	}

	if (fseek(fin, 0, SEEK_END) != 0) {
		fprintf(stderr, "could not seek in input\n");
		goto out;
	}
	long insize = ftell(fin);
	if (insize < 0) {
		fprintf(stderr, "could not determine input size\n");
		goto out;
	}
	rewind(fin);

	indata = malloc(insize);
	if (!indata) {
		fprintf(stderr, "out of memory\n");
		goto out;
	}

	void *outdata = malloc(insize);
	if (!outdata) {
		fprintf(stderr, "out of memory\n");
		goto out;
	}
	int outsize;

	int remsize = insize;
	while (remsize > 0) {
		int readsz = fread(indata, 1, remsize, fin);
		if (readsz < 0) {
			fprintf(stderr, "failed to read input with %d bytes left\n", remsize);
			goto out;
		}
		remsize -= readsz;
	}

	if (comp(indata, insize, outdata, &outsize) == -1) {
		outsize = insize;
		free(outdata);
		outdata = indata;
		algo = &types_cbfs_compression[0];
	}

	if (write_header) {
		char header[8];
		header[0] = algo->type & 0xff;
		header[1] = (algo->type >> 8) & 0xff;
		header[2] = (algo->type >> 16) & 0xff;
		header[3] = (algo->type >> 24) & 0xff;
		header[4] = insize & 0xff;
		header[5] = (insize >> 8) & 0xff;
		header[6] = (insize >> 16) & 0xff;
		header[7] = (insize >> 24) & 0xff;
		if (fwrite(header, 8, 1, fout) != 1) {
			fprintf(stderr, "failed writing header\n");
			goto out;
		}
	}
	if (fwrite(outdata, outsize, 1, fout) != 1) {
		fprintf(stderr, "failed writing compressed data\n");
		goto out;
	}

	err = 0;
out:
	if (fin) fclose(fin);
	if (fout) fclose(fout);
	if (indata) free(indata);
	return err;
}

int main(int argc, char **argv)
{
	if ((argc == 2) && (strcmp(argv[1], "benchmark") == 0))
		return benchmark();
	if ((argc == 5) && (strcmp(argv[1], "compress") == 0))
		return compress(argv[2], argv[3], argv[4], 1);
	if ((argc == 5) && (strcmp(argv[1], "rawcompress") == 0))
		return compress(argv[2], argv[3], argv[4], 0);
	usage();
	return 1;
}
