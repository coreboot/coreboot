/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include "pin_mux.h"

void configure_pin_mux(FSP_S_CONFIG *supd)
{
	supd->SerialIoUartRxPinMuxPolicy[0] = 0x190B0208;
	supd->SerialIoUartTxPinMuxPolicy[0] = 0x190B1209;
	supd->SerialIoUartRtsPinMuxPolicy[0] = 0x190B220a;
	supd->SerialIoUartCtsPinMuxPolicy[0] = 0x190B320b;
	supd->SerialIoUartRxPinMuxPolicy[0] = 0x18050208;
	supd->SerialIoUartTxPinMuxPolicy[0] = 0x18051209;
	supd->SerialIoUartRtsPinMuxPolicy[0] = 0x1805220c;
	supd->SerialIoUartCtsPinMuxPolicy[0] = 0x1805320b;
}
