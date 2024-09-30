/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <soc/pnpconfig.h>
#include <static.h>
#include "chip.h"

static const struct pnpconfig perf[] = {
	VALUEFORPERF_MSG_VALUES_PLATFORM_DEFAULT,
};

static const struct pnpconfig power[] = {
	VALUEFORPOWER_MSG_VALUES_PLATFORM_DEFAULT,
};

static const struct pnpconfig power_perf[] = {
	VALUEFORPWRPERF_MSG_VALUES_PLATFORM_DEFAULT,
};

static void pnp_settings(void *unused)
{
	int index;
	size_t arrsize;
	const struct pnpconfig *pnpconfigarr;
	struct soc_intel_apollolake_config *config;

	config = config_of_soc();

	switch (config->pnp_settings) {
	case PNP_PERF:
		pnpconfigarr = perf;
		arrsize = ARRAY_SIZE(perf);
		break;
	case PNP_POWER:
		pnpconfigarr = power;
		arrsize = ARRAY_SIZE(power);
		break;
	case PNP_PERF_POWER:
		pnpconfigarr = power_perf;
		arrsize = ARRAY_SIZE(power_perf);
		break;
	default:
		printk(BIOS_NOTICE, "Invalid PNP settings selected");
		return;
	}

	for (index = 0; index < arrsize; index++)
		pcr_rmw32(pnpconfigarr[index].msgport,
			  pnpconfigarr[index].msgregaddr,
			  pnpconfigarr[index].mask,
			  pnpconfigarr[index].value);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, pnp_settings, NULL);
