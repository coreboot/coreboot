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

#ifndef __FSP_API_H__
#define __FSP_API_H__

///
/// FSP Init continuation function prototype.
/// Control will be returned to this callback function after FspInit API call.
///
typedef VOID       (FSPAPI *CONTINUATION_PROC)  (EFI_STATUS Status,  VOID *HobListPtr);

#pragma pack(1)

typedef struct {
  ///
  /// Non-volatile storage buffer pointer.
  ///
  VOID               *NvsBufferPtr;
  ///
  /// Runtime buffer pointer
  ///
  VOID               *RtBufferPtr;
  ///
  /// Continuation function address
  ///
  CONTINUATION_PROC   ContinuationFunc;
} FSP_INIT_PARAMS;

typedef struct {
  ///
  /// Stack top pointer used by the bootloader.
  /// The new stack frame will be set up at this location after FspInit API call.
  ///
  UINT32             *StackTop;
  ///
  /// Current system boot mode.
  ///
  UINT32              BootMode;
  ///
  /// User platform configuraiton data region pointer.
  ///
  VOID               *UpdDataRgnPtr;
  ///
  /// Reserved
  ///
  UINT32              Reserved[7];
} FSP_INIT_RT_COMMON_BUFFER;

typedef enum {
  ///
  /// Notification code for post PCI enuermation
  ///
  EnumInitPhaseAfterPciEnumeration = 0x20,
  ///
  /// Notification code before transfering control to the payload
  ///
  EnumInitPhaseReadyToBoot         = 0x40
} FSP_INIT_PHASE;

typedef struct {
  ///
  /// Notification phase used for NotifyPhase API
  ///
  FSP_INIT_PHASE     Phase;
} NOTIFY_PHASE_PARAMS;

#pragma pack()

///
/// FspInit API function prototype
///
typedef FSP_STATUS (FSPAPI *FSP_FSP_INIT)       (FSP_INIT_PARAMS     *FspInitParamPtr);

///
/// NotifyPhase API function prototype
///
typedef FSP_STATUS (FSPAPI *FSP_NOTFY_PHASE)    (NOTIFY_PHASE_PARAMS *NotifyPhaseParamPtr);

#endif
