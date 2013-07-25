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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*;********************************************************************************
;
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
FchInitEnvCreatePrivateData (
  IN       AMD_ENV_PARAMS        *EnvParams
  );

FCH_DATA_BLOCK*
FchInitLoadDataBlock (
  IN       FCH_INTERFACE        *FchInterface,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

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


STATIC VOID
RetrieveDataBlockFromInitReset (
  IN       FCH_DATA_BLOCK       *FchParams
  )
{
  LOCATE_HEAP_PTR           LocHeapPtr;
  FCH_RESET_DATA_BLOCK      *ResetDb;
  AGESA_STATUS              AgesaStatus;

  LocHeapPtr.BufferHandle = AMD_FCH_RESET_DATA_BLOCK_HANDLE;
  AgesaStatus = HeapLocateBuffer (&LocHeapPtr, FchParams->StdHeader);
  if (AgesaStatus == AGESA_SUCCESS) {
    ASSERT (LocHeapPtr.BufferPtr != NULL);
    ResetDb = (FCH_RESET_DATA_BLOCK *) (LocHeapPtr.BufferPtr - sizeof (ResetDb) + sizeof (UINT32));
    // Override FchParams with contents in ResetDb

    FchParams->Usb.Xhci0Enable              = ResetDb->FchReset.Xhci0Enable;
    FchParams->Usb.Xhci1Enable              = ResetDb->FchReset.Xhci1Enable;
    FchParams->Spi.SpiFastSpeed             = ResetDb->FastSpeed;
    FchParams->Spi.WriteSpeed               = ResetDb->WriteSpeed;
    FchParams->Spi.SpiMode                  = ResetDb->Mode;
    FchParams->Spi.AutoMode                 = ResetDb->AutoMode;
    FchParams->Spi.SpiBurstWrite            = ResetDb->BurstWrite;
    FchParams->Sata.SataMode.Sata6AhciCap   = (UINT8) ResetDb->Sata6AhciCap;
    FchParams->Misc.Cg2Pll                  = ResetDb->Cg2Pll;
    FchParams->Sata.SataMode.SataSetMaxGen2 = ResetDb->SataSetMaxGen2;
    FchParams->Sata.SataMode.SataClkMode    = ResetDb->SataClkMode;
    FchParams->Sata.SataMode.SataModeReg    = ResetDb->SataModeReg;
    FchParams->Sata.SataInternal100Spread   = (UINT8) ResetDb->SataInternal100Spread;
    FchParams->Spi.SpiSpeed                 = ResetDb->SpiSpeed;
    FchParams->Gpp                          = ResetDb->Gpp;
  }
}


FCH_DATA_BLOCK*
FchInitEnvCreatePrivateData (
  IN       AMD_ENV_PARAMS        *EnvParams
  )
{
  FCH_DATA_BLOCK            *FchParams;
  ALLOCATE_HEAP_PARAMS      AllocHeapParams;
  AGESA_STATUS              AgesaStatus;

  // First allocate internal data block via heap manager
  AllocHeapParams.RequestedBufferSize = sizeof (FCH_DATA_BLOCK);
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  AllocHeapParams.BufferHandle = AMD_FCH_DATA_BLOCK_HANDLE;
  AgesaStatus = HeapAllocateBuffer (&AllocHeapParams, &EnvParams->StdHeader);
  ASSERT (!AgesaStatus);

  FchParams = (FCH_DATA_BLOCK *) AllocHeapParams.BufferPtr;
  ASSERT (FchParams != NULL);
  IDS_HDT_CONSOLE (FCH_TRACE, "    FCH Data Block Allocation: [0x%x], Ptr = 0x%08x\n", AgesaStatus, FchParams);

  // Load private data block with default
  *FchParams = InitEnvCfgDefault;
  FchParams->StdHeader = &EnvParams->StdHeader;

  RetrieveDataBlockFromInitReset (FchParams);

  // Update with external parameters
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
  FchParams->HwAcpi.PwrFailShadow        = EnvParams->FchInterface.FchPowerFail;
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
  FchParams->Azalia.AzaliaSsid               = UserOptions.FchBldCfg->CfgAzaliaSsid;
  FchParams->Smbus.SmbusSsid             = UserOptions.FchBldCfg->CfgSmbusSsid;
  FchParams->Ide.IdeSsid                 = UserOptions.FchBldCfg->CfgIdeSsid;
  FchParams->Usb.EhciSsid                = UserOptions.FchBldCfg->CfgEhciSsid;
  FchParams->Usb.OhciSsid                = UserOptions.FchBldCfg->CfgOhciSsid;
  FchParams->Usb.XhciSsid                = UserOptions.FchBldCfg->CfgXhciSsid;
  FchParams->Ir.IrPinControl             = UserOptions.FchBldCfg->CfgFchIrPinControl;
  FchParams->Sd.SdClockControl           = UserOptions.FchBldCfg->CfgFchSdClockControl;
  return FchParams;
}


