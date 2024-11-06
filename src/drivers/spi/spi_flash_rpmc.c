/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <string.h>
#include <timer.h>
#include <types.h>

#include "spi_flash_internal.h"

void spi_flash_fill_rpmc_caps(struct spi_flash *flash)
{
	struct sfdp_rpmc_info rpmc_info;

	flash->rpmc_caps.rpmc_available = false;

	if (spi_flash_get_sfdp_rpmc(flash, &rpmc_info) != CB_SUCCESS)
		return;

	if (rpmc_info.monotonic_counter_size != SFDP_RPMC_COUNTER_BITS_32) {
		printk(BIOS_WARNING, "RPMC: unexpected counter size\n");
		return;
	}

	flash->rpmc_caps.poll_op2_ext_stat = rpmc_info.busy_polling_method ==
					     SFDP_RPMC_POLL_OP2_EXTENDED_STATUS;
	flash->rpmc_caps.number_of_counters = rpmc_info.number_of_counters;
	flash->rpmc_caps.op1_write_cmd = rpmc_info.op1_write_command;
	flash->rpmc_caps.op2_read_cmd = rpmc_info.op2_read_command;
	flash->rpmc_caps.rpmc_available = true;
}
