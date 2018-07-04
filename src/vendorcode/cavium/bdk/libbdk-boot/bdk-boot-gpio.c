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
#include "libbdk-arch/bdk-csrs-gpio.h"
#include "libbdk-hal/bdk-config.h"
#include "libbdk-hal/bdk-gpio.h"
#include "libbdk-boot/bdk-boot-gpio.h"

/**
 * Configure GPIO on all nodes as part of booting
 */
void bdk_boot_gpio(void)
{
    const int NUM_GPIO = bdk_gpio_get_num();
    for (bdk_node_t n = BDK_NODE_0; n < BDK_NUMA_MAX_NODES; n++)
    {
        if (bdk_numa_exists(n))
        {
            for (int gpio = 0; gpio < NUM_GPIO; gpio++)
            {
                int pin_sel = bdk_config_get_int(BDK_CONFIG_GPIO_PIN_SELECT, gpio, n);
                if (pin_sel >= 0)
                {
                    BDK_TRACE(INIT, "Connecting N%d.GPIO%d to pin select 0x%x\n",
                        n, gpio, pin_sel);
                    bdk_gpio_select_pin(n, gpio, pin_sel);
                }
                int invert = bdk_config_get_int(BDK_CONFIG_GPIO_POLARITY, gpio, n);
                if (invert)
                    BDK_CSR_MODIFY(c, n, BDK_GPIO_BIT_CFGX(gpio), c.s.pin_xor = 1);
            }
        }
    }
}
