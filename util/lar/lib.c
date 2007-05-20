/*
 * lar - LinuxBIOS archiver
 *
 * Copyright (C) 2006-2007 coresystems GmbH
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
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "lib.h"

#define MAX_PATH 1024

static struct file *files = NULL;

int mkdirp(const char *dirpath, mode_t mode)
{
	char *pos, *currpath, *path;
	char cwd[MAX_PATH];
	int ret = 0;

	path = strdup(dirpath);
	if (!path) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	currpath = path;

	if (!getcwd(cwd, MAX_PATH)) {
		free(path);
		fprintf(stderr, "Error getting cwd.\n");
		return -1;
	}

	do {
		pos = index(currpath, '/');
		if (pos)
			*pos = 0;

		/* printf("cp=%s\n", currpath); */
		mkdir(currpath, mode);
		ret = chdir(currpath);

		if (pos)
			currpath = pos + 1;
	} while (pos && !ret && strlen(currpath));

	chdir(cwd);
	free(path);

	return ret;
}

static int handle_directory(const char *name)
{
	int n;
	int ret = -1;
	struct dirent **namelist;

	n = scandir(name, &namelist, 0, alphasort);

	if (n < 0) {
		fprintf(stderr, "Could not enter directory %s\n", name);
	} else {
		while (n--) {
			char fullname[MAX_PATH];

			fullname[0] = '\0';

			if (strncmp("..", namelist[n]->d_name, 3) &&
			    strncmp(".", namelist[n]->d_name, 2)) {

				strncpy(fullname, name, MAX_PATH);

				if (name[(strlen(name)) - 1] != '/') {
					strncat(fullname, "/", MAX_PATH);
				}

				strncat(fullname, namelist[n]->d_name,
					MAX_PATH);

				add_files(fullname);
			}
			free(namelist[n]);

		}
		free(namelist);
		ret = 0;
	}

	return ret;
}

/*
 * Add physically existing files to the file list. 
 * This function is used when an archive is created.
 */

int add_files(const char *name)
{
	struct stat filestat;
	int ret = -1;

	/* printf("... add_files %s\n", name); */
	if (stat(name, &filestat) == -1) {
		fprintf(stderr, "Error getting file attributes of %s\n", name);
		return -1;
	}

	if (S_ISCHR(filestat.st_mode) || S_ISBLK(filestat.st_mode)) {
		fprintf(stderr, "Device files are not supported: %s\n", name);
	}

	if (S_ISFIFO(filestat.st_mode)) {
		fprintf(stderr, "FIFOs are not supported: %s\n", name);
	}

	if (S_ISSOCK(filestat.st_mode)) {
		fprintf(stderr, "Sockets are not supported: %s\n", name);
	}

	if (S_ISLNK(filestat.st_mode)) {
		fprintf(stderr, "Symbolic links are not supported: %s\n", name);
	}
	// Is it a directory?
	if (S_ISDIR(filestat.st_mode)) {
		ret = handle_directory(name);
	}
	// Is it a regular file?
	if (S_ISREG(filestat.st_mode)) {
		struct file *tmpfile;

		/* printf("... adding %s\n", name); */
		tmpfile = malloc(sizeof(struct file));
		if (!tmpfile) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}

		tmpfile->name = strdup(name);
		if (!tmpfile->name) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}

		tmpfile->next = files;
		files = tmpfile;
		ret = 0;
	}

	return ret;
}

/*
 * Add files or directories as specified to the file list. 
 * This function is used when an archive is listed or extracted.
 */

int add_file_or_directory(const char *name)
{
	struct file *tmpfile;

	tmpfile = malloc(sizeof(struct file));
	if (!tmpfile) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	tmpfile->name = strdup(name);
	if (!tmpfile->name) {
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	tmpfile->next = files;
	files = tmpfile;

	return 0;
}

struct file *get_files(void)
{
	return files;
}

void free_files(void)
{
	struct file *temp;

	while (files) {
		temp = files;
		files = files->next;
		free(temp->name);
		free(temp);
	}
}

#ifdef DEBUG
int list_files(void)
{
	struct file *walk = files;

	printf("File list:\n");
	while (walk) {
		printf("- %s\n", walk->name);
		walk = walk->next;
	}
	printf("-----\n");
	return 0;
}
#endif
