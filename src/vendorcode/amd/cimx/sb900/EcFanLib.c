/**
 * @file
 *
 * Southbridge EC IO access common routine
 *
 */
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/

#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

VOID
ReadECmsg (
  IN       UINT8 Address,
  IN       UINT8 OpFlag,
     OUT   VOID* Value
  )
{
 UINT8 i;

  OpFlag = OpFlag & 0x7f;
  if (OpFlag == 0x02) OpFlag = 0x03;
  WriteIo8((UINT16) (0x3E), Address);  // EC_LDN9_MAILBOX_BASE_ADDRESS
  i = ReadIo8((UINT16) (0x3F));   // EC_LDN9_MAILBOX_BASE_ADDRESS
  *((UINT8*) (Value)) = i;
}


VOID
WriteECmsg (
  IN       UINT8 Address,
  IN       UINT8 OpFlag,
  IN       VOID* Value
  )
{
 UINT8 i;

  OpFlag = OpFlag & 0x7f;
  if (OpFlag == 0x02) OpFlag = 0x03;
  WriteIo8(0x3E, Address);  // EC_LDN9_MAILBOX_BASE_ADDRESS
  i = *(UINT8*)Value;
  WriteIo8(0x3F, i);   // EC_LDN9_MAILBOX_BASE_ADDRESS
}

VOID
WaitForEcLDN9MailboxCmdAck (
  VOID
  )
{
  UINT8 Msgdata;
  UINT16 Delaytime;
  Msgdata = 0;
  for (Delaytime = 0; Delaytime <= 500; Delaytime++) {
    ReadECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
    if ( Msgdata == 0xfa) {
      break;
    }
    cimSbStall (1000); // Wait for 1ms
  }
}

/**
 * imcDisableSurebootTimer - IMC Disable Sureboot Timer.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
imcDisableSurebootTimer (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 Msgdata;

  if (!(isImcEnabled ()) ) {
    return; //IMC is not enabled
  }
  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
  Msgdata = 0x94;
  WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
  WaitForEcLDN9MailboxCmdAck ();
}

/**
 * imcDisarmSurebootTimer - IMC Disarm Sureboot Timer.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
imcDisarmSurebootTimer (
  IN       AMDSBCFG* pConfig
  )
{
  imcDisableSurebootTimer (pConfig);
  pConfig->imc.imcSureBootTimer = 0;
}

/**
 * imcEnableSurebootTimer - IMC Enable Sureboot Timer.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
imcEnableSurebootTimer (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 Msgdata;

  imcDisableSurebootTimer (pConfig);

  Msgdata = 0x00;
  if (!(isImcEnabled ()) || (pConfig->imc.imcSureBootTimer == 0)) {
    return; //IMC is not enabled
  }
  WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
  Msgdata = ( (pConfig->imc.imcSureBootTimer) << 6) -1;
  WriteECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
  Msgdata = 0x94;
  WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
  WaitForEcLDN9MailboxCmdAck ();
}

/**
 * imcSleep - IMC Sleep.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
imcSleep (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 Msgdata;

  if (!(isImcEnabled ()) ) {
    return; //IMC is not enabled
  }
  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
  Msgdata = 0xB4;
  WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
  Msgdata = 0x96;
  WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
  WaitForEcLDN9MailboxCmdAck ();
}

/**
 * imcWakeup - IMC Wakeup.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
imcWakeup (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 Msgdata;

  if (!(isImcEnabled ()) ) {
    return; //IMC is not enabled
  }
  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
  Msgdata = 0xB5;
  WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
  Msgdata = 0x96;
  WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
  WaitForEcLDN9MailboxCmdAck ();
}

/**
 * imcIdle - IMC Idle.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
imcIdle (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 Msgdata;

  if (!(isImcEnabled ()) ) {
    return; //IMC is not enabled
  }
  Msgdata = 0x00;
  WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
  Msgdata = 0x01;
  WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
  Msgdata = 0x00;
  WriteECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
  Msgdata = 0x98;
  WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
  WaitForEcLDN9MailboxCmdAck ();
}

VOID
imcThermalZoneEnable (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8   Msgdata;
  UINT8   ZoneNum;
  BOOLEAN IsSendEcMsg;

  if (!(isImcEnabled ()) ) {
    return; //IMC is not enabled
  }

  for ( ZoneNum = 0; ZoneNum < 4; ZoneNum++ ) {
    IsSendEcMsg = IsZoneFuncEnable (pConfig->Pecstruct.IMCFUNSupportBitMap, 0, ZoneNum);
    if (IsSendEcMsg) {
      Msgdata = 0x00;
      WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
      Msgdata = ZoneNum;
      WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
      Msgdata = 0x80;
      WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
      WaitForEcLDN9MailboxCmdAck ();

      Msgdata = 0x00;
      WriteECmsg (MSG_REG0, AccWidthUint8, &Msgdata);
      Msgdata = ZoneNum;
      WriteECmsg (MSG_REG1, AccWidthUint8, &Msgdata);
      ReadECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
      Msgdata |= BIT0;
      WriteECmsg (MSG_REG2, AccWidthUint8, &Msgdata);
      Msgdata = 0x81;
      WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &Msgdata);
      WaitForEcLDN9MailboxCmdAck ();
    }
  }
}

