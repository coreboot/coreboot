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

// use CBFS core
#ifndef __SMM__
#define CBFS_CORE_WITH_LZMA
#endif
#define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
#define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
#if CONFIG_DEBUG_CBFS
#define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
#else
#define DEBUG(x...)
#endif

#ifdef CONFIG_CBFS_HEADER_ROM_OFFSET
# define CBFS_HEADER_ROM_ADDRESS (CONFIG_CBFS_HEADER_ROM_OFFSET)
#else
// Indirect ROM address
# define CBFS_HEADER_ROM_ADDRESS *(uint32_t*)0xfffffffc
#endif

#if CONFIG_CBFS_MEMORY_MAPPED
# define CBFS_MEMORY_MAPPED
#else
  // TODO make this configurable.
# define CBFS_CACHE_ADDRESS	CONFIG_CBFS_CACHE_ADDRESS
#endif

#include "cbfs_core.c"

#ifndef __SMM__
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

	LOG("loading stage %s @ 0x%x (%d bytes), entry @ 0x%llx\n",
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

	DEBUG("stage loaded.\n");

	entry = stage->entry;
	// entry = ntohll(stage->entry);

	return (void *) entry;
}

int cbfs_execute_stage(const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_find_file(name, CBFS_TYPE_STAGE);

	if (stage == NULL)
		return 1;

	if (ntohl(stage->compression) != CBFS_COMPRESS_NONE) {
		LOG("Unable to run %s:  Compressed file"
		       "Not supported for in-place execution\n", name);
		return 1;
	}

	/* FIXME: This isn't right */
	LOG("run @ %p\n", (void *) ntohl((u32) stage->entry));
	return run_address((void *) (intptr_t)ntohll(stage->entry));
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
#endif
