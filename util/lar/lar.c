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

#include "lib.h"
#include "lar.h"
#include "create.h"
#include "extract.h"
#include "list.h"

int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("Usage: lar [cxl] archive.lar <files>\n");
		exit(0);
	}

	if (strcmp(argv[1], "x")==0)
		extract_lar(argc, argv);
	else if (strcmp(argv[1], "c")==0)
		create_lar(argc,argv);
	else if (strcmp(argv[1], "l")==0)
		list_lar(argc,argv);
	else {
		printf("mode must be c or x\n");
		exit(1);
	}

	return 0;
}
