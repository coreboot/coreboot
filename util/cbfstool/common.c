/*
 * common utility functions for cbfstool
 *
 * Copyright (C) 2009 coresystems GmbH
 *                 written by Patrick Georgi <patrick.georgi@coresystems.de>
 * Copyright (C) 2012 Google, Inc.
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

/* Utilities */

/* Small, OS/libc independent runtime check for endianess */
int is_big_endian(void)
{
	static const uint32_t inttest = 0x12345678;
	uint8_t inttest_lsb = *(uint8_t *)&inttest;
	if (inttest_lsb == 0x12) {
		return 1;
	}
	return 0;
}

/* Buffer and file I/O */

int buffer_create(struct buffer *buffer, size_t size, const char *name) {
	buffer->name = strdup(name);
	buffer->size = size;
	buffer->data = (char *)malloc(buffer->size);
	if (!buffer->data) {
		fprintf(stderr, "buffer_create: Insufficient memory (0x%zx).\n",
			size);
	}
	return (buffer->data == NULL);
}

int buffer_from_file(struct buffer *buffer, const char *filename) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	buffer->size = ftell(fp);
	buffer->name = strdup(filename);
	rewind(fp);
	buffer->data = (char *)malloc(buffer->size);
	assert(buffer->data);
	if (fread(buffer->data, 1, buffer->size, fp) != buffer->size) {
		fprintf(stderr, "incomplete read: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int buffer_write_file(struct buffer *buffer, const char *filename) {
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		perror(filename);
		return -1;
	}
	assert(buffer && buffer->data);
	if (fwrite(buffer->data, 1, buffer->size, fp) != buffer->size) {
		fprintf(stderr, "incomplete write: %s\n", filename);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

void buffer_delete(struct buffer *buffer) {
	assert(buffer);
	if (buffer->name) {
		free(buffer->name);
		buffer->name = NULL;
	}
	if (buffer->data) {
		free(buffer->data);
		buffer->data = NULL;
	}
	buffer->size = 0;
}

size_t getfilesize(const char *filename)
{
	size_t size;
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
		return -1;

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
			ERROR("Could not get %d bytes for file %s\n",
			      *romsize_p, filename);
			exit(1);
		}
	} else if (place == SEEK_END)
		content -= *romsize_p;

	if (!fread(content, *romsize_p, 1, file)) {
		ERROR("Failed to read %s\n", filename);
		return NULL;
	}
	fclose(file);
	return content;
}

static struct cbfs_header *master_header;
static uint32_t phys_start, phys_end, align;
uint32_t romsize;
void *offset;
uint32_t arch = CBFS_ARCHITECTURE_UNKNOWN;

static struct {
	uint32_t arch;
	const char *name;
} arch_names[] = {
	{ CBFS_ARCHITECTURE_ARMV7, "armv7" },
	{ CBFS_ARCHITECTURE_X86, "x86" },
	{ CBFS_ARCHITECTURE_UNKNOWN, "unknown" }
};

uint32_t string_to_arch(const char *arch_string)
{
	int i;
	uint32_t ret = CBFS_ARCHITECTURE_UNKNOWN;

	for (i = 0; i < ARRAY_SIZE(arch_names); i++) {
		if (!strcasecmp(arch_string, arch_names[i].name)) {
			ret = arch_names[i].arch;
			break;
		}
	}

	return ret;
}

const char *arch_to_string(uint32_t a)
{
	int i;
	const char *ret = NULL;

	for (i = 0; i < ARRAY_SIZE(arch_names); i++) {
		if (a == arch_names[i].arch) {
			ret = arch_names[i].name;
			break;
		}
	}

	return ret;

}

int find_master_header(void *romarea, size_t size)
{
	size_t offset;

	if (master_header)
		return 0;

	for (offset = 0; offset < size - sizeof(struct cbfs_header); offset++) {
		struct cbfs_header *tmp = romarea + offset;

		if (tmp->magic == ntohl(CBFS_HEADER_MAGIC)) {
			master_header = tmp;
			break;
		}
	}

	return master_header ? 0 : 1;
}

