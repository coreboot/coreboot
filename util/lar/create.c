/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH
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

int create_lar(int argc, char *argv[])
{
	int i, ret;
	FILE *archive, *source;
	char *archivename;
	char *tempmem;
	char *filebuf;
	char *pathname;
	u32 *walk;
	u32 csum;
	int pathlen, entrylen, filelen;
	struct lar_header  *header;
	struct stat statbuf;

	archivename=argv[2];
	if (argc<=3) {
		printf("No files for archive %s\n", archivename);
		exit(1);
	}

	printf("Opening %s\n", archivename);
	archive=fopen(archivename, "w");
	if(!archive) {
	// error
		exit(1);
	}

	for (i=3; i<argc; i++) {
		printf("  Adding %s to archive\n", argv[i]);

		ret=stat(argv[i], &statbuf);
		if(ret) {
			printf(" no such file %s\n", argv[i]);
			exit(1);
		}
		filelen=statbuf.st_size;

		tempmem=malloc(sizeof(struct lar_header)+MAX_PATHLEN+filelen+16);
		if(!tempmem) {
			printf ("no memory\n");
			return (1);
		}
		memset(tempmem, 0, sizeof(struct lar_header)+MAX_PATHLEN+filelen+16);

		header=(struct lar_header *)tempmem;
		pathname=tempmem+sizeof(struct lar_header);
		pathlen=sprintf(pathname, argv[i])+1;
		pathlen = (pathlen+15)&0xfffffff0; // align it to 16 bytes

		/* read file into memory */
		filebuf=pathname+pathlen;
		source=fopen(argv[i], "r");
		if(!source) {
		
			printf(" no such file %s\n", argv[i]);
			exit(1);
		}
		fread(filebuf, statbuf.st_size, 1, source);
		fclose(source);

		/* create correct header */
		memcpy(header, MAGIC, 8);
		header->len=htonl(statbuf.st_size);
		header->offset=htonl(sizeof(struct lar_header)+pathlen);

		/* calculate checksum */
		csum=0;
		for (walk=(u32 *)tempmem; 
		  walk<(u32 *)(tempmem+ statbuf.st_size+sizeof(struct lar_header)+pathlen);
		  walk++) {
			csum+=ntohl(*walk);
		}
		header->checksum=htonl(csum);

		/* write out entry to archive */
		entrylen=(filelen+pathlen+sizeof(struct lar_header)+15) & 0xfffffff0;

		fwrite(tempmem, entrylen, 1, archive);

		free(tempmem);

	}

	fclose(archive);
	printf("done.\n");

	return 0;
}


