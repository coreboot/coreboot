/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2016 Siemens AG
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>
#include <stdlib.h>

#include <soc/gfx.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

#define GFX_TIMEOUT 100000 /* 100ms */

static const struct reg_script gpu_pre_vbios_script[] = {
	/* Make sure GFX is bus master with MMIO access */
	REG_PCI_OR32(PCI_COMMAND, PCI_COMMAND_MASTER|PCI_COMMAND_MEMORY),
	/* Display */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xc0),
	REG_IOSF_POLL(IOSF_PORT_PMC, PUNIT_PWRGT_STATUS, 0xc0, 0xc0,
			GFX_TIMEOUT),
	/* Tx/Rx Lanes */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xfff0c0),
	REG_IOSF_POLL(IOSF_PORT_PMC, PUNIT_PWRGT_STATUS, 0xfff0c0, 0xfff0c0,
			GFX_TIMEOUT),
	/* Common Lane */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xfffcc0),
	REG_IOSF_POLL(IOSF_PORT_PMC, PUNIT_PWRGT_STATUS, 0xfffcc0, 0xfffcc0,
			GFX_TIMEOUT),
	/* Ungating Tx only */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xf00cc0),
	REG_IOSF_POLL(IOSF_PORT_PMC, PUNIT_PWRGT_STATUS, 0xfffcc0, 0xf00cc0,
			GFX_TIMEOUT),
	/* Ungating Common Lane only */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xf000c0),
	REG_IOSF_POLL(IOSF_PORT_PMC, PUNIT_PWRGT_STATUS, 0xffffc0, 0xf000c0,
			GFX_TIMEOUT),
	/* Ungating Display */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xf00000),
	REG_IOSF_POLL(IOSF_PORT_PMC, PUNIT_PWRGT_STATUS, 0xfffff0, 0xf00000,
			GFX_TIMEOUT),
	REG_SCRIPT_END
};

static const struct reg_script gfx_post_vbios_script[] = {
	/* Deassert Render Force-Wake */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x1300b0, 0x80000000),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x1300b4, 0x8000, 0, GFX_TIMEOUT),
	/* Deassert Media Force-Wake */
	REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x1300b8, 0x80000000),
	REG_RES_POLL32(PCI_BASE_ADDRESS_0, 0x1300bc, 0x8000, 0, GFX_TIMEOUT),
	/* Set Lock bits */
	REG_PCI_RMW32(GGC, 0xffffffff, 1),
	REG_PCI_RMW32(GSM_BASE, 0xffffffff, 1),
	REG_PCI_RMW32(GTT_BASE, 0xffffffff, 1),
	REG_SCRIPT_END
};

static inline void gfx_run_script(struct device *dev, const struct reg_script *ops)
{
	reg_script_run_on_dev(dev, ops);
}

static void gfx_pre_vbios_init(struct device *dev)
{
	printk(BIOS_INFO, "GFX: Pre VBIOS Init\n");
	gfx_run_script(dev, gpu_pre_vbios_script);
}

static void gfx_post_vbios_init(struct device *dev)
{
	printk(BIOS_INFO, "GFX: Post VBIOS Init\n");
	gfx_run_script(dev, gfx_post_vbios_script);
}

static void gfx_init(struct device *dev)
{
	/* Pre VBIOS Init */
	gfx_pre_vbios_init(dev);

	/* Run VBIOS */
	pci_dev_init(dev);

	/* Post VBIOS Init */
	gfx_post_vbios_init(dev);
}

static struct device_operations gfx_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= gfx_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver gfx_driver __pci_driver = {
	.ops	= &gfx_device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= GFX_DEVID,
};
