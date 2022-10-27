/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <intelblocks/p2sb.h>
#include <soc/iomap.h>

void p2sb_soc_get_sb_mask(uint32_t *ep_mask, size_t count)
{
	uint32_t mask;

	if (count != P2SB_EP_MASK_MAX_REG) {
		printk(BIOS_ERR, "Unable to program EPMASK registers\n");
		return;
	}

	/* Remove the host accessing right to PSF register range.
	 * Set p2sb PCI offset EPMASK5 [29, 28, 27, 26] to disable Sideband
	 * access for PCI Root Bridge.
	 */
	mask = (1 << 29) | (1 << 28) | (1 << 27)  | (1 << 26);

	ep_mask[P2SB_EP_MASK_5_REG] = mask;

	/*
	 * Set p2sb PCI offset EPMASK7 [31, 30] to disable Sideband
	 * access for Broadcast and Multicast.
	 */
	mask = (1 << 31) | (1 << 30);

	ep_mask[P2SB_EP_MASK_7_REG] = mask;
}

static void ioe_p2sb_read_resources(struct device *dev)
{
	/* Add the fixed MMIO resource for IOM */
	mmio_resource_kb(dev, 0, IOM_BASE_ADDR / KiB, IOM_BASE_SIZE / KiB);
}

static void p2sb_read_resources(struct device *dev)
{
	/*
	 * There's only one resource on the P2SB device. It's also already
	 * manually set to a fixed address in earlier boot stages.
	 * The following code makes sure that it doesn't change even the
	 * resource allocator is being run.
	 */
	mmio_range(dev, PCI_BASE_ADDRESS_0, P2SB_BAR, P2SB_SIZE);
}

struct device_operations ioe_p2sb_ops = {
	.read_resources   = ioe_p2sb_read_resources,
	.set_resources    = noop_set_resources,
	.scan_bus         = scan_static_bus,
};

struct device_operations soc_p2sb_ops = {
	.read_resources   = p2sb_read_resources,
	.set_resources    = noop_set_resources,
	.scan_bus         = scan_static_bus,
};
