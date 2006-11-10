/*
 * lar - LinuxBIOS archiver
 *
 * Copright (C) 2006 coresystems GmbH
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

int extract_lar(int argc, char *argv[])
{
	int archivefile;
	char * archivename;
	char * inmap;
	char * walk;
	char *fullname, *pathname, *pos;
	struct lar_header * header;
	struct stat statbuf;
	int archivelen;
	int do_extract;
	int i;

	archivename=argv[2];

	if (stat(archivename, &statbuf)!=0) {
		printf("Error opening %s: %s\n", 
				archivename, strerror(errno));
		exit(1);
	}
	printf("Opening %s\n", archivename);

	archivefile=open(archivename, O_RDONLY);
	if (archivefile==-1) {
		printf("Error while opening %s: %s\n", 
				archivename, strerror(errno));
		exit(1);
	}
	archivelen=statbuf.st_size;

		
	inmap=mmap(NULL, statbuf.st_size, PROT_READ,
			MAP_SHARED, archivefile, 0);

	for (walk=inmap; walk<inmap+statbuf.st_size; walk+=16) {
		FILE * file_to_extract;

		if(strcmp(walk, MAGIC)!=0)
			continue;

		header=(struct lar_header *)walk;
		fullname=walk+sizeof(struct lar_header);
		
		// FIXME: check checksum

		do_extract=1;
		if(argc>3) {
			do_extract=0;
			for (i=3; i<argc; i++) {
				if(strcmp(fullname, argv[i])==0) {
					do_extract=1;
					break;
				}
			}
		}

		// dont extract this one, skip it.
		if(!do_extract) 
			continue;

		printf("  Extracting file %s\n", walk+sizeof(struct lar_header));

		// Create the directory if it does not exist.
		pathname=strdup(fullname);
		pos=strrchr(pathname, '/');
		if(pos) {
			pos[1]=0;
			//printf("pathname %s\n",pathname);
			mkdirp(pathname);
		}
		free(pathname);

		file_to_extract=fopen(fullname, "w");
		if(!file_to_extract) {
			printf("error creating file.\n");
			exit(1);
		}
		//printf("starting offs=%d, len=%d\n", ntohl(header->offset),
		//		ntohl(header->len));
		fwrite(walk+ntohl(header->offset), ntohl(header->len),
				1, file_to_extract);
		fclose(file_to_extract);
	}
	munmap(inmap, statbuf.st_size);
	close(archivefile);
	printf("done.\n");

	return 0;
}


