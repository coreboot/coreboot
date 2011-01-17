/*
 * common utility functions for cbfstool
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
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
#include <libgen.h>
#include "common.h"
#include "cbfs.h"
#include "elf.h"

#define dprintf

uint32_t getfilesize(const char *filename)
{
	uint32_t size;
	FILE *file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fclose(file);
	return size;
}

void *loadfile(const char *filename, uint32_t * romsize_p, void *content,
	       int place)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
		return NULL;
	fseek(file, 0, SEEK_END);
	*romsize_p = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (!content) {
		content = malloc(*romsize_p);
		if (!content) {
			printf("Could not get %d bytes for file %s\n",
			       *romsize_p, filename);
			exit(1);
		}
	} else if (place == SEEK_END)
		content -= *romsize_p;

	if (!fread(content, *romsize_p, 1, file)) {
		printf("failed to read %s\n", filename);
		return NULL;
	}
	fclose(file);
	return content;
}

struct cbfs_header *master_header;
uint32_t phys_start, phys_end, align, romsize;
void *offset;

void recalculate_rom_geometry(void *romarea)
{
	offset = romarea + romsize - 0x100000000ULL;
	master_header = (struct cbfs_header *)
	    phys_to_virt(*((uint32_t *) phys_to_virt(0xfffffffc)));
	phys_start = (0 - romsize + ntohl(master_header->offset)) & 0xffffffff;
	phys_end =
	    (0 - ntohl(master_header->bootblocksize) -
	     sizeof(struct cbfs_header)) & 0xffffffff;
	align = ntohl(master_header->align);
}

void *loadrom(const char *filename)
{
	void *romarea = loadfile(filename, &romsize, 0, SEEK_SET);
	if (romarea == NULL)
		return NULL;
	recalculate_rom_geometry(romarea);
	return romarea;
}

int writerom(const char *filename, void *start, uint32_t size)
{
	FILE *file = fopen(filename, "wb");
	if (!file) {
		fprintf(stderr, "Could not open '%s' for writing: ", filename);
		perror("");
		return 1;
	}

	if (fwrite(start, size, 1, file) != 1) {
		fprintf(stderr, "Could not write to '%s': ", filename);
		perror("");
		return 1;
	}

	fclose(file);
	return 0;
}

int cbfs_file_header(uint32_t physaddr)
{
	/* maybe improve this test */
	return (strncmp(phys_to_virt(physaddr), "LARCHIVE", 8) == 0);
}

struct cbfs_file *cbfs_create_empty_file(uint32_t physaddr, uint32_t size)
{
	struct cbfs_file *nextfile = (struct cbfs_file *)phys_to_virt(physaddr);
	strncpy(nextfile->magic, "LARCHIVE", 8);
	nextfile->len = htonl(size);
	nextfile->type = htonl(0xffffffff);
	nextfile->checksum = 0;	// FIXME?
	nextfile->offset = htonl(sizeof(struct cbfs_file) + 16);
	memset(((void *)nextfile) + sizeof(struct cbfs_file), 0, 16);
	return nextfile;
}

int iself(unsigned char *input)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *) input;
	return !memcmp(ehdr->e_ident, ELFMAG, 4);
}

struct filetypes_t {
	uint32_t type;
	const char *name;
} filetypes[] = {
	{CBFS_COMPONENT_STAGE, "stage"},
	{CBFS_COMPONENT_PAYLOAD, "payload"},
	{CBFS_COMPONENT_OPTIONROM, "optionrom"},
	{CBFS_COMPONENT_BOOTSPLASH, "bootsplash"},
	{CBFS_COMPONENT_RAW, "raw"},
	{CBFS_COMPONENT_VSA, "vsa"},
	{CBFS_COMPONENT_MBI, "mbi"},
	{CBFS_COMPONENT_MICROCODE, "microcode"},
	{CBFS_COMPONENT_CMOS_DEFAULT, "cmos default"},
	{CBFS_COMPONENT_DELETED, "deleted"},
	{CBFS_COMPONENT_NULL, "null"}
};

void print_supported_filetypes(void)
{
	int i, number = ARRAY_SIZE(filetypes);

	for (i=0; i<number; i++) {
		printf(" %s%c", filetypes[i].name, (i==(number-1))?'\n':',');
		if ((i%8) == 7)
			printf("\n");
	}
}

const char *strfiletype(uint32_t number)
{
	int i;
	for (i = 0; i < (sizeof(filetypes) / sizeof(struct filetypes_t)); i++)
		if (filetypes[i].type == number)
			return filetypes[i].name;
	return "unknown";
}

uint64_t intfiletype(const char *name)
{
	int i;
	for (i = 0; i < (sizeof(filetypes) / sizeof(struct filetypes_t)); i++)
		if (strcmp(filetypes[i].name, name) == 0)
			return filetypes[i].type;
	return -1;
}

