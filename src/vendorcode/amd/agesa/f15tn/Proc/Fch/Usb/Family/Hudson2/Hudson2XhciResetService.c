/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
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
#define FILECODE PROC_FCH_USB_FAMILY_HUDSON2_HUDSON2XHCIRESETSERVICE_FILECODE

/**
 * FchInitResetXhciProgram - Config Xhci controller during
 * Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetXhciProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                     IndexValue;
  UINT32                    ValueDword;
  UINT8                     ValueByte;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;
  BOOLEAN               Xhci0Enable;
  BOOLEAN               Xhci1Enable;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  Xhci0Enable = LocalCfgPtr->FchReset.Xhci0Enable;
  Xhci1Enable = LocalCfgPtr->FchReset.Xhci1Enable;

  if ( Xhci0Enable ) {
    ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
    if ( ValueDword == (FCH_USB_XHCI_DID << 16) + FCH_USB_XHCI_VID) {
      //
      // First Xhci controller.
      //
      ReadPci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
      ValueDword = 0;

      IndexValue = XHCI_REGISTER_BAR00;
      ReadBiosram (IndexValue, AccessWidth32, &ValueDword, StdHeader);
      WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x10, AccessWidth32, &ValueDword, StdHeader);

      IndexValue = XHCI_REGISTER_04H;
      ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
      WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x04, AccessWidth8, &ValueByte, StdHeader);

      IndexValue = XHCI_REGISTER_0CH;
      ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
      WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x0C, AccessWidth8, &ValueByte, StdHeader);

      IndexValue = XHCI_REGISTER_3CH;
      ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
      WritePci ((USB_XHCI_BUS_DEV_FUN << 16) + 0x3C, AccessWidth8, &ValueByte, StdHeader);
      //
      // Second Xhci controller.
      //
      ReadPci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x00, AccessWidth32, &ValueDword, StdHeader);
      ValueDword = 0;

      IndexValue = XHCI1_REGISTER_BAR00;
      ReadBiosram (IndexValue, AccessWidth32, &ValueDword, StdHeader);
      WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x10, AccessWidth32, &ValueDword, StdHeader);

      IndexValue = XHCI1_REGISTER_04H;
      ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
      WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x04, AccessWidth8, &ValueByte, StdHeader);

      IndexValue = XHCI1_REGISTER_0CH;
      ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
      WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x0C, AccessWidth8, &ValueByte, StdHeader);

      IndexValue = XHCI1_REGISTER_3CH;
      ReadBiosram (IndexValue, AccessWidth8, &ValueByte, StdHeader);
      WritePci ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x3C, AccessWidth8, &ValueByte, StdHeader);
    }
  } else {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0x00000000, 0x00400700);
  }
}

