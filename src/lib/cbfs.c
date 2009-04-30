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

#ifndef CONFIG_BIG_ENDIAN
#define ntohl(x) ( ((x&0xff)<<24) | ((x&0xff00)<<8) | \
		((x&0xff0000) >> 8) | ((x&0xff000000) >> 24) )
#else
#define ntohl(x) (x)
#endif

int run_address(void *f);

int cbfs_decompress(int algo, void *src, void *dst, int len)
{
	switch(algo) {
	case CBFS_COMPRESS_NONE:
		memcpy(dst, src, len);
		return 0;

#if CONFIG_COMPRESSED_PAYLOAD_LZMA==1

	case CBFS_COMPRESS_LZMA: {
		unsigned long ulzma(unsigned char *src, unsigned char *dst);
		ulzma(src, dst);
	}
		return 0;
#endif

#if CONFIG_COMPRESSED_PAYLOAD_NRV2B==1
	case CBFS_COMPRESS_NRV2B: {
		unsigned long unrv2b(u8 *src, u8 *dst, unsigned long *ilen_p);
		unsigned long tmp;

		unrv2b(src, dst, &tmp);
	}
		return 0;
#endif
	default:
		printk_info( "CBFS:  Unknown compression type %d\n",
		       algo);
		return -1;
	}
}

int cbfs_check_magic(struct cbfs_file *file)
{
	return !strcmp(file->magic, CBFS_FILE_MAGIC) ? 1 : 0;
}

struct cbfs_header *cbfs_master_header(void)
{
	struct cbfs_header *header;

	void *ptr = (void *)*((unsigned long *) CBFS_HEADPTR_ADDR);
	printk_debug("Check CBFS header at %p\n", ptr);
	header = (struct cbfs_header *) ptr;

	printk_debug("magic is %08x\n", ntohl(header->magic));
	if (ntohl(header->magic) != CBFS_HEADER_MAGIC) {
		printk_err("NO CBFS HEADER\n");
		return NULL;
	}

	printk_debug("Found CBFS header at %p\n", ptr);
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
		printk_info("Check %s\n", CBFS_NAME(file));
		if (!strcmp(CBFS_NAME(file), name))
			return file;

		int flen = ntohl(file->len);
		int foffset = ntohl(file->offset);
		printk_spew("CBFS: follow chain: %p + %x + %x + align -> ", offset, foffset, flen);

		unsigned long oldoffset = offset;
		offset = ALIGN(offset + foffset + flen, align);
		printk_spew("%p\n", offset);
		if (offset <= oldoffset) return NULL;

		if (offset < 0xFFFFFFFF - ntohl(header->romsize))
			return NULL;
	}
}

struct cbfs_stage *cbfs_find_file(const char *name, int type)
{
	struct cbfs_file *file = cbfs_find(name);

	if (file == NULL) {
		printk_info( "CBFS:  Could not find file %s\n",
		       name);
		return NULL;
	}

	if (ntohl(file->type) != type) {
		printk_info( "CBFS:  File %s is of type %x instead of"
		       "type %x\n", name, file->type, type);

		return NULL;
	}

	return (void *) CBFS_SUBHEADER(file);
}

void *cbfs_load_optionrom(u16 vendor, u16 device, void * dest)
{
	char name[17];
	struct cbfs_optionrom *orom;
	u8 *src;

	sprintf(name,"pci%04x,%04x.rom", vendor, device);

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

void * cbfs_load_payload(struct lb_memory *lb_mem, const char *name)
{
	int selfboot(struct lb_memory *mem, struct cbfs_payload *payload);
	struct cbfs_payload *payload = (struct cbfs_payload *)
		cbfs_find_file(name, CBFS_TYPE_PAYLOAD);

	struct cbfs_payload_segment *segment, *first_segment;

	if (payload == NULL)
		return (void *) -1;
	printk_debug("Got a payload\n");
	first_segment = segment = &payload->segments;
	selfboot(lb_mem, payload);
	printk_emerg("SELFBOOT RETURNED!\n");

	return (void *) -1;
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

	printk_info("Stage: load @ %d/%d bytes, enter @ %llx\n", 
			ntohl((u32) stage->load), ntohl(stage->memlen), 
			stage->entry);
	memset((void *) ntohl((u32) stage->load), 0, ntohl(stage->memlen));

	if (cbfs_decompress(ntohl(stage->compression),
			     ((unsigned char *) stage) +
			     sizeof(struct cbfs_stage),
			     (void *) ntohl((u32) stage->load),
			     ntohl(stage->len)))
		return (void *) -1;

	entry = stage->entry;
//	return (void *) ntohl((u32) stage->entry);
	return (void *) entry;
}

void * cbfs_get_file(const char *name)
{
	return cbfs_find(name);
}

int cbfs_execute_stage(const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_find_file(name, CBFS_TYPE_STAGE);

	if (stage == NULL)
		return 1;

	if (ntohl(stage->compression) != CBFS_COMPRESS_NONE) {
		printk_info( "CBFS:  Unable to run %s:  Compressed file"
		       "Not supported for in-place execution\n", name);
		return 1;
	}

	/* FIXME: This isn't right */
	printk_info( "CBFS: run @ %p\n", (void *) ntohl((u32) stage->entry));
	return run_address((void *) ntohl((u32) stage->entry));
}

/**
 * run_address is passed the address of a function taking no parameters and
 * jumps to it, returning the result. 
 * @param f the address to call as a function. 
 * returns value returned by the function. 
 */

int run_address(void *f)
{
	int (*v) (void);
	v = f;
	return v();
}

