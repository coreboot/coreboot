/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_LPC_H_
#define _SOC_APOLLOLAKE_LPC_H_

#include <stddef.h>
#include <stdint.h>

#define REG_SERIRQ_CTL			0x64
#define  SCNT_EN			(1 << 7)
#define  SCNT_MODE			(1 << 6)

/*
 * IO decode enable macros are in the format IO_<peripheral>_<IO port>.
 * For example, to open ports 0x60, 0x64 for the keyboard controller,
 * use IOE_KBC_60_64 macro. For IOE_ macros that do not specify a port range,
 * the port range is selectable via the IO decodes register.
 */
#define REG_IO_DECODE			0x80
#define  IOD_COMA_RANGE			(0 << 0) /* 0x3F8 - 0x3FF COMA*/
#define  IOD_COMB_RANGE			(1 << 4) /* 0x2F8 - 0x2FF COMB*/
#define REG_IO_ENABLES			0x82
#define  IOE_EC_4E_4F			(1 << 13)
#define  IOE_SUPERIO_2E_2F		(1 << 12)
#define  IOE_EC_62_66			(1 << 11)
#define  IOE_KBC_60_64			(1 << 10)
#define  IOE_HGE_208			(1 << 9)
#define  IOE_LGE_200			(1 << 8)
#define  IOE_FDD_EN			(1 << 3)
#define  IOE_LPT_EN			(1 << 2)
#define  IOE_COMB_EN			(1 << 1)
#define  IOE_COMA_EN			(1 << 0)
#define REG_GENERIC_IO_RANGE(n)		((((n) & 0x3) * 4) + 0x84)
#define  LGIR_AMASK_MASK		(0xfc << 16)
#define  LGIR_ADDR_MASK			0xfffc
#define  LGIR_EN			(1 << 0)
#define LGIR_MAX_WINDOW_SIZE		256
#define NUM_GENERIC_IO_RANGES		4
#define REG_GENERIC_MEM_RANGE		0x98
#define  LGMR_ADDR_MASK			0xffff0000
#define  LGMR_EN			(1 << 0)
#define LGMR_WINDOW_SIZE		(64 * KiB)

/* Configure the SOC's LPC pads and mux them to the LPC function. */
void lpc_configure_pads(void);
/* Enable fixed IO ranges to LPC. IOE_* macros can be OR'ed together. */
void lpc_enable_fixed_io_ranges(uint16_t io_enables);
/* Open a generic IO window to the LPC bus. Four windows are available. */
void lpc_open_pmio_window(uint16_t base, uint16_t size);
/* Close all generic IO windows to the LPC bus. */
void lpc_close_pmio_windows(void);
/* Open a generic MMIO window to the LPC bus. One window is available. */
void lpc_open_mmio_window(uintptr_t base, size_t size);
/* Returns true if given window is decoded to LPC via a fixed range. */
bool lpc_fits_fixed_mmio_window(uintptr_t base, size_t size);

#endif /* _SOC_APOLLOLAKE_LPC_H_ */
