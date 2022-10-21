/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Glinda */

#include <acpi/acpi_device.h>
#include <amdblocks/data_fabric.h>
#include <arch/hpet.h>
#include <console/console.h>
#include <cpu/x86/lapic_def.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/data_fabric.h>
#include <soc/iomap.h>
#include <types.h>

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
	const uint32_t np_top = (LAPIC_DEFAULT_BASE - 1) >> D18F0_MMIO_SHIFT;

	data_fabric_print_mmio_conf();

	for (i = 0; i < NUM_NB_MMIO_REGS; i++) {
		/* Adjust all registers that overlap */
		ctrl = data_fabric_broadcast_read32(0, NB_MMIO_CONTROL(i));
		if (!(ctrl & (DF_MMIO_WE | DF_MMIO_RE)))
			continue; /* not enabled */

		base = data_fabric_broadcast_read32(0, NB_MMIO_BASE(i));
		limit = data_fabric_broadcast_read32(0, NB_MMIO_LIMIT(i));

		if (base > np_top || limit < np_bot)
			continue; /* no overlap at all */

		if (base >= np_bot && limit <= np_top) {
			data_fabric_disable_mmio_reg(i); /* 100% within, so remove */
			continue;
		}

		if (base < np_bot && limit > np_top) {
			/* Split the configured region */
			data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(i), np_bot - 1);
			reg = data_fabric_find_unused_mmio_reg();
			if (reg < 0) {
				/* Although a pair could be freed later, this condition is
				 * very unusual and deserves analysis.  Flag an error and
				 * leave the topmost part unconfigured. */
				printk(BIOS_ERR,
				       "Error: Not enough NB MMIO routing registers\n");
				continue;
			}
			data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), np_top + 1);
			data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), limit);
			data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg), ctrl);
			continue;
		}

		/* If still here, adjust only the base or limit */
		if (base <= np_bot)
			data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(i), np_bot - 1);
		else
			data_fabric_broadcast_write32(0, NB_MMIO_BASE(i), np_top + 1);
	}

	reg = data_fabric_find_unused_mmio_reg();
	if (reg < 0) {
		printk(BIOS_ERR, "Error: cannot configure region as NP\n");
		return;
	}

	data_fabric_broadcast_write32(0, NB_MMIO_BASE(reg), np_bot);
	data_fabric_broadcast_write32(0, NB_MMIO_LIMIT(reg), np_top);
	data_fabric_broadcast_write32(0, NB_MMIO_CONTROL(reg),
			   (IOMS0_FABRIC_ID << DF_MMIO_DST_FABRIC_ID_SHIFT) | DF_MMIO_NP
				   | DF_MMIO_WE | DF_MMIO_RE);

	data_fabric_print_mmio_conf();
}

static const char *data_fabric_acpi_name(const struct device *dev)
{
	switch (dev->device) {
	case PCI_DID_AMD_FAM17H_MODELA0H_DF0:
		return "DFD0";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF1:
		return "DFD1";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF2:
		return "DFD2";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF3:
		return "DFD3";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF4:
		return "DFD4";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF5:
		return "DFD5";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF6:
		return "DFD6";
	case PCI_DID_AMD_FAM17H_MODELA0H_DF7:
		return "DFD7";
	default:
		printk(BIOS_ERR, "%s: Unhandled device id 0x%x\n", __func__, dev->device);
	}

	return NULL;
}

struct device_operations glinda_data_fabric_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_name		= data_fabric_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};
