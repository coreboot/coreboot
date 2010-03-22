/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008, Jordan Crouse <jordan@cosmicpenguin.net>
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <cbfs.h>
#include <lib.h>
#include <arch/byteorder.h>


/**
 * Decompression wrapper for CBFS
 * @param algo
 * @param src
 * @param dst
 * @param len
 * @return 0 on success, -1 on failure
 */
static int cbfs_decompress(int algo, void *src, void *dst, int len)
{
	switch(algo) {
	case CBFS_COMPRESS_NONE:
		memcpy(dst, src, len);
		return 0;

	case CBFS_COMPRESS_LZMA:
		if (!ulzma(src, dst)) {
			printk(BIOS_ERR, "CBFS: LZMA decompression failed!\n");
			return -1;
		}
		return 0;

	default:
		printk(BIOS_INFO,  "CBFS:  Unknown compression type %d\n", algo);
		return -1;
	}
}

static int cbfs_check_magic(struct cbfs_file *file)
{
	return !strcmp(file->magic, CBFS_FILE_MAGIC) ? 1 : 0;
}

static struct cbfs_header *cbfs_master_header(void)
{
	struct cbfs_header *header;

	void *ptr = (void *)*((unsigned long *) CBFS_HEADPTR_ADDR);
	printk(BIOS_SPEW, "Check CBFS header at %p\n", ptr);
	header = (struct cbfs_header *) ptr;

	printk(BIOS_SPEW, "magic is %08x\n", ntohl(header->magic));
	if (ntohl(header->magic) != CBFS_HEADER_MAGIC) {
		printk(BIOS_ERR, "ERROR: No valid CBFS header found!\n");
		if (header->magic == 0xffffffff) {
			printk(BIOS_ERR, "Maybe the ROM isn't entirely mapped yet?\n"
				"See (and report to) http://www.coreboot.org/Infrastructure_Projects#CBFS\n");
		}
		return NULL;
	}

	printk(BIOS_SPEW, "Found CBFS header at %p\n", ptr);
	return header;
}

struct cbfs_file *cbfs_find(const char *name)
{
	struct cbfs_header *header = cbfs_master_header();
	unsigned long offset;

	if (header == NULL)
		return NULL;
	offset = 0 - ntohl(header->romsize) + ntohl(header->offset);

	int align= ntohl(header->align);

	while(1) {
		struct cbfs_file *file = (struct cbfs_file *) offset;
		if (!cbfs_check_magic(file)) return NULL;
		printk(BIOS_SPEW, "Check %s\n", CBFS_NAME(file));
		if (!strcmp(CBFS_NAME(file), name))
			return file;

		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		printk(BIOS_SPEW, "CBFS: follow chain: %p + %x + %x + align -> ", (void *)offset, foffset, flen);

		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		printk(BIOS_SPEW, "%p\n", (void *)offset);
		if (offset <= oldoffset) return NULL;

		if (offset < 0xFFFFFFFF - ntohl(header->romsize))
			return NULL;
	}
}

void *cbfs_find_file(const char *name, int type)
{
	struct cbfs_file *file = cbfs_find(name);

	if (file == NULL) {
		printk(BIOS_INFO,  "CBFS:  Could not find file %s\n",
		       name);
		return NULL;
	}

	if (ntohl(file->type) != type) {
		printk(BIOS_INFO,  "CBFS:  File %s is of type %x instead of"
		       "type %x\n", name, file->type, type);

		return NULL;
	}

	return (void *) CBFS_SUBHEADER(file);
}

static inline int tohex4(unsigned int c)
{
	return (c<=9)?(c+'0'):(c-10+'a');
}

static void tohex16(unsigned int val, char* dest)
{
	dest[0]=tohex4(val>>12);
	dest[1]=tohex4((val>>8) & 0xf);
	dest[2]=tohex4((val>>4) & 0xf);
	dest[3]=tohex4(val & 0xf);
}

void *cbfs_load_optionrom(u16 vendor, u16 device, void * dest)
{
	char name[17]="pciXXXX,XXXX.rom";
	struct cbfs_optionrom *orom;
	u8 *src;

	tohex16(vendor, name+3);
	tohex16(device, name+8);

	orom = (struct cbfs_optionrom *)
		cbfs_find_file(name, CBFS_TYPE_OPTIONROM);

	if (orom == NULL)
		return NULL;

	/* They might have specified a dest address. If so, we can decompress. 
	 * If not, there's not much hope of decompressing or relocating the rom.
	 * in the common case, the expansion rom is uncompressed, we
	 * pass 0 in for the dest, and all we have to do is find the rom and 
	 * return a pointer to it. 
 	 */

	/* BUG: the cbfstool is (not yet) including a cbfs_optionrom header */
	src = ((unsigned char *) orom); // + sizeof(struct cbfs_optionrom);

	if (! dest)
		return src;

	if (cbfs_decompress(ntohl(orom->compression),
			     src,
			     dest,
			     ntohl(orom->len)))
		return NULL;

	return dest;
}

void * cbfs_load_stage(const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_find_file(name, CBFS_TYPE_STAGE);
	/* this is a mess. There is no ntohll. */
	/* for now, assume compatible byte order until we solve this. */
	u32 entry;

	if (stage == NULL)
		return (void *) -1;

	printk(BIOS_INFO, "Stage: loading %s @ 0x%x (%d bytes), entry @ 0x%llx\n", 
			name,
			(u32) stage->load, stage->memlen, 
			stage->entry);
	memset((void *) (u32) stage->load, 0, stage->memlen);

	if (cbfs_decompress(stage->compression,
			     ((unsigned char *) stage) +
			     sizeof(struct cbfs_stage),
			     (void *) (u32) stage->load,
			     stage->len))
		return (void *) -1;

	printk(BIOS_DEBUG, "Stage: done loading.\n");

	entry = stage->entry;
	// entry = ntohl((u32) stage->entry);

	return (void *) entry;
}

int cbfs_execute_stage(const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_find_file(name, CBFS_TYPE_STAGE);

	if (stage == NULL)
		return 1;

	if (ntohl(stage->compression) != CBFS_COMPRESS_NONE) {
		printk(BIOS_INFO,  "CBFS:  Unable to run %s:  Compressed file"
		       "Not supported for in-place execution\n", name);
		return 1;
	}

	/* FIXME: This isn't right */
	printk(BIOS_INFO,  "CBFS: run @ %p\n", (void *) ntohl((u32) stage->entry));
	return run_address((void *) ntohl((u32) stage->entry));
}

/**
 * run_address is passed the address of a function taking no parameters and
 * jumps to it, returning the result. 
 * @param f the address to call as a function. 
 * @return value returned by the function. 
 */

int run_address(void *f)
{
	int (*v) (void);
	v = f;
	return v();
}

