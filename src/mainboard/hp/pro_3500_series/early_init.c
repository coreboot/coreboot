/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <superio/ite/common/ite.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/hwm5_conf.h>
#include "common_defines.h"
#include "led.h"

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1,  6, 0 },
	{ 1,  6, 0 },
	{ 1,  6, 1 },
	{ 1,  9, 1 },
	{ 1, 10, 2 },
	{ 1, 11, 2 },
	{ 1,  6, 3 },
	{ 1,  6, 3 },
	{ 1,  6, 4 },
	{ 1, 12, 4 },
	{ 1,  6, 6 },
	{ 1, 11, 5 },
	{ 1,  6, 5 },
	{ 1,  6, 6 },
};

void bootblock_mainboard_early_init(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | KBC_LPC_EN);

	/* Early SuperIO setup */
	ite_conf_clkin(GPIO_DEV, ITE_UART_CLK_PREDIVIDE_24);
	/* Clear when resuming from S3: */
	ite_disable_3vsbsw(GPIO_DEV);
	ite_disable_pme_out(EC_DEV);
	ite_ac_resume_southbridge(EC_DEV);

	set_power_led(LED_WHITE);
}
