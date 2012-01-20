/* $NoKeywords:$ */
/**
 * @file
 *
 * Init the Socket and Node maps for Recovery mode.
 *
 * Create the Socket and Node maps just like normal boot,
 * except that they only indicate the BSC is present.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
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


#include "AGESA.h"
#include "Ids.h"
#include "Topology.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_HT_HTINITRECOVERY_FILECODE

AGESA_STATUS
AmdHtInitRecovery (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Get new Socket and Node Maps.
 *
 * Put the Socket Die Table and the Node Table in heap with known handles.
 *
 * @param[out]    SocketDieToNodeMap   The Socket, Module to Node info map
 * @param[out]    NodeToSocketDieMap   The Node to Socket, Module map.
 * @param[in]     StdHeader            Header for library and services.
 */
VOID
STATIC
NewNodeAndSocketTablesRecovery (
     OUT   SOCKET_DIE_TO_NODE_MAP *SocketDieToNodeMap,
     OUT   NODE_TO_SOCKET_DIE_MAP *NodeToSocketDieMap,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT8 i;
  UINT8 j;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  // Allocate heap for the table
  AllocHeapParams.RequestedBufferSize = (((MAX_SOCKETS) * (MAX_DIES)) * sizeof (SOCKET_DIE_TO_NODE_ITEM));
  AllocHeapParams.BufferHandle = SOCKET_DIE_MAP_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    // HeapAllocateBuffer must set BufferPtr to valid or NULL.
    *SocketDieToNodeMap = (SOCKET_DIE_TO_NODE_MAP)AllocHeapParams.BufferPtr;
    ASSERT (SocketDieToNodeMap != NULL);
    // Initialize shared data structures
    for (i = 0; i < MAX_SOCKETS; i++) {
      for (j = 0; j < MAX_DIES; j++) {
        (**SocketDieToNodeMap)[i][j].Node = HT_LIST_TERMINAL;
        (**SocketDieToNodeMap)[i][j].LowCore = HT_LIST_TERMINAL;
        (**SocketDieToNodeMap)[i][j].HighCore = HT_LIST_TERMINAL;
      }
    }
  }
  // Allocate heap for the table
  AllocHeapParams.RequestedBufferSize = (MAX_NODES * sizeof (NODE_TO_SOCKET_DIE_ITEM));
  AllocHeapParams.BufferHandle = NODE_ID_MAP_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    // HeapAllocateBuffer must set BufferPtr to valid or NULL.
    *NodeToSocketDieMap = (NODE_TO_SOCKET_DIE_MAP)AllocHeapParams.BufferPtr;
    ASSERT (NodeToSocketDieMap != NULL);
    // Initialize shared data structures
    for (i = 0; i < MAX_NODES; i++) {
      (**NodeToSocketDieMap)[i].Socket = HT_LIST_TERMINAL;
      (**NodeToSocketDieMap)[i].Die = HT_LIST_TERMINAL;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize the Node and Socket maps for an AP Core.
 *
 * In each core's local heap, create a Node to Socket map and a Socket/Module to Node map.
 * The mapping is filled in by reading the AP Mailboxes from PCI config on each node.
 *
 * @param[in]    StdHeader    global state, input data
 *
 * @retval       AGESA_SUCCESS  Always succeeds.
 */
AGESA_STATUS
AmdHtInitRecovery (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  AP_MAILBOXES NodeApMailBox;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  SOCKET_DIE_TO_NODE_MAP SocketDieToNodeMap = NULL;
  NODE_TO_SOCKET_DIE_MAP NodeToSocketDieMap = NULL;

  NodeApMailBox.ApMailInfo.Info = 0;
  NodeApMailBox.ApMailExtInfo.Info = 0;

  // Allocate heap for caching the mailboxes
  AllocHeapParams.RequestedBufferSize = sizeof (AP_MAILBOXES);
  AllocHeapParams.BufferHandle = LOCAL_AP_MAIL_BOX_CACHE_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    *(AP_MAILBOXES *)AllocHeapParams.BufferPtr = NodeApMailBox;
  }

  NewNodeAndSocketTablesRecovery (&SocketDieToNodeMap, &NodeToSocketDieMap, StdHeader);
  // HeapAllocateBuffer must set BufferPtr to valid or NULL, so the checks below are ok.

  // There is no option to not have socket - node maps, if they aren't allocated that is a fatal bug.
  ASSERT (SocketDieToNodeMap != NULL);
  ASSERT (NodeToSocketDieMap != NULL);

  (*SocketDieToNodeMap)[0][0].Node = 0;
  (*SocketDieToNodeMap)[0][0].LowCore = 0;
  (*SocketDieToNodeMap)[0][0].HighCore = 0;

  // We lie about being Socket 0 and Module 0 always, it isn't necessarily true.
  (*NodeToSocketDieMap)[0].Socket = (UINT8)0;
  (*NodeToSocketDieMap)[0].Die = (UINT8)0;

  return AGESA_SUCCESS;
}

