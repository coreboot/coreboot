/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008, Jordan Crouse <jordan@cosmicpenguin.net>
 * Copyright (C) 2013 The Chromium OS Authors. All rights reserved.
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

#ifdef LIBPAYLOAD
# include <libpayload-config.h>
# ifdef CONFIG_LZMA
#  include <lzma.h>
#  define CBFS_CORE_WITH_LZMA
# endif
# define CBFS_MINI_BUILD
#elif defined(__SMM__)
# define CBFS_MINI_BUILD
#else
# define CBFS_CORE_WITH_LZMA
# include <lib.h>
#endif

#include <cbfs.h>
#include <string.h>

#ifdef LIBPAYLOAD
# include <stdio.h>
# define DEBUG(x...)
# define LOG(x...) printf(x)
# define ERROR(x...) printf(x)
#else
# include <console/console.h>
# define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
# define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
# if CONFIG_DEBUG_CBFS
#  define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
# else
#  define DEBUG(x...)
# endif
#endif

#if defined(CONFIG_CBFS_HEADER_ROM_OFFSET) && (CONFIG_CBFS_HEADER_ROM_OFFSET)
# define CBFS_HEADER_ROM_ADDRESS (CONFIG_CBFS_HEADER_ROM_OFFSET)
#else
// Indirect address: only works on 32bit top-aligned systems.
# define CBFS_HEADER_ROM_ADDRESS (*(uint32_t *)0xfffffffc)
#endif

#include "cbfs_core.c"

#ifndef __SMM__
static inline int tohex4(unsigned int c)
{
	return (c <= 9) ? (c + '0') : (c - 10 + 'a');
}

static void tohex16(unsigned int val, char* dest)
{
	dest[0] = tohex4(val>>12);
	dest[1] = tohex4((val>>8) & 0xf);
	dest[2] = tohex4((val>>4) & 0xf);
	dest[3] = tohex4(val & 0xf);
}

void *cbfs_load_optionrom(struct cbfs_media *media, uint16_t vendor,
			  uint16_t device, void *dest)
{
	char name[17] = "pciXXXX,XXXX.rom";
	struct cbfs_optionrom *orom;
	uint8_t *src;

	tohex16(vendor, name+3);
	tohex16(device, name+8);

	orom = (struct cbfs_optionrom *)
		cbfs_get_file_content(media, name, CBFS_TYPE_OPTIONROM);

	if (orom == NULL)
		return NULL;

	/* They might have specified a dest address. If so, we can decompress.
	 * If not, there's not much hope of decompressing or relocating the rom.
	 * in the common case, the expansion rom is uncompressed, we
	 * pass 0 in for the dest, and all we have to do is find the rom and
	 * return a pointer to it.
	 */

	/* BUG: the cbfstool is (not yet) including a cbfs_optionrom header */
	src = (uint8_t *)orom; // + sizeof(struct cbfs_optionrom);

	if (! dest)
		return src;

	if (cbfs_decompress(ntohl(orom->compression),
			     src,
			     dest,
			     ntohl(orom->len)))
		return NULL;

	return dest;
}

void * cbfs_load_stage(struct cbfs_media *media, const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_get_file_content(media, name, CBFS_TYPE_STAGE);
	/* this is a mess. There is no ntohll. */
	/* for now, assume compatible byte order until we solve this. */
	uint32_t entry;

	if (stage == NULL)
		return (void *) -1;

	LOG("loading stage %s @ 0x%x (%d bytes), entry @ 0x%llx\n",
			name,
			(uint32_t) stage->load, stage->memlen,
			stage->entry);
	memset((void *) (uint32_t) stage->load, 0, stage->memlen);

	if (cbfs_decompress(stage->compression,
			     ((unsigned char *) stage) +
			     sizeof(struct cbfs_stage),
			     (void *) (uint32_t) stage->load,
			     stage->len))
		return (void *) -1;

	DEBUG("stage loaded.\n");

	entry = stage->entry;
	// entry = ntohll(stage->entry);

	return (void *) entry;
}

int cbfs_execute_stage(struct cbfs_media *media, const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_get_file_content(media, name, CBFS_TYPE_STAGE);

	if (stage == NULL)
		return 1;

	if (ntohl(stage->compression) != CBFS_COMPRESS_NONE) {
		LOG("Unable to run %s:  Compressed file"
		       "Not supported for in-place execution\n", name);
		return 1;
	}

	/* FIXME: This isn't right */
	LOG("run @ %p\n", (void *) ntohl((uint32_t) stage->entry));
	return run_address((void *)(uintptr_t)ntohll(stage->entry));
}

void *cbfs_load_payload(struct cbfs_media *media, const char *name)
{
	return (struct cbfs_payload *)cbfs_get_file_content(
			media, name, CBFS_TYPE_PAYLOAD);
}

/* Simple buffer */

void *cbfs_simple_buffer_map(struct cbfs_simple_buffer *buffer,
			     struct cbfs_media *media,
			     size_t offset, size_t count) {
	void *address = buffer->buffer + buffer->allocated;;
	DEBUG("simple_buffer_map(offset=%d, count=%d): "
	      "allocated=%d, size=%d, last_allocate=%d\n",
	    offset, count, buffer->allocated, buffer->size,
	    buffer->last_allocate);
	if (buffer->allocated + count >= buffer->size)
		return CBFS_MEDIA_INVALID_MAP_ADDRESS;
	if (media->read(media, address, offset, count) != count) {
		ERROR("simple_buffer: fail to read %zd bytes from 0x%zx\n",
		      count, offset);
		return CBFS_MEDIA_INVALID_MAP_ADDRESS;
	}
	buffer->allocated += count;
	buffer->last_allocate = count;
	return address;
}

void *cbfs_simple_buffer_unmap(struct cbfs_simple_buffer *buffer,
			       const void *address) {
	// TODO Add simple buffer management so we can free more than last
	// allocated one.
	DEBUG("simple_buffer_unmap(address=0x%p): "
	      "allocated=%d, size=%d, last_allocate=%d\n",
	    address, buffer->allocated, buffer->size,
	    buffer->last_allocate);
	if ((buffer->buffer + buffer->allocated - buffer->last_allocate) ==
	    address) {
		buffer->allocated -= buffer->last_allocate;
		buffer->last_allocate = 0;
	}
	return NULL;
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
