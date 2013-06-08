/* $NoKeywords:$ */
/**
 * @file
 *
 * HT Features.
 *
 * This file provides definitions used in common by HT internal modules.  The
 * data is private and not for external client access.
 * Definitions include the HT global internal state data structures, and
 * access to the available HT features from the main HT entry point.
 *
 * This file includes the feature constructor and feature support which is not
 * removed with various build options.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 63425 $        @e  \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

#ifndef _HT_FEAT_H_
#define _HT_FEAT_H_

/**
 * @page htimplfeat HT Features Implementation Guide
 *
 * HT Features provides access to the HT Feature set, in a manner that isolates
 * calling code from knowledge about the Feature set implementation or which
 * features are supported in the current build.  In the case of feature sets, this
 * is mostly used for build options to reduce code size by removing unneeded features.
 *
 * @par Adding a Method to HT Features
 *
 * To add a new method to the HT Features, follow these steps.
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
 *     @n <tt> typedef F_METHOD_NAME *PF_METHOD_NAME </tt> @n
 *   </ul>
 *
 * <li> Provide a standard doxygen function preamble for the Method typedef. Begin the
 *   detailed description by providing a reference to the method instances page by including
 *   the lines below:
 *   @code
 *   *
 *   * @HtFeatInstances.
 *   *
 *   @endcode
 *   @note It is important to provide documentation for the method type, because the method may not
 *   have an implementation in any families supported by the current package. @n
 *
 * <li> Add to the _HT_FEATURES struct an item for the Method:
 *   @n <tt>  PF_METHOD_NAME MethodName; ///< Method: description. </tt> @n
 * </ul>
 *
 * @par Implementing an HT Features Instance of the method.
 *
 * To implement an instance of a method for a specific feature follow these steps.
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
 *   *  @HtFeatMethod{::F_METHOD_NAME}.
 *   *
 *   @endcode
 *
 * - To access other Ht feature routines or data as part of the method implementation, the function
 *   must use HtFeatures->OtherMethod().  Do not directly access other HT feature
 *   routines, because in the table there may be overrides or this routine may be shared by multiple configurations.
 *
 * - Add the instance to the HT_FEATURES instances.
 *
 * - If a configuration does not need an instance of the method use one of the CommonReturns from
 *   CommonReturns.h with the same return type.
 *
 * @par Invoking HT Features Methods.
 *
 * The first step is carried out only once by the top level HT entry point.
 * @n @code
 * HT_FEATURES HtFeatures;
 *   // Get the current HT Feature Set
 *   NewHtFeatures (&HtFeatures);
 *   State->HtFeatures = &HtFeatures;
 * @endcode
 *
 * The following example shows how to invoke a HT Features method.
 * @n @code
 *    State->HtFeatures->MethodName ();
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
#define MAX_PLATFORM_LINKS 64
#define MAX_LINK_PAIRS 4

/* These following are internal definitions */
#define ROUTE_TO_SELF 0x0F
#define INVALID_LINK 0xCC     /* Used in port list data structure to mark unused data entries.
                                 Can also be used for no Link found in a port list search */

/* definitions for working with the port list structure */
#define PORTLIST_TYPE_CPU 0
#define PORTLIST_TYPE_IO  1

/*
 * Hypertransport Capability definitions and macros
 *
 */

#define HT_INTERFACE_CAP_SUBTYPE_MASK        ((UINT32)0xE00000FFul)
#define HT_CAP_SUBTYPE_MASK                  ((UINT32)0xF80000FFul)

/* HT Host Capability */
#define HT_HOST_CAPABILITY                    1
#define HT_HOST_CAP_SIZE                      0x20