void recalculate_rom_geometry(void *romarea)
{
	if (find_master_header(romarea, romsize)) {
		ERROR("Cannot find master header\n");
		exit(1);
	}

	/* Update old headers */
	if (master_header->version == CBFS_HEADER_VERSION1 &&
	    ntohl(master_header->architecture) == CBFS_ARCHITECTURE_UNKNOWN) {
		DEBUG("Updating CBFS master header to version 2\n");
		master_header->architecture = htonl(CBFS_ARCHITECTURE_X86);
	}

	arch = ntohl(master_header->architecture);

	switch (arch) {
	case CBFS_ARCHITECTURE_ARMV7:
		offset = romarea;
		phys_start = (0 + ntohl(master_header->offset)) & 0xffffffff;
		phys_end = romsize & 0xffffffff;
		break;
	case CBFS_ARCHITECTURE_X86:
		offset = romarea + romsize - 0x100000000ULL;
		phys_start = (0 - romsize + ntohl(master_header->offset)) &
				0xffffffff;
		phys_end = (0 - ntohl(master_header->bootblocksize) -
		     sizeof(struct cbfs_header)) & 0xffffffff;
		break;
	default:
		ERROR("Unknown architecture\n");
		exit(1);
	}

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
		ERROR("Could not open '%s' for writing: ", filename);
		perror("");
		return 1;
	}

	if (fwrite(start, size, 1, file) != 1) {
		ERROR("Could not write to '%s': ", filename);
		perror("");
		return 1;
	}

	fclose(file);
	return 0;
}

int cbfs_file_header(unsigned long physaddr)
{
	/* maybe improve this test */
	return (strncmp(phys_to_virt(physaddr), "LARCHIVE", 8) == 0);
}

struct cbfs_file *cbfs_create_empty_file(uint32_t physaddr, uint32_t size)
{
	struct cbfs_file *nextfile = (struct cbfs_file *)phys_to_virt(physaddr);
	strncpy((char *)(nextfile->magic), "LARCHIVE", 8);
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

static struct filetypes_t {
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
	{CBFS_COMPONENT_CMOS_LAYOUT, "cmos layout"},
	{CBFS_COMPONENT_DELETED, "deleted"},
	{CBFS_COMPONENT_NULL, "null"}
};

void print_supported_filetypes(void)
{
	int i, number = ARRAY_SIZE(filetypes);

	for (i=0; i<number; i++) {
		LOG(" %s%c", filetypes[i].name, (i==(number-1))?'\n':',');
		if ((i%8) == 7)
			LOG("\n");
	}
}

const char *strfiletype(uint32_t number)
{
	size_t i;
	for (i = 0; i < (sizeof(filetypes) / sizeof(struct filetypes_t)); i++)
		if (filetypes[i].type == number)
			return filetypes[i].name;
	return "unknown";
}

uint64_t intfiletype(const char *name)
{
	size_t i;
	for (i = 0; i < (sizeof(filetypes) / sizeof(struct filetypes_t)); i++)
		if (strcmp(filetypes[i].name, name) == 0)
			return filetypes[i].type;
	return -1;
}

void print_cbfs_directory(const char *filename)
{
	char *name = strdup(filename);
	printf
		("%s: %d kB, bootblocksize %d, romsize %d, offset 0x%x\n"
		 "alignment: %d bytes, architecture: %s\n\n",
		 basename(name), romsize / 1024, ntohl(master_header->bootblocksize),
		 romsize, ntohl(master_header->offset), align, arch_to_string(arch));
	free(name);
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
		       current - phys_start + ntohl(master_header->offset),
		       strfiletype(ntohl(thisfile->type)), length);

