/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 secunet Security Networks AG
 * Copyright (C) 2013 The Chromium OS Authors. All rights reserved.
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

/* The CBFS core requires a couple of #defines or functions to adapt it to the
 * target environment:
 *
 * CBFS_CORE_WITH_LZMA (must be #define)
 *      if defined, ulzma() must exist for decompression of data streams
 *
 * ERROR(x...)
 *      print an error message x (in printf format)
 *
 * LOG(x...)
 *      print a message x (in printf format)
 *
 * DEBUG(x...)
 *      print a debug message x (in printf format)
 *
 */

#include <cbfs.h>
#include <string.h>
#include <symbols.h>

#if IS_ENABLED(CONFIG_MULTIPLE_CBFS_INSTANCES)
void cbfs_set_header_offset(size_t offset)
{
	_cbfs_header_offset[0] = offset;
	LOG("header set to: %#zx\n", offset);
}

static size_t get_header_offset(void)
{
	return _cbfs_header_offset[0];
}
#else
static size_t get_header_offset(void)
{
	return 0;
}
#endif

#include "cbfs_core.h"

/* returns a pointer to CBFS master header, or CBFS_HEADER_INVALID_ADDRESS
 *  on failure */
const struct cbfs_header *cbfs_get_header(struct cbfs_media *media)
{
	size_t offset;
	const struct cbfs_header *header;
	struct cbfs_media default_media;

	if (media == CBFS_DEFAULT_MEDIA) {
		media = &default_media;
		if (init_default_cbfs_media(media) != 0) {
			ERROR("Failed to initialize default media.\n");
			return CBFS_HEADER_INVALID_ADDRESS;
		}
	}
	media->open(media);

	/* TODO: allow negative offsets from the end of the CBFS image at media
	 * layer (like libpayload) so we can combine these two cases. */
	if (IS_ENABLED(CONFIG_ARCH_X86)) {
		offset = *(int32_t *)(uintptr_t)0xfffffffc;
		header = media->map(media, offset, sizeof(*header));
	} else {

		offset = get_header_offset();

		if (!offset) {
			int32_t rel_offset;
			size_t cbfs_top = CONFIG_CBFS_SIZE;
			DEBUG("CBFS top at offset: 0x%zx\n", cbfs_top);
			if (!media->read(media, &rel_offset, cbfs_top -
					 sizeof(int32_t),
					 sizeof(int32_t))) {
				ERROR("Could not read master header offset!\n");
				media->close(media);
				return CBFS_HEADER_INVALID_ADDRESS;
			}
			offset = cbfs_top + rel_offset;
		}
		header = media->map(media, offset, sizeof(*header));
	}
	DEBUG("CBFS header offset: 0x%zx/0x%x\n", offset, CONFIG_ROM_SIZE);
	media->close(media);

	if (header == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		ERROR("Failed to load CBFS header from 0x%zx\n", offset);
		return CBFS_HEADER_INVALID_ADDRESS;
	}

	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		ERROR("Could not find valid CBFS master header at %#zx: "
		      "magic %#.8x vs %#.8x.\n", offset, CBFS_HEADER_MAGIC,
		      ntohl(header->magic));
		if (header->magic == 0xffffffff) {
			ERROR("Maybe ROM is not mapped properly?\n");
		}
		return CBFS_HEADER_INVALID_ADDRESS;
	}
	return header;
}


int init_backing_media(struct cbfs_media **media, struct cbfs_media *backing)
{
	if (*media == CBFS_DEFAULT_MEDIA) {
		*media = backing;
		if (init_default_cbfs_media(*media) != 0) {
			ERROR("Failed to initialize default media.\n");
			return -1;
		}
	}
	return 0;
}

