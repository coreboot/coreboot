/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch LPC controller
 *
 * Init LPC Controller features.
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

#define FILECODE PROC_FCH_SPI_FAMILY_HUDSON2_HUDSON2LPCRESETSERVICE_FILECODE

/**
 * FchInitHudson2ResetLpcPciTable - Lpc (Spi) device registers
 * initial during the power on stage.
 *
 *
 *
 *
 */
REG8_MASK FchInitHudson2ResetLpcPciTable[] =
{
  //
  // LPC Device (Bus 0, Dev 20, Func 3)
  //
  {0x00, LPC_BUS_DEV_FUN, 0},

  {FCH_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {FCH_LPC_REG7C, 0x00, BIT0 + BIT2},
  {0x78 , 0xF0, BIT2 + BIT3},                       /// Enable LDRQ pin
  {FCH_LPC_REGBB, 0xFF, BIT3 + BIT4 + BIT5},
  //
  // Set 0xBB [5:3] = 111 to improve SPI timing margin.
  // Set 0xBA [6:5] = 11 improve SPI timing margin. (SPI Prefetch enhancement)
  //
  {FCH_LPC_REGBB, 0xBE, BIT0 + BIT3 + BIT4 + BIT5},
  {FCH_LPC_REGBA, 0x9F, BIT5 + BIT6},
  // Force EC_PortActive to 1 to fix possible IR non function issue when NO_EC_SUPPORT is defined
  {FCH_LPC_REGA4, (UINT8)~ BIT0, BIT0},
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitResetLpcProgram - Config Lpc controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetLpcProgram (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //
  // enable prefetch on Host, set LPC cfg 0xBB bit 0 to 1
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT8, StdHeader);

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG6C, AccessWidth32, 0xFFFFFF00, 0, StdHeader);

  ProgramPciByteTable ( (REG8_MASK*) (&FchInitHudson2ResetLpcPciTable[0]), sizeof (FchInitHudson2ResetLpcPciTable) / sizeof (REG8_MASK), StdHeader);

  //
  // Enabling ClkRun Function
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBB, AccessWidth8, 0xFB, BIT2, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0, AccessWidth8, 0xFB, 0, StdHeader);
  //
  // LPC CLK0 Power-Down Function
  //
  if (!IsImcEnabled (StdHeader)) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD2, AccessWidth8, 0xFF, BIT3);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD2, AccessWidth8, (UINT32)~ (BIT3), 0);
  }

  if ( LocalCfgPtr->LegacyFree ) {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0x0003C000, StdHeader);
  } else {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0xFF03FFD5, StdHeader);
  }
}
