/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <console/console.h>
#include <types.h>
#include "psp_def.h"

void psp_set_tpm_irq_gpio(unsigned int gpio)
{
	int cmd_status;
	struct mbox_cmd_dtpm_config_buffer buffer = {
		.header = {
			.size = sizeof(buffer)
		},
		.request_type = DTPM_REQUEST_CONFIG,
		.config = {
			.gpio = gpio
		}
	};

	printk(BIOS_DEBUG, "PSP: Setting TPM GPIO to %u...", gpio);

	cmd_status = send_psp_command(MBOX_BIOS_CMD_I2C_TPM_ARBITRATION, &buffer);

	psp_print_cmd_status(cmd_status, &buffer.header);
}
