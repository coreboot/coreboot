/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <device/pnp_ops.h>
#include <option.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

#define GLOBAL_DEV   PNP_DEV(0x2e, 0)
#define SERIAL_DEV   PNP_DEV(0x2e, NCT6776_SP1)
#define GPIO6789_DEV PNP_DEV(0x2e, NCT6776_GPIO6789_V)

/* As defined in cmos.layout */
enum cpu_fan_tach_src {
	CPU_FAN_HEADER_NONE,
	CPU_FAN_HEADER_1,
	CPU_FAN_HEADER_2,
	CPU_FAN_HEADER_BOTH
};

/*
 * The tachometer signal that goes to CPUFANIN of the Super I/O is set via
 * GPIOs.
 *
 * When GP77 (register E1h[7]) is '0', CPU_FAN1 is connected.
 * When GP76 (register E1h[6]) is '0', CPU_FAN2 is connected.
 * When both are '0' and both fans are connected, wrong readings will
 * be reported.
 */
static u8 get_cpufanin_gpio_config(void)
{
	switch (get_uint_option("cpu_fan_tach_src", CPU_FAN_HEADER_1)) {
	case CPU_FAN_HEADER_NONE:
		return 0xff;
	case CPU_FAN_HEADER_1:
	default:
		return 0x7f;
	case CPU_FAN_HEADER_2:
		return 0xbf;
	case CPU_FAN_HEADER_BOTH:
		return 0x3f;
	}
};

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Configure Super I/O pins */
	pnp_write_config(GLOBAL_DEV, 0x1b, 0x68);
	pnp_write_config(GLOBAL_DEV, 0x1c, 0x80);
	pnp_write_config(GLOBAL_DEV, 0x24, 0x5c);
	pnp_write_config(GLOBAL_DEV, 0x27, 0xc0);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x62);
	pnp_write_config(GLOBAL_DEV, 0x2b, 0x08);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x80);

	/* GP77 and GP76 are outputs. They set the tachometer input on CPUFANIN. */
	pnp_set_logical_device(GPIO6789_DEV);
	pnp_write_config(GPIO6789_DEV, 0xe0, 0x3f);
	pnp_write_config(GPIO6789_DEV, 0xe1, get_cpufanin_gpio_config());

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
