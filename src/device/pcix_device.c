/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pcix.h>
#include <stdint.h>

static void pcix_tune_dev(struct device *dev)
{
	u32 status;
	u16 orig_cmd, cmd;
	unsigned int cap, max_read, max_tran;

	if (dev->hdr_type != PCI_HEADER_TYPE_NORMAL)
		return;

	cap = pci_find_capability(dev, PCI_CAP_ID_PCIX);
	if (!cap)
		return;

	printk(BIOS_DEBUG, "%s PCI-X tuning\n", dev_path(dev));

	status = pci_read_config32(dev, cap + PCI_X_STATUS);
	orig_cmd = cmd = pci_read_config16(dev, cap + PCI_X_CMD);

	max_read = (status & PCI_X_STATUS_MAX_READ) >> 21;
	max_tran = (status & PCI_X_STATUS_MAX_SPLIT) >> 23;
	if (max_read != ((cmd & PCI_X_CMD_MAX_READ) >> 2)) {
		cmd &= ~PCI_X_CMD_MAX_READ;
		cmd |= max_read << 2;
	}
	if (max_tran != ((cmd & PCI_X_CMD_MAX_SPLIT) >> 4)) {
		cmd &= ~PCI_X_CMD_MAX_SPLIT;
		cmd |= max_tran << 4;
	}

	/* Don't attempt to handle PCI-X errors. */
	cmd &= ~PCI_X_CMD_DPERR_E;

	/* Enable relaxed ordering. */
	cmd |= PCI_X_CMD_ERO;

	if (orig_cmd != cmd)
		pci_write_config16(dev, cap + PCI_X_CMD, cmd);
}

static void pcix_tune_bus(struct bus *bus)
{
	struct device *child;

	for (child = bus->children; child; child = child->sibling)
		pcix_tune_dev(child);
}

const char *pcix_speed(u16 sstatus)
{
	static const char conventional[] = "Conventional PCI";
	static const char pcix_66mhz[] = "66MHz PCI-X";
	static const char pcix_100mhz[] = "100MHz PCI-X";
	static const char pcix_133mhz[] = "133MHz PCI-X";
	static const char pcix_266mhz[] = "266MHz PCI-X";
	static const char pcix_533mhz[] = "533MHZ PCI-X";
	static const char unknown[] = "Unknown";
	const char *result;

	result = unknown;

	switch (PCI_X_SSTATUS_MFREQ(sstatus)) {
	case PCI_X_SSTATUS_CONVENTIONAL_PCI:
		result = conventional;
		break;
	case PCI_X_SSTATUS_MODE1_66MHZ:
		result = pcix_66mhz;
		break;
	case PCI_X_SSTATUS_MODE1_100MHZ:
		result = pcix_100mhz;
		break;
	case PCI_X_SSTATUS_MODE1_133MHZ:
		result = pcix_133mhz;
		break;
	case PCI_X_SSTATUS_MODE2_266MHZ_REF_66MHZ:
	case PCI_X_SSTATUS_MODE2_266MHZ_REF_100MHZ:
	case PCI_X_SSTATUS_MODE2_266MHZ_REF_133MHZ:
		result = pcix_266mhz;
		break;
	case PCI_X_SSTATUS_MODE2_533MHZ_REF_66MHZ:
	case PCI_X_SSTATUS_MODE2_533MHZ_REF_100MHZ:
	case PCI_X_SSTATUS_MODE2_533MHZ_REF_133MHZ:
		result = pcix_533mhz;
		break;
	}

	return result;
}

void pcix_scan_bridge(struct device *dev)
{
	unsigned int pos;
	u16 sstatus;

	do_pci_scan_bridge(dev, pci_scan_bus);

	/* Find the PCI-X capability. */
	pos = pci_find_capability(dev, PCI_CAP_ID_PCIX);
	sstatus = pci_read_config16(dev, pos + PCI_X_SEC_STATUS);

	if (PCI_X_SSTATUS_MFREQ(sstatus) != PCI_X_SSTATUS_CONVENTIONAL_PCI)
		pcix_tune_bus(dev->link_list);

	/* Print the PCI-X bus speed. */
	printk(BIOS_DEBUG, "PCI: %02x: %s\n", dev->link_list->secondary,
	       pcix_speed(sstatus));
}

/** Default device operations for PCI-X bridges */
static struct pci_operations pcix_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_pcix_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = pcix_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pcix_bus_ops_pci,
};