void print_cbfs_directory(const char *filename)
{
	printf
	    ("%s: %d kB, bootblocksize %d, romsize %d, offset 0x%x\nAlignment: %d bytes\n\n",
	     basename((char *)filename), romsize / 1024, ntohl(master_header->bootblocksize),
	     romsize, ntohl(master_header->offset), align);
	printf("%-30s %-10s %-12s Size\n", "Name", "Offset", "Type");
	uint32_t current = phys_start;
	while (current < phys_end) {
		if (!cbfs_file_header(current)) {
			current += align;
			continue;
		}
		struct cbfs_file *thisfile =
		    (struct cbfs_file *)phys_to_virt(current);
		uint32_t length = ntohl(thisfile->len);
		char *fname = (char *)(phys_to_virt(current) + sizeof(struct cbfs_file));
		if (strlen(fname) == 0)
			fname = "(empty)";

		printf("%-30s 0x%-8x %-12s %d\n", fname,
		       current - phys_start, strfiletype(ntohl(thisfile->type)),
		       length);
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}
}

int extract_file_from_cbfs(const char *filename, const char *payloadname, const char *outpath)
{
	// Identify the coreboot image.
	printf(
	     "%s: %d kB, bootblocksize %d, romsize %d, offset 0x%x\nAlignment: %d bytes\n\n",
	     basename((char *)filename), romsize / 1024, ntohl(master_header->bootblocksize),
	     romsize, ntohl(master_header->offset), align);

	FILE *outfile = NULL;
	uint32_t current = phys_start;
	while (current < phys_end) {
		if (!cbfs_file_header(current)) {
			current += align;
			continue;
		}

		// Locate the file start struct
		struct cbfs_file *thisfile =
		    (struct cbfs_file *)phys_to_virt(current);
		// And its length
		uint32_t length = ntohl(thisfile->len);
		// Locate the file name
		char *fname = (char *)(phys_to_virt(current) + sizeof(struct cbfs_file));
		// It's not the file we are looking for..
		if (strcmp(fname, payloadname) != 0)
		{
			current =
			   ALIGN(current + ntohl(thisfile->len) +
				  ntohl(thisfile->offset), align);
			continue;
		}

		// Else, it's our file.
		printf("Found file %.30s at 0x%x, type %.12s, size %d\n", fname,
		       current - phys_start, strfiletype(ntohl(thisfile->type)),
		       length);

		// If we are not dumping to stdout, open the out file.
		outfile = fopen(outpath, "wb");
		if (!outfile)
		{
			printf("Could not open the file %s for writing. Aborting.\n", outpath);
			return 1;
		}

		if (ntohl(thisfile->type) != CBFS_COMPONENT_RAW)
		{
			printf("Warning: only 'raw' files are safe to extract.\n");
		}

		fwrite(((char *)thisfile)
				+ ntohl(thisfile->offset), length, 1, outfile);

		fclose(outfile);
		printf("Successfully dumped the file.\n");

		// We'll only dump one file.
		return 0;
	}

}


int add_file_to_cbfs(void *content, uint32_t contentsize, uint32_t location)
{
	uint32_t current = phys_start;
	while (current < phys_end) {
		if (!cbfs_file_header(current)) {
			current += align;
			continue;
		}
		struct cbfs_file *thisfile =
		    (struct cbfs_file *)phys_to_virt(current);
		uint32_t length = ntohl(thisfile->len);

		dprintf("at %x, %x bytes\n", current, length);
		/* Is this a free chunk? */
		if ((thisfile->type == CBFS_COMPONENT_DELETED)
		    || (thisfile->type == CBFS_COMPONENT_NULL)) {
			dprintf("null||deleted at %x, %x bytes\n", current,
				length);
			/* if this is the right size, and if specified, the right location, use it */
			if ((contentsize <= length)
			    && ((location == 0) || (current == location))) {
				if (contentsize < length) {
					dprintf
					    ("this chunk is %x bytes, we need %x. create a new chunk at %x with %x bytes\n",
					     length, contentsize,
					     ALIGN(current + contentsize,
						   align),
					     length - contentsize);
					uint32_t start =
					    ALIGN(current + contentsize, align);
					uint32_t size =
					    current + ntohl(thisfile->offset)
					    + length - start - 16 -
					    sizeof(struct cbfs_file);
					cbfs_create_empty_file(start, size);
				}
				dprintf("copying data\n");
				memcpy(phys_to_virt(current), content,
				       contentsize);
				return 0;
			}
			if (location != 0) {
				/* CBFS has the constraint that the chain always moves up in memory. so once
				   we're past the place we seek, we don't need to look any further */
				if (current > location) {
					printf
					    ("the requested space is not available\n");
					return 1;
				}

				/* Is the requested location inside the current chunk? */
				if ((current < location)
				    && ((location + contentsize) <=
					(current + length))) {
					/* Split it up. In the next iteration the code will be at the right place. */
					dprintf("split up. new length: %x\n",
						location - current -
						ntohl(thisfile->offset));
					thisfile->len =
					    htonl(location - current -
						  ntohl(thisfile->offset));
					struct cbfs_file *nextfile =
					    cbfs_create_empty_file(location,
								   length -
								   (location -
								    current));
				}
			}
		}
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}
	printf("Could not add the file to CBFS, it's probably too big.\n");
	printf("File size: %d bytes (%d KB).\n", contentsize, contentsize/1024);
	return 1;
}

