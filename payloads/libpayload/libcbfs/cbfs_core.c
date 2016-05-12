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

/* The CBFS core requires a couple of #defines or functions to adapt it to the
 * target environment:
 *
 * CBFS_CORE_WITH_LZMA (must be #define)
 *      if defined, ulzma() must exist for decompression of data streams
 *
 * CBFS_CORE_WITH_LZ4 (must be #define)
 *      if defined, ulz4f() must exist for decompression of data streams
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
#include <sysinfo.h>

/* returns a pointer to CBFS master header, or CBFS_HEADER_INVALID_ADDRESS
 *  on failure */
const struct cbfs_header *cbfs_get_header(struct cbfs_media *media)
{
	int32_t rel_offset;
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

	if (!media->read(media, &rel_offset, (size_t)(0 - sizeof(int32_t)),
			 sizeof(int32_t))) {
		ERROR("Could not read CBFS master header offset!\n");
		return CBFS_HEADER_INVALID_ADDRESS;
	}
	header = media->map(media, (size_t)rel_offset, sizeof(*header));
	DEBUG("CBFS header at %#zx (-%#zx from end of image).\n",
		(size_t)rel_offset, (size_t)-rel_offset);
	media->close(media);

	if (header == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		ERROR("Failed to load CBFS header from %#zx(-%#zx)\n",
			(size_t)rel_offset, (size_t)-rel_offset);
		return CBFS_HEADER_INVALID_ADDRESS;
	}

	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		ERROR("Could not find valid CBFS master header at %#zx(-%#zx): "
		      "magic %#.8x vs %#.8x.\n", (size_t)rel_offset,
		      (size_t)-rel_offset, CBFS_HEADER_MAGIC,
		      ntohl(header->magic));
		if (header->magic == 0xffffffff) {
			ERROR("Maybe ROM is not mapped properly?\n");
		}
		return CBFS_HEADER_INVALID_ADDRESS;
	}
	return header;
}

static int get_cbfs_range(uint32_t *offset, uint32_t *cbfs_end,
			  struct cbfs_media *media)
{
	const struct cbfs_header *header;

	if (media == CBFS_DEFAULT_MEDIA &&
		lib_sysinfo.cbfs_offset && lib_sysinfo.cbfs_size) {
		*offset = lib_sysinfo.cbfs_offset;
		*cbfs_end = *offset + lib_sysinfo.cbfs_size;
		return 0;
	}

	/* read offset and size from cbfs master header */
	DEBUG("Read CBFS offset & size from master header\n");
	header = cbfs_get_header(media);
	if (header == CBFS_HEADER_INVALID_ADDRESS)
		return -1;
	// Logical offset (for source media) of first file.
	*offset = ntohl(header->offset);
	*cbfs_end = ntohl(header->romsize);
#if IS_ENABLED(CONFIG_LP_ARCH_X86)
	// resolve actual length of ROM used for CBFS components
	// the bootblock size was not taken into account
	*cbfs_end -= ntohl(header->bootblocksize);

	// fine tune the length to handle alignment positioning.
	// using (bootblock size) % align, to derive the
	// number of bytes the bootblock is off from the alignment size.
	if ((ntohl(header->bootblocksize) % CBFS_ALIGNMENT))
		*cbfs_end -= (CBFS_ALIGNMENT -
			(ntohl(header->bootblocksize) % CBFS_ALIGNMENT));
	else
		*cbfs_end -= 1;
#endif
	return 0;
}

/* public API starts here*/
struct cbfs_handle *cbfs_get_handle(struct cbfs_media *media, const char *name)
{
	const char *vardata;
	uint32_t offset, cbfs_end, vardata_len;
	struct cbfs_file file;
	struct cbfs_handle *handle = malloc(sizeof(*handle));

	if (!handle)
		return NULL;

	if (get_cbfs_range(&offset, &cbfs_end, media)) {
		ERROR("Failed to find cbfs range\n");
		return NULL;
	}

	if (media == CBFS_DEFAULT_MEDIA) {
		media = &handle->media;
		if (init_default_cbfs_media(media) != 0) {
			ERROR("Failed to initialize default media.\n");
			return NULL;
		}
	} else {
		memcpy(&handle->media, media, sizeof(*media));
	}

	DEBUG("CBFS location: 0x%x~0x%x\n", offset, cbfs_end);
	DEBUG("Looking for '%s' starting from 0x%x.\n", name, offset);

