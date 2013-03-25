/*
 * This file is part of the libpayload project.
 *
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
#include <cbfs.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Implementation of a media source based on given memory buffer.
struct ram_media {
	char *start;
	size_t size;
};

static int ram_open(struct cbfs_media *media) {
	return 0;
}

static void *ram_map(struct cbfs_media *media, size_t offset, size_t count) {
	struct ram_media *m = (struct ram_media*)media->context;
	/* assume addressing from top of image in this case */
	if (offset > 0xf0000000) {
		offset = m->size + offset;
	}
	if (offset + count > m->size) {
		printf("ERROR: ram_map: request out of range (0x%zx+0x%zx)\n",
		       offset, count);
		return NULL;
	}
	return (void*)(m->start + offset);
}

static void *ram_unmap(struct cbfs_media *media, const void *address) {
	return NULL;
}

static size_t ram_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	void *ptr = ram_map(media, offset, count);
	memcpy(dest, ptr, count);
	ram_unmap(media, ptr);
	return count;
}

static int ram_close(struct cbfs_media *media) {
	return 0;
}

int init_cbfs_ram_media(struct cbfs_media *media, void *start, size_t size);
int init_cbfs_ram_media(struct cbfs_media *media, void *start, size_t size) {
	// TODO Find a way to release unused media. Maybe adding media->destroy.
	struct ram_media *m = (struct ram_media*)malloc(sizeof(*m));
	m->start = start;
	m->size = size;
	media->context = (void*)m;
	media->open = ram_open;
	media->close = ram_close;
	media->map = ram_map;
	media->unmap = ram_unmap;
	media->read = ram_read;
	return 0;
}

// Legacy setup_cbfs_from_*.
static int is_default_cbfs_media_initialized;
static struct cbfs_media default_cbfs_media;

int setup_cbfs_from_ram(void *start, uint32_t size);
int setup_cbfs_from_ram(void *start, uint32_t size) {
	int result = init_cbfs_ram_media(&default_cbfs_media, start, size);
	if (result == 0)
		is_default_cbfs_media_initialized = 1;
	return result;
}

extern int libpayload_init_default_cbfs_media(struct cbfs_media *media);
int setup_cbfs_from_flash(void);
int setup_cbfs_from_flash(void) {
	int result = libpayload_init_default_cbfs_media(&default_cbfs_media);
	if (result == 0)
	    is_default_cbfs_media_initialized = 1;
	return result;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	int result = 0;
	if (is_default_cbfs_media_initialized != 1) {
		result = setup_cbfs_from_flash();
	}
	if (result == 0)
		memcpy(media, &default_cbfs_media, sizeof(*media));
	return result;
}
