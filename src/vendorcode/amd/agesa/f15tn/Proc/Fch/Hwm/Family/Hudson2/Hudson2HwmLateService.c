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
 * @e \$Revision: 64785 $   @e \$Date: 2012-01-30 21:46:59 -0600 (Mon, 30 Jan 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

/**
 * Table for Function Number
 *
 *
 *
 *
 */
STATIC UINT8 FunctionNumber[] =
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
  4,
  4,
  4,
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
;Procedure:  IsZoneFuncEnable
;
;Description:  This routine will check every zone support function with BitMap from user define
;
;
;Exit:  None
;
;Modified:  None
;
;-----------------------------------------------------------------------------
*/
STATIC BOOLEAN
IsZoneFuncEnable (
  IN     UINT16 Flag,
  IN     UINT8  func,
  IN     UINT8  Zone
  )
{
  return (BOOLEAN) (((Flag >> (func *4)) & 0xF) & ((UINT8 )1 << Zone));
}

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
  BOOLEAN      IsSendEcMsg;
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
      IsSendEcMsg = IsZoneFuncEnable (LocalCfgPtr->Imc.EcStruct.IMCFUNSupportBitMap, FunIndex, ZoneNum);
      if (IsSendEcMsg) {
        for ( RegNum = MSG_REG0; RegNum <= MaxRegister[FunIndex]; RegNum++ ) {
          WriteECmsg (RegNum, AccessWidth8, CurPoint, StdHeader);
          CurPoint += 1;
        }
        WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &FunNum, StdHeader);     // function number
        WaitForEcLDN9MailboxCmdAck (StdHeader);
      } else {
        CurPoint += (MaxRegister[FunIndex] - MSG_REG0 + 1);
      }
    }
  }

  CurPoint = &LocalCfgPtr->Imc.EcStruct.MsgFun81Zone0MsgReg0;
  for ( FunIndex = 0; FunIndex <= 0; FunIndex++ ) {
    FunNum = FunctionNumber[FunIndex];
    for ( ZoneNum = 0; ZoneNum < MaxZone[FunIndex]; ZoneNum++ ) {
      IsSendEcMsg = IsZoneFuncEnable (LocalCfgPtr->Imc.EcStruct.IMCFUNSupportBitMap, FunIndex, ZoneNum);
      if (IsSendEcMsg) {
        for ( RegNum = MSG_REG0; RegNum <= MaxRegister[FunIndex]; RegNum++ ) {
          if (RegNum == MSG_REG2) {
            *CurPoint &= 0xFE;
          }
          WriteECmsg (RegNum, AccessWidth8, CurPoint, StdHeader);
          CurPoint += 1;
        }
        WriteECmsg (MSG_SYS_TO_IMC, AccessWidth8, &FunNum, StdHeader);      // function number
        WaitForEcLDN9MailboxCmdAck (StdHeader);
      } else {
        CurPoint += (MaxRegister[FunIndex] - MSG_REG0 + 1);
      }
    }
  }
}

