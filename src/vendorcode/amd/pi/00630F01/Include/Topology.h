/* $NoKeywords:$ */
/**
 * @file
 *
 * Topology interface definitions.
 *
 * Contains AMD AGESA internal interface for topology related data which
 * is consumed by code other than Topology Services init.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 281181 $   @e \$Date: 2013-12-18 02:18:55 -0600 (Wed, 18 Dec 2013) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
 ***************************************************************************/

#ifndef _TOPOLOGY_H_
#define _TOPOLOGY_H_

// Defines for limiting data structure maximum allocation and limit checking.
#define MAX_NODES 1
#define MAX_SOCKETS MAX_NODES
#define MAX_DIES 1

/**
 * Socket and Module to Node Map Item.
 * Provide the Node Id and core id range for each module in each processor.
 */
typedef struct {
  UINT8 Node;                 ///< The module's Node id.
  UINT8 LowCore;              ///< The lowest processor core id for this module.
  UINT8 HighCore;             ///< The highest processor core id for this module.
  UINT8 EnabledComputeUnits;  ///< The value of Enabled for this processor module.
  UINT8 DualCoreComputeUnits; ///< The value of DualCore for this processor module.
  UINT8 TripleCoreComputeUnits;///< The value of TripleCore for this processor module.
  UINT8 QuadCoreComputeUnits; ///< The value of QuadCore for this processor module.
} SOCKET_DIE_TO_NODE_ITEM;

/**
 * Socket and Module to Node Map.
 * This type is a pointer to the actual map, it can be used for a struct item or
 * for typecasting a heap buffer pointer.
 */
typedef SOCKET_DIE_TO_NODE_ITEM (*SOCKET_DIE_TO_NODE_MAP)[MAX_SOCKETS][MAX_DIES];

/**
 * Node id to Socket Die Map Item.
 */
typedef struct {
  UINT8 Socket;             ///< socket of the processor containing the Node.
  UINT8 Die;                ///< the module in the processor which is Node.
} NODE_TO_SOCKET_DIE_ITEM;

/**
 * Node id to Socket Die Map.
 */
typedef NODE_TO_SOCKET_DIE_ITEM (*NODE_TO_SOCKET_DIE_MAP)[MAX_NODES];

#endif  // _TOPOLOGY_H_
