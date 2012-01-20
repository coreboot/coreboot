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

//Table for class code of SATA Controller in different modes
UINT32 sataIfCodeTable[] = {
	0x01018f00,	//sata class ID of IDE
	0x01040000,	//sata class ID of RAID
	0x01060100,	//sata class ID of AHCI
	0x01018a00,	//sata class ID of Legacy IDE
	0x01018f00,	//sata class ID of IDE to AHCI mode
	0x01060100,	//sata class ID of AMD-AHCI mode
	0x01018f00	//sata class ID of IDE to AMD-AHCI mode
};

//Table for device id of SATA Controller in different modes
UINT16 sataDeviceIDTable[] = {
	0x4390,	//sata device ID of IDE
	0x4392,	//sata device ID of RAID
	0x4391,	//sata class ID of AHCI
	0x4390,	//sata device ID of Legacy IDE
	0x4390,	//sata device ID of IDE->AHCI mode
	0x4394,	//sata device ID for AMD-AHCI mode
	0x4390	//sata device ID of IDE->AMDAHCI mode
};


void sataInitBeforePciEnum(AMDSBCFG*	pConfig){
	UINT32	ddValue, *tempptr;
	UINT16	*pDeviceIdptr, dwDeviceId;
	UINT8	dbValue, dbOrMask, dbAndMask;


	dbAndMask=0;
	dbOrMask=0;
	// Enable/Disable Combined mode & do primary/secondary selections, enable/disable
	if (pConfig->SataIdeCombinedMode == CIMX_OPTION_DISABLED) 		dbAndMask= BIT3;		//Clear BIT3
	if (pConfig->SataIdeCombMdPriSecOpt == 1) 	dbOrMask = BIT4;		//Set BIT4
	if (pConfig->SataSmbus == 0)	dbOrMask = BIT1;

	RWPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGAD), AccWidthUint8 | S3_SAVE, ~(dbAndMask), dbOrMask);

	if (pConfig->SataController == 0){
		// SATA Controller Disabled & set Power Saving mode to disabled
		RWPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGAD), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, BIT1);
		return;
	}

	restrictSataCapabilities(pConfig);

	//	Get the appropriate class code from the table and write it to PCI register 08h-0Bh
	//	Set the appropriate SATA class based on the input parameters
	dbValue=pConfig->SataClass;
	tempptr= (UINT32 *) FIXUP_PTR (&sataIfCodeTable[0]);
	ddValue=tempptr[dbValue];

	// BIT0: Enable write access to PCI header (reg 08h-0Bh) by setting SATA PCI register 40h, bit 0
	// BIT4:disable fast boot
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT4+BIT0);

	// Write the class code to SATA PCI register 08h-0Bh
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG08), AccWidthUint32 | S3_SAVE, 0, ddValue);

	if	(pConfig->SataClass == LEGACY_IDE_MODE)		//SATA = Legacy IDE
		//Set PATA controller to native mode
		RWPCI(((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG09), AccWidthUint8 | S3_SAVE, 0x00, 0x08F);

	//Change the appropriate device id
	if (pConfig->SataClass == AMD_AHCI_MODE)	{
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 3), AccWidthUint8 | S3_SAVE, 0xff, BIT0);
	}
	pDeviceIdptr= (UINT16 *) FIXUP_PTR (&sataDeviceIDTable[0]);

	ReadPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, &dwDeviceId);
	if ( !((dwDeviceId==SB750_SATA_DEFAULT_DEVICE_ID) && (pConfig->SataClass == RAID_MODE)) ){
		//if not (SB750 & RAID mode), then program the device id
		dwDeviceId=pDeviceIdptr[dbValue];
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, dwDeviceId);
	}

	if (pConfig->AcpiS1Supported)
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG34), AccWidthUint8 | S3_SAVE, 00, 0x70);//Disable SATA PM & MSI capability
	else
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG60+1), AccWidthUint8 | S3_SAVE, 00, 0x70);//Disable SATA MSI capability

	if (getRevisionID() >= SB700_A13){
		//Enable test/enhancement mode for A13
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40+3), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT5, 00);
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48), AccWidthUint32 | S3_SAVE, ~(UINT32)(BIT24+BIT21), 0xBF80);
	}

	if (getRevisionID() >= SB700_A14){
		//Fix for TT SB01352 -  LED Stays On When ODD Attached To Slave Port In IDE Mode
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48), AccWidthUint8 | S3_SAVE, 0xFF, BIT6);
	}

	// Disable write access to PCI header
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, 0);

	// RPR 6.5 SATA PHY Programming Sequence
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG86, AccWidthUint16 | S3_SAVE, 0x00, 0x2C00);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG88, AccWidthUint32 | S3_SAVE, 0x00, 0x01B48016);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG8C, AccWidthUint32 | S3_SAVE, 0x00, 0x01B48016);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG90, AccWidthUint32 | S3_SAVE, 0x00, 0x01B48016);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG94, AccWidthUint32 | S3_SAVE, 0x00, 0x01B48016);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG98, AccWidthUint32 | S3_SAVE, 0x00, 0x01B48016);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG9C, AccWidthUint32 | S3_SAVE, 0x00, 0x01B48016);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REGA0, AccWidthUint32 | S3_SAVE, 0x00, 0xA07AA07A);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REGA4, AccWidthUint32 | S3_SAVE, 0x00, 0xA07AA07A);
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REGA8, AccWidthUint32 | S3_SAVE, 0x00, 0xA07AA07A);

	CallBackToOEM(SATA_PHY_PROGRAMMING, NULL, pConfig);
}

