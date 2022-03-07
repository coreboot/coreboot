/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <southbridge/intel/common/pciehp.h>
#include <assert.h>

#include "chip.h"
#include "pch.h"

static const char *pch_pcie_acpi_name(const struct device *dev)
{
	ASSERT(dev);

	if (PCI_SLOT(dev->path.pci.devfn) == 0x1c) {
		static const char *names[] = { "RP01",
				"RP02",
				"RP03",
				"RP04",
				"RP05",
				"RP06",
				"RP07",
				"RP08"};

		return names[PCI_FUNC(dev->path.pci.devfn)];
	}

	return NULL;
}

static void pch_pcie_pm_early(struct device *dev)
{
	u16 link_width_p0, link_width_p4;
	u8 slot_power_limit = 10; /* 10W for x1 */
	u32 reg32;
	u8 reg8;

	reg32 = RCBA32(RPC);

	/* Port 0-3 link aggregation from PCIEPCS1[1:0] soft strap */
	switch (reg32 & 3) {
	case 3:
		link_width_p0 = 4;
		break;
	case 1:
	case 2:
		link_width_p0 = 2;
		break;
	case 0:
	default:
		link_width_p0 = 1;
	}

	/* Port 4-7 link aggregation from PCIEPCS2[1:0] soft strap */
	switch ((reg32 >> 2) & 3) {
	case 3:
		link_width_p4 = 4;
		break;
	case 1:
	case 2:
		link_width_p4 = 2;
		break;
	case 0:
	default:
		link_width_p4 = 1;
	}

	/* Enable dynamic clock gating where needed */
	reg8 = pci_read_config8(dev, 0xe1);
	switch (PCI_FUNC(dev->path.pci.devfn)) {
	case 0: /* Port 0 */
		if (link_width_p0 == 4)
			slot_power_limit = 40; /* 40W for x4 */
		else if (link_width_p0 == 2)
			slot_power_limit = 20; /* 20W for x2 */
		reg8 |= 0x3f;
		break;
	case 4: /* Port 4 */
		if (link_width_p4 == 4)
			slot_power_limit = 40; /* 40W for x4 */
		else if (link_width_p4 == 2)
			slot_power_limit = 20; /* 20W for x2 */
		reg8 |= 0x3f;
		break;
	case 1: /* Port 1 only if Port 0 is x1 */
		if (link_width_p0 == 1)
			reg8 |= 0x3;
		break;
	case 2: /* Port 2 only if Port 0 is x1 or x2 */
	case 3: /* Port 3 only if Port 0 is x1 or x2 */
		if (link_width_p0 <= 2)
			reg8 |= 0x3;
		break;
	case 5: /* Port 5 only if Port 4 is x1 */
		if (link_width_p4 == 1)
			reg8 |= 0x3;
		break;
	case 6: /* Port 7 only if Port 4 is x1 or x2 */
	case 7: /* Port 7 only if Port 4 is x1 or x2 */
		if (link_width_p4 <= 2)
			reg8 |= 0x3;
		break;
	}
	pci_write_config8(dev, 0xe1, reg8);

	/* Set 0xE8[0] = 1 */
	pci_or_config32(dev, 0xe8, 1);

	/* Adjust Common Clock exit latency */
	reg32 = pci_read_config32(dev, 0xd8);
	reg32 &= ~(1 << 17);
	reg32 |= (1 << 16) | (1 << 15);
	reg32 &= ~(1 << 31); /* Disable PME# SCI for native PME handling */
	pci_write_config32(dev, 0xd8, reg32);

	/* Adjust ASPM L1 exit latency */
	reg32 = pci_read_config32(dev, 0x4c);
	reg32 &= ~((1 << 17) | (1 << 16) | (1 << 15));
	if (RCBA32(CIR9) & (1 << 16)) {
		/* If RCBA+2320[15]=1 set ASPM L1 to 8-16us */
		reg32 |= (1 << 17);
	} else {
		/* Else set ASPM L1 to 2-4us */
		reg32 |= (1 << 16);
	}
	pci_write_config32(dev, 0x4c, reg32);

	/* Set slot power limit as configured above */
	reg32 = pci_read_config32(dev, 0x54);
	reg32 &= ~((1 << 15) | (1 << 16)); /* 16:15 = Slot power scale */
	reg32 &= ~(0xff << 7);             /* 14:7  = Slot power limit */
	reg32 |= (slot_power_limit << 7);
	pci_write_config32(dev, 0x54, reg32);
}

