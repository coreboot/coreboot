/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/ramstage.h>
#include <common/pin_mux.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	configure_pin_mux(supd);

	/*
	 * Enable Hot Plug on RP5 to slow down coreboot so that
	 * third-party drives are detected.
	 */
	if (get_uint_option("pci_hot_plug", 0) == 1)
		supd->PcieRpHotPlug[8] = 1;
}
