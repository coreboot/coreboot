/* $NoKeywords:$ */
/**
 * @file
 *
 * Topology interface definitions.
 *
 * Contains AMD AGESA internal interface for topology related data which
 * is consumed by code other than HyperTransport init (and produced by
 * HyperTransport init.)
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#define MAX_NODES 8
#define MAX_SOCKETS MAX_NODES
#define MAX_DIES 2

// Defines useful with package link
#define HT_LIST_MATCH_INTERNAL_LINK_0 0xFA
#define HT_LIST_MATCH_INTERNAL_LINK_1 0xFB
#define HT_LIST_MATCH_INTERNAL_LINK_2 0xFC

/**
 * Hop Count Table.
 * This is a heap data structure.  The Hops array is filled as a size x size matrix.
 * The unused space, if any, is all at the end.
 */
typedef struct {
  UINT8 Size;                         ///< The row and column size of actual hop count data */
  UINT8 Hops[MAX_NODES * MAX_NODES];  ///< Room for a dynamic two dimensional array of [size][size] */
} HOP_COUNT_TABLE;

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

/**
 * Provide AP core with socket and node context at start up.
 * This information is posted to the AP cores using a register as a mailbox.
 */
typedef struct {
  UINT32 Node:4;          ///< The node id of Core's node.
  UINT32 Socket:4;        ///< The socket of this Core's node.
  UINT32 Module:2;        ///< The internal module number for Core's node.
  UINT32 ModuleType:2;    ///< Single Module = 0, Multi-module = 1.
  UINT32 :20;             ///< Reserved
} AP_MAIL_INFO_FIELDS;

/**
 * AP info fields can be written and read to a register.
 */
typedef union {
  UINT32              Info;        ///< Just a number for register access, or opaque passing.
  AP_MAIL_INFO_FIELDS Fields;      ///< access to the info fields.
} AP_MAIL_INFO;

/**
 * Provide AP core with system degree and system core number at start up.
 * This information is posted to the AP cores using a register as a mailbox.
 */
typedef struct {
  UINT32 SystemDegree:3;  ///< The number of connected links
  UINT32 :3;              ///< Reserved
  UINT32 HeapIndex:6;     ///< The zero-based system core number
  UINT32 :20;             ///< Reserved
} AP_MAIL_EXT_INFO_FIELDS;

/**
 * AP info fields can be written and read to a register.
 */
typedef union {
  UINT32              Info;        ///< Just a number for register access, or opaque passing.
  AP_MAIL_EXT_INFO_FIELDS Fields;  ///< access to the info fields.
} AP_MAIL_EXT_INFO;

/**
 * AP Info mailbox set.
 */
typedef struct {
  AP_MAIL_INFO      ApMailInfo;    ///< The AP mail info
  AP_MAIL_EXT_INFO  ApMailExtInfo; ///< The extended AP mail info
} AP_MAILBOXES;

/**
 * Provide a northbridge to package mapping for link assignments.
 *
 */
typedef struct {
  UINT8  Link;             ///< The Node's link
  UINT8  Module;           ///< The internal module position of Node
  UINT8  PackageLink;      ///< The corresponding package link
} PACKAGE_HTLINK_MAP_ITEM;

/**
 * A Processor's complete set of link assignments
 */
typedef PACKAGE_HTLINK_MAP_ITEM (*PACKAGE_HTLINK_MAP)[];

#endif  // _TOPOLOGY_H_
