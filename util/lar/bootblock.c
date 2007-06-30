/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lar.h"
#include "lib.h"

char *bootblock_code;
int bootblock_len;

int load_bootblock(const char *bootblock)
{
	struct stat statbuf;
	int ret, filelen;
	FILE *fh;

	ret = stat(bootblock, &statbuf);
	if (ret) {
		fprintf(stderr, "No such file %s\n", bootblock);
		exit(1);
	}
	bootblock_len = statbuf.st_size;

	/* We might want to find additional criteria
	 * for identifying a bootblock file
	 */
	if (bootblock_len != BOOTBLOCK_SIZE) {
		printf("Warning: %s does not seem to be a bootblock, "
				"so ignore it\n", bootblock);
		bootblock_code=NULL;
		bootblock_len=0;
		// Is this an error condition?
	}	

	bootblock_code = malloc(bootblock_len);
	if (!bootblock_code) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	fh = fopen(bootblock, "r");
	if (!fh) {
		fprintf(stderr, "Error while reading file %s\n", bootblock);
		exit(1);
	}

	fread(bootblock_code, bootblock_len, 1, fh);
	fclose(fh);

	return ret;
}

int fixup_bootblock(void)
{
	int i;
	uint32_t *size_pos;

	/* This cleans out the area after the reset vector */
	for(i=13; i>0; i--)
		bootblock_code[bootblock_len - i] = '\0';
	
	/* add lar size to image */
	size_pos=(uint32_t *)(bootblock_code+bootblock_len-12);
	size_pos[0] = get_larsize();

	return 0;
}

