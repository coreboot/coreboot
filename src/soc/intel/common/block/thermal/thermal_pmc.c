/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/thermal.h>

/*
 * Thermal configuration has evolved over time. With older platform the
 * thermal device is sitting over PCI and allow to configure its configuration
 * register by accessing the PCI configuration space or MMIO space.
 *
 * Since Tiger Lake, thermal registers are being moved behind the PMC PCI device
 * hence, accessing thermal configuration registers would need making access
 * to PWRMBASE. In this case SoC Kconfig to select
 * SOC_INTEL_COMMON_BLOCK_THERMAL_BEHIND_PMC to allow thermal configuration.
 */
void pch_thermal_configuration(void)
{
	uintptr_t pmc_bar = soc_read_pmc_base();

	struct pmc_thermal_config {
		uint16_t offset;
		uint32_t mask;
		uint32_t value;
	} config[] = {
		{
			.offset = PMC_PWRM_THERMAL_CTEN,
			.value = PMC_PWRM_THERMAL_CTEN_CPDEN | PMC_PWRM_THERMAL_CTEN_CTENLOCK,
		},
		{
			.offset = PMC_PWRM_THERMAL_ECRPTEN,
			.value = PMC_PWRM_THERMAL_ECRPTEN_EN_RPT
					 | PMC_PWRM_THERMAL_ECRPTEN_ECRPTENLOCK,
		},
		{
			.offset = PMC_PWRM_THERMAL_TL,
			.mask = ~0,
			.value = pch_get_ltt_value() | PMC_PWRM_THERMAL_TL_TTEN
					 | PMC_PWRM_THERMAL_TL_TLLOCK,
		},
		{
			.offset = PMC_PWRM_THERMAL_PHLC,
			.value = PMC_PWRM_THERMAL_PHLC_PHLCLOCK,
		},
		{
			.offset = PMC_PWRM_THERMAL_TLEN,
			.value = PMC_PWRM_THERMAL_TLEN_TLENLOCK,
		},
	};

	for (int i = 0; i < ARRAY_SIZE(config); i++)
		clrsetbits32((void *)(pmc_bar + config[i].offset), config[i].mask,
				 config[i].value);
}