void sataInitAfterPciEnum(AMDSBCFG* pConfig){
	UINT32	ddAndMask=0, ddOrMask=0, ddBar5=0;
	UINT8	dbVar, dbPortNum;

	if (pConfig->SataController == 0) return;		//return if SATA controller is disabled.

	//Enable write access to pci header, pm capabilities
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xFF, BIT0);

        //Disable AHCI enhancement function (RPR 7.2)
        RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8 | S3_SAVE, 0xFF, BIT7);

	restrictSataCapabilities(pConfig);

	ReadPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, &ddBar5);

	if	( (ddBar5 == 0) || (ddBar5 == -1) ) {
		//assign temporary BAR5
		if	( (pConfig->TempMMIO == 0) || (pConfig->TempMMIO == -1))
			ddBar5 = 0xFEC01000;
		else
			ddBar5=pConfig->TempMMIO;

		WritePCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, &ddBar5);
	}

	ReadPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
	RWPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8,0xFF, 0x03);	//memory and io access enable

	ddBar5 &= 0xFFFFFC00;			//Clear Bits 9:0
	if (!pConfig->SataPortMultCap)
		ddAndMask |= BIT12;
	if (!pConfig->SataAggrLinkPmCap)
		ddAndMask |= BIT11;
	if (pConfig->SataSscPscCap)
		ddOrMask |= BIT1;

	RWMEM((ddBar5 + SB_SATA_BAR5_REGFC),AccWidthUint32 | S3_SAVE, ~ddAndMask, ddOrMask);


	//Clear HPCP and ESP by default
	RWMEM((ddBar5 + SB_SATA_BAR5_REGF8),AccWidthUint32 | S3_SAVE, 0xFFFC0FC0, 0);

	if	(pConfig->SataHpcpButNonESP !=0) {
		RWMEM((ddBar5 + SB_SATA_BAR5_REGF8),AccWidthUint32 | S3_SAVE, 0xFFFFFFC0, pConfig->SataHpcpButNonESP);
	}

	// SATA ESP port setting
	// These config bits are set for SATA driver to identify which ports are external SATA ports and need to
	// support hotplug. If a port is set as an external SATA port and need to support hotplug, then driver will
	// not enable power management(HIPM & DIPM) for these ports.
	if	(pConfig->SataEspPort !=0) {
		RWMEM((ddBar5 + SB_SATA_BAR5_REGFC),AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, BIT20);
		RWMEM((ddBar5 + SB_SATA_BAR5_REGF8),AccWidthUint32 | S3_SAVE, ~(pConfig->SataEspPort), 0);
		RWMEM((ddBar5 + SB_SATA_BAR5_REGF8),AccWidthUint32 | S3_SAVE, ~(UINT32)(BIT17+BIT16+BIT15+BIT14+BIT13+BIT12),(pConfig->SataEspPort << 12));
	}

	if	( ((pConfig->SataClass) != NATIVE_IDE_MODE)  && ((pConfig->SataClass) != LEGACY_IDE_MODE) )
		RWPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50+2), AccWidthUint8, ~(UINT32)(BIT3+BIT2+BIT1), BIT2+BIT1); //set MSI to 8 messages

	if	( ((pConfig->SataClass) != NATIVE_IDE_MODE)  && ((pConfig->SataClass) != LEGACY_IDE_MODE) && ((pConfig->SataIdeCombinedMode) == CIMX_OPTION_DISABLED) ){
		RWMEM((ddBar5 + SB_SATA_BAR5_REG00),AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT2+BIT1+BIT0), BIT2+BIT0);
		RWMEM((ddBar5 + SB_SATA_BAR5_REG0C),AccWidthUint8 | S3_SAVE, 0xC0, 0x3F);
	}

	for (dbPortNum=0;dbPortNum<=5;dbPortNum++){
		if (pConfig->SataPortMode & (1 << dbPortNum)){
			//downgrade to GEN1
			RWMEM(ddBar5+ SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0x0F, 0x10);
			RWMEM(ddBar5+ SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFF, 0x01);
			Stall(1000);
			RWMEM(ddBar5+ SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFE, 0x00);
		}
	}

	//If this is not S3 resume and also if SATA set to one of IDE mode, then implement drive detection workaround.
	if ( !(pConfig->S3Resume) && ( ((pConfig->SataClass) != AHCI_MODE)  && ((pConfig->SataClass) != RAID_MODE) && ((pConfig->SataClass) != AMD_AHCI_MODE) ) )
		sataDriveDetection(pConfig, ddBar5);

	if ( (pConfig->SataPhyWorkaround==1) || ( (pConfig->SataPhyWorkaround==0) && (getRevisionID() < SB700_A13)) )
		sataPhyWorkaround(pConfig, ddBar5);

	// Set the handshake bit for IDE driver to detect the disabled IDE channel correctly.
	// Set IDE PCI Config 0x63 [3] if primary channel disabled, [4] if secondary channel disabled.
	if (pConfig->SataIdeCombinedMode == CIMX_OPTION_DISABLED)
		RWPCI( ((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG63), AccWidthUint8 , 0xF9, (0x02 << (pConfig->SataIdeCombMdPriSecOpt)) );

	WritePCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);

	//Disable write access to pci header, pm capabilities
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, 0);
}


