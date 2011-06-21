/* $NoKeywords:$ */
/**
 * @file
 *
 * HyperTransport features and sequence implementation.
 *
 * Implements the external AmdHtInitialize entry point.
 * Contains routines for directing the sequence of available features.
 * Mostly, but not exclusively, AGESA_TESTPOINT invocations should be
 * contained in this file, and not in the feature code.
 *
 * From a build option perspective, it may be that a few lines could be removed
 * from compilation in this file for certain options.  It is considered that
 * the code savings from this are too small to be of concern and this file
 * should not have any explicit build option implementation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35978 $   @e \$Date: 2010-08-07 02:18:50 +0800 (Sat, 07 Aug 2010) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNb.h"
#include "heapManager.h"
#include "cpuServices.h"
#include "OptionsHt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_HTMAIN_FILECODE
#define APIC_Base_BSP    8
#define APIC_Base        0x1b

extern OPTION_HT_CONFIGURATION OptionHtConfiguration;

BOOLEAN
STATIC
IsBootCore (
  IN       STATE_DATA    *State
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Update maps with the core range for each module.
 *
 * Cores are numbered relative to a Processor, but sometimes there is a need to know the
 * starting and ending core ids on a particular node.  This same info is also useful for
 * supporting the Core count on a node other than the one currently executing.
 *
 * For each Processor, get the core count of each node using the family specific PCI core count
 * interface. The order of cores in a processor, and whether it is special for the BSP is family
 * specific.  But whether the processor orders core ids by module or node, iterate in the right
 * order and use the counts to determine each start and end range.
 *
 * Update compute unit status for each node.
 *
 * @param[in]   State    number of Nodes discovered.
*/
VOID
STATIC
UpdateCoreRanges (
  IN       STATE_DATA    *State
    )
{
  UINT8 Node;
  UINT8 ProcessorCores;
  UINT8 ModuleCoreCount[MAX_DIES];
  UINT8 Socket;
  UINT8 Module;

  ASSERT (State->SocketDieToNodeMap != NULL);
  ASSERT (State->NodeToSocketDieMap != NULL);

  for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
    // Is a Processor present in Socket?
    if ((*State->SocketDieToNodeMap)[Socket][0].Node != HT_LIST_TERMINAL) {
      // Get all the Module core counts for this processor
      // Note that the core counts are 1 based counts.
      // Since Compute Unit info is not module ordering dependent, write it now.
      for (Module = 0; Module < MAX_DIES; Module++) {
        if ((*State->SocketDieToNodeMap)[Socket][Module].Node != HT_LIST_TERMINAL) {
          ModuleCoreCount[Module] = State->Nb->GetNumCoresOnNode ((*State->SocketDieToNodeMap)[Socket][Module].Node, State->Nb);
          (*State->SocketDieToNodeMap)[Socket][Module].EnabledComputeUnits =
            State->Nb->GetEnabledComputeUnits ((*State->SocketDieToNodeMap)[Socket][Module].Node, State->Nb);
          (*State->SocketDieToNodeMap)[Socket][Module].DualCoreComputeUnits =
            State->Nb->GetDualCoreComputeUnits ((*State->SocketDieToNodeMap)[Socket][Module].Node, State->Nb);
        } else {
          ModuleCoreCount[Module] = 0;
        }
      }
      // Determine the core ordering rule for this processor.
      if ((((*State->NodeToSocketDieMap)[0].Socket == Socket) && State->Nb->IsOrderBSPCoresByNode) ||
          (!State->Nb->IsOrderCoresByModule)) {
        // Order core ranges on this processor by Node Id.
        ProcessorCores = 0;
        for (Node = 0; Node < State->Nb->GetNodeCount (State->Nb); Node++) {
          // Is this node a module in this processor?
          if ((*State->NodeToSocketDieMap)[Node].Socket == Socket) {
            Module = (*State->NodeToSocketDieMap)[Node].Die;
            if (ModuleCoreCount[Module] != 0) {
              (*State->SocketDieToNodeMap)[Socket][Module].LowCore = ProcessorCores;
              (*State->SocketDieToNodeMap)[Socket][Module].HighCore = ProcessorCores + (ModuleCoreCount[Module] - 1);
              IDS_HDT_CONSOLE (
                HT_TRACE,
                (IsBootCore (State) ?
                 "Topology: Socket %d, Die %d, is Node %d, with Cores %d thru %d. Compute Unit status (0x%x,0x%x).\n" :
                 ""),
                Socket,
                Module,
                Node,
                (*State->SocketDieToNodeMap)[Socket][Module].LowCore,
                (*State->SocketDieToNodeMap)[Socket][Module].HighCore,
                (*State->SocketDieToNodeMap)[Socket][Module].EnabledComputeUnits,
                (*State->SocketDieToNodeMap)[Socket][Module].DualCoreComputeUnits
                );
              ProcessorCores = ProcessorCores + ModuleCoreCount[Module];
            }
          }
        }
      } else {
        // Order core ranges in this processor by Module Id.
        ProcessorCores = 0;
        for (Module = 0; Module < MAX_DIES; Module++) {
          if (ModuleCoreCount[Module] != 0) {
            (*State->SocketDieToNodeMap)[Socket][Module].LowCore = ProcessorCores;
            (*State->SocketDieToNodeMap)[Socket][Module].HighCore = ProcessorCores + (ModuleCoreCount[Module] - 1);
              IDS_HDT_CONSOLE (
                HT_TRACE,
                (IsBootCore (State) ?
                 "Topology: Socket %d, Die %d, is Node %d, with Cores %d thru %d. Compute Unit status (0x%x,0x%x).\n" :
                 ""),
                Socket,
                Module,
                (*State->SocketDieToNodeMap)[Socket][Module].Node,
                (*State->SocketDieToNodeMap)[Socket][Module].LowCore,
                (*State->SocketDieToNodeMap)[Socket][Module].HighCore,
                (*State->SocketDieToNodeMap)[Socket][Module].EnabledComputeUnits,
                (*State->SocketDieToNodeMap)[Socket][Module].DualCoreComputeUnits
                );
            ProcessorCores = ProcessorCores + ModuleCoreCount[Module];
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Complete the coherent init with any system level initialization.
 *
 * Find the total number of cores and update the number of Nodes and cores in all cpus.
 * Limit cpu config access to installed cpus.
 *
 * @param[in]   State    number of Nodes discovered.
*/
VOID
STATIC
FinalizeCoherentInit (
  IN       STATE_DATA    *State
    )
{
  UINT8 Node;
  UINT8 TotalCores;

  TotalCores = 0;

  for (Node = 0; Node < (State->NodesDiscovered + 1); Node++) {
    TotalCores = TotalCores + State->Nb->GetNumCoresOnNode (Node, State->Nb);
  }

  for (Node = 0; Node < (State->NodesDiscovered + 1); Node++) {
    State->Nb->SetTotalNodesAndCores (Node, State->NodesDiscovered + 1, TotalCores, State->Nb);
  }

  // Set all nodes to limit config space based on node count, after all nodes have a valid count.
  // (just being cautious, probably we could combine the loops.)
  for (Node = 0; Node < (State->NodesDiscovered + 1); Node++) {
    State->Nb->LimitNodes (Node, State->Nb);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize the coherent fabric.
 *
 * Perform discovery and initialization of the coherent fabric, for builds including
 * support for multiple coherent nodes.
 *
 * @param[in]   State   global state
 */
VOID
STATIC
CoherentInit (
  IN OUT   STATE_DATA    *State
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 ModuleType;
  UINT8 Module;
  UINT8 HardwareSocket;
  COHERENT_FABRIC Fabric;

  // Because Node 0, the BSP, is not discovered, initialize info about it specially here.
  // Allocate Socket Die Map.
  // While the BSP is always capable of being the only processor in the system, call the
  // IsExceededCapable method to make sure the BSP's capability is included in the aggregate system
  // capability. We don't care to check the return value.
  //
  State->Fabric = &Fabric;
  State->NodesDiscovered = 0;
  State->TotalLinks = 0;
  State->SysMpCap = MAX_NODES;
  State->Nb->IsExceededCapable (0, State, State->Nb);
  HardwareSocket = State->Nb->GetSocket (0, 0, State->Nb);
  ModuleType = 0;
  Module = 0;
  State->Nb->GetModuleInfo (0, &ModuleType, &Module, State->Nb);
  // No predecessor info for BSP, so pass 0xFF for those parameters.
  State->HtInterface->SetNodeToSocketMap (0xFF, 0xFF, 0xFF, 0, HardwareSocket, Module, State);

  // Initialize system state data structures
  for (i = 0; i < MAX_NODES; i++) {
    State->Fabric->SysDegree[i] = 0;
    for (j = 0; j < MAX_NODES; j++) {
      State->Fabric->SysMatrix[i][j] = 0;
    }
  }

  //
  // Call the coherent init features
  //

  // Discovery
  State->HtFeatures->CoherentDiscovery (State);
  State->HtInterface->PostMapToAp (State);
  // Topology matching and Routing
  AGESA_TESTPOINT (TpProcHtTopology, State->ConfigHandle);
  State->HtFeatures->LookupComputeAndLoadRoutingTables (State);
  State->HtFeatures->MakeHopCountTable (State);

  // UpdateCoreRanges requires the other maps to be initialized, and the node count set.
  FinalizeCoherentInit (State);
  UpdateCoreRanges (State);
  State->Fabric = NULL;
}

/***************************************************************************
 ***                       Non-coherent init code                        ***
 ***                             Algorithms                              ***
 ***************************************************************************/
/*----------------------------------------------------------------------------------------*/
/**
 * Initialize the non-coherent fabric.
 *
 * Begin with the Compat Link on the BSP, then find and initialize all other
 * non-coherent chains.
 *
 * @param[in]   State    our global state
 */
VOID
STATIC
NcInit (
  IN       STATE_DATA    *State
  )
{
  UINT8 Node;
  UINT8 Link;
  UINT8 CompatLink;
  FINAL_LINK_STATE FinalLinkState;

  // Initialize the southbridge chain.
  State->AutoBusCurrent = State->HtBlock->AutoBusStart;
  State->UsedCfgMapEntries = 0;
  CompatLink = State->Nb->ReadSouthbridgeLink (State->Nb);
  State->HtFeatures->ProcessLink (0, CompatLink, TRUE, State);

  // Find and initialize all other non-coherent chains.
  for (Node = 0; Node <= State->NodesDiscovered; Node++) {
    for (Link = 0; Link < State->Nb->MaxLinks; Link++) {
      // Skip the Link, if any of these tests indicate
      FinalLinkState = State->HtInterface->GetIgnoreLink (Node, Link, State->Nb->DefaultIgnoreLinkList, State);
      if (FinalLinkState == UNMATCHED) {
        if ( !((Node == 0) && (Link == CompatLink))) {
          if ( !(State->Nb->ReadTrueLinkFailStatus (Node, Link, State, State->Nb))) {
            if (State->Nb->VerifyLinkIsNonCoherent (Node, Link, State->Nb)) {
              State->HtFeatures->ProcessLink (Node, Link, FALSE, State);
            }
          }
        }
      }
    }
  }
}

/***************************************************************************
 ***                            Link Optimization                        ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Optimize Link Features.
 *
 * Based on Link capabilities, apply optimization rules to come up with the best
 * settings, including several external limit decision from the interface. This includes
 * handling of subLinks.  Finally, after the port list data is updated, set the hardware
 * state for all Links.
 *
 * @param[in] State our global state
 */
VOID
STATIC
LinkOptimization (
  IN       STATE_DATA *State
  )
{
  AGESA_TESTPOINT (TpProcHtOptGather, State->ConfigHandle);
  State->HtFeatures->GatherLinkData (State);

  AGESA_TESTPOINT (TpProcHtOptRegang, State->ConfigHandle);
  State->HtFeatures->RegangLinks (State);

  AGESA_TESTPOINT (TpProcHtOptLinks, State->ConfigHandle);
  State->HtFeatures->SelectOptimalWidthAndFrequency (State);

  // A likely cause of mixed Retry settings on coherent links is sublink ratio balancing
  // so check this after doing the sublinks.
  AGESA_TESTPOINT (TpProcHtOptSubLinks, State->ConfigHandle);
  State->HtFeatures->SubLinkRatioFixup (State);
  if (State->HtFeatures->IsCoherentRetryFixup (State)) {
    // Fix sublinks again within HT1 only frequencies, as ratios may be invalid again.
    State->HtFeatures->SubLinkRatioFixup (State);
  }

  AGESA_TESTPOINT (TpProcHtOptFinish, State->ConfigHandle);
  State->HtFeatures->SetLinkData (State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Handle system and performance tunings.
 *
 * Including traffic distribution, fifo and
 * buffer tuning that can't be placed in the register table,
 * and special config tunings.
 *
 * @param[in] State    Total Nodes, port list data
 */
VOID
STATIC
Tuning (
  IN       STATE_DATA *State
  )
{
  UINT8 Node;

  // See if traffic distribution can be done and do it if so.
  //
  AGESA_TESTPOINT (TpProcHtTrafficDist, State->ConfigHandle);
  State->HtFeatures->TrafficDistribution (State);

  // For each Node, invoke northbridge specific buffer tunings that can not be done in reg table.
  //
  AGESA_TESTPOINT (TpProcHtTuning, State->ConfigHandle);
  for (Node = 0; Node < (State->NodesDiscovered + 1); Node++) {
    State->Nb->BufferOptimizations (Node, State, State->Nb);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize the Node and Socket maps for an AP Core.
 *
 * In each core's local heap, create a Node to Socket map and a Socket/Module to Node map.
 * The mapping is filled in by reading the AP Mailboxes from PCI config on each node.
 *
 * @param[in]    State    global state, input data
 *
 */
VOID
STATIC
InitApMaps (
  IN       STATE_DATA *State
  )
{
  UINT8 Node;
  AP_MAIL_INFO NodeApMailBox;

  // There is no option to not have socket - node maps, if they aren't allocated that is a fatal bug.
  ASSERT (State->SocketDieToNodeMap != NULL);
  ASSERT (State->NodeToSocketDieMap != NULL);

  for (Node = 0; Node < State->Nb->GetNodeCount (State->Nb); Node++) {
    NodeApMailBox = State->Nb->RetrieveMailbox (Node, State->Nb);
    (*State->SocketDieToNodeMap)[NodeApMailBox.Fields.Socket][NodeApMailBox.Fields.Module].Node = Node;
    (*State->NodeToSocketDieMap)[Node].Socket = (UINT8)NodeApMailBox.Fields.Socket;
    (*State->NodeToSocketDieMap)[Node].Die = (UINT8)NodeApMailBox.Fields.Module;
  }
  // This requires the other maps to be initialized.
  UpdateCoreRanges (State);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Is the currently running core the BSC?
 *
 * Determine whether the init steps for BSC or AP core should be run.
 *
 * @param[in]    State    global state, input data
 *
 * @retval       TRUE    This is the boot core.
 * @retval       FALSE   This is not the boot core.
 */
BOOLEAN
STATIC
IsBootCore (
  IN       STATE_DATA    *State
  )
{
  UINT64 Value;

  LibAmdMsrRead (APIC_Base, &Value, State->ConfigHandle);

  return ((BOOLEAN) (((UINT32) (Value & 0xFFFFFFFF) & ((UINT32)1 << APIC_Base_BSP)) != 0));
}

/***************************************************************************
 ***                            HT Initialize                             ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * The top level external interface for Hypertransport Initialization.
 *
 * Create our initial internal state, initialize the coherent fabric,
 * initialize the non-coherent chains, and perform any required fabric tuning or
 * optimization.
 *
 * @param[in]   StdHeader              Opaque handle to standard config header
 * @param[in]   PlatformConfiguration  The platform configuration options.
 * @param[in]   AmdHtInterface         HT Interface structure.
 *
 * @retval      AGESA_SUCCESS     Only information events logged.
 * @retval      AGESA_ALERT       Sync Flood or CRC error logged.
 * @retval      AGESA_WARNING     Example: expected capability not found
 * @retval      AGESA_ERROR       logged events indicating some devices may not be available
 * @retval      AGESA_FATAL       Mixed Family or MP capability mismatch
 *
 */
AGESA_STATUS
AmdHtInitialize (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfiguration,
  IN       AMD_HT_INTERFACE       *AmdHtInterface
  )
{
  STATE_DATA State;
  NORTHBRIDGE Nb;
  HT_FEATURES HtFeatures;
  HT_INTERFACE HtInterface;
  AGESA_STATUS DeallocateStatus;
  AP_MAIL_INFO ApMailboxInfo;
  UINT8 ApNode;

  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  State.HtBlock = AmdHtInterface;
  State.ConfigHandle = StdHeader;
  State.PlatformConfiguration = PlatformConfiguration;

  // Get the current HT internal interface (to HtBlock data)
  NewHtInterface (&HtInterface, State.ConfigHandle);
  State.HtInterface = &HtInterface;

  // Get the current HT Feature Set
  NewHtFeatures (&HtFeatures, State.ConfigHandle);
  State.HtFeatures = &HtFeatures;

  // Initialize from static options
  State.IsUsingRecoveryHt = OptionHtConfiguration.IsUsingRecoveryHt;
  State.IsSetHtCrcFlood = OptionHtConfiguration.IsSetHtCrcFlood;
  State.IsUsingUnitIdClumping = OptionHtConfiguration.IsUsingUnitIdClumping;

  // Initialize for status and event output
  State.MaxEventClass = AGESA_SUCCESS;

  // Allocate permanent heap structs that are interfaces to other AGESA services.
  State.HtInterface->NewNodeAndSocketTables (&State);

  if (IsBootCore (&State)) {
    AGESA_TESTPOINT (TpProcHtEntry, State.ConfigHandle);
    // Allocate Bsp only interface heap structs.
    State.HtInterface->NewHopCountTable (&State);
    // Allocate heap for our temporary working space.
    AllocHeapParams.RequestedBufferSize = (sizeof (PORT_DESCRIPTOR) * (MAX_PLATFORM_LINKS * 2));
    AllocHeapParams.BufferHandle = HT_STATE_DATA_HANDLE;
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    if (HeapAllocateBuffer (&AllocHeapParams, State.ConfigHandle) == AGESA_SUCCESS) {
      State.PortList = (PORT_LIST)AllocHeapParams.BufferPtr;
      // Create the BSP's northbridge.
      NewNorthBridge (0, &State, &Nb);
      State.Nb = &Nb;

      CoherentInit (&State);
      NcInit (&State);
      LinkOptimization (&State);
      Tuning (&State);

      DeallocateStatus = HeapDeallocateBuffer (HT_STATE_DATA_HANDLE, State.ConfigHandle);
      ASSERT (DeallocateStatus == AGESA_SUCCESS);
      AGESA_TESTPOINT (TpProcHtDone, State.ConfigHandle);
    } else {
      ASSERT (FALSE);
      State.MaxEventClass = AGESA_ERROR;
      // Cannot Log entry due to heap allocate failed.
    }
  } else {
    // Do the AP HT Init, which produces Node and Socket Maps for the AP's use.
    AGESA_TESTPOINT (TpProcHtApMapEntry, State.ConfigHandle);
    GetApMailbox (&ApMailboxInfo.Info, State.ConfigHandle);
    ASSERT (ApMailboxInfo.Fields.Node < MAX_NODES);
    ApNode = (UINT8)ApMailboxInfo.Fields.Node;
    NewNorthBridge (ApNode, &State, &Nb);
    State.Nb = &Nb;
    InitApMaps (&State);
    AGESA_TESTPOINT (TpProcHtApMapDone, State.ConfigHandle);
  }
  return State.MaxEventClass;
}
