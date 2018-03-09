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
#include "libbdk-arch/bdk-csrs-mio_tws.h"

/**
 * Report boot status to the BMC or whomever might care. This function
 * will return quickly except for a status of "power cycle". In the power cycle
 * case it is assumed the board is in a bad state and should not continue until
 * a power cycle restarts us.
 *
 * @param status Status to report. Enumerated in bdk_boot_status_t
 */
void bdk_boot_status(bdk_boot_status_t status)
{
    bdk_node_t node = bdk_numa_master();
    int twsi = bdk_config_get_int(BDK_CONFIG_BMC_TWSI);

    /* Update status */
    if (twsi != -1)
    {
        BDK_CSR_DEFINE(sw_twsi, BDK_MIO_TWSX_SW_TWSI(twsi));
        sw_twsi.u = 0;
        sw_twsi.s.v = 1; /* Valid data */
        sw_twsi.s.slonly = 1; /* Slave only */
        sw_twsi.s.data = status;
        BDK_CSR_WRITE(node, BDK_MIO_TWSX_SW_TWSI(twsi), sw_twsi.u);
    }

    /* As a special case, power cycle will display a message and try a
       soft reset if we can't power cycle in 5 seconds */
    if (status == BDK_BOOT_STATUS_REQUEST_POWER_CYCLE)
    {
        if (twsi != -1)
        {
            printf("Requested power cycle\n");
            bdk_wait_usec(5000000); /* 5 sec */
            printf("Power cycle failed, trying soft reset\n");
        }
        else
            printf("Performing soft reset\n");
        bdk_reset_chip(node);
    }
}