void sataDriveDetection(AMDSBCFG* pConfig, UINT32 ddBar5){
	UINT32	ddVar0;
	UINT8	dbPortNum, dbVar0;
	UINT16	dwIoBase, dwVar0;

	TRACE((DMSG_SB_TRACE, "CIMx - Entering sata drive detection procedure\n\n"));
	TRACE((DMSG_SB_TRACE, "SATA BAR5 is %X \n", ddBar5));

	if ( (pConfig->SataClass == NATIVE_IDE_MODE) || (pConfig->SataClass == LEGACY_IDE_MODE) || (pConfig->SataClass == IDE_TO_AHCI_MODE) || (pConfig->SataClass == IDE_TO_AMD_AHCI_MODE) ){
		for (dbPortNum=0;dbPortNum<4;dbPortNum++){
			ReadMEM(ddBar5+ SB_SATA_BAR5_REG128 + dbPortNum * 0x80, AccWidthUint32, &ddVar0);
			if ( ( ddVar0 & 0x0F ) == 0x03){
				if ( dbPortNum & BIT0)
					//this port belongs to secondary channel
					ReadPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG18), AccWidthUint16, &dwIoBase);
				else
					//this port belongs to primary channel
					ReadPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG10), AccWidthUint16, &dwIoBase);

				//if legacy ide mode, then the bar registers don't contain the correct values. So we need to hardcode them
				if (pConfig->SataClass == LEGACY_IDE_MODE)
					dwIoBase = ( (0x170) | ( (~((dbPortNum & BIT0) << 7)) & 0x80 ) );

				if ( dbPortNum & BIT1)
					//this port is slave
					dbVar0=0xB0;
				else
					//this port is master
					dbVar0=0xA0;
				dwIoBase &= 0xFFF8;
				WriteIO(dwIoBase+6, AccWidthUint8, &dbVar0);

				//Wait in loop for 30s for the drive to become ready
				for (dwVar0=0;dwVar0<3000;dwVar0++){
					ReadIO(dwIoBase+7, AccWidthUint8, &dbVar0);
					if ( (dbVar0 & 0x88) == 0)
						break;
					Stall(10000);
				}
			}	//end of if ( ( ddVar0 & 0x0F ) == 0x03)
		}	//for (dbPortNum=0;dbPortNum<4;dbPortNum++)
	}	//if ( (pConfig->SataClass == NATIVE_IDE_MODE) || (pConfig->SataClass == LEGACY_IDE_MODE) || (pConfig->SataClass == IDE_TO_AHCI_MODE) || (pConfig->SataClass == IDE_TO_AMD_AHCI_MODE) )
}


