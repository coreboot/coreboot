/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	supd->TcNotifyIgd = 2; // Auto
	/* eSPI GPIOs */
	supd->SerialIoSpiCsPinMux[0] = 0x14a48a;
	supd->SerialIoSpiClkPinMux[0] = 0x14a48b;
	supd->SerialIoSpiMisoPinMux[0] = 0x14a48c;
	supd->SerialIoSpiMosiPinMux[0] = 0x14a48d;
}
