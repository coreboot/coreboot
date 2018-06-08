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
 * General Purpose IO interface.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */


/**
 * Initialize a single GPIO as either an input or output. If it is
 * an output, also set its output value.
 *
 * @param node      Node to use in a Numa setup. Can be an exact ID or a special
 *                  value.
 * @param gpio      GPIO to initialize
 * @param is_output Non zero if this GPIO should be an output
 * @param output_value
 *                  Value of the GPIO if it should be an output. Not used if the
 *                  GPIO isn't an output.
 *
 * @return Zero on success, negative ob failure
 */
extern int bdk_gpio_initialize(bdk_node_t node, int gpio, int is_output, int output_value);

/**
 * GPIO Read Data
 *
 * @param node       Node GPIO block is on
 * @param gpio_block GPIO block to access. Each block contains up to 64 GPIOs
 *
 * @return Status of the GPIO pins for the given block
 */
extern uint64_t bdk_gpio_read(bdk_node_t node, int gpio_block);

/**
 * GPIO Clear pin
 *
 * @param node       Node GPIO block is on
 * @param gpio_block GPIO block to access. Each block contains up to 64 GPIOs
 * @param clear_mask Bit mask to indicate which bits to drive to '0'.
 */
extern void bdk_gpio_clear(bdk_node_t node, int gpio_block, uint64_t clear_mask);

/**
 * GPIO Set pin
 *
 * @param node       Node GPIO block is on
 * @param gpio_block GPIO block to access. Each block contains up to 64 GPIOs
 * @param set_mask   Bit mask to indicate which bits to drive to '1'.
 */
extern void bdk_gpio_set(bdk_node_t node, int gpio_block, uint64_t set_mask);

/** GPIO Select pin
 *
 * @param node      CPU node
 * @param gpio      GPIO number
 * @param pin       Pin number
 */
extern void bdk_gpio_select_pin(bdk_node_t node, int gpio, int pin);

/**
 * Return the number of GPIO pins on this chip
 *
 * @return Number of GPIO pins
 */
extern int bdk_gpio_get_num(void);

/** @} */
