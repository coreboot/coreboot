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

	if (lib_sysinfo.cbfs_offset && lib_sysinfo.cbfs_size) {
		*offset = lib_sysinfo.cbfs_offset;
		*cbfs_end = *offset + lib_sysinfo.cbfs_size;
		return 0;
	}

	/*
	 * If sysinfo doesn't have offset or size, we read them from
	 * a master header.
	 */
	DEBUG("CBFS offset & size not found in sysinfo\n");
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
struct cbfs_file *cbfs_get_file(struct cbfs_media *media, const char *name)
{
	const char *vardata;
	uint32_t offset, cbfs_end, vardata_len;
	struct cbfs_file file, *file_ptr;
	struct cbfs_media default_media;

	if (media == CBFS_DEFAULT_MEDIA) {
		media = &default_media;
		if (init_default_cbfs_media(media) != 0) {
			ERROR("Failed to initialize default media.\n");
			return NULL;
		}
	}

	if (get_cbfs_range(&offset, &cbfs_end, media)) {
		ERROR("Failed to find cbfs range\n");
		return NULL;
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
			file_ptr = media->map(media, offset,
					      file_offset + file_len);
			media->close(media);
			return file_ptr;
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

void *cbfs_get_file_content(struct cbfs_media *media, const char *name,
			    int type, size_t *sz)
{
	struct cbfs_media default_media;

	if (media == CBFS_DEFAULT_MEDIA) {
		media = &default_media;
		if (init_default_cbfs_media(media) != 0) {
			ERROR("Failed to initialize default media.\n");
			return NULL;
		}
	}

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

	void *file_content = (void *)CBFS_SUBHEADER(file);

	struct cbfs_file_attribute *attr =
		cbfs_file_find_attr(file, CBFS_FILE_ATTR_TAG_COMPRESSION);

	size_t final_size = ntohl(file->len);
	int compression_algo = CBFS_COMPRESS_NONE;
	if (attr) {
		struct cbfs_file_attr_compression *comp =
			(struct cbfs_file_attr_compression *)attr;
		compression_algo = ntohl(comp->compression);
		DEBUG("File '%s' is compressed (alg=%d)\n",
		      name, compression_algo);
		final_size = ntohl(comp->decompressed_size);
	}

	void *dst = malloc(final_size);
	if (dst == NULL)
		goto err;

	if (!cbfs_decompress(compression_algo, file_content, dst, final_size))
		goto err;

	if (sz)
		*sz = final_size;

	media->unmap(media, file);
	return dst;

err:
	media->unmap(media, file);
	free(dst);
	return NULL;
}

struct cbfs_file_attribute *cbfs_file_first_attr(struct cbfs_file *file)
{
	/* attributes_offset should be 0 when there is no attribute, but all
	 * values that point into the cbfs_file header are invalid, too. */
	if (ntohl(file->attributes_offset) <= sizeof(*file))
		return NULL;

	/* There needs to be enough space for the file header and one
	 * attribute header for this to make sense. */
	if (ntohl(file->offset) <=
		sizeof(*file) + sizeof(struct cbfs_file_attribute))
		return NULL;

	return (struct cbfs_file_attribute *)
		(((uint8_t *)file) + ntohl(file->attributes_offset));
}

struct cbfs_file_attribute *cbfs_file_next_attr(struct cbfs_file *file,
	struct cbfs_file_attribute *attr)
{
	/* ex falso sequitur quodlibet */
	if (attr == NULL)
		return NULL;

	/* Is there enough space for another attribute? */
	if ((uint8_t *)attr + ntohl(attr->len) +
		sizeof(struct cbfs_file_attribute) >=
		(uint8_t *)file + ntohl(file->offset))
		return NULL;

	struct cbfs_file_attribute *next = (struct cbfs_file_attribute *)
		(((uint8_t *)attr) + ntohl(attr->len));
	/* If any, "unused" attributes must come last. */
	if (ntohl(next->tag) == CBFS_FILE_ATTR_TAG_UNUSED)
		return NULL;
	if (ntohl(next->tag) == CBFS_FILE_ATTR_TAG_UNUSED2)
		return NULL;

	return next;
}

struct cbfs_file_attribute *cbfs_file_find_attr(struct cbfs_file *file,
	uint32_t tag)
{
	struct cbfs_file_attribute *attr = cbfs_file_first_attr(file);
	while (attr) {
		if (ntohl(attr->tag) == tag)
			break;
		attr = cbfs_file_next_attr(file, attr);
	}
	return attr;

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
		default:
			ERROR("tried to decompress %d bytes with algorithm #%x,"
			      "but that algorithm id is unsupported.\n", len,
			      algo);
			return 0;
	}
}
