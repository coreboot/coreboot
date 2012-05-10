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
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
 *       its contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  if (OpFlag == 0x02) OpFlag = 0x03;

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
  if (OpFlag == 0x02) OpFlag = 0x03;

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
  UINT16   Delaytime;

  Msgdata = 0;

  for (Delaytime = 0; Delaytime <= 500; Delaytime++) {
    ReadECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
    if ( Msgdata == 0xfa) {
      break;
    }

    FchStall (1000, StdHeader);                            /// Wait for 1ms
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

  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = ( (LocalCfgPtr->Imc.ImcSureBootTimer) << 6) -1;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x94;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
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

  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccessWidth8, &Msgdata, StdHeader);
  Msgdata = 0x94;
  WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &Msgdata, StdHeader);
  WaitForEcLDN9MailboxCmdAck (StdHeader);
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