//This patch is to workaround the SATA PHY logic hardware issue in the SB700.
//Internally this workaround is called as 7NewA
void	sataPhyWorkaround(AMDSBCFG* pConfig, UINT32 ddBar5){

	UINT8	dbPortNum, dbVar0;

	if (pConfig->Gen1DeviceShutdownDuringPhyWrknd == 0x01){
		for (dbPortNum=0;dbPortNum<=5;dbPortNum++){
			ReadMEM(ddBar5+ SB_SATA_BAR5_REG128 + dbPortNum * 0x80, AccWidthUint8, &dbVar0);
			if ( (dbVar0 & 0xF0) == 0x10){
				RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40+2, AccWidthUint8 | S3_SAVE, 0xFF, (01 << dbPortNum));
			}

		}
	}

	RWPMIO(SB_PMIO_REGD0, AccWidthUint8, ~(UINT32)(BIT4+BIT3), BIT4+BIT3);//set PMIO_D0[4:3] = 11b // this is to tell SATA PHY to use the internal 100MHz clock
	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG86, AccWidthUint8 | S3_SAVE, 0x00, 0x40);//	set SATA PCI_CFG 0x86[7:0] = 0x40  //after the reset is done, perform this to turn on the diff clock path into SATA PHY
	Stall(2000);//	Wait for 2ms
	RWPMIO(SB_PMIO_REGD0, AccWidthUint8, ~(UINT32)(BIT4+BIT3), 00);//13.	set PMIO_D0[4:3] = 00b
	Stall(20000);//	Wait 20ms
	forceOOB(ddBar5);//	Force OOB

	RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40+2, AccWidthUint8 | S3_SAVE, ~(0x03F), 00);
}


void	forceOOB(UINT32	ddBar5){
	UINT8 dbPortNum;
	for (dbPortNum=0;dbPortNum<=5;dbPortNum++)
		RWMEM(ddBar5+ SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFF, 0x01);
	Stall(2000);
	for (dbPortNum=0;dbPortNum<=5;dbPortNum++)
		RWMEM(ddBar5+ SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFE, 0x00);
	Stall(2000);//	Wait for 2ms
}

