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
#include <bdk.h>
#include "libbdk-arch/bdk-csrs-ap.h"
#include "libbdk-arch/bdk-csrs-l2c.h"
#include "libbdk-arch/bdk-csrs-l2c_cbc.h"
#include "libbdk-arch/bdk-csrs-mio_fus.h"
#include "libbdk-hal/bdk-l2c.h"
#include "libbdk-hal/bdk-utils.h"

typedef struct
{
    int sets;
    int ways;
    bool is_locked;
} l2_node_state_t;

static l2_node_state_t l2_node_state[BDK_NUMA_MAX_NODES];

int bdk_l2c_set_core_way_partition(bdk_node_t node, int core, uint32_t mask)
{
    uint32_t valid_mask = (1 << bdk_l2c_get_num_assoc(node)) - 1;
    mask &= valid_mask;

    BDK_CSR_WRITE(node, BDK_L2C_WPAR_PPX(core), mask);
    return 0;
}


int bdk_l2c_set_hw_way_partition(bdk_node_t node, uint32_t mask)
{
    uint32_t valid_mask = (1 << bdk_l2c_get_num_assoc(node)) - 1;
    mask &= valid_mask;

    BDK_CSR_WRITE(node, BDK_L2C_WPAR_IOBX(0), mask);
    return 0;
}

int bdk_l2c_get_hw_way_partition(bdk_node_t node)
{
    return (BDK_CSR_READ(node, BDK_L2C_WPAR_IOBX(0)) & 0xffff);
}

int bdk_l2c_get_cache_size_bytes(bdk_node_t node)
{
    return bdk_l2c_get_num_sets(node) * bdk_l2c_get_num_assoc(node) * BDK_CACHE_LINE_SIZE;
}

/* Return the number of sets in the L2 Cache */
int bdk_l2c_get_num_sets(bdk_node_t node)
{
    if (bdk_unlikely(l2_node_state[node].sets == 0))
    {
        /* Select the L2 cache */
        bdk_ap_csselr_el1_t csselr_el1;
        csselr_el1.u = 0;
        csselr_el1.s.ind = 0;
        csselr_el1.s.level = CAVIUM_IS_MODEL(CAVIUM_CN8XXX) ? 1 : 2;
        BDK_MSR(CSSELR_EL1, csselr_el1.u);
        /* Read its size */
        bdk_ap_ccsidr_el1_t ccsidr_el1;
        BDK_MRS(CCSIDR_EL1, ccsidr_el1.u);
        /* Store it for use later */
        l2_node_state[node].sets = ccsidr_el1.s.numsets + 1;
        l2_node_state[node].ways = ccsidr_el1.s.associativity + 1;

        /* Early chips didn't update the number of ways based on fusing */
        if ((l2_node_state[node].ways == 16) && CAVIUM_IS_MODEL(CAVIUM_CN8XXX))
        {
            /* The l2 can be reduced in 25% increments */
            BDK_CSR_INIT(mio_fus_dat3, node, BDK_MIO_FUS_DAT3);
            switch (mio_fus_dat3.s.l2c_crip)
            {
                case 3: /* 1/4 size */
                    l2_node_state[node].ways *= 1;
                    break;
                case 2: /* 1/2 size */
                    l2_node_state[node].ways *= 2;
                    break;
                case 1: /* 3/4 size */
                    l2_node_state[node].ways *= 3;
                    break;
                default: /* Full size */
                    l2_node_state[node].ways *= 4;
                    break;
            }
            l2_node_state[node].ways /= 4;
        }
    }
    return l2_node_state[node].sets;
}

/* Return the number of associations in the L2 Cache */
int bdk_l2c_get_num_assoc(bdk_node_t node)
{
    /* Get the number of sets if the global sets/ways is not setup */
    if (bdk_unlikely(l2_node_state[node].ways == 0))
        bdk_l2c_get_num_sets(node);
    return l2_node_state[node].ways;
}

int bdk_l2c_unlock_mem_region(bdk_node_t node, uint64_t start, uint64_t len)
{
    /* Round start/end to cache line boundaries */
    len += start & BDK_CACHE_LINE_MASK;
    start &= ~BDK_CACHE_LINE_MASK;
    len = (len + BDK_CACHE_LINE_MASK) & ~BDK_CACHE_LINE_MASK;
    void *ptr = (start) ? bdk_phys_to_ptr(start) : NULL;

    while (len > 0)
    {
        /* Must use invalidate version to release lock */
        BDK_CACHE_WBI_L2(ptr);
        ptr += BDK_CACHE_LINE_SIZE;
        len -= BDK_CACHE_LINE_SIZE;
    }

    l2_node_state[node].is_locked = false;
    return 0;
}
