/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * For reference see "AMD64 Architecture Programmer's Manual Volume 2",
 * Document 24593-Rev. 3.31-July 2019 Chapter 5.3.4
 *
 * Page table attributes: WB, User+Supervisor, Present, Writeable, Accessed, Dirty
 */

.section .rodata
#define _PRES (1ULL << 0)
#define _RW   (1ULL << 1)
#define _US   (1ULL << 2)
#define _A    (1ULL << 5)
#define _D    (1ULL << 6)
#define _PS   (1ULL << 7)
#define _GEN_DIR(a) (_PRES + _RW + _US + _A + (a))
#define _GEN_PAGE(a) (_PRES + _RW + _US + _PS + _A +  _D + (a))

.global PM4LE
.align 32
PM4LE:
.quad _GEN_DIR(PDPE_table)

.align 4096
PDE_tables: /* identity map 2MiB pages */
.rept 2048
.quad _GEN_PAGE(0x200000 * ((. - PDE_tables) >> 3))
.endr

.align 4096
PDPE_table: /* Point to PDE */
.rept 4
.quad _GEN_DIR(PDE_tables + 4096 * ((. - PDPE_table) >> 3))
.endr
