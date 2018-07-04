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
#include "dram-internal.h"

/* FIXME(dhendrix): added */
#include <libbdk-hal/bdk-l2c.h>
#include <libbdk-hal/bdk-utils.h>

int limit_l2_ways(bdk_node_t node, int ways, int verbose)
{
    int ways_max = bdk_l2c_get_num_assoc(node);
    int ways_min = 0;
    int errors = 0;

    if (ways >= ways_min && ways <= ways_max)
    {
        uint32_t valid_mask = (0x1 << ways_max) - 1;
        uint32_t mask = (valid_mask << ways) & valid_mask;
        if (verbose)
            printf("Limiting L2 to %d ways\n", ways);
        for (int i = 0; i < (int)bdk_get_num_cores(node); i++)
            errors += bdk_l2c_set_core_way_partition(node, i, mask);
        errors += bdk_l2c_set_hw_way_partition(node, mask);
    }
    else
    {
        errors++;
        printf("ERROR: invalid limit_l2_ways %d, must be between %d and %d\n",
               ways, ways_min, ways_max);
    }
    if (errors)
        puts("ERROR limiting L2 cache ways\n");

    return errors;
}

