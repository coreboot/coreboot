/* $NoKeywords:$ */
/**
 * @file
 *
 * Fch Init during POWER-ON
 *
 * Prepare Fch environment during power on stage.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 49753 $   @e \$Date: 2011-03-29 04:51:46 +0800 (Tue, 29 Mar 2011) $
 *
 */
/*;********************************************************************************
;
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
;*********************************************************************************/

#include    "FchPlatform.h"
#include    "Ids.h"
#include    "heapManager.h"
#include    "Filecode.h"
#define FILECODE PROC_FCH_INTERFACE_INITENVDEF_FILECODE

extern FCH_DATA_BLOCK InitEnvCfgDefault;

FCH_DATA_BLOCK*
FchInitLoadDataBlock (
  IN       FCH_INTERFACE        *FchInterface,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  FCH_DATA_BLOCK            *FchParams;
  LOCATE_HEAP_PTR           LocHeapPtr;
  AMD_CONFIG_PARAMS         TempStdHeader;
  AGESA_STATUS              AgesaStatus;

  TempStdHeader = *StdHeader;
  TempStdHeader.HeapStatus = HEAP_SYSTEM_MEM;

  // Locate the internal data block via heap manager
  LocHeapPtr.BufferHandle = AMD_FCH_DATA_BLOCK_HANDLE;
  AgesaStatus = HeapLocateBuffer (&LocHeapPtr, &TempStdHeader);
  ASSERT (!AgesaStatus);

  FchParams = (FCH_DATA_BLOCK *) LocHeapPtr.BufferPtr;
  ASSERT (FchParams != NULL);
  FchParams->StdHeader = StdHeader;
  return FchParams;
}


