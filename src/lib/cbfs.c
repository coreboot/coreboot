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


#include <program_loading.h>
#include "cbfs_core.h"

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
	  cbfs_get_file_content(media, name, CBFS_TYPE_OPTIONROM, NULL);

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

static int cbfs_load_prog_stage_by_offset(struct cbfs_media *media,
					struct prog *prog, ssize_t offset)
{
	struct cbfs_stage stage;
	struct cbfs_media backing_store;

	if (init_backing_media(&media, &backing_store))
		return -1;

	if (cbfs_read(media, &stage, offset, sizeof(stage)) != sizeof(stage)) {
		ERROR("ERROR: failed to read stage header\n");
		return -1;
	}

	LOG("loading stage @ 0x%llx (%d bytes), entry @ 0x%llx\n",
	    stage.load, stage.memlen, stage.entry);

	/* Stages rely the below clearing so that the bss is initialized. */
	memset((void *)(uintptr_t)stage.load, 0, stage.memlen);

	if (stage.compression == CBFS_COMPRESS_NONE) {
		if (cbfs_read(media, (void *)(uintptr_t)stage.load,
			      offset + sizeof(stage), stage.len) != stage.len) {
			ERROR("ERROR: Reading stage failed.\n");
			return -1;
		}
	} else {
		void *data = media->map(media, offset + sizeof(stage),
					stage.len);
		if (data == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
			ERROR("ERROR: Mapping stage failed.\n");
			return -1;
		}
		if (!cbfs_decompress(stage.compression, data,
				    (void *)(uintptr_t)stage.load, stage.len))
			return -1;
		media->unmap(media, data);
	}

	arch_segment_loaded(stage.load, stage.memlen, SEG_FINAL);
	DEBUG("stage loaded\n");

	prog_set_area(prog, (void *)(uintptr_t)stage.load, stage.memlen);
	prog_set_entry(prog, (void *)(uintptr_t)stage.entry, NULL);

	return 0;
}

int cbfs_load_prog_stage(struct cbfs_media *media, struct prog *prog)
{
	struct cbfs_file file;
	ssize_t offset;
	struct cbfs_media backing_store;

	if (init_backing_media(&media, &backing_store))
		return -1;

	offset = cbfs_locate_file(media, &file, prog->name);
	if (offset < 0 || file.type != CBFS_TYPE_STAGE)
		return -1;

	if (cbfs_load_prog_stage_by_offset(media, prog, offset) < 0)
		return -1;

	return 0;
}

void *cbfs_load_stage_by_offset(struct cbfs_media *media, ssize_t offset)
{
	struct prog prog = {
		.name = NULL,
	};

	if (cbfs_load_prog_stage_by_offset(media, &prog, offset) < 0)
		return (void *)-1;

	return prog_entry(&prog);
}

void *cbfs_load_stage(struct cbfs_media *media, const char *name)
{
	struct prog prog = {
		.name = name,
	};

	if (cbfs_load_prog_stage(media, &prog) < 0)
		return (void *)-1;

	return prog_entry(&prog);
}

/* Simple buffer */

void *cbfs_simple_buffer_map(struct cbfs_simple_buffer *buffer,
			     struct cbfs_media *media,
			     size_t offset, size_t count) {
	void *address = buffer->buffer + buffer->allocated;
	DEBUG("simple_buffer_map(offset=%zd, count=%zd): "
	      "allocated=%zd, size=%zd, last_allocate=%zd\n",
	    offset, count, buffer->allocated, buffer->size,
	    buffer->last_allocate);
	if (buffer->allocated + count > buffer->size)
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

#endif
