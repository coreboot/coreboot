/** @file

Copyright (C) 2013 - 2014, Intel Corporation

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

#ifndef __FSP_SUPPORT_H__
#define __FSP_SUPPORT_H__

#include "fsptypes.h"
#include "fspfv.h"
#include "fspffs.h"
#include "fspapi.h"
#include "fsphob.h"
#include "fspguid.h"
#include "fspplatform.h"
#include "fspinfoheader.h"
#include "fspbootmode.h"
#include "fspvpd.h"

UINT32
GetUsableLowMemTop (
  CONST VOID             *HobListPtr
  );

UINT64
GetUsableHighMemTop (
  CONST VOID             *HobListPtr
  );

VOID *
GetGuidHobDataBuffer (
  CONST VOID             *HobListPtr,
  UINT32                 *Length,
  EFI_GUID               *Guid
  );

VOID
GetFspReservedMemoryFromGuid (
  CONST VOID             *HobListPtr,
  EFI_PHYSICAL_ADDRESS   *FspMemoryBase,
  UINT64                 *FspMemoryLength,
  EFI_GUID               *FspReservedMemoryGuid
  );

UINT32
GetTsegReservedMemory (
  CONST VOID             *HobListPtr,
  UINT32                 *Length
);

UINT32
GetFspReservedMemory (
  CONST VOID             *HobListPtr,
  UINT32                 *Length
);

VOID*
GetFspNvsDataBuffer (
  CONST VOID             *HobListPtr,
  UINT32                 *Length
  );

VOID *
GetBootloaderTempMemoryBuffer (
  CONST VOID             *HobListPtr,
  UINT32                 *Length
  );


#endif
