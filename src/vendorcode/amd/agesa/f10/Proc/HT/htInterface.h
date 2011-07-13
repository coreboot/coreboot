/**
 * @file
 *
 * Internal access to HT Interface.
 *
 * This file provides definitions used by HT internal modules.  The
 * external HT interface (in agesa.h) is accessed using these methods.
 * This keeps the HT Feature implementations abstracted from the HT
 * interface.
 *
 * This file includes the interface access constructor and interface
 * support which is not removed with various build options.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $        @e  \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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

#ifndef _HT_INTERFACE_H_
#define _HT_INTERFACE_H_

/**
 * @page htimplintf HT Internal Interface Implementation Guide
 *
 * HT Internal Interface provides access to the HT Component external interface (see AGESA.h),
 * in a manner that isolates calling code from knowledge about the external interface or which
 * interfaces are supported in the current build.
 *
 * @par Adding a Method to HT Internal Interface
 *
 * To add a new method to the HT Internal Interface, follow these steps.
 * <ul>
 * <li> Create a typedef for the Method with the correct parameters and return type.
 *
 *   <ul>
 *   <li> Name the method typedef (F_METHOD_NAME)(), where METHOD_NAME is the same name as the method table item,
 *     but with "_"'s and UPPERCASE, rather than mixed case.
 *     @n <tt> typedef VOID (F_METHOD_NAME)(); </tt> @n
 *
 *   <li> Make a reference type for references to a method implementation:
 *     @n <tt> /// Reference to a Method              </tt>
 *     @n <tt> typedef F_METHOD_NAME *PF_METHOD_NAME  </tt> @n
 *   </ul>
 *
 * <li> Provide a standard doxygen function preamble for the Method typedef. Begin the
 *   detailed description by providing a reference to the method instances page by including
 *   the lines below:
 *   @code
 *   *
 *   * @HtInterfaceInstances
 *   *
 *   @endcode
 *   @note It is important to provide documentation for the method type, because the method may not
 *   have an implementation in any families supported by the current package. @n
 *
 * <li> Add to the ::HT_INTERFACE struct an item for the Method:
 *   @n <tt>  PF_METHOD_NAME MethodName; ///< Method: description. </tt> @n
 * </ul>
 *
 * @par Implementing an HT Internal Interface Instance of the method.
 *
 * To implement an instance of a method for a specific interface  follow these steps.
 *
 * - In appropriate files, implement the method with the return type and parameters
 * matching the method typedef.
 *
 * - Name the function MethodName().
 *
 * - Create a doxygen function preamble for the method instance.  Begin the detailed description with
 *   an Implements command to reference the method type and add this instance to the Method Instances page.
 *   @code
 *   *
 *   *  @HtInterfaceMethod{::F_METHOD_NAME}.
 *   *
 *   @endcode
 *
 * - To access other Ht internal interface routines or data as part of the method implementation, the function
 *   must use HtInterface->OtherMethod().  Do not directly access other HT internal interface
 *   routines, because in the table there may be overrides or this routine may be shared by multiple families.
 *
 * - Add the instance to the ::HT_INTERFACE instances.
 *
 * - If a configuration does not need an instance of the method use one of the CommonReturns from
 *   CommonReturns.h with the same return type.
 *
 * @par Invoking HT Internal Interface Methods.
 *
 * The first step is carried out only once by the top level HT entry point.
 * @n @code
 *   HT_INTERFACE HtInterface;
 *   // Get the current HT internal interface (to HtBlock data)
 *   NewHtInterface (&HtInterface);
 *   State->HtInterface = &HtInterface;
 * @endcode
 *
 * The following example shows how to invoke a HT Internal Interface method.
 * @n @code
 *    State->HtInterface->MethodName ();
 * @endcode
 *
 */

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/**
 * Get limits for CPU to CPU Links.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     NodeA            One Node on which this Link is located
 * @param[in]     LinkA            The Link on this Node
 * @param[in]     NodeB            The other Node on which this Link is located
 * @param[in]     LinkB            The Link on that Node
 * @param[in,out] ABLinkWidthLimit modify to change the Link Width In
 * @param[in,out] BALinkWidthLimit modify to change the Link Width Out
 * @param[in,out] PcbFreqCap       modify to change the Link's frequency capability
 * @param[in]     State            the input data
 *
 */
