/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Ec controller
 *
 * Init Ec Controller features (PEI phase).
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
#define FILECODE PROC_FCH_IMC_FCHECRESET_FILECODE

/**
 * FchInitResetEc - Config Ec controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetEc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Enable config mode
  //
  EnterEcConfig (StdHeader);

  //
  //Do settings for mailbox - logical device 0x09
  //
  RwEc8 (0x07, 0x00, 0x09, StdHeader);                      ///switch to device 9 (Mailbox)
  RwEc8 (0x60, 0x00, (MailBoxPort >> 8), StdHeader);        ///set MSB of Mailbox port
  RwEc8 (0x61, 0x00, (MailBoxPort & 0xFF), StdHeader);      ///set LSB of Mailbox port
  RwEc8 (0x30, 0x00, 0x01, StdHeader);                      ///;Enable Mailbox Registers Interface, bit0=1

  if ( LocalCfgPtr->EcKbd == ENABLED) {
    //
    //Enable KBRST#, IRQ1 & IRQ12, GateA20 Function signal from IMC
    //
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD6, AccessWidth8, (UINT32)~BIT8, BIT0 + BIT1 + BIT2 + BIT3);

    //
    //Disable LPC Decoding of port 60/64
    //
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG47), AccessWidth8, (UINT32)~BIT5, 0, StdHeader);

    //
    //Enable logical device 0x07 (Keyboard controller)
    //
    RwEc8 (0x07, 0x00, 0x07, StdHeader);
    RwEc8 (0x30, 0x00, 0x01, StdHeader);
  }

  if (IsImcEnabled (StdHeader) && ( LocalCfgPtr->EcChannel0 == ENABLED)) {
    //
    //Logical device 0x03
    //
    RwEc8 (0x07, 0x00, 0x03, StdHeader);
    RwEc8 (0x60, 0x00, 0x00, StdHeader);
    RwEc8 (0x61, 0x00, 0x62, StdHeader);
    RwEc8 (0x30, 0x00, 0x01, StdHeader);                    ///;Enable Device 3
  }

  //
  //Enable EC (IMC) to generate SMI to BIOS
  //
  RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB3, AccessWidth8, (UINT32)~BIT6, BIT6);
  ExitEcConfig (StdHeader);
}