/* Host CapabilityRegisters */
#define HTHOST_LINK_CAPABILITY_REG            0x00
#define HTHOST_LINK_CONTROL_REG               0x04
#define HTHOST_FREQ_REV_REG                   0x08
#define HTHOST_REV_REV3                       0x60
#define HTHOST_FEATURE_CAP_REG                0x0C
#define HTHOST_BUFFER_COUNT_REG               0x10
#define HTHOST_ISOC_REG                       0x14
#define HTHOST_LINK_TYPE_REG                  0x18
#define HTHOST_FREQ_EXTENSION                 0x1C
#define HTHOST_TYPE_COHERENT                  3
#define HTHOST_TYPE_NONCOHERENT               7
#define HTHOST_TYPE_MASK                      0x1F

/* HT Slave Capability (HT1 compat) */
#define HT_SLAVE_CAPABILITY                   0
#define HTSLAVE_LINK01_OFFSET                 4
#define HTSLAVE_LINK_CONTROL_0_REG            4
#define HTSLAVE_FREQ_REV_0_REG                0xC
#define HTSLAVE_FEATURECAP_REG                0x10
#define HT_CONTROL_CLEAR_CRC                  (~(3 << 8))
#define HT_FREQUENCY_CLEAR_LINK_ERRORS        (~(0x7 << 12))
#define MAX_BUID                              31

/* HT3 gen Capability */
#define HT_GEN3_CAPABILITY                    (0xD << 1)
#define HTGEN3_LINK01_OFFSET                  0x10
#define HTGEN3_LINK_TRAINING_0_REG            0x10

/* HT3 Retry Capability */
#define HT_RETRY_CAPABILITY                   (0xC << 1)
#define HTRETRY_CONTROL_REG                   4

/* Unit ID Clumping Capability */
#define HT_UNITID_CAPABILITY                  (0x9 << 1)
#define HTUNIT_SUPPORT_REG                    4
#define HTUNIT_ENABLE_REG                     8
#define HT_CLUMPING_PASSIVE                   1

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

// Forward declarations.
/// Used for forward reference.
typedef struct _NORTHBRIDGE NORTHBRIDGE;
/// Used for forward reference.
typedef struct _HT_FEATURES HT_FEATURES;
/// Used for forward reference.
typedef struct _HT_INTERFACE HT_INTERFACE;

/**
 * Coherent Init Data.
 *
 * Metrics representing the coherent fabric which was discovered:  Degree of nodes, adjacency,
 * node numbering permutations, and the topology which it matched.
 */
typedef struct {
  /** The number of coherent Links connected on each Node (the 'Degree' of the Node) */
  UINT8 SysDegree[MAX_NODES];
  /** The systems adjacency (sysMatrix[i][j] is true if Node_i has a Link to Node_j) */
  BOOLEAN SysMatrix[MAX_NODES][MAX_NODES];

  UINT8 DbDegree[MAX_NODES];               /**< Like sysDegree, but for the current database topology */
  BOOLEAN DbMatrix[MAX_NODES][MAX_NODES];  /**< Like sysMatrix, but for the current database topology */

  UINT8 Perm[MAX_NODES];           /**< The Node mapping from the system to the database */
  UINT8 ReversePerm[MAX_NODES];    /**< The Node mapping from the database to the system */
  UINT8 *MatchedTopology;          /**< The topology that matched the current system or NULL */
} COHERENT_FABRIC;

/**
 * Represent the system as Links of matched port pairs.
 * A pair consists of a source Node, a Link to the destination Node, the
 * destination Node, and its Link back to source Node.  The even indices are
 * the source Nodes and Links, and the odd indices are for the destination
 * Nodes and Links.
 * @note The Port pair 2*N and 2*N+1 are connected together to form a Link
 * (e.g. 0,1 and 8,9 are ports on either end of an HT Link) The lower number
 * port (2*N) is the source port.  The device that owns the source port is
 * always the device closer to the BSP. (i.e. nearer the CPU in a
 * non-coherent chain, or the CPU with the lower NodeID).
 */
