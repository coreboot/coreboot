/*
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
#define LIBPAYLOAD

#include <cbfs.h>
#include <string.h>

#ifdef LIBPAYLOAD
# define printk(x...)
# define init_default_cbfs_media libpayload_init_default_cbfs_media
  extern int libpayload_init_default_cbfs_media(struct cbfs_media *media);
#else
# include <console/console.h>
#endif

// Implementation of memory-mapped ROM media source on X86.

static int x86_rom_open(struct cbfs_media *media) {
	return 0;
}

static void *x86_rom_map(struct cbfs_media *media, size_t offset, size_t count) {
	void *ptr;
	// Some address (ex, pointer to master header) may be given in memory
	// mapped location. To workaround that, we handle >0xf0000000 as real
	// memory pointer.

	if ((uint32_t)offset > (uint32_t)0xf0000000)
		ptr = (void*)offset;
	else
		ptr = (void*)(0 - (uint32_t)media->context + offset);
	return ptr;
}

static void *x86_rom_unmap(struct cbfs_media *media, const void *address) {
	return NULL;
}

static size_t x86_rom_read(struct cbfs_media *media, void *dest, size_t offset,
			   size_t count) {
	void *ptr = x86_rom_map(media, offset, count);
	memcpy(dest, ptr, count);
	x86_rom_unmap(media, ptr);
	return count;
}

static int x86_rom_close(struct cbfs_media *media) {
	return 0;
}

int init_x86rom_cbfs_media(struct cbfs_media *media);
int init_x86rom_cbfs_media(struct cbfs_media *media) {
	// On X86, we always keep a reference of pointer to CBFS header in
	// 0xfffffffc, and the pointer is still a memory-mapped address.
	// Since the CBFS core always use ROM offset, we need to figure out
	// header->romsize even before media is initialized.
	struct cbfs_header *header = (struct cbfs_header*)
			*(uint32_t*)(0xfffffffc);
	if (CBFS_HEADER_MAGIC != ntohl(header->magic)) {
#if CONFIG(LP_ROM_SIZE)
		printk(BIOS_ERR, "Invalid CBFS master header at %p\n", header);
		media->context = (void*)CONFIG_LP_ROM_SIZE;
#else
		return -1;
#endif
	} else {
		uint32_t romsize = ntohl(header->romsize);
		media->context = (void*)romsize;
#if CONFIG(LP_ROM_SIZE)
		if (CONFIG_LP_ROM_SIZE != romsize)
			printk(BIOS_WARNING, "rom size unmatch (%d/%d)\n",
			       CONFIG_LP_ROM_SIZE, romsize);
#endif
	}
	media->open = x86_rom_open;
	media->close = x86_rom_close;
	media->map = x86_rom_map;
	media->unmap = x86_rom_unmap;
	media->read = x86_rom_read;
	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	return init_x86rom_cbfs_media(media);
}
