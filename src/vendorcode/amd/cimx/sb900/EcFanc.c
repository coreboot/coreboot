/*;********************************************************************************
;
;Copyright (c) 2011, Advanced Micro Devices, Inc.
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without
;modification, are permitted provided that the following conditions are met:
;    * Redistributions of source code must retain the above copyright
;      notice, this list of conditions and the following disclaimer.
;    * Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;    * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;      its contributors may be used to endorse or promote products derived 
;      from this software without specific prior written permission.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
;DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/


#include "SbPlatform.h"
#include "cbtypes.h"

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
BOOLEAN
IsZoneFuncEnable (
  IN     UINT16 Flag,
  IN     UINT8  func,
  IN     UINT8  Zone
  )
{
  return (BOOLEAN) (((Flag >> (func *4)) & 0xF) & ((UINT8 )1 << Zone));
}

/*-------------------------------------------------------------------------------
;Procedure:  sbECfancontrolservice
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
sbECfancontrolservice (
  IN     AMDSBCFG* pConfig
  )
{
  UINT8 ZoneNum;
  UINT8 FunNum;
  UINT8 RegNum;
  UINT8 * CurPoint;
  UINT8 FunIndex;
  BOOLEAN IsSendEcMsg;
  if (!isImcEnabled ()) {
    return; //IMC is not enabled
  }

  CurPoint = &pConfig->Pecstruct.MSGFun81zone0MSGREG0 + MaxZone[0] * (MaxRegister[0] - MSG_REG0 + 1);
  for ( FunIndex = 1; FunIndex <= 3; FunIndex++ ) {
    FunNum = FunctionNumber[FunIndex];
    for ( ZoneNum = 0; ZoneNum < MaxZone[FunIndex]; ZoneNum++ ) {
      IsSendEcMsg = IsZoneFuncEnable (pConfig->Pecstruct.IMCFUNSupportBitMap, FunIndex, ZoneNum);
      if (IsSendEcMsg) {
        for ( RegNum = MSG_REG0; RegNum <= MaxRegister[FunIndex]; RegNum++ ) {
          WriteECmsg (RegNum, AccWidthUint8, CurPoint); //
          CurPoint += 1;
        }
        WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &FunNum); // function number
        WaitForEcLDN9MailboxCmdAck ();
      } else {
        CurPoint += (MaxRegister[FunIndex] - MSG_REG0 + 1);
      }
    }
  }
  CurPoint = &pConfig->Pecstruct.MSGFun81zone0MSGREG0;
  for ( FunIndex = 0; FunIndex <= 0; FunIndex++ ) {
    FunNum = FunctionNumber[FunIndex];
    for ( ZoneNum = 0; ZoneNum < MaxZone[FunIndex]; ZoneNum++ ) {
      IsSendEcMsg = IsZoneFuncEnable (pConfig->Pecstruct.IMCFUNSupportBitMap, FunIndex, ZoneNum);
      if (IsSendEcMsg) {
        for ( RegNum = MSG_REG0; RegNum <= MaxRegister[FunIndex]; RegNum++ ) {
          if (RegNum == MSG_REG2) {
            *CurPoint &= 0xFE;
          }
          WriteECmsg (RegNum, AccWidthUint8, CurPoint); //
          CurPoint += 1;
        }
        WriteECmsg (MSG_SYS_TO_IMC, AccWidthUint8, &FunNum); // function number
        WaitForEcLDN9MailboxCmdAck ();
      } else {
        CurPoint += (MaxRegister[FunIndex] - MSG_REG0 + 1);
      }
    }
  }
}

/*-------------------------------------------------------------------------------
;Procedure:  hwmImcInit
;
;Description:  This routine Init EC fan policy
;
;
;Exit:  None
;
;Modified:  None
;
;-----------------------------------------------------------------------------
*/
VOID
hwmImcInit (
  IN     AMDSBCFG* pConfig
  )
{
  imcWakeup ( pConfig);
  if ( pConfig->hwm.hwmSbtsiAutoPoll == FALSE ) {
    sbECfancontrolservice (pConfig);
  }
}