FCH_DATA_BLOCK*
FchInitEnvCreatePrivateData (
  IN       AMD_ENV_PARAMS        *EnvParams
  )
{
  FCH_DATA_BLOCK            *FchParams;
  ALLOCATE_HEAP_PARAMS      AllocHeapParams;
  AMD_CONFIG_PARAMS         TempStdHeader;
  AGESA_STATUS              AgesaStatus;

  TempStdHeader = EnvParams->StdHeader;
  TempStdHeader.HeapStatus = HEAP_SYSTEM_MEM;

  // First allocate internal data block via heap manager
  AllocHeapParams.RequestedBufferSize = sizeof (FCH_DATA_BLOCK);
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  AllocHeapParams.BufferHandle = AMD_FCH_DATA_BLOCK_HANDLE;
  AgesaStatus = HeapAllocateBuffer (&AllocHeapParams, &TempStdHeader);
  ASSERT (!AgesaStatus);

  FchParams = (FCH_DATA_BLOCK *) AllocHeapParams.BufferPtr;
  ASSERT (FchParams != NULL);
  IDS_HDT_CONSOLE (FCH_TRACE, "    FCH Data Block Allocation: [0x%x], Ptr = 0x%08x\n", AgesaStatus, FchParams);

  // Load private data block with default
  LibAmdMemCopy (
  (UINT8 *) FchParams,
  (UINT8 *) &InitEnvCfgDefault,
  sizeof (FCH_DATA_BLOCK),
  &EnvParams->StdHeader
  );

  // Update with external parameters
  FchParams->StdHeader                   = &EnvParams->StdHeader;

  FchParams->Sd.SdConfig                 = EnvParams->FchInterface.SdConfig;
  FchParams->Azalia.AzaliaEnable         = EnvParams->FchInterface.AzaliaController;
  FchParams->Ir.IrConfig                 = EnvParams->FchInterface.IrConfig;
  FchParams->Ab.NbSbGen2                 = EnvParams->FchInterface.UmiGen2;
  FchParams->Sata.SataClass              = EnvParams->FchInterface.SataClass;
  FchParams->Sata.SataMode.SataEnable    = EnvParams->FchInterface.SataEnable;
  FchParams->Sata.SataMode.IdeEnable     = EnvParams->FchInterface.IdeEnable;
  FchParams->Sata.SataIdeMode            = EnvParams->FchInterface.SataIdeMode;
  FchParams->Usb.Ohci1Enable             = EnvParams->FchInterface.Ohci1Enable;
  FchParams->Usb.Ehci1Enable             = EnvParams->FchInterface.Ohci1Enable;
  FchParams->Usb.Ohci2Enable             = EnvParams->FchInterface.Ohci2Enable;
  FchParams->Usb.Ehci2Enable             = EnvParams->FchInterface.Ohci2Enable;
  FchParams->Usb.Ohci3Enable             = EnvParams->FchInterface.Ohci3Enable;
  FchParams->Usb.Ehci3Enable             = EnvParams->FchInterface.Ohci3Enable;
  FchParams->Usb.Ohci4Enable             = EnvParams->FchInterface.Ohci4Enable;
  FchParams->Usb.Xhci0Enable             = EnvParams->FchInterface.XhciSwitch;
  FchParams->Usb.Xhci1Enable             = EnvParams->FchInterface.XhciSwitch;
  FchParams->Gpp.GppFunctionEnable       = EnvParams->FchInterface.GppEnable;
  FchParams->HwAcpi.PwrFailShadow        = EnvParams->FchInterface.FchPowerFail;

  FchParams->HwAcpi.Smbus0BaseAddress    = UserOptions.CfgSmbus0BaseAddress;
  FchParams->HwAcpi.Smbus1BaseAddress    = UserOptions.CfgSmbus1BaseAddress;
  FchParams->HwAcpi.SioPmeBaseAddress    = UserOptions.CfgSioPmeBaseAddress;
  FchParams->HwAcpi.AcpiPm1EvtBlkAddr    = UserOptions.CfgAcpiPm1EvtBlkAddr;
  FchParams->HwAcpi.AcpiPm1CntBlkAddr    = UserOptions.CfgAcpiPm1CntBlkAddr;
  FchParams->HwAcpi.AcpiPmTmrBlkAddr     = UserOptions.CfgAcpiPmTmrBlkAddr;
  FchParams->HwAcpi.CpuControlBlkAddr    = UserOptions.CfgCpuControlBlkAddr;
  FchParams->HwAcpi.AcpiGpe0BlkAddr      = UserOptions.CfgAcpiGpe0BlkAddr;
  FchParams->HwAcpi.SmiCmdPortAddr       = UserOptions.CfgSmiCmdPortAddr;
  FchParams->HwAcpi.AcpiPmaCntBlkAddr    = UserOptions.CfgAcpiPmaCntBlkAddr;
  FchParams->HwAcpi.WatchDogTimerBase    = UserOptions.CfgWatchDogTimerBase;
  FchParams->Sata.SataRaid5Ssid          = UserOptions.CfgSataRaid5Ssid;
  FchParams->Sata.SataRaidSsid           = UserOptions.CfgSataRaidSsid;
  FchParams->Sata.SataAhciSsid           = UserOptions.CfgSataAhciSsid;
  FchParams->Sata.SataIdeSsid            = UserOptions.CfgSataIdeSsid;
  FchParams->Gec.GecShadowRomBase        = UserOptions.CfgGecShadowRomBase;
  FchParams->Spi.RomBaseAddress          = UserOptions.CfgSpiRomBaseAddress;
  FchParams->Spi.LpcSsid                 = UserOptions.CfgLpcSsid;
  FchParams->Hpet.HpetBase               = UserOptions.CfgHpetBaseAddress;
  FchParams->Azalia.AzaliaSsid           = UserOptions.CfgAzaliaSsid;
  FchParams->Smbus.SmbusSsid             = UserOptions.CfgSmbusSsid;
  FchParams->Ide.IdeSsid                 = UserOptions.CfgIdeSsid;
  FchParams->Usb.EhciSsid                = UserOptions.CfgEhciSsid;
  FchParams->Usb.OhciSsid                = UserOptions.CfgOhciSsid;
  FchParams->Gpp.GppLinkConfig           = UserOptions.CfgFchGppLinkConfig;
  FchParams->Gpp.PortCfg[0].PortPresent  = UserOptions.CfgFchGppPort0Present;
  FchParams->Gpp.PortCfg[1].PortPresent  = UserOptions.CfgFchGppPort1Present;
  FchParams->Gpp.PortCfg[2].PortPresent  = UserOptions.CfgFchGppPort2Present;
  FchParams->Gpp.PortCfg[3].PortPresent  = UserOptions.CfgFchGppPort3Present;
  FchParams->Gpp.PortCfg[0].PortHotPlug  = UserOptions.CfgFchGppPort0HotPlug;
  FchParams->Gpp.PortCfg[1].PortHotPlug  = UserOptions.CfgFchGppPort1HotPlug;
  FchParams->Gpp.PortCfg[2].PortHotPlug  = UserOptions.CfgFchGppPort2HotPlug;
  FchParams->Gpp.PortCfg[3].PortHotPlug  = UserOptions.CfgFchGppPort3HotPlug;


  return FchParams;
}


