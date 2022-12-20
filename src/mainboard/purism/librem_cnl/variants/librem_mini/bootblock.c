/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8528e/it8528e.h>
#include <intelblocks/lpc_lib.h>

void bootblock_mainboard_early_init(void)
{
	/*
	 * Enable the EC BRAM interface, and set bank 1's I/O BAR.
	 * BRAM on this board uses bank 0 from the PCH and bank 1 from the EC:
	 *
	 * - PCH bank 0 is used for the RTC.  Either should work, but the PCH
	 *   RTC was used historically, and it's preferable not to change this
	 *   in an update.
	 * - EC bank 1 contains the automatic-power-on setting, which is useful
	 *   to use the Mini as an appliance-like server.
	 *
	 * cmos.default can be used to configure the automatic power-on setting
	 * in firmware.  We have to set up the BRAM interface in bootblock,
	 * because cmos.default is applied early in bootblock.
	 */
	const pnp_devfn_t ec_rtct_dev = PNP_DEV(0x2E, IT8528E_RTCT);
	pnp_enter_conf_state(ec_rtct_dev);
	pnp_set_logical_device(ec_rtct_dev);
	pnp_set_enable(ec_rtct_dev, 1);
	pnp_set_iobase(ec_rtct_dev, 0x62, CONFIG_PC_CMOS_BASE_PORT_BANK1);
	pnp_exit_conf_state(ec_rtct_dev);

	/* Open the BRAM 1 bank interface on LPC */
	lpc_open_pmio_window(CONFIG_PC_CMOS_BASE_PORT_BANK1, 2);
}
