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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
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
  FchParams->Gec.GecShadowRomBase        = UserOptions.FchBldCfg->CfgGecShadowRomBase;
  FchParams->Spi.RomBaseAddress          = UserOptions.FchBldCfg->CfgSpiRomBaseAddress;
  FchParams->Sd.SdSsid                   = UserOptions.FchBldCfg->CfgSdSsid;
  FchParams->Spi.LpcSsid                 = UserOptions.FchBldCfg->CfgLpcSsid;
  FchParams->Hpet.HpetBase               = UserOptions.FchBldCfg->CfgHpetBaseAddress;
  FchParams->Smbus.SmbusSsid             = UserOptions.FchBldCfg->CfgSmbusSsid;
  FchParams->Ide.IdeSsid                 = UserOptions.FchBldCfg->CfgIdeSsid;
  FchParams->Usb.EhciSsid                = UserOptions.FchBldCfg->CfgEhciSsid;
  FchParams->Usb.OhciSsid                = UserOptions.FchBldCfg->CfgOhciSsid;
  FchParams->Usb.XhciSsid                = UserOptions.FchBldCfg->CfgXhciSsid;
  FchParams->Ir.IrPinControl             = UserOptions.FchBldCfg->CfgFchIrPinControl;
  FchParams->Sd.SdClockControl           = UserOptions.FchBldCfg->CfgFchSdClockControl;
  return FchParams;
}