typedef struct {
  /* This section is where the Link is in the system and how to find it */
  UINT8 Type;              /**< 0 = CPU, 1 = Device, all others reserved */
  UINT8 Link;              /**< 0-1 for devices, 0-7 for CPUs */
  UINT8 NodeID;            /**< The Node, or a pointer to the devices parent Node */
  UINT8 HostLink;          /**< For Devices, the root CPU's Link to the chain */
  UINT8 HostDepth;         /**< Link Depth in chain, only used by devices */
  PCI_ADDR Pointer;        /**< A pointer to the device's slave HT capability, so we don't have to keep searching */

  /* This section is for the final settings, which are written to hardware */
  BOOLEAN SelRegang;          /**< Indicates to software regang Link, only used for CPU->CPU Links */
  UINT8 SelWidthIn;           /**< Width in setting */
  UINT8 SelWidthOut;          /**< Width out setting */
  UINT8 SelFrequency;         /**< Frequency setting */

  /* This section is for keeping track of capabilities and possible configurations */
  BOOLEAN RegangCap;            /**< Is the port capable of reganging? CPUs only */
  UINT32 PrvFrequencyCap;       /**< Possible frequency settings */
  UINT8 PrvWidthInCap;          /**< Possible Width setting */
  UINT8 PrvWidthOutCap;         /**< Possible Width setting */
  UINT32 CompositeFrequencyCap; /**< Possible Link frequency setting */
  UINT32 ClumpingSupport;       /**< Unit ID Clumping value (bit 0 = passive support) */
} PORT_DESCRIPTOR;

/// Reference to a set of PORT_DESCRIPTORs.
typedef PORT_DESCRIPTOR (*PORT_LIST)[MAX_PLATFORM_LINKS*2];

/**
 * Our global state data structure
 */
typedef struct {
  AMD_HT_INTERFACE *HtBlock;  /**< The input data structure. */

  UINT8 NodesDiscovered;      /**< One less than the number of Nodes found in the system */
  UINT8 TotalLinks;           /**< How many HT Links have we discovered so far. */
  UINT8 SysMpCap;             /**< The maximum number of Nodes that all processors are capable of */
  AGESA_STATUS MaxEventClass; /**< The event class of the highest severity event generated */

  PORT_LIST PortList;         /**< Represent the system as a set of Links, each two Ports. */
  COHERENT_FABRIC *Fabric;                        /**< Describe metrics about the coherent fabric.
                                                   * Limited scope to CoherentInit().  */
  /* Data interface to other Agesa Modules */
  SOCKET_DIE_TO_NODE_MAP SocketDieToNodeMap;      /**< For each Socket, Die the Node ids */
  NODE_TO_SOCKET_DIE_MAP NodeToSocketDieMap;      /**< For each Node id, Socket and Die */
  HOP_COUNT_TABLE *HopCountTable;                 /**< Table of hops between nodes */

  /* Data for non-coherent initialization */
  UINT8 AutoBusCurrent;                      /**< The next bus number available */
  UINT8 UsedCfgMapEntries;                   /**< The next Config address Map set available, Limit 4 (F1X[EC:E0]) */
  BOOLEAN IsUsingRecoveryHt;                 /**< Manual BUID Swap List processing should assume that HT Recovery was used */
  BOOLEAN IsSetHtCrcFlood;                   /**< Enable setting of HT CRC Flood */
  BOOLEAN IsUsingUnitIdClumping;             /**< Enable automatic Unit Id Clumping configuration. */

  HT_INTERFACE *HtInterface;   /**< Interface for feature code to external parameters */
  HT_FEATURES *HtFeatures;     /**< The current feature implementations */
  NORTHBRIDGE *Nb;             /**< The current northbridge */

  PLATFORM_CONFIGURATION *PlatformConfiguration;     /**< The platform specific configuration customizations */
  VOID *ConfigHandle;          /**< Config Pointer, opaque handle for passing to lib */
} STATE_DATA;

//
// Feature Method types
//

/**
 * Discover all coherent devices in the system.
 *
 * @HtFeatInstances.
 *
 * @param[in,out] State     our global state
 *
 */
typedef VOID F_COHERENT_DISCOVERY (
  IN OUT   STATE_DATA *State
  );
/// Reference to a method.
typedef F_COHERENT_DISCOVERY *PF_COHERENT_DISCOVERY;

