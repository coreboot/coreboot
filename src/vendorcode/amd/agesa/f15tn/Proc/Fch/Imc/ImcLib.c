/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH IMC lib
 *
 *
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
#define FILECODE PROC_FCH_IMC_IMCLIB_FILECODE

VOID
WriteECmsg (
  IN       UINT8     Address,
  IN       UINT8     OpFlag,
  IN       VOID      *Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   Index;

  OpFlag = OpFlag & 0x7f;
  if (OpFlag == 0x02) {
    OpFlag = 0x03;
  }

  for (Index = 0; Index <= OpFlag; Index++) {
    /// EC_LDN9_MAILBOX_BASE_ADDRESS
    LibAmdIoWrite (AccessWidth8, 0x3E, &Address, StdHeader);
    Address++;
    /// EC_LDN9_MAILBOX_BASE_ADDRESS
    LibAmdIoWrite (AccessWidth8, 0x3F, (UINT8 *)Value + Index, StdHeader);
  }
}

VOID
ReadECmsg (
  IN       UINT8     Address,
  IN       UINT8     OpFlag,
     OUT   VOID      *Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 Index;

  OpFlag = OpFlag & 0x7f;
  if (OpFlag == 0x02) {
    OpFlag = 0x03;
  }

  for (Index = 0; Index <= OpFlag; Index++) {
    /// EC_LDN9_MAILBOX_BASE_ADDRESS
    LibAmdIoWrite (AccessWidth8, 0x3E, &Address, StdHeader);
    Address++;
    /// EC_LDN9_MAILBOX_BASE_ADDRESS
    LibAmdIoRead (AccessWidth8, 0x3F, (UINT8 *)Value + Index, StdHeader);
  }
}

VOID
WaitForEcLDN9MailboxCmdAck (
  IN AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8    Msgdata;
  UINT32   Delaytime;

  Msgdata = 0;

  for (Delaytime = 0; Delaytime <= 100000; Delaytime++) {
    ReadECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
    if ( Msgdata == 0xfa) {
      break;
    }

    FchStall (5, StdHeader);                            /// Wait for 1ms
  }
}

/**
 * ImcSleep - IMC Sleep.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ImcSleep (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Msgdata;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if (!(IsImcEnabled (StdHeader)) ) {
    return;                                                ///IMC is not enabled
  }

  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0xB4;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x96;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
}


/**
 * ImcEnableSurebootTimer - IMC Enable Sureboot Timer.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ImcEnableSurebootTimer (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  Msgdata;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ImcDisableSurebootTimer (LocalCfgPtr);

  Msgdata = 0x00;

  if (!(IsImcEnabled (StdHeader)) || (LocalCfgPtr->Imc.ImcSureBootTimer == 0)) {
    return;                                      ///IMC is not enabled
  }

  ImcWakeup (FchDataPtr);
  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = ( (LocalCfgPtr->Imc.ImcSureBootTimer) << 6) -1;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x94;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
  ImcSleep (FchDataPtr);
}

/**
 * ImcDisableSurebootTimer - IMC Disable Sureboot Timer.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ImcDisableSurebootTimer (
  IN  VOID     *FchDataPtr
  )
{
  UINT8   Msgdata;
  AMD_CONFIG_PARAMS      *StdHeader;

  StdHeader = ((FCH_DATA_BLOCK *) FchDataPtr)->StdHeader;

  if (!(IsImcEnabled (StdHeader)) ) {
    return;                                      ///IMC is not enabled
  }

  ImcWakeup (FchDataPtr);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x94;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
  ImcSleep (FchDataPtr);
}

/**
 * ImcWakeup - IMC Wakeup.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ImcWakeup (
  IN  VOID     *FchDataPtr
  )
{
  UINT8   Msgdata;
  AMD_CONFIG_PARAMS     *StdHeader;

  StdHeader = ((FCH_DATA_BLOCK *) FchDataPtr)->StdHeader;
  if (!(IsImcEnabled (StdHeader)) ) {
    return;                                      ///IMC is not enabled
  }

  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0xB5;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x96;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
}

/**
 * ImcIdle - IMC Idle.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ImcIdle (
  IN  VOID     *FchDataPtr
  )
{
  UINT8   Msgdata;
  AMD_CONFIG_PARAMS     *StdHeader;

  StdHeader = ((FCH_DATA_BLOCK *) FchDataPtr)->StdHeader;

  if (!(IsImcEnabled (StdHeader)) ) {
    return;                                      ///IMC is not enabled
  }

  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x98;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
}
