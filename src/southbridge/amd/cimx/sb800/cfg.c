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

#include <console/console.h>
#include "SBPLATFORM.h"
#include "cfg.h"
#include "OEM.h"
#include <cbmem.h>

#include <arch/io.h>
#include <arch/acpi.h>

#if CONFIG_HAVE_ACPI_RESUME
int acpi_get_sleep_type(void)
{
	u16 tmp = inw(PM1_CNT_BLK_ADDRESS);
	tmp = ((tmp & (7 << 10)) >> 10);
	printk(BIOS_DEBUG, "SLP_TYP type was %x\n", tmp);
	return (int)tmp;
}
#endif

#ifndef __PRE_RAM__
void backup_top_of_ram(uint64_t ramtop)
{
	u32 dword = (u32) ramtop;
	int nvram_pos = 0xf8, i; /* temp */
	printk(BIOS_DEBUG, "dword=%x\n", dword);
	for (i = 0; i<4; i++) {
		printk(BIOS_DEBUG, "nvram_pos=%x, dword>>(8*i)=%x\n", nvram_pos, (dword >>(8 * i)) & 0xff);
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >>(8 * i)) & 0xff , BIOSRAM_DATA);
		nvram_pos++;
	}
}
#endif

#if CONFIG_HAVE_ACPI_RESUME
unsigned long get_top_of_ram(void)
{
	u32 xdata = 0;
	int xnvram_pos = 0xf8, xi;
	if (acpi_get_sleep_type() != 3)
		return 0;
	for (xi = 0; xi<4; xi++) {
		outb(xnvram_pos, BIOSRAM_INDEX);
		xdata &= ~(0xff << (xi * 8));
		xdata |= inb(BIOSRAM_DATA) << (xi *8);
		xnvram_pos++;
	}
	return (unsigned long) xdata;
}
#endif

/**
 * @brief South Bridge CIMx configuration
 *
 * should be called before exeucte CIMx function.
 * this function will be called in romstage and ramstage.
 */
void sb800_cimx_config(AMDSBCFG *sb_config)
{
	if (!sb_config)
		return;

#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_get_sleep_type() == 3)
		sb_config->S3Resume = 1;
#endif

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

	/* USB */
	sb_config->USBMODE.UsbModeReg = USB_CONFIG;
	sb_config->SbUsbPll = 0;

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

#ifndef __PRE_RAM__
	/* ramstage cimx config here */
	if (!sb_config->StdHeader.CALLBACK.CalloutPtr) {
		sb_config->StdHeader.CALLBACK.CalloutPtr = sb800_callout_entry;
	}
#endif //!__PRE_RAM__
}
