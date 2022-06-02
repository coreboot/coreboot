/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/rtc.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/intel/common/block/lpc/lpc_def.h>
#include "chip.h"

void soc_get_gen_io_dec_range(uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES])
{
	const config_t *config = config_of_soc();

	gen_io_dec[0] = config->gen1_dec;
	gen_io_dec[1] = config->gen2_dec;
	gen_io_dec[2] = config->gen3_dec;
	gen_io_dec[3] = config->gen4_dec;
}

void lpc_soc_init(struct device *dev)
{
	const struct soc_intel_apollolake_config *cfg;
	cfg = config_of(dev);

	/* Set LPC Serial IRQ mode */
	lpc_set_serirq_mode(cfg->serirq_mode);

	/* Initialize RTC */
	rtc_init();
}
