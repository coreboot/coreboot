/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <common/pin_mux.h>

void configure_pin_mux(FSP_S_CONFIG *supd)
{
	supd->SataPortDevSlpPinMux[0] = 0x59673e0c;
	supd->SataPortDevSlpPinMux[1] = 0x5967400d;
	supd->PchSerialIoI2cSdaPinMux[0] = 0x1947c404;
	supd->PchSerialIoI2cSdaPinMux[1] = 0x1947c606;
	supd->PchSerialIoI2cSdaPinMux[4] = 0;
	supd->PchSerialIoI2cSdaPinMux[7] = 0x1947d20c;
	supd->PchSerialIoI2cSclPinMux[0] = 0x1947a405;
	supd->PchSerialIoI2cSclPinMux[1] = 0x1947a607;
	supd->PchSerialIoI2cSclPinMux[4] = 0;
	supd->PchSerialIoI2cSclPinMux[7] = 0x1947b20d;
	supd->IshGpGpioPinMuxing[4] = 0x290ea809;
	supd->IshGpGpioPinMuxing[5] = 0x4900aa04;
	supd->IshGpGpioPinMuxing[6] = 0x4907ac0c;
	supd->IshGpGpioPinMuxing[7] = 0x5900ae0f;
	supd->CnviRfResetPinMux = 0x194ce404;
	supd->CnviClkreqPinMux = 0x294ce605;
}
