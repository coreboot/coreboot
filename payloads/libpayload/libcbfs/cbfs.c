/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2011 secunet Security Networks AG
 * Copyright (C) 2013 Google, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#define LIBPAYLOAD

#ifdef LIBPAYLOAD
# include <libpayload-config.h>
# if IS_ENABLED(CONFIG_LP_LZMA)
#  include <lzma.h>
#  define CBFS_CORE_WITH_LZMA
# endif
# if IS_ENABLED(CONFIG_LP_LZ4)
#  include <lz4.h>
#  define CBFS_CORE_WITH_LZ4
# endif
# define CBFS_MINI_BUILD
#elif defined(__SMM__)
# define CBFS_MINI_BUILD
#else
# define CBFS_CORE_WITH_LZMA
# define CBFS_CORE_WITH_LZ4
# include <lib.h>
#endif

#include <cbfs.h>
#include <string.h>

#ifdef LIBPAYLOAD
# include <stdio.h>
# define DEBUG(x...)
# define LOG(x...)
# define ERROR(x...) printf(x)
#else
# include <console/console.h>
# define ERROR(x...) printk(BIOS_ERR, "CBFS: " x)
# define LOG(x...) printk(BIOS_INFO, "CBFS: " x)
# if CONFIG_LP_DEBUG_CBFS
#  define DEBUG(x...) printk(BIOS_SPEW, "CBFS: " x)
# else
#  define DEBUG(x...)
# endif
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
			  uint16_t device)
{
	char name[17] = "pciXXXX,XXXX.rom";

	tohex16(vendor, name+3);
	tohex16(device, name+8);

	return cbfs_get_file_content(media, name, CBFS_TYPE_OPTIONROM, NULL);
}

void * cbfs_load_stage(struct cbfs_media *media, const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_get_file_content(media, name, CBFS_TYPE_STAGE, NULL);
	/* this is a mess. There is no ntohll. */
	/* for now, assume compatible byte order until we solve this. */
	uintptr_t entry;
	uint32_t final_size;

	if (stage == NULL)
		return (void *) -1;

	LOG("loading stage %s @ 0x%p (%d bytes), entry @ 0x%llx\n",
			name,
			(void*)(uintptr_t) stage->load, stage->memlen,
			stage->entry);

	final_size = cbfs_decompress(stage->compression,
				     ((unsigned char *) stage) +
				     sizeof(struct cbfs_stage),
				     (void *) (uintptr_t) stage->load,
				     stage->len);
	if (!final_size) {
		entry = -1;
		goto out;
	}

	memset((void *)((uintptr_t)stage->load + final_size), 0,
	       stage->memlen - final_size);

	DEBUG("stage loaded.\n");

	entry = stage->entry;
	// entry = ntohll(stage->entry);

out:
	free(stage);
	return (void *) entry;
}

int cbfs_execute_stage(struct cbfs_media *media, const char *name)
{
	struct cbfs_stage *stage = (struct cbfs_stage *)
		cbfs_get_file_content(media, name, CBFS_TYPE_STAGE, NULL);

	if (stage == NULL)
		return 1;

	if (ntohl(stage->compression) != CBFS_COMPRESS_NONE) {
		LOG("Unable to run %s:  Compressed file"
		       "Not supported for in-place execution\n", name);
		free(stage);
		return 1;
	}

	LOG("run @ %p\n", (void *) (uintptr_t)ntohll(stage->entry));
	int result = run_address((void *)(uintptr_t)ntohll(stage->entry));
	free(stage);
	return result;
}

void *cbfs_load_payload(struct cbfs_media *media, const char *name)
{
	return (struct cbfs_payload *)cbfs_get_file_content(
		media, name, CBFS_TYPE_SELF, NULL);
}

struct cbfs_file *cbfs_find(const char *name) {
	struct cbfs_handle *handle = cbfs_get_handle(CBFS_DEFAULT_MEDIA, name);
	struct cbfs_media *m = &handle->media;
	void *ret;

	if (!handle)
		return NULL;

	ret = m->map(m, handle->media_offset,
		     handle->content_offset + handle->content_size);
	if (ret == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		free(handle);
		return NULL;
	}

	free(handle);
	return ret;
}

void *cbfs_find_file(const char *name, int type) {
	return cbfs_get_file_content(CBFS_DEFAULT_MEDIA, name, type, NULL);
}

const struct cbfs_header *get_cbfs_header(void) {
	return cbfs_get_header(CBFS_DEFAULT_MEDIA);
}

/* Simple buffer */

void *cbfs_simple_buffer_map(struct cbfs_simple_buffer *buffer,
			     struct cbfs_media *media,
			     size_t offset, size_t count) {
	void *address = buffer->buffer + buffer->allocated;;
	DEBUG("simple_buffer_map(offset=%zu, count=%zu): "
	      "allocated=%zu, size=%zu, last_allocate=%zu\n",
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
	      "allocated=%zu, size=%zu, last_allocate=%zu\n",
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
