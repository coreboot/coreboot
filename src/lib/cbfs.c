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
#elif defined(__BOOT_BLOCK__)
  /* No LZMA in boot block. */
#elif defined(__PRE_RAM__) && !CONFIG_COMPRESS_RAMSTAGE
  /* No LZMA in romstage if ramstage is not compressed. */
#else
# define CBFS_CORE_WITH_LZMA
# include <lib.h>
#endif

#include <cbfs.h>
#include <string.h>
#include <cbmem.h>

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

#if CONFIG_VBOOT_VERIFY_FIRMWARE
#include <vendorcode/google/chromeos/chromeos.h>
#else
static inline void *vboot_get_payload(int *len) { return NULL; }
#endif

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

	if (!cbfs_decompress(ntohl(orom->compression),
			     src,
			     dest,
			     ntohl(orom->len)))
		return NULL;

	return dest;
}

#if CONFIG_RELOCATABLE_RAMSTAGE && defined(__PRE_RAM__)

#include <rmodule.h>
#include <romstage_handoff.h>
/* When CONFIG_RELOCATABLE_RAMSTAGE is enabled and this file is being compiled
 * for the romstage, the rmodule loader is used.  */
void __attribute__((weak))
cache_loaded_ramstage(struct romstage_handoff *handoff,
                      const struct cbmem_entry *ramstage, void *entry_point)
{
	uint32_t ramstage_size;
	const struct cbmem_entry *entry;

	if (handoff == NULL)
		return;

	ramstage_size = cbmem_entry_size(ramstage);
	/* cbmem_entry_add() does a find() before add(). */
	entry = cbmem_entry_add(CBMEM_ID_RAMSTAGE_CACHE, ramstage_size);

	if (entry == NULL)
		return;

	/* Keep track of the entry point in the handoff structure. */
	handoff->ramstage_entry_point = (uint32_t)entry_point;

	memcpy(cbmem_entry_start(entry), cbmem_entry_start(ramstage),
	       ramstage_size);
}

void * __attribute__((weak))
load_cached_ramstage(struct romstage_handoff *handoff,
                     const struct cbmem_entry *ramstage)
{
	const struct cbmem_entry *entry_cache;

	if (handoff == NULL)
		return NULL;

	entry_cache = cbmem_entry_find(CBMEM_ID_RAMSTAGE_CACHE);

	if (entry_cache == NULL)
		return NULL;

	/* Load the cached ramstage copy into the to-be-run region. */
	memcpy(cbmem_entry_start(ramstage), cbmem_entry_start(entry_cache),
	       cbmem_entry_size(ramstage));

	return (void *)handoff->ramstage_entry_point;
}

static void *load_stage_from_cbfs(struct cbfs_media *media, const char *name,
                                  struct romstage_handoff *handoff)
{
	struct cbfs_stage *stage;
	struct rmodule ramstage;
	void *entry_point;
	size_t region_size;
	char *ramstage_region;
	int rmodule_offset;
	int load_offset;
	const struct cbmem_entry *ramstage_entry;

	stage = (struct cbfs_stage *)
		cbfs_get_file_content(media, name, CBFS_TYPE_STAGE);

	if (stage == NULL)
		return (void *) -1;

	rmodule_offset =
		rmodule_calc_region(DYN_CBMEM_ALIGN_SIZE,
	                            stage->memlen, &region_size, &load_offset);

	ramstage_entry = cbmem_entry_add(CBMEM_ID_RAMSTAGE, region_size);

	if (ramstage_entry == NULL)
		return (void *) -1;

	ramstage_region = cbmem_entry_start(ramstage_entry);

	LOG("Decompressing stage %s @ 0x%p (%d bytes)\n",
	    name, &ramstage_region[rmodule_offset], stage->memlen);

	if (!cbfs_decompress(stage->compression, &stage[1],
	                     &ramstage_region[rmodule_offset], stage->len))
		return (void *) -1;

	if (rmodule_parse(&ramstage_region[rmodule_offset], &ramstage))
		return (void *) -1;

	/* The ramstage is responsible for clearing its own bss. */
	if (rmodule_load(&ramstage_region[load_offset], &ramstage))
		return (void *) -1;

	entry_point = rmodule_entry(&ramstage);

	cache_loaded_ramstage(handoff, ramstage_entry, entry_point);

	return entry_point;
}

void * cbfs_load_stage(struct cbfs_media *media, const char *name)
{
	struct romstage_handoff *handoff;
	const struct cbmem_entry *ramstage;
	void *entry;

	handoff = romstage_handoff_find_or_add();

	if (handoff == NULL) {
		LOG("Couldn't find or allocate romstage handoff.\n");
		return load_stage_from_cbfs(media, name, handoff);
	} else if (!handoff->s3_resume)
		return load_stage_from_cbfs(media, name, handoff);

	ramstage = cbmem_entry_find(CBMEM_ID_RAMSTAGE);

	if (ramstage == NULL)
		return load_stage_from_cbfs(media, name, handoff);

	/* S3 resume path. Load a cached copy of the loaded ramstage. If
	 * return value is NULL load from cbfs. */
	entry = load_cached_ramstage(handoff, ramstage);
	if (entry == NULL)
		return load_stage_from_cbfs(media, name, handoff);

	return entry;
}

#else

void * cbfs_load_stage(struct cbfs_media *media, const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_get_file_content(media, name, CBFS_TYPE_STAGE);
	/* this is a mess. There is no ntohll. */
	/* for now, assume compatible byte order until we solve this. */
	uint32_t entry;
	uint32_t final_size;

	if (stage == NULL)
		return (void *) -1;

	LOG("loading stage %s @ 0x%x (%d bytes), entry @ 0x%llx\n",
			name,
			(uint32_t) stage->load, stage->memlen,
			stage->entry);

	final_size = cbfs_decompress(stage->compression,
				     ((unsigned char *) stage) +
				     sizeof(struct cbfs_stage),
				     (void *) (uint32_t) stage->load,
				     stage->len);
	if (!final_size)
		return (void *) -1;

	/* Stages rely the below clearing so that the bss is initialized. */
	memset((void *)((uintptr_t)stage->load + final_size), 0,
	       stage->memlen - final_size);

	DEBUG("stage loaded.\n");

	entry = stage->entry;
	// entry = ntohll(stage->entry);

	return (void *) entry;
}
#endif /* CONFIG_RELOCATABLE_RAMSTAGE */

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

#if !CONFIG_ALT_CBFS_LOAD_PAYLOAD
void *cbfs_load_payload(struct cbfs_media *media, const char *name)
{
	struct cbfs_payload *payload;

	payload = vboot_get_payload(NULL);
	if (payload != NULL)
		return payload;

	payload = (struct cbfs_payload *)cbfs_get_file_content(
			media, name, CBFS_TYPE_PAYLOAD);
	return payload;
}
#endif

/* Simple buffer */

void *cbfs_simple_buffer_map(struct cbfs_simple_buffer *buffer,
			     struct cbfs_media *media,
			     size_t offset, size_t count) {
	void *address = buffer->buffer + buffer->allocated;;
	DEBUG("simple_buffer_map(offset=%zd, count=%zd): "
	      "allocated=%zd, size=%zd, last_allocate=%zd\n",
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
	      "allocated=%zd, size=%zd, last_allocate=%zd\n",
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
