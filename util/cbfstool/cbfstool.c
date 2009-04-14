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

/* v2 compat: First, assumes a 64K bootblock. 
 * cbfstool coreboot.rom create 0x80000 coreboot.strip 
 * cbfstool coreboot.rom add-payload /tmp/filo.elf payload 
 * cbfstool coreboot.rom print
 */


#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/mman.h>
#include "cbfstool.h"

extern int create_handler(struct rom *, int, char **);
extern int bootblock_handler(struct rom *, int, char **);
extern int print_handler(struct rom *, int, char **);
extern int add_handler(struct rom *, int, char **);
extern int delete_handler(struct rom *, int, char **);
extern int resize_handler(struct rom *, int, char **);
extern int add_payload_handler(struct rom *, int, char **);
extern int add_stage_handler(struct rom *, int, char **);

extern void create_usage(void);
extern void bootblock_usage(void);
extern void print_usage(void);
extern void add_usage(void);
extern void delete_usage(void);
extern void resize_usage(void);
extern void add_payload_usage(void);
extern void add_stage_usage(void);

struct {
	char *command;
	int (*handler) (struct rom *, int, char **);
	void (*help) (void);
} commands[] = {
	{
	"add", add_handler, add_usage}, {
	"add-payload", add_payload_handler, add_payload_usage}, {
	"add-stage", add_stage_handler, add_stage_usage}, {
	"bootblock", bootblock_handler, bootblock_usage}, {
	"create", create_handler, create_usage}, {
	"delete", delete_handler, delete_usage}, {
	"print", print_handler, print_usage}, {
	"resize", resize_handler, resize_usage}, {
"", NULL},};

static struct rom rom;

char cbfstool_bindir[255];

void show_help(void)
{
	int i;

	printf("cbfstool [OPTION] [[FILE] [COMMAND] [PARAMETERS]...\n");
	printf("Apply COMMANDS with PARAMETERS to FILE.  If no COMMAND is\n");
	printf("given, run in interactive mode\n\n");
	printf("OPTIONs:\n");
	printf(" -h\t\tDisplay this help message\n");
	printf(" -C <dir>\tChange to the directory before operating\n\n");
	printf("COMMANDs:\n");

	for (i = 0; commands[i].handler != NULL; i++)
		commands[i].help();
}

int main(int argc, char **argv)
{
	char *cdir = NULL;
	char *rname;
	char *cmd;
	int ret = -1, i;

	strncpy(cbfstool_bindir, dirname(argv[0]), 254);

	while (1) {
		signed ch = getopt(argc, argv, "hC:");
		if (ch == -1)
			break;
		switch (ch) {
		case 'h':
			show_help();
			return -1;
		case 'C':
			cdir = optarg;
			break;
		}
	}

	if (optind >= argc) {
		show_help();
		return -1;
	}

	if (cdir != NULL && chdir(cdir)) {
		ERROR("Unable to switch to %s: %m\n", cdir);
		return -1;
	}

	rname = argv[optind];
	cmd = optind + 1 < argc ? argv[optind + 1] : NULL;

	/* Open the ROM (if it exists) */
	rom.name = (unsigned char *)strdup(rname);

	if (!access(rname, F_OK)) {
		if (open_rom(&rom, rname)) {
			ERROR("Problem while reading the ROM\n");
			return -1;
		}
	}

	if (cmd) {
		/* Process the incoming comand */

		for (i = 0; commands[i].handler != NULL; i++) {
			if (!strcmp(commands[i].command, cmd)) {
				ret = commands[i].handler(&rom,
							  argc - 3, &argv[3]);
				goto leave;
			}
		}

		ERROR("Command %s not valid\n", cmd);
	} else {
		printf("Interactive mode not ready yet!\n");
	}

leave:
	if (rom.ptr != NULL && rom.ptr != MAP_FAILED)
		munmap(rom.ptr, rom.size);

	if (rom.fd > 0)
		close(rom.fd);

	return ret;
}