/**
 * Using the description of the fabric topology we discovered, try to find a match
 * among the supported topologies.
 *
 * @HtFeatInstances.
 *
 * @param[in,out]    State    the discovered fabric, degree matrix, permutation
 *
 */
typedef VOID F_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES (
  IN OUT   STATE_DATA    *State
  );
/// Reference to a method.
typedef F_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES *PF_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES;

/**
 * Make a Hop Count Table for the installed topology.
 *
 * @HtFeatInstances.
 *
 * @param[in,out]   State    access topology, permutation, update hop table
 *
 */
typedef VOID F_MAKE_HOP_COUNT_TABLE (
  IN OUT   STATE_DATA    *State
  );
/// Reference to a method.
typedef F_MAKE_HOP_COUNT_TABLE *PF_MAKE_HOP_COUNT_TABLE;

/**
 * Process a non-coherent Link.
 *
 * @HtFeatInstances.
 *
 * @param[in]     Node          Node on which to process nc init
 * @param[in]     Link          The non-coherent Link on that Node
 * @param[in]     IsCompatChain Is this the chain with the southbridge? TRUE if yes.
 * @param[in,out] State         our global state
 */
typedef VOID F_PROCESS_LINK (
  IN       UINT8         Node,
  IN       UINT8         Link,
  IN       BOOLEAN       IsCompatChain,
  IN OUT   STATE_DATA    *State
  );
/// Reference to a method.
typedef F_PROCESS_LINK *PF_PROCESS_LINK;

/**
 * Get Link features into system data structure.
 *
 * @HtFeatInstances.
 *
 * @param[in]     State our global state, port list
 */
