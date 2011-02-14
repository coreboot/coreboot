/* $NoKeywords:$ */
/**
 * @file
 *
 * HT NorthBridge header
 *
 * Defines the interface to the HT NorthBridge module for use by other internal
 * HT modules.  This is not a wrapper or external interface, "public" in the
 * comments below is used in the class definition style and refers to HT client
 * modules only ("private" being for use only by the HT NB module itself).
 *
 * It is expected that there will be multiple northbridge implementation files all
 * conforming to this common interface.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 39162 $   @e \$Date: 2010-10-07 22:41:37 +0800 (Thu, 07 Oct 2010) $
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

#ifndef _HT_NB_H_
#define _HT_NB_H_

/**
 * @page htimplnb HT Northbridge Implementation Guide
 *
 * The HT Northbridge provides access to the Northbridge hardware, in a manner that
 * isolates calling code from knowledge about the hardware implementation or which
 * features are supported in the current build.  This is the mechanism in the HT code for
 * supporting new Family or Model northbridges, as well as the means for supporting
 * multiple northbridges in a single build or mixed revision northbridge sets.
 *
 * @par Adding a Method to the Northbridge
 *
 * To add a new method to the Northbridge, follow these steps.
 * <ul>
 * <li> Create a typedef for the Method with the correct parameters and return type.
 *
 *   <ul>
 *   <li> Name the method typedef (F_METHOD_NAME)(), where METHOD_NAME is the same
 *     name as the method table item, but with "_"'s and UPPERCASE, rather than mixed case.
 *     @n <tt> typedef VOID (F_METHOD_NAME)(); </tt> @n
 *
 *   <li> Make a reference type for references to a method implementation:
 *     @n <tt> /// Reference to a Method              </tt>
 *     @n <tt> typedef F_METHOD_NAME *PF_METHOD_NAME </tt> @n
 *   </ul>
 *
 * <li> One of the parameters to @b all northbridge Methods is @b required to be a
 *   reference to its current northbridge object.  By convention, this is the
 *   last parameter.
 *
 * <li> Provide a standard doxygen function preamble for the Method typedef. Begin the
 *   detailed description by providing a reference to the method instances page by including
 *   the lines below:
 *   @code
 *   *
 *   * @HtNbInstances
 *   *
 *   @endcode
 *   @note It is important to provide documentation for the method type, because the method may not
 *   have an implementation in any families supported by the current package. @n
 *
 * <li> Add to the NORTHBRIDGE struct an item for the Method:
 *   @n <tt>  PF_METHOD_NAME MethodName; ///< Method: description. </tt> @n
 * </ul>
 *
 * @par Implementing an Instance of a Northbridge method.
 *
 * To implement an instance of a method for a specific feature follow these steps.
 *
 * - In appropriate files, implement the method with the return type and parameters
 * matching the Method typedef.
 *   - If the Method implementation is common to all families, use the northbridge file
 *   for the function area, for example, add a new coherent initialization support method to the
 *   coherent northbridge file.
 *   - If the Method implementation is unique to each supported northbridge, use the
 *   family specific file for that function area (create it, if it doesn't already exist).
 *   The family specific files have the same name as the common one suffixed with "FamNN",
 *   or "FamNNRevX" if for a model or revision.
 *
 * - Name the function MethodName().  If Family specific, FamNNMethodName().
 *
 * - Create a doxygen function preamble for the method instance.  Begin the detailed description with
 *   an Implements command to reference the method type and add this instance to the Method Instances page.
 *   @code
 *   *
 *   *  @HtNbMethod{::F_METHOD_NAME}.
 *   *
 *   @endcode
 *
 * - To access other northbridge routines or data as part of the method implementation,
 *   the function must use Nb->OtherMethod().  Do not directly access other northbridge
 *   routines, because in the table there may be overrides or this routine may be shared by
 *   multiple configurations.
 *
 * - Add the instance, or the correct family specific instance, to the NORTHBRIDGE instances
 *   used by the northbridge constructor.
 *
 * - If a northbridge does not need an instance of the method use one of the CommonReturns from
 *   CommonReturns.h with the same return type.
 *
 * @par Making common Northbridge Methods.
 *
 * In some cases, Northbridge methods can easily have a common implementation because the hardware
 * is very compatible or is even standard.  In other cases, where processor family northbridges
 * differ in their implementation, it may be possible to provide a single, common method
 * implementation.  This can be accomplished by adding Northbridge data members.
 *
 * For example, a bit position or bit field mask can be used to accommodate different bit placement or size.
 * Another example, a small table can be used to translate index values from a common set
 * to specific sets.
 *
 * The Northbridge Method Instance must use its NORTHBRIDGE reference parameter to access
 * private data members.
 *
 * @par Invoking HT Northbridge Methods.
 *
 * Each unique northbridge is constructed based on matching the current northbridge.
 * @n @code
 *   NORTHBRIDGE Nb;
 *   // Create the BSP's northbridge.
 *   NewNorthBridge (0, State, &Nb);
 *   State->Nb = &Nb;
 * @endcode
 *
 * The following example shows how to invoke a Northbridge method.
 * @n @code
 *   State->Nb->MethodName (State->Nb);
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

/** Use a macro to convert a Node number to a PCI device.  If some future port of
 * this code needs to, this can easily be replaced by the function declaration:
 * UINT8 makePCIDeviceFromNode(UINT8 Node);
 */
