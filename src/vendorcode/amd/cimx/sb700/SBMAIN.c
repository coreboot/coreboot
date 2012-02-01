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

#ifndef	B1_IMAGE

BUILDPARAM	DfltStaticOptions={
	BIOS_SIZE,							// BIOS Size
	LEGACY_FREE,						// Legacy Free Option
	0x00,								// Dummy space holder

	0x00,								// ECKbd disable/enable
	0x00,								// EcChannel0 disable/enable
	0x00,								// Dummy space holder1

	SMBUS0_BASE_ADDRESS,				// Smbus Base Address;
	SMBUS1_BASE_ADDRESS,				// Smbus Base Address;
	SIO_PME_BASE_ADDRESS,				// SIO PME Base Address
	WATCHDOG_TIMER_BASE_ADDRESS,		// Watchdog Timer Base Address
	SPI_BASE_ADDRESS,

	PM1_EVT_BLK_ADDRESS,				//	AcpiPm1EvtBlkAddr;
	PM1_CNT_BLK_ADDRESS,				//	AcpiPm1CntBlkAddr;
	PM1_TMR_BLK_ADDRESS,				//	AcpiPmTmrBlkAddr;
	CPU_CNT_BLK_ADDRESS,				//	CpuControlBlkAddr;
	GPE0_BLK_ADDRESS,					//  AcpiGpe0BlkAddr;
	SMI_CMD_PORT,						//	SmiCmdPortAddr;
	ACPI_PMA_CNT_BLK_ADDRESS,			//	AcpiPmaCntBlkAddr;

	EC_LDN5_MAILBOX_ADDRESS,
	EC_LDN5_IRQ,
	EC_LDN9_MAILBOX_ADDRESS,			//	EC LDN9 Mailbox address
	RESERVED_VALUE,
	RESERVED_VALUE,
	RESERVED_VALUE,
	RESERVED_VALUE,

	HPET_BASE_ADDRESS,					// HPET Base address

	SATA_IDE_MODE_SSID,
	SATA_RAID_MODE_SSID,
	SATA_RAID5_MODE_SSID,
	SATA_AHCI_SSID,

	OHCI0_SSID,
	OHCI1_SSID,
	EHCI0_SSID,
	OHCI2_SSID,
	OHCI3_SSID,
	EHCI1_SSID,
	OHCI4_SSID,
	SMBUS_SSID,
	IDE_SSID,
	AZALIA_SSID,
	LPC_SSID,
	P2P_SSID,
};


/*********************************************************************************
*
* Routine Description: Config SB Before PCI INIT
*
* Arguments:
*
*   pConfig - SBconfiguration
*
* Returns:
*
*   void
*
**********************************************************************************/
void	sbBeforePciInit (AMDSBCFG* pConfig){
	BUILDPARAM	*pStaticOptions;

	pStaticOptions = &pConfig->BuildParameters;
	TRACE((DMSG_SB_TRACE, "CIMx - Entering sbBeforePciInit \n"));
	commonInitEarlyBoot(pConfig);
	commonInitEarlyPost(pConfig);
#ifndef	NO_EC_SUPPORT
	ecInitBeforePciEnum(pConfig);
#endif
	usbInitBeforePciEnum(pConfig);				// USB POST TIME Only
	fcInitBeforePciEnum(pConfig);				// Preinit flash controller
	sataInitBeforePciEnum(pConfig);						// Init SATA class code and PHY
	programSubSystemIDs(pConfig, pStaticOptions);		// Set subsystem/vendor ID

	TRACE((DMSG_SB_TRACE, "CIMx - Exiting sbBeforePciInit \n"));
}


/*********************************************************************************
*
* Routine Description:	Config SB After PCI INIT
*
* Arguments:
*
*   pConfig - SBconfiguration
*
* Returns:	void
*
*  Reference:	atiSbAfterPciInit
*
**********************************************************************************/
void	sbAfterPciInit(AMDSBCFG* pConfig){
	BUILDPARAM	*pStaticOptions;

	TRACE((DMSG_SB_TRACE, "CIMx - Entering sbAfterPciInit \n"));

	pStaticOptions = &pConfig->BuildParameters;
	usbInitMidPost(pConfig);				//usb initialization which is required only during post
	usbInitAfterPciInit(pConfig);			// Init USB MMIO
	sataInitAfterPciEnum(pConfig);			// SATA port enumeration
	azaliaInitAfterPciEnum(pConfig);		// Detect and configure High Definition Audio

	TRACE((DMSG_SB_TRACE, "CIMx - Exiting sbAfterPciInit \n"));
}


