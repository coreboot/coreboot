/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <console/console.h>

#include <arch/io.h>
#include <arch/acpi.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include "hudson.h"
#include "smbus.h"

/* Offsets from ACPI_MMIO_BASE
 * This is defined by AGESA, but we don't include AGESA headers to avoid
 * polluting the namesace.
 */
#define PM_MMIO_BASE 0xfed80300


#if CONFIG_HAVE_ACPI_RESUME
int acpi_get_sleep_type(void)
{
	u16 tmp = inw(PM1_CNT_BLK_ADDRESS);
	tmp = ((tmp & (7 << 10)) >> 10);
	/* printk(BIOS_DEBUG, "SLP_TYP type was %x\n", tmp); */
	return (int)tmp;
}
#endif

void backup_top_of_ram(uint64_t ramtop)
{
	u32 dword = (u32) ramtop;
	int nvram_pos = 0xf8, i; /* temp */
	/* printk(BIOS_DEBUG, "dword=%x\n", dword); */
	for (i = 0; i<4; i++) {
		/* printk(BIOS_DEBUG, "nvram_pos=%x, dword>>(8*i)=%x\n", nvram_pos, (dword >>(8 * i)) & 0xff); */
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >>(8 * i)) & 0xff , BIOSRAM_DATA);
		nvram_pos++;
	}
}

void set_sm_enable_bits(device_t sm_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_read_config32(sm_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(sm_dev, reg_pos, reg);
	}
}

void pm_write8(u8 reg, u8 value)
{
	write8(PM_MMIO_BASE + reg, value);
}

u8 pm_read8(u8 reg)
{
	return read8(PM_MMIO_BASE + reg);
}

void pm_write16(u8 reg, u16 value)
{
	write16(PM_MMIO_BASE + reg, value);
}

u16 pm_read16(u16 reg)
{
	return read16(PM_MMIO_BASE + reg);
}

void hudson_enable(device_t dev)
{
	printk(BIOS_DEBUG, "hudson_enable()\n");
	switch (dev->path.pci.devfn) {
	case (0x14 << 3) | 7: /* 0:14.7  SD */
		if (dev->enabled == 0) {
			// read the VENDEV ID
			device_t sd_dev = dev_find_slot( 0, PCI_DEVFN( 0x14, 7));
			u32 sd_device_id = pci_read_config32( sd_dev, 0) >> 16;
			/* turn off the SDHC controller in the PM reg */
			u8 reg8;
			if (sd_device_id == PCI_DEVICE_ID_AMD_HUDSON_SD) {
				reg8 = pm_read8(0xe7);
				reg8 &= ~(1 << 0);
				pm_write8(0xe7, reg8);
			}
			else if (sd_device_id == PCI_DEVICE_ID_AMD_YANGTZE_SD) {
				reg8 = pm_read8(0xe8);
				reg8 &= ~(1 << 0);
				pm_write8(0xe8, reg8);
			}
			/* remove device 0:14.7 from PCI space */
			reg8 = pm_read8(0xd3);
			reg8 &= ~(1 << 6);
			pm_write8(0xd3, reg8);
		}
		break;
	default:
		break;
	}
}

#if CONFIG_HAVE_ACPI_RESUME
unsigned long get_top_of_ram(void)
{
	uint32_t xdata = 0;
	int xnvram_pos = 0xf8, xi;
	if (acpi_get_sleep_type() != 3)
		return 0;
	for (xi = 0; xi<4; xi++) {
		outb(xnvram_pos, BIOSRAM_INDEX);
		xdata &= ~(0xff << (xi * 8));
		xdata |= inb(BIOSRAM_DATA) << (xi *8);
		xnvram_pos++;
	}
	return (unsigned long) xdata;
}
#endif

struct chip_operations southbridge_amd_agesa_hudson_ops = {
	CHIP_NAME("ATI HUDSON")
	.enable_dev = hudson_enable,
};
