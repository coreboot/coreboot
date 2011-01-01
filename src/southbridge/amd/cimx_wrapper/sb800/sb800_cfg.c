/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include "SBPLATFORM.h"
#include "sb800_cfg.h"


/**
 * @brief South Bridge CIMx configuration
 *
 * should be called before exeucte CIMx function.
 * this function will be called in romstage and ramstage.
 */
void sb800_cimx_config(AMDSBCFG *sb_config)
{
	if (!sb_config) {
		return;
	}
	//memset(sb_config, 0, sizeof(AMDSBCFG));

	/* header */
	sb_config->StdHeader.PcieBasePtr = PCIEX_BASE_ADDRESS;

	/* static Build Parameters */
	sb_config->BuildParameters.BiosSize = BIOS_SIZE;
	sb_config->BuildParameters.LegacyFree = LEGACY_FREE;
	sb_config->BuildParameters.WatchDogTimerBase = WATCHDOG_TIMER_BASE_ADDRESS;
	sb_config->BuildParameters.AcpiGpe0BlkAddr = GPE0_BLK_ADDRESS;
	sb_config->BuildParameters.CpuControlBlkAddr = CPU_CNT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPmTmrBlkAddr = PM1_TMR_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPm1CntBlkAddr = PM1_CNT_BLK_ADDRESS;
	sb_config->BuildParameters.AcpiPm1EvtBlkAddr = PM1_EVT_BLK_ADDRESS;
	sb_config->BuildParameters.SioPmeBaseAddress = SIO_PME_BASE_ADDRESS;
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

	/* General */
	sb_config->SpreadSpectrum = SPREAD_SPECTRUM;
	sb_config->PciClks = PCI_CLOCK_CTRL;
	sb_config->HpetTimer = HPET_TIMER;

	/* USB */
	sb_config->USBMODE.UsbModeReg = USB_CINFIG;
  	sb_config->SbUsbPll = 0;

	/* SATA */
	sb_config->SataClass = SATA_MODE;
	sb_config->SataIdeMode = SATA_IDE_MODE;
	sb_config->SataPortMultCap = SATA_PORT_MULT_CAP_RESERVED;
  	sb_config->SATAMODE.SataMode.SataController = SATA_CONTROLLER;
	sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary, 1 -IDE as secondary.
								//TODO: set to secondary not take effect.
	sb_config->SATAMODE.SataMode.SataIdeCombinedMode = 0; //IDE controlor exposed and combined mode enabled
	sb_config->SATAMODE.SataMode.SATARefClkSel = SATA_CLOCK_SOURCE;

  	/* Azalia HDA */
	sb_config->AzaliaController = AZALIA_CONTROLLER;
	sb_config->AzaliaPinCfg = AZALIA_PIN_CONFIG;
	sb_config->AZALIACONFIG.AzaliaSdinPin = AZALIA_SDIN_PIN;
	sb_config->AZOEMTBL.pAzaliaOemCodecTablePtr = NULL;

	/*
	 * GPP. default configure only enable port0 with 4 lanes,
	 * configure in devicetree.cb would overwrite the default configuration
	 */
	sb_config->GppFunctionEnable = GPP_CONTROLLER;
	sb_config->GppLinkConfig = GPP_CFGMODE;
	//sb_config->PORTCONFIG[0].PortCfg.PortHotPlug = TRUE;
	sb_config->PORTCONFIG[0].PortCfg.PortPresent = ENABLED;
	sb_config->PORTCONFIG[1].PortCfg.PortPresent = ENABLED;
	sb_config->PORTCONFIG[2].PortCfg.PortPresent = ENABLED;
	sb_config->PORTCONFIG[3].PortCfg.PortPresent = ENABLED;
	sb_config->GppUnhidePorts = TRUE; //visable always, even port empty
	//sb_config->NbSbGen2 = TRUE;
	//sb_config->GppGen2 = TRUE;

	//cimx BTS fix
	sb_config->GppMemWrImprove = TRUE;
	sb_config->SbPcieOrderRule = TRUE;
	sb_config->AlinkPhyPllPowerDown = TRUE;
	sb_config->GppPhyPllPowerDown = TRUE; //GPP power saving
	sb_config->SBGecPwr = 0x03;//11b << 5, rpr BDF: 00:20:06, PLATFORM.H default define 0x11 was wrong
	sb_config->GecConfig = 0; //ENABLE GEC controller

#ifndef __PRE_RAM__
	/* ramstage cimx config here */
	if (!sb_config->StdHeader.CALLBACK.CalloutPtr) {
		sb_config->StdHeader.CALLBACK.CalloutPtr = sb800_callout_entry;
	}

	//sb_config->
#endif //!__PRE_RAM__
}

