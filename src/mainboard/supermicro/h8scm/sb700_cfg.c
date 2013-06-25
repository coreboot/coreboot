/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <string.h>
#include <console/console.h>    /* printk */
#include "Platform.h"
#include "sb700_cfg.h"


/**
 * @brief South Bridge CIMx configuration
 *
 * should be called before exeucte CIMx function.
 * this function will be called in romstage and ramstage.
 */
void sb700_cimx_config(AMDSBCFG *sb_config)
{
	if (!sb_config) {
		printk(BIOS_DEBUG, "SB700 - Cfg.c - sb700_cimx_config - No sb_config.\n");
		return;
	}
	printk(BIOS_DEBUG, "SB700 - Cfg.c - sb700_cimx_config - Start.\n");
	memset(sb_config, 0, sizeof(AMDSBCFG));

	/* SB_POWERON_INIT */
	sb_config->StdHeader.Func = SB_POWERON_INIT;

	/* header */
	sb_config->StdHeader.pPcieBase = PCIEX_BASE_ADDRESS;

	/* static Build Parameters */
	sb_config->BuildParameters.BiosSize = BIOS_SIZE;
	sb_config->BuildParameters.LegacyFree = LEGACY_FREE;
	sb_config->BuildParameters.EcKbd = 0;
	sb_config->BuildParameters.EcChannel0 = 0;
	sb_config->BuildParameters.Smbus0BaseAddress = SMBUS0_BASE_ADDRESS;
	sb_config->BuildParameters.Smbus1BaseAddress = SMBUS1_BASE_ADDRESS;
	sb_config->BuildParameters.SioPmeBaseAddress = SIO_PME_BASE_ADDRESS;
	sb_config->BuildParameters.WatchDogTimerBase = WATCHDOG_TIMER_BASE_ADDRESS;
	sb_config->BuildParameters.SpiRomBaseAddress = SPI_BASE_ADDRESS;

	sb_config->BuildParameters.AcpiPm1EvtBlkAddr = PM1_EVT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPm1CntBlkAddr = PM1_CNT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPmTmrBlkAddr = PM1_TMR_BLK_ADDRESS;
	sb_config->BuildParameters.CpuControlBlkAddr = CPU_CNT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiGpe0BlkAddr = GPE0_BLK_ADDRESS;
	sb_config->BuildParameters.SmiCmdPortAddr = SMI_CMD_PORT;
	sb_config->BuildParameters.AcpiPmaCntBlkAddr = ACPI_PMA_CNT_BLK_ADDRESS;

	sb_config->BuildParameters.SataIDESsid = SATA_IDE_MODE_SSID;
	sb_config->BuildParameters.SataRAIDSsid = SATA_RAID_MODE_SSID;
	sb_config->BuildParameters.SataRAID5Ssid = SATA_RAID5_MODE_SSID;
	sb_config->BuildParameters.SataAHCISsid = SATA_AHCI_SSID;
	sb_config->BuildParameters.Ohci0Ssid = OHCI0_SSID;
	sb_config->BuildParameters.Ohci1Ssid = OHCI1_SSID;
	sb_config->BuildParameters.Ohci2Ssid = OHCI2_SSID;
	sb_config->BuildParameters.Ohci3Ssid = OHCI3_SSID;
	sb_config->BuildParameters.Ohci4Ssid = OHCI4_SSID;
	sb_config->BuildParameters.Ehci0Ssid = EHCI0_SSID;
	sb_config->BuildParameters.Ehci1Ssid = EHCI1_SSID;
	sb_config->BuildParameters.SmbusSsid = SMBUS_SSID;
	sb_config->BuildParameters.IdeSsid = IDE_SSID;
	sb_config->BuildParameters.AzaliaSsid = AZALIA_SSID;
	sb_config->BuildParameters.LpcSsid = LPC_SSID;

	sb_config->BuildParameters.HpetBase = HPET_BASE_ADDRESS;

	/* General */
	sb_config->Spi33Mhz = 1;
	sb_config->SpreadSpectrum = 0;
	sb_config->PciClk5 = 1;
	sb_config->PciClks = 0x1F;
	sb_config->ResetCpuOnSyncFlood = 1; // Do not reset CPU on sync flood
	sb_config->TimerClockSource = 2;  // Auto
	sb_config->S3Resume = 0;
	sb_config->RebootRequired = 0;

	/* HPET */
	sb_config->HpetTimer = HPET_TIMER;

	/* USB */
	sb_config->UsbIntClock = 0;     // Use external clock
	sb_config->Usb1Ohci0 = 1; //0:disable  1:enable Bus 0 Dev 18 Func0
	sb_config->Usb1Ohci1 = 1; //0:disable  1:enable Bus 0 Dev 18 Func1
	sb_config->Usb1Ehci  = 1; //0:disable  1:enable Bus 0 Dev 18 Func2
	sb_config->Usb2Ohci0 = 1; //0:disable  1:enable Bus 0 Dev 19 Func0
	sb_config->Usb2Ohci1 = 1; //0:disable  1:enable Bus 0 Dev 19 Func1
	sb_config->Usb2Ehci  = 1; //0:disable  1:enable Bus 0 Dev 19 Func2
	sb_config->Usb3Ohci  = 1; //0:disable  1:enable Bus 0 Dev 20 Func5
	sb_config->UsbOhciLegacyEmulation = 1; //0:Enable  1:Disable

	sb_config->AcpiS1Supported = 1;

	/* SATA */
	sb_config->SataController = 1;
	sb_config->SataClass = CONFIG_SATA_CONTROLLER_MODE; //0 native, 1 raid, 2 ahci
	sb_config->SataSmbus = 0;
	sb_config->SataAggrLinkPmCap = 1;
	sb_config->SataPortMultCap = 1;
	sb_config->SataClkAutoOff = 1;
	sb_config->SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary, 1 -IDE as secondary.
	//TODO: set to secondary not take effect.
	sb_config->SataIdeCombinedMode = 0; //1 IDE controlor exposed and combined mode enabled, 0 disabled
	sb_config->SataEspPort = 0;
	sb_config->SataClkAutoOffAhciMode = 1;
	sb_config->SataHpcpButNonESP = 0;
	sb_config->SataHideUnusedPort = 0;

	/* Azalia HDA */
	sb_config->AzaliaController = AZALIA_CONTROLLER;
	sb_config->AzaliaPinCfg = AZALIA_PIN_CONFIG;
	sb_config->AzaliaSdin0 = AZALIA_SDIN_PIN_0;
	sb_config->AzaliaSdin1 = AZALIA_SDIN_PIN_1;
	sb_config->AzaliaSdin2 = AZALIA_SDIN_PIN_2;
	sb_config->AzaliaSdin3 = AZALIA_SDIN_PIN_3;
	sb_config->pAzaliaOemCodecTablePtr = NULL;

#ifndef __PRE_RAM__
	/* ramstage cimx config here */
	if (!sb_config->StdHeader.pCallBack) {
		sb_config->StdHeader.pCallBack = sb700_callout_entry;
	}

	//sb_config->
#endif //!__PRE_RAM__
	printk(BIOS_DEBUG, "SB700 - Cfg.c - sb700_cimx_config - End.\n");
}

