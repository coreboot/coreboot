/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>

/*
 * TODO:
 * - Add kill switches for WLAN, BT, LTE, CCD
 * - Add support for WoL (LAN, WLAN)
 * - Make M.2 port configurable (SATA <> PCIe)
 *   - Make SATA DevSlp configurable
 * - Make TBT port configurable (TBT <> DisplayPort)
 */

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	mainboard_configure_gpios();
}
