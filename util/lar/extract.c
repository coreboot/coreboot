/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006-2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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

#include "lib.h"
#include "lar.h"

int extract_lar(const char *archivename, struct file *files)
{
	int archivefile;
	char *inmap;
	char *walk;
	char *fullname, *pathname, *pos;
	struct lar_header *header;
	struct stat statbuf;
	int archivelen;
	int do_extract;
	int i;

	if (stat(archivename, &statbuf) != 0) {
		printf("Error opening %s: %s\n", archivename, strerror(errno));
		exit(1);
	}

	if (verbose())
		printf("Opening %s\n", archivename);

	archivefile = open(archivename, O_RDONLY);
	if (archivefile == -1) {
		printf("Error while opening %s: %s\n",
		       archivename, strerror(errno));
		exit(1);
	}
	archivelen = statbuf.st_size;

	inmap = mmap(NULL, statbuf.st_size, PROT_READ,
		     MAP_SHARED, archivefile, 0);

	for (walk = inmap; walk < inmap + statbuf.st_size; walk += 16) {
		FILE *file_to_extract;

		if (strcmp(walk, MAGIC) != 0)
			continue;

		header = (struct lar_header *)walk;
		fullname = walk + sizeof(struct lar_header);

		/* FIXME: check checksum. */

		do_extract = 1;
		if (files) {
			struct file *fwalk = files;
			do_extract = 0;
			while (fwalk) {
				if (strcmp(fullname, fwalk->name) == 0) {
					do_extract = 1;
					break;
				}
				fwalk = fwalk->next;
			}
		}

		/* Don't extract this one, skip it. */
		if (!do_extract)
			continue;

		if (verbose())
			printf("  Extracting file %s\n",
			       walk + sizeof(struct lar_header));

		/* Create the directory if it does not exist. */
		pathname = strdup(fullname);
		if (!pathname) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}

		pos = strrchr(pathname, '/');
		if (pos) {
			pos[1] = 0;
			/* printf("Pathname %s\n",pathname); */
			mkdirp(pathname,0755);
		}
		free(pathname);

		file_to_extract = fopen(fullname, "w");
		if (!file_to_extract) {
			fprintf(stderr, "error creating file %s.\n", fullname);
			exit(1);
		}

		fwrite(walk + ntohl(header->offset), ntohl(header->len),
		       1, file_to_extract);
		fclose(file_to_extract);

		walk += (ntohl(header->offset) + ntohl(header->len)
			 - 1) & 0xfffffff0;
	}

	munmap(inmap, statbuf.st_size);
	close(archivefile);

	if (verbose())
		printf("done.\n");

	return 0;
}
