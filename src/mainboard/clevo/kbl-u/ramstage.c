/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <variant/gpio.h>

/*
 * TODO:
 * - Add kill switches for WLAN, BT, LTE, CCD
 * - Add support for WoL (LAN, WLAN)
 * - Make M.2 port configurable (SATA <> PCIe)
 *   - Make SATA DevSlp configurable
 * - Make TBT port configurable (TBT <> DisplayPort)
 */

static void init_mainboard(void *chip_info)
{
	variant_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
};
