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

UINT8
getNumberOfCpuCores (
  OUT VOID
  )
{
  UINT8 Result;
  Result = 1;
  Result = ReadNumberOfCpuCores ();
  return Result;
}

UINT32
readAlink (
  IN       UINT32 Index
  )
{
  UINT32 Data;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
  ReadIO (ALINK_ACCESS_DATA, AccWidthUint32, &Data);
  //Clear Index
  Index = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32, &Index);
  return Data;
}

VOID
writeAlink (
  IN       UINT32 Index,
  IN       UINT32 Data
  )
{
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &Index);
  WriteIO (ALINK_ACCESS_DATA, AccWidthUint32 | S3_SAVE, &Data);
  //Clear Index
  Index = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &Index);
}

VOID
rwAlink (
  IN       UINT32 Index,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32 AccessType;
  AccessType = Index & 0xE0000000;
  if (AccessType == (AXINDC << 29)) {
    writeAlink ((SB_AX_INDXC_REG30 | AccessType), Index & 0x1FFFFFFF);
    Index = (SB_AX_DATAC_REG34 | AccessType);
  } else if (AccessType == (AXINDP << 29)) {
    writeAlink ((SB_AX_INDXP_REG38 | AccessType), Index & 0x1FFFFFFF);
    Index = (SB_AX_DATAP_REG3C | AccessType);
  }
  writeAlink (Index, ((readAlink (Index) & AndMask) | OrMask) );
}

