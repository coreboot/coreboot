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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <string.h>
#include "SbPlatform.h"
#include "platform_cfg.h"


/**
 * @brief South Bridge CIMx configuration
 *
 * should be called before exeucte CIMx function.
 * this function will be called in romstage and ramstage.
 */
void sb900_cimx_config(AMDSBCFG *sb_config)
{
	if (!sb_config) {
        printk(BIOS_INFO, "SB900 - Cfg.c - sb900_cimx_config - No sb_config.\n");
		return;
	}
    printk(BIOS_INFO, "SB900 - Cfg.c - sb900_cimx_config - Start.\n");
	memset(sb_config, 0, sizeof(AMDSBCFG));

	/* static Build Parameters */
	sb_config->BuildParameters.BiosSize				= BIOS_SIZE;
	sb_config->BuildParameters.LegacyFree			= LEGACY_FREE;
	sb_config->BuildParameters.WatchDogTimerBase	= WATCHDOG_TIMER_BASE_ADDRESS;	// Board Level
	sb_config->BuildParameters.AcpiGpe0BlkAddr		= GPE0_BLK_ADDRESS;				// Board Level
	sb_config->BuildParameters.CpuControlBlkAddr	= CPU_CNT_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.AcpiPmTmrBlkAddr		= PM1_TMR_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.AcpiPm1CntBlkAddr	= PM1_CNT_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.AcpiPm1EvtBlkAddr	= PM1_EVT_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.SioPmeBaseAddress	= SIO_PME_BASE_ADDRESS;			// Board Level
	sb_config->BuildParameters.SpiRomBaseAddress	= SPI_BASE_ADDRESS;				// Board Level
	sb_config->BuildParameters.Smbus0BaseAddress	= SMBUS0_BASE_ADDRESS;			// Board Level
	sb_config->BuildParameters.Smbus1BaseAddress	= SMBUS1_BASE_ADDRESS;			// Board Level

	/* Turn on CDROM and HDD Power */
	sb_config->SATAMODE.SataMode.SataClkMode		= SATA_CLK_RESERVED;

	// header
	sb_config->StdHeader.PcieBasePtr				= PCIEX_BASE_ADDRESS;

	// Build Parameters
	sb_config->BuildParameters.ImcEnableOverWrite	= IMC_ENABLE_OVER_WRITE;		// Internal Option
	sb_config->BuildParameters.UsbMsi				= USB_MSI;						// Internal Option
	sb_config->BuildParameters.HdAudioMsi			= HDAUDIO_MSI;					// Internal Option
	sb_config->BuildParameters.LpcMsi				= LPC_MSI;						// Internal Option
	sb_config->BuildParameters.PcibMsi				= PCIB_MSI;						// Internal Option
	sb_config->BuildParameters.AbMsi				= AB_MSI;						// Internal Option
	sb_config->BuildParameters.GecShadowRomBase		= GEC_SHADOWROM_BASE;			// Board Level
	sb_config->BuildParameters.HpetBase				= HPET_BASE_ADDRESS;			// Board Level
	sb_config->BuildParameters.SataIDESsid			= SATA_IDE_MODE_SSID;			// Board Level
	sb_config->BuildParameters.SataRAIDSsid			= SATA_RAID_MODE_SSID;			// Board Level
	sb_config->BuildParameters.SataRAID5Ssid		= SATA_RAID5_MODE_SSID;			// Board Level
	sb_config->BuildParameters.SataAHCISsid			= SATA_AHCI_SSID;				// Board Level
	sb_config->BuildParameters.OhciSsid				= OHCI_SSID;					// Board Level
	sb_config->BuildParameters.EhciSsid				= EHCI_SSID;					// Board Level
	sb_config->BuildParameters.Ohci4Ssid			= OHCI4_SSID;					// Board Level
	sb_config->BuildParameters.SmbusSsid			= SMBUS_SSID;					// Board Level
	sb_config->BuildParameters.IdeSsid				= IDE_SSID;						// Board Level
	sb_config->BuildParameters.AzaliaSsid			= AZALIA_SSID;					// Board Level
	sb_config->BuildParameters.LpcSsid				= LPC_SSID;						// Board Level
	// sb_config->BuildParameters.PCIBSsid				= PCIB_SSID;					// Field Retired

	//
	// Common Function
	//
	sb_config->SATAMODE.SataMode.SataController		= SATA_CONTROLLER;				// External Option
	sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt	= SATA_IDE_COMBMD_PRISEC_OPT;	// External Option
	sb_config->SATAMODE.SataMode.SataIdeCombinedMode	= SATA_IDECOMBINED_MODE;	// External Option
	sb_config->S3Resume								= 0;							// CIMx Internal Used
	sb_config->SpreadSpectrum						= INCHIP_SPREAD_SPECTRUM;		// Board Level
	sb_config->NbSbGen2								= INCHIP_NB_SB_GEN2;			// External Option
	sb_config->GppGen2								= INCHIP_GPP_GEN2;				// External Option
	sb_config->GppMemWrImprove						= INCHIP_GPP_MEMORY_WRITE_IMPROVE;	// Internal Option
	sb_config->S4Resume								= 0;							// CIMx Internal Used
	sb_config->SataClass							= CONFIG_SATA_CONTROLLER_MODE;	// INCHIP_SATA_MODE	// External Option
	sb_config->SataIdeMode							= INCHIP_IDE_MODE;				// External Option
	sb_config->sdConfig								= SB_SD_CONFIG;					// External Option
	sb_config->sdSpeed								= SB_SD_SPEED;					// Internal Option
	sb_config->sdBitwidth							= SB_SD_BITWIDTH;				// Internal Option
	sb_config->SataDisUnusedIdePChannel				= SATA_DISUNUSED_IDE_P_CHANNEL;	// External Option
	sb_config->SataDisUnusedIdeSChannel				= SATA_DISUNUSED_IDE_S_CHANNEL;	// External Option
	sb_config->IdeDisUnusedIdePChannel				= IDE_DISUNUSED_IDE_P_CHANNEL;	// External Option
	sb_config->IdeDisUnusedIdeSChannel				= IDE_DISUNUSED_IDE_S_CHANNEL;	// External Option
	sb_config->SATAESPPORT.SataEspPort.PORT0		= SATA_ESP_PORT0;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT1		= SATA_ESP_PORT1;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT2		= SATA_ESP_PORT2;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT3		= SATA_ESP_PORT3;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT4		= SATA_ESP_PORT4;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT5		= SATA_ESP_PORT5;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT6		= SATA_ESP_PORT6;				// Board Level
	sb_config->SATAESPPORT.SataEspPort.PORT7		= SATA_ESP_PORT7;				// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT0	= SATA_PORT_POWER_PORT0;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT1	= SATA_PORT_POWER_PORT1;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT2	= SATA_PORT_POWER_PORT2;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT3	= SATA_PORT_POWER_PORT3;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT4	= SATA_PORT_POWER_PORT4;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT5	= SATA_PORT_POWER_PORT5;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT6	= SATA_PORT_POWER_PORT6;		// Board Level
	sb_config->SATAPORTPOWER.SataPortPower.PORT7	= SATA_PORT_POWER_PORT7;		// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT0		= SATA_PORTMODE_PORT0;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT1		= SATA_PORTMODE_PORT1;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT2		= SATA_PORTMODE_PORT2;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT3		= SATA_PORTMODE_PORT3;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT4		= SATA_PORTMODE_PORT4;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT5		= SATA_PORTMODE_PORT5;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT6		= SATA_PORTMODE_PORT6;			// Board Level
	sb_config->SATAPORTMODE.SataPortMd.PORT7		= SATA_PORTMODE_PORT7;			// Board Level
	sb_config->SataAggrLinkPmCap					= INCHIP_SATA_AGGR_LINK_PM_CAP;	// Internal Option
	sb_config->SataPortMultCap						= INCHIP_SATA_PORT_MULT_CAP;	// Internal Option
	sb_config->SataClkAutoOff						= INCHIP_SATA_CLK_AUTO_OFF;		// External Option
	sb_config->SataPscCap							= INCHIP_SATA_PSC_CAP;			// External Option
	sb_config->SataFisBasedSwitching				= INCHIP_SATA_FIS_BASE_SW;		// External Option
	sb_config->SataCccSupport						= INCHIP_SATA_CCC_SUPPORT;		// External Option
	sb_config->SataSscCap							= INCHIP_SATA_SSC_CAP;			// External Option
	sb_config->SataMsiCapability					= INCHIP_SATA_MSI_CAP;			// Internal Option
	sb_config->SataForceRaid						= INCHIP_SATA_FORCE_RAID5;		// Internal Option
	sb_config->SataTargetSupport8Device				= CIMXSB_SATA_TARGET_8DEVICE_CAP;	// External Option
	sb_config->SataDisableGenericMode				= SATA_DISABLE_GENERIC_MODE_CAP;// External Option
	sb_config->SataAhciEnclosureManagement			= SATA_AHCI_ENCLOSURE_CAP;		// Internal Option
	sb_config->SataSgpio0							= SATA_GPIO_0_CAP;				// External Option
	sb_config->SataSgpio1							= SATA_GPIO_1_CAP;				// External Option
	sb_config->SataPhyPllShutDown					= SATA_PHY_PLL_SHUTDOWN;		// External Option
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT0	= SATA_HOTREMOVEL_ENH_PORT0;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT1	= SATA_HOTREMOVEL_ENH_PORT1;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT2	= SATA_HOTREMOVEL_ENH_PORT2;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT3	= SATA_HOTREMOVEL_ENH_PORT3;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT4	= SATA_HOTREMOVEL_ENH_PORT4;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT5	= SATA_HOTREMOVEL_ENH_PORT5;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT6	= SATA_HOTREMOVEL_ENH_PORT6;	// Board Level
	sb_config->SATAHOTREMOVALENH.SataHotRemoveEnhPort.PORT7	= SATA_HOTREMOVEL_ENH_PORT7;	// Board Level
	// USB
	sb_config->USBMODE.UsbMode.Ohci1				= INCHIP_USB_OHCI1_CINFIG;		// External Option
	sb_config->USBMODE.UsbMode.Ehci1				= INCHIP_USB_EHCI1_CINFIG;		// Internal Option*
	sb_config->USBMODE.UsbMode.Ohci2				= INCHIP_USB_OHCI2_CINFIG;		// External Option
	sb_config->USBMODE.UsbMode.Ehci2				= INCHIP_USB_EHCI2_CINFIG;		// Internal Option*
	sb_config->USBMODE.UsbMode.Ohci3				= INCHIP_USB_OHCI3_CINFIG;		// External Option
	sb_config->USBMODE.UsbMode.Ehci3				= INCHIP_USB_EHCI3_CINFIG;		// Internal Option*
	sb_config->USBMODE.UsbMode.Ohci4				= INCHIP_USB_OHCI4_CINFIG;		// External Option
	// GEC
	sb_config->GecConfig							= INCHIP_GEC_CONTROLLER;		// External Option
	sb_config->IrConfig								= SB_IR_CONTROLLER;				// External Option
	sb_config->XhciSwitch							= SB_XHCI_SWITCH;				// External Option
	// Azalia
	sb_config->AzaliaController						= INCHIP_AZALIA_CONTROLLER;		// External Option
	sb_config->AzaliaPinCfg							= INCHIP_AZALIA_PIN_CONFIG;		// Board Level
	sb_config->FrontPanelDetected					= INCHIP_FRONT_PANEL_DETECTED;	// Board Level
	sb_config->AZALIACONFIG.AzaliaSdinPin			= AZALIA_PIN_CONFIG;			// Board Level
	sb_config->AZOEMTBL.pAzaliaOemCodecTablePtr		= NULL;							// Board Level
	sb_config->AZOEMFPTBL.pAzaliaOemFpCodecTablePtr	= NULL;							// Board Level
	sb_config->AnyHT200MhzLink						= INCHIP_ANY_HT_200MHZ_LINK;	// Internal Option
	sb_config->HpetTimer							= SB_HPET_TIMER;				// External Option
	sb_config->AzaliaSnoop							= INCHIP_AZALIA_SNOOP;			// Internal Option*
	// Generic
	sb_config->NativePcieSupport					= INCHIP_NATIVE_PCIE_SUPPOORT;	// External Option
	// USB
	sb_config->UsbPhyPowerDown						= INCHIP_USB_PHY_POWER_DOWN;	// External Option
	sb_config->PcibClkStopOverride					= INCHIP_PCIB_CLK_STOP_OVERRIDE;// Internal Option
	// sb_config->HpetMsiDis							= 0;								// Field Retired
	// sb_config->ResetCpuOnSyncFlood					= 0;								// Field Retired
	// sb_config->PcibAutoClkCtr						= 0;								// Field Retired
	sb_config->OEMPROGTBL.OemProgrammingTablePtr	= NULL;							// Board Level
	sb_config->PORTCONFIG[0].PortCfg.PortPresent	= SB_GPP_PORT0;					// Board Level
	sb_config->PORTCONFIG[0].PortCfg.PortDetected	= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[0].PortCfg.PortIsGen2		= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[0].PortCfg.PortHotPlug	= 0;							// CIMx Internal Used
	// sb_config->PORTCONFIG[0].PortCfg.PortIntxMap		= 0;								// Field Retired
	sb_config->PORTCONFIG[1].PortCfg.PortPresent	= SB_GPP_PORT1;					// Board Level
	sb_config->PORTCONFIG[1].PortCfg.PortDetected	= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[1].PortCfg.PortIsGen2		= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[1].PortCfg.PortHotPlug	= 0;							// CIMx Internal Used
	// sb_config->PORTCONFIG[0].PortCfg.PortIntxMap		= 0;								// Field Retired
	sb_config->PORTCONFIG[2].PortCfg.PortPresent	= SB_GPP_PORT2;					// Board Level
	sb_config->PORTCONFIG[2].PortCfg.PortDetected	= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[2].PortCfg.PortIsGen2		= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[2].PortCfg.PortHotPlug	= 0;							// CIMx Internal Used
	// sb_config->PORTCONFIG[0].PortCfg.PortIntxMap		= 0;								// Field Retired
	sb_config->PORTCONFIG[3].PortCfg.PortPresent	= SB_GPP_PORT3;					// Board Level
	sb_config->PORTCONFIG[3].PortCfg.PortDetected	= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[3].PortCfg.PortIsGen2		= 0;							// CIMx Internal Used
	sb_config->PORTCONFIG[3].PortCfg.PortHotPlug	= 0;							// CIMx Internal Used
	// sb_config->PORTCONFIG[0].PortCfg.PortIntxMap		= 0;								// Field Retired
	sb_config->GppLinkConfig						= INCHIP_GPP_LINK_CONFIG;		// External Option
	sb_config->GppFoundGfxDev						= 0;							// CIMx Internal Used
	sb_config->GppFunctionEnable					= SB_GPP_CONTROLLER;			// External Option
	sb_config->GppUnhidePorts						= INCHIP_GPP_UNHIDE_PORTS;		// Internal Option
	sb_config->GppPortAspm							= INCHIP_GPP_PORT_ASPM;			// Internal Option
	sb_config->GppLaneReversal						= INCHIP_GPP_LANEREVERSAL;		// External Option
	sb_config->AlinkPhyPllPowerDown					= INCHIP_ALINK_PHY_PLL_POWER_DOWN;	// External Option
	sb_config->GppPhyPllPowerDown					= INCHIP_GPP_PHY_PLL_POWER_DOWN;// External Option
	sb_config->GppDynamicPowerSaving				= INCHIP_GPP_DYNAMIC_POWER_SAVING;	// External Option
	sb_config->PcieAER								= INCHIP_PCIE_AER;				// External Option
	sb_config->PcieRAS								= INCHIP_PCIE_RAS;				// External Option
	sb_config->GppHardwareDowngrade					= INCHIP_GPP_HARDWARE_DOWNGRADE;// Internal Option
	sb_config->GppToggleReset						= INCHIP_GPP_TOGGLE_RESET;		// External Option
	sb_config->sdbEnable							= 0;							// CIMx Internal Used
	sb_config->TempMMIO								= NULL;							// CIMx Internal Used
	// sb_config->GecPhyStatus							= INCHIP_GEC_PHY_STATUS;		// Field Retired
	sb_config->SBGecPwr								= INCHIP_GEC_POWER_POLICY;		// Internal Option
	sb_config->SBGecDebugBus						= INCHIP_GEC_DEBUGBUS;			// Internal Option
	sb_config->SbPcieOrderRule						= INCHIP_SB_PCIE_ORDER_RULE;	// External Option
	sb_config->AcDcMsg								= INCHIP_ACDC_MSG;				// Internal Option
	sb_config->TimerTickTrack						= INCHIP_TIMER_TICK_TRACK;		// Internal Option
	sb_config->ClockInterruptTag					= INCHIP_CLOCK_INTERRUPT_TAG;	// Internal Option
	sb_config->OhciTrafficHanding					= INCHIP_OHCI_TRAFFIC_HANDING;	// Internal Option
	sb_config->EhciTrafficHanding					= INCHIP_EHCI_TRAFFIC_HANDING;	// Internal Option
	sb_config->FusionMsgCMultiCore					= INCHIP_FUSION_MSGC_MULTICORE;	// Internal Option
	sb_config->FusionMsgCStage						= INCHIP_FUSION_MSGC_STAGE;		// Internal Option
	sb_config->ALinkClkGateOff						= INCHIP_ALINK_CLK_GATE_OFF;	// External Option
	sb_config->BLinkClkGateOff						= INCHIP_BLINK_CLK_GATE_OFF;	// External Option
	// sb_config->sdb									= 0;								// Field Retired
	sb_config->GppGen2Strap							= 0;							// CIMx Internal Used
	sb_config->SlowSpeedABlinkClock					= INCHIP_SLOW_SPEED_ABLINK_CLOCK;	// Internal Option
	sb_config->DYNAMICGECROM.DynamicGecRomAddress_Ptr	= NULL;						// Board Level
	sb_config->AbClockGating						= INCHIP_AB_CLOCK_GATING;		// External Option
	sb_config->GppClockGating						= INCHIP_GPP_CLOCK_GATING;		// External Option
	sb_config->L1TimerOverwrite						= INCHIP_L1_TIMER_OVERWRITE;	// Internal Option
	// sb_config->UmiLinkWidth							= 0;								// Field Retired
	sb_config->UmiDynamicSpeedChange				= INCHIP_UMI_DYNAMIC_SPEED_CHANGE;	// Internal Option
	// sb_config->PcieRefClockOverclocking				= 0;								// Field Retired
	sb_config->SbAlinkGppTxDriverStrength			= INCHIP_ALINK_GPP_TX_DRV_STRENGTH;	// Internal Option
	sb_config->PwrFailShadow						= 0x02;							// Board Level
	sb_config->StressResetMode						= INCHIP_STRESS_RESET_MODE;		// Internal Option
	sb_config->hwm.fanSampleFreqDiv					= 0x03;							// Board Level
	sb_config->hwm.hwmSbtsiAutoPoll					= 1;							// Board Level

	/* General */
	sb_config->PciClks								= SB_PCI_CLOCK_RESERVED;
	sb_config->hwm.hwmEnable						= 0x0;

#ifndef __PRE_RAM__
	/* ramstage cimx config here */
	if (!sb_config->StdHeader.CALLBACK.CalloutPtr) {
		sb_config->StdHeader.CALLBACK.CalloutPtr = sb900_callout_entry;
	}

	//sb_config->
#endif //!__PRE_RAM__
    printk(BIOS_INFO, "SB900 - Cfg.c - sb900_cimx_config - End.\n");
}

