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

#ifndef _CBMEM_H_
#define _CBMEM_H_

/* Reserve 128k for ACPI and other tables */
#define HIGH_MEMORY_DEF_SIZE	( 128 * 1024 )
#ifndef __PRE_RAM__
extern uint64_t high_tables_base, high_tables_size;
#endif

#if CONFIG_HAVE_ACPI_RESUME
#define HIGH_MEMORY_SIZE	((CONFIG_RAMTOP - CONFIG_RAMBASE) + HIGH_MEMORY_DEF_SIZE)
#define HIGH_MEMORY_SAVE	( HIGH_MEMORY_SIZE - HIGH_MEMORY_DEF_SIZE )
#else
#define HIGH_MEMORY_SIZE	HIGH_MEMORY_DEF_SIZE
#endif

#define CBMEM_ID_FREESPACE	0x46524545
#define CBMEM_ID_GDT		0x4c474454
#define CBMEM_ID_ACPI		0x41435049
#define CBMEM_ID_CBTABLE	0x43425442
#define CBMEM_ID_PIRQ		0x49525154
#define CBMEM_ID_MPTABLE	0x534d5054
#define CBMEM_ID_RESUME		0x5245534d
#define CBMEM_ID_SMBIOS         0x534d4254
#define CBMEM_ID_NONE		0x00000000

int cbmem_initialize(void);

void cbmem_init(u64 baseaddr, u64 size);
int cbmem_reinit(u64 baseaddr);
void *cbmem_add(u32 id, u64 size);
void *cbmem_find(u32 id);
void cbmem_list(void);
void cbmem_arch_init(void);

extern struct cbmem_entry *get_cbmem_toc(void);

#ifndef __PRE_RAM__
void set_cbmem_toc(struct cbmem_entry *);
#endif
#endif
