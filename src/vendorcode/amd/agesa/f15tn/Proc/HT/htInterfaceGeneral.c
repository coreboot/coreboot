/* $NoKeywords:$ */
/**
 * @file
 *
 * External Interface implementation, general purpose features.
 *
 * Contains routines for implementing the interface to the client BIOS.  This file
 * includes the interface support which is not removed with various build options.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 *****************************************************************************
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
#include "OptionMultiSocket.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htInterfaceGeneral.h"
#include "htNb.h"
#include "cpuServices.h"
#include "cpuFeatures.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_HT_HTINTERFACEGENERAL_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Is PackageLink an Internal Link?
 *
 * This is a test for the logical link match codes in the user interface, not a test for
 * the actual northbridge links.
 *
 * @param[in]    PackageLink   The link
 *
 * @retval       TRUE          This is an internal link
 * @retval       FALSE         This is not an internal link
 */
BOOLEAN
IsPackageLinkInternal (
  IN       UINT8  PackageLink
  )
{
  return (BOOLEAN) ((PackageLink <= HT_LIST_MATCH_INTERNAL_LINK_2) && (PackageLink >= HT_LIST_MATCH_INTERNAL_LINK_0));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Ignore a Link.
 *
 * @HtInterfaceMethod{::F_GET_IGNORE_LINK}
 *
 * This routine is called every time a coherent Link is found and then every time a
 * non-coherent Link from a CPU is found.  Any coherent or non-coherent Link from a
 * CPU can be ignored and not used for discovery or initialization.  Useful for
 * connection based systems.
 *
 * @note not called for IO device to IO Device Links.
 *
 * @param[in]     Node             The Node on which this Link is located
 * @param[in]     Link             The Link about to be initialized
 * @param[in]     NbIgnoreLinkList The northbridge default ignore link list
 * @param[in]     State            the input data
 *
 * @retval        MATCHED     ignore this Link and skip it
 * @retval        POWERED_OFF ignore this link and power it off.
 * @retval        UNMATCHED   initialize the Link normally
 */
FINAL_LINK_STATE
GetIgnoreLink (
  IN       UINT8            Node,
  IN       UINT8            Link,
  IN       IGNORE_LINK     *NbIgnoreLinkList,
  IN       STATE_DATA      *State
  )
{
  IGNORE_LINK *p;
  FINAL_LINK_STATE Result;
  BOOLEAN IsFound;
  UINT8 Socket;
  UINT8 PackageLink;

  ASSERT ((Node < MAX_NODES) && (Link < MAX_NODES));

  Result = UNMATCHED;
  IsFound = FALSE;
  Socket = State->HtInterface->GetSocketFromMap (Node, State);
  PackageLink = State->Nb->GetPackageLink (Node, Link, State->Nb);

  if (State->HtBlock->IgnoreLinkList != NULL) {
    p = State->HtBlock->IgnoreLinkList;
    while (p->Socket != HT_LIST_TERMINAL) {
      if (((p->Socket == Socket) || (p->Socket == HT_LIST_MATCH_ANY)) &&
          ((p->Link == PackageLink) ||
           ((p->Link == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLink))) ||
           ((p->Link == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLink))))) {
        // Found a match return the desired link state.
        ASSERT (Result < MaxFinalLinkState);
        Result = p->LinkState;
        IsFound = TRUE;
        break;
      } else {
        p++;
      }
    }
  }
  // If there wasn't a match in the user interface, see if the northbridge provides one.
  if (!IsFound && (NbIgnoreLinkList != NULL)) {
    p = NbIgnoreLinkList;
    while (p->Socket != HT_LIST_TERMINAL) {
      if (((p->Socket == Socket) || (p->Socket == HT_LIST_MATCH_ANY)) &&
          ((p->Link == PackageLink) ||
           ((p->Link == HT_LIST_MATCH_ANY) && (!IsPackageLinkInternal (PackageLink))) ||
           ((p->Link == HT_LIST_MATCH_INTERNAL_LINK) && (IsPackageLinkInternal (PackageLink))))) {
        // Found a match return the desired link state.
        ASSERT (Result < MaxFinalLinkState);
        Result = p->LinkState;
        break;
      } else {
        p++;
      }
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the Socket number for a given Node number.
 *
 * @HtInterfaceMethod{::F_GET_SOCKET_FROM_MAP}
 *
 * Return the id.
 *
 * @param[in]     Node    The Node to translate
 * @param[in]     State   reference to Node to socket map
 *
 * @return the socket id
 *
 */
UINT8
GetSocketFromMap (
  IN       UINT8         Node,
  IN       STATE_DATA    *State
  )
{
  UINT8 Socket;

  ASSERT (State->NodeToSocketDieMap != NULL);

  Socket = (*State->NodeToSocketDieMap)[Node].Socket;
  return Socket;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get a new Socket Die to Node Map.
 *
 * @HtInterfaceMethod{::F_NEW_NODE_AND_SOCKET_TABLES}
 *
 * Put the Socket Die Table in heap with a known handle.  Content will be generated as
 * each node is discovered.
 *
 * @param[in,out] State global state
 */
VOID
NewNodeAndSocketTables (
  IN OUT   STATE_DATA *State
  )
{
  UINT8 i;
  UINT8 j;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  // Allocate heap for the table
  State->SocketDieToNodeMap = NULL;
  AllocHeapParams.RequestedBufferSize = (((MAX_SOCKETS) * (MAX_DIES)) * sizeof (SOCKET_DIE_TO_NODE_ITEM));
  AllocHeapParams.BufferHandle = SOCKET_DIE_MAP_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocHeapParams, State->ConfigHandle) == AGESA_SUCCESS) {
    State->SocketDieToNodeMap = (SOCKET_DIE_TO_NODE_MAP)AllocHeapParams.BufferPtr;
    // Initialize shared data structures
    for (i = 0; i < MAX_SOCKETS; i++) {
      for (j = 0; j < MAX_DIES; j++) {
        (*State->SocketDieToNodeMap)[i][j].Node = HT_LIST_TERMINAL;
        (*State->SocketDieToNodeMap)[i][j].LowCore = HT_LIST_TERMINAL;
        (*State->SocketDieToNodeMap)[i][j].HighCore = HT_LIST_TERMINAL;
      }
    }
  }
  // Allocate heap for the table
  State->NodeToSocketDieMap = NULL;
  AllocHeapParams.RequestedBufferSize = (MAX_NODES * sizeof (NODE_TO_SOCKET_DIE_ITEM));
  AllocHeapParams.BufferHandle = NODE_ID_MAP_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocHeapParams, State->ConfigHandle) == AGESA_SUCCESS) {
    State->NodeToSocketDieMap = (NODE_TO_SOCKET_DIE_MAP)AllocHeapParams.BufferPtr;
    // Initialize shared data structures
    for (i = 0; i < MAX_NODES; i++) {
      (*State->NodeToSocketDieMap)[i].Socket = HT_LIST_TERMINAL;
      (*State->NodeToSocketDieMap)[i].Die = HT_LIST_TERMINAL;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the minimum Northbridge frequency for the system.
 *
 * @HtInterfaceMethod{::F_GET_MIN_NB_CORE_FREQ}
 *
 * Invoke the CPU component power mgt interface.
 *
 * @param[in]    PlatformConfig   Platform profile/build option config structure.
 * @param[in]    StdHeader        Config for library and services.
 *
 * @return Frequency in MHz.
 *
 */
UINT32
GetMinNbCoreFreq (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 MinSysNbFreq;
  UINT32 MinP0NbFreq;

  OptionMultiSocketConfiguration.GetMinNbCof (PlatformConfig, &MinSysNbFreq, &MinP0NbFreq, StdHeader);

  ASSERT (MinSysNbFreq != 0);

  return MinSysNbFreq;
}

/**
 * @page physicalsockethowto Physical Socket Map, How To Create
 *
 *  To create a physical system socket map for a platform:
 *
 *  - Start at the Node which will be the BSP.
 *
 *  - Begin a breadth first enumeration of all the coherent Links between sockets
 *  by creating a socket structure for each socket connection from the BSP.
 *  For example, if the BSP is in socket zero and Link one connects to socket two,
 *  create socket {0, 1, 2}.
 *
 *  - When all Links from the BSP are described, go to the first socket connected
 *  to the BSP and continue the breadth first enumeration.
 *
 *  - It should not be necessary to describe the back Links; in the example above, there
 *  should be no need to create {2, 1, 0} (assuming socket two connects back to
 *  socket zero on its Link one).
 *
 *  - When completed:
 *
 *     - Every socket except the BSP's (usually zero) must be listed as a targetSocket,
 *     at least once.  Some sockets may be listed more than once.
 *
 *     - There usually should be at least as many entries as Links. An exception is a
 *     fully connected system, only the Links from the BSP are needed.
 *
 *     - Every socket but the last one in the breadth first order should usually have one
 *     or more entries listing it as a currentSocket. (The last one has only back Links.)
 *
 *  There are no strict assumptions about the ordering of the socket structures.
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Update maps between Sockets and Nodes for a specific newly discovered node.
 *
 * @HtInterfaceMethod{::F_SET_NODE_TO_SOCKET_MAP}
 *
 * There are two methods for providing socket naming of nodes.
 *
 * Hardware Method (preferred):  A value strapped in hardware by the board is read and
 * passed to this routine.
 *
 * Software Method:  The current node's socket is looked up, since it was
 * previously a new node and went through this process. The link is converted to
 * a package level link.  A user data structure describing the package level
 * layout of the system is searched for the current node's socket and package link,
 * and now we know the new node's socket.
 *
 * In either case, the Socket, Module to Node map and the Node to Socket, Module
 * map are updated with the new node, socket, and module.
 *
 * Data needed to do this is passed in to the routine as arguments rather than read by this routine,
 * so that it is not necessary to know a valid temporary route to either node at the time this code runs.
 *
 * @param[in] Node           Node from which a new node was discovered
 * @param[in] CurrentNodeModule  The current node's module id in it's processor.
 * @param[in] PackageLink    The package link for the current node's link.
 * @param[in] NewNode        The new node's id
 * @param[in] HardwareSocket If we use the hardware method (preferred), this is the socket of new node.
 * @param[in] Module         The new node's module id in it's processor.
 * @param[in] State          our State
 */
VOID
SetNodeToSocketMap (
  IN       UINT8 Node,
  IN       UINT8 CurrentNodeModule,
  IN       UINT8 PackageLink,
  IN       UINT8 NewNode,
  IN       UINT8 HardwareSocket,
  IN       UINT8 Module,
  IN       STATE_DATA *State
  )
{
  UINT8 SourceSocket;
  UINT8 TargetSocket;
  SYSTEM_PHYSICAL_SOCKET_MAP *Map;

  // While this code could be written to recover from a NULL socket map, AGESA cannot function without one.
  ASSERT (State->SocketDieToNodeMap != NULL);

  if (State->HtBlock->SystemPhysicalSocketMap != NULL) {
    if (NewNode != 0) {
      // Find the logical Node from which a new Node was discovered in the Node field of
      // some socket.  It must already be there, Nodes are assigned ascending.
      //
      for (SourceSocket = 0; SourceSocket < MAX_SOCKETS; SourceSocket++) {
        if ((*State->SocketDieToNodeMap)[SourceSocket][CurrentNodeModule].Node == Node) {
          break;
        }
      }
      // This ASSERT should be understood as "the Node did not have a match", not as a limit check on SourceSocket.
      ASSERT (SourceSocket != MAX_SOCKETS);

      // Find the sourceSocket in the CurrentSocket field, for the Link on which a new Node
      // was discovered.  When we find an entry with that socket and Link number, update the
      // Node for that socket.
      //
      if (IsPackageLinkInternal (PackageLink)) {
        // Internal Nodes are in the same socket, don't search the physical system map.
        TargetSocket = SourceSocket;
      } else {
        // Find the target socket in the physical system map.
        Map = State->HtBlock->SystemPhysicalSocketMap;
        while ((Map->CurrentSocket != 0xFF) &&
               ((Map->CurrentSocket != SourceSocket) || (Map->CurrentLink != PackageLink))) {
          Map++;
        }
        ASSERT (Map->CurrentSocket != 0xFF);
        TargetSocket = Map->TargetSocket;
      }
    } else {
      // The BSP (BSN, if you will) has no predecessor node from which it is discovered.
      TargetSocket = 0;
    }
  } else {
    // Use the hardware method
    // The hardware strapped socket id is passed to us in this case.
    TargetSocket = HardwareSocket;
  }
  // If the target socket, module is already mapped to something, that's not good.  Socket labeling conflict.
    // Check that the board is strapped correctly. If not you need a SystemPhysicalSocketMap.  If you have one,
    // check it for correctness.
  ASSERT ((*State->SocketDieToNodeMap)[TargetSocket][Module].Node == 0xFF);
  // Update the map for the rest of agesa
  (*State->SocketDieToNodeMap)[TargetSocket][Module].Node = NewNode;
  // and the node to socket map
  ASSERT (State->NodeToSocketDieMap != NULL);
  (*State->NodeToSocketDieMap)[NewNode].Socket = TargetSocket;
  (*State->NodeToSocketDieMap)[NewNode].Die = Module;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Clean up the map structures after severe event has caused a fall back to 1 node.
 *
 * @HtInterfaceMethod{::F_CLEAN_MAPS_AFTER_ERROR}
 *
 * @param[in] State Our state, access to socket, node maps
 *
 */
VOID
CleanMapsAfterError (
  IN       STATE_DATA *State
  )
{
  UINTN  Socket;
  UINTN  Module;
  UINTN  Node;

  ASSERT (State->NodeToSocketDieMap != NULL);
  ASSERT (State->SocketDieToNodeMap != NULL);

  // Clear all the socket, module items except for the socket and module containing node zero.
  for (Socket = 0; Socket < MAX_SOCKETS; Socket++) {
    for (Module = 0; Module < MAX_DIES; Module++) {
      if (((*State->NodeToSocketDieMap)[0].Socket != Socket) || ((*State->NodeToSocketDieMap)[0].Die != Module)) {
        (*State->SocketDieToNodeMap)[Socket][Module].Node = HT_LIST_TERMINAL;
        (*State->SocketDieToNodeMap)[Socket][Module].LowCore = HT_LIST_TERMINAL;
        (*State->SocketDieToNodeMap)[Socket][Module].HighCore = HT_LIST_TERMINAL;
      }
    }
  }
  // Clear all the node items except for node zero.
  for (Node = 1; Node < MAX_NODES; Node++) {
    (*State->NodeToSocketDieMap)[Node].Socket = HT_LIST_TERMINAL;
    (*State->NodeToSocketDieMap)[Node].Die = HT_LIST_TERMINAL;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Post Node id and other context info to AP cores via mailbox.
 *
 * @HtInterfaceMethod{::F_POST_MAP_TO_AP}
 *
 * Since Ap's can not view map until after mp communication is established,
 * provide them with initial context info via a mailbox register. A mailbox
 * register is one that can be written in PCI space and read in MSR space.
 *
 * @param[in] State Our state, access to socket, node maps
 */
VOID
PostMapToAp (
  IN       STATE_DATA *State
  )
{
  UINT8 ModuleType;
  UINT8 Module;
  AP_MAILBOXES ApMailboxes;
  UINT8 Node;
  UINT32 Degree;
  AGESA_STATUS CalledStatus;

  // Dispatch any features (such as Preserve Mailbox) that need to run as soon as discovery is completed.
  IDS_HDT_CONSOLE (CPU_TRACE, "  Dispatch CPU features after HT discovery\n");
  CalledStatus = DispatchCpuFeatures (CPU_FEAT_AFTER_COHERENT_DISCOVERY, State->PlatformConfiguration, State->ConfigHandle);

  ASSERT (State->Fabric != NULL);
  Degree = 0;
  // Compute the degree of the system by finding the maximum degree of any node.
  for (Node = 0; Node < (State->NodesDiscovered + 1); Node++) {
    if (State->Fabric->SysDegree[Node] > Degree) {
      Degree = State->Fabric->SysDegree[Node];
    }
  }
  // Post the information on all nodes.
  for (Node = 0; Node < (State->NodesDiscovered + 1); Node++) {
    ModuleType = 0;
    Module = 0;
    State->Nb->GetModuleInfo (Node, &ModuleType, &Module, State->Nb);
    ApMailboxes.ApMailInfo.Info = 0;
    ApMailboxes.ApMailInfo.Fields.Node = Node;
    ApMailboxes.ApMailInfo.Fields.Socket = State->HtInterface->GetSocketFromMap (Node, State);
    ApMailboxes.ApMailInfo.Fields.ModuleType = ModuleType;
    ApMailboxes.ApMailInfo.Fields.Module = Module;
    ApMailboxes.ApMailExtInfo.Info = 0;
    ApMailboxes.ApMailExtInfo.Fields.SystemDegree = Degree;
    // other fields of the extended info are used during ap init, and will be initialized at that time.
    State->Nb->PostMailbox (Node, ApMailboxes, State->Nb);
  }
  // Now that the mailboxes have been initialized, cache the info on the BSC.  The APs
  // will cache during heap initialization.
  CacheApMailbox (State->ConfigHandle);
}
