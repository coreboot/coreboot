/**
 * @file
 *
 * Southbridge EC IO access common routine
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
 * 
 * ***************************************************************************
 *
 */

#include "SBPLATFORM.h"
#include "ECfan.h"

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
  for (i = 0; i <= OpFlag; i++) {
    WriteIO(MailBoxPort, AccWidthUint8, &Address);  // EC_LDN9_MAILBOX_BASE_ADDRESS
    Address++;
    ReadIO(MailBoxPort + 1, AccWidthUint8, (UINT8 *)Value+i);   // EC_LDN9_MAILBOX_BASE_ADDRESS
  }
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
  for (i = 0; i <= OpFlag; i++) {
    WriteIO(MailBoxPort, AccWidthUint8, &Address);  // EC_LDN9_MAILBOX_BASE_ADDRESS
    Address++;
    WriteIO(MailBoxPort + 1, AccWidthUint8, (UINT8 *)Value+i);   // EC_LDN9_MAILBOX_BASE_ADDRESS
  }
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
    SbStall (1000); // Wait for 1ms
  }
}


