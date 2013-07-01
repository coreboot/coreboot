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
 * CBFS_HEADER_ROM_ADDRESS
 *	ROM address (offset) of CBFS header. Underlying CBFS media may interpret
 *	it in other way so we call this "address".
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

/* returns a pointer to CBFS master header, or CBFS_HEADER_INVALID_ADDRESS
 *  on failure */
const struct cbfs_header *cbfs_get_header(struct cbfs_media *media)
{
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
	DEBUG("CBFS_HEADER_ROM_ADDRESS: 0x%x/0x%x\n", CBFS_HEADER_ROM_ADDRESS,
	      CONFIG_ROM_SIZE);
	header = media->map(media, CBFS_HEADER_ROM_ADDRESS, sizeof(*header));
	media->close(media);

	if (header == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
		ERROR("Failed to load CBFS header from 0x%x\n",
		      CBFS_HEADER_ROM_ADDRESS);
		return CBFS_HEADER_INVALID_ADDRESS;
	}

	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		ERROR("Could not find valid CBFS master header at %x: "
		      "%x vs %x.\n", CBFS_HEADER_ROM_ADDRESS, CBFS_HEADER_MAGIC,
		      ntohl(header->magic));
		if (header->magic == 0xffffffff) {
			ERROR("Maybe ROM is not mapped properly?\n");
		}
		return CBFS_HEADER_INVALID_ADDRESS;
	}
	return header;
}

/* public API starts here*/
struct cbfs_file *cbfs_get_file(struct cbfs_media *media, const char *name)
{
	const char *file_name;
	uint32_t offset, align, romsize, name_len;
	const struct cbfs_header *header;
	struct cbfs_file file, *file_ptr;
	struct cbfs_media default_media;

	if (media == CBFS_DEFAULT_MEDIA) {
		media = &default_media;
		if (init_default_cbfs_media(media) != 0) {
			ERROR("Failed to initialize default media.\n");
			return NULL;
		}
	}

	if (CBFS_HEADER_INVALID_ADDRESS == (header = cbfs_get_header(media)))
		return NULL;

	// Logical offset (for source media) of first file.
	offset = ntohl(header->offset);
	align = ntohl(header->align);
	romsize = ntohl(header->romsize);

	// TODO Add a "size" in CBFS header for a platform independent way to
	// determine the end of CBFS data.
#if defined(CONFIG_ARCH_X86) && CONFIG_ARCH_X86
	romsize -= htonl(header->bootblocksize);
#endif
	DEBUG("CBFS location: 0x%x~0x%x, align: %d\n", offset, romsize, align);

	DEBUG("Looking for '%s' starting from 0x%x.\n", name, offset);
	media->open(media);
	while (offset < romsize &&
	       media->read(media, &file, offset, sizeof(file)) == sizeof(file)) {
		if (memcmp(CBFS_FILE_MAGIC, file.magic,
			   sizeof(file.magic)) != 0) {
			uint32_t new_align = align;
			if (offset % align)
				new_align += align - (offset % align);
			ERROR("ERROR: No file header found at 0x%xx - "
			      "try next aligned address: 0x%x.\n", offset,
			      offset + new_align);
			offset += new_align;
			continue;
		}
		name_len = ntohl(file.offset) - sizeof(file);
		DEBUG(" - load entry 0x%x file name (%d bytes)...\n", offset,
		      name_len);

		// load file name (arbitrary length).
		file_name = (const char*)media->map(
				media, offset + sizeof(file), name_len);
		if (file_name == CBFS_MEDIA_INVALID_MAP_ADDRESS) {
			ERROR("ERROR: Failed to get filename: 0x%x.\n", offset);
		} else if (strcmp(file_name, name) == 0) {
			int file_offset = ntohl(file.offset),
			    file_len = ntohl(file.len);
			DEBUG("Found file (offset=0x%x, len=%d).\n",
			    offset + file_offset, file_len);
			media->unmap(media, file_name);
			file_ptr = media->map(media, offset,
					      file_offset + file_len);
			media->close(media);
			return file_ptr;
		} else {
			DEBUG(" (unmatched file @0x%x: %s)\n", offset,
			      file_name);
			media->unmap(media, file_name);
		}

		// Move to next file.
		offset += ntohl(file.len) + ntohl(file.offset);
		if (offset % align)
			offset += align - (offset % align);
	}
	media->close(media);
	LOG("WARNING: '%s' not found.\n", name);
	return NULL;
}

void *cbfs_get_file_content(struct cbfs_media *media, const char *name, int type)
{
	struct cbfs_file *file = cbfs_get_file(media, name);

	if (file == NULL) {
		ERROR("Could not find file '%s'.\n", name);
		return NULL;
	}

	if (ntohl(file->type) != type) {
		ERROR("File '%s' is of type %x, but we requested %x.\n", name,
		      ntohl(file->type), type);
		return NULL;
	}

	return (void*)CBFS_SUBHEADER(file);
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