typedef VOID F_GET_CPU_2_CPU_PCB_LIMITS (
  IN       UINT8        NodeA,
  IN       UINT8        LinkA,
  IN       UINT8        NodeB,
  IN       UINT8        LinkB,
  IN OUT   UINT8        *ABLinkWidthLimit,
  IN OUT   UINT8        *BALinkWidthLimit,
  IN OUT   UINT32       *PcbFreqCap,
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_GET_CPU_2_CPU_PCB_LIMITS *PF_GET_CPU_2_CPU_PCB_LIMITS;

/**
 * Skip reganging of subLinks.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     NodeA    One Node on which this Link is located
 * @param[in]     LinkA    The Link on this Node
 * @param[in]     NodeB    The other Node on which this Link is located
 * @param[in]     LinkB    The Link on that Node
 * @param[in]     State    the input data
 *
 * @retval        MATCHED      leave Link unganged
 * @retval        POWERED_OFF  leave link unganged and power off the paired sublink
 * @retval        UNMATCHED    regang Link automatically
 */
typedef FINAL_LINK_STATE F_GET_SKIP_REGANG (
  IN       UINT8        NodeA,
  IN       UINT8        LinkA,
  IN       UINT8        NodeB,
  IN       UINT8        LinkB,
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_GET_SKIP_REGANG *PF_GET_SKIP_REGANG;

/**
 * Manually control bus number assignment.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     Node      The Node on which this chain is located
 * @param[in]     Link      The Link on the host for this chain
 * @param[out]    SecBus    Secondary Bus number for this non-coherent chain
 * @param[out]    SubBus    Subordinate Bus number
 * @param[in]     State     the input data
 *
 * @retval        TRUE      this routine is supplying the bus numbers
 * @retval        FALSE     use auto Bus numbering
 */
typedef BOOLEAN F_GET_OVERRIDE_BUS_NUMBERS (
  IN       UINT8        Node,
  IN       UINT8        Link,
     OUT   UINT8        *SecBus,
     OUT   UINT8        *SubBus,
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_GET_OVERRIDE_BUS_NUMBERS *PF_GET_OVERRIDE_BUS_NUMBERS;

/**
 * Get Manual BUID assignment list.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]    Node    The Node on which this chain is located
 * @param[in]    Link    The Link on the host for this chain
 * @param[out]   List    a pointer to a list, if returns TRUE
 * @param[in]    State   the input data
 *
 * @retval      TRUE     use manual List
 * @retval      FALSE    initialize the Link automatically. List not valid.
 */
typedef BOOLEAN F_GET_MANUAL_BUID_SWAP_LIST (
  IN       UINT8            Node,
  IN       UINT8            Link,
     OUT   BUID_SWAP_LIST   **List,
  IN       STATE_DATA       *State
  );
/// Reference to a method.
typedef F_GET_MANUAL_BUID_SWAP_LIST *PF_GET_MANUAL_BUID_SWAP_LIST;

/**
 * Override capabilities of a device.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     HostNode      The Node on which this chain is located
 * @param[in]     HostLink      The Link on the host for this chain
 * @param[in]     Depth         The Depth in the I/O chain from the Host
 * @param[in]     PciAddress    The Device's PCI config address (for callout)
 * @param[in]     DevVenId      The Device's PCI Vendor + Device ID (offset 0x00)
 * @param[in]     Revision      The Device's PCI Revision
 * @param[in]     Link          The Device's Link number (0 or 1)
 * @param[in,out] LinkWidthIn   modify to change the Link Width In
 * @param[in,out] LinkWidthOut  modify to change the Link Width Out
 * @param[in,out] FreqCap       modify to change the Link's frequency capability
 * @param[in,out] Clumping      modify to change unit id clumping capability
 * @param[in]     State         the input data
 *
 */
typedef VOID F_GET_DEVICE_CAP_OVERRIDE (
  IN       UINT8       HostNode,
  IN       UINT8       HostLink,
  IN       UINT8       Depth,
  IN       PCI_ADDR    PciAddress,
  IN       UINT32      DevVenId,
  IN       UINT8       Revision,
  IN       UINT8       Link,
  IN OUT   UINT8       *LinkWidthIn,
  IN OUT   UINT8       *LinkWidthOut,
  IN OUT   UINT32      *FreqCap,
  IN OUT   UINT32      *Clumping,
  IN       STATE_DATA  *State
  );
/// Reference to a method.
typedef F_GET_DEVICE_CAP_OVERRIDE *PF_GET_DEVICE_CAP_OVERRIDE;

/**
 * Get limits for non-coherent Links.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     HostNode                   The Node on which this Link is located
 * @param[in]     HostLink                   The Link about to be initialized
 * @param[in]     Depth                      The Depth in the I/O chain from the Host
 * @param[in,out] DownstreamLinkWidthLimit   modify to change the Link Width In
 * @param[in,out] UpstreamLinkWidthLimit     modify to change the Link Width Out
 * @param[in,out] PcbFreqCap                 modify to change the Link's frequency capability
 * @param[in]     State                      the input data
 */
typedef VOID F_GET_IO_PCB_LIMITS (
  IN       UINT8        HostNode,
  IN       UINT8        HostLink,
  IN       UINT8        Depth,
  IN OUT   UINT8        *DownstreamLinkWidthLimit,
  IN OUT   UINT8        *UpstreamLinkWidthLimit,
  IN OUT   UINT32       *PcbFreqCap,
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_GET_IO_PCB_LIMITS *PF_GET_IO_PCB_LIMITS;

/**
 * Get the Socket number for a given Node number.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     Node    Node discovered event data.
 * @param[in]     State   reference to Node to socket map
 *
 * @return the socket id
 *
 */
typedef UINT8 F_GET_SOCKET_FROM_MAP (
  IN       UINT8         Node,
  IN       STATE_DATA    *State
  );
/// Reference to a method.
typedef F_GET_SOCKET_FROM_MAP *PF_GET_SOCKET_FROM_MAP;

/**
 * Ignore a Link.
 *
 * @HtInterfaceInstances.
 *
 * @param[in]     Node   The Node on which this Link is located
 * @param[in]     Link   The Link about to be initialized
 * @param[in]     NbList The northbridge default ignore link list
 * @param[in]     State   the input data
 *
 * @retval        MATCHED     ignore this Link and skip it
 * @retval        POWERED_OFF ignore this link and power it off.
 * @retval        UNMATCHED   initialize the Link normally
 */
typedef FINAL_LINK_STATE F_GET_IGNORE_LINK (
  IN       UINT8        Node,
  IN       UINT8        Link,
  IN       IGNORE_LINK  *NbIgnoreLinkList,
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_GET_IGNORE_LINK *PF_GET_IGNORE_LINK;

/**
 * Post Node id and other context info to AP cores via mailbox.
 *
 * @HtInterfaceInstances.
 *
 * @param[in] State Our state
 */
typedef VOID F_POST_MAP_TO_AP (
  IN       STATE_DATA *State
  );
/// Reference to a method.
typedef F_POST_MAP_TO_AP *PF_POST_MAP_TO_AP;

/**
 * Clean up the map structures after severe event has caused a fall back to 1 node.
 *
 * @HtInterfaceInstances.
 *
 * @param[in] State Our state
 */
typedef VOID F_CLEAN_MAPS_AFTER_ERROR (
  IN       STATE_DATA *State
  );
/// Reference to a method.
typedef F_CLEAN_MAPS_AFTER_ERROR *PF_CLEAN_MAPS_AFTER_ERROR;

/**
 * Get a new Socket Die to Node Map.
 *
 * @HtInterfaceInstances.
 *
 * @param[in,out] State global state
 */
typedef VOID F_NEW_NODE_AND_SOCKET_TABLES (
  IN OUT   STATE_DATA *State
  );
/// Reference to a method.
typedef F_NEW_NODE_AND_SOCKET_TABLES *PF_NEW_NODE_AND_SOCKET_TABLES;

/**
 * Fill in the socket's Node id when a processor is discovered in that socket.
 *
 * @HtInterfaceInstances.
 *
 * @param[in] Node                Node from which a new node was discovered
 * @param[in] CurrentNodeModule   The current node's module id in it's processor.
 * @param[in] PackageLink         The package level link from Node to NewNode.
 * @param[in] NewNode             The new node's id
 * @param[in] HardwareSocket      If we use the hardware method (preferred), this is the socket of new node.
 * @param[in] Module              The new node's module id in it's processor.
 * @param[in] State               our State
 */
typedef VOID F_SET_NODE_TO_SOCKET_MAP (
  IN       UINT8 Node,
  IN       UINT8 CurrentNodeModule,
  IN       UINT8 PackageLink,
  IN       UINT8 NewNode,
  IN       UINT8 HardwareSocket,
  IN       UINT8 Module,
  IN       STATE_DATA *State
  );
/// Reference to a method.
typedef F_SET_NODE_TO_SOCKET_MAP *PF_SET_NODE_TO_SOCKET_MAP;

/**
 * Get a new, empty Hop Count Table, to make one for the installed topology.
 *
 * @HtInterfaceInstances.
 *
 * @param[in,out]   State    Keep our buffer handle.
 *
 */
typedef VOID F_NEW_HOP_COUNT_TABLE (
  IN OUT   STATE_DATA     *State
  );
/// Reference to a method.
typedef F_NEW_HOP_COUNT_TABLE *PF_NEW_HOP_COUNT_TABLE;

/**
 * Get the minimum Northbridge frequency for the system.
 *
 * @HtInterfaceInstances.
 *
 * Invoke the CPU component power mgt interface.
 *
 * @param[in]    StdHeader    Config for library and services.
 *
 * @return Frequency in MHz.
 *
 */
typedef UINT32 F_GET_MIN_NB_CORE_FREQ (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );
/// Reference to a Method.
typedef F_GET_MIN_NB_CORE_FREQ *PF_GET_MIN_NB_CORE_FREQ;

/**
 * The HT Interface, feature code uses these methods to get interface parameters.
 */
struct _HT_INTERFACE {
  PF_GET_CPU_2_CPU_PCB_LIMITS GetCpu2CpuPcbLimits;    /**< Method: Get link limits for coherent links. */
  PF_GET_SKIP_REGANG GetSkipRegang;                   /**< Method: Skip reganging for coherent links. */
  PF_NEW_HOP_COUNT_TABLE NewHopCountTable;            /**< Method: Get a new hop count table. */
  PF_GET_OVERRIDE_BUS_NUMBERS GetOverrideBusNumbers;  /**< Method: Control Bus number assignment. */
  PF_GET_MANUAL_BUID_SWAP_LIST GetManualBuidSwapList; /**< Method: Assign device IDs. */
  PF_GET_DEVICE_CAP_OVERRIDE GetDeviceCapOverride;    /**< Method: Override Device capabilities. */
  PF_GET_IO_PCB_LIMITS GetIoPcbLimits;                /**< Method: Get link limits for noncoherent links. */
  PF_GET_SOCKET_FROM_MAP GetSocketFromMap;            /**< Method: Get the Socket for a node id. */
  PF_GET_IGNORE_LINK GetIgnoreLink;                   /**< Method: Ignore a link. */
  PF_POST_MAP_TO_AP PostMapToAp;                      /**< Method: Post Socket and other info to AP cores. */
  PF_NEW_NODE_AND_SOCKET_TABLES NewNodeAndSocketTables; /**< Method: Get new socket and node maps. */
  PF_CLEAN_MAPS_AFTER_ERROR CleanMapsAfterError;       /**< Method: Clean up maps for forced 1P on error fall back. */
  PF_SET_NODE_TO_SOCKET_MAP SetNodeToSocketMap;       /**< Method: Associate a node id with a socket. */
  PF_GET_MIN_NB_CORE_FREQ GetMinNbCoreFreq;           /**< Method: Get the minimum northbridge frequency */
};

/*----------------------------------------------------------------------------
 *                         Prototypes to Interface from Feature Code
 *
 *----------------------------------------------------------------------------
 */

/**
 * A constructor for the internal Ht Interface.
 *
*/
VOID
NewHtInterface (
     OUT   HT_INTERFACE         *HtInterface,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif  /* _HT_INTERFACE_H_ */
