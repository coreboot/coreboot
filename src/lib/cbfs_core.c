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

/* The CBFS core requires a couple of #defines or functions to adapt it to the target environment:
 *
 * CBFS_CORE_WITH_LZMA (must be #define)
 *      if defined, ulzma() must exist for decompression of data streams
 *
 * phys_to_virt(x), virt_to_phys(x)
 *      translate physical addresses to virtual and vice versa
 *      can be idempotent if no mapping is necessary.
 *
 * ERROR(x...)
 *      print an error message x (in printf format)
 *
 * LOG(x...)
 *      print a debug message x (in printf format)
 *
 * romstart()
 *      returns the start address of the ROM image, or 0xffffffff if ROM is
 *      top-aligned. This is a physical address.
 *
 * romend()
 *      returns the highest address of the ROM image + 1, for use if
 *      romstart() == 0xffffffff. This is a physical address.
 */

#include <cbfs_core.h>


/* returns pointer to master header or 0xffffffff if not found */
struct cbfs_header *get_cbfs_header(void)
{
	struct cbfs_header *header;

	/* find header */
	if (romstart() == 0xffffffff) {
		header = (struct cbfs_header*)phys_to_virt(*(uint32_t*)phys_to_virt(romend() + CBFS_HEADPTR_ADDR));
	} else {
		// FIXME: where's the master header on ARM (our current bottom-aligned platform)?
		header = NULL;
	}
	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
		ERROR("Could not find valid CBFS master header at %p: %x vs %x.\n", header, CBFS_HEADER_MAGIC, ntohl(header->magic));
		if (header->magic == 0xffffffff) {
			ERROR("Maybe ROM is not mapped properly?\n");
		}
		return (void*)0xffffffff;
	}
	return header;
}

// by must be power-of-two
#define CBFS_ALIGN(val, by) (typeof(val))((uint32_t)(val + by - 1) & (uint32_t)~(by - 1))
#define CBFS_ALIGN_UP(val, by) CBFS_ALIGN(val + 1, by)

/* public API starts here*/
struct cbfs_file *cbfs_find(const char *name)
{
	struct cbfs_header *header = get_cbfs_header();
	if (header == (void*)0xffffffff) return NULL;

	LOG("Searching for %s\n", name);

	void *data, *dataend, *origdata;
	/* find first entry */
	if (romstart() == 0xffffffff) {
		data = (void*)phys_to_virt(romend()) - ntohl(header->romsize) + ntohl(header->offset);
		dataend = (void*)phys_to_virt(romend());
	} else {
		data = (void*)phys_to_virt(romstart()) + ntohl(header->offset);
		dataend = (void*)phys_to_virt(romstart()) + ntohl(header->romsize);
	}
	dataend -= ntohl(header->bootblocksize);

	int align = ntohl(header->align);

	origdata = data;
	while ((data < (dataend - 1)) && (data >= origdata)) {
		struct cbfs_file *file = data;
		if (memcmp(CBFS_FILE_MAGIC, file->magic, strlen(CBFS_FILE_MAGIC)) != 0) {
			// no file header found. corruption?
			// proceed in aligned steps to resynchronize
			LOG("ERROR: No file header found at %p, attempting to recover by searching for header\n", data);
			data = phys_to_virt(CBFS_ALIGN_UP(virt_to_phys(data), align));
			continue;
		}
		LOG("Check %s\n", CBFS_NAME(file));
		if (strcmp(CBFS_NAME(file), name) == 0) {
			return file;
		}
		void *olddata = data;
		data = phys_to_virt(CBFS_ALIGN(virt_to_phys(data) + ntohl(file->len) + ntohl(file->offset), align));
		if (olddata > data) {
			LOG("Something is wrong here. File chain moved from %p to %p\n", olddata, data);
			return NULL;
		}
	}
	return NULL;
}

void *cbfs_get_file(const char *name)
{
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
		ERROR("File '%s' is of type %x, but we requested %x.\n", name, ntohl(file->type), type);
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
			ERROR("tried to decompress %d bytes with algorithm #%x, but that algorithm id is unsupported.\n", len, algo);
			return -1;
	}
}

