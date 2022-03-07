/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/p2sblib.h>
#include <soc/iomap.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <string.h>

#define PCH_P2SB_EPMASK(mask_number) (PCH_P2SB_EPMASK0 + ((mask_number) * 4))

void p2sb_enable_bar(void)
{
	/* Enable PCR Base address in PCH */
	pci_write_config32(PCH_DEV_P2SB, PCI_BASE_ADDRESS_0, P2SB_BAR);
	pci_write_config32(PCH_DEV_P2SB, PCI_BASE_ADDRESS_1, 0);

	/* Enable P2SB MSE */
	pci_write_config16(PCH_DEV_P2SB, PCI_COMMAND,
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
	pci_write_config8(PCH_DEV_P2SB, HPTC_OFFSET, HPTC_ADDR_ENABLE_BIT);
}

union p2sb_bdf p2sb_get_hpet_bdf(void)
{
	const bool was_hidden = p2sb_dev_is_hidden(PCH_DEV_P2SB);
	if (was_hidden)
		p2sb_unhide();

	union p2sb_bdf bdf = { .raw = pci_read_config16(PCH_DEV_P2SB, PCH_P2SB_HBDF) };

	if (was_hidden)
		p2sb_hide();

	return bdf;
}

void p2sb_set_hpet_bdf(union p2sb_bdf bdf)
{
	pci_write_config16(PCH_DEV_P2SB, PCH_P2SB_HBDF, bdf.raw);
}

union p2sb_bdf p2sb_get_ioapic_bdf(void)
{
	const bool was_hidden = p2sb_dev_is_hidden(PCH_DEV_P2SB);
	if (was_hidden)
		p2sb_unhide();

	union p2sb_bdf bdf = { .raw = pci_read_config16(PCH_DEV_P2SB, PCH_P2SB_IBDF) };

	if (was_hidden)
		p2sb_hide();

	return bdf;
}

void p2sb_set_ioapic_bdf(union p2sb_bdf bdf)
{
	pci_write_config16(PCH_DEV_P2SB, PCH_P2SB_IBDF, bdf.raw);
}

void p2sb_unhide(void)
{
	p2sb_dev_unhide(PCH_DEV_P2SB);
}

void p2sb_hide(void)
{
	p2sb_dev_hide(PCH_DEV_P2SB);
}

static void p2sb_configure_endpoints(int epmask_id, uint32_t mask)
{
	uint32_t reg32;

	reg32 = pci_read_config32(PCH_DEV_P2SB, PCH_P2SB_EPMASK(epmask_id));
	pci_write_config32(PCH_DEV_P2SB, PCH_P2SB_EPMASK(epmask_id),
			reg32 | mask);
}

static void p2sb_lock_endpoints(void)
{
	uint8_t reg8;

	/* Set the "Endpoint Mask Lock!", P2SB PCI offset E2h bit[1] to 1. */
	reg8 = pci_read_config8(PCH_DEV_P2SB, PCH_P2SB_E0 + 2);
	pci_write_config8(PCH_DEV_P2SB, PCH_P2SB_E0 + 2,
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
	 * The following code makes sure that it doesn't change if the device
	 * is visible and the resource allocator is being run.
	 */
	mmio_resource(dev, PCI_BASE_ADDRESS_0, P2SB_BAR / KiB, P2SB_SIZE / KiB);
}

static const struct device_operations device_ops = {
	.read_resources		= read_resources,
	.set_resources		= noop_set_resources,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_APL_P2SB,
	PCI_DID_INTEL_GLK_P2SB,
	PCI_DID_INTEL_LWB_P2SB,
	PCI_DID_INTEL_LWB_P2SB_SUPER,
	PCI_DID_INTEL_SKL_LP_P2SB,
	PCI_DID_INTEL_SKL_P2SB,
	PCI_DID_INTEL_KBL_P2SB,
	PCI_DID_INTEL_CNL_P2SB,
	PCI_DID_INTEL_CNP_H_P2SB,
	PCI_DID_INTEL_ICL_P2SB,
	PCI_DID_INTEL_CMP_P2SB,
	PCI_DID_INTEL_CMP_H_P2SB,
	PCI_DID_INTEL_TGL_P2SB,
	PCI_DID_INTEL_TGL_H_P2SB,
	PCI_DID_INTEL_EHL_P2SB,
	PCI_DID_INTEL_JSP_P2SB,
	PCI_DID_INTEL_ADP_P_P2SB,
	PCI_DID_INTEL_ADP_S_P2SB,
	PCI_DID_INTEL_ADP_M_P2SB,
	0,
};

static const struct pci_driver pmc __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
