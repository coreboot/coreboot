/**@file

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

#ifndef _AZALIA_H_
#define _AZALIA_H_

#include <stdint.h>

#pragma pack(1)

typedef struct {
  uint32_t  VendorDeviceId;
  uint16_t  SubSystemId;
  uint8_t   RevisionId;                       /// 0xFF applies to all steppings
  uint8_t   FrontPanelSupport;
  uint16_t  NumberOfRearJacks;
  uint16_t  NumberOfFrontJacks;
} PCH_AZALIA_VERB_TABLE_HEADER;

typedef struct {
  PCH_AZALIA_VERB_TABLE_HEADER  VerbTableHeader;
  uint32_t                        *VerbTableData;
} PCH_AZALIA_VERB_TABLE;

typedef struct {
  uint8_t                 Pme       : 1;      /// 0: Disable; 1: Enable
  uint8_t                 DS        : 1;      /// 0: Docking is not supported; 1:Docking is supported
  uint8_t                 DA        : 1;      /// 0: Docking is not attached; 1:Docking is attached
  uint8_t                 HdmiCodec : 1;      /// 0: Disable; 1: Enable
  uint8_t                 AzaliaVCi : 1;      /// 0: Disable; 1: Enable
  uint8_t                 Rsvdbits  : 3;
  uint8_t                 AzaliaVerbTableNum; /// Number of verb tables provided by platform
  PCH_AZALIA_VERB_TABLE  *AzaliaVerbTable;   /// Pointer to the actual verb table(s)
  uint16_t                ResetWaitTimer;     /// The delay timer after Azalia reset, the value is number of microseconds
} PCH_AZALIA_CONFIG;

#pragma pack()

#endif
