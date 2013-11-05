/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Advanced Micro Devices, Inc.
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

#include "FchPlatform.h"
#include "Fch.h"
#include <cpu/amd/agesa/s3_resume.h>
#include <device/device.h>
#include "hudson.h"
#include "AGESA.h"

extern FCH_DATA_BLOCK InitEnvCfgDefault;
extern FCH_INTERFACE FchInterfaceDefault;
extern FCH_RESET_DATA_BLOCK  InitResetCfgDefault;
extern FCH_RESET_INTERFACE FchResetInterfaceDefault;

#define DUMP_FCH_SETTING 0

void s3_resume_init_data(void *data)
{
	FCH_DATA_BLOCK *FchParams = (FCH_DATA_BLOCK *)data;
	AMD_CONFIG_PARAMS *StdHeader = FchParams->StdHeader;

	*FchParams = InitEnvCfgDefault;
	FchParams->StdHeader = StdHeader;

	FchParams->Usb.Xhci0Enable              = InitResetCfgDefault.FchReset.Xhci0Enable;
	FchParams->Usb.Xhci1Enable              = InitResetCfgDefault.FchReset.Xhci1Enable;
	FchParams->Spi.SpiFastSpeed             = InitResetCfgDefault.FastSpeed;
	FchParams->Spi.WriteSpeed               = InitResetCfgDefault.WriteSpeed;
	FchParams->Spi.SpiMode                  = InitResetCfgDefault.Mode;
	FchParams->Spi.AutoMode                 = InitResetCfgDefault.AutoMode;
	FchParams->Spi.SpiBurstWrite            = InitResetCfgDefault.BurstWrite;
	FchParams->Sata.SataMode.Sata6AhciCap   = (UINT8) InitResetCfgDefault.Sata6AhciCap;
	FchParams->Misc.Cg2Pll                  = InitResetCfgDefault.Cg2Pll;
	FchParams->Sata.SataMode.SataSetMaxGen2 = InitResetCfgDefault.SataSetMaxGen2;
	FchParams->Sata.SataMode.SataClkMode    = InitResetCfgDefault.SataClkMode;
	FchParams->Sata.SataMode.SataModeReg    = InitResetCfgDefault.SataModeReg;
	FchParams->Sata.SataInternal100Spread   = (UINT8) InitResetCfgDefault.SataInternal100Spread;
	FchParams->Spi.SpiSpeed                 = InitResetCfgDefault.SpiSpeed;
	FchParams->Gpp                          = InitResetCfgDefault.Gpp;
	FchParams->Gpp.GppFunctionEnable        = FchResetInterfaceDefault.GppEnable;

	FchParams->Gpp.GppLinkConfig           = UserOptions.FchBldCfg->CfgFchGppLinkConfig;
	FchParams->Gpp.PortCfg[0].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort0Present;
	FchParams->Gpp.PortCfg[1].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort1Present;
	FchParams->Gpp.PortCfg[2].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort2Present;
	FchParams->Gpp.PortCfg[3].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort3Present;
	FchParams->Gpp.PortCfg[0].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort0HotPlug;
	FchParams->Gpp.PortCfg[1].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort1HotPlug;
	FchParams->Gpp.PortCfg[2].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort2HotPlug;
	FchParams->Gpp.PortCfg[3].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort3HotPlug;

	FchParams->HwAcpi.Smbus0BaseAddress    = UserOptions.FchBldCfg->CfgSmbus0BaseAddress;
	FchParams->HwAcpi.Smbus1BaseAddress    = UserOptions.FchBldCfg->CfgSmbus1BaseAddress;
	FchParams->HwAcpi.SioPmeBaseAddress    = UserOptions.FchBldCfg->CfgSioPmeBaseAddress;
	FchParams->HwAcpi.AcpiPm1EvtBlkAddr    = UserOptions.FchBldCfg->CfgAcpiPm1EvtBlkAddr;
	FchParams->HwAcpi.AcpiPm1CntBlkAddr    = UserOptions.FchBldCfg->CfgAcpiPm1CntBlkAddr;
	FchParams->HwAcpi.AcpiPmTmrBlkAddr     = UserOptions.FchBldCfg->CfgAcpiPmTmrBlkAddr;
	FchParams->HwAcpi.CpuControlBlkAddr    = UserOptions.FchBldCfg->CfgCpuControlBlkAddr;
	FchParams->HwAcpi.AcpiGpe0BlkAddr      = UserOptions.FchBldCfg->CfgAcpiGpe0BlkAddr;
	FchParams->HwAcpi.SmiCmdPortAddr       = UserOptions.FchBldCfg->CfgSmiCmdPortAddr;
	FchParams->HwAcpi.AcpiPmaCntBlkAddr    = UserOptions.FchBldCfg->CfgAcpiPmaCntBlkAddr;
	FchParams->HwAcpi.WatchDogTimerBase    = UserOptions.FchBldCfg->CfgWatchDogTimerBase;
	FchParams->Sata.SataRaid5Ssid          = UserOptions.FchBldCfg->CfgSataRaid5Ssid;
	FchParams->Sata.SataRaidSsid           = UserOptions.FchBldCfg->CfgSataRaidSsid;
	FchParams->Sata.SataAhciSsid           = UserOptions.FchBldCfg->CfgSataAhciSsid;
	FchParams->Sata.SataIdeSsid            = UserOptions.FchBldCfg->CfgSataIdeSsid;
	FchParams->Spi.RomBaseAddress          = UserOptions.FchBldCfg->CfgSpiRomBaseAddress;
	FchParams->Sd.SdSsid                   = UserOptions.FchBldCfg->CfgSdSsid;
	FchParams->Spi.LpcSsid                 = UserOptions.FchBldCfg->CfgLpcSsid;
	FchParams->Hpet.HpetBase               = UserOptions.FchBldCfg->CfgHpetBaseAddress;
	FchParams->Azalia.AzaliaSsid           = UserOptions.FchBldCfg->CfgAzaliaSsid;
	FchParams->Smbus.SmbusSsid             = UserOptions.FchBldCfg->CfgSmbusSsid;
	FchParams->Ide.IdeSsid                 = UserOptions.FchBldCfg->CfgIdeSsid;
	FchParams->Usb.EhciSsid                = UserOptions.FchBldCfg->CfgEhciSsid;
	FchParams->Usb.OhciSsid                = UserOptions.FchBldCfg->CfgOhciSsid;
	FchParams->Usb.XhciSsid                = UserOptions.FchBldCfg->CfgXhciSsid;
	FchParams->Ir.IrPinControl             = UserOptions.FchBldCfg->CfgFchIrPinControl;
	FchParams->Sd.SdClockControl           = UserOptions.FchBldCfg->CfgFchSdClockControl;

	FchParams->Sd.SdConfig                 = FchInterfaceDefault.SdConfig;
	FchParams->Azalia.AzaliaEnable         = FchInterfaceDefault.AzaliaController;
	FchParams->Ir.IrConfig                 = FchInterfaceDefault.IrConfig;
	FchParams->Ab.NbSbGen2                 = FchInterfaceDefault.UmiGen2;
	FchParams->Sata.SataClass              = FchInterfaceDefault.SataClass;
	FchParams->Sata.SataMode.SataEnable    = FchInterfaceDefault.SataEnable;
	FchParams->Sata.SataMode.IdeEnable     = FchInterfaceDefault.IdeEnable;
	FchParams->Sata.SataIdeMode            = FchInterfaceDefault.SataIdeMode;
	FchParams->Usb.Ohci1Enable             = FchInterfaceDefault.Ohci1Enable;
	FchParams->Usb.Ehci1Enable             = FchInterfaceDefault.Ohci1Enable;
	FchParams->Usb.Ohci2Enable             = FchInterfaceDefault.Ohci2Enable;
	FchParams->Usb.Ehci2Enable             = FchInterfaceDefault.Ohci2Enable;
	FchParams->Usb.Ohci3Enable             = FchInterfaceDefault.Ohci3Enable;
	FchParams->Usb.Ehci3Enable             = FchInterfaceDefault.Ohci3Enable;
	FchParams->Usb.Ohci4Enable             = FchInterfaceDefault.Ohci4Enable;
	FchParams->HwAcpi.PwrFailShadow        = FchInterfaceDefault.FchPowerFail;

#if !CONFIG_HUDSON_XHCI_ENABLE
	FchParams->Usb.Xhci0Enable = FALSE;
#endif
	FchParams->Usb.Xhci1Enable = FALSE;

#if DUMP_FCH_SETTING
	int i;

	for (i = 0; i < sizeof(FchParams); i++) {
		printk(BIOS_DEBUG, " %02x", ((u8 *) FchParams)[i]);
		if ((i % 16) == 15)
			printk(BIOS_DEBUG, "\n");
	}
#endif
}