/* returns new data block with cbfs_file header, suitable to dump into the ROM. location returns
   the new location that points to the cbfs_file header */
void *create_cbfs_file(const char *filename, void *data, uint32_t * datasize,
		       uint32_t type, uint32_t * location)
{
	uint32_t filename_len = ALIGN(strlen(filename) + 1, 16);
	uint32_t headersize = sizeof(struct cbfs_file) + filename_len;
	if ((location != 0) && (*location != 0)) {
		uint32_t offset = *location % align;
		/* If offset >= (headersize % align), we can stuff the header into the offset.
		   Otherwise the header has to be aligned itself, and put before the offset data */
		if (offset >= (headersize % align)) {
			offset -= (headersize % align);
		} else {
			offset += align - (headersize % align);
		}
		headersize += offset;
		*location -= headersize;
	}
	void *newdata = malloc(*datasize + headersize);
	if (!newdata) {
		printf("Could not get %d bytes for CBFS file.\n", *datasize +
		       headersize);
		exit(1);
	}
	memset(newdata, 0xff, *datasize + headersize);
	struct cbfs_file *nextfile = (struct cbfs_file *)newdata;
	strncpy(nextfile->magic, "LARCHIVE", 8);
	nextfile->len = htonl(*datasize);
	nextfile->type = htonl(type);
	nextfile->checksum = 0;	// FIXME?
	nextfile->offset = htonl(headersize);
	strcpy(newdata + sizeof(struct cbfs_file), filename);
	memcpy(newdata + headersize, data, *datasize);
	*datasize += headersize;
	return newdata;
}

int create_cbfs_image(const char *romfile, uint32_t _romsize,
		      const char *bootblock, uint32_t align)
{
	romsize = _romsize;
	unsigned char *romarea = malloc(romsize);
	if (!romarea) {
		printf("Could not get %d bytes of memory for CBFS image.\n",
		       romsize);
		exit(1);
	}
	memset(romarea, 0xff, romsize);

	// Set up physical/virtual mapping
	offset = romarea + romsize - 0x100000000ULL;

	if (align == 0)
		align = 64;

	uint32_t bootblocksize = 0;
	loadfile(bootblock, &bootblocksize, romarea + romsize, SEEK_END);
	struct cbfs_header *master_header =
	    (struct cbfs_header *)(romarea + romsize - bootblocksize -
				   sizeof(struct cbfs_header));
	master_header->magic = ntohl(0x4f524243);
	master_header->version = ntohl(0x31313131);
	master_header->romsize = htonl(romsize);
	master_header->bootblocksize = htonl(bootblocksize);
	master_header->align = htonl(align);
	master_header->offset = htonl(0);
	((uint32_t *) phys_to_virt(0xfffffffc))[0] =
	    virt_to_phys(master_header);

	recalculate_rom_geometry(romarea);

	struct cbfs_file *one_empty_file =
	    cbfs_create_empty_file((0 - romsize) & 0xffffffff,
				   romsize - bootblocksize -
				   sizeof(struct cbfs_header) -
				   sizeof(struct cbfs_file) - 16);

	writerom(romfile, romarea, romsize);
	return 0;
}

static int in_segment(int addr, int size, int gran)
{
	return ((addr & ~(gran - 1)) == ((addr + size) & ~(gran - 1)));
}

uint32_t cbfs_find_location(const char *romfile, uint32_t filesize,
			    const char *filename, uint32_t alignment)
{
	void *rom = loadrom(romfile);
	int filename_size = strlen(filename);

	int headersize =
	    sizeof(struct cbfs_file) + ALIGN(filename_size + 1,
					     16) + sizeof(struct cbfs_stage);
	int totalsize = headersize + filesize;

	uint32_t current = phys_start;
	while (current < phys_end) {
		if (!cbfs_file_header(current)) {
			current += align;
			continue;
		}
		struct cbfs_file *thisfile =
		    (struct cbfs_file *)phys_to_virt(current);

		uint32_t top =
		    current + ntohl(thisfile->len) + ntohl(thisfile->offset);
		if (((ntohl(thisfile->type) == 0x0)
		     || (ntohl(thisfile->type) == 0xffffffff))
		    && (ntohl(thisfile->len) + ntohl(thisfile->offset) >=
			totalsize)) {
			if (in_segment
			    (current + headersize, filesize, alignment))
				return current + headersize;
			if ((ALIGN(current, alignment) + filesize < top)
			    && (ALIGN(current, alignment) - headersize >
				current)
			    && in_segment(ALIGN(current, alignment), filesize,
					  alignment))
				return ALIGN(current, alignment);
			if ((ALIGN(current, alignment) + alignment + filesize <
			     top)
			    && (ALIGN(current, alignment) + alignment -
				headersize > current)
			    && in_segment(ALIGN(current, alignment) + alignment,
					  filesize, alignment))
				return ALIGN(current, alignment) + alignment;
		}
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}
	return 0;
}
