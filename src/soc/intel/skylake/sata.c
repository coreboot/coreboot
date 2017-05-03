/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <soc/ramstage.h>

static void *get_ahci_bar(void)
{
	device_t dev = PCH_DEV_SATA;
	uint32_t bar;

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_5);

	return (void *)(bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK);
}

/*
 * SATA Port control and Status. By default, the SATA ports are set (by HW)
 * to the disabled state (e.g. bits[3:0] == '0') as a result of an initial
 * power on reset. When enabled by software as per SATA port mapping,
 * the ports can transition between the on, partial and slumber states
 * and can detect devices. When disabled, the port is in the off state and
 * can't detect any devices.
 */
static void sata_final(device_t dev)
{
	void *ahcibar = get_ahci_bar();
	u32 port_impl, temp;

	dev = PCH_DEV_SATA;
	/* Read Ports Implemented (GHC_PI) */
	port_impl = read32(ahcibar + 0x0c) & 0x07;
	/* Port enable */
	temp = pci_read_config32(dev, 0x92);
	temp |= port_impl;
	pci_write_config32(dev, 0x92, temp);

}

static struct device_operations sata_ops = {
	.read_resources		  = &pci_dev_read_resources,
	.set_resources		  = &pci_dev_set_resources,
	.enable_resources	  = &pci_dev_enable_resources,
	.final			  = sata_final,
	.ops_pci		  = &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	0x9d03, /* SKL-U Base */
	0x9d07, /* SKL-Y Premium, SKL-U Premium */
	0x282a, /* KBL */
	0
};

static const struct pci_driver pch_sata __pci_driver = {
	.ops	 = &sata_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices	 = pci_device_ids,
};
