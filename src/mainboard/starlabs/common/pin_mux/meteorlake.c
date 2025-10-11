/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include "pin_mux.h"

void configure_pin_mux(FSP_S_CONFIG *supd)
{
	supd->IshUartRxPinMuxing[1] = 0x146806;
	supd->IshUartTxPinMuxing[1] = 0x146807;
	supd->IshI2cSdaPinMuxing[2] = 0x143012;
	supd->IshI2cSclPinMuxing[2] = 0x143013;
	supd->IshSpiMosiPinMuxing[0] = 0x147088;
	supd->IshSpiMisoPinMuxing[0] = 0x147087;
	supd->IshSpiClkPinMuxing[0] = 0x147086;
	supd->IshSpiCsPinMuxing[0] = 0x147085;
	supd->SerialIoUartRxPinMuxPolicy[1] = 0x144806;
	supd->SerialIoUartTxPinMuxPolicy[1] = 0x144807;
	supd->PchSerialIoI2cSdaPinMux[2] = 0x142804;
	supd->PchSerialIoI2cSdaPinMux[3] = 0x142806;
	supd->PchSerialIoI2cSdaPinMux[4] = 0x15048c;
	supd->PchSerialIoI2cSdaPinMux[5] = 0x15088d;
	supd->PchSerialIoI2cSclPinMux[2] = 0x142805;
	supd->PchSerialIoI2cSclPinMux[3] = 0x142807;
	supd->PchSerialIoI2cSclPinMux[4] = 0x15048d;
	supd->PchSerialIoI2cSclPinMux[5] = 0x15088c;
	supd->SerialIoI3cSdaPinMux[1] = 0x144815;
	supd->SerialIoI3cSclPinMux[1] = 0x144816;
	supd->SerialIoI3cSclFbPinMux[1] = 0x144818;
	supd->IshGpGpioPinMuxing[5] = 0x149016;
	supd->IshGpGpioPinMuxing[6] = 0x149017;
	supd->IshGpGpioPinMuxing[8] = 0x149014;
	supd->IshGpGpioPinMuxing[9] = 0x150897;
	supd->IshGpGpioPinMuxing[10] = 0x150490;
	supd->IshGpGpioPinMuxing[11] = 0x150889;
	supd->SerialIoSpiCsPinMux[0] = 0x14a48a;
	supd->SerialIoSpiClkPinMux[0] = 0x14a48b;
	supd->SerialIoSpiMisoPinMux[0] = 0x14a48c;
	supd->SerialIoSpiMosiPinMux[0] = 0x14a48d;
}
