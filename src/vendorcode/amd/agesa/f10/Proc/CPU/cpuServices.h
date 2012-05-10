/**
 * @file
 *
 * AMD CPU Services
 *
 * Related to the General Services API's, but for the CPU component.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_SERVICES_H_
#define _CPU_SERVICES_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
 /// WARM RESET STATE_BITS
#define WR_STATE_COLD  00
#define WR_STATE_RESET 01
#define WR_STATE_EARLY 02
#define WR_STATE_POST  03

/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */
//----------------------------------------------------------------------------
//                         CPU SYSTEM INFO TYPEDEFS, STRUCTURES, ENUMS
//
//----------------------------------------------------------------------------
/// SYSTEM INFO
typedef struct _SYSTEM_INFO {
  UINT32            TotalNumberOfSockets;               ///< Total Number of Sockets
  UINT32            TotalNumberOfCores;                 ///< Total Number Of Cores
  UINT32            CurrentSocketNum;                   ///< Current Socket Number
  UINT32            CurrentCoreNum;                     ///< Current Core Number
  UINT32            CurrentCoreApicId;                  ///< Current Core Apic ID
  UINT32            CurrentLogicalCpuId;                ///< Current Logical CPU ID
} SYSTEM_INFO;

/// WARM_RESET
typedef struct _WARM_RESET_REQUEST {
  UINT8             RequestBit:1;                       ///< Request Bit
  UINT8             StateBits:2;                        ///< State Bits
  UINT8             Reserved:(8-3);                     ///< Reserved
} WARM_RESET_REQUEST;
/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

VOID
GetCurrentNodeNum (
     OUT   UINT32 *Node,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Get the current Platform's number of Sockets, regardless of how many are populated.
 *
 */
UINT32
GetPlatformNumberOfSockets (VOID);

/**
 * Get the number of Modules to check presence in each Processor.
 *
 */
UINT32
GetPlatformNumberOfModules (VOID);

BOOLEAN
IsProcessorPresent (
  IN       UINT32            Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * For a specific Node, get its Socket and Module ids.
 *
 */
BOOLEAN
GetSocketModuleOfNode (
  IN       UINT32    Node,
     OUT   UINT32    *Socket,
     OUT   UINT32    *Module,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Get the current core's Processor APIC Index.
 */
UINT32
GetProcessorApicIndex (
  IN       UINT32            Node,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 *  Writes to all nodes on the executing core's socket.
 *
 */
VOID
ModifyCurrentSocketPci (
  IN       PCI_ADDR          *PciAddress,
  IN       UINT32            Mask,
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * Provide the number of installed processors (not Nodes! and not Sockets!)
 */
UINT32
GetNumberOfProcessors (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetActiveCoresInCurrentSocket (
     OUT   UINT32 *CoreCount,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetActiveCoresInGivenSocket (
  IN       UINT32             Socket,
     OUT   UINT32             *CoreCount,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINTN
GetActiveCoresInCurrentModule (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN
GetGivenModuleCoreRange (
  IN       UINT32            Socket,
  IN       UINT32            Module,
     OUT   UINT32            *LowCore,
     OUT   UINT32            *HighCore,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetCurrentCore (
     OUT   UINT32 *Core,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetCurrentNodeAndCore (
     OUT   UINT32 *Node,
     OUT   UINT32 *Core,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
IsCurrentCorePrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetApMailbox (
     OUT   UINT32               *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
CacheApMailbox (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

UINTN
GetSystemDegree (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

BOOLEAN
GetNodeId (
  IN       UINT32  SocketId,
  IN       UINT32  ModuleId,
     OUT   UINT8   *NodeId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
WaitMicroseconds (
  IN       UINT32 Microseconds,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
SetWarmResetFlag (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       WARM_RESET_REQUEST *Request
  );

VOID
GetWarmResetFlag (
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   WARM_RESET_REQUEST *Request
  );

BOOLEAN
IsWarmReset (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
CheckBistStatus (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif  // _CPU_SERVICES_H_
