/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
