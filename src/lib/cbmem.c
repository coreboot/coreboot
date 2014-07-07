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
#include <bootmem.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/cbmem_console.h>
#include <arch/early_variables.h>
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
static uint64_t cbmem_base = 0;
static uint64_t cbmem_size = 0;
#endif

static void cbmem_trace_location(uint64_t base, uint64_t size, const char *s)
{
	if (base && size && s) {
		printk(BIOS_DEBUG, "CBMEM region %llx-%llx (%s)\n",
			base, base + size - 1, s);
	}
}

static void cbmem_locate_table(uint64_t *base, uint64_t *size)
{
#ifdef __PRE_RAM__
	get_cbmem_table(base, size);
#else
	if (!(cbmem_base && cbmem_size)) {
		get_cbmem_table(&cbmem_base, &cbmem_size);
		cbmem_trace_location(cbmem_base, cbmem_size, __FUNCTION__);
	}
	*base = cbmem_base;
	*size = cbmem_size;
#endif
}

struct cbmem_entry *get_cbmem_toc(void)
{
	uint64_t base, size;
	cbmem_locate_table(&base, &size);
	return (struct cbmem_entry *)(unsigned long)base;
}

#if !defined(__PRE_RAM__)
void cbmem_late_set_table(uint64_t base, uint64_t size)
{
	cbmem_trace_location(base, size, __FUNCTION__);
	cbmem_base = base;
	cbmem_size = size;
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

static void cbmem_initialize_empty(void)
{
	uint64_t baseaddr, size;
	struct cbmem_entry *cbmem_toc;

	cbmem_locate_table(&baseaddr, &size);
	cbmem_trace_location(baseaddr, size, __FUNCTION__);

	if (!(baseaddr && size)) {
		printk(BIOS_CRIT, "Unable to set location for CBMEM.\n");
		return;
	}

	cbmem_toc = (struct cbmem_entry *)(unsigned long)baseaddr;
	memset(cbmem_toc, 0, CBMEM_TOC_RESERVED);

	cbmem_toc[0] = (struct cbmem_entry) {
		.magic	= CBMEM_MAGIC,
		.id	= CBMEM_ID_FREESPACE,
		.base	= baseaddr + CBMEM_TOC_RESERVED,
		.size	= size - CBMEM_TOC_RESERVED
	};
}

static int cbmem_check_toc(void)
{
	uint64_t baseaddr, size;
	struct cbmem_entry *cbmem_toc;

	cbmem_locate_table(&baseaddr, &size);
	cbmem_trace_location(baseaddr, size, __FUNCTION__);

	cbmem_toc = (struct cbmem_entry *)(unsigned long)baseaddr;

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

	return (void *)(uintptr_t)cbmem_toc[i].base;
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

/* Returns True if it was not initialized before. */
int cbmem_recovery(int is_wakeup)
{
	int found = cbmem_check_toc();
	int wipe = 0;

	/* CBMEM TOC is wiped clean when we are not waking up from S3
	 * suspend. Boards with EARLY_CBMEM_INIT do this in romstage,
	 * boards without EARLY_CBMEM_INIT do this in ramstage.
	 */
#if defined(__PRE_RAM__) && CONFIG_EARLY_CBMEM_INIT
	wipe = 1;
#endif
#if !defined(__PRE_RAM__) && !CONFIG_EARLY_CBMEM_INIT
	wipe = 1;
#endif

	if (!is_wakeup && wipe)
		cbmem_initialize_empty();

	if (is_wakeup && !found) {
		cbmem_initialize_empty();
		cbmem_fail_resume();
	}

	cbmem_arch_init();
	car_migrate_variables();
	return !found;
}

#ifndef __PRE_RAM__
static void init_cbmem_post_device(void *unused)
{
#if CONFIG_HAVE_ACPI_RESUME
	cbmem_recovery(acpi_is_wakeup());
#else
	cbmem_recovery(0);
#endif
#if CONFIG_CONSOLE_CBMEM
	cbmemc_reinit();
#endif
}

BOOT_STATE_INIT_ENTRIES(cbmem_bscb) = {
	BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_ENTRY,
	                      init_cbmem_post_device, NULL),
};

void cbmem_add_bootmem(void)
{
	bootmem_add_range(cbmem_base, cbmem_size, LB_MEM_TABLE);
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