/* public API starts here*/
ssize_t cbfs_locate_file(struct cbfs_media *media, struct cbfs_file *file,
				const char *name)
{
	const char *file_name;
	uint32_t offset, align, romsize, name_len;
	const struct cbfs_header *header;
	struct cbfs_media default_media;

	if (init_backing_media(&media, &default_media))
		return -1;

	if (CBFS_HEADER_INVALID_ADDRESS == (header = cbfs_get_header(media)))
		return -1;

	// Logical offset (for source media) of first file.
	offset = ntohl(header->offset);
	align = ntohl(header->align);
	romsize = ntohl(header->romsize);

	// TODO Add a "size" in CBFS header for a platform independent way to
	// determine the end of CBFS data.
#if defined(CONFIG_ARCH_X86) && CONFIG_ARCH_X86
	// resolve actual length of ROM used for CBFS components
	// the bootblock size was not taken into account
	romsize -= ntohl(header->bootblocksize);

	// fine tune the length to handle alignment positioning.
	// using (bootblock size) % align, to derive the
	// number of bytes the bootblock is off from the alignment size.
	if ((ntohl(header->bootblocksize) % align))
		romsize -= (align - (ntohl(header->bootblocksize) % align));
	else
		romsize -= 1;
#endif

	DEBUG("CBFS location: 0x%x~0x%x, align: %d\n", offset, romsize, align);
	DEBUG("Looking for '%s' starting from 0x%x.\n", name, offset);

	media->open(media);
	while (offset < romsize &&
	       media->read(media, file, offset, sizeof(*file)) == sizeof(*file)) {
		if (memcmp(CBFS_FILE_MAGIC, file->magic,
			   sizeof(file->magic)) != 0) {
			uint32_t new_align = align;
			if (offset % align)
				new_align += align - (offset % align);
			LOG("WARNING: No file header found at 0x%x - "
			      "try next aligned address: 0x%x.\n", offset,
			      offset + new_align);
			offset += new_align;
			continue;
		}

		file->len = ntohl(file->len);
		file->type= ntohl(file->type);
		file->offset = ntohl(file->offset);

		name_len = file->offset - sizeof(*file);
		DEBUG(" - load entry 0x%x file name (%d bytes)...\n", offset,
		      name_len);

		// load file name (arbitrary length).
		file_name = (const char *)media->map(
				media, offset + sizeof(*file), name_len);
		if (file_name == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
			ERROR("ERROR: Failed to get filename: 0x%x.\n", offset);
		} else if (strcmp(file_name, name) == 0) {
			DEBUG("Found file (offset=0x%x, len=%d).\n",
			    offset + file->offset, file->len);
			media->unmap(media, file_name);
			return offset + file->offset;
		} else {
			DEBUG(" (unmatched file @0x%x: %s)\n", offset,
			      file_name);
			media->unmap(media, file_name);
		}

		// Move to next file.
		offset += file->len + file->offset;
		if (offset % align)
			offset += align - (offset % align);
	}
	media->close(media);
	LOG("WARNING: '%s' not found.\n", name);
	return -1;
}

size_t cbfs_read(struct cbfs_media *media, void *dest, size_t offset,
			size_t count)
{
	struct cbfs_media default_media;
	size_t nread;

	if (init_backing_media(&media, &default_media))
		return 0;

	media->open(media);
	nread = media->read(media, dest, offset, count);
	media->close(media);

	return nread;
}

struct cbfs_file *cbfs_get_file(struct cbfs_media *media, const char *name)
{
	struct cbfs_media default_media;
	struct cbfs_file file, *file_ptr;
	ssize_t offset;

	if (init_backing_media(&media, &default_media))
		return NULL;

	offset = cbfs_locate_file(media, &file, name);
	if (offset < 0)
		return NULL;

	/* Map both the metadata and the file contents. */
	media->open(media);
	offset -= file.offset;
	file_ptr = media->map(media, offset, file.offset + file.len);
	media->close(media);

	if (file_ptr == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		ERROR("ERROR: Mapping %s failed.\n", name);
		return NULL;
	}

	return file_ptr;
}

void *cbfs_get_file_content(struct cbfs_media *media, const char *name,
			    int type, size_t *sz)
{
	struct cbfs_file *file = cbfs_get_file(media, name);

	if (sz)
		*sz = 0;

	if (file == NULL) {
		ERROR("Could not find file '%s'.\n", name);
		return NULL;
	}

	if (ntohl(file->type) != type) {
		ERROR("File '%s' is of type %x, but we requested %x.\n", name,
		      ntohl(file->type), type);
		return NULL;
	}

	if (sz)
		*sz = ntohl(file->len);

	return (void *)CBFS_SUBHEADER(file);
}

int cbfs_decompress(int algo, void *src, void *dst, int len)
{
	switch (algo) {
		case CBFS_COMPRESS_NONE:
			/* Reads need to be aligned at 4 bytes to avoid
			   poor flash performance.  */
			while (len && ((uintptr_t)src & 3)) {
				*(u8*)dst++ = *(u8*)src++;
				len--;
			}
			memmove(dst, src, len);
			return len;
#ifdef CBFS_CORE_WITH_LZMA
		case CBFS_COMPRESS_LZMA:
			return ulzma(src, dst);
#endif
		default:
			ERROR("tried to decompress %d bytes with algorithm #%x,"
			      "but that algorithm id is unsupported.\n", len,
			      algo);
			return 0;
	}
}
