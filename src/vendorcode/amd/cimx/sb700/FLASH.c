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

void fcInitBeforePciEnum(AMDSBCFG* pConfig){

	TRACE((DMSG_SB_TRACE, "Entering PreInit Flash \n"));
	RWPMIO(SB_PMIO_REGB2, AccWidthUint8, ~(UINT32)BIT1, 00);

	//Enable IDE and disable flash
		//Enable IDE and disable flash
	RWPMIO(SB_PMIO_REG59, AccWidthUint8, ~(UINT32)(BIT1+BIT0), 0);
	RWPMIO(SB_PMIO_REGB2, AccWidthUint8, ~(UINT32)(BIT3), BIT0);	//Configure GPIO3 as IDE_RST# and release RST
	if (pConfig->IdeController){
		//Disabling IDE controller
		RWPCI((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG04, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT2+BIT1+BIT0), 0);
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGAE, AccWidthUint8 | S3_SAVE, 0xFF, BIT3);
	}
	else{
		//Enable IDE controller
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGAE, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT3), 0);
	}

	//RPR 8.2	Enable IDE Data bus DD7 Pull down Resistor if IDE is enabled and FC is disabled
	RWPMIO2(SB_PMIO2_REGE5, AccWidthUint8, 0xFF, BIT2);
	//Slowdown the clock to FC if FC is not enabled, this is a power savings feature
	RWPMIO(SB_PMIO_REGB2, AccWidthUint8, ~(UINT32)(BIT4), BIT4);
	RWPMIO(SB_PMIO_REGBC, AccWidthUint8, 0xC0, 0);
}
