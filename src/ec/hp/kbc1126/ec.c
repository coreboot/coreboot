/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <ec/acpi/ec.h>

#include "chip.h"

#define KBC_TIMEOUT_US 1000000 // 1s

/*
 * kbc1126_thermal_init: initialize fan control
 * The code is found in EcThermalInit of the vendor firmware.
 */
static int kbc1126_thermal_init(u8 cmd, u8 value)
{
	printk(BIOS_DEBUG, "KBC1126: initialize fan control.\n");

	if (send_ec_command_timeout(cmd, KBC_TIMEOUT_US) < 0)
		return -1;

	if (send_ec_data_timeout(0x27, KBC_TIMEOUT_US) < 0)
		return -1;

	if (send_ec_data_timeout(0x01, KBC_TIMEOUT_US) < 0)
		return -1;

	/*
	 * The following code is needed for fan control when AC is plugged in.
	 */

	if (send_ec_command_timeout(cmd, KBC_TIMEOUT_US) < 0)
		return -1;

	if (send_ec_data_timeout(0xd5, KBC_TIMEOUT_US) < 0)
		return -1;

	if (send_ec_data_timeout(value, KBC_TIMEOUT_US) < 0)
		return -1;

	printk(BIOS_DEBUG, "KBC1126: fan control initialized.\n");
	return 0;
}

/*
 * kbc1126_kbd_led: set CapsLock and NumLock LEDs
 * This is used in MemoryErrorReport of the vendor firmware.
 */
static void kbc1126_kbd_led(u8 cmd, u8 val)
{
	if (send_ec_command_timeout(cmd, KBC_TIMEOUT_US) < 0)
		return;

	if (send_ec_data_timeout(0xf0, KBC_TIMEOUT_US) < 0)
		return;

	if (send_ec_data_timeout(val, KBC_TIMEOUT_US) < 0)
		return;
}

static void kbc1126_enable(struct device *dev)
{
	struct ec_hp_kbc1126_config *conf = dev->chip_info;

	ec_set_ports(conf->ec_cmd_port, conf->ec_data_port);
	kbc1126_kbd_led(conf->ec_ctrl_reg, 0);
	if (kbc1126_thermal_init(conf->ec_ctrl_reg, conf->ec_fan_ctrl_value) < 0)
		printk(BIOS_DEBUG, "KBC1126: error when initializing fan control.\n");
}

struct chip_operations ec_hp_kbc1126_ops = {
	.name = "SMSC KBC1126 for HP laptops",
	.enable_dev = kbc1126_enable
};
