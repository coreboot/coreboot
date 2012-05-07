/*;********************************************************************************
;
; Copyright (C) 2012 Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/
#ifndef _AMD_SB_CIMx_OEM_H_
#define _AMD_SB_CIMx_OEM_H_

#ifndef BIOS_SIZE
	#define BIOS_SIZE					0x04	//04 - 1MB
#endif
#ifndef LEGACY_FREE
	#define	LEGACY_FREE					0x00
#endif

/**
 * PCIEX_BASE_ADDRESS - Define PCIE base address
 *
 * @param[Option]     MOVE_PCIEBAR_TO_F0000000 Set PCIe base address to 0xF7000000
 */
#ifdef  MOVE_PCIEBAR_TO_F0000000
  #define PCIEX_BASE_ADDRESS           0xF7000000
#else
  #define PCIEX_BASE_ADDRESS           0xE0000000
#endif


#define SMBUS0_BASE_ADDRESS			0xB00
#define SMBUS1_BASE_ADDRESS			0xB20
#define	SIO_PME_BASE_ADDRESS		0xE00
#define	SPI_BASE_ADDRESS			0xFEC10000

#define	WATCHDOG_TIMER_BASE_ADDRESS	0xFEC000F0			// Watchdog Timer Base Address
#define	HPET_BASE_ADDRESS			0xFED00000			// HPET Base address

#define PM1_EVT_BLK_ADDRESS			0x800				//	AcpiPm1EvtBlkAddr;
#define	PM1_CNT_BLK_ADDRESS			0x804				//	AcpiPm1CntBlkAddr;
#define	PM1_TMR_BLK_ADDRESS			0x808				//	AcpiPmTmrBlkAddr;
#define	CPU_CNT_BLK_ADDRESS			0x810				//	CpuControlBlkAddr;
#define	GPE0_BLK_ADDRESS			0x820				//  AcpiGpe0BlkAddr;
#define	SMI_CMD_PORT				0xB0				//	SmiCmdPortAddr;
#define ACPI_PMA_CNT_BLK_ADDRESS	0xFE00				//	AcpiPmaCntBlkAddr;

#define	EC_LDN5_MAILBOX_ADDRESS		0x550
#define	EC_LDN5_IRQ					0x05
#define	EC_LDN9_MAILBOX_ADDRESS		0x3E

#define	SATA_IDE_MODE_SSID			0x43901002
#define SATA_RAID_MODE_SSID			0x43921002
#define	SATA_RAID5_MODE_SSID		0x43931002
#define SATA_AHCI_SSID				0x43911002
#define OHCI0_SSID					0x43971002
#define OHCI1_SSID					0x43981002
#define EHCI0_SSID					0x43961002
#define OHCI2_SSID					0x43971002
#define OHCI3_SSID					0x43981002
#define EHCI1_SSID					0x43961002
#define OHCI4_SSID					0x43991002

#define SMBUS_SSID					0x43851002
#define IDE_SSID					0x439C1002
#define AZALIA_SSID					0x43831002
#define LPC_SSID					0x439D1002
#define P2P_SSID					0x43841002

#define RESERVED_VALUE				0x00

#endif //ifndef _AMD_SB_CIMx_OEM_H_
