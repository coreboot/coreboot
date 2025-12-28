/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__
#include <arch/io.h>
#include <bootstate.h>
#include <device/device.h>
#include <device/pnp_ops.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>
#include <southbridge/intel/lynxpoint/pch.h>

#define GPIO1_DEV PNP_DEV(0x2e, NCT6776_WDT1_GPIO01A_V)

static void turn_off_leds(void *unused)
{
	nuvoton_pnp_enter_conf_state(GPIO1_DEV);

	pnp_set_logical_device(GPIO1_DEV);

	pnp_write_config(GPIO1_DEV, 0xf0, 0xff);
	pnp_write_config(GPIO1_DEV, 0xf1, 0xff);
	pnp_write_config(GPIO1_DEV, 0x27, 0xd0);

	nuvoton_pnp_exit_conf_state(GPIO1_DEV);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, turn_off_leds, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_BOOT, BS_ON_ENTRY, turn_off_leds, NULL);
