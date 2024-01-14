/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SIFIVE_COMMON_SDRAM_H
#define SIFIVE_COMMON_SDRAM_H

#include <stdint.h>
#include <types.h>

void sdram_init(size_t dram_size);
size_t sdram_size(void);

#endif
