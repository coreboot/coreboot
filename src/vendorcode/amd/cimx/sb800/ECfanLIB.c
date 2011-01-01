/**
 * @file
 *
 * Southbridge EC IO access common routine
 *
 */
/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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