/*++

Routine Description:

  SATA Late Configuration

  if the mode is selected as IDE->AHCI
  { 1. Set class ID to AHCI
    2. Enable AHCI interrupt
  }

Arguments:

  pConfig - SBconfiguration

Returns:

  void

--*/
void sataInitLatePost(AMDSBCFG* pConfig){
	UINT32	ddBar5;
	UINT8	dbVar;

	//Return immediately is sata controller is not enabled
	if (pConfig->SataController == 0) return;

	restrictSataCapabilities(pConfig);

	//Get BAR5 value
	ReadPCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, &ddBar5);

	//Assign temporary BAR if is not already assigned
	if	( (ddBar5 == 0) || (ddBar5 == -1) ){
		//assign temporary BAR5
		if	( (pConfig->TempMMIO == 0) || (pConfig->TempMMIO == -1))
			ddBar5 = 0xFEC01000;
		else
			ddBar5=pConfig->TempMMIO;
		WritePCI( ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, &ddBar5);
	}

	ReadPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
	//Enable memory and io access
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, 0xFF, 0x03);
	//Enable write access to pci header, pm capabilities
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT0);

	shutdownUnconnectedSataPortClock(pConfig, ddBar5);

	if ( (pConfig->SataClass == IDE_TO_AHCI_MODE) || (pConfig->SataClass == IDE_TO_AMD_AHCI_MODE)){
		//program the AHCI class code
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG08), AccWidthUint32 | S3_SAVE, 0, 0x01060100);
		//Set interrupt enable bit
		RWMEM((ddBar5 + 0x04),AccWidthUint8,~(UINT32)0,BIT1);
		//program the correct device id for AHCI mode
		RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, 0x4391);

		if (pConfig->SataClass == IDE_TO_AMD_AHCI_MODE)
			//program the correct device id for AMD-AHCI mode
			RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 3), AccWidthUint8 | S3_SAVE, 0xFF, BIT0);
	}

	//Disable write access to pci header and pm capabilities
	RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, 0);
	//Clear error status
	RWMEM((ddBar5 + SB_SATA_BAR5_REG130),AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
	RWMEM((ddBar5 + SB_SATA_BAR5_REG1B0),AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
	RWMEM((ddBar5 + SB_SATA_BAR5_REG230),AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
	RWMEM((ddBar5 + SB_SATA_BAR5_REG2B0),AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
	//Restore memory and io access bits
	WritePCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar );
}


void shutdownUnconnectedSataPortClock(AMDSBCFG* pConfig, UINT32 ddBar5){
	UINT8	dbPortNum, dbPortSataStatus, NumOfPorts=0;
	UINT8	UnusedPortBitMap;
	UINT8	SataType;
	UINT8   ClockOffEnabled ;

	UnusedPortBitMap = 0;

	// First scan for all unused SATA ports
	for (dbPortNum = 5; dbPortNum <= 5; dbPortNum--) {
	  ReadMEM (ddBar5 + SB_SATA_BAR5_REG128 + (dbPortNum * 0x80), AccWidthUint8, &dbPortSataStatus);
	  if ((!(dbPortSataStatus & 0x01)) && (!((pConfig->SataEspPort) & (1 << dbPortNum)))) {
	    UnusedPortBitMap |= (1 << dbPortNum);
	  }
        }

	// Decide if we need to shutdown the clock for all unused ports
	SataType = pConfig->SataClass;
	ClockOffEnabled = (pConfig->SataClkAutoOff && ((SataType == NATIVE_IDE_MODE) || (SataType == LEGACY_IDE_MODE) || \
	                                                (SataType == IDE_TO_AHCI_MODE) || (SataType == IDE_TO_AMD_AHCI_MODE))) || \
			  (pConfig->SataClkAutoOffAhciMode && ((SataType == AHCI_MODE) || (SataType == AMD_AHCI_MODE)));

	if (ClockOffEnabled) {
	  //Shutdown the clock for the port and do the necessary port reporting changes.
	  TRACE((DMSG_SB_TRACE, "Shutting down clock for SATA ports %X \n", UnusedPortBitMap));
	  RWPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, 0xFF, UnusedPortBitMap);
	  RWMEM(ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, ~UnusedPortBitMap, 00);
	}

	// If all ports are in disabled state, report at least one
	ReadMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, &dbPortSataStatus);
	if ( (dbPortSataStatus & 0x3F) == 0) {
	  dbPortSataStatus = 1;
	  RWMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, ~(0x3F), dbPortSataStatus);
	}

	// Decide if we need to hide unused ports from being seen by OS (this saves OS startup time)
	if (pConfig->SataHideUnusedPort && ClockOffEnabled) {
	  dbPortSataStatus &= ~UnusedPortBitMap;    // Mask off unused ports
	  for (dbPortNum = 0; dbPortNum <= 6; dbPortNum++) {
	    if (dbPortSataStatus & (1 << dbPortNum))
	      NumOfPorts++;
	    }
	  if (NumOfPorts == 0 ) {
		NumOfPorts = 0x01;
	    }
	  RWMEM (ddBar5 + SB_SATA_BAR5_REG00, AccWidthUint8, 0xE0, NumOfPorts - 1);
	}
}


void	restrictSataCapabilities(AMDSBCFG* pConfig){
	//Restrict capabilities
	if ( ((getSbCapability(Sb_Raid0_1_Capability)== 0x02) && (pConfig->SataClass == RAID_MODE)) || \
		((getSbCapability(Sb_Raid5_Capability)== 0x02) && (pConfig->SataClass == RAID_MODE))  || \
		((getSbCapability(Sb_Ahci_Capability)== 0x02) && ((pConfig->SataClass == AHCI_MODE) || (pConfig->SataClass == IDE_TO_AHCI_MODE)))){
			pConfig->SataClass = NATIVE_IDE_MODE;
		}
}