typedef VOID F_GATHER_LINK_DATA (
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_GATHER_LINK_DATA *PF_GATHER_LINK_DATA;

/**
 * Optimize Links.
 *
 * @HtFeatInstances.
 *
 * @param[in,out]    State         Process and update portlist
 */
typedef VOID F_SELECT_OPTIMAL_WIDTH_AND_FREQUENCY (
  IN OUT   STATE_DATA   *State
  );
/// Reference to a method.
typedef F_SELECT_OPTIMAL_WIDTH_AND_FREQUENCY *PF_SELECT_OPTIMAL_WIDTH_AND_FREQUENCY;

/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 *
 * @HtFeatInstances.
 *
 * @param[in]     State   our global state, port list
 */
typedef VOID F_SET_LINK_DATA (
  IN       STATE_DATA   *State
  );
/// Reference to a method.
typedef F_SET_LINK_DATA *PF_SET_LINK_DATA;

/**
 * Retry must be enabled on all coherent links if it is enabled on any coherent links.
 *
 * @HtFeatInstances.
 *
 * @param[in,out]   State       global state, port frequency settings.
 *
 * @retval          TRUE        Fixup occurred, all coherent links HT1
 * @retval          FALSE       No changes
 */
typedef BOOLEAN F_IS_COHERENT_RETRY_FIXUP (
  IN       STATE_DATA       *State
  );
/// Reference to a method.
typedef F_IS_COHERENT_RETRY_FIXUP *PF_IS_COHERENT_RETRY_FIXUP;


/**
 * Test the subLinks of a Link to see if they qualify to be reganged.
 *
 * @HtFeatInstances.
 *
 * @param[in,out] State          Our global state
 */
typedef VOID F_REGANG_LINKS (
  IN OUT   STATE_DATA *State
  );
/// Reference to a method.
typedef F_REGANG_LINKS *PF_REGANG_LINKS;

/**
 * Iterate through all Links, checking the frequency of each subLink pair.
 *
 * @HtFeatInstances.
 *
 * @param[in,out]     State     Link state and port list
 *
 */
typedef VOID F_SUBLINK_RATIO_FIXUP (
  IN OUT   STATE_DATA    *State
  );
/// Reference to a method.
typedef F_SUBLINK_RATIO_FIXUP *PF_SUBLINK_RATIO_FIXUP;

/**
 * Identify Links which can have traffic distribution.
 *
 * @HtFeatInstances.
 *
 * @param[in]   State   port list data
 */
typedef VOID F_TRAFFIC_DISTRIBUTION (
  IN       STATE_DATA *State
  );
/// Reference to a method.
typedef F_TRAFFIC_DISTRIBUTION *PF_TRAFFIC_DISTRIBUTION;

/**
 * Access HT Link Control Register.
 *
 * @HtFeatInstances.
 *
 * @param[in]     Reg      the PCI config address the control register
 * @param[in]     HiBit    the high bit number
 * @param[in]     LoBit    the low bit number
 * @param[in]     Value    the value to write to that bit range. Bit 0 => loBit.
 * @param[in]     State    Our state, config handle for lib
 */
typedef VOID F_SET_HT_CONTROL_REGISTER_BITS (
  IN       PCI_ADDR    Reg,
  IN       UINT8       HiBit,
  IN       UINT8       LoBit,
  IN       UINT32      *Value,
  IN       STATE_DATA  *State
  );
/// Reference to a method.
typedef F_SET_HT_CONTROL_REGISTER_BITS *PF_SET_HT_CONTROL_REGISTER_BITS;

/**
 * Translate a desired width setting to the bits to set in the register field.
 *
 * @HtFeatInstances.
 *
 * @param[in]     Value   the width Value
 *
 * @return The bits for the register
 */
typedef UINT8 F_CONVERT_WIDTH_TO_BITS (
  IN       UINT8       Value
  );
/// Reference to a method.
typedef F_CONVERT_WIDTH_TO_BITS *PF_CONVERT_WIDTH_TO_BITS;

/**
 * HT Feature Methods.
 *
 * Provides abstract methods which are bound to specific feature implementations.
 */
struct _HT_FEATURES {
  PF_COHERENT_DISCOVERY CoherentDiscovery;    /**< Method: Coherent Discovery. */
  PF_LOOKUP_COMPUTE_AND_LOAD_ROUTING_TABLES LookupComputeAndLoadRoutingTables;
                                              /**< Method: Route the discovered system */
  PF_MAKE_HOP_COUNT_TABLE MakeHopCountTable;  /**< Method: Compute slit hop counts */
  PF_PROCESS_LINK ProcessLink;                /**< Method: Process a non-coherent Link. */
  PF_GATHER_LINK_DATA GatherLinkData;         /**< Method: Gather Link Capabilities and data. */
  PF_SELECT_OPTIMAL_WIDTH_AND_FREQUENCY SelectOptimalWidthAndFrequency;
                                              /**< Method: Optimize link features. */
  PF_REGANG_LINKS RegangLinks;                /**< Method: Regang Sublinks. */
  PF_SUBLINK_RATIO_FIXUP SubLinkRatioFixup;   /**< Method: Fix Sublink Frequency ratios */
  PF_IS_COHERENT_RETRY_FIXUP IsCoherentRetryFixup;
                                              /**< Method: Fix Retry mixed on coherent links. */
  PF_SET_LINK_DATA SetLinkData;               /**< Method: Set optimized values. */
  PF_TRAFFIC_DISTRIBUTION TrafficDistribution; /**< Method: Detect and Initialize Traffic Distribution */
  PF_SET_HT_CONTROL_REGISTER_BITS SetHtControlRegisterBits; /**< Method: Access HT Link Control Reg. */
  PF_CONVERT_WIDTH_TO_BITS ConvertWidthToBits; /**< Method: Convert a bit width to the value used for register setting. */
} ;

/*----------------------------------------------------------------------------
 *                         Prototypes
 *
 *----------------------------------------------------------------------------
 */

/**
 * Provide the current Feature set implementation.
 *
 * Add an implementation reference for the constructor, just to make sure the page is created.
 * @HtFeatMethod{_HT_FEATURES}.
 *
 */
VOID
NewHtFeatures (
     OUT   HT_FEATURES          *HtFeatures,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );


#endif  /* _HT_FEAT_H_ */
