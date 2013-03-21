/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include "Platform.h"
#include "sb_cimx.h"
#include "sb700_cfg.h"                /*sb700_cimx_config*/
#include <console/console.h>
#include <console/loglevel.h>
#include "smbus.h"

#if CONFIG_RAMINIT_SYSINFO
/**
 * @brief Get SouthBridge device number
 * @param[in] bus target bus number
 * @return southbridge device number
 */
u32 get_sbdn(u32 bus)
{
	device_t dev;

	printk(BIOS_SPEW, "SB700 - Early.c - %s - Start.\n", __func__);
	dev = pci_locate_device_on_bus(
			PCI_ID(PCI_VENDOR_ID_ATI, PCI_DEVICE_ID_ATI_SB700_SM),
			bus);

	printk(BIOS_SPEW, "SB700 - Early.c - %s - End.\n", __func__);
	return (dev >> 15) & 0x1f;
}
#endif

/**
 * @brief Enable A-Link Express Configuration DMA Access.
 */

/**
 * @brief South Bridge CIMx romstage entry,
 *        wrapper of sbPowerOnInit entry point.
 */
void sb_Poweron_Init(void)
{
	AMDSBCFG sb_early_cfg;

	printk(BIOS_SPEW, "cimx/sb700 early.c, %s() Start:\n", __func__);
	/* Enable A-Link Base Address */
	//sb_enable_alink ();

	sb700_cimx_config(&sb_early_cfg);
	sbPowerOnInit(&sb_early_cfg);
	printk(BIOS_SPEW, "cimx/sb700 early.c, %s() End\n", __func__);
}

void sb7xx_51xx_enable_wideio(u8 wio_index, u16 base)
{
	/* TODO: Now assume wio_index=0 */
	device_t dev;
	u8 reg8;

	//dev = pci_locate_device(PCI_ID(0x1002, 0x439d), 0);	/* LPC Controller */
	dev = PCI_DEV(0, 0x14, 3);	/* LPC Controller */
	pci_write_config32(dev, 0x64, base);
	reg8 = pci_read_config8(dev, 0x48);
	reg8 |= 1 << 2;
	pci_write_config8(dev, 0x48, reg8);
}

void sb7xx_51xx_disable_wideio(u8 wio_index)
{
	/* TODO: Now assume wio_index=0 */
	device_t dev;
	u8 reg8;

	//dev = pci_locate_device(PCI_ID(0x1002, 0x439d), 0);	/* LPC Controller */
	dev = PCI_DEV(0, 0x14, 3);	/* LPC Controller */
	pci_write_config32(dev, 0x64, 0);
	reg8 = pci_read_config8(dev, 0x48);
	reg8 &= ~(1 << 2);
	pci_write_config8(dev, 0x48, reg8);
}

