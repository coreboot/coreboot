/** @file

Copyright (C) 2013, Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/

#ifndef _FSP_INFO_HEADER_H_
#define _FSP_INFO_HEADER_H_

#pragma pack(1)

typedef struct  {

  UINT32  Signature;       // Off 0x94
  UINT32  HeaderLength;
  UINT8   Reserved1[3];
  UINT8   HeaderRevision;
  UINT32  ImageRevision;

  CHAR8   ImageId[8];      // Off 0xA4
  UINT32  ImageSize;
  UINT32  ImageBase;

  UINT32  ImageAttribute;  // Off 0xB4
  UINT32  CfgRegionOffset;
  UINT32  CfgRegionSize;
  UINT32  ApiEntryNum;

  UINT32  NemInitEntry;    // Off 0xC4
  UINT32  FspInitEntry;
  UINT32  NotifyPhaseEntry;
  UINT32  Reserved2;

} FSP_INFO_HEADER;

#pragma pack()

#endif
