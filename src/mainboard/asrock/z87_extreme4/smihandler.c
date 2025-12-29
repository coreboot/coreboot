/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <device/pnp_ops.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

#define GPIO1_DEV PNP_DEV(0x2e, NCT6776_WDT1_GPIO01A_V)

void mainboard_smi_sleep(u8 slp_typ)
{
	/*
	* Cut off power to LEDs when system goes to sleep.
	*/
	if (slp_typ >= ACPI_S3) {

		nuvoton_pnp_enter_conf_state(GPIO1_DEV);

		pnp_set_logical_device(GPIO1_DEV);

		pnp_write_config(GPIO1_DEV, 0xf0, 0xff);
		pnp_write_config(GPIO1_DEV, 0xf1, 0xff);

		nuvoton_pnp_exit_conf_state(GPIO1_DEV);
	}
}