/*********************************************************************************
*
* Routine Description:	Config SB during late POST
*
* Arguments:
*
*   pConfig - SBconfiguration
*
* Returns:	void
*
*  Reference:	atiSbLatePost
*
**********************************************************************************/
void	sbLatePost(AMDSBCFG* pConfig){
	UINT16	dwVar;
	BUILDPARAM	*pStaticOptions;
	pStaticOptions = &pConfig->BuildParameters;
	TRACE((DMSG_SB_TRACE, "CIMx - Entering sbLatePost \n"));
	ReadPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG02, AccWidthUint16, &dwVar);
	if (dwVar != SB7XX_DEVICE_ID){
		// Display message that the SB is wrong and stop the system
		TRACE((DMSG_SB_TRACE, "Current system does not have SB700 chipset. Stopping\n"));
		for(;;);
	}
	commonInitLateBoot(pConfig);
	sataInitLatePost(pConfig);
	hpetInit(pConfig, pStaticOptions);		// SB Configure HPET base and enable bit
#ifndef	NO_EC_SUPPORT
	ecInitLatePost(pConfig);
#endif
}

/*********************************************************************************
*
* Routine Description:	Config SB before ACPI S3 resume PCI config device restore
*
* Arguments:
*
*   pConfig - SBconfiguration
*
* Returns:	void
*
*  Reference:	AtiSbBfPciRestore
*
**********************************************************************************/
void	sbBeforePciRestoreInit(AMDSBCFG* pConfig){
	BUILDPARAM	*pStaticOptions;

	TRACE((DMSG_SB_TRACE, "CIMx - Entering sbBeforePciRestoreInit \n"));

	pConfig->S3Resume = 1;

	pStaticOptions = &pConfig->BuildParameters;
	commonInitEarlyBoot(pConfig);				// set /SMBUS/ACPI/IDE/LPC/PCIB
	abLinkInitBeforePciEnum(pConfig);			// Set ABCFG registers
	usbInitBeforePciEnum(pConfig);				// USB POST TIME Only
	fcInitBeforePciEnum(pConfig);				// Preinit flash controller
	sataInitBeforePciEnum(pConfig);
	programSubSystemIDs(pConfig, pStaticOptions);				// Set subsystem/vendor ID
}


/*********************************************************************************
*
* Routine Description:	Config SB after ACPI S3 resume PCI config device restore
*
* Arguments:
*
*   pConfig - SBconfiguration
*
* Returns:	void
*
*  Reference:	AtiSbAfPciRestore
*
**********************************************************************************/
void	sbAfterPciRestoreInit(AMDSBCFG* pConfig){
	BUILDPARAM	*pStaticOptions;

	pConfig->S3Resume = 1;

	pStaticOptions = &pConfig->BuildParameters;
	TRACE((DMSG_SB_TRACE, "CIMx - Entering sbAfterPciRestoreInit \n"));

	commonInitLateBoot(pConfig);
	sataInitAfterPciEnum(pConfig);
	azaliaInitAfterPciEnum(pConfig); 			// Detect and configure High Definition Audio
	hpetInit(pConfig, pStaticOptions);       	// SB Configure HPET base and enable bit
	sataInitLatePost(pConfig);
	sbSmmAcpiOn(pConfig);
}


/*++

Routine Description:

  SB config hook during ACPI_ON

Arguments:

  pConfig - SBconfiguration

Returns:

  void

--*/

void	sbSmmAcpiOn(AMDSBCFG* pConfig){
	UINT32     ddBar5;
	UINT8      dbPort;

	//RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG60+2, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT1+BIT0), 0);
	if (getRevisionID() >= SB700_A13)
		RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, 0xFF, BIT0);		//Enable Legacy DMA prefetch enhancement

	RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG60+2, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT1+BIT0), 0);
	RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG64+3, AccWidthUint8| S3_SAVE, ~(UINT32)BIT7, 0);
	programOhciMmioForEmulation();

	// For IDE_TO_AHCI_MODE and IDE_TO_AMD_AHCI_MODE, clear Interrupt Status register for all ports
	ReadPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, &ddBar5);
	if ((pConfig->SataClass == IDE_TO_AHCI_MODE) || (pConfig->SataClass == IDE_TO_AMD_AHCI_MODE)){
	  for (dbPort = 0; dbPort <= 5; dbPort++) {
	    RWMEM(ddBar5 + SB_SATA_BAR5_REG110 + dbPort * 0x80, AccWidthUint32, 0x00, 0xFFFFFFFF);
	  }
	}
}


UINT32	CallBackToOEM(UINT32 Func, UINTN Data,AMDSBCFG* pConfig){
	UINT32 Result=0;
	TRACE((DMSG_SB_TRACE,"OEM Call Back Func [%x] Data [%x]\n",Func,Data));
	if	(pConfig->StdHeader.pCallBack==NULL)
		return Result;
	Result = (*(pConfig->StdHeader.pCallBack))(Func,Data,pConfig);
	TRACE((DMSG_SB_TRACE,"SB Hook Status [%x]\n",Result));
	return Result;
}

#endif
