/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH Hwm controller
 *
 * Init Hwm Controller features.
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
#define FILECODE PROC_FCH_HWM_FAMILY_HUDSON2_HUDSON2HWMLATESERVICE_FILECODE

FCH_EC EcDefaultMassege = {
  0x00, 0x00, 0x35, 0x0E, 0x00, 0x54, 0x9B, 0x02, 0x01, 0x00,
  0x00, 0x01, 0x35, 0x0A, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x02, 0x05, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x46, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00,
  0x00, 0x01, 0x5F, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x00,
  0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3E, 0x87, 0x00, 0x00, 0xA8, 0x11, 0x01, 0x00, 0x00,
  0x00, 0x01, 0x86, 0x87, 0x00, 0x00, 0x14, 0x12, 0x01, 0x00, 0x01,
  0x00, 0x02, 0x31, 0x86, 0x00, 0x00, 0x33, 0x15, 0x01, 0x00, 0x02,
  0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x04, 0x86, 0x87, 0x00, 0x00, 0xA6, 0x11, 0x01, 0x00, 0x00
};

/**
 * Table for Function Number
 *
 *
 *
 *
 */
UINT8 FunctionNumber[] =
{
  Fun_81,
  Fun_83,
  Fun_85,
  Fun_89,
};

/**
 * Table for Max Thermal Zone
 *
 *
 *
 *
 */
UINT8 MaxZone[] =
{
  4,
  2,
  2,
  5,
};

/**
 * Table for Max Register
 *
 *
 *
 *
 */
UINT8 MaxRegister[] =
{
  MSG_REG9,
  MSG_REGB,
  MSG_REG9,
  MSG_REGA,
};

/*-------------------------------------------------------------------------------
;Procedure:  FchECfancontrolservice
;
;Description:  This routine service EC fan policy
;
;
;Exit:  None
;
;Modified:  None
;
;-----------------------------------------------------------------------------
*/
VOID
FchECfancontrolservice (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        ZoneNum;
  UINT8        FunNum;
  UINT8        RegNum;
  UINT8        *CurPoint;
  UINT8        FunIndex;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if (!IsImcEnabled (StdHeader)) {
    return;                                                    //IMC is not enabled
  }

  CurPoint = &LocalCfgPtr->Imc.EcStruct.MsgFun81Zone0MsgReg0 + MaxZone[0] * (MaxRegister[0] - MSG_REG0 + 1);

  for ( FunIndex = 1; FunIndex <= 3; FunIndex++ ) {
    FunNum = FunctionNumber[FunIndex];
    for ( ZoneNum = 0; ZoneNum < MaxZone[FunIndex]; ZoneNum++ ) {
      for ( RegNum = MSG_REG0; RegNum <= MaxRegister[FunIndex]; RegNum++ ) {
        WriteECmsg (RegNum, AccessWidth8, CurPoint, StdHeader);
        CurPoint += 1;
      }

      WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &FunNum, StdHeader);     // function number
      WaitForEcLDN9MailboxCmdAck (StdHeader);
    }
  }

  CurPoint = &LocalCfgPtr->Imc.EcStruct.MsgFun81Zone0MsgReg0;
  for ( FunIndex = 0; FunIndex <= 0; FunIndex++ ) {
    FunNum = FunctionNumber[FunIndex];
    for ( ZoneNum = 0; ZoneNum < MaxZone[FunIndex]; ZoneNum++ ) {
      for ( RegNum = MSG_REG0; RegNum <= MaxRegister[FunIndex]; RegNum++ ) {
        WriteECmsg (RegNum, AccessWidth8, CurPoint, StdHeader);
        CurPoint += 1;
      }

      WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &FunNum, StdHeader);      // function number
      WaitForEcLDN9MailboxCmdAck (StdHeader);
    }
  }
}

