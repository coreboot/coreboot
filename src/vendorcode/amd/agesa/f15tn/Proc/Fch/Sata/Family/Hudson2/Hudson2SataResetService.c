/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Sata controller
 *
 * Init Sata Controller features (PEI phase).
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
#define FILECODE PROC_FCH_SATA_FAMILY_HUDSON2_HUDSON2SATARESETSERVICE_FILECODE

/**
 * FchInitResetSataProgram - Config Sata controller during
 * Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSataProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        SataPortNum;
  UINT8        PortStatusByte;
  UINT8        EfuseByte;
  UINT8        FchSataMode;
  UINT8        FchSataInternal100Spread;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //FchSataMode = LocalCfgPtr->Sata.SATA_MODE.SataMode.SataModeReg;
  //New structure need calculate Sata Register value
  //
  FchSataMode = 0;
  if ( LocalCfgPtr->FchReset.SataEnable ) {
    FchSataMode |= 0x01;
  }
  if ( LocalCfgPtr->Sata6AhciCap ) {
    FchSataMode |= 0x02;
  }
  if ( LocalCfgPtr->SataSetMaxGen2 ) {
    FchSataMode |= 0x04;
  }
  if ( LocalCfgPtr->FchReset.IdeEnable ) {
    FchSataMode |= 0x08;
  }

  FchSataMode |= (( LocalCfgPtr->SataClkMode ) << 4 ) ;
  LocalCfgPtr->SataModeReg = FchSataMode;            ///Save Back to Structure

  FchSataInternal100Spread = ( UINT8 ) LocalCfgPtr->SataInternal100Spread;
  SataPortNum = 0;

  //
  // Sata Workaround
  //
  for ( SataPortNum = 0; SataPortNum < 0x08; SataPortNum++ ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040  + 2), AccessWidth8, 0xFF, 1 << SataPortNum, StdHeader);
    FchStall (2, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040  + 2), AccessWidth8, (0xFF ^ (1 << SataPortNum)) , 0x00, StdHeader);
    FchStall (2, StdHeader);
  }

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084  + 3), AccessWidth8,  (UINT32)~BIT2, 0, StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x0A0 ), AccessWidth8, (UINT32)~(BIT2 + BIT3 + BIT4 + BIT5 + BIT6), BIT2 + BIT3 + BIT4 + BIT5, StdHeader);

  //
  // Sata Setting for clock mode only
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth8, 0, FchSataMode);

  if ( FchSataInternal100Spread ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1E, AccessWidth8, 0xFF, BIT4);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 ), AccessWidth32, 0xFFFFFFFB, 0x00, StdHeader);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x1E, AccessWidth8, (UINT32)~BIT4, 0x00);
  }

  EfuseByte = SATA_FIS_BASE_EFUSE_LOC;
  GetEfuseStatus (&EfuseByte, StdHeader);

  if (EfuseByte & BIT0) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth8, 0xFB, 0x04);
  }

  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth8, &PortStatusByte);
  if ( ((PortStatusByte & 0xF0) == 0x10) ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_PMIOA_REG08, AccessWidth8, 0, BIT5);
  }

  if ( FchSataInternal100Spread ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 ), AccessWidth32, 0xFFFFFFFF, 0x04, StdHeader);
  }
}


