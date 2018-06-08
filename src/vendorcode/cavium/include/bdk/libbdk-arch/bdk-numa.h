/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/

/**
 * @file
 *
 * Functions for dealing with multiple chips organized into a
 * NUMA cluster.
 *
 * <hr>$Revision: 49448 $<hr>
 */

typedef enum
{
    BDK_NODE_0 = 0,
    BDK_NODE_1 = 1,
    BDK_NODE_2 = 2,
    BDK_NODE_3 = 3,
    BDK_NUMA_MAX_NODES = 4
} bdk_node_t;

/**
 * Return the local node number
 *
 * @return Node number
 */
static inline bdk_node_t bdk_numa_local(void) __attribute__ ((always_inline, pure));
static inline bdk_node_t bdk_numa_local(void)
{
#ifndef BDK_BUILD_HOST
    int mpidr_el1;
    BDK_MRS_NV(MPIDR_EL1, mpidr_el1);
    int result;
    result = (mpidr_el1 >> 16) & 0xff;
    return BDK_NODE_0 + result;
#else
    return BDK_NODE_0; /* FIXME: choose remote node */
#endif
}

/**
 * Return the master node number
 *
 * @return Node number
 */
static inline bdk_node_t bdk_numa_master(void)
{
    extern int __bdk_numa_master_node;
    return __bdk_numa_master_node;
}

/**
 * Get a bitmask of the nodes that exist
 *
 * @return bitmask
 */
extern uint64_t bdk_numa_get_exists_mask(void);

/**
 * Add a node to the exists mask
 *
 * @param node   Node to add
 */
extern void bdk_numa_set_exists(bdk_node_t node);

/**
 * Return true if a node exists
 *
 * @param node   Node to check
 *
 * @return Non zero if the node exists
 */
extern int bdk_numa_exists(bdk_node_t node);

/**
 * Return true if there is only one node
 *
 * @return
 */
extern int bdk_numa_is_only_one();

/**
 * Given a physical address without a node, return the proper physical address
 * for the given node.
 *
 * @param node   Node to create address for
 * @param pa     Base physical address
 *
 * @return Node specific address
 */
static inline uint64_t bdk_numa_get_address(bdk_node_t node, uint64_t pa) __attribute__((pure,always_inline));
static inline uint64_t bdk_numa_get_address(bdk_node_t node, uint64_t pa)
{
    if (pa & (1ull << 47))
        pa |= (uint64_t)(node&3) << 44;
    else if (CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        pa |= (uint64_t)(node & 3) << 40; /* CN8XXX uses bits [41:40] for nodes */
    else
        pa |= (uint64_t)(node & 3) << 44; /* CN9XXX uses bits [45:44] for nodes */
    return pa;
}


