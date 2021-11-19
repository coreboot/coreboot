/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/thermal.h>

#define MAX_TRIP_TEMP 205
/* This is the safest default Trip Temp value */
#define DEFAULT_TRIP_TEMP 50

/*
 * Trip Point = T2L | T1L | T0L where T2L > T1L > T0L
 * T2L = Bit 28:20
 * T1L = Bit 18:10
 * T0L = Bit 8:0
 */
#define GET_LTT_VALUE(x) ((x + 10) << 20 | (x + 5) << 10 | x)

static uint8_t get_thermal_trip_temp(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return common_config->pch_thermal_trip;
}

/* PCH Low Temp Threshold (LTT) */
static uint32_t pch_get_ltt_value(void)
{
	uint8_t thermal_config;

	thermal_config = get_thermal_trip_temp();
	if (!thermal_config)
		thermal_config = DEFAULT_TRIP_TEMP;

	if (thermal_config > MAX_TRIP_TEMP)
		die("Input PCH temp trip is higher than allowed range!");

	return GET_LTT_VALUE(thermal_config);
}

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
