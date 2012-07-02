/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch IDE controller
 *
 * Init IDE Controller features.
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
#define FILECODE PROC_FCH_IDE_IDEENV_FILECODE

/**
 * FchInitEnvIde - Config Ide controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvIde (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Channel;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG40), AccessWidth8, 0xff, BIT0, StdHeader);

  //
  // Enabling IDE Explicit Pre-Fetch  IDE PCI Config 0x62[8]=0
  //
  RwPci (((IDE_BUS_DEV_FUN << 16) + 0x62  + 1), AccessWidth8,  (UINT32)~BIT0, BIT5, StdHeader);

  //
  // Disable SATA MSI
  //
  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG34), AccessWidth8, 0x00, 0x00, StdHeader);
  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG06), AccessWidth8, 0xEF, 0x00, StdHeader);

  //
  // Set Ide Channel enable/disable by parameter
  //
  ReadPci (((IDE_BUS_DEV_FUN << 16) + 0x040  + 11), AccessWidth8, &Channel, StdHeader);
  Channel &= 0xCF;
  if ( LocalCfgPtr->Sata.IdeDisUnusedIdePChannel ) {
    Channel |= 0x10;
  }
  if ( LocalCfgPtr->Sata.IdeDisUnusedIdeSChannel ) {
    Channel |= 0x20;
  }
  WritePci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG40 + 11), AccessWidth8, &Channel, StdHeader);

  //
  // IDE Controller Class ID & SSID
  // ** Get Sata Configuration ** for sync Sata & Ide with only one Legacy Ide device
  //
  if ( (LocalCfgPtr->Sata.SataIdeMode == 1) && (LocalCfgPtr->Sata.SataClass != SataLegacyIde) ) {
    //
    // Write the class code to IDE PCI register 08h-0Bh
    //
    RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG08), AccessWidth32, 0, 0x01018F40, StdHeader);
  }
  if ( LocalCfgPtr->Sata.SataClass == SataLegacyIde ) {
    //
    //Set SATA controller to native mode
    //
    RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG09), AccessWidth8, 0x00, 0x08F, StdHeader);
  }
  if (LocalCfgPtr->Ide.IdeSsid != 0 ) {
    RwPci ((IDE_BUS_DEV_FUN << 16) + 0x2C , AccessWidth32, 0x00, LocalCfgPtr->Ide.IdeSsid, StdHeader);
  }

  //
  // Disable write access to PCI header
  //
  RwPci (((IDE_BUS_DEV_FUN << 16) + FCH_IDE_REG40), AccessWidth8, (UINT32)~BIT0, 0, StdHeader);
}