		/* note the components of the subheader are in host order ... */
		switch (ntohl(thisfile->type)) {
		case CBFS_COMPONENT_STAGE:
		{
			struct cbfs_stage *stage = CBFS_SUBHEADER(thisfile);
			INFO("    %s compression, entry: 0x%llx, load: 0x%llx, length: %d/%d\n",
			       stage->compression == CBFS_COMPRESS_LZMA ? "LZMA" : "no",
			       (unsigned long long)stage->entry,
			       (unsigned long long)stage->load,
			       stage->len,
			       stage->memlen);
			break;
		}
		case CBFS_COMPONENT_PAYLOAD:
		{
			struct cbfs_payload_segment *payload = CBFS_SUBHEADER(thisfile);
			while(payload) {
				switch(payload->type) {
				case PAYLOAD_SEGMENT_CODE:
				case PAYLOAD_SEGMENT_DATA:
					INFO("    %s (%s compression, offset: 0x%x, load: 0x%llx, length: %d/%d)\n",
						payload->type == PAYLOAD_SEGMENT_CODE ? "code " : "data" ,
						payload->compression == CBFS_COMPRESS_LZMA ? "LZMA" : "no",
						ntohl(payload->offset),
						(unsigned long long)ntohll(payload->load_addr),
						ntohl(payload->len), ntohl(payload->mem_len));
					break;
				case PAYLOAD_SEGMENT_ENTRY:
					INFO("    entry (0x%llx)\n", (unsigned long long)ntohll(payload->load_addr));
					break;
				case PAYLOAD_SEGMENT_BSS:
					INFO("    BSS (address 0x%016llx, length 0x%x)\n", (unsigned long long)ntohll(payload->load_addr), ntohl(payload->len));
					break;
				case PAYLOAD_SEGMENT_PARAMS:
					INFO("    parameters\n");
					break;
				default:
					INFO("    %x (%s compression, offset: 0x%x, load: 0x%llx, length: %d/%d\n",
						payload->type,
						payload->compression == CBFS_COMPRESS_LZMA ? "LZMA" : "no",
						ntohl(payload->offset),
						(unsigned long long)ntohll(payload->load_addr),
						ntohl(payload->len),
						ntohl(payload->mem_len));
					break;
				}

				if(payload->type == PAYLOAD_SEGMENT_ENTRY)
					payload=NULL;
				else
					payload++;
			}
			break;
		}
		default:
			break;
		}
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}
}

