/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Pcib controller
 *
 * Init Pcib Controller features.
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
#define FILECODE PROC_FCH_PCIB_PCIBLATE_FILECODE

/**
 * FchInitLatePcib - Prepare Pcib controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLatePcib (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Value;
  UINT8        NStBit;
  UINT8        NSBit;
  UINT32       VarDd;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // We need to do the following setting in late post also because some bios core pci enumeration changes these values
  // programmed during early post.
  //  Master Latency Timer
  //
  Value = 0x40;
  WritePci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG0D, AccessWidth8, &Value, StdHeader);
  WritePci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG1B, AccessWidth8, &Value, StdHeader);

  //
  // CLKRUN#
  // FCH P2P AutoClock control settings.
  // VarDd = (FchDataPtr->PcibAutoClkCtrlLow) | (FchDataPtr->PcibAutoClkCtrlLow);
  //
  if ( LocalCfgPtr->Pcib.PcibClockRun ) {
    ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG54, AccessWidth8, &Value);
    NStBit = Value & 0x03;
    NSBit = (Value & 0x3F ) >> 2;
    VarDd = (4 + (NStBit * 2) + (( 17 + NSBit) * 3) + 4) | 0x01;

    VarDd = 9; // for A12
    WritePci ((PCIB_BUS_DEV_FUN << 16) + FCH_PCIB_REG4C, AccessWidth32, &VarDd, StdHeader);
  }

  VarDd = (LocalCfgPtr->Pcib.PcibClkStopOverride);
  RwPci ((PCIB_BUS_DEV_FUN << 16) + 0x50 , AccessWidth16, 0x3F, (UINT16) (VarDd << 6), StdHeader);
}
