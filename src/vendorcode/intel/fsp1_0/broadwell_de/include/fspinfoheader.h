/**

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

#ifndef __FSP_INFOHEADER_H__
#define __FSP_INFOHEADER_H__

///
/// Fixed FSP header offset in the FSP image
///
#define  FSP_INFO_HEADER_OFF    0x94

#pragma pack(1)

typedef struct  {
  ///
  /// Signature ('FSPH') for the FSP Information Header
  ///
  UINT32  Signature;
  ///
  /// Length of the FSP Information Header
  ///
  UINT32  HeaderLength;
  ///
  /// Reserved
  ///
  UINT8   Reserved1[3];
  ///
  /// Revision of the FSP Information Header
  ///
  UINT8   HeaderRevision;
  ///
  /// Revision of the FSP binary
  ///
  UINT32  ImageRevision;


  ///
  /// Signature string that will help match the FSP Binary to a supported
  /// hardware configuration.
  ///
  CHAR8   ImageId[8];
  ///
  /// Size of the entire FSP binary
  ///
  UINT32  ImageSize;
  ///
  /// FSP binary preferred base address
  ///
  UINT32  ImageBase;


  ///
  /// Attribute for the FSP binary
  ///
  UINT32  ImageAttribute;
  ///
  /// Offset of the FSP configuration region
  ///
  UINT32  CfgRegionOffset;
  ///
  /// Size of the FSP configuration region
  ///
  UINT32  CfgRegionSize;
  ///
  /// Number of API entries this FSP supports
  ///
  UINT32  ApiEntryNum;


  ///
  /// TempRamInit API entry offset
  ///
  UINT32  TempRamInitEntry;
  ///
  /// FspInit API entry offset
  ///
  UINT32  FspInitEntry;
  ///
  /// NotifyPhase API entry offset
  ///
  UINT32  NotifyPhaseEntry;
  ///
  /// Reserved
  ///
  UINT32  Reserved2;

} FSP_INFO_HEADER;

#pragma pack()

#endif
