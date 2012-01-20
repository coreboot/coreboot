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

#ifndef	NO_EC_SUPPORT

REG8MASK sb710PorInitPciTable[] = {
	// SMBUS Device(Bus 0, Dev 20, Func 0)
	{0x00, SMBUS_BUS_DEV_FUN, 0},
	{SB_SMBUS_REG43, ~(UINT8)BIT3, 0x00},			//Make some hidden registers of smbus visible.
	{SB_SMBUS_REG38, (UINT8)~(BIT7+BIT5+BIT4+BIT3+BIT2+BIT1), 0x0D},
	{SB_SMBUS_REG38+1, ~(UINT8)(BIT2+BIT1), BIT3 },
	{SB_SMBUS_REGE1, 0xFF, BIT1},
	{SB_SMBUS_REG43, 0xFF, BIT3},			//Make some hidden registers of smbus invisible.
	{0xFF, 0xFF, 0xFF},

	// LPC Device(Bus 0, Dev 20, Func 3)
	{0x00, LPC_BUS_DEV_FUN, 0},
	{SB_LPC_REGB8+3, ~(UINT8)(BIT1), BIT7+BIT2},
	{0xFF, 0xFF, 0xFF},
};

REG8MASK sb710PorPmioInitTbl[]={
	// index		andmask		ormask
	{SB_PMIO_REGD7, 0xFF, BIT5},
	{SB_PMIO_REGBB, 0xFF, BIT5},
};


void	ecPowerOnInit(BUILDPARAM *pBuildOptPtr, AMDSBCFG* pConfig){
	UINT8	dbVar0, i=0;

	if (!(isEcPresent()))
		return;				//return if EC is not enabled

	for(i=0;i<0xFF;i++){
		ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG40, AccWidthUint8 | S3_SAVE, &dbVar0);
		if ( dbVar0 & BIT7 ) break;	//break if EC is ready
		Stall(500); //wait for EC to become ready
	}

	if (getRevisionID() >= SB700_A14){
		programPciByteTable( (REG8MASK*)FIXUP_PTR(&sb710PorInitPciTable[0]), sizeof(sb710PorInitPciTable)/sizeof(REG8MASK) );
		programPmioByteTable( (REG8MASK *)FIXUP_PTR(&sb710PorPmioInitTbl[0]), (sizeof(sb710PorPmioInitTbl)/sizeof(REG8MASK)) );
	}

	RWPCI(((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGBA), AccWidthUint8 | S3_SAVE, 0xFF, BIT2);	//Enable SPI Prefetch in EC

	//Enable config mode
	EnterEcConfig();

	//Do settings for mailbox - logical device 0x09
	RWEC8(0x07, 0x00, 0x09);					//switch to device 9 (Mailbox)
	RWEC8(0x60, 0x00, (pBuildOptPtr->EcLdn9MailBoxAddr >> 8));		//set MSB of Mailbox port
	RWEC8(0x61, 0x00, (pBuildOptPtr->EcLdn9MailBoxAddr & 0xFF));	//set LSB of Mailbox port
	RWEC8(0x30, 0x00, 0x01);					//;Enable Mailbox Registers Interface, bit0=1

	if (pBuildOptPtr->EcKbd == CIMX_OPTION_ENABLED){
		RWPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG60+3), AccWidthUint8 | S3_SAVE, 0xFF, BIT7+BIT3);
		//Enable KBRST#, IRQ1 & IRQ12, GateA20 Function signal from IMC
		RWPMIO(SB_PMIO_REGBB, AccWidthUint8, 0xFF, BIT3+BIT2+BIT1+BIT0);
		//Disable LPC Decoding of port 60/64
		RWPCI(((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG47), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT5, 0);
		//Enable logical device 0x07 (Keyboard controller)
		RWEC8(0x07, 0x00, 0x07);
		RWEC8(0x30, 0x00, 0x01);
	}

	if (pBuildOptPtr->EcChannel0 == CIMX_OPTION_ENABLED){
		//Logical device 0x08
		RWEC8(0x07, 0x00, 0x08);
		RWEC8(0x60, 0x00, 0x00);
		RWEC8(0x61, 0x00, 0x62);
		RWEC8(0x30, 0x00, 0x01);				//;Enable Device 8
	}
	//Logical device 0x05
	RWEC8(0x07, 0x00, 0x05);			//Select logical device 05, IR controller
	RWEC8(0x60, 0x00, pBuildOptPtr->EcLdn5MailBoxAddr >> 8);
	RWEC8(0x61, 0x00, (pBuildOptPtr->EcLdn5MailBoxAddr & 0xFF));
	RWEC8(0x70, 0xF0, (pBuildOptPtr->EcLdn5Irq));			//Set IRQ to 05h
	RWEC8(0x30, 0x00, 0x01);			//Enable logical device 5, IR controller

	RWPMIO(SB_PMIO_REGBB, AccWidthUint8, 0xFF, BIT4); //Enable EC(IMC) to generate SMI to BIOS
	ExitEcConfig();
}


void	ecInitBeforePciEnum(AMDSBCFG* pConfig){
	if (!(isEcPresent()))
		return;				//return if EC is not enabled
}


void	ecInitLatePost(AMDSBCFG* pConfig){
	if (!(isEcPresent()) )
		return;				//return if EC is not enabled
	//Enable config mode
	EnterEcConfig();					//Enable config mode
	//for future use
	ExitEcConfig();
}

#endif
