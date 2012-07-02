/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_HWACPIRESET_FILECODE

extern ACPI_REG_WRITE FchInitResetAcpiMmioTable[];


/**
 * FchInitResetHwAcpiP - Config HwAcpi controller ( Preliminary
 * ) during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetHwAcpiP (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;

  StdHeader = &((AMD_RESET_PARAMS *)FchDataPtr)->StdHeader;

  //
  // Enabled (Mmio_mem_enable)
  //
  RwPmio (FCH_PMIOA_REG24, AccessWidth8, 0xFF, BIT0, StdHeader);

  ProgramFchHwAcpiResetP (FchDataPtr);

  //
  // enable CF9
  //
  RwPmio (0xD2, AccessWidth8, (UINT32)~BIT6, 0, StdHeader);
}

/**
 * FchInitResetHwAcpi - Config HwAcpi controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  UINT16       SmbusBase;
  UINT8        Value;
  UINT16       AsfPort;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Set Build option into SB
  //
  WritePci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG64, AccessWidth16, &(UserOptions.FchBldCfg->CfgSioPmeBaseAddress), StdHeader);

  //
  // Enabled SMBUS0/SMBUS1 (ASF) Base Address
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG2C, AccessWidth16, 06, (UserOptions.FchBldCfg->CfgSmbus0BaseAddress) + BIT0);     ///protect BIT[2:1]
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0x28 , AccessWidth16, 06, (UserOptions.FchBldCfg->CfgSmbus1BaseAddress) + BIT0);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgAcpiPm1EvtBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG62, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgAcpiPm1CntBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG64, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgAcpiPmTmrBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG66, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgCpuControlBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG68, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgAcpiGpe0BlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgSmiCmdPortAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6C, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgAcpiPmaCntBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6E, AccessWidth16, 00, (UserOptions.FchBldCfg->CfgSmiCmdPortAddr) + 8);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG48, AccessWidth32, 00, (UserOptions.FchBldCfg->CfgWatchDogTimerBase));

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG2E, AccessWidth8, (UINT32)~(BIT1 + BIT2), 0);                                    ///clear BIT[2:1]
  SmbusBase = (UINT16) (UserOptions.FchBldCfg->CfgSmbus0BaseAddress);
  Value = 0x00;
  LibAmdIoWrite (AccessWidth8, SmbusBase + 0x14, &Value, StdHeader);

  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE*) (&FchInitResetAcpiMmioTable[0]), StdHeader);

  if (UserOptions.FchBldCfg->CfgFchSciMapControl != NULL) {
    ProgramFchSciMapTbl ((UserOptions.FchBldCfg->CfgFchSciMapControl), LocalCfgPtr);
  }

  if (UserOptions.FchBldCfg->CfgFchGpioControl != NULL) {
    ProgramFchGpioTbl ((UserOptions.FchBldCfg->CfgFchGpioControl), LocalCfgPtr);
  }

  if (UserOptions.FchBldCfg->CfgFchSataPhyControl != NULL) {
    ProgramFchSataPhyTbl ((UserOptions.FchBldCfg->CfgFchSataPhyControl), LocalCfgPtr);
  }

  //
  // Prevent RTC error
  //
  Value = 0x0A;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG70, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
  Value &= 0xEF;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);

  Value = 0x08;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);
  if ( !LocalCfgPtr->EcKbd ) {
    //
    // Route SIO IRQ1/IRQ12 to USB IRQ1/IRQ12 input
    //
    Value = Value | 0x0A;
  }
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);

  Value = 0x09;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);
  if ( !LocalCfgPtr->EcKbd ) {
    //
    // Route SIO IRQ1/IRQ12 to USB IRQ1/IRQ12 input
    //
    Value = Value & 0xF9;
  }

  if ( LocalCfgPtr->LegacyFree ) {
    //
    // Disable IRQ1/IRQ12 filter enable for Legacy free with USB KBC emulation.
    //
    Value = Value & 0x9F;
  }
  //
  // Enabled IRQ input
  //
  Value = Value | BIT4;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);

  AsfPort = ((UINT16) UserOptions.FchBldCfg->CfgSmbus1BaseAddress & 0xFFF0);
  if ( AsfPort != 0 ) {
    UINT8  dbValue;
    dbValue = 0x70;
    LibAmdIoWrite (AccessWidth8, AsfPort + 0x0E, &dbValue, StdHeader);
  }
  //
  // Set ACPIMMIO by OEM Input table
  //
  if ( LocalCfgPtr->OemResetProgrammingTablePtr != NULL ) {
    ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE *) (LocalCfgPtr->OemResetProgrammingTablePtr), StdHeader);
  }
}


