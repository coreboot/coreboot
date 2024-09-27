/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/compiler.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>

#include "ramstage.h"

#define INTXSWZCTL 0xf8

enum pci_pin pciexp_get_rp_irq_pin(struct device *bridge, enum pci_pin pin)
{
	/* Swizzle according to INTXSWZCTL register. */
	enum pci_pin reg_swizzle_pin = pin;
	uint8_t int_swx_ctl = pci_read_config8(bridge, INTXSWZCTL) & 0x07;
	switch (int_swx_ctl) {
	case 0:
	case 1:
	case 2:
	case 3:
		reg_swizzle_pin = (reg_swizzle_pin - 1 + int_swx_ctl) % 4 + 1;
		break;
	case 4:
		reg_swizzle_pin = PCI_INT_A;
		break;
	default:
		reg_swizzle_pin = PCI_INT_NONE;
		break;
	}

	printk(BIOS_DEBUG, "Root Port %s %s swizzled to %s (INTXSWZCTL).\n", dev_path(bridge),
	       pin_to_str(pin), pin_to_str(reg_swizzle_pin));
	if (reg_swizzle_pin < PCI_INT_A || reg_swizzle_pin > PCI_INT_D)
		return reg_swizzle_pin;

	/* Swizzle between PCIe Root Ports. */
	enum pci_pin hardwired_swizzle_pin = reg_swizzle_pin;
	switch (bridge->device) {
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP3:
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP11:
		hardwired_swizzle_pin += 1;
		__fallthrough;
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP2:
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP10:
		hardwired_swizzle_pin += 1;
		__fallthrough;
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP1:
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP9:
		hardwired_swizzle_pin += 1;
		__fallthrough;
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP0:
	case PCI_DID_INTEL_SNR_PCH_PCIE_RP8:
		hardwired_swizzle_pin = (hardwired_swizzle_pin - 1) % 4 + 1;
		printk(BIOS_DEBUG, "Root Port %s %s swizzled to %s (Hard-wired).\n",
		       dev_path(bridge), pin_to_str(reg_swizzle_pin),
		       pin_to_str(hardwired_swizzle_pin));
		__fallthrough;
	default:
		return hardwired_swizzle_pin;
	}
}

static bool pcie_rp_is_vrp(struct device *dev)
{
	switch (dev_get_domain_id(dev)) {
	case 0:
		switch (dev->path.pci.devfn) {
		case PCH_DEVFN_QAT_1_7:
			return true;
		default:
			break;
		}
		break;
	case 3:
		if (dev->path.pci.devfn == NIS_DEVFN)
			return true;
		break;
	case 4:
		if (dev->path.pci.devfn == QAT_1_8_DEVFN)
			return true;
		break;
	default:
		break;
	}

	return false;
}

static void pcie_rp_scan_bridge(struct device *dev)
{
	if (pcie_rp_is_vrp(dev))
		return pciexp_scan_bridge(dev);
	else if (CONFIG(PCIEXP_HOTPLUG) && pciexp_dev_is_slot_hot_plug_cap(dev))
		return pciexp_hotplug_scan_bridge(dev);
	else
		return pciexp_scan_bridge(dev);
}

static const char *pcie_rp_acpi_name(const struct device *dev)
{
	switch (dev_get_domain_id(dev)) {
	case 0:
		switch (dev->path.pci.devfn) {
		case PCH_DEVFN_QAT_1_7:
			return "VRP6";
		case PCH_DEVFN_PCIE_RP0:
			return "RP00";
		case PCH_DEVFN_PCIE_RP1:
			return "RP01";
		case PCH_DEVFN_PCIE_RP2:
			return "RP02";
		case PCH_DEVFN_PCIE_RP3:
			return "RP03";
		case PCH_DEVFN_PCIE_RP8:
			return "RP08";
		case PCH_DEVFN_PCIE_RP9:
			return "RP09";
		case PCH_DEVFN_PCIE_RP10:
			return "RP10";
		case PCH_DEVFN_PCIE_RP11:
			return "RP11";
		default:
			break;
		}
		break;
	case 1:
		switch (dev->path.pci.devfn) {
		case CPU_DEVFN_PCIE_RPA:
			return "RPA";
		case CPU_DEVFN_PCIE_RPB:
			return "RPB";
		case CPU_DEVFN_PCIE_RPC:
			return "RPC";
		case CPU_DEVFN_PCIE_RPD:
			return "RPD";
		default:
			break;
		}
		break;
	case 3:
		if (dev->path.pci.devfn == NIS_DEVFN)
			return "VRP4";
		break;
	case 4:
		if (dev->path.pci.devfn == QAT_1_8_DEVFN)
			return "VRP5";
		break;
	default:
		break;
	}

	return NULL;
}

static struct device_operations snr_pcie_rp_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus = pcie_rp_scan_bridge,
	.acpi_fill_ssdt = pcie_rp_fill_ssdt,
	.acpi_name = pcie_rp_acpi_name,
};

static const unsigned short snr_pcie_rp_ids[] = {
	PCI_DID_INTEL_SNR_VRP0_QAT_1_7,
	PCI_DID_INTEL_SNR_VRP5_QAT_1_8,
	PCI_DID_INTEL_SNR_VRP4_NIS,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP0,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP1,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP2,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP3,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP8,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP9,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP10,
	PCI_DID_INTEL_SNR_PCH_PCIE_RP11,
	PCI_DID_INTEL_SNR_CPU_PCIE_RPA,
	PCI_DID_INTEL_SNR_CPU_PCIE_RPB,
	PCI_DID_INTEL_SNR_CPU_PCIE_RPC,
	PCI_DID_INTEL_SNR_CPU_PCIE_RPD,
	0
};

static const struct pci_driver snr_pcie_rp_driver __pci_driver = {
	.ops = &snr_pcie_rp_ops,
	.vendor = PCI_VID_INTEL,
	.devices = snr_pcie_rp_ids
};