#define MakePciDeviceFromNode(Node)             \
  ((UINT8) (24 + (Node)))

/** Use a macro to convert a Node number to a PCI bus.  If some future port of
 * this code needs to, this can easily be replaced by the function declaration:
 * UINT8 MakePciBusFromNode(UINT8 Node);
 */
#define MakePciBusFromNode(Node)                \
  ((UINT8) (0))

/** Use a macro to convert a Node number to a PCI Segment.  If some future port of
 * this code needs to, this can easily be replaced by the function declaration:
 * UINT8 MakePciSegmentFromNode(UINT8 Node);
 */
#define MakePciSegmentFromNode(Node)            \
  ((UINT8) (0))

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/**
 * Status for iterating through internal (if supported) and external links.
 */
typedef enum {
  LinkIteratorEnd,         ///< This is the end of all links, no valid link.
  LinkIteratorExternal,    ///< The next link (the one we got on this call) is an external link.
  LinkIteratorInternal,    ///< The next link (the one we got on this call) is an internal link.
  LinkIteratorMax          ///< For bounds checking and limit only.
} LINK_ITERATOR_STATUS;

#define LINK_ITERATOR_BEGIN 0xFF

/**
 * Write a temporary Route.
 *
 * @HtNbInstances
 *
 * @param[in] Node The node on which to set a temporary route
 * @param[in] Target A route to this node, which route table entry is to be set
 * @param[in] Link The link which routes to the target node
 * @param[in] Nb This northbridge
 */