static void pch_pcie_pm_late(struct device *dev)
{
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;
	enum aspm_type apmc = 0;

	/* Set 0x314 = 0x743a361b */
	pci_write_config32(dev, 0x314, 0x743a361b);

	/* Set 0x318[31:16] = 0x1414 */
	pci_update_config32(dev, 0x318, 0x0000ffff, 0x14140000);

	/* Set 0x324[5] = 1 */
	pci_or_config32(dev, 0x324, 1 << 5);

	/* Set 0x330[7:0] = 0x40 */
	pci_update_config32(dev, 0x330, ~0xff, 0x40);

	/* Set 0x33C[24:0] = 0x854c74 */
	pci_update_config32(dev, 0x33c, 0xff000000, 0x00854c74);

	/* No IO-APIC, Disable EOI forwarding */
	pci_or_config32(dev, 0xd4, 1 << 1);

	/* Check for a rootport ASPM override */
	apmc = config->pcie_aspm[PCI_FUNC(dev->path.pci.devfn)];

	/* Setup the override or get the real ASPM setting */
	if (apmc) {
		pci_or_config32(dev, 0xd4, (apmc << 2) | (1 << 4));

	} else {
		apmc = pci_read_config32(dev, 0x50) & 3;
	}

	/* If both L0s and L1 enabled then set root port 0xE8[1]=1 */
	if (apmc == PCIE_ASPM_BOTH)
		pci_or_config32(dev, 0xe8, 1 << 1);
}

static void pci_init(struct device *dev)
{
	u16 reg16;
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "Initializing PCH PCIe bridge.\n");

	/* Enable Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, 0x0c, 0x10);

	pci_and_config16(dev, PCI_BRIDGE_CONTROL, ~PCI_BRIDGE_CTL_PARITY);

	/* Clear errors in status registers. FIXME: Do something? */
	reg16 = pci_read_config16(dev, 0x06);
	//reg16 |= 0xf900;
	pci_write_config16(dev, 0x06, reg16);

	reg16 = pci_read_config16(dev, 0x1e);
	//reg16 |= 0xf900;
	pci_write_config16(dev, 0x1e, reg16);

	/* Enable expresscard hotplug events.  */
	if (config->pcie_hotplug_map[PCI_FUNC(dev->path.pci.devfn)]) {
		pci_or_config32(dev, 0xd8, 1 << 30);
		pci_write_config16(dev, 0x42, 0x142);
	}
}

static void pch_pcie_enable(struct device *dev)
{
	/* Power Management init before enumeration */
	pch_pcie_pm_early(dev);
}

static void pch_pciexp_scan_bridge(struct device *dev)
{
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;

	if (CONFIG(PCIEXP_HOTPLUG) && config->pcie_hotplug_map[PCI_FUNC(dev->path.pci.devfn)]) {
		pciexp_hotplug_scan_bridge(dev);
	} else {
		/* Normal PCIe Scan */
		pciexp_scan_bridge(dev);
	}

	/* Late Power Management init after bridge device enumeration */
	pch_pcie_pm_late(dev);
}

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.enable			= pch_pcie_enable,
	.scan_bus		= pch_pciexp_scan_bridge,
	.acpi_name		= pch_pcie_acpi_name,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = { 0x1c10, 0x1c12, 0x1c14, 0x1c16,
						 0x1c18, 0x1c1a, 0x1c1c, 0x1c1e,
						 0x1e10, 0x1e12, 0x1e14, 0x1e16,
						 0x1e18, 0x1e1a, 0x1e1c, 0x1e1e,
						 0 };

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
