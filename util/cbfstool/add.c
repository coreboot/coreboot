/*
 * cbfstool
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "cbfstool.h"

#define MAX_PATH 255

static int add_from_fd(struct rom *rom, const char *name, int type, int fd)
{
	unsigned char *buffer = malloc(16 * 1024);
	unsigned char *ptr = buffer;

	int size = 0;
	int aloc = 16 * 1024;
	int remain = 16 * 1024;
	int ret;

	if (buffer == NULL)
		return -1;

	while (1) {
		ret = read(fd, ptr, remain);

		if (ret <= 0)
			break;

		ptr += ret;
		remain -= ret;
		size += ret;

		if (remain == 0) {
			buffer = realloc(buffer, aloc + 16 * 1024);

			if (buffer == NULL) {
				ret = -1;
				break;
			}

			ptr = buffer + size;

			aloc += (16 * 1024);
			remain = 16 * 1024;
		}
	}

	if (ret == -1 || size == 0) {

		if (buffer != NULL)
			free(buffer);

		return -1;
	}

	ret = rom_add(rom, name, buffer, 0, size, type);
	free(buffer);

	return ret;
}

int fork_tool_and_add(struct rom *rom, const char *tool, const char *input,
		      const char *name, int type, int argc, char **argv)
{
	int output[2];
	pid_t pid;
	int ret;
	int status;
	char **toolargs;
	int i;

	/* Create the pipe */

	if (pipe(output)) {
		ERROR("Couldn't create a pipe: %m\n");
		return -1;
	}

	toolargs = (char **)malloc((5 + argc) * sizeof(char *));

	if (toolargs == NULL) {
		ERROR("Unable to allocate memory: %m\n");
		return -1;
	}

	toolargs[0] = (char *)tool;

	/* these are args. So they need a - in front */
	for (i = 0; i < argc; i++) {
		/* I wish I had python */
		char *c = malloc(strlen(argv[i])) + 2;
		c[0] = '-';
		strcpy(&c[1], argv[i]);
		c[strlen(argv[i])+1] = 0;
		toolargs[1 + i] = c;
	}

	toolargs[1 + argc] = "-o";
	toolargs[2 + argc] = "-";
	toolargs[3 + argc] = (char *)input;
	toolargs[4 + argc] = NULL;

	pid = fork();

	if (pid == 0) {

		/* Set up stdin/stdout for the child */

		dup2(output[1], STDOUT_FILENO);
		close(output[0]);

		/* Execute the tool */
		if (execv(tool, toolargs)) {
			ERROR("Unable to execute %s: %m\n", tool);
			exit(-1);
		}

		exit(0);
	}

	free(toolargs);

	close(output[1]);

	/* Read from the file */
	ret = add_from_fd(rom, name, type, output[0]);

	/* Reap the child */
	waitpid(pid, &status, 0);

	if (WIFSIGNALED(status)) {
		kill(pid, WTERMSIG(status));
		ERROR("Error while executing %s\n", tool);
		return -1;
	} else if (WEXITSTATUS(status) != 0) {
		ERROR("Error while executing %s: %d\n", tool,
		      (int)WEXITSTATUS(status));
		return -1;
	}

	return ret;
}

