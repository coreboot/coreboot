/* $NoKeywords:$ */
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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_SERVICES_H_
#define _CPU_SERVICES_H_

#include "Topology.h"

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

/**
 * The role of primary core for each compute unit can be relative to the cores' launch order.
 *
 * One core of a compute unit is always given the role as primary.  In different feature algorithms
 * the core performing the primary core role can be designated relative to compute order.  In most cases,
 * the primary core is the first core of a compute unit to execute.  However, in some cases the primary core
 * role is associated with the last core to execute.
 *
 * If the launch order is strictly ascending, then first core is the lowest number and last core is highest.
 * But if the launch order is not ascending, the first and last core follow the launch order, not the numbering order.
 *
 * Note that for compute units with only one core (AllCoresMapping), that core is primary for both orderings.
 * (This includes processors without hardware compute units.)
 *
 */
typedef enum {
  FirstCoreIsComputeUnitPrimary,                   ///< the primary core role associates with the first core.
  LastCoreIsComputeUnitPrimary,                    ///< the primary core role associates with the last core.
  MaxComputeUnitPrimarySelector,                   ///< limit check.
} COMPUTE_UNIT_PRIMARY_SELECTOR;

/**
 * The supported Core to Compute unit mappings.
 */
typedef enum {
  AllCoresMapping,                            ///< All Cores are primary cores
  EvenCoresMapping,                           ///< Compute units are even/odd core pairs.
  TripleCoresMapping,                         ///< Compute units has three cores enabled.
  QuadCoresMapping,                           ///< Compute units has four cores enabled.
  BitMapMapping,                              ///< Currently not supported by any family, arbitrary core
                                              ///< to compute unit mapping.
  MaxComputeUnitMapping                       ///< Not a mapping, use for limit check.
} COMPUTE_UNIT_MAPPING;

/**
 * Compute unit status register.
 */

/**
 * Compute Unit Map entry.
 * Provide for interpreting the core pairing for the processor's compute units.
 *
 * HT_LIST_TERMINAL as an Enabled value means the end of a list of map structs.
 * Zero as an Enabled value implies Compute Units are not supported by the processor
 * and the mapping is assumed to be AllCoresMapping.
 *
 */
typedef struct {
  UINT8 Enabled;                              ///< The value of the Enabled Compute Units
  UINT8 DualCore;                             ///< The value of the Dual Core Compute Units
  UINT8 TripleCore;                           ///< the value of the Triple Core Compute Units
  UINT8 QuadCore;                             ///< the value of the Quad Core Compute Units
  COMPUTE_UNIT_MAPPING Mapping;               ///< When the processor module matches these values, use this mapping method.
} COMPUTE_UNIT_MAP;

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

/// WARM_RESET_REQUEST
typedef struct _WARM_RESET_REQUEST {
  UINT8             RequestBit:1;                       ///< Request Bit
  UINT8             StateBits:2;                        ///< State Bits
  UINT8             PostStage:2;                        ///< Post Stage
  UINT8             Reserved:(8 - 5);                   ///< Reserved
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
GetPlatformNumberOfSockets ( VOID );

/**
 * Get the number of Modules to check presence in each Processor.
 *
 */
UINT32
GetPlatformNumberOfModules ( VOID );

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

UINTN
GetNumberOfCompUnitsInCurrentModule (
  IN       AMD_CONFIG_PARAMS *StdHeader
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

VOID
CacheBspMailbox (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AP_MAILBOXES         *ApMailboxes,
  IN       UINT8                NumberOfNodes
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

/**
 * Get the compute unit mapping algorithm.
 */
COMPUTE_UNIT_MAPPING
GetComputeUnitMapping (
  IN      AMD_CONFIG_PARAMS    *StdHeader
  );

/**
 * Does the current core have the role of primary core for the compute unit?
 */
BOOLEAN
IsCoreComputeUnitPrimary (
  IN       COMPUTE_UNIT_PRIMARY_SELECTOR Selector,
  IN       AMD_CONFIG_PARAMS    *StdHeader
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

VOID
SetWarmResetAtEarly (
  IN      UINT32            Data,
  IN      AMD_CONFIG_PARAMS *StdHeader
);

#ifndef CPU_DEADLOOP
  #define CPU_DEADLOOP()    { volatile UINTN __i; __i = 1; while (__i); }
#endif

#endif  // _CPU_SERVICES_H_
