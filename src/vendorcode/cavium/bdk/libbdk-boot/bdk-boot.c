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
#include <string.h>
#include "libbdk-hal/if/bdk-if.h"
#include "libbdk-arch/bdk-csrs-pem.h"
#include "libbdk-boot/bdk-boot-pcie.h"
#include "libbdk-boot/bdk-boot-qlm.h"
#include "libbdk-boot/bdk-boot-usb.h"
#include "libbdk-hal/bdk-pcie.h"
#include "libbdk-hal/bdk-mdio.h"
#include "libbdk-hal/bdk-qlm.h"
#include "libbdk-hal/bdk-ecam.h"
#include "libbdk-hal/bdk-rng.h"
#include "libbdk-boot/bdk-boot-gpio.h"
#include "libbdk-arch/bdk-csrs-iobn.h"
#include "libbdk-arch/bdk-csrs-dap.h"

/**
 * Configure hardware
 */
void bdk_boot(void)
{
    for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
    {
        if (bdk_numa_exists(n))
        {
            /* Allow CAP access from cores so we can read system registers through
               memory mapped addresses. See bdk_sysreg_read() */
            BDK_CSR_MODIFY(c, n, BDK_DAP_IMP_DAR, c.s.caben = 1);

            /* Enable IOBN */
            if (CAVIUM_IS_MODEL(CAVIUM_CN88XX) || CAVIUM_IS_MODEL(CAVIUM_CN81XX))
            {
                BDK_CSR_MODIFY(c, n, BDK_IOBNX_NCB0_HP(0),
                    c.s.hp = 1);
                if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
                    BDK_CSR_MODIFY(c, n, BDK_IOBNX_NCB0_HP(1),
                        c.s.hp = 0);
            }

            bdk_ecam_scan_all(n);
            bdk_mdio_init(n);
            bdk_qlm_init(n);
            bdk_rng_init(n);
        }
    }

    bdk_boot_gpio();
    bdk_boot_usb();
    bdk_boot_qlm();
    bdk_boot_pcie();

    /* Initialize PHYs */
    for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
    {
        if (bdk_numa_exists(n))
        {
            bdk_if_phy_setup(n);
        }
    }
}