typedef VOID F_WRITE_ROUTING_TABLE (
  IN       UINT8       Node,
  IN       UINT8       Target,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_WRITE_ROUTING_TABLE *PF_WRITE_ROUTING_TABLE;

/**
 * Modifies the NodeID register on the target Node
 *
 * @HtNbInstances
 *
 * @param[in]     Node   the Node that will have its NodeID altered.
 * @param[in]     NodeID the new value for NodeID
 * @param[in]     Nb     this northbridge
 */
typedef VOID F_WRITE_NODEID (
  IN       UINT8       Node,
  IN       UINT8       NodeID,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_WRITE_NODEID *PF_WRITE_NODEID;

/**
 * Read the Default Link
 *
 * @HtNbInstances
 *
 * @param[in]     Node   the Node that will have its NodeID altered.
 * @param[in]     Nb     this northbridge
 *
 * @return The HyperTransport Link where the request to
 *        read the default Link came from.  Since this code is running on the BSP,
 *        this should be the Link pointing back towards the BSP.
 */
typedef UINT8 F_READ_DEFAULT_LINK (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_READ_DEFAULT_LINK *PF_READ_DEFAULT_LINK;

/**
 * Turns routing tables on for a given Node
 *
 * @HtNbInstances
 *
 * @param[in] Node the Node that will have it's routing tables enabled
 * @param[in] Nb this northbridge
 */
typedef VOID F_ENABLE_ROUTING_TABLES (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_ENABLE_ROUTING_TABLES *PF_ENABLE_ROUTING_TABLES;

/**
 * Turns routing tables off for a given Node
 *
 * @HtNbInstances
 *
 * @param[in] Node the Node that will have it's routing tables disabled
 * @param[in] Nb this northbridge
 */
typedef VOID F_DISABLE_ROUTING_TABLES (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_DISABLE_ROUTING_TABLES *PF_DISABLE_ROUTING_TABLES;

/**
 * Verify that the Link is coherent, connected, and ready
 *
 * @HtNbInstances
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Link    the Link on that Node to examine
 * @param[in]     Nb      this northbridge
 *
 * @retval        TRUE    The Link is coherent
 *  @retval       FALSE   The Link has some other status
*/
typedef BOOLEAN F_VERIFY_LINK_IS_COHERENT (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_VERIFY_LINK_IS_COHERENT *PF_VERIFY_LINK_IS_COHERENT;

/**
 * Read the token stored in the scratchpad register field.
 *
 * @HtNbInstances
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Nb      this northbridge
 *
 * @return        the Token read from the Node
 */
typedef UINT8 F_READ_TOKEN (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_READ_TOKEN *PF_READ_TOKEN;

/**
 * Write the token stored in the scratchpad register
 *
 * @HtNbInstances
 *
 * @param[in]     Node the Node that marked with token
 * @param[in]     Value the token Value
 * @param[in]     Nb this northbridge
 */
typedef VOID F_WRITE_TOKEN (
  IN       UINT8       Node,
  IN       UINT8       Value,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_WRITE_TOKEN *PF_WRITE_TOKEN;

/**
 * Full Routing Table Register initialization
 *
 * @HtNbInstances
 *
 * @param[in]     Node            the Node that will be examined
 * @param[in]     Target          the Target Node for these routes
 * @param[in]     ReqLink         the Link for requests to Target
 * @param[in]     RspLink         the Link for responses to Target
 * @param[in]     BroadcastLinks  the broadcast Links
 * @param[in]     Nb              this northbridge
 */
typedef VOID F_WRITE_FULL_ROUTING_TABLE (
  IN       UINT8       Node,
  IN       UINT8       Target,
  IN       UINT8       ReqLink,
  IN       UINT8       RspLink,
  IN       UINT32      BroadcastLinks,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_WRITE_FULL_ROUTING_TABLE *PF_WRITE_FULL_ROUTING_TABLE;

/**
 * Determine whether a Node is compatible with the discovered configuration so far.
 *
 * @HtNbInstances
 *
 * @param[in] Node the Node
 * @param[in] Nb this northbridge
 *
 * @retval   TRUE    the node is not compatible
 * @retval   FALSE   the node is compatible
 */
typedef BOOLEAN F_IS_ILLEGAL_TYPE_MIX (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_IS_ILLEGAL_TYPE_MIX *PF_IS_ILLEGAL_TYPE_MIX;

/**
 * Return whether the current configuration exceeds the capability
 * of the nodes detected.
 *
 * @HtNbInstances
 *
 * @param[in]     Node    the Node
 * @param[in]     State    sysMpCap (updated) and NodesDiscovered
 * @param[in]     Nb      this northbridge
 *
 * @retval        TRUE     system is not capable of current config.
 * @retval        FALSE    system is capable of current config.
 */
typedef BOOLEAN F_IS_EXCEEDED_CAPABLE (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_IS_EXCEEDED_CAPABLE *PF_IS_EXCEEDED_CAPABLE;

/**
 * Stop a link, so that it is isolated from a connected device.
 *
 * @HtNbInstances
 *
 * Use is for fatal incompatible configurations.
 * While XMIT and RCV off are HT standard, the use of these bits
 * is generally family specific.
 *
 * @param[in] Node   the node to stop a link on.
 * @param[in] Link   the link to stop.
 * @param[in] State  access to special routine for writing link control register
 * @param[in] Nb     this northbridge.
 */
typedef VOID F_STOP_LINK (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_STOP_LINK *PF_STOP_LINK;

/**
 * Fix (hopefully) exceptional conditions.
 *
 * @HtNbInstances
 *
 * This routine is expected to be unimplemented for most families.
 * Some configurations may require that links be processed specially to prevent
 * serious problems, like hangs.  Check for that condition in this routine,
 * handle the link both for hardware and for adding to port list, if appropriate.
 * If this routine adds the link to port list or the link should not be added, return TRUE.
 *
 * @param[in] Node  The Node which has this link
 * @param[in] Link  The link to check for special conditions.
 * @param[in] State our global state.
 * @param[in] Nb    this northbridge.
 *
 * @retval    TRUE  This link received special handling.
 * @retval    FALSE This link was not handled specially, handle it normally.
 *
 */
typedef BOOLEAN F_HANDLE_SPECIAL_LINK_CASE (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_HANDLE_SPECIAL_LINK_CASE *PF_HANDLE_SPECIAL_LINK_CASE;

/**
 * Fix (hopefully) exceptional conditions.
 *
 * @HtNbInstances
 *
 * This routine is expected to be unimplemented for most families.
 * Some configurations may require that nodes be processed specially to prevent
 * serious problems, like hangs.  Check for that condition in this routine,
 * handle the node both for hardware and for adding to port list, if appropriate.
 * If this routine adds the node to port list or the node should not be added, return TRUE.
 *
 * @param[in] Node  The Node which need to be checked.
 * @param[in] Link  The link to check for special conditions.
 * @param[in] State our global state.
 * @param[in] Nb    this northbridge.
 *
 * @retval    TRUE  This node received special handling.
 * @retval    FALSE This node was not handled specially, handle it normally.
 *
 */
typedef BOOLEAN F_HANDLE_SPECIAL_NODE_CASE (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_HANDLE_SPECIAL_NODE_CASE *PF_HANDLE_SPECIAL_NODE_CASE;

/**
 * Get Info about Module Type of this northbridge
 *
 * @HtNbInstances
 *
 * @param[in]     Node                the Node
 * @param[out]    ModuleType          0 for Single, 1 for Multi
 * @param[out]    Module              The module number of this node (0 if Single)
 * @param[in]     Nb                  this northbridge
 *
 */
typedef VOID F_GET_MODULE_INFO (
  IN       UINT8       Node,
     OUT   UINT8       *ModuleType,
     OUT   UINT8       *Module,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_GET_MODULE_INFO *PF_GET_MODULE_INFO;

/**
 * Post info to AP cores via a mailbox.
 *
 * @HtNbInstances
 *
 * @param[in]     Node          the Node
 * @param[in]     ApMailInfo    The info to post
 * @param[in]     Nb            this northbridge
 *
 */
typedef VOID F_POST_MAILBOX (
  IN       UINT8        Node,
  IN       AP_MAILBOXES ApMailInfo,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_POST_MAILBOX *PF_POST_MAILBOX;

/**
 * Retrieve info from a node's AP mailbox.
 *
 * @HtNbInstances
 *
 * @param[in]     Node          the Node
 * @param[in]     ApMailInfo    The info to post
 * @param[in]     Nb            this northbridge
 *
 */
typedef AP_MAIL_INFO F_RETRIEVE_MAILBOX (
  IN       UINT8        Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_RETRIEVE_MAILBOX *PF_RETRIEVE_MAILBOX;

/**
 * Implement the hardware method of doing Socket Naming, by accessing this northbridge's Socket Id register.
 *
 * @HtNbInstances
 *
 * @param[in]   Node     The node for which we want the socket id.
 * @param[in]   TempNode The temporary node id route where the node can be accessed.
 * @param[in]   Nb       Our Northbridge.
 *
 * @return      The Socket Id
 */
typedef UINT8 F_GET_SOCKET (
  IN       UINT8       Node,
  IN       UINT8       TempNode,
  IN       NORTHBRIDGE *Nb
  );

/// Reference to a method.
typedef F_GET_SOCKET *PF_GET_SOCKET;

/**
 * Get the enabled Compute Units.
 *
 * Processors which don't support compute units return zero.
 *
 * @HtNbInstances
 *
 * @param[in]   Node     The node for which we want the socket id.
 * @param[in]   Nb       Our Northbridge.
 *
 * @return      The Socket Id
 */
typedef UINT8 F_GET_ENABLED_COMPUTE_UNITS (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/// Reference to a method.
typedef F_GET_ENABLED_COMPUTE_UNITS *PF_GET_ENABLED_COMPUTE_UNITS;

/**
 * Get the dual core Compute Units.
 *
 * Processors which don't support compute units return zero.
 *
 * @HtNbInstances
 *
 * @param[in]   Node     The node for which we want the socket id.
 * @param[in]   Nb       Our Northbridge.
 *
 * @return      The Socket Id
 */
typedef UINT8 F_GET_DUALCORE_COMPUTE_UNITS (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/// Reference to a method.
typedef F_GET_DUALCORE_COMPUTE_UNITS *PF_GET_DUALCORE_COMPUTE_UNITS;

/**
 * Return the Link to the Southbridge
 *
 * @HtNbInstances
 *
 * @param[in] Nb this northbridge
 *
 * @return the Link to the southbridge
 */
typedef UINT8 F_READ_SB_LINK (
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_READ_SB_LINK *PF_READ_SB_LINK;

/**
 * Verify that the Link is non-coherent, connected, and ready
 *
 * @HtNbInstances
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Link    the Link on that Node to examine
 * @param[in]     Nb      this northbridge
 *
 * @retval        TRUE    The Link is non-coherent.
 * @retval        FALSE   The Link has some other status
 */
typedef BOOLEAN F_VERIFY_LINK_IS_NON_COHERENT (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_VERIFY_LINK_IS_NON_COHERENT *PF_VERIFY_LINK_IS_NON_COHERENT;

/**
 * Enable config access to a non-coherent chain for the given bus range.
 *
 * @HtNbInstances
 *
 * @param[in]     ConfigMapIndex the map entry to set
 * @param[in]     SecBus         The secondary bus number to use
 * @param[in]     SubBus         The subordinate bus number to use
 * @param[in]     TargetNode     The Node  that shall be the recipient of the traffic
 * @param[in]     TargetLink     The Link that shall be the recipient of the traffic
 * @param[in]     State          our global state
 * @param[in]     Nb             this northbridge
 */
typedef VOID F_SET_CONFIG_ADDR_MAP (
  IN       UINT8       ConfigMapIndex,
  IN       UINT8       SecBus,
  IN       UINT8       SubBus,
  IN       UINT8       TargetNode,
  IN       UINT8       TargetLink,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_SET_CONFIG_ADDR_MAP *PF_SET_CONFIG_ADDR_MAP;

/**
 * Northbridge specific Frequency limit.
 *
 * @HtNbInstances
 *
 * Return a mask that eliminates HT frequencies that cannot be used due to a slow
 * northbridge frequency.
 *
 * @param[in]     Node             Result could (later) be for a specific Node
 * @param[in]     Interface        Access to non-HT support functions.
 * @param[in]     PlatformConfig   Platform profile/build option config structure.
 * @param[in]     Nb               this northbridge
 *
 * @return Frequency mask
 */
typedef UINT32 F_NORTH_BRIDGE_FREQ_MASK (
  IN       UINT8                  Node,
  IN       HT_INTERFACE           *Interface,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       NORTHBRIDGE            *Nb
  );
/// Reference to a method.
typedef F_NORTH_BRIDGE_FREQ_MASK *PF_NORTH_BRIDGE_FREQ_MASK;

/**
 * Get Link features into system data structure.
 *
 * @HtNbInstances
 *
 * @param[in,out] ThisPort         The PortList structure entry for this link's port
 * @param[in]     Interface        Access to non-HT support functions.
 * @param[in]     PlatformConfig   Platform profile/build option config structure.
 * @param[in]     Nb               this northbridge
 */
typedef VOID F_GATHER_LINK_FEATURES (
  IN OUT   PORT_DESCRIPTOR        *ThisPort,
  IN       HT_INTERFACE           *Interface,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       NORTHBRIDGE            *Nb
  );
/// Reference to a method.
typedef F_GATHER_LINK_FEATURES *PF_GATHER_LINK_FEATURES;

/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 *
 * @HtNbInstances
 *
 * @param[in]     Node   the node on which to regang a link
 * @param[in]     Link   the sublink 0 of the sublink pair to regang
 * @param[in]     Nb     this northbridge
 */
typedef VOID F_SET_LINK_REGANG (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_SET_LINK_REGANG *PF_SET_LINK_REGANG;

/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 *
 * @HtNbInstances
 *
 * @param[in]     Node        the node on which to set frequency for a link
 * @param[in]     Link        the link to set frequency
 * @param[in]     Frequency   the frequency to set
 * @param[in]     Nb          this northbridge
 */
typedef VOID F_SET_LINK_FREQUENCY (
  IN       UINT8          Node,
  IN       UINT8          Link,
  IN       UINT8          Frequency,
  IN       NORTHBRIDGE    *Nb
  );
/// Reference to a method.
typedef F_SET_LINK_FREQUENCY *PF_SET_LINK_FREQUENCY;

/**
 * Set the link's Unit Id Clumping enable.
 *
 * @HtNbInstances
 *
 * This applies to the host root of a non-coherent chain.
 *
 * @param[in]     Node              the node on which to set frequency for a link
 * @param[in]     Link              the link to set frequency
 * @param[in]     ClumpingEnables   the unit id clumping enables to set
 * @param[in]     Nb                this northbridge
 */
typedef VOID F_SET_LINK_UNITID_CLUMPING (
  IN       UINT8          Node,
  IN       UINT8          Link,
  IN       UINT32         ClumpingEnables,
  IN       NORTHBRIDGE    *Nb
  );
/// Reference to a method.
typedef F_SET_LINK_UNITID_CLUMPING *PF_SET_LINK_UNITID_CLUMPING;

/**
 * Set the traffic distribution register for the Links provided.
 *
 * @HtNbInstances
 *
 * @param[in]     Links01   coherent Links from Node 0 to 1
 * @param[in]     Links10   coherent Links from Node 1 to 0
 * @param[in]     Nb        this northbridge
 */
typedef VOID F_WRITE_TRAFFIC_DISTRIBUTION (
  IN       UINT32       Links01,
  IN       UINT32       Links10,
  IN       NORTHBRIDGE  *Nb
  );
/// Reference to a method.
typedef F_WRITE_TRAFFIC_DISTRIBUTION *PF_WRITE_TRAFFIC_DISTRIBUTION;

/**
 * Write a link pair to the link pair distribution and fixups.
 *
 * @HtNbInstances
 *
 * @param[in]    Node           Set the pair on this node
 * @param[in]    ConnectedNode  The Node to which this link pair directly connects.
 * @param[in]    Pair           Using this pair set in the register
 * @param[in]    Asymmetric     True if different widths
 * @param[in]    MasterLink     Set this as the master link and in the route
 * @param[in]    AlternateLink  Set this as the alternate link
 * @param[in]    Nb             this northbridge
 *
 */
typedef VOID F_WRITE_LINK_PAIR_DISTRIBUTION (
  IN       UINT8        Node,
  IN       UINT8        ConnectedNode,
  IN       UINT8        Pair,
  IN       BOOLEAN      Asymmetric,
  IN       UINT8        MasterLink,
  IN       UINT8        AlternateLink,
  IN       NORTHBRIDGE  *Nb
  );
/// Pointer to method WriteLinkPairDistribution
typedef F_WRITE_LINK_PAIR_DISTRIBUTION *PF_WRITE_LINK_PAIR_DISTRIBUTION;

/**
 * Family specific tunings.
 *
 * @HtNbInstances
 *
 * Buffer tunings are inherently northbridge specific. Check for specific configs
 * which require adjustments and apply any standard workarounds to this Node.
 *
 * @param[in]     Node   the Node to tune
 * @param[in]     State   global state
 * @param[in]     Nb     this northbridge
 */
typedef VOID F_BUFFER_OPTIMIZATIONS (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_BUFFER_OPTIMIZATIONS *PF_BUFFER_OPTIMIZATIONS;

/**
 * Return the number of cores (1 based count) on Node.
 *
 * @HtNbInstances
 *
 * @param[in]     Node   the Node that will be examined
 * @param[in]     Nb     this northbridge
 *
 * @return        the number of cores
 */
typedef UINT8 F_GET_NUM_CORES_ON_NODE (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_GET_NUM_CORES_ON_NODE *PF_GET_NUM_CORES_ON_NODE;

/**
 * Write the total number of cores and Nodes to the Node
 *
 * @HtNbInstances
 *
 * @param[in]     Node         the Node that will be examined
 * @param[in]     TotalNodes   the total number of Nodes
 * @param[in]     TotalCores   the total number of cores
 * @param[in]     Nb           this northbridge
 */
typedef VOID F_SET_TOTAL_NODES_AND_CORES (
  IN       UINT8       Node,
  IN       UINT8       TotalNodes,
  IN       UINT8       TotalCores,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_SET_TOTAL_NODES_AND_CORES *PF_SET_TOTAL_NODES_AND_CORES;

/**
 * Get the Count of Nodes in the system.
 *
 * @HtNbInstances
 *
 * @param[in] Nb    This Northbridge.
 *
 * @return    The Count (1 based) of Nodes in the system.
 */
typedef UINT8 F_GET_NODE_COUNT (
  IN       NORTHBRIDGE *Nb
  );

/// Reference to a method.
typedef F_GET_NODE_COUNT *PF_GET_NODE_COUNT;

/**
 * Limit coherent config accesses to cpus as indicated by Nodecnt.
 *
 * @HtNbInstances
 *
 * @param[in]     Node    the Node that will be examined
 * @param[in]     Nb      this northbridge
 */
typedef VOID F_LIMIT_NODES (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_LIMIT_NODES *PF_LIMIT_NODES;

/**
 * Return the LinkFailed status AFTER an attempt is made to clear the bit.
 *
 * @HtNbInstances
 *
 * @param[in]     Node  the Node that will be examined
 * @param[in]     Link  the Link on that Node to examine
 * @param[in]     State  access to call back routine
 * @param[in]     Nb    this northbridge
 *
 * @retval        TRUE   the Link is not connected or has hard error
 * @retval        FALSE  the Link is connected
 */
typedef BOOLEAN F_READ_TRUE_LINK_FAIL_STATUS (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_READ_TRUE_LINK_FAIL_STATUS *PF_READ_TRUE_LINK_FAIL_STATUS;

/**
 * Get the next link for iterating over the links on a node in the correct order.
 *
 * @HtNbInstances
 *
 * @param[in]     Node The node on which to iterate links.
 * @param[in,out] Link IN: the current iteration context, OUT: the next link.
 * @param[in]     Nb   This Northbridge, access to config pointer.
 *
 * @retval LinkIteratorEnd         There is no next link (Link is back to BEGIN).
 * @retval LinkIteratorExternal    The next Link is an external link.
 * @retval LinkIteratorInternal    The next Link is an internal link.
 */
typedef LINK_ITERATOR_STATUS F_GET_NEXT_LINK (
  IN       UINT8       Node,
  IN OUT   UINT8       *Link,
  IN       NORTHBRIDGE *Nb
  );
/// Pointer to method GetNextLink
typedef F_GET_NEXT_LINK *PF_GET_NEXT_LINK;

/**
 * Get the Package Link number, given the node and real link number.
 *
 * @HtNbInstances
 *
 * @param[in]   Node       the node which has this link
 * @param[in]   Link       the link on that node
 * @param[in]   Nb         this northbridge
 *
 * @return                 the Package Link
 *
 */
typedef UINT8 F_GET_PACKAGE_LINK (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method
typedef F_GET_PACKAGE_LINK *PF_GET_PACKAGE_LINK;

/**
 * Return the HT Host capability base PCI config address for a Link.
 *
 * @HtNbInstances
 *
 * @param[in]     Node    the Node this Link is on
 * @param[in]     Link    the Link
 * @param[in]     Nb      this northbridge
 *
 * @return  the pci config address
 */
typedef PCI_ADDR F_MAKE_LINK_BASE (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_MAKE_LINK_BASE *PF_MAKE_LINK_BASE;

/**
 * Make a compatibility key.
 *
 * @HtNbInstances
 *
 * @param[in] Node the Node
 * @param[in] Nb   this northbridge
 *
 * @return the key
 */
typedef UINT64 F_MAKE_KEY (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );
/// Reference to a method.
typedef F_MAKE_KEY *PF_MAKE_KEY;

/**
 * The northbridge interface.
 *
 * Abstract the hardware implementation of the processor northbridge.  Feature code does
 * not need to be tailored to specific families.  Also, more than a single family (or
 * model in some cases) can be supported at once.  Multiple family support can be for
 * mixed revisions or for incompatible revisions where only one is used at a time.
 *
 * The northbridge object contains both HT component public and northbridge private
 * members.  These sets are grouped together.  Within each group, members are grouped
 * according to the function area they support.
 *
 */
struct _NORTHBRIDGE {               // See forward declaration in HtFeats.h
  /* Public data, clients of northbridge can access */
  UINT8 MaxLinks;               /**< The maximum number of Links implemented by the northbridge */

  /* Public Interfaces for northbridge clients, coherent init*/
  PF_WRITE_ROUTING_TABLE WriteRoutingTable;    /**< Method: Write a Temporary route for discovery */
  PF_WRITE_NODEID WriteNodeID;    /**< Method: Assign a Node ID*/
  PF_READ_DEFAULT_LINK ReadDefaultLink;    /**< Method: Which link are we connected to on a remote node? */
  PF_ENABLE_ROUTING_TABLES EnableRoutingTables;    /**< Method: Make the routing table active */
  PF_DISABLE_ROUTING_TABLES DisableRoutingTables;    /**< Method: Put a node back in discoverable state (deflnk) */
  PF_VERIFY_LINK_IS_COHERENT VerifyLinkIsCoherent;    /**< Method: is a link connected and coherent? */
  PF_READ_TOKEN ReadToken;    /**< Method: Read the enumeration token from a node */
  PF_WRITE_TOKEN WriteToken;    /**< Method: Assign an enumeration token to a node */
  PF_WRITE_FULL_ROUTING_TABLE WriteFullRoutingTable;    /**< Method: Set a complete routing table entry on a node */
  PF_IS_ILLEGAL_TYPE_MIX IsIllegalTypeMix;    /**< Method: Is this node compatible with the system */
  PF_IS_EXCEEDED_CAPABLE IsExceededCapable;    /**< Method: Is this node capable of working in this system */
  PF_STOP_LINK StopLink;    /**< Method: stop a link which must be unused */
  PF_HANDLE_SPECIAL_LINK_CASE HandleSpecialLinkCase;    /**< Method: Fix broken configuration designs */
  PF_HANDLE_SPECIAL_NODE_CASE HandleSpecialNodeCase;    /**< Method: Fix broken configuration designs */

  /* Public Interfaces for northbridge clients, noncoherent init */
  PF_READ_SB_LINK ReadSouthbridgeLink;    /**< Method: Which link goes to the southbridge? */
  PF_VERIFY_LINK_IS_NON_COHERENT VerifyLinkIsNonCoherent;    /**< Method: is a link connected and non-coherent? */
  PF_SET_CONFIG_ADDR_MAP SetConfigAddrMap;    /**< Method: Add a non-coherent chain to the PCI Config Bus Address Map */

  /* Public Interfaces for northbridge clients, Optimization */
  PF_NORTH_BRIDGE_FREQ_MASK NorthBridgeFreqMask;    /**< Method: Check for frequency limits other than HT */
  PF_GATHER_LINK_FEATURES GatherLinkFeatures;        /**< Method: Get frequency and link features */
  PF_SET_LINK_REGANG SetLinkRegang;              /**< Method: Set a Link to regang */
  PF_SET_LINK_FREQUENCY SetLinkFrequency;        /**< Method: Set the link Frequency */
  PF_SET_LINK_UNITID_CLUMPING SetLinkUnitIdClumping;   /**< Method: Set the link's Unit Id Clumping register */

  /* Public Interfaces for northbridge clients, System and performance Tuning. */
  PF_WRITE_TRAFFIC_DISTRIBUTION WriteTrafficDistribution;    /**< Method: traffic distribution setting */
  PF_WRITE_LINK_PAIR_DISTRIBUTION WriteLinkPairDistribution;    /**< Method: Link Pair setting and fix up */
  PF_BUFFER_OPTIMIZATIONS BufferOptimizations;    /**< Method: system tunings which can not be
                                                   * done using register table */

  /* Public Interfaces for northbridge clients, utility routines */
  PF_GET_NUM_CORES_ON_NODE GetNumCoresOnNode;    /**< Method: Count cores */
  PF_SET_TOTAL_NODES_AND_CORES SetTotalNodesAndCores;    /**< Method: Set Node and Core counts */
  PF_GET_NODE_COUNT GetNodeCount;    /**< Method: Get the Count (1 based) of Nodes in the system. */
  PF_LIMIT_NODES LimitNodes;    /**< Method: Set the Limit Config Space feature */
  PF_READ_TRUE_LINK_FAIL_STATUS ReadTrueLinkFailStatus;    /**< Method: Get Fault status and connectivity of a link */
  PF_GET_NEXT_LINK GetNextLink;    /**< Method: Iterate over a node's Internal, then External links. */
  PF_GET_PACKAGE_LINK GetPackageLink;    /**< Method: the package link corresponding to a node's link */
  PF_MAKE_LINK_BASE MakeLinkBase;    /**< Method: Provide the PCI Config Base register offset of a CPU link */
  PF_GET_MODULE_INFO GetModuleInfo;    /**< Method: Get Module Type and internal Module number */
  PF_POST_MAILBOX PostMailbox;     /**< Method: Post info to the mailbox register */
  PF_RETRIEVE_MAILBOX RetrieveMailbox;     /**< Method: Retrieve info from the mailbox register */
  PF_GET_SOCKET GetSocket;    /**< Method: Get a node's Socket, using the hardware naming method. */
  PF_GET_ENABLED_COMPUTE_UNITS GetEnabledComputeUnits; /**< Method: Get the Enabled Compute Units */
  PF_GET_DUALCORE_COMPUTE_UNITS GetDualCoreComputeUnits; /**< Method: Get which Compute Units have two cores. */

  /* Private Data for northbridge implementation use only */
  UINT32 SelfRouteRequestMask;  /**< Bit pattern for route request to self in routing table register */
  UINT32 SelfRouteResponseMask; /**< Bit pattern for route response to self in routing table register */
  UINT8 BroadcastSelfBit;       /**< Bit offset of broadcast self bit in routing table register */
  BOOLEAN IsOrderBSPCoresByNode; /**< This processor orders Cores by Node id on the BSP, if TRUE. */
  BOOLEAN IsOrderCoresByModule;  /**< Processors other than the BSP order Cores by Module, if TRUE. */
  UINT64 CompatibleKey;         /**< Used for checking compatibility of northbridges in the system */
  PACKAGE_HTLINK_MAP PackageLinkMap;    /**< Tell GetPackageLink() how to assign link names */
  UINT32 CoreFrequency;           /**< Cache the northbridge core frequency, so repeated interface calls are avoided.
                                   *   A value of zero, means no value yet. */
  IGNORE_LINK *DefaultIgnoreLinkList;   /**< After processing the user interface ignore link, process this list. */

  /* Private Interfaces for northbridge implementation. */
  PF_MAKE_KEY MakeKey;    /**< Method: make the compatibility key for this node */

  /** Config Pointer, opaque handle for passing to lib */
  VOID *ConfigHandle;
};

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
/**
 * Make a compatibility key.
 *
 */
UINT64
MakeKey (
  IN       UINT8 Node,
  IN       NORTHBRIDGE *Nb
  );

VOID
NewNorthBridge (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
     OUT   NORTHBRIDGE *Nb
  );

#endif  /* _HT_NB_H_ */