void SbPowerOnInit_Config(AMDSBCFG *sb_config)
{
	if (!sb_config) {
        printk(BIOS_INFO, "SB900 - Cfg.c - SbPowerOnInit_Config - No sb_config.\n");
		return;
	}
    printk(BIOS_INFO, "SB900 - Cfg.c - SbPowerOnInit_Config - Start.\n");
	memset(sb_config, 0, sizeof(AMDSBCFG));

    // Set the build parameters
	sb_config->BuildParameters.BiosSize				= BIOS_SIZE;					// Field Retired
	sb_config->BuildParameters.LegacyFree			= SBCIMx_LEGACY_FREE;			// Board Level
	sb_config->BuildParameters.SpiSpeed				= SBCIMX_SPI_SPEED;				// Internal Option
	sb_config->BuildParameters.SpiFastSpeed			= SBCIMX_SPI_FASTSPEED;			// Internal Option
	// sb_config->BuildParameters.SpiWriteSpeed			=  0;							// Field Retired
	sb_config->BuildParameters.SpiMode				= SBCIMX_SPI_MODE;				// Internal Option
	sb_config->BuildParameters.SpiBurstWrite		= SBCIMX_SPI_BURST_WRITE;		// Internla Option
	sb_config->BuildParameters.EcKbd				= INCHIP_EC_KBD;				// Board Level
	sb_config->BuildParameters.Smbus0BaseAddress	= SMBUS0_BASE_ADDRESS;			// Board Level
	sb_config->BuildParameters.Smbus1BaseAddress	= SMBUS1_BASE_ADDRESS;			// Board Level
	sb_config->BuildParameters.SioPmeBaseAddress	= SIO_PME_BASE_ADDRESS;			// Board Level
	sb_config->BuildParameters.WatchDogTimerBase	= WATCHDOG_TIMER_BASE_ADDRESS;	// Board Level
	sb_config->BuildParameters.GecShadowRomBase		= GEC_ROM_SHADOW_ADDRESS;		// Board Level
	sb_config->BuildParameters.SpiRomBaseAddress	= SPI_BASE_ADDRESS;				// Board Level
	sb_config->BuildParameters.AcpiPm1EvtBlkAddr	= PM1_EVT_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.AcpiPm1CntBlkAddr	= PM1_CNT_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.AcpiPmTmrBlkAddr		= PM1_TMR_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.CpuControlBlkAddr	= CPU_CNT_BLK_ADDRESS;			// Board Level
	sb_config->BuildParameters.AcpiGpe0BlkAddr		= GPE0_BLK_ADDRESS;				// Board Level
	sb_config->BuildParameters.SmiCmdPortAddr		= SMI_CMD_PORT;					// Board Level
	sb_config->BuildParameters.AcpiPmaCntBlkAddr	= ACPI_PMA_CNT_BLK_ADDRESS;		// Board Level
    sb_config->SATAMODE.SataMode.SataController		= INCHIP_SATA_CONTROLLER;		// External Option
    sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt	= SATA_COMBINE_MODE_CHANNEL;// External Option
    sb_config->SATAMODE.SataMode.SataSetMaxGen2		= SATA_MAX_GEN2_MODE;			// External Option
    sb_config->SATAMODE.SataMode.SataIdeCombinedMode= SATA_COMBINE_MODE;			// External Option
    sb_config->SATAMODE.SataMode.SataClkMode		= SATA_CLK_RESERVED;			// Internal Option
    sb_config->NbSbGen2								= NB_SB_GEN2;					// External Option
    sb_config->SataInternal100Spread				= INCHIP_SATA_INTERNAL_100_SPREAD;	// External Option
    sb_config->OEMPROGTBL.OemProgrammingTablePtr	= NULL;							// Board Level
    sb_config->sdbEnable							= 0;							// CIMx Internal Used
    sb_config->Cg2Pll								= INCHIP_CG2_PLL;				// Internal Option

    printk(BIOS_INFO, "SB900 - Cfg.c - SbPowerOnInit_Config - End.\n");
}


