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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
  BitMapMapping,                              ///< Currently not supported by any family, arbitrary core
                                              ///< to compute unit mapping.
  MaxComputeUnitMapping                       ///< Not a mapping, use for limit check.
} COMPUTE_UNIT_MAPPING;

/**
 * Core Pair Map entry.
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
  COMPUTE_UNIT_MAPPING Mapping;               ///< When the processor module matches these values, use this mapping method.
} CORE_PAIR_MAP;

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
IsCorePairPrimary (
  IN       COMPUTE_UNIT_PRIMARY_SELECTOR Selector,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

/**
 * Are the two specified cores shared in a compute unit?
 */
BOOLEAN
AreCoresPaired (
  IN       UINT32  Socket,
  IN       UINT32  Module,
  IN       UINT32  CoreA,
  IN       UINT32  CoreB,
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

VOID
SetWarmResetAtEarly (
  IN      UINT32            Data,
  IN      AMD_CONFIG_PARAMS *StdHeader
);

#endif  // _CPU_SERVICES_H_
