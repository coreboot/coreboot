/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/systemagent.h>
#include <option.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <types.h>
#include "chip.h"

bool soc_vtd_enabled(void)
{
	const unsigned int vtd = get_uint_option("vtd", 1);
	if (!vtd)
		return false;
	struct device *const root_dev = pcidev_path_on_root(SA_DEVFN_ROOT);
	return root_dev &&
		!(pci_read_config32(root_dev, CAPID0_A) & VTD_DISABLE);
}

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
		{ DMIBAR, DMI_BASE_ADDRESS, DMI_BASE_SIZE, "DMIBAR" },
		{ EPBAR, EP_BASE_ADDRESS, EP_BASE_SIZE, "EPBAR" },
		{ GDXCBAR, GDXC_BASE_ADDRESS, GDXC_BASE_SIZE, "GDXCBAR" },
		{ EDRAMBAR, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE, "EDRAMBAR" },
	};

	sa_add_fixed_mmio_resources(dev, index, soc_fixed_resources,
			ARRAY_SIZE(soc_fixed_resources));

	if (soc_vtd_enabled()) {
		if (is_devfn_enabled(SA_DEVFN_IGD))
			sa_add_fixed_mmio_resources(dev, index,
					&soc_gfxvt_mmio_descriptor, 1);

		sa_add_fixed_mmio_resources(dev, index,
				&soc_vtvc0_mmio_descriptor, 1);
	}
}

/*
 * SoC implementation
 *
 * Perform System Agent Initialization during Ramstage phase.
 */
void soc_systemagent_init(struct device *dev)
{
	struct soc_power_limits_config *soc_config;
	config_t *config;

	/* Enable Power Aware Interrupt Routing */
	enable_power_aware_intr();

	/* Enable BIOS Reset CPL */
	enable_bios_reset_cpl();

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	config = config_of_soc();
	soc_config = &config->power_limits_config;
	set_power_limits(MOBILE_SKU_PL1_TIME_SEC, soc_config);
}

int soc_get_uncore_prmmr_base_and_mask(uint64_t *prmrr_base,
	uint64_t *prmrr_mask)
{
	msr_t msr;
	msr = rdmsr(MSR_UNCORE_PRMRR_PHYS_BASE);
	*prmrr_base = (uint64_t)msr.hi << 32 | msr.lo;
	msr = rdmsr(MSR_UNCORE_PRMRR_PHYS_MASK);
	*prmrr_mask = (uint64_t)msr.hi << 32 | msr.lo;
	return 0;
}

uint32_t soc_systemagent_max_chan_capacity_mib(u8 capid0_a_ddrsz)
{
	switch (capid0_a_ddrsz) {
	case 1:
		return 8192;
	case 2:
		return 4096;
	case 3:
		return 2048;
	default:
		return 32768;
	}
}
