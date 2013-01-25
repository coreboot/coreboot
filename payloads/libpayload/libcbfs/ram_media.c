/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2013 The Chromium OS Authors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
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
	if (offset + count >= m->size) {
		printf("ERROR: ram_map: request out of range (0x%x+0x%x)\n",
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
