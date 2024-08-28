/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// TODO: Pin Mux settings

	// Enable TCP1 and TCP3 USB-A conversion
	// BIT 0:3 is mapping to PCH XHCI USB2 port
	// BIT 4:5 is reserved
	// BIT 6 is orientational
	// BIT 7 is enable
	// TODO: Add to coreboot FSP headers as no Client FSP release will be made.
	//params->EnableTcssCovTypeA[1] = 0x81;
	//params->EnableTcssCovTypeA[3] = 0x85;

	// Disable reporting CPU C10 state over eSPI (causes LED flicker).
	params->PchEspiHostC10ReportEnable = 0;
}
