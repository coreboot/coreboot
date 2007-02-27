/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006 coresystems GmbH
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
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PATH 1024

int mkdirp(const char *dirpath)
{
	char *pos, *currpath, *path;
	char cwd[MAX_PATH];
	int ret = 0;

	path = strdup(dirpath);
	currpath = path;

	if (!getcwd(cwd, MAX_PATH)) {
		free(path);
		printf("Error getting cwd.\n");
		return -1;
	}

	do {
		pos = index(currpath, '/');
		if (pos)
			*pos = 0;

		/* printf("cp=%s\n", currpath); */
		mkdir(currpath, 0755);
		ret = chdir(currpath);

		if (pos)
			currpath = pos + 1;
	} while (pos && !ret && strlen(currpath));

	chdir(cwd);
	free(path);

	return ret;
}
