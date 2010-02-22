/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <console/console.h>

#if 1
#define debug(x...) printk_debug(x)
#else
#define debug(x...)
#endif

// The CBMEM TOC reserves 512 bytes to keep
// the other entries somewhat aligned.
// Increase if MAX_CBMEM_ENTRIES exceeds 21
#define CBMEM_TOC_RESERVED	512
#define MAX_CBMEM_ENTRIES	16
#define CBMEM_MAGIC		0x434f5245

struct cbmem_entry {
	u32 magic;
	u32 id;
	u64 base;
	u64 size;
} __attribute__((packed));

#ifndef __PRE_RAM__
struct cbmem_entry *bss_cbmem_toc;
#endif

/**
 * cbmem is a simple mechanism to do some kind of book keeping of the coreboot
 * high tables memory. This is a small amount of memory which is "stolen" from
 * the system memory for coreboot purposes. Usually this memory is used for
 *  - the coreboot table
 *  - legacy tables (PIRQ, MP table)
 *  - ACPI tables
 *  - suspend/resume backup memory
 */

void cbmem_init(u64 baseaddr, u64 size)
{
	struct cbmem_entry *cbmem_toc;
	cbmem_toc = (struct cbmem_entry *)(unsigned long)baseaddr;

#ifndef __PRE_RAM__
	bss_cbmem_toc = cbmem_toc;
#endif
	
	debug("Initializing CBMEM area to 0x%llx (%lld bytes)\n", baseaddr, size);

	if (size < (64 * 1024)) {
		debug("Increase CBMEM size!!\n");
		for (;;) ;
	}

	memset(cbmem_toc, 0, CBMEM_TOC_RESERVED);

	cbmem_toc[0] = (struct cbmem_entry) {
		.magic	= CBMEM_MAGIC,
		.id	= CBMEM_ID_FREESPACE,
		.base	= baseaddr + CBMEM_TOC_RESERVED,
		.size	= size - CBMEM_TOC_RESERVED
	};
}

int cbmem_reinit(u64 baseaddr)
{
	struct cbmem_entry *cbmem_toc;
	cbmem_toc = (struct cbmem_entry *)(unsigned long)baseaddr;

	debug("Re-Initializing CBMEM area to 0x%lx\n", (unsigned long)baseaddr);
#ifndef __PRE_RAM__
	bss_cbmem_toc = cbmem_toc;
#endif

	return (cbmem_toc[0].magic == CBMEM_MAGIC);
}

void *cbmem_add(u32 id, u64 size)
{
	struct cbmem_entry *cbmem_toc;
	int i;
#ifdef __PRE_RAM__
	 cbmem_toc = (struct cbmem_entry *)(get_top_of_ram() - HIGH_MEMORY_SIZE);
#else
	 cbmem_toc = bss_cbmem_toc;
#endif
	
	if (cbmem_toc == NULL) {
		return NULL;
	}

	if (cbmem_toc[0].magic != CBMEM_MAGIC) {
		printk_err("ERROR: CBMEM was not initialized yet.\n");
		return NULL;
	}

	/* Will the entry fit at all? */
	if (size > cbmem_toc[0].size) {
		printk_err("ERROR: Not enough memory for table %x\n", id);
		return NULL;
	}

	/* Align size to 512 byte blocks */

	size = ALIGN(size, 512) < cbmem_toc[0].size ? 
		ALIGN(size, 512) : cbmem_toc[0].size;

	/* Now look for the first free/usable TOC entry */
	for (i = 0; i < MAX_CBMEM_ENTRIES; i++) {
		if (cbmem_toc[i].id == CBMEM_ID_NONE)
			break;
	}

	if (i >= MAX_CBMEM_ENTRIES) {
		printk_err("ERROR: No more CBMEM entries available.\n");
		return NULL;
	}

	debug("Adding CBMEM entry as no. %d\n", i);

	cbmem_toc[i] = (struct cbmem_entry) {
		.magic = CBMEM_MAGIC,
		.id	= id,
		.base	= cbmem_toc[0].base,
		.size	= size
	};

	cbmem_toc[0].base += size;
	cbmem_toc[0].size -= size;

	return (void *)(u32)cbmem_toc[i].base;
}

void *cbmem_find(u32 id)
{
	struct cbmem_entry *cbmem_toc;
	int i;
#ifdef __PRE_RAM__
	 cbmem_toc = (struct cbmem_entry *)(get_top_of_ram() - HIGH_MEMORY_SIZE);
#else
	 cbmem_toc = bss_cbmem_toc;
#endif
	
	if (cbmem_toc == NULL)
		return NULL;

	for (i = 0; i < MAX_CBMEM_ENTRIES; i++) {
		if (cbmem_toc[i].id == id)
			return (void *)(unsigned long)cbmem_toc[i].base;
	}

	return (void *)NULL;
}

#ifndef __PRE_RAM__
#if CONFIG_HAVE_ACPI_RESUME
extern u8 acpi_slp_type;
#endif
extern uint64_t high_tables_base, high_tables_size;

void cbmem_initialize(void)
{
#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_slp_type == 3) {
		if (!cbmem_reinit(high_tables_base)) {
			/* Something went wrong, our high memory area got wiped */
			acpi_slp_type = 0;
			cbmem_init(high_tables_base, high_tables_size);
		}
	} else {
		cbmem_init(high_tables_base, high_tables_size);
	}
#else
	cbmem_init(high_tables_base, high_tables_size);
#endif
	cbmem_arch_init();
}

#ifndef __PRE_RAM__
void cbmem_list(void)
{
	struct cbmem_entry *cbmem_toc;
	int i;
#ifdef __PRE_RAM__
	 cbmem_toc = (struct cbmem_entry *)(get_top_of_ram() - HIGH_MEMORY_SIZE);
#else
	 cbmem_toc = bss_cbmem_toc;
#endif
	
	if (cbmem_toc == NULL)
		return;

	for (i = 0; i < MAX_CBMEM_ENTRIES; i++) {

		if (cbmem_toc[i].magic != CBMEM_MAGIC)
			continue;
		printk_debug("%2d. ", i);
		switch (cbmem_toc[i].id) {
		case CBMEM_ID_FREESPACE: printk_debug("FREE SPACE "); break;
		case CBMEM_ID_GDT:	 printk_debug("GDT        "); break;
		case CBMEM_ID_ACPI:	 printk_debug("ACPI       "); break;
		case CBMEM_ID_CBTABLE:	 printk_debug("COREBOOT   "); break;
		case CBMEM_ID_PIRQ:	 printk_debug("IRQ TABLE  "); break;
		case CBMEM_ID_MPTABLE:	 printk_debug("SMP TABLE  "); break;
		case CBMEM_ID_RESUME:	 printk_debug("ACPI RESUME"); break;
		default: printk_debug("%08x ", cbmem_toc[i].id);
		}
		printk_debug("%08llx ", cbmem_toc[i].base);
		printk_debug("%08llx\n", cbmem_toc[i].size);
	}
}
#endif

#endif

