/**

Copyright (c) 2008 - 2013, Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _FSP_API_H_
#define _FSP_API_H_

#pragma pack(1)

typedef VOID (* CONTINUATION_PROC)(EFI_STATUS Status, VOID *HobListPtr);

typedef struct {
  VOID               *NvsBufferPtr;
  VOID               *RtBufferPtr;
  CONTINUATION_PROC   ContinuationFunc;
} FSP_INIT_PARAMS;

typedef struct {
  UINT32            *StackTop;
  UINT32             BootMode;  /* Refer to boot mode defined in MdePkg\Include\Pi\PiBootMode.h */
  VOID              *UpdDataRgnPtr;
  UINT32             Reserved[7];
} FSP_INIT_RT_COMMON_BUFFER;

typedef enum {
  EnumInitPhaseAfterPciEnumeration = 0x20,
  EnumInitPhaseReadyToBoot = 0x40
} FSP_INIT_PHASE;

typedef struct {
  FSP_INIT_PHASE     Phase;
} NOTIFY_PHASE_PARAMS;

#pragma pack()


typedef FSP_STATUS (FSPAPI *FSP_FSP_INIT)    (FSP_INIT_PARAMS     *FspInitParamPtr);
typedef FSP_STATUS (FSPAPI *FSP_NOTFY_PHASE) (NOTIFY_PHASE_PARAMS *NotifyPhaseParamPtr);

#endif
