/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <intelblocks/lpc_lib.h>
#include <soc/iomap.h>
#include <soc/pcr_ids.h>
#include <static.h>

#include <chip.h>

void soc_get_gen_io_dec_range(uint32_t gen_io_dec[LPC_NUM_GENERIC_IO_RANGES])
{
	const config_t *config = config_of_soc();

	gen_io_dec[0] = config->gen1_dec;
	gen_io_dec[1] = config->gen2_dec;
	gen_io_dec[2] = config->gen3_dec;
	gen_io_dec[3] = config->gen4_dec;
}

void pch_lpc_soc_fill_io_resources(struct device *dev)
{
}
