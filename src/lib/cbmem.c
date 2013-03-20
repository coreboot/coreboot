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
#if CONFIG_HAVE_ACPI_RESUME && !defined(__PRE_RAM__)
#include <arch/acpi.h>
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
static struct cbmem_entry *bss_cbmem_toc;

struct cbmem_entry *__attribute__((weak)) get_cbmem_toc(void)
{
	return bss_cbmem_toc;
}

void __attribute__((weak)) set_cbmem_toc(struct cbmem_entry * x)
{
	/* do nothing, this should be called by chipset to save TOC in NVRAM */
}
#else

struct cbmem_entry *__attribute__((weak)) get_cbmem_toc(void)
{
	printk(BIOS_WARNING, "WARNING: you need to define get_cbmem_toc() for your chipset\n");
	return NULL;
}

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

	printk(BIOS_DEBUG, "Initializing CBMEM area to 0x%llx (%lld bytes)\n",
	       baseaddr, size);

	if (size < (64 * 1024)) {
		printk(BIOS_DEBUG, "Increase CBMEM size!\n");
		for (;;) ;
	}

	/* we don't need to call this in romstage, usefull only from ramstage */
#ifndef __PRE_RAM__
	set_cbmem_toc((struct cbmem_entry *)(unsigned long)baseaddr);
#endif
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

	printk(BIOS_DEBUG, "Re-Initializing CBMEM area to 0x%lx\n",
	       (unsigned long)baseaddr);

#ifndef __PRE_RAM__
	bss_cbmem_toc = cbmem_toc;
#endif

	return (cbmem_toc[0].magic == CBMEM_MAGIC);
}

void *cbmem_add(u32 id, u64 size)
{
	struct cbmem_entry *cbmem_toc;
	int i;
	void *p;

	/*
	 * This could be a restart, check if the section is there already. It
	 * is remotely possible that the dram contents persisted over the
	 * bootloader upgrade AND the same section now needs more room, but
	 * this is quite a remote possibility and it is ignored here.
	 */
	p = cbmem_find(id);
	if (p) {
		printk(BIOS_NOTICE,
		       "CBMEM section %x: using existing location at %p.\n",
		       id, p);
		return p;
	}

	cbmem_toc = get_cbmem_toc();

	if (cbmem_toc == NULL) {
		return NULL;
	}

	if (cbmem_toc[0].magic != CBMEM_MAGIC) {
		printk(BIOS_ERR, "ERROR: CBMEM was not initialized yet.\n");
		return NULL;
	}

	/* Will the entry fit at all? */
	if (size > cbmem_toc[0].size) {
		printk(BIOS_ERR, "ERROR: Not enough memory for table %x\n", id);
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
		printk(BIOS_ERR, "ERROR: No more CBMEM entries available.\n");
		return NULL;
	}

	printk(BIOS_DEBUG, "Adding CBMEM entry as no. %d\n", i);

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
	cbmem_toc = get_cbmem_toc();

	if (cbmem_toc == NULL)
		return NULL;

	for (i = 0; i < MAX_CBMEM_ENTRIES; i++) {
		if (cbmem_toc[i].id == id)
			return (void *)(unsigned long)cbmem_toc[i].base;
	}

	return (void *)NULL;
}

#if CONFIG_EARLY_CBMEM_INIT || !defined(__PRE_RAM__)
/* Returns True if it was not intialized before. */
int cbmem_initialize(void)
{
	int rv = 0;

#ifdef __PRE_RAM__
	extern unsigned long get_top_of_ram(void);
	uint64_t high_tables_base = get_top_of_ram() - HIGH_MEMORY_SIZE;
	uint64_t high_tables_size = HIGH_MEMORY_SIZE;
#endif

	/* We expect the romstage to always initialize it. */
	if (!cbmem_reinit(high_tables_base)) {
#if CONFIG_HAVE_ACPI_RESUME && !defined(__PRE_RAM__)
		/* Something went wrong, our high memory area got wiped */
		if (acpi_slp_type == 3 || acpi_slp_type == 2)
			acpi_slp_type = 0;
#endif
		cbmem_init(high_tables_base, high_tables_size);
		rv = 1;
	}
#ifndef __PRE_RAM__
	cbmem_arch_init();
#endif
	return rv;
}
#endif

#ifndef __PRE_RAM__
/* cbmem cannot be initialized before device drivers, but it can be initialized
 * after the drivers have run. */
void init_cbmem_pre_device(void) {}

void init_cbmem_post_device(void)
{
	cbmem_initialize();
#if CONFIG_CONSOLE_CBMEM
	cbmemc_reinit();
#endif
}

void cbmem_list(void)
{
	struct cbmem_entry *cbmem_toc;
	int i;
	cbmem_toc = get_cbmem_toc();

	if (cbmem_toc == NULL)
		return;

	for (i = 0; i < MAX_CBMEM_ENTRIES; i++) {

		if (cbmem_toc[i].magic != CBMEM_MAGIC)
			continue;
		cbmem_print_entry(i, cbmem_toc[i].id, cbmem_toc[i].base,
		                  cbmem_toc[i].size);
	}
}
#endif


