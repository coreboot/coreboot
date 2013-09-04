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

static void pmio_write_index(u16 port_base, u8 reg, u8 value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static u8 pmio_read_index(u16 port_base, u8 reg)
{
	outb(reg, port_base);
	return inb(port_base + 1);
}

void pm_iowrite(u8 reg, u8 value)
{
	pmio_write_index(PM_INDEX, reg, value);
}

u8 pm_ioread(u8 reg)
{
	return pmio_read_index(PM_INDEX, reg);
}

void pm2_iowrite(u8 reg, u8 value)
{
	pmio_write_index(PM2_INDEX, reg, value);
}

u8 pm2_ioread(u8 reg)
{
	return pmio_read_index(PM2_INDEX, reg);
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
			u8 sd_tmp;
			if (sd_device_id == PCI_DEVICE_ID_AMD_HUDSON_SD) {
				outb(0xE7, PM_INDEX);
				sd_tmp = inb(PM_DATA);
				sd_tmp &= ~(1 << 0);
				outb(sd_tmp, PM_DATA);
			}
			else if (sd_device_id == PCI_DEVICE_ID_AMD_YANGTZE_SD) {
				outb(0xE8, PM_INDEX);
				sd_tmp = inb(PM_DATA);
				sd_tmp &= ~(1 << 0);
				outb(sd_tmp, PM_DATA);
			}
			/* remove device 0:14.7 from PCI space */
			outb(0xD3, PM_INDEX);
			sd_tmp = inb(PM_DATA);
			sd_tmp &= ~(1 << 6);
			outb(sd_tmp, PM_DATA);
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
