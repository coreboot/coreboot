/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __RISCV_PMP_H__
#define __RISCV_PMP_H__

#include <stdint.h>

/*
 * this function needs to be implemented by a specific SoC.
 * return number of PMP entries for current hart
 */
int pmp_entries_num(void);

void print_pmp_regions(void);

/* reset PMP setting */
void reset_pmp(void);

/*
 * set up PMP record
 * reminder: base and size are 34-bits on RV32.
 */
void setup_pmp(u64 base, u64 size, u8 flags);

/* write the last PMP record, i.e. the "default" case. */
void close_pmp(void);

#endif /* __RISCV_PMP_H__ */
