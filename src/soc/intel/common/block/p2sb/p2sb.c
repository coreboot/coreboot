/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2018 Intel Corporation.
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/p2sb.h>
#include <rules.h>
#include <soc/iomap.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <string.h>

#define PCH_P2SB_EPMASK(mask_number) (PCH_P2SB_EPMASK0 + ((mask_number) * 4))

#define HIDE_BIT (1 << 0)

#if defined(__SIMPLE_DEVICE__)
static pci_devfn_t p2sb_get_device(void)
{
	int devfn = PCH_DEVFN_P2SB;
	pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	if (dev == PCI_DEV_INVALID)
		die("PCH_DEV_P2SB not found!\n");

	return dev;
}
#else
static struct device *p2sb_get_device(void)
{
	struct device *dev = PCH_DEV_P2SB;
	if (!dev)
		die("PCH_DEV_P2SB not found!\n");

	return dev;
}
#endif

#define P2SB_GET_DEV p2sb_get_device()

void p2sb_enable_bar(void)
{
	/* Enable PCR Base address in PCH */
	pci_write_config32(P2SB_GET_DEV, PCI_BASE_ADDRESS_0, P2SB_BAR);
	pci_write_config32(P2SB_GET_DEV, PCI_BASE_ADDRESS_1, 0);

	/* Enable P2SB MSE */
	pci_write_config8(P2SB_GET_DEV, PCI_COMMAND,
			  PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

/*
 * Enable decoding for HPET range.
 * This is needed for FspMemoryInit to store and retrieve a global data
 * pointer.
 */
void p2sb_configure_hpet(void)
{
	/*
	 * Enable decoding for HPET memory address range.
	 * HPTC_OFFSET(0x60) bit 7, when set the P2SB will decode
	 * the High Performance Timer memory address range
	 * selected by bits 1:0
	 */
	pci_write_config8(P2SB_GET_DEV, HPTC_OFFSET, HPTC_ADDR_ENABLE_BIT);
}

static void p2sb_set_hide_bit(int hide)
{
	const uint16_t reg = PCH_P2SB_E0 + 1;
	const uint8_t mask = HIDE_BIT;
	uint8_t val;

	val = pci_read_config8(P2SB_GET_DEV, reg);
	val &= ~mask;
	if (hide)
		val |= mask;
	pci_write_config8(P2SB_GET_DEV, reg, val);
}

void p2sb_unhide(void)
{
	p2sb_set_hide_bit(0);

	if (pci_read_config16(P2SB_GET_DEV, PCI_VENDOR_ID) !=
			PCI_VENDOR_ID_INTEL)
		die("Unable to unhide PCH_DEV_P2SB device !\n");
}

void p2sb_hide(void)
{
	p2sb_set_hide_bit(1);

	if (pci_read_config16(P2SB_GET_DEV, PCI_VENDOR_ID) !=
			0xFFFF)
		die("Unable to hide PCH_DEV_P2SB device !\n");
}

static void p2sb_configure_endpoints(int epmask_id, uint32_t mask)
{
	uint32_t reg32;

	reg32 = pci_read_config32(P2SB_GET_DEV, PCH_P2SB_EPMASK(epmask_id));
	pci_write_config32(P2SB_GET_DEV, PCH_P2SB_EPMASK(epmask_id),
			reg32 | mask);
}

static void p2sb_lock_endpoints(void)
{
	uint8_t reg8;

	/* Set the "Endpoint Mask Lock!", P2SB PCI offset E2h bit[1] to 1. */
	reg8 = pci_read_config8(P2SB_GET_DEV, PCH_P2SB_E0 + 2);
	pci_write_config8(P2SB_GET_DEV, PCH_P2SB_E0 + 2,
			reg8 | P2SB_E0_MASKLOCK);
}

void p2sb_disable_sideband_access(void)
{
	uint32_t ep_mask[P2SB_EP_MASK_MAX_REG];
	int i;

	memset(ep_mask, 0, sizeof(ep_mask));

	p2sb_soc_get_sb_mask(ep_mask, ARRAY_SIZE(ep_mask));

	/* Remove the host accessing right to PSF register range. */
	for (i = 0; i < P2SB_EP_MASK_MAX_REG; i++)
		p2sb_configure_endpoints(i, ep_mask[i]);

	p2sb_lock_endpoints();
}

static void read_resources(struct device *dev)
{
	/*
	 * There's only one resource on the P2SB device. It's also already
	 * manually set to a fixed address in earlier boot stages.
	 */
	mmio_resource(dev, PCI_BASE_ADDRESS_0, P2SB_BAR / KiB, P2SB_SIZE / KiB);
}

static const struct device_operations device_ops = {
	.read_resources		= read_resources,
	.set_resources		= DEVICE_NOOP,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_APL_P2SB,
	PCI_DEVICE_ID_INTEL_GLK_P2SB,
	PCI_DEVICE_ID_INTEL_CNL_P2SB,
	0,
};

static const struct pci_driver pmc __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
