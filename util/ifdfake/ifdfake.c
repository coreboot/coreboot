/*
 * ifdfake - Create an Intel Firmware Descriptor with just a section layout
 *
 * Copyright (C) 2013 secunet Security Networks AG
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

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define REGION_COUNT 5

#define FDBAR_OFFSET 0x10
#define FRBA_OFFSET 0x40

typedef struct {
	uint32_t base, limit, size;
} region_t;

static void write_image(const region_t regions[], const char *const image)
{
	FILE *const f = fopen(image, "w");
	if (!f) {
		perror("Could not open file");
		exit(EXIT_FAILURE);
	}

	if (fseek(f, 0x1000 - 1, SEEK_SET)) {
		perror("Failed to seek to end of descriptor");
		exit(EXIT_FAILURE);
	}
	char zero = '\0';
	if (fwrite(&zero, 1, 1, f) != 1) {
		fprintf(stderr, "Failed to write at end of descriptor.\n");
		exit(EXIT_FAILURE);
	}

	if (fseek(f, FDBAR_OFFSET, SEEK_SET)) {
		perror("Failed to seek to fdbar");
		exit(EXIT_FAILURE);
	}

	struct {
		uint32_t flvalsig;
		uint32_t flmap0;
	} fdbar;
	memset(&fdbar, 0x00, sizeof(fdbar));
	fdbar.flvalsig = 0x0ff0a55a;
	fdbar.flmap0 = (REGION_COUNT - 1) << 24 | (FRBA_OFFSET >> 4) << 16;
	if (fwrite(&fdbar, sizeof(fdbar), 1, f) != 1) {
		fprintf(stderr, "Failed to write fdbar.\n");
		exit(EXIT_FAILURE);
	}

	int i;
	uint32_t frba[REGION_COUNT];
	for (i = 0; i < REGION_COUNT; ++i) {
		if (regions[i].size)
			frba[i] = ((regions[i].limit & 0xfff000) << (16 - 12)) |
				  ((regions[i].base & 0xfff000) >> 12);
		else
			frba[i] = 0x00000fff;
	}

	if (fseek(f, FRBA_OFFSET, SEEK_SET)) {
		perror("Failed to seek to frba");
		exit(EXIT_FAILURE);
	}
	if (fwrite(frba, sizeof(frba), 1, f) != 1) {
		fprintf(stderr, "Failed to write frba.\n");
		exit(EXIT_FAILURE);
	}

	fclose(f);
}

static int parse_region(const char *_arg, region_t *const region)
{
	char *const start = strdup(_arg);
	int size_spec = 0;
	unsigned long first, second;
	if (!start) {
		fprintf(stderr, "Out of memory.\n");
		exit(EXIT_FAILURE);
	}

	char *colon = strchr(start, ':');
	if (!colon) {
		colon = strchr(start, '+');
		if (!colon) {
			free(start);
			return -1;
		}
		size_spec = 1;
	}
	*colon = '\0';

	char *const end = colon + 1;

	errno = 0;
	first = strtoul(start, NULL, 0);
	second = strtoul(end, NULL, 0);

	if (size_spec) {
		region->base = first;
		region->size = second;
		region->limit = region->base + region->size - 1;
	} else {
		region->base = first;
		region->limit = second;
		region->size = region->limit - region->base + 1;
	}

	free(start);
	if (errno) {
		perror("Failed to parse region");
		return -1;
	} else {
		return 0;
	}
}

static void print_usage(const char *name)
{
	printf("usage: %s [(-b|-m|-g|-p) <start>:<end>]... <output file>\n", name);
	printf("\n"
	       "   -b | --bios       <start>:<end>   BIOS region\n"
	       "   -m | --me         <start>:<end>   Intel ME region\n"
	       "   -g | --gbe        <start>:<end>   Gigabit Ethernet region\n"
	       "   -p | --platform   <start>:<end>   Platform Data region\n"
	       "   -h | --help                       print this help\n\n"
	       "<start> and <end> bounds are given in bytes, the <end> bound is inclusive.\n"
	       "All regions must be multiples of 4K in size and 4K aligned.\n"
	       "The descriptor region always resides in the first 4K.\n\n"
	       "An IFD created with ifdfake won't work as a replacement for a real IFD.\n"
	       "Never try to flash such an IFD to your board!\n\n");
}

int main(int argc, char *argv[])
{
	int opt, option_index = 0, idx;
	region_t regions[REGION_COUNT];

	memset(regions, 0x00, sizeof(regions));

	static struct option long_options[] = {
		{"bios", 1, NULL, 'b'},
		{"me", 1, NULL, 'm'},
		{"gbe", 1, NULL, 'g'},
		{"platform", 1, NULL, 'p'},
		{"help", 0, NULL, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "b:m:g:p:h?",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'b': case 'm': case 'g': case 'p':
			switch (opt) {
				case 'b': idx = 1; break;
				case 'm': idx = 2; break;
				case 'g': idx = 3; break;
				case 'p': idx = 4; break;
				default:  idx = 0; break; /* can't happen */
			}
			if (parse_region(optarg, &regions[idx])) {
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		}
	}

	if (optind + 1 != argc) {
		fprintf(stderr, "No output file given.\n\n");
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	regions[0].base   = 0x00000000;
	regions[0].limit  = 0x00000fff;
	regions[0].size   = 0x00001000;
	for (idx = 1; idx < REGION_COUNT; ++idx) {
		if (regions[idx].size) {
			if (regions[idx].base & 0xfff)
				fprintf(stderr, "Region %d is "
					"not 4K aligned.\n", idx);
			else if (regions[idx].size & 0xfff)
				fprintf(stderr, "Region %d size is "
					"no multiple of 4K.\n", idx);
			else if (regions[idx].limit <= regions[idx].base)
				fprintf(stderr, "Region %d is empty.\n", idx);
			else
				continue;
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	write_image(regions, argv[optind]);

	return 0;
}
