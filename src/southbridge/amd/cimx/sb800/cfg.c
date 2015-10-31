/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include "SBPLATFORM.h"
#include "cfg.h"
#include "OEM.h"
#include <cbmem.h>

#include <arch/io.h>
#include <arch/acpi.h>

/**
 * @brief South Bridge CIMx configuration
 *
 * should be called before executing CIMx functions.
 * this function will be called in romstage and ramstage.
 */
void sb800_cimx_config(AMDSBCFG *sb_config)
{
	uint16_t bios_size = BIOS_SIZE;
	if (!sb_config)
		return;

	sb_config->S3Resume = acpi_is_wakeup_s3();

	/* header */
	sb_config->StdHeader.PcieBasePtr = PCIEX_BASE_ADDRESS;

	/* static Build Parameters */
	sb_config->BuildParameters.BiosSize = bios_size;
	sb_config->BuildParameters.LegacyFree = LEGACY_FREE;
	sb_config->BuildParameters.WatchDogTimerBase = WATCHDOG_TIMER_BASE_ADDRESS;
	sb_config->BuildParameters.AcpiGpe0BlkAddr = GPE0_BLK_ADDRESS;
	sb_config->BuildParameters.CpuControlBlkAddr = CPU_CNT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPmTmrBlkAddr = PM1_TMR_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPm1CntBlkAddr = PM1_CNT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPm1EvtBlkAddr = PM1_EVT_BLK_ADDRESS;
	sb_config->BuildParameters.SioPmeBaseAddress = SIO_PME_BASE_ADDRESS;
	sb_config->BuildParameters.SioHwmBaseAddress = SIO_HWM_BASE_ADDRESS;
	sb_config->BuildParameters.SpiRomBaseAddress = SPI_BASE_ADDRESS;
	sb_config->BuildParameters.GecShadowRomBase = GEC_BASE_ADDRESS;
	sb_config->BuildParameters.Smbus0BaseAddress = SMBUS0_BASE_ADDRESS;
	sb_config->BuildParameters.Smbus1BaseAddress = SMBUS1_BASE_ADDRESS;
	sb_config->BuildParameters.SataIDESsid = SATA_IDE_MODE_SSID;
	sb_config->BuildParameters.SataRAIDSsid = SATA_RAID_MODE_SSID;
	sb_config->BuildParameters.SataRAID5Ssid = SATA_RAID5_MODE_SSID;
	sb_config->BuildParameters.SataAHCISsid = SATA_AHCI_SSID;
	sb_config->BuildParameters.OhciSsid = OHCI_SSID;
	sb_config->BuildParameters.EhciSsid = EHCI_SSID;
	sb_config->BuildParameters.Ohci4Ssid = OHCI4_SSID;
	sb_config->BuildParameters.SmbusSsid = SMBUS_SSID;
	sb_config->BuildParameters.IdeSsid = IDE_SSID;
	sb_config->BuildParameters.AzaliaSsid = AZALIA_SSID;
	sb_config->BuildParameters.LpcSsid = LPC_SSID;
	sb_config->BuildParameters.PCIBSsid = PCIB_SSID;
	sb_config->BuildParameters.SpreadSpectrumType = Spread_Spectrum_Type;
	sb_config->BuildParameters.HpetBase = HPET_BASE_ADDRESS;
	sb_config->BuildParameters.ImcEnableOverWrite = IMC_ENABLE_OVER_WRITE;

	/* General */
	sb_config->SpreadSpectrum = SPREAD_SPECTRUM;
	sb_config->PciClks = PCI_CLOCK_CTRL;
	sb_config->HpetTimer = HPET_TIMER;
	sb_config->SbSpiSpeedSupport = 1;

	/* USB */
	sb_config->USBMODE.UsbModeReg = USB_CONFIG;
	sb_config->SbUsbPll = 0;
	/* CG PLL multiplier for USB Rx 1.1 mode (0=disable, 1=enable) */
	sb_config->UsbRxMode = USB_RX_MODE;

	/* SATA */
	sb_config->SataClass = SATA_MODE;
	sb_config->SataIdeMode = SATA_IDE_MODE;
	sb_config->SataPortMultCap = SATA_PORT_MULT_CAP_RESERVED;
	sb_config->SATAMODE.SataMode.SataController = SATA_CONTROLLER;
	sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary, 1 -IDE as secondary.
								//TODO: set to secondary not take effect.
	sb_config->SATAMODE.SataMode.SataIdeCombinedMode = CONFIG_IDE_COMBINED_MODE;
	sb_config->SATAMODE.SataMode.SATARefClkSel = SATA_CLOCK_SOURCE;

	/* Azalia HDA */
	sb_config->AzaliaController = AZALIA_CONTROLLER;
	sb_config->AzaliaPinCfg = AZALIA_PIN_CONFIG;
	sb_config->AZALIACONFIG.AzaliaSdinPin = AZALIA_SDIN_PIN;
	/* Mainboard Specific Azalia Cocec Verb Table */
#ifdef AZALIA_OEM_VERB_TABLE
	sb_config->AZOEMTBL.pAzaliaOemCodecTablePtr = (CODECTBLLIST *)AZALIA_OEM_VERB_TABLE;
#else
	sb_config->AZOEMTBL.pAzaliaOemCodecTablePtr = NULL;
#endif
	/* LPC */
	/* SuperIO hardware monitor register access */
	sb_config->SioHwmPortEnable = CONFIG_SB_SUPERIO_HWM;

	/*
	 * GPP. default configure only enable port0 with 4 lanes,
	 * configure in devicetree.cb would overwrite the default configuration
	 */
	sb_config->GppFunctionEnable = GPP_CONTROLLER;
	sb_config->GppLinkConfig = GPP_CFGMODE;
	//sb_config->PORTCONFIG[0].PortCfg.PortHotPlug = TRUE;
	sb_config->PORTCONFIG[0].PortCfg.PortPresent = CIMX_OPTION_ENABLED;
	sb_config->PORTCONFIG[1].PortCfg.PortPresent = CIMX_OPTION_ENABLED;
	sb_config->PORTCONFIG[2].PortCfg.PortPresent = CIMX_OPTION_ENABLED;
	sb_config->PORTCONFIG[3].PortCfg.PortPresent = CIMX_OPTION_ENABLED;
	sb_config->GppUnhidePorts = SB_GPP_UNHIDE_PORTS;
	sb_config->NbSbGen2 = NB_SB_GEN2;
	sb_config->GppGen2 = SB_GPP_GEN2;

	//cimx BTS fix
	sb_config->GppMemWrImprove = TRUE;
	sb_config->SbPcieOrderRule = TRUE;
	sb_config->AlinkPhyPllPowerDown = TRUE;
	sb_config->GppPhyPllPowerDown = TRUE; //GPP power saving
	sb_config->SBGecPwr = 0x03;//11b << 5, rpr BDF: 00:20:06
	sb_config->GecConfig = GEC_CONFIG;
}
