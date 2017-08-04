/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
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

#ifndef _SOC_COMMON_BLOCK_LPC_LIB_H_
#define _SOC_COMMON_BLOCK_LPC_LIB_H_

#include <device/device.h>
#include <stddef.h>
#include <stdint.h>

/*
 * IO decode enable macros are in the format IO_<peripheral>_<IO port>.
 * For example, to open ports 0x60, 0x64 for the keyboard controller,
 * use IOE_KBC_60_64 macro. For IOE_ macros that do not specify a port range,
 * the port range is selectable via the IO decodes register.
 */
#define  LPC_IOE_EC_4E_4F               (1 << 13)
#define  LPC_IOE_SUPERIO_2E_2F          (1 << 12)
#define  LPC_IOE_EC_62_66               (1 << 11)
#define  LPC_IOE_KBC_60_64              (1 << 10)
#define  LPC_IOE_HGE_208                (1 << 9)
#define  LPC_IOE_LGE_200                (1 << 8)
#define  LPC_IOE_FDD_EN                 (1 << 3)
#define  LPC_IOE_LPT_EN                 (1 << 2)
#define  LPC_IOE_COMB_EN                (1 << 1)
#define  LPC_IOE_COMA_EN                (1 << 0)

/* Serial IRQ control. SERIRQ_QUIET is the default (0). */
enum serirq_mode {
	SERIRQ_QUIET,
	SERIRQ_CONTINUOUS,
	SERIRQ_OFF,
};

struct lpc_mmio_range {
	uintptr_t base;
	size_t size;
};

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
/* Init SoC Spcific LPC features. Common definition will be weak and
each soc will need to define the init. */
void lpc_init(struct device *dev);
/* Init LPC GPIO pads */
void lpc_configure_pads(void);
/* Get SoC speicific MMIO ranges */
const struct lpc_mmio_range *soc_get_fixed_mmio_ranges(void);
/* Set LPC BIOS Control BILD bit. */
void lpc_set_bios_interface_lock_down(void);
/* Set LPC BIOS Control LE bit. */
void lpc_set_lock_enable(void);
/* Set LPC BIOS Control EISS bit. */
void lpc_set_eiss(void);
/* Set LPC Serial IRQ mode. */
void lpc_set_serirq_mode(enum serirq_mode mode);
/*
* Setup I/O Decode Range Register for LPC
* ComA Range 3F8h-3FFh [2:0]
* ComB Range 2F8h-2FFh [6:4]
* Enable ComA and ComB Port
*/
void lpc_io_setup_comm_a_b(void);

#endif /* _SOC_COMMON_BLOCK_LPC_LIB_H_ */
