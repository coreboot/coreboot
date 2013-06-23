/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2013 Google, Inc.
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

#ifndef _CBMEM_H_
#define _CBMEM_H_

/* Reserve 128k for ACPI and other tables */
#if CONFIG_CONSOLE_CBMEM
#define HIGH_MEMORY_DEF_SIZE	( 256 * 1024 )
#else
#define HIGH_MEMORY_DEF_SIZE	( 128 * 1024 )
#endif

#if CONFIG_HAVE_ACPI_RESUME
#if CONFIG_RELOCATABLE_RAMSTAGE
#define HIGH_MEMORY_SAVE	0
#else
#define HIGH_MEMORY_SAVE	(CONFIG_RAMTOP - CONFIG_RAMBASE)
#endif

#define HIGH_MEMORY_SIZE	(HIGH_MEMORY_SAVE + CONFIG_HIGH_SCRATCH_MEMORY_SIZE + HIGH_MEMORY_DEF_SIZE)

/* Delegation of resume backup memory so we don't have to
 * (slowly) handle backing up OS memory in romstage.c
 */
#define CBMEM_BOOT_MODE		0x610
#define CBMEM_RESUME_BACKUP	0x614

#else /* CONFIG_HAVE_ACPI_RESUME */
#define HIGH_MEMORY_SIZE	HIGH_MEMORY_DEF_SIZE
#endif /* CONFIG_HAVE_ACPI_RESUME */

#define CBMEM_ID_FREESPACE	0x46524545
#define CBMEM_ID_GDT		0x4c474454
#define CBMEM_ID_ACPI		0x41435049
#define CBMEM_ID_ACPI_GNVS	0x474e5653
#define CBMEM_ID_ACPI_GNVS_PTR	0x474e5650
#define CBMEM_ID_CBTABLE	0x43425442
#define CBMEM_ID_PIRQ		0x49525154
#define CBMEM_ID_MPTABLE	0x534d5054
#define CBMEM_ID_RESUME		0x5245534d
#define CBMEM_ID_RESUME_SCRATCH	0x52455343
#define CBMEM_ID_SMBIOS         0x534d4254
#define CBMEM_ID_TIMESTAMP	0x54494d45
#define CBMEM_ID_MRCDATA	0x4d524344
#define CBMEM_ID_CONSOLE	0x434f4e53
#define CBMEM_ID_ELOG		0x454c4f47
#define CBMEM_ID_COVERAGE	0x47434f56
#define CBMEM_ID_ROMSTAGE_INFO	0x47545352
#define CBMEM_ID_ROMSTAGE_RAM_STACK 0x90357ac4
#define CBMEM_ID_RAMSTAGE	0x9a357a9e
#define CBMEM_ID_RAMSTAGE_CACHE	0x9a3ca54e
#define CBMEM_ID_ROOT		0xff4007ff
#define CBMEM_ID_VBOOT_HANDOFF	0x780074f0
#define CBMEM_ID_CAR_GLOBALS	0xcac4e6a3
#define CBMEM_ID_EHCI_DEBUG	0xe4c1deb9
#define CBMEM_ID_NONE		0x00000000
#define CBMEM_ID_AGESA_RUNTIME	0x41474553

#ifndef __ASSEMBLER__
#include <stdint.h>

struct cbmem_entry;

#if CONFIG_DYNAMIC_CBMEM

/*
 * The dynamic cbmem infrastructure allows for growing cbmem dynamically as
 * things are added. It requires an external function, cbmem_top(), to be
 * implemented by the board or chipset to define the upper address where
 * cbmem lives. This address is required to be a 32-bit address. Additionally,
 * the address needs to be consistent in both romstage and ramstage.  The
 * dynamic cbmem infrastructure allocates new regions below the last allocated
 * region. Regions are defined by a cbmem_entry struct that is opaque. Regions
 * may be removed, but the last one added is the only that can be removed.
 *
 * Dynamic cbmem has two allocators within it. All allocators use a top down
 * allocation scheme. However, there are 2 modes for each allocation depending
 * on the requested size. There are large allocations and small allocations.
 * An allocation is considered to be small when it is less than or equal to
 * DYN_CBMEM_ALIGN_SIZE / 2. The smaller allocations are fit into a larger
 * allocation region.
 */

#define DYN_CBMEM_ALIGN_SIZE (4096)

/* Initialize cbmem to be empty. */
void cbmem_initialize_empty(void);

/* Return the top address for dynamic cbmem. The address returned needs to
 * be consistent across romstage and ramstage, and it is required to be
 * below 4GiB. */
void *cbmem_top(void);

/* Add a cbmem entry of a given size and id. These return NULL on failure. The
 * add function performs a find first and do not check against the original
 * size. */
const struct cbmem_entry *cbmem_entry_add(u32 id, u64 size);

/* Find a cbmem entry of a given id. These return NULL on failure. */
const struct cbmem_entry *cbmem_entry_find(u32 id);

/* Remove a region defined by a cbmem_entry. Returns 0 on success, < 0 on
 * error. Note: A cbmem_entry cannot be removed unless it was the last one
 * added. */
int cbmem_entry_remove(const struct cbmem_entry *entry);

/* cbmem_entry accessors to get pointer and size of a cbmem_entry. */
void *cbmem_entry_start(const struct cbmem_entry *entry);
u64 cbmem_entry_size(const struct cbmem_entry *entry);


#else /* !CONFIG_DYNAMIC_CBMEM */

#ifndef __PRE_RAM__
void set_top_of_ram(uint64_t ramtop);
void backup_top_of_ram(uint64_t ramtop);
void cbmem_late_set_table(uint64_t base, uint64_t size);
int cbmem_base_check(void);
#endif

int cbmem_reinit(void);

void get_cbmem_table(uint64_t *base, uint64_t *size);
struct cbmem_entry *get_cbmem_toc(void);

#endif /* CONFIG_DYNAMIC_CBMEM */

/* Common API between cbmem and dynamic cbmem. */

unsigned long get_top_of_ram(void);

/* By default cbmem is attempted to be recovered. Returns 0 if cbmem was
 * recovered or 1 if cbmem had to be reinitialized. */
int cbmem_initialize(void);
/* Add a cbmem entry of a given size and id. These return NULL on failure. The
 * add function performs a find first and do not check against the original
 * size. */
void *cbmem_add(u32 id, u64 size);
/* Find a cbmem entry of a given id. These return NULL on failure. */
void *cbmem_find(u32 id);

#ifndef __PRE_RAM__
/* Ramstage only functions. */
/* Add the cbmem memory used to the memory tables. */
struct lb_memory;
void cbmem_add_lb_mem(struct lb_memory *mem);
void cbmem_list(void);
void cbmem_arch_init(void);
void cbmem_print_entry(int n, u32 id, u64 start, u64 size);
#else
static inline void cbmem_arch_init(void) {}
#endif /* __PRE_RAM__ */

#endif /* __ASSEMBLER__ */


#endif /* _CBMEM_H_ */
