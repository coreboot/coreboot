/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <assert.h>
#include <types.h>

#include "chip.h"
#include "haswell.h"

#if CONFIG(HAVE_ACPI_TABLES)
static const char *pcie_acpi_name(const struct device *dev)
{
	assert(dev);

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	assert(dev->bus);
	if (dev->bus->secondary == 0)
		switch (dev->path.pci.devfn) {
		case PCI_DEVFN(1, 0):
			return "PEGP";
		case PCI_DEVFN(1, 1):
			return "PEG1";
		case PCI_DEVFN(1, 2):
			return "PEG2";
		};

	struct device *const port = dev->bus->dev;
	assert(port);
	assert(port->bus);

	if (dev->path.pci.devfn == PCI_DEVFN(0, 0) &&
	    port->bus->secondary == 0 &&
	    (port->path.pci.devfn == PCI_DEVFN(1, 0) ||
	    port->path.pci.devfn == PCI_DEVFN(1, 1) ||
	    port->path.pci.devfn == PCI_DEVFN(1, 2)))
		return "DEV0";

	return NULL;
}
#endif

static const struct peg_config *get_peg_config(struct device *dev, const uint8_t func)
{
	static const struct peg_config default_config = { 0 };

	if (!dev || !dev->chip_info)
		return &default_config;

	const struct northbridge_intel_haswell_config *config = dev->chip_info;

	if (func >= ARRAY_SIZE(config->peg_cfg)) {
		printk(BIOS_ERR, "%s: Found PEG function %u, which doesn't exist on Haswell\n",
		       __func__, func);
		return &default_config;
	}
	return &config->peg_cfg[func];
}

static void peg_enable(struct device *dev)
{
	const uint8_t func = PCI_FUNC(dev->path.pci.devfn);

	const struct peg_config *peg_cfg = get_peg_config(dev, func);

	const bool slot_implemented = !peg_cfg->is_onboard;

	if (slot_implemented) {
		/* Default is 1, but register is R/WO and needs to be written to once */
		pci_or_config16(dev, PEG_CAP, 1 << 8);
	} else {
		pci_and_config16(dev, PEG_CAP, ~(1 << 8));
	}

	/* Note: this register is write-once */
	uint32_t slotcap = pci_read_config32(dev, PEG_SLOTCAP);

	/* Physical slot number (zero for ports connected to onboard devices) */
	slotcap &= ~(0x1fff << 19);
	if (slot_implemented) {
		uint16_t slot_number = peg_cfg->phys_slot_number & 0x1fff;
		if (slot_number == 0) {
			/* Slot number must be non-zero and unique */
			slot_number = func + 1;
		}
		slotcap |= slot_number << 19;
	}

	/* Default to 1.0 watt scale */
	slotcap &= ~(3 << 15);
	slotcap |= (peg_cfg->power_limit_scale & 3) << 15;

	uint8_t power_limit_value = peg_cfg->power_limit_value;
	if (power_limit_value == 0) {
		/* Default to 75 watts */
		power_limit_value = 75;
	}
	slotcap &= ~(0xff << 7);
	slotcap |= power_limit_value << 7;

	pci_write_config32(dev, PEG_SLOTCAP, slotcap);

	/* Clear errors */
	pci_write_config16(dev, PCI_STATUS, 0xffff);
	pci_write_config16(dev, PCI_SEC_STATUS, 0xffff);
	pci_write_config16(dev, PEG_DSTS, 0xffff);
	pci_write_config32(dev, PEG_UESTS, 0xffffffff);
	pci_write_config32(dev, PEG_CESTS, 0xffffffff);
	pci_write_config32(dev, 0x1f0, 0xffffffff);

	pci_or_config32(dev, PEG_VC0RCTL, 0x7f << 1);

	/* Advertise OBFF support using WAKE# signaling only */
	pci_or_config32(dev, PEG_DCAP2, 1 << 19);

	pci_or_config32(dev, PEG_UESEV, 1 << 14);

	/* Select -3.5 dB de-emphasis */
	pci_or_config32(dev, PEG_LCTL2, 1 << 6);

	pci_or_config32(dev, PEG_L0SLAT, 1 << 31);

	pci_update_config32(dev, 0x250, ~(7 << 20), 2 << 20);

	pci_or_config32(dev, 0x238, 1 << 29);

	pci_or_config32(dev, 0x1f8, 1 << 16);

	pci_update_config32(dev, PEG_AFE_PM_TMR, ~0x1f, 0x13);

	/* Lock DCAP */
	pci_update_config32(dev, PEG_DCAP,  ~0, 0);

	if (func == 0)
		pci_or_config32(dev, 0xcd0, 1 << 11);

	/* Enable support for L0s and L1 */
	pci_or_config32(dev, PEG_LCAP, 3 << 10);

	pci_and_config32(dev, 0x200, ~(3 << 26));

	/* Other fields in this register must not be changed while writing this */
	pci_or_config16(dev, 0x258, 1 << 2);
}

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.enable			= peg_enable,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= pcie_acpi_name,
#endif
};

static const unsigned short pci_device_ids[] = {
	0x0c01, 0x0c05, 0x0c09, 0x0c0d,
	0x0d01, 0x0d05, 0x0d09, /* Crystal Well */
	0 };

static const struct pci_driver pch_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
