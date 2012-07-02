/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init Spread Spectrum features.
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
#include "amdlib.h"
#include "cpuServices.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_HWACPI_FAMILY_HUDSON2_HUDSON2SSSERVICE_FILECODE

/**
 * FchInitResetAcpiMmioTable - Fch ACPI MMIO initial
 * during the power on stage.
 *
 *
 *
 *
 */
ACPI_REG_WRITE FchInitResetAcpiMmioTable[] =
{
  {00, 00, 0xB0, 0xAC},                                         /// Signature
  {MISC_BASE >> 8,  FCH_MISC_REG41, 0x1F, 0x40}, //keep Auxiliary_14Mclk_Sel [12]
  //
  // USB 3.0 Reference Clock MISC_REG 0x40 [4] = 0 Enable spread-spectrum reference clock.
  //
  {MISC_BASE >> 8,  FCH_MISC_REG40, 0xEF, 0x00},

  {PMIO_BASE >> 8,  0x5D , 0x00, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGD2, 0xCF, BIT4 + BIT5},
  {SMBUS_BASE >> 8, FCH_SMBUS_REG12, 0x00, BIT0},
  {PMIO_BASE >> 8,  0x28 , 0xFF, BIT0 + BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG44 + 3, 0x67, 0},            /// Stop Boot timer
  {PMIO_BASE >> 8,  FCH_PMIOA_REG48, 0xFF, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG00, 0xFF, 0x0E},
  {PMIO_BASE >> 8,  0x00  + 2, 0xFF, 0x40},
  {PMIO_BASE >> 8,  0x00  + 3, 0xFF, 0x08},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG34, 0xEF, BIT0 + BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGEC, 0xFD, BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG08, 0xFE, BIT2 + BIT4},
  {PMIO_BASE >> 8,  0x04  + 1, 0xFF, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4 + BIT6 + BIT7},
  {PMIO_BASE >> 8,  0x04  + 3, 0xFD, BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG74, 0xF6, BIT0 + BIT3},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGF0, (UINT8)~BIT2, 0x00},

  //
  // GEC I/O Termination Setting
  // PM_Reg 0xF6 = Power-on default setting
  // PM_Reg 0xF7 = Power-on default setting
  // PM_Reg 0xF8 = 0x6C
  // PM_Reg 0xF9 = 0x21
  // PM_Reg 0xFA = 0x00 Hudson-2 A12 GEC I/O Pad settings for 3.3V CMOS
  //
  {PMIO_BASE >> 8, FCH_PMIOA_REGF8,     0x00, 0x6C},
  {PMIO_BASE >> 8, FCH_PMIOA_REGF8 + 1, 0x00, 0x07},
  {PMIO_BASE >> 8, FCH_PMIOA_REGF8 + 2, 0x00, 0x00},
  //
  // GEC -end
  //

  {PMIO_BASE >> 8, FCH_PMIOA_REGC4, 0xee, 0x04},                 /// Release NB_PCIE_RST
  {PMIO_BASE >> 8, FCH_PMIOA_REGC0 + 2, 0xBF, 0x40},
  {PMIO_BASE >> 8, FCH_PMIOA_REGBE, 0xDF, BIT5},

  //
  // Enabling ClkRun Function
  //
  {PMIO_BASE >> 8,  FCH_PMIOA_REGBB, 0xFF, BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGD0, (UINT8)~BIT2, 0},

  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * ProgramFchHwAcpiResetP  - Config SpreadSpectrum before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchHwAcpiResetP (
  IN VOID  *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = &((AMD_RESET_PARAMS *)FchDataPtr)->StdHeader;

  RwPmio (0xD3, AccessWidth8, (UINT32)~BIT4, 0, StdHeader);
  RwPmio (0xD3, AccessWidth8, (UINT32)~BIT4, BIT4, StdHeader);

  if ( LocalCfgPtr->Cg2Pll == 1 ) {
    TurnOffCG2 ();
    LocalCfgPtr->SataClkMode = 0x0a;
  }
}

