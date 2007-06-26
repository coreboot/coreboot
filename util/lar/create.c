/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 * Copyright (C) 2007 Patrick Georgi <patrick@georgi-clan.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <libgen.h>

#include "lib.h"
#include "lar.h"

extern enum compalgo algo;

void compress_impossible(char *in, u32 in_len, char *out, u32 *out_len) {
	fprintf(stderr,
		"The selected compression algorithm wasn't compiled in.\n");
	exit(1);
}

void do_no_compress(char *in, u32 in_len, char *out, u32 *out_len) {
	memcpy(out, in, in_len);
	out_len[0] = in_len;
}

int create_lar(const char *archivename, struct file *files)
{
	int i, ret;
	int diff = 0;
	int bb_header_len = 0;
	FILE *archive, *source;
	char *tempmem;
	char *filebuf, *filetarget;
	char *pathname;
	u32 *walk;
	u32 csum;
	int pathlen, entrylen, filelen;
	u32 compfilelen;
	long currentsize = 0;
	struct lar_header *header;
	struct stat statbuf;
	enum compalgo thisalgo;

	if (!files) {
		fprintf(stderr, "No files for archive %s\n", archivename);
		exit(1);
	}

	if (verbose())
		printf("Opening %s\n", archivename);

	archive = fopen(archivename, "w");
	if (!archive) {
		fprintf(stderr, "Could not open archive %s for writing\n",
			archivename);
		exit(1);
	}

	while (files) {
		char *name = files->name;

		thisalgo = algo;

		if (strstr(name, "nocompress:") == name) {
			name += 11;
			thisalgo = none;
		}

		/* skip ./ if available */
		if (name[0] == '.' && name[1] == '/')
			name += 2;

		if (verbose())
			printf("  Adding %s to archive\n", name);

		ret = stat(name, &statbuf);
		if (ret) {
			fprintf(stderr, "No such file %s\n", name);
			exit(1);
		}
		filelen = statbuf.st_size;

		tempmem = malloc(sizeof(struct lar_header) + MAX_PATHLEN
				 + filelen + 16);
		if (!tempmem) {
			fprintf(stderr, "Out of memory.\n");
			return (1);
		}
		memset(tempmem, 0, sizeof(struct lar_header) + MAX_PATHLEN
		       + filelen + 16);

		header = (struct lar_header *)tempmem;
		pathname = tempmem + sizeof(struct lar_header);
		pathlen = snprintf(pathname, MAX_PATHLEN - 1, name) + 1;
		pathlen = (pathlen + 15) & 0xfffffff0;/* Align to 16 bytes. */

		/* Read file into memory. */
		filebuf = malloc(filelen);
		filetarget = pathname + pathlen;
		source = fopen(name, "r");
		if (!source) {
			fprintf(stderr, "No such file %s\n", name);
			exit(1);
		}
		fread(filebuf, filelen, 1, source);
		fclose(source);
		compress_functions[thisalgo](filebuf, filelen, filetarget,
					     &compfilelen);
		if ((compfilelen >= filelen) && (thisalgo != none)) {
			thisalgo = none;
			compress_functions[thisalgo](filebuf, filelen,
						     filetarget, &compfilelen);
		}
		free(filebuf);

		/* Create correct header. */
		memcpy(header, MAGIC, 8);
		header->compression = htonl(thisalgo);
		header->reallen = htonl(filelen);
		header->len = htonl(compfilelen);
		header->offset = htonl(sizeof(struct lar_header) + pathlen);

		/* Calculate checksum. */
		csum = 0;
		for (walk = (u32 *) tempmem;
		     walk < (u32 *) (tempmem + compfilelen +
				     sizeof(struct lar_header) + pathlen);
		     walk++) {
			csum += ntohl(*walk);
		}
		header->checksum = htonl(csum);

		/* Write out entry to archive. */
		entrylen = (compfilelen + pathlen + sizeof(struct lar_header) +
			    15) & 0xfffffff0;

		fwrite(tempmem, entrylen, 1, archive);

		free(tempmem);

		/* size counter */
		currentsize += entrylen;

		files = files->next;
	}

	/* Calculate difference, if a size has been specified.
	 * If diff is below zero, the size has been exceeded.
	 * If diff is above zero, it specifies the number of 
	 * padding bytes required for the image.
	 * Otherwise diff stays 0 and no action is taken below.
	 */
	if (get_larsize())
		diff = get_larsize() - currentsize;

	/* If there's a bootblock loaded, some space is required
	 * _after_ the padding. 
	 * Calculate this size here, but write the bootblock later.
	 */

	if (bootblock_len) {
		if (verbose())
			printf ("Detected bootblock of %d bytes\n", bootblock_len);

		bb_header_len = sizeof(struct lar_header) +
			((strlen(basename(get_bootblock()))+15) & 0xfffffff0);

		bb_header_len = (bb_header_len + 15) & 0xfffffff0;

		if (verbose())
			printf ("Required bootblock header of %d bytes\n", bb_header_len);

		diff -= bootblock_len;
		diff -= bb_header_len;
	}

	/* The image became too big. Print an error message and exit,
	 * deleting the file. So nobody used an invalid image by accident.
	 *
	 * Don't delete the image in "Out of memory" situations. If memory
	 * is _that_ tight that a few bytes don't fit anymore, everything 
	 * else will fail as well, so just print an error and exit the 
	 * program as soon as possible.
	 */

	if (diff < 0) {
		fprintf(stderr,
			"Error: LAR archive exceeded size (%ld > %ld)\n",
			currentsize, get_larsize());

		/* Open files can not be deleted. */
		fclose(archive);
		/* File is too big, delete it. */
		unlink(archivename);
		return -1;
	}

	/* Pad the image. */

	if (diff > 0) {
		char *padding;
		/* generate padding (0xff is flash friendly) */
		padding = malloc(diff);
		if (!padding) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}
		memset(padding, 0xff, diff);
		fwrite(padding, diff, 1, archive);
		free(padding);
	}

	if (bootblock_len) {
		char *bootblock_header;
		struct lar_header *bb;

		bootblock_header = malloc(bb_header_len);
		if(!bootblock_header) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}

		memset (bootblock_header, 0, bb_header_len);

		/* construct header */
		bb=(struct lar_header *)bootblock_header;
		memcpy(bb->magic, MAGIC, 8);
		bb->reallen = htonl(bootblock_len);
		bb->len = htonl(bootblock_len);
		bb->offset = htonl(bb_header_len);

		/* TODO checksum */

		/* Write filename. we calculated the buffer size, 
		 * so no overflow danger here.
		 */
		strcpy(bootblock_header+sizeof(struct lar_header), 
				basename(get_bootblock()) );

		fwrite(bootblock_header, bb_header_len, 1, archive);
		fwrite(bootblock_code, bootblock_len, 1, archive);
	}

	fclose(archive);

	if (verbose())
		printf("done.\n");

	return 0;
}
