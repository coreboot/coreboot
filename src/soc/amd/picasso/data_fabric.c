/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <console/console.h>
#include <cpu/x86/lapic_def.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <types.h>

static void disable_mmio_reg(unsigned int reg)
{
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg),
			   IOMS0_FABRIC_ID << MMIO_DST_FABRIC_ID_SHIFT);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), 0);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), 0);
}

static bool is_mmio_reg_disabled(unsigned int reg)
{
	uint32_t val = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg));
	return !(val & ((MMIO_WE | MMIO_RE)));
}

static int find_unused_mmio_reg(void)
{
	unsigned int i;

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		if (is_mmio_reg_disabled(i))
			return i;
	}
	return -1;
}

void data_fabric_set_mmio_np(void)
{
	/*
	 * Mark region from HPET-LAPIC or 0xfed00000-0xfee00000-1 as NP.
	 *
	 * AGESA has already programmed the NB MMIO routing, however nothing
	 * is yet marked as non-posted.
	 *
	 * If there exists an overlapping routing base/limit pair, trim its
	 * base or limit to avoid the new NP region.  If any pair exists
	 * completely within HPET-LAPIC range, remove it.  If any pair surrounds
	 * HPET-LAPIC, it must be split into two regions.
	 *
	 * TODO(b/156296146): Remove the settings from AGESA and allow coreboot
	 * to own everything.  If not practical, consider erasing all settings
	 * and have coreboot reprogram them.  At that time, make the source
	 * below more flexible.
	 *   * Note that the code relies on the granularity of the HPET and
	 *     LAPIC addresses being sufficiently large that the shifted limits
	 *     +/-1 are always equivalent to the non-shifted values +/-1.
	 */

	unsigned int i;
	int reg;
	uint32_t base, limit, ctrl;
	const uint32_t np_bot = HPET_BASE_ADDRESS >> D18F0_MMIO_SHIFT;
	const uint32_t np_top = (LOCAL_APIC_ADDR - 1) >> D18F0_MMIO_SHIFT;

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		/* Adjust all registers that overlap */
		ctrl = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(i));
		if (!(ctrl & (MMIO_WE | MMIO_RE)))
			continue; /* not enabled */

		base = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(i));
		limit = pci_read_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(i));

		if (base > np_top || limit < np_bot)
			continue; /* no overlap at all */

		if (base >= np_bot && limit <= np_top) {
			disable_mmio_reg(i); /* 100% within, so remove */
			continue;
		}

		if (base < np_bot && limit > np_top) {
			/* Split the configured region */
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(i), np_bot - 1);
			reg = find_unused_mmio_reg();
			if (reg < 0) {
				/* Although a pair could be freed later, this condition is
				 * very unusual and deserves analysis.  Flag an error and
				 * leave the topmost part unconfigured. */
				printk(BIOS_ERR,
				       "Error: Not enough NB MMIO routing registers\n");
				continue;
			}
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), np_top + 1);
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), limit);
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg), ctrl);
			continue;
		}

		/* If still here, adjust only the base or limit */
		if (base <= np_bot)
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(i), np_bot - 1);
		else
			pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(i), np_top + 1);
	}

	reg = find_unused_mmio_reg();
	if (reg < 0) {
		printk(BIOS_ERR, "Error: cannot configure region as NP\n");
		return;
	}

	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_BASE(reg), np_bot);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_LIMIT(reg), np_top);
	pci_write_config32(SOC_DF_F0_DEV, NB_MMIO_CONTROL(reg),
			   (IOMS0_FABRIC_ID << MMIO_DST_FABRIC_ID_SHIFT) | MMIO_NP | MMIO_WE
				   | MMIO_RE);
}

static const char *data_fabric_acpi_name(const struct device *dev)
{
	switch (dev->device) {
	case PCI_DEVICE_ID_AMD_FAM17H_DF0:
		return "DFD0";
	case PCI_DEVICE_ID_AMD_FAM17H_DF1:
		return "DFD1";
	case PCI_DEVICE_ID_AMD_FAM17H_DF2:
		return "DFD2";
	case PCI_DEVICE_ID_AMD_FAM17H_DF3:
		return "DFD3";
	case PCI_DEVICE_ID_AMD_FAM17H_DF4:
		return "DFD4";
	case PCI_DEVICE_ID_AMD_FAM17H_DF5:
		return "DFD5";
	case PCI_DEVICE_ID_AMD_FAM17H_DF6:
		return "DFD6";
	default:
		printk(BIOS_ERR, "%s: Unhandled device id 0x%x\n", __func__, dev->device);
	}

	return NULL;
}

static struct device_operations data_fabric_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= data_fabric_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_FAM17H_DF0,
	PCI_DEVICE_ID_AMD_FAM17H_DF1,
	PCI_DEVICE_ID_AMD_FAM17H_DF2,
	PCI_DEVICE_ID_AMD_FAM17H_DF3,
	PCI_DEVICE_ID_AMD_FAM17H_DF4,
	PCI_DEVICE_ID_AMD_FAM17H_DF5,
	PCI_DEVICE_ID_AMD_FAM17H_DF6,
	0
};

static const struct pci_driver data_fabric_driver __pci_driver = {
	.ops			= &data_fabric_ops,
	.vendor			= PCI_VENDOR_ID_AMD,
	.devices		= pci_device_ids,
};
