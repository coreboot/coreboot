/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 ***************************************************************************/


#include	"Platform.h"

UINT8	isEcPresent(){
	UINT8	dbFlag;
	UINT16	dwVar0;

	//Read the EC configuration register base address from LPCCfg_A4[15:1]
	//Write 0x5A to the EC config index register to unlock the access
	//Write 0x20 to the EC config index register to select the device ID register
	//Read the value of device ID register from the EC config data register
	//If the value read is 0xB7, then EC is enabled.
	//Write 0xA5 to re-lock the EC config index register if EC is enabled.

	ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4, AccWidthUint16 | S3_SAVE, &dwVar0);
	dwVar0 &= 0xFFFE;
	RWIO(dwVar0, AccWidthUint8, 0, 0x5A);
	RWIO(dwVar0, AccWidthUint8, 0, 0x20);
	ReadIO(dwVar0+1, AccWidthUint8, &dbFlag);
	RWIO(dwVar0, AccWidthUint8, 0, 0xA5);

	return ( dbFlag == 0xB7);
}

void
getSbInformation	(
SB_INFORMATION *sbInfo){
	UINT16 dwDevId;
	UINT8	dbRev;

	ReadPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG02, AccWidthUint16 | S3_SAVE, &dwDevId);
	ReadPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG08, AccWidthUint8 | S3_SAVE, &dbRev);
	sbInfo->sbModelMask = SB_MODEL_UNKNOWN;
	if ( (dwDevId == SB7XX_DEVICE_ID) && (dbRev <= SB_Rev_Sb7xx_A14) ){
		sbInfo->sbModelMask |= SB_MODEL_SB700;
		sbInfo->sbModelMask |= SB_MODEL_SR5690;
		sbInfo->sbRev = dbRev;
		ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG9C, AccWidthUint8 | S3_SAVE, &dbRev);
		if (dbRev & 01)
			sbInfo->sbModelMask |= SB_MODEL_SB750;
		if (isEcPresent())
			sbInfo->sbModelMask |= SB_MODEL_SB710;
		return;
		}
}


SB_CAPABILITY_SETTING
getSbCapability	(
SB_CAPABILITY_ITEM sbCapabilityItem
)
{
	SB_CAPABILITY_SETTING sbCapSetting=SB_UNKNOWN;
	UINT32 ddTemp0;
	RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT3, 00);
	ReadPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG10, AccWidthUint32, &ddTemp0);

	if (sbCapabilityItem < Sb_Unknown_Capability)
		sbCapSetting = ((ddTemp0 >> (sbCapabilityItem << 1) ) & 0x03);

	RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, 0xFF, BIT3);
	return sbCapSetting;
}


void
setSbCapability	(
SB_CAPABILITY_ITEM sbCapabilityItem, SB_CAPABILITY_SETTING sbCapSetting
)
{
	UINT32 ddTemp0;
	RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT3, 00);
	ReadPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG10, AccWidthUint32, &ddTemp0);
	if ( (sbCapabilityItem < Sb_Unknown_Capability) & (sbCapSetting < Sb_Cap_Setting_Unknown) )
		ddTemp0 = (ddTemp0 & ~(0x03 << (sbCapabilityItem << 1))) | ( (sbCapSetting & 0x03) << (sbCapabilityItem << 1));
	WritePCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG10, AccWidthUint32, &ddTemp0);
	RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, 0xFF, BIT3);
}
