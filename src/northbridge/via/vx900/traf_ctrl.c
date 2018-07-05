/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <drivers/generic/ioapic/chip.h>

#include "vx900.h"
#include "chip.h"

/**
 * @file vx900/traf_ctrl.c
 *
 * STATUS:
 * The same issues with the IOAPIC pointe in lpc.c also apply here.
 *
 * We need to check if the current PCIE lane configuration mechanism is sane.
 */

/**
 * \brief VX900: Set up the north module IOAPIC (for PCIE and VGA)
 *
 * Enable the IOAPIC in the south module, and properly set it up.
 * \n
 * This is the hardware specific initialization for the IOAPIC, and complements
 * the setup done by the generic IOAPIC driver. In order for the IOAPIC to work
 * properly, it _must_ be declared in devicetree.cb .
 * \n
 * We are assuming this is called before the drivers/generic/ioapic code,
 * which should be the case if devicetree.cb is set up properly.
 */
static void vx900_north_ioapic_setup(struct device *dev)
{
	u8 base_val;
	struct device *ioapic;
	ioapic_config_t *config;
	/* Find the IOAPIC, and make sure it's set up correctly in devicetree.cb
	 * If it's not, then the generic ioapic driver will not set it up
	 * correctly, and the MP table will not be correctly generated */
	for (ioapic = dev->next; ioapic; ioapic = ioapic->next) {
		if (ioapic->path.type == DEVICE_PATH_IOAPIC)
			break;
	}
	/* You did put an IOAPIC in devicetree.cb, didn't you? */
	if (ioapic == 0) {
		/* We don't have enough info to set up the IOAPIC */
		printk(BIOS_ERR, "ERROR: North module IOAPIC not found. "
		       "Check your devicetree.cb\n");
		return;
	}
	/* Found our IOAPIC, and it should not carry ISA interrupts */
	config = (ioapic_config_t *) ioapic->chip_info;
	if (config->have_isa_interrupts) {
		/* Umh, is this the right IOAPIC ? */
		printk(BIOS_ERR, "ERROR: North module IOAPIC should not carry "
		       "ISA interrupts.\n" "Check your devicetree.cb\n");
		printk(BIOS_ERR, "Will not initialize this IOAPIC.\n");
		return;
	}
	/* The base address of this IOAPIC _must_
	 *     be between 0xfec00000 and 0xfecfff00
	 *     be 256-byte aligned
	 */
	if ((config->base < (void *)0xfec0000 || config->base > (void *)0xfecfff00)
	    || (((uintptr_t)config->base & 0xff) != 0)) {
		printk(BIOS_ERR, "ERROR: North module IOAPIC base should be "
		       "between 0xfec00000 and 0xfecfff00\n"
		       "and must be aligned to a 256-byte boundary, "
		       "but we found it at 0x%p\n", config->base);
		return;
	}

	printk(BIOS_DEBUG, "VX900 TRAF_CTR: Setting up the north module IOAPIC "
	       "at %p\n", config->base);

	/* First register of the IOAPIC base */
	base_val = (((uintptr_t)config->base) >> 8) & 0xff;
	pci_write_config8(dev, 0x41, base_val);
	/* Second register of the base.
	 * Bit[7] also enables the IOAPIC and bit[5] enables MSI cycles */
	base_val = (((uintptr_t)config->base) >> 16) & 0xf;
	pci_mod_config8(dev, 0x40, 0, base_val | (1 << 7) | (1 << 5));
}

/*
 * Configures the PCI-express ports
 *
 * FIXME: triple-quadruple-check this
 */
static void vx900_pex_link_setup(struct device *dev)
{
	u8 reg8;
	struct northbridge_via_vx900_config *nb = (void *)dev->chip_info;

	reg8 = pci_read_config8(dev, 0xb0);
	reg8 &= ~((1 << 7) | (1 << 3));

	if (nb->assign_pex_to_dp)
		reg8 |= (1 << 7);

	if (!nb->pcie_port1_2_lane_wide)
		reg8 |= (1 << 3);

	pci_write_config8(dev, 0xb0, reg8);
}

static void vx900_traf_ctr_init(struct device *dev)
{
	vx900_north_ioapic_setup(dev);
	vx900_pex_link_setup(dev);
}

static struct device_operations traf_ctrl_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vx900_traf_ctr_init,
	/* Need this here, or the IOAPIC driver won't be called.
	 * FIXME: Technically not a LPC bus. */
	.scan_bus = scan_lpc_bus,
};

static const struct pci_driver traf_ctrl_driver __pci_driver = {
	.ops = &traf_ctrl_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_TRAF,
};
