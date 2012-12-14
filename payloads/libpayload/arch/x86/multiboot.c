/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <libpayload-config.h>
#include <libpayload.h>
#include <multiboot_tables.h>

extern unsigned long loader_eax;
extern unsigned long loader_ebx;

static void mb_parse_mmap(struct multiboot_header *table,
			struct sysinfo_t *info)
{
	u8 *start = (u8 *) phys_to_virt(table->mmap_addr);
	u8 *ptr = start;

	info->n_memranges = 0;

	while(ptr < (start + table->mmap_length)) {
		struct multiboot_mmap *mmap = (struct multiboot_mmap *) ptr;

#ifdef CONFIG_MEMMAP_RAM_ONLY
		/* 1 == normal RAM.  Ignore everything else for now */

		if (mmap->type == 1) {
#endif
			info->memrange[info->n_memranges].base = mmap->addr;
			info->memrange[info->n_memranges].size = mmap->length;
			info->memrange[info->n_memranges].type = mmap->type;

			if (++info->n_memranges == SYSINFO_MAX_MEM_RANGES)
				return;
#ifdef CONFIG_MEMMAP_RAM_ONLY
		}
#endif

		ptr += (mmap->size + sizeof(mmap->size));
	}
}

static void mb_parse_cmdline(struct multiboot_header *table)
{
	extern int main_argc;
	extern char *main_argv[];
	char *c = phys_to_virt(table->cmdline);

	while(*c != '\0' && main_argc < MAX_ARGC_COUNT) {
		main_argv[main_argc++] = c;

		for( ; *c != '\0' && !isspace(*c); c++);

		if (*c) {
			*c = 0;
			c++;
		}
	}
}

int get_multiboot_info(struct sysinfo_t *info)
{
	struct multiboot_header *table;

	if (loader_eax != MULTIBOOT_MAGIC)
		return -1;

	table = (struct multiboot_header *) phys_to_virt(loader_ebx);

	info->mbtable = phys_to_virt(loader_ebx);

	if (table->flags & MULTIBOOT_FLAGS_MMAP)
		mb_parse_mmap(table, info);

	if (table->flags & MULTIBOOT_FLAGS_CMDLINE)
		mb_parse_cmdline(table);

	return 0;
}
