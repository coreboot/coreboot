/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2011 secunet Security Networks AG
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
 * CBFS_MEMORY_MAPPED
 *	if defined, assume whole ROM is already mapped into memory
 *
 * CBFS_COPY_FROM_ROM(source, dest, len)
 *	copies len of bytes from ROM into memory, if not memory-mapped
 *
 * CBFS_CACHE_ADDRESS
 *	memory address of cache for CBFS, if not memory-mapped.
 *
 * CBFS_HEADER_ROM_ADDRESS
 *	ROM address of CBFS header.
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

#include <cbfs_core.h>
#include <assert.h>

/* loads len of bytes data starting at offset from rom (described by header).
 * If header is NULL, load the header itself.
 * Returns 0 on success (with pointer to data in *pre_ref), or -1 on failure.
 */
int cbfs_load_rom(const struct cbfs_header *header, size_t offset,
		  void **ptr_ref, size_t len) {
#ifdef CBFS_MEMORY_MAPPED
	if (header)
		*ptr_ref = (void*)(0 - romsize + offset);
	else
		*ptr_ref = (void*)offset;
	return 0;
#else
	char *data;
	static int used_cache_size = -1;
	if (used_cache_size < 0)
		used_cache_size = 0;
	// FIXME we need to properly initialize used_cache_size (currently it's
	// not really initialized)... The -1 is simply a workaround for non-init
	// ROM data.

	data = (char*)CBFS_CACHE_ADDRESS + used_cache_size;
	used_cache_size += len;
	LOG("board_copy_from_rom(0x%x, 0x%p, %d)[%d]\n", offset, data, len,
	    used_cache_size);

	// TODO check if we're run out of cache.
	// TODO reuse cache.
	board_copy_from_rom((void*)offset, (void*)data, len);
	LOG("board_copy_from_rom finished.\n");
	*ptr_ref = data;
	return 0;
#endif
}

/* returns 0 on success (and pointer to header stored in *header_ref),
 * -1 on failure */
int cbfs_get_header(struct cbfs_header **header_ref)
{
	struct cbfs_header *header;
	void *p;

	ERROR("entry of cbfs_core(cbfs_get_header)\n");
	if (!header_ref) {
		ERROR("Need a pointer to hold header reference.\n");
		return -1;
	}
	if (cbfs_load_rom(NULL, CBFS_HEADER_ROM_ADDRESS,
			  &p, sizeof(*header)) != 0) {
		ERROR("Failed to load CBFS header from 0x%x\n",
		      CBFS_HEADER_ROM_ADDRESS);
		return -1;
	}

	header = (struct cbfs_header*)p;
	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		ERROR("Could not find valid CBFS master header at %x: "
		      "%x vs %x.\n", CBFS_HEADER_ROM_ADDRESS, CBFS_HEADER_MAGIC,
		      ntohl(header->magic));
		if (header->magic == 0xffffffff) {
			ERROR("Maybe ROM is not mapped properly?\n");
		}
		return -1;
	}

	// TODO Cache header.
	*header_ref = header;
	return 0;
}

/* public API starts here*/
struct cbfs_file *cbfs_find(const char *name)
{
	struct cbfs_header *header;
	struct cbfs_file *file;
	const char *file_name;
	uint32_t offset, align, romsize;
	void *p;

	if (cbfs_get_header(&header) != 0)
		return NULL;

	// Logical offset (for source media) of first file.
	offset = ntohl(header->offset);
	align = ntohl(header->align);

	// TODO header->romsize seems broken now on ARM.
	// Let's trust CONFIG_ROM_SIZE.
	romsize = ntohl(header->romsize);

#if CONFIG_ROM_SIZE
	romsize = CONFIG_ROM_SIZE;
#endif
	
	LOG("offset: %d, align: %d, romsize: %d\n",
	    offset, align, romsize);

	LOG("Looking for '%s' starting from 0x%x.\n", name, offset);
	while (offset < romsize &&
	       cbfs_load_rom(header, offset, &p, sizeof(*file)) == 0) {
		file = (struct cbfs_file*)p;
		if (memcmp(CBFS_FILE_MAGIC, file->magic,
			   sizeof(file->magic)) != 0) {
			// no file header found. corruption?
			LOG("ERROR: No file header found at 0x%x.", offset);
			// TODO proceed in aligned steps to resynchronize
			return NULL;
		}
		// load file name
		cbfs_load_rom(header, offset + sizeof(*file), &p,
			      ntohl(file->offset) - sizeof(*file));
		file_name = (const char *)p;
		DEBUG("Check '%s' at 0x%x\n", file_name, offset);

		if (strcmp(file_name, name) == 0) {
			LOG("found.\n");
			cbfs_load_rom(header, offset, &p,
				      ntohl(file->offset) + ntohl(file->len));
			file = (struct cbfs_file*)p;
			return file;
		}

		// Move to next file.
		offset += ntohl(file->len) + ntohl(file->offset);
		if (offset % align)
			offset += align - (offset % align);
	}

	LOG("ERROR: Not found.\n");
	return NULL;
}

void *cbfs_get_file(const char *name)
{
	// TODO Support CBFS files in 0x0.
	struct cbfs_file *file = cbfs_find(name);

	if (file == NULL) {
		ERROR("Could not find file '%s'.\n", name);
		return NULL;
	}

	return (void*)CBFS_SUBHEADER(file);
}

void *cbfs_find_file(const char *name, int type)
{
	struct cbfs_file *file = cbfs_find(name);

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
			return 0;
#ifdef CBFS_CORE_WITH_LZMA
		case CBFS_COMPRESS_LZMA:
			if (ulzma(src, dst) != 0) {
				return 0;
			}
			return -1;
#endif
		default:
			ERROR("tried to decompress %d bytes with algorithm #%x,"
			      "but that algorithm id is unsupported.\n", len,
			      algo);
			return -1;
	}
}

