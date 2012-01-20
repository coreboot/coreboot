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


void usbInitBeforePciEnum(AMDSBCFG* pConfig){
	UINT8	dbVar=0;

	TRACE((DMSG_SB_TRACE, "Entering PreInit Usb \n"));
	if (pConfig->Usb1Ohci0){
		dbVar = (pConfig->Usb1Ehci << 2);
		dbVar |= ((pConfig->Usb1Ohci0) << 0);
		dbVar |= ((pConfig->Usb1Ohci1) << 1);
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG68, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT0+BIT1+BIT2), dbVar );
	}
	if (pConfig->Usb2Ohci0){
		dbVar = (pConfig->Usb2Ehci << 6) ;
		dbVar |= ((pConfig->Usb2Ohci0) << 4);
		dbVar |= ((pConfig->Usb2Ohci1) << 5);
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG68, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT6+BIT4+BIT5), dbVar );
	}
	if (pConfig->Usb3Ohci)
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG68, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT7), ((pConfig->Usb3Ohci) << 7) );

	RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50+1, AccWidthUint16 | S3_SAVE, ~(UINT32)(BIT4), BIT4);
}


void usbInitAfterPciInit(AMDSBCFG* pConfig){
	UINT32	ddBarAddress, ddVar;

	ReadPCI((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG10, AccWidthUint32, &ddBarAddress);//Get BAR address
	if ( (ddBarAddress != -1) && (ddBarAddress != 0) ){
		//Enable Memory access
		RWPCI((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG04, AccWidthUint8, 0, BIT1);
		//USB Common PHY CAL & Control Register setting
		ddVar = 0x00020F00;
		WriteMEM(ddBarAddress+SB_EHCI_BAR_REGC0, AccWidthUint32, &ddVar);
		//RPR - IN AND OUT DATA PACKET FIFO THRESHOLD
		//EHCI BAR 0xA4 //IN threshold bits[7:0]=0x40	//OUT threshold bits[23:16]=0x40
		RWMEM(ddBarAddress+SB_EHCI_BAR_REGA4, AccWidthUint32, 0xFF00FF00, 0x00400040);
		//RPR - EHCI dynamic clock gating feature
		//EHCI_BAR 0xBC Bit[12] = 0, For normal operation, the clock gating feature must be disabled.
		// Disables HS uFrame babble detection for erratum: EHCI_EOR + 9Ch [11] = 1
		RWMEM(ddBarAddress+SB_EHCI_BAR_REGBC, AccWidthUint16, ~(UINT32)(BIT12+BIT11), BIT11);
	}

	ReadPCI((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG10, AccWidthUint32, &ddBarAddress);//Get BAR address
	if ( (ddBarAddress != -1) && (ddBarAddress != 0) ){
		//Enable Memory access
		RWPCI((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG04, AccWidthUint8, 0, BIT1);
		//USB Common PHY CAL & Control Register setting
		ddVar = 0x00020F00;
		WriteMEM(ddBarAddress+SB_EHCI_BAR_REGC0, AccWidthUint32, &ddVar);
		//RPR - IN AND OUT DATA PACKET FIFO THRESHOLD
		//EHCI BAR 0xA4 //IN threshold bits[7:0]=0x40	//OUT threshold bits[23:16]=0x40
		RWMEM(ddBarAddress+SB_EHCI_BAR_REGA4, AccWidthUint32, 0xFF00FF00, 0x00400040);
		//RPR - EHCI dynamic clock gating feature
		//EHCI_BAR 0xBC Bit[12] = 0, For normal operation, the clock gating feature must be disabled.
		// Disables HS uFrame babble detection for erratum: EHCI_EOR + 9Ch [11] = 1
		RWMEM(ddBarAddress+SB_EHCI_BAR_REGBC, AccWidthUint16, ~(UINT32)(BIT12+BIT11), BIT11);
	}

	if (pConfig->UsbPhyPowerDown)
		RWPMIO(SB_PMIO_REG65, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, BIT0);
	else
		RWPMIO(SB_PMIO_REG65, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, 0);

	// Disable the MSI capability of USB host controllers
	RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG40+1, AccWidthUint8 | S3_SAVE, 0xFF, BIT1+BIT0);
	RWPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG40+1, AccWidthUint8 | S3_SAVE, 0xFF, BIT1+BIT0);
	RWPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG40+1, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);

	//RPR recommended setting "EHCI Advance Asynchronous Enhancement DISABLE"
	//Set EHCI_pci_configx50[28]='1' to disable the advance async enhancement feature to avoid the bug found in Linux.
	//Set EHCI_pci_configx50[6]='1' to disable EHCI MSI support
	//RPR recommended setting "EHCI Async Park Mode"
	//Set EHCI_pci_configx50[23]='1' to disable "EHCI Async Park Mode support"
	// RPR recommended setting "EHCI Advance PHY Power Savings"
	// Set EHCI_pci_configx50[31]='1' if SB700 A12 & above
	// Fix for EHCI controller driver  yellow sign issue under device manager
	// when used in conjunction with HSET tool driver. EHCI PCI config 0x50[20]=1
	RWPCI((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, BIT31+BIT28+BIT23+BIT20+BIT6);
	RWPCI((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, BIT31+BIT28+BIT23+BIT20+BIT6);

	//RPR recommended setting to, enable fix to cover the corner case S3 wake up issue from some USB 1.1 devices
	//OHCI 0_PCI_Config 0x50[16] = 1
	RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50+2, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);
	RWPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50+2, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);
	RWPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50+2, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);

	if (getRevisionID() >= SB700_A14){
		RWPCI((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~(UINT32)(BIT28), BIT8+BIT7+BIT4+BIT3);
		RWPCI((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~(UINT32)(BIT28), BIT8+BIT7+BIT4+BIT3);

		RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, BIT26+BIT25+BIT17);
		RWPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, BIT26+BIT25+BIT17);
		RWPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, BIT26+BIT25);
	}

	if (getRevisionID() >= SB700_A15) {
		//USB PID Error checking
		RWPCI((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50+1, AccWidthUint8 | S3_SAVE, 0xFF, BIT1);
		RWPCI((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50+1, AccWidthUint8 | S3_SAVE, 0xFF, BIT1);
	}

	// RPR 6.25 - Optionally disable OHCI isochronous out prefetch
	if (pConfig->OhciIsoOutPrefetchDis) {
	  RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint16 | S3_SAVE, ~(UINT32)(BIT9 + BIT8), 0);
	  RWPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint16 | S3_SAVE, ~(UINT32)(BIT9 + BIT8), 0);
	  RWPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG50, AccWidthUint16 | S3_SAVE, ~(UINT32)(BIT9 + BIT8), 0);
	}

	if ( pConfig->EhciDataCacheDis ) {
      // Disable Async Data Cache, EHCI_pci_configx50[26]='1'
      RWPCI ((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~(UINT32)BIT26, BIT26);
	  RWPCI ((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG50, AccWidthUint32 | S3_SAVE, ~(UINT32)BIT26, BIT26);
    }
}


void	usbInitMidPost(AMDSBCFG* pConfig){
	if (pConfig->UsbOhciLegacyEmulation == 0){
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG60+2, AccWidthUint8 | S3_SAVE, 0xFF, BIT1+BIT0);
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG64+3, AccWidthUint8 | S3_SAVE, 0xFF, BIT7);
	}
	else{
		programOhciMmioForEmulation();
	}
}


