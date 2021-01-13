/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/rtc.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include "chip.h"

void lpc_soc_init(struct device *dev)
{
	const struct soc_intel_apollolake_config *cfg;
	cfg = config_of(dev);

	/* Set LPC Serial IRQ mode */
	lpc_set_serirq_mode(cfg->serirq_mode);

	/* Initialize RTC */
	rtc_init();
}