	media->open(media);
	while (offset < cbfs_end &&
	       media->read(media, &file, offset, sizeof(file)) == sizeof(file)) {
		if (memcmp(CBFS_FILE_MAGIC, file.magic,
			   sizeof(file.magic)) != 0) {
			uint32_t new_align = CBFS_ALIGNMENT;
			if (offset % CBFS_ALIGNMENT)
				new_align += CBFS_ALIGNMENT -
					(offset % CBFS_ALIGNMENT);
			ERROR("ERROR: No file header found at 0x%xx - "
			      "try next aligned address: 0x%x.\n", offset,
			      offset + new_align);
			offset += new_align;
			continue;
		}
		vardata_len = ntohl(file.offset) - sizeof(file);
		DEBUG(" - load entry 0x%x variable data (%d bytes)...\n",
			offset, vardata_len);

		// load file name (arbitrary length).
		vardata = (const char*)media->map(
				media, offset + sizeof(file), vardata_len);
		if (vardata == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
			ERROR("ERROR: Failed to get filename: 0x%x.\n", offset);
		} else if (strcmp(vardata, name) == 0) {
			int file_offset = ntohl(file.offset),
			    file_len = ntohl(file.len);
			DEBUG("Found file (offset=0x%x, len=%d).\n",
			    offset + file_offset, file_len);
			media->unmap(media, vardata);
			media->close(media);
			handle->type = ntohl(file.type);
			handle->media_offset = offset;
			handle->content_offset = file_offset;
			handle->content_size = file_len;
			handle->attribute_offset =
				ntohl(file.attributes_offset);
			return handle;
		} else {
			DEBUG(" (unmatched file @0x%x: %s)\n", offset,
			      vardata);
			media->unmap(media, vardata);
		}

		// Move to next file.
		offset += ntohl(file.len) + ntohl(file.offset);
		if (offset % CBFS_ALIGNMENT)
			offset += CBFS_ALIGNMENT - (offset % CBFS_ALIGNMENT);
	}
	media->close(media);
	LOG("WARNING: '%s' not found.\n", name);
	return NULL;
}

void *cbfs_get_contents(struct cbfs_handle *handle, size_t *size, size_t limit)
{
	struct cbfs_media *m = &handle->media;
	size_t on_media_size = handle->content_size;
	int algo = CBFS_COMPRESS_NONE;
	void *ret = NULL;
	size_t dummy_size;

	if (!size)
		size = &dummy_size;

	struct cbfs_file_attr_compression *comp =
		cbfs_get_attr(handle, CBFS_FILE_ATTR_TAG_COMPRESSION);
	if (comp) {
		algo = ntohl(comp->compression);
		DEBUG("File '%s' is compressed (alg=%d)\n", name, algo);
		*size = ntohl(comp->decompressed_size);
		/* TODO: Implement partial decompression with |limit| */
	}

	if (algo == CBFS_COMPRESS_NONE) {
		if (limit != 0 && limit < on_media_size) {
			*size = limit;
			on_media_size = limit;
		} else {
			*size = on_media_size;
		}
	}

	void *data = m->map(m, handle->media_offset + handle->content_offset,
			    on_media_size);
	if (data == CBFS_MEDIA_INVALID_MAP_ADDRESS)
		return NULL;

	ret = malloc(*size);
	if (ret != NULL && !cbfs_decompress(algo, data, ret, *size)) {
		free(ret);
		ret = NULL;
	}

	m->unmap(m, data);
	return ret;
}

void *cbfs_get_file_content(struct cbfs_media *media, const char *name,
			    int type, size_t *sz)
{
	void *ret = NULL;
	struct cbfs_handle *handle = cbfs_get_handle(media, name);

	if (!handle)
		return NULL;

	if (handle->type == type)
		ret = cbfs_get_contents(handle, sz, 0);
	else
		ERROR("File '%s' is of type %x, but we requested %x.\n", name,
		      handle->type, type);

	free(handle);
	return ret;
}

void *cbfs_get_attr(struct cbfs_handle *handle, uint32_t tag)
{
	struct cbfs_media *m = &handle->media;
	uint32_t offset = handle->media_offset + handle->attribute_offset;
	uint32_t end = handle->media_offset + handle->content_offset;
	struct cbfs_file_attribute attr;
	void *ret;

	/* attribute_offset should be 0 when there is no attribute, but all
	 * values that point into the cbfs_file header are invalid, too. */
	if (handle->attribute_offset <= sizeof(struct cbfs_file))
		return NULL;

	m->open(m);
	while (offset + sizeof(attr) <= end) {
		if (m->read(m, &attr, offset, sizeof(attr)) != sizeof(attr)) {
			ERROR("Failed to read attribute header %#x\n", offset);
			m->close(m);
			return NULL;
		}
		if (ntohl(attr.tag) != tag) {
			offset += ntohl(attr.len);
			continue;
		}
		ret = m->map(m, offset, ntohl(attr.len));
		if (ret == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
			ERROR("Failed to map attribute at %#x\n", offset);
			m->close(m);
			return NULL;
		}
		return ret;
	}
	m->close(m);

	return NULL;
}

int cbfs_decompress(int algo, void *src, void *dst, int len)
{
	switch (algo) {
		case CBFS_COMPRESS_NONE:
			memcpy(dst, src, len);
			return len;
#ifdef CBFS_CORE_WITH_LZMA
		case CBFS_COMPRESS_LZMA:
			return ulzma(src, dst);
#endif
#ifdef CBFS_CORE_WITH_LZ4
		case CBFS_COMPRESS_LZ4:
			return ulz4f(src, dst);
#endif
		default:
			ERROR("tried to decompress %d bytes with algorithm #%x,"
			      "but that algorithm id is unsupported.\n", len,
			      algo);
			return 0;
	}
}
