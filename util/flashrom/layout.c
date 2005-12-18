#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "layout.h"
#include "lbtable.h"
#include "debug.h"

char * mainboard_vendor=NULL;
char * mainboard_part=NULL;
int romimages=0;

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


	walk=(unsigned int *)(bios+size-0x10);
	walk--; 
	
	if((*walk)==0 || ((*walk)&0x3ff) != 0) {
		/* We might have an Nvidia chipset bios 
		 * which stores the id information at a 
		 * different location.
		 */
		walk=(unsigned int *)(bios+size-0x80);
		walk--; 
	}
	
	if((*walk)==0 || ((*walk)&0x3ff) != 0) {
		printf("Flash image seems to be a legacy BIOS. Disabling checks.\n");
		mainboard_vendor=def_name;
		mainboard_part=def_name;
		return 0;
	}
	
	printf("LinuxBIOS last image size (not rom size) is %d bytes.\n", *walk);
	
	walk--; mainboard_part=strdup((const char *)(bios+size-*walk));
	walk--; mainboard_vendor=strdup((const char *)(bios+size-*walk));
	printf("MANUFACTURER: %s\n", mainboard_vendor);
	printf("MAINBOARD ID: %s\n", mainboard_part);
	
	/* These comparisons are case insensitive to make things
	 * a little less user^Werror prone. 
	 */
	if(lb_vendor && !strcasecmp(mainboard_vendor, lb_vendor) && 
	   lb_part && !strcasecmp(mainboard_part, lb_part)) {
		printf ("This firmware image matches "
		        "this motherboard.\n");
	} else {
		if(force) {
			printf("WARNING: This firmware image does not "
			"fit to this machine - forcing it.\n");
		} else {
			printf("ERROR: This firmware image does not "
			"fit to this machine\nOverride with -m if"
			"you know exactly what you are doing.\n");
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

	romlayout=fopen (name, "r");
	
	if(!romlayout) {
		printf("Error while opening rom layout.\n");
		return -1;
	}
	
	while(!feof(romlayout)) {
		char *tstr1, *tstr2;
		fscanf(romlayout,"%s %s\n", tempstr, rom_entries[romimages].name);
#if 0
		// fscanf does not like arbitrary comments like that :( later
		if (tempstr[0]=='#') {
			continue;
		}
#endif
		tstr1=strtok(tempstr,":");
		tstr2=strtok(NULL,":");
		rom_entries[romimages].start=strtol(tstr1, (char **)NULL, 16);
		rom_entries[romimages].end=strtol(tstr2, (char **)NULL, 16);
		rom_entries[romimages].included=0;
		romimages++;
	}
	
	for(i=0; i<romimages; i++) {
		printf("romlayout %08x - %08x named %s\n", 
			rom_entries[i].start,
			rom_entries[i].end,
			rom_entries[i].name);
	}

	fclose(romlayout);
	return 0;	
}

int find_romentry(char *name)
{
	int i;

	if(!romimages) return -1;

	printf("Looking for \"%s\"... ", name);
	
	for (i=0; i<romimages; i++) {
		if(!strcmp(rom_entries[i].name, name)) {
			rom_entries[i].included=1;
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

	
	for (i=0; i<romimages; i++) {
		
		if (rom_entries[i].included) 
			continue;
		
		memcpy (buffer+rom_entries[i].start,
			content+rom_entries[i].start,
			rom_entries[i].end-rom_entries[i].start);
	}

	return 0;
}
