/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EARLY_VX900_H
#define EARLY_VX900_H

#include "raminit.h"
#include "vx900.h"

#include <arch/io.h>
#include <cbmem.h>
#include <stdint.h>
#include <arch/io.h>

/* North Module devices */
#define HOST_CTR PCI_DEV(0, 0, 0)
#define ERR_REP  PCI_DEV(0, 0, 1)
#define HOST_BUS PCI_DEV(0, 0, 2)
#define MCU      PCI_DEV(0, 0, 3)
#define POWERMAN PCI_DEV(0, 0, 4)
#define TRAF_CTR PCI_DEV(0, 0, 5)
#define NSBIC    PCI_DEV(0, 0, 7)

#define GFX      PCI_DEV(0, 1, 0)
#define HDMI     PCI_DEV(0, 1, 0)

#define PEXx     PCI_DEV(0, 3, x)
#define PEX_CTR  PCI_DEV(0, 3, 4)

/* South Module devices */
#define UARTx    PCI_DEV(0, 0x0a, x)
#define USB_MASS PCI_DEV(0, 0x0b, 0)
#define SDIO     PCI_DEV(0, 0x0c, 0)
#define CARD_RD  PCI_DEV(0, 0x0d, 0)
#define SATA     PCI_DEV(0, 0x0d, 0)
#define USBx     PCI_DEV(0, 0x10, x)
#define USB_EHCI PCI_DEV(0, 0x10, 4)
#define LPC      PCI_DEV(0, 0x11, 0)
#define PMU      LPC
#define SNMIC    PCI_DEV(0, 0x11, 7)
#define P2P      PCI_DEV(0, 0x13, 0)
#define HDAC     PCI_DEV(0, 0x14, 0)

/* These control the behavior of raminit */
#define RAMINIT_USE_HW_RXCR_CALIB	0
#define RAMINIT_USE_HW_MRS_SEQ		0


void enable_smbus(void);
void dump_spd_data(spd_raw_data spd);
void spd_read(u8 addr, spd_raw_data spd);

void vx900_enable_pci_config_space(void);
void vx900_disable_legacy_rom_shadow(void);

void vx900_print_strapping_info(void);
void vx900_disable_auto_reboot(void);

void vx900_cpu_bus_interface_setup(void);

void vx900_dram_set_gfx_resources(void);
void vx900_disable_gfx(void);

#endif				/* EARLY_VX900_H */
