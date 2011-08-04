/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access heap.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-13 19:07:10 -0700 (Tue, 13 Jul 2010) $
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
 * 
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Porting.h"
#include  "AMD.h"
#include  "heapManager.h"
#include  "GnbLibPciAcc.h"
#include  "GnbLibHeap.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCOMMONLIB_GNBLIBHEAP_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 * Allocates space for a new buffer in the heap
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      Length            Buffer length
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer allocation fail
 *
 */

VOID*
GnbAllocateHeapBuffer (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;

  AllocHeapParams.RequestedBufferSize = (UINT32) Length;
  AllocHeapParams.BufferHandle = Handle;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return NULL;
  }
  return AllocHeapParams.BufferPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Locates a previously allocated buffer on the heap.
 *
 *
 * @param[in]      Handle            Buffer handle
 * @param[in]      StdHeader         Standard configuration header
 *
 * @retval         NULL              Buffer handle not found
 *
 */

VOID *
GnbLocateHeapBuffer (
  IN      UINT32              Handle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  LOCATE_HEAP_PTR       LocHeapParams;
  LocHeapParams.BufferHandle = Handle;
  Status = HeapLocateBuffer (&LocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    return NULL;
  }
  return LocHeapParams.BufferPtr;
}

