#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "flash.h"

char *mainboard_vendor = NULL;
char *mainboard_part = NULL;
int romimages = 0;

extern int force;

#define MAX_ROMLAYOUT	16

typedef struct {
	unsigned int start;
	unsigned int end;
	unsigned int included;
	char name[256];
} romlayout_t;

romlayout_t rom_entries[MAX_ROMLAYOUT];

static char *def_name = "DEFAULT";

int show_id(uint8_t *bios, int size)
{
	unsigned int *walk;

	walk = (unsigned int *)(bios + size - 0x10);
	walk--;

	if ((*walk) == 0 || ((*walk) & 0x3ff) != 0) {
		/* We might have an Nvidia chipset bios 
		 * which stores the id information at a 
		 * different location.
		 */
		walk = (unsigned int *)(bios + size - 0x80);
		walk--;
	}

	if ((*walk) == 0 || ((*walk) & 0x3ff) != 0) {
		printf("Flash image seems to be a legacy BIOS. Disabling checks.\n");
		mainboard_vendor = def_name;
		mainboard_part = def_name;
		return 0;
	}

	printf_debug("LinuxBIOS last image size "
		     "(not rom size) is %d bytes.\n", *walk);

	walk--;
	mainboard_part = strdup((const char *)(bios + size - *walk));
	walk--;
	mainboard_vendor = strdup((const char *)(bios + size - *walk));
	printf_debug("MANUFACTURER: %s\n", mainboard_vendor);
	printf_debug("MAINBOARD ID: %s\n", mainboard_part);

	/*
	 * If lb_vendor is not set, the linuxbios table was
	 * not found. Nor was -mVENDOR:PART specified
	 */

	if (!lb_vendor || !lb_part) {
		printf("Note: If the following flash access fails, "
		       "you might need to specify -m <vendor>:<mainboard>\n");
		return 0;
	}

	/* These comparisons are case insensitive to make things
	 * a little less user^Werror prone. 
	 */

	if (!strcasecmp(mainboard_vendor, lb_vendor) &&
	    !strcasecmp(mainboard_part, lb_part)) {
		printf_debug("This firmware image matches "
			     "this motherboard.\n");
	} else {
		if (force) {
			printf("WARNING: This firmware image does not "
			       "seem to fit to this machine - forcing it.\n");
		} else {
			printf("ERROR: Your firmware image (%s:%s) does not "
			       "appear to\n       be correct for the detected "
			       "mainboard (%s:%s)\n\nOverride with --force if you "
			       "are absolutely sure that you\nare using a correct "
			       "image for this mainboard or override\nthe detected "
			       "values with --mainboard <vendor>:<mainboard>.\n\n",
			       mainboard_vendor, mainboard_part, lb_vendor,
			       lb_part);
			exit(1);
		}
	}

	return 0;
}

int read_romlayout(char *name)
{
	FILE *romlayout;
	char tempstr[256];
	int i;

	romlayout = fopen(name, "r");

	if (!romlayout) {
		fprintf(stderr, "ERROR: Could not open rom layout (%s).\n",
			name);
		return -1;
	}

	while (!feof(romlayout)) {
		char *tstr1, *tstr2;
		fscanf(romlayout, "%s %s\n", tempstr,
		       rom_entries[romimages].name);
#if 0
		// fscanf does not like arbitrary comments like that :( later
		if (tempstr[0] == '#') {
			continue;
		}
#endif
		tstr1 = strtok(tempstr, ":");
		tstr2 = strtok(NULL, ":");
		rom_entries[romimages].start = strtol(tstr1, (char **)NULL, 16);
		rom_entries[romimages].end = strtol(tstr2, (char **)NULL, 16);
		rom_entries[romimages].included = 0;
		romimages++;
	}

	for (i = 0; i < romimages; i++) {
		printf_debug("romlayout %08x - %08x named %s\n",
			     rom_entries[i].start,
			     rom_entries[i].end, rom_entries[i].name);
	}

	fclose(romlayout);

	return 0;
}

int find_romentry(char *name)
{
	int i;

	if (!romimages)
		return -1;

	printf("Looking for \"%s\"... ", name);

	for (i = 0; i < romimages; i++) {
		if (!strcmp(rom_entries[i].name, name)) {
			rom_entries[i].included = 1;
			printf("found.\n");
			return i;
		}
	}
	printf("not found.\n");
	// Not found. Error.

	return -1;
}

int handle_romentries(uint8_t *buffer, uint8_t *content)
{
	int i;

	// This function does not safe flash write cycles.
	// 
	// Also it does not cope with overlapping rom layout
	// sections. 
	// example:
	// 00000000:00008fff gfxrom
	// 00009000:0003ffff normal
	// 00040000:0007ffff fallback
	// 00000000:0007ffff all
	//
	// If you'd specify -i all the included flag of all other
	// sections is still 0, so no changes will be made to the
	// flash. Same thing if you specify -i normal -i all only 
	// normal will be updated and the rest will be kept.

	for (i = 0; i < romimages; i++) {

		if (rom_entries[i].included)
			continue;

		memcpy(buffer + rom_entries[i].start,
		       content + rom_entries[i].start,
		       rom_entries[i].end - rom_entries[i].start);
	}

	return 0;
}
