/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/cpu.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <fsp/util.h>
#include <intelblocks/systemagent.h>
#include <soc/iomap.h>
#include <soc/systemagent.h>

/*
 * SoC implementation
 *
 * Add all known fixed memory ranges for Host Controller/Memory
 * controller.
 */
void soc_add_fixed_mmio_resources(struct device *dev, int *index)
{
	static const struct sa_mmio_descriptor soc_fixed_resources[] = {
		{ PCIEXBAR, CONFIG_ECAM_MMCONF_BASE_ADDRESS, CONFIG_ECAM_MMCONF_LENGTH,
				"PCIEXBAR" },
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
	};

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));

	/* Add VTd resources if VTd is enabled. These resources were
	   set up by the FSP-S call. */
	if ((pci_read_config32(dev, CAPID0_A) & VTD_DISABLE))
		return;

	if (MCHBAR32(GFXVTBAR) & VTBAR_ENABLED) {
		mmio_resource_kb(dev, *index,
			      (MCHBAR64(GFXVTBAR) & VTBAR_MASK) / KiB,
			      VTBAR_SIZE / KiB);
		(*index)++;
	}
	if (MCHBAR32(DEFVTBAR) & VTBAR_ENABLED) {
		mmio_resource_kb(dev, *index,
			      (MCHBAR64(DEFVTBAR) & VTBAR_MASK) / KiB,
			      VTBAR_SIZE / KiB);
		(*index)++;
	}
}

int soc_get_uncore_prmmr_base_and_mask(uint64_t *prmrr_base,
			uint64_t *prmrr_mask)
{
	const void *hob;
	size_t hob_size, prmrr_size;
	uint64_t phys_address_mask;
	const uint8_t prmrr_phys_base_guid[16] = {
		0x38, 0x3a, 0x81, 0x9f, 0xb0, 0x6f, 0xa7, 0x4f,
		0xaf, 0x79, 0x8a, 0x4e, 0x74, 0xdd, 0x48, 0x33
	};
	const uint8_t prmrr_size_guid[16] = {
		0x44, 0xed, 0x0b, 0x99, 0x4e, 0x9b, 0x26, 0x42,
		0xa5, 0x97, 0x28, 0x36, 0x76, 0x6b, 0x5c, 0x41
	};

	hob = fsp_find_extension_hob_by_guid(prmrr_phys_base_guid,
			&hob_size);
	if (!hob) {
		printk(BIOS_ERR, "Failed to locate PRMRR base hob\n");
		return -1;
	}
	if (hob_size != sizeof(uint64_t)) {
		printk(BIOS_ERR, "Incorrect PRMRR base hob size\n");
		return -1;
	}
	*prmrr_base = *(uint64_t *) hob;

	hob = fsp_find_extension_hob_by_guid(prmrr_size_guid,
			&hob_size);
	if (!hob) {
		printk(BIOS_ERR, "Failed to locate PRMRR size hob\n");
		return -1;
	}
	if (hob_size != sizeof(uint64_t)) {
		printk(BIOS_ERR, "Incorrect PRMRR base hob size\n");
		return -1;
	}
	prmrr_size = *(uint64_t *) hob;
	phys_address_mask = (1ULL << cpu_phys_address_size()) - 1;
	*prmrr_mask = phys_address_mask & ~(uint64_t)(prmrr_size - 1);

	return 0;
}

uint32_t soc_systemagent_max_chan_capacity_mib(u8 capid0_a_ddrsz)
{
	/* Max 4GiB per rank, 2 ranks per channel. Intel Document: 332092-002 */
	return 8192;
}
