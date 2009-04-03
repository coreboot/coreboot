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
#include <boot/coreboot_tables.h>
#include <romfs.h>

#ifndef CONFIG_BIG_ENDIAN
#define ntohl(x) ( ((x&0xff)<<24) | ((x&0xff00)<<8) | \
		((x&0xff0000) >> 8) | ((x&0xff000000) >> 24) )
#else
#define ntohl(x) (x)
#endif

int run_address(void *f);

int romfs_decompress(int algo, void *src, void *dst, int len)
{
	switch(algo) {
	case ROMFS_COMPRESS_NONE:
		memcpy(dst, src, len);
		return 0;

#ifdef CONFIG_COMPRESSION_LZMA

	case ROMFS_COMPRESS_LZMA: {
		unsigned long ulzma(unsigned char *src, unsigned char *dst);
		ulzma(src, dst);
	}
		return 0;
#endif

#ifdef CONFIG_COMPRESSION_NRV2B
	case ROMFS_COMPRESS_NRV2B: {
		unsigned long unrv2b(u8 *src, u8 *dst, unsigned long *ilen_p);
		unsigned long tmp;

		unrv2b(src, dst, &tmp);
	}
		return 0;
#endif
	default:
		printk_info( "ROMFS:  Unknown compression type %d\n",
		       algo);
		return -1;
	}
}

int romfs_check_magic(struct romfs_file *file)
{
	return !strcmp(file->magic, ROMFS_FILE_MAGIC) ? 1 : 0;
}

struct romfs_header *romfs_master_header(void)
{
	struct romfs_header *header;

	void *ptr = (void *)*((unsigned long *) ROMFS_HEADPTR_ADDR);
	printk_debug("Check ROMFS header at %p\n", ptr);
	header = (struct romfs_header *) ptr;

	printk_debug("magic is %08x\n", ntohl(header->magic));
	if (ntohl(header->magic) != ROMFS_HEADER_MAGIC) {
		printk_err("NO ROMFS HEADER\n");
		return NULL;
	}

	printk_debug("Found ROMFS header at %p\n", ptr);
	return header;
}

struct romfs_file *romfs_find(const char *name)
{
	struct romfs_header *header = romfs_master_header();
	unsigned long offset;

	if (header == NULL)
		return NULL;
	offset = 0 - ntohl(header->romsize) + ntohl(header->offset);

	while(1) {
		struct romfs_file *file = (struct romfs_file *) offset;
		if (romfs_check_magic(file)) printk_info("Check %s\n", ROMFS_NAME(file));
		if (romfs_check_magic(file) &&
		    !strcmp(ROMFS_NAME(file), name))
			return file;

		offset += ntohl(header->align);

		if (offset < 0xFFFFFFFF - ntohl(header->romsize))
			return NULL;
	}
}

struct romfs_stage *romfs_find_file(const char *name, int type)
{
	struct romfs_file *file = romfs_find(name);

	if (file == NULL) {
		printk_info( "ROMFS:  Could not find file %s\n",
		       name);
		return NULL;
	}

	if (ntohl(file->type) != type) {
		printk_info( "ROMFS:  File %s is of type %x instead of"
		       "type %x\n", name, file->type, type);

		return NULL;
	}

	return (void *) ROMFS_SUBHEADER(file);
}

void *romfs_load_optionrom(u16 vendor, u16 device, void * dest)
{
	char name[17];
	struct romfs_optionrom *orom;
	u8 *src;

	sprintf(name,"pci%04x,%04x.rom", vendor, device);

	orom = (struct romfs_optionrom *)
		romfs_find_file(name, ROMFS_TYPE_OPTIONROM);

	if (orom == NULL)
		return NULL;

	/* They might have specified a dest address. If so, we can decompress. 
	 * If not, there's not much hope of decompressing or relocating the rom.
	 * in the common case, the expansion rom is uncompressed, we
	 * pass 0 in for the dest, and all we have to do is find the rom and 
	 * return a pointer to it. 
 	 */

	src = ((unsigned char *) orom) + sizeof(struct romfs_optionrom);

	if (! dest)
		return src;

	if (romfs_decompress(ntohl(orom->compression),
			     src,
			     dest,
			     ntohl(orom->len)))
		return NULL;

	return dest;
}

void * romfs_load_payload(struct lb_memory *lb_mem, const char *name)
{
	int selfboot(struct lb_memory *mem, struct romfs_payload *payload);
	struct romfs_payload *payload = (struct romfs_payload *)
		romfs_find_file(name, ROMFS_TYPE_PAYLOAD);

	struct romfs_payload_segment *segment, *first_segment;

	if (payload == NULL)
		return (void *) -1;
	printk_debug("Got a payload\n");
	first_segment = segment = &payload->segments;
	selfboot(lb_mem, payload);
	printk_emerg("SELFBOOT RETURNED!\n");

	return (void *) -1;
}

void * romfs_load_stage(const char *name)
{
	struct romfs_stage *stage = (struct romfs_stage *)
		romfs_find_file(name, ROMFS_TYPE_STAGE);
	/* this is a mess. There is no ntohll. */
	/* for now, assume compatible byte order until we solve this. */
	u32 entry;

	if (stage == NULL)
		return (void *) -1;

	printk_info("Stage: load @ %d/%d bytes, enter @ %llx\n", 
			ntohl((u32) stage->load), ntohl(stage->memlen), 
			stage->entry);
	memset((void *) ntohl((u32) stage->load), 0, ntohl(stage->memlen));

	if (romfs_decompress(ntohl(stage->compression),
			     ((unsigned char *) stage) +
			     sizeof(struct romfs_stage),
			     (void *) ntohl((u32) stage->load),
			     ntohl(stage->len)))
		return (void *) -1;

	entry = stage->entry;
//	return (void *) ntohl((u32) stage->entry);
	return (void *) entry;
}

void * romfs_get_file(const char *name)
{
	return romfs_find(name);
}

int romfs_execute_stage(const char *name)
{
	struct romfs_stage *stage = (struct romfs_stage *)
		romfs_find_file(name, ROMFS_TYPE_STAGE);

	if (stage == NULL)
		return 1;

	if (ntohl(stage->compression) != ROMFS_COMPRESS_NONE) {
		printk_info( "ROMFS:  Unable to run %s:  Compressed file"
		       "Not supported for in-place execution\n", name);
		return 1;
	}

	/* FIXME: This isn't right */
	printk_info( "ROMFS: run @ %p\n", (void *) ntohl((u32) stage->entry));
	return run_address((void *) ntohl((u32) stage->entry));
}

/**
 *  * run_address is passed the address of a function taking no parameters and
 *   * jumps to it, returning the result. 
 *    * @param f the address to call as a function. 
 *     * returns value returned by the function. 
 *      */

int run_address(void *f)
{
        int (*v) (void);
        v = f;
        return v();
}