void programOhciMmioForEmulation(void){
	UINT32	ddBarAddress;

	ReadPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG10, AccWidthUint32, &ddBarAddress);//Get BAR address
	ddBarAddress &= 0xFFFFF000;
	if ( (ddBarAddress != 0xFFFFF000) && (ddBarAddress != 0) ){
		//Enable Memory access
		RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG04, AccWidthUint8, 0, BIT1);
		RWMEM(ddBarAddress+SB_OHCI_BAR_REG160, AccWidthUint32, 0, 0);
	}

	ReadPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG10, AccWidthUint32, &ddBarAddress);//Get BAR address
	ddBarAddress &= 0xFFFFF000;
	if ( (ddBarAddress != 0xFFFFF000) && (ddBarAddress != 0) ){
		//Enable Memory access
		RWPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG04, AccWidthUint8, 0, BIT1);
		RWMEM(ddBarAddress+SB_OHCI_BAR_REG160, AccWidthUint32, 0, 0);
	}

	ReadPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG10, AccWidthUint32, &ddBarAddress);//Get BAR address
	if ( (ddBarAddress != 0xFFFFF000) && (ddBarAddress != 0) ){
		//Enable Memory access
		RWPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG04, AccWidthUint8, 0, BIT1);
		RWMEM(ddBarAddress+SB_OHCI_BAR_REG160, AccWidthUint32, 0, 0);
	}
}
