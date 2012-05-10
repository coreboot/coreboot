/* $NoKeywords:$ */
/**
 * @file
 *
 * Construct a northbridge interface for a Node.
 *
 * Handle build options and run-time detection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44846 $   @e \$Date: 2011-01-07 13:21:05 +0800 (Fri, 07 Jan 2011) $
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
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "OptionsHt.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htNb.h"
#include "htNbCommonHardware.h"
#include "CommonReturns.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuFamRegisters.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#include "Filecode.h"

#define FILECODE PROC_HT_HTNB_FILECODE

extern OPTION_HT_CONFIGURATION OptionHtConfiguration;

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***               FAMILY/NORTHBRIDGE SPECIFIC FUNCTIONS                 ***
 ***************************************************************************/


/**
 * Initial construction data for no HT Northbridge.
 */
CONST NORTHBRIDGE ROMDATA HtFam10NbNone =
{
  1,
  (PF_WRITE_ROUTING_TABLE)CommonVoid,
  (PF_WRITE_NODEID)CommonVoid,
  (PF_READ_DEFAULT_LINK)CommonReturnZero8,
  (PF_ENABLE_ROUTING_TABLES)CommonVoid,
  (PF_DISABLE_ROUTING_TABLES)CommonVoid,
  (PF_VERIFY_LINK_IS_COHERENT)CommonReturnFalse,
  (PF_READ_TOKEN)CommonReturnZero8,
  (PF_WRITE_TOKEN)CommonVoid,
  (PF_WRITE_FULL_ROUTING_TABLE)CommonVoid,
  (PF_IS_ILLEGAL_TYPE_MIX)CommonReturnFalse,
  (PF_IS_EXCEEDED_CAPABLE)CommonReturnFalse,
  (PF_STOP_LINK)CommonVoid,
  (PF_HANDLE_SPECIAL_LINK_CASE)CommonReturnFalse,
  (PF_HANDLE_SPECIAL_NODE_CASE)CommonReturnFalse,
  (PF_READ_SB_LINK)CommonReturnZero8,
  (PF_VERIFY_LINK_IS_NON_COHERENT)CommonReturnFalse,
  (PF_SET_CONFIG_ADDR_MAP)CommonVoid,
  (PF_NORTH_BRIDGE_FREQ_MASK)CommonReturnZero32,
  (PF_GATHER_LINK_FEATURES)CommonVoid,
  (PF_SET_LINK_REGANG)CommonVoid,
  (PF_SET_LINK_FREQUENCY)CommonVoid,
  (PF_SET_LINK_UNITID_CLUMPING)CommonVoid,
  (PF_WRITE_TRAFFIC_DISTRIBUTION)CommonVoid,
  (PF_WRITE_LINK_PAIR_DISTRIBUTION)CommonVoid,
  (PF_WRITE_VICTIM_DISTRIBUTION)CommonVoid,
  (PF_BUFFER_OPTIMIZATIONS)CommonVoid,
  (PF_GET_NUM_CORES_ON_NODE)CommonReturnZero8,
  (PF_SET_TOTAL_NODES_AND_CORES)CommonVoid,
  (PF_GET_NODE_COUNT)CommonReturnZero8,
  (PF_LIMIT_NODES)CommonVoid,
  (PF_READ_TRUE_LINK_FAIL_STATUS)CommonReturnFalse,
  (PF_GET_NEXT_LINK)CommonReturnZero32,
  (PF_GET_PACKAGE_LINK)CommonReturnZero8,
  (PF_MAKE_LINK_BASE)CommonReturnZero32,
  (PF_GET_MODULE_INFO)CommonVoid,
  (PF_POST_MAILBOX)CommonVoid,
  (PF_RETRIEVE_MAILBOX)CommonReturnZero32,
  (PF_GET_SOCKET)CommonReturnZero8,
  (PF_GET_ENABLED_COMPUTE_UNITS)CommonReturnZero8,
  (PF_GET_DUALCORE_COMPUTE_UNITS)CommonReturnZero8,
  0,
  0,
  0,
  TRUE,
  TRUE,
  0,
  NULL,
  0,
  NULL,
  (PF_MAKE_KEY)CommonReturnZero64,
  NULL
};

/*----------------------------------------------------------------------------------------*/
/**
 * Make a compatibility key.
 *
 * @HtNbMethod{::F_MAKE_KEY}
 *
 * Private routine to northbridge code.
 * Create a key which can be used to determine whether a Node is compatible with
 * the discovered configuration so far.  Currently, that means the family,
 * extended family of the new Node are the same as the BSP's.  Family specific
 * implementations can add whatever else is necessary.
 *
 * @param[in] Node the Node
 * @param[in] Nb   this northbridge
 *
 * @return the key
 */
UINT64
MakeKey (
  IN       UINT8 Node,
  IN       NORTHBRIDGE *Nb
  )
{
  CPU_LOGICAL_ID LogicalId;
  UINT32 RawCpuId;
  PCI_ADDR Reg;

  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CPUID_3XFC);

  LibAmdPciReadBits (Reg, 31, 0, &RawCpuId, Nb->ConfigHandle);
  GetLogicalIdFromCpuid (RawCpuId, &LogicalId, Nb->ConfigHandle);
  return LogicalId.Family;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Construct a new northbridge.
 *
 * This routine encapsulates knowledge of how to tell significant differences between
 * families of supported northbridges and what routines can be used in common and
 * which are unique.  A fully populated northbridge interface is provided by Nb.
 *
 * @param[in]     Node    create a northbridge interface for this Node.
 * @param[in]     State   global state
 * @param[out]    Nb      the caller's northbridge structure to initialize.
 */
VOID
NewNorthBridge (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
     OUT   NORTHBRIDGE *Nb
  )
{
  CPU_LOGICAL_ID LogicalId;
  UINT64  Match;
  UINT32 RawCpuId;
  PCI_ADDR Reg;
  NORTHBRIDGE **InitializerInstance;

  // Start with enough of the key to identify the northbridge interface
  Reg.AddressValue = MAKE_SBDFO (MakePciSegmentFromNode (Node),
                                 MakePciBusFromNode (Node),
                                 MakePciDeviceFromNode (Node),
                                 CPU_NB_FUNC_03,
                                 REG_NB_CPUID_3XFC);
  LibAmdPciReadBits (Reg, 31, 0, &RawCpuId, State->ConfigHandle);
  IDS_HDT_CONSOLE (HT_TRACE, "AMD Processor at Node %d has raw CPUID=%x.\n", Node, RawCpuId);
  GetLogicalIdFromCpuid (RawCpuId, &LogicalId, State->ConfigHandle);
  Match = LogicalId.Family;

  // Test each Northbridge interface in turn looking for a match.
  // Use it to Init the Nb struct if a match is found.
  //
  ASSERT (OptionHtConfiguration.HtOptionFamilyNorthbridgeList != NULL);
  InitializerInstance = (NORTHBRIDGE **) (OptionHtConfiguration.HtOptionFamilyNorthbridgeList);
  while (*InitializerInstance != NULL) {
    if ((Match & (*InitializerInstance)->CompatibleKey) != 0) {
      LibAmdMemCopy ((VOID *)Nb, (VOID *)*InitializerInstance, (UINT32) sizeof (NORTHBRIDGE), State->ConfigHandle);
      break;
    }
    InitializerInstance++;
  }
  // There must be an available northbridge implementation.
  ASSERT (*InitializerInstance != NULL);

  // Set the config handle for passing to the library.
  Nb->ConfigHandle = State->ConfigHandle;
}

