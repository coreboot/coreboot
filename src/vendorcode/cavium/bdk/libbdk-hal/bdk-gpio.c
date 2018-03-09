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

/* This code is an optional part of the BDK. It is only linked in
    if BDK_REQUIRE() needs it */
BDK_REQUIRE_DEFINE(GPIO);

/**
 * Initialize a single GPIO as either an input or output. If it is
 * an output, also set its output value.
 *
 * @param gpio      GPIO to initialize
 * @param is_output Non zero if this GPIO should be an output
 * @param output_value
 *                  Value of the GPIO if it should be an output. Not used if the
 *                  GPIO isn't an output.
 *
 * @return Zero on success, negative ob failure
 */
int bdk_gpio_initialize(bdk_node_t node, int gpio, int is_output, int output_value)
{
    if ((gpio >= 0) && (gpio < bdk_gpio_get_num()))
    {
        int gpio_group = gpio >> 6;
        int gpio_index = gpio & 63;
        if (output_value)
            bdk_gpio_set(node, gpio_group, 1ull << gpio_index);
        else
            bdk_gpio_clear(node, gpio_group, 1ull << gpio_index);

        BDK_CSR_DEFINE(cfg, BDK_GPIO_BIT_CFGX(gpio));
        cfg.u = 0;
        cfg.s.tx_oe = !!is_output;
        BDK_CSR_WRITE(node, BDK_GPIO_BIT_CFGX(gpio), cfg.u);
    }
    else
    {
        bdk_error("bdk_gpio_initialize: Illegal GPIO\n");
        return -1;
    }
    return 0;
}


/**
 * GPIO Read Data
 *
 * @param node       Node GPIO block is on
 * @param gpio_block GPIO block to access. Each block contains up to 64 GPIOs
 *
 * @return Status of the GPIO pins for the given block
 */
uint64_t bdk_gpio_read(bdk_node_t node, int gpio_block)
{
    bdk_gpio_rx_dat_t gpio_rx_dat;
    switch (gpio_block)
    {
        case 0:
            gpio_rx_dat.u = BDK_CSR_READ(node, BDK_GPIO_RX_DAT);
            break;
        case 1:
            gpio_rx_dat.u = BDK_CSR_READ(node, BDK_GPIO_RX1_DAT);
            break;
        default:
            bdk_error("GPIO block %d not supported\n", gpio_block);
            gpio_rx_dat.u = 0;
            break;
    }
    return gpio_rx_dat.s.dat;
}


/**
 * GPIO Clear pin
 *
 * @param node       Node GPIO block is on
 * @param gpio_block GPIO block to access. Each block contains up to 64 GPIOs
 * @param clear_mask Bit mask to indicate which bits to drive to '0'.
 */
void bdk_gpio_clear(bdk_node_t node, int gpio_block, uint64_t clear_mask)
{
    switch (gpio_block)
    {
        case 0:
            BDK_CSR_WRITE(node, BDK_GPIO_TX_CLR, clear_mask);
            break;
        case 1:
            BDK_CSR_WRITE(node, BDK_GPIO_TX1_CLR, clear_mask);
            break;
        default:
            bdk_error("GPIO block %d not supported\n", gpio_block);
            break;
    }
}


/**
 * GPIO Set pin
 *
 * @param node       Node GPIO block is on
 * @param gpio_block GPIO block to access. Each block contains up to 64 GPIOs
 * @param set_mask   Bit mask to indicate which bits to drive to '1'.
 */
void bdk_gpio_set(bdk_node_t node, int gpio_block, uint64_t set_mask)
{
    switch (gpio_block)
    {
        case 0:
            BDK_CSR_WRITE(node, BDK_GPIO_TX_SET, set_mask);
            break;
        case 1:
            BDK_CSR_WRITE(node, BDK_GPIO_TX1_SET, set_mask);
            break;
        default:
            bdk_error("GPIO block %d not supported\n", gpio_block);
            break;
    }
}


/** GPIO Select pin
 *
 * @param node      CPU node
 * @param gpio      GPIO number
 * @param pin       Pin number
 */
void bdk_gpio_select_pin(bdk_node_t node, int gpio, int pin)
{
    if ((gpio < 0) || (gpio >= bdk_gpio_get_num()))
    {
        bdk_warn("bdk_gpio_select_pin: Illegal GPIO %d\n", gpio);
        return;
    }

    BDK_CSR_MODIFY(c, node, BDK_GPIO_BIT_CFGX(gpio), c.s.pin_sel = pin);
}


/**
 * Return the number of GPIO pins on this chip
 *
 * @return Number of GPIO pins
 */
int bdk_gpio_get_num(void)
{
    if (CAVIUM_IS_MODEL(CAVIUM_CN88XX))
        return 51;
    else if (CAVIUM_IS_MODEL(CAVIUM_CN81XX))
        return 48;
    else if (CAVIUM_IS_MODEL(CAVIUM_CN83XX))
        return 80;
    else if (CAVIUM_IS_MODEL(CAVIUM_CN93XX))
        return 96;
    else
    {
        bdk_error("bdk_gpio_get_num(): Unsupported chip");
        return 0;
    }
}
