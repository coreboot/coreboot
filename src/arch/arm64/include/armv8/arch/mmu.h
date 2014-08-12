/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __ARCH_ARM64_MMU_H__
#define __ARCH_ARM64_MMU_H__

#include <memrange.h>

/* IMPORTANT!!!!!!!
 * Assumptions made:
 * Granule size is 64KiB
 * BITS per Virtual address is 33
 * All the calculations for tables L1,L2 and L3 are based on these assumptions
 * If these values are changed, recalculate the other macros as well
 */


/* Memory attributes for mmap regions
 * These attributes act as tag values for memrange regions
 */

/* Normal memory / device */
#define MA_MEM                     (1 << 0)
#define MA_DEV                     (0 << 0)

/* Secure / non-secure */
#define MA_NS                      (1 << 1)
#define MA_S                       (0 << 1)

/* Read only / Read-write */
#define MA_RO                      (1 << 2)
#define MA_RW                      (0 << 2)

/* Non-cacheable memory. */
#define MA_MEM_NC                  (1 << 3)

/* Descriptor attributes */

#define INVALID_DESC               0x0
#define BLOCK_DESC                 0x1
#define TABLE_DESC                 0x3
#define PAGE_DESC                  0x3

/* Block descriptor */
#define BLOCK_NS                   (1 << 5)

#define BLOCK_AP_RW                (0 << 7)
#define BLOCK_AP_RO                (1 << 7)

#define BLOCK_ACCESS               (1 << 10)

/* XLAT Table Init Attributes */

#define VA_START                   0x0
/* If BITS_PER_VA or GRANULE_SIZE are changed, recalculate and change the
   macros following them */
#define BITS_PER_VA                33
/* Granule size of 64KB is being used */
#define GRANULE_SIZE_SHIFT         16
#define GRANULE_SIZE               (1 << GRANULE_SIZE_SHIFT)
#define XLAT_TABLE_MASK            ~(0xffffUL)
#define GRANULE_SIZE_MASK          ((1 << 16) - 1)

#define L1_ADDR_SHIFT              42
#define L2_ADDR_SHIFT              29
#define L3_ADDR_SHIFT              16

#define L1_ADDR_MASK               (0UL << L1_ADDR_SHIFT)
#define L2_ADDR_MASK               (0xfUL << L2_ADDR_SHIFT)
#define L3_ADDR_MASK               (0x1fffUL << L3_ADDR_SHIFT)

/* Dependent on BITS_PER_VA and GRANULE_SIZE */
#define INIT_LEVEL                 2
#define XLAT_MAX_LEVEL             3

/* Each entry in XLAT table is 8 bytes */
#define XLAT_ENTRY_SHIFT           3
#define XLAT_ENTRY_SIZE            (1 << XLAT_ENTRY_SHIFT)

#define XLAT_TABLE_SHIFT           GRANULE_SIZE_SHIFT
#define XLAT_TABLE_SIZE            (1 << XLAT_TABLE_SHIFT)

#define XLAT_NUM_ENTRIES_SHIFT     (XLAT_TABLE_SHIFT - XLAT_ENTRY_SHIFT)
#define XLAT_NUM_ENTRIES           (1 << XLAT_NUM_ENTRIES_SHIFT)

#define L3_XLAT_SIZE_SHIFT         (GRANULE_SIZE_SHIFT)
#define L2_XLAT_SIZE_SHIFT         (GRANULE_SIZE_SHIFT + XLAT_NUM_ENTRIES_SHIFT)
#define L1_XLAT_SIZE_SHIFT         (GRANULE_SIZE_SHIFT + XLAT_NUM_ENTRIES_SHIFT)

/* These macros give the size of the region addressed by each entry of a xlat
   table at any given level */
#define L3_XLAT_SIZE               (1 << L3_XLAT_SIZE_SHIFT)
#define L2_XLAT_SIZE               (1 << L2_XLAT_SIZE_SHIFT)
#define L1_XLAT_SIZE               (1 << L1_XLAT_SIZE_SHIFT)

/* Block indices required for MAIR */
#define BLOCK_INDEX_MEM_DEV_NGNRNE 0
#define BLOCK_INDEX_MEM_DEV_NGNRE  1
#define BLOCK_INDEX_MEM_DEV_GRE    2
#define BLOCK_INDEX_MEM_NORMAL_NC  3
#define BLOCK_INDEX_MEM_NORMAL     4

#define BLOCK_INDEX_SHIFT          2

/* MAIR attributes */
#define MAIR_ATTRIBUTES            ((0x00 << (BLOCK_INDEX_MEM_DEV_NGNRNE*8)) | \
				    (0x04 << (BLOCK_INDEX_MEM_DEV_NGNRE*8))  | \
				    (0x0c << (BLOCK_INDEX_MEM_DEV_GRE*8))    | \
				    (0x44 << (BLOCK_INDEX_MEM_NORMAL_NC*8))  | \
				    (0xffUL << (BLOCK_INDEX_MEM_NORMAL*8)))

/* TCR attributes */
#define TCR_TOSZ                   (64 - BITS_PER_VA)

#define TCR_IRGN0_SHIFT            8
#define TCR_IRGN0_NM_NC            (0x00 << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NM_WBWAC         (0x01 << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NM_WTC           (0x02 << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NM_WBNWAC        (0x03 << TCR_IRGN0_SHIFT)

#define TCR_ORGN0_SHIFT            10
#define TCR_ORGN0_NM_NC            (0x00 << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NM_WBWAC         (0x01 << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NM_WTC           (0x02 << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NM_WBNWAC        (0x03 << TCR_ORGN0_SHIFT)

#define TCR_SH0_SHIFT              12
#define TCR_SH0_NC                 (0x0 << TCR_SH0_SHIFT)
#define TCR_SH0_OS                 (0x2 << TCR_SH0_SHIFT)
#define TCR_SH0_IS                 (0x3 << TCR_SH0_SHIFT)

#define TCR_TG0_SHIFT              14
#define TCR_TG0_4KB                (0x0 << TCR_TG0_SHIFT)
#define TCR_TG0_64KB               (0x1 << TCR_TG0_SHIFT)
#define TCR_TG0_16KB               (0x2 << TCR_TG0_SHIFT)

#define TCR_PS_SHIFT               16
#define TCR_PS_4GB                 (0x0 << TCR_PS_SHIFT)
#define TCR_PS_64GB                (0x1 << TCR_PS_SHIFT)
#define TCR_PS_1TB                 (0x2 << TCR_PS_SHIFT)
#define TCR_PS_4TB                 (0x3 << TCR_PS_SHIFT)
#define TCR_PS_16TB                (0x4 << TCR_PS_SHIFT)
#define TCR_PS_256TB               (0x5 << TCR_PS_SHIFT)

#define TCR_TBI_SHIFT              20
#define TCR_TBI_USED               (0x0 << TCR_TBI_SHIFT)
#define TCR_TBI_IGNORED            (0x1 << TCR_TBI_SHIFT)

void mmu_init(struct memranges *,uint64_t *,uint64_t);
void mmu_enable(uint64_t);

#endif // __ARCH_ARM64_MMU_H__
