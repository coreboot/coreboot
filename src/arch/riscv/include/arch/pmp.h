/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __RISCV_PMP_H__
#define __RISCV_PMP_H__

#include <stdint.h>

/*
 * this function needs to be implemented by a specific SoC.
 * return number of PMP entries for current hart
 */
extern int pmp_entries_num(void);

/* reset PMP setting */
void reset_pmp(void);

/* set up PMP record */
void setup_pmp(uintptr_t base, uintptr_t size, uintptr_t flags);

#endif /* __RISCV_PMP_H__ */