int extract_file_from_cbfs(const char *filename, const char *payloadname, const char *outpath)
{
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
		LOG("Found file %.30s at 0x%x, type %.12s, size %d\n", fname,
		       current - phys_start, strfiletype(ntohl(thisfile->type)),
		       length);

		// If we are not dumping to stdout, open the out file.
		outfile = fopen(outpath, "wb");
		if (!outfile)
		{
			ERROR("Could not open the file %s for writing.\n", outpath);
			return 1;
		}

		if (ntohl(thisfile->type) != CBFS_COMPONENT_RAW)
		{
			WARN("Only 'raw' files are safe to extract.\n");
		}

		fwrite(((char *)thisfile)
				+ ntohl(thisfile->offset), length, 1, outfile);

		fclose(outfile);
		LOG("Successfully dumped the file.\n");

		// We'll only dump one file.
		return 0;
	}
	ERROR("File %s not found.\n", payloadname);
	return 1;
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

		DEBUG("at %x, %x bytes\n", current, length);
		/* Is this a free chunk? */
		if ((thisfile->type == CBFS_COMPONENT_DELETED)
		    || (thisfile->type == CBFS_COMPONENT_NULL)) {
			DEBUG("null||deleted at %x, %x bytes\n", current,
				length);
			/* if this is the right size, and if specified, the right location, use it */
			if ((contentsize <= length)
			    && ((location == 0) || (current == location))) {
				if (contentsize < length) {
					DEBUG("this chunk is %x bytes, we need %x. create a new chunk at %x with %x bytes\n",
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
				DEBUG("copying data\n");
				memcpy(phys_to_virt(current), content,
				       contentsize);
				return 0;
			}
			if (location != 0) {
				/* CBFS has the constraint that the chain always moves up in memory. so once
				   we're past the place we seek, we don't need to look any further */
				if (current > location) {
					ERROR("The requested space is not available\n");
					return 1;
				}

				/* Is the requested location inside the current chunk? */
				if ((current < location)
				    && ((location + contentsize) <=
					(current + length))) {
					/* Split it up. In the next iteration the code will be at the right place. */
					DEBUG("split up. new length: %x\n",
						location - current -
						ntohl(thisfile->offset));
					thisfile->len =
					    htonl(location - current -
						  ntohl(thisfile->offset));
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
	ERROR("Could not add the file to CBFS, it's probably too big.\n");
	ERROR("File size: %d bytes (%d KB).\n", contentsize, contentsize/1024);
	return 1;
}


static struct cbfs_file *merge_adjacent_files(struct cbfs_file *first,
					      struct cbfs_file *second)
{
	uint32_t new_length =
	    ntohl(first->len) + ntohl(second->len) + ntohl(second->offset);
	first->len = htonl(new_length);
	first->checksum = 0; // FIXME?
	return first;
}

static struct cbfs_file *next_file(struct cbfs_file *prev)
{
	uint32_t pos = (prev == NULL) ? phys_start :
	    ALIGN(virt_to_phys(prev) + ntohl(prev->len) + ntohl(prev->offset),
		  align);

	for (; pos < phys_end; pos += align) {
		if (cbfs_file_header(pos))
			return (struct cbfs_file *)phys_to_virt(pos);
	}
	return NULL;
}


int remove_file_from_cbfs(const char *filename)
{
	struct cbfs_file *prev = NULL;
	struct cbfs_file *cur = next_file(prev);
	struct cbfs_file *next = next_file(cur);
	for (; cur; prev = cur, cur = next, next = next_file(next)) {

		/* Check if this is the file to remove. */
		char *name = (char *)cur + sizeof(*cur);
		if (strcmp(name, filename))
			continue;

		/* Mark the file as free space and erase its name. */
		cur->type = CBFS_COMPONENT_NULL;
		name[0] = '\0';

		/* Merge it with the previous file if possible. */
		if (prev && prev->type == CBFS_COMPONENT_NULL)
			cur = merge_adjacent_files(prev, cur);

		/* Merge it with the next file if possible. */
		if (next && next->type == CBFS_COMPONENT_NULL)
			merge_adjacent_files(cur, next);

		return 0;
	}
	ERROR("CBFS file %s not found.\n", filename);
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
		ERROR("Could not get %d bytes for CBFS file.\n", *datasize +
		       headersize);
		exit(1);
	}
	memset(newdata, 0xff, *datasize + headersize);
	struct cbfs_file *nextfile = (struct cbfs_file *)newdata;
	strncpy((char *)(nextfile->magic), "LARCHIVE", 8);
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
		const char *bootblock, uint32_t align, uint32_t offs)
{
	uint32_t bootblocksize = 0;
	struct cbfs_header *master_header;
	unsigned char *romarea, *bootblk;

	romsize = _romsize;
	romarea = malloc(romsize);
	if (!romarea) {
		ERROR("Could not get %d bytes of memory"
			" for CBFS image.\n", romsize);
		exit(1);
	}
	memset(romarea, 0xff, romsize);

	if (align == 0)
		align = 64;

	bootblk = loadfile(bootblock, &bootblocksize,
				romarea + romsize, SEEK_END);
	if (!bootblk) {
		ERROR("Could not load bootblock %s.\n",
			bootblock);
		free(romarea);
		return 1;
	}

	// TODO(hungte) Replace magic numbers by named constants.
	switch (arch) {
	case CBFS_ARCHITECTURE_ARMV7:
		/* Set up physical/virtual mapping */
		offset = romarea;

		/*
		 * The initial jump instruction and bootblock will be placed
		 * before and after the master header, respectively. The
		 * bootblock image must contain a blank, aligned region large
		 * enough for the master header to fit.
		 *
		 * An anchor string must be left such that when cbfstool is run
		 * we can find it and insert the master header at the next
		 * aligned boundary.
		 */
		loadfile(bootblock, &bootblocksize, romarea + offs, SEEK_SET);

		unsigned char *p = romarea + offs;
		while (1) {
			/* FIXME: assumes little endian... */
			if (*(uint32_t *)p == 0xdeadbeef)
				break;
			if (p >= (romarea + _romsize)) {
				ERROR("Could not determine CBFS "
				      "header location.\n");
				return 1;
			}
			p += (sizeof(unsigned int));
		}
		unsigned int u = ALIGN((unsigned int)(p - romarea), align);
		master_header = (struct cbfs_header *)(romarea + u);

		master_header->magic = ntohl(CBFS_HEADER_MAGIC);
		master_header->version = ntohl(CBFS_HEADER_VERSION);
		master_header->romsize = htonl(romsize);
		master_header->bootblocksize = htonl(bootblocksize);
		master_header->align = htonl(align);
		master_header->offset = htonl(
				ALIGN((0x40 + bootblocksize), align));
		master_header->architecture = htonl(CBFS_ARCHITECTURE_ARMV7);

		((uint32_t *) phys_to_virt(0x4 + offs))[0] =
				virt_to_phys(master_header);

		recalculate_rom_geometry(romarea);

		cbfs_create_empty_file(
				offs + ALIGN((0x40 + bootblocksize), align),
				romsize - offs - sizeof(struct cbfs_file) -
				ALIGN((bootblocksize + 0x40), align));
		break;

	case CBFS_ARCHITECTURE_X86:
		// Set up physical/virtual mapping
		offset = romarea + romsize - 0x100000000ULL;

		loadfile(bootblock, &bootblocksize, romarea + romsize,
			 SEEK_END);
		master_header = (struct cbfs_header *)(romarea + romsize -
				  bootblocksize - sizeof(struct cbfs_header));

		master_header->magic = ntohl(CBFS_HEADER_MAGIC);
		master_header->version = ntohl(CBFS_HEADER_VERSION);
		master_header->romsize = htonl(romsize);
		master_header->bootblocksize = htonl(bootblocksize);
		master_header->align = htonl(align);
		master_header->offset = htonl(offs);
		master_header->architecture = htonl(CBFS_ARCHITECTURE_X86);

		((uint32_t *) phys_to_virt(CBFS_HEADPTR_ADDR_X86))[0] =
		    virt_to_phys(master_header);

		recalculate_rom_geometry(romarea);

		cbfs_create_empty_file((0 - romsize + offs) & 0xffffffff,
				       romsize - offs - bootblocksize -
				       sizeof(struct cbfs_header) -
				       sizeof(struct cbfs_file) - 16);
		break;

	default:
		// Should not happen.
		ERROR("You found a bug in cbfstool.\n");
		exit(1);
	}

	writerom(romfile, romarea, romsize);
	free(romarea);
	return 0;
}

static int in_segment(int addr, int size, int gran)
{
	return ((addr & ~(gran - 1)) == ((addr + size) & ~(gran - 1)));
}

uint32_t cbfs_find_location(const char *romfile, uint32_t filesize,
			    const char *filename, uint32_t alignment)
{
	void *rom;
	size_t filename_size, headersize, totalsize;
	int ret = 0;
	uint32_t current;

	rom = loadrom(romfile);
	if (rom == NULL) {
		ERROR("Could not load ROM image '%s'.\n", romfile);
		return 0;
	}

	filename_size = strlen(filename);
	headersize = sizeof(struct cbfs_file) + ALIGN(filename_size + 1, 16) +
			sizeof(struct cbfs_stage);
	totalsize = headersize + filesize;

	current = phys_start;
	while (current < phys_end) {
		uint32_t top;
		struct cbfs_file *thisfile;

		if (!cbfs_file_header(current)) {
			current += align;
			continue;
		}

		thisfile = (struct cbfs_file *)phys_to_virt(current);

		top = current + ntohl(thisfile->len) + ntohl(thisfile->offset);

		if (((ntohl(thisfile->type) == 0x0)
		     || (ntohl(thisfile->type) == 0xffffffff))
		    && (ntohl(thisfile->len) + ntohl(thisfile->offset) >=
			totalsize)) {
			if (in_segment
			    (current + headersize, filesize, alignment)) {
				ret = current + headersize;
				break;
			}
			if ((ALIGN(current, alignment) + filesize < top)
			    && (ALIGN(current, alignment) - headersize >
				current)
			    && in_segment(ALIGN(current, alignment), filesize,
					  alignment)) {
				ret = ALIGN(current, alignment);
				break;
			}
			if ((ALIGN(current, alignment) + alignment + filesize <
			     top)
			    && (ALIGN(current, alignment) + alignment -
				headersize > current)
			    && in_segment(ALIGN(current, alignment) + alignment,
					  filesize, alignment)) {
				ret = ALIGN(current, alignment) + alignment;
				break;
			}
		}
		current =
		    ALIGN(current + ntohl(thisfile->len) +
			  ntohl(thisfile->offset), align);
	}

	free(rom);
	return ret;
}