static int add_blob(struct rom *rom, const char *filename,
		    const char *name, unsigned long address, int type)
{
	void *ptr;
	struct stat s;
	int fd, ret;

	if (!strcmp(filename, "-"))
		return add_from_fd(rom, name, type, 0);

	fd = open(filename, O_RDONLY);

	if (fd == -1) {
		ERROR("Could not open %s: %m\n", filename);
		return -1;
	}

	if (fstat(fd, &s)) {
		ERROR("Could not stat %s: %m\n", filename);
		close(fd);
		return -1;
	}

	ptr = mmap(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (ptr == MAP_FAILED) {
		ERROR("Unable to map %s: %m\n", filename);
		close(fd);
		return -1;
	}

	ret = rom_add(rom, name, ptr, address, s.st_size, type);

	munmap(ptr, s.st_size);
	close(fd);

	return ret;
}

void add_usage(void)
{
	printf("add FILE NAME TYPE [base address]\tAdd a component\n");
}

void add_stage_usage(void)
{
	printf("add-stage FILE NAME [OPTIONS]\tAdd a stage to the ROM\n");
}

void add_payload_usage(void)
{
	printf
	    ("add-payload FILE NAME [OPTIONS]\tAdd a payload to the ROM\n");
}

int select_component_type(char *s)
{
	int i = 0;
	char *accepted_strings[] = {
		"stage",
		"payload",
		"optionrom",
		"deleted",
		"free",
	};
	for (i=0; i < 5; i++)
		if (!strcmp(s, accepted_strings[i]))
			return i;
	return -1;
}

int add_handler(struct rom *rom, int argc, char **argv)
{
	unsigned int type = CBFS_COMPONENT_NULL;
	unsigned long address = 0;

	if ((argc < 3) || (argc >  4)) {
		add_usage();
		return -1;
	}

	if (argc > 3) {
		address = strtoul(argv[3], 0, 0);
	}

	if (!rom_exists(rom)) {
		ERROR("You need to create the ROM before adding files to it\n");
		return -1;
	}

	int component_type;

	/* There are two ways to specify the type - a string or a number */

	if (isdigit(*(argv[2])))
		type = strtoul(argv[2], 0, 0);
	else {
		switch( component_type = select_component_type(argv[2])) {
			case 0:
				type = CBFS_COMPONENT_STAGE;
				break;
			case 1:
				type = CBFS_COMPONENT_PAYLOAD;
				break;
			case 2:
				type = CBFS_COMPONENT_OPTIONROM;
				break;
			case 3:
				type = CBFS_COMPONENT_DELETED;
				break;
			case 4:
				type = CBFS_COMPONENT_NULL;
				break;
			default:
				ERROR("Unrecognized component type %s.\nValid options are: stage, payload, optionrom, deleted, free.\n", argv[2]);
				return -1;
		}
	}

	return add_blob(rom, argv[0], argv[1], address, type);
}

char *find_tool(char *tool)
{
	static char toolpath[MAX_PATH];
	extern char cbfstool_bindir[];

	snprintf(toolpath, MAX_PATH - 1, "tools/%s", tool);
	if (!access(toolpath, X_OK))
		return toolpath;

	snprintf(toolpath, MAX_PATH - 1, "%s/tools/%s", cbfstool_bindir, tool);

	if (!access(toolpath, X_OK))
		return toolpath;

	snprintf(toolpath, MAX_PATH - 1, "%s/%s", cbfstool_bindir, tool);

	if (!access(toolpath, X_OK))
		return toolpath;

	strncpy(toolpath, tool, MAX_PATH - 1);
	return toolpath;
}

/* Invoke the cbfs-mkpayload utility */

int add_payload_handler(struct rom *rom, int argc, char **argv)
{
	if (argc < 2) {
		add_payload_usage();
		return -1;
	}

	/* Make sure the ROM exists */

	if (!rom_exists(rom)) {
		ERROR("You need to create the ROM before adding files to it\n");
		return -1;
	}

	/* Check that the incoming file exists */

	if (access(argv[0], R_OK)) {
		ERROR("File %s does not exist\n", argv[0]);
		return -1;
	}

	return fork_tool_and_add(rom, find_tool("cbfs-mkpayload"), argv[0],
				 argv[1], CBFS_COMPONENT_PAYLOAD, argc - 2,
				 argc > 2 ? &argv[2] : NULL);
}

/* Invoke the cbfs-mkstage utility */

int add_stage_handler(struct rom *rom, int argc, char **argv)
{
	if (argc < 2) {
		add_stage_usage();
		return -1;
	}

	/* Make sure the ROM exists */

	if (!rom_exists(rom)) {
		ERROR("You need to create the ROM before adding files to it\n");
		return -1;
	}

	/* Check that the incoming file exists */

	if (access(argv[0], R_OK)) {
		ERROR("File %s does not exist\n", argv[0]);
		return -1;
	}

	return fork_tool_and_add(rom, find_tool("cbfs-mkstage"), argv[0],
				 argv[1], CBFS_COMPONENT_STAGE, argc - 2,
				 argc > 2 ? &argv[2] : NULL);
}
