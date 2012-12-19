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
#if CONFIG_CONSOLE_CBMEM
#define HIGH_MEMORY_DEF_SIZE	( 256 * 1024 )
#else
#define HIGH_MEMORY_DEF_SIZE	( 128 * 1024 )
#endif

#if CONFIG_HAVE_ACPI_RESUME
#define HIGH_MEMORY_SAVE	(CONFIG_RAMTOP - CONFIG_RAMBASE)
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
#define CBMEM_ID_NONE		0x00000000

#ifndef __ASSEMBLER__
#ifndef __PRE_RAM__
extern uint64_t high_tables_base, high_tables_size;
#endif

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
void __attribute__((weak)) cbmem_post_handling(void);
#endif
#endif
#endif
