#ifndef __BDK_HAL_USB_H__
#define __BDK_HAL_USB_H__
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
 * Interface to USB3 or USB2.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @addtogroup hal
 * @{
 */

typedef enum
{
    BDK_USB_TEST_USB2_DISABLE,
    BDK_USB_TEST_USB2_J_STATE,
    BDK_USB_TEST_USB2_K_STATE,
    BDK_USB_TEST_USB2_SE0_NAK,
    BDK_USB_TEST_USB2_PACKET,
    BDK_USB_TEST_USB2_FORCE_ENABLE,
    BDK_USB_XHCI_INIT,
    BDK_USB_XHCI_LIST_ADDRESSES,
    BDK_USB_XHCI_POLL_STATUS,
    BDK_USB_XHCI_TOGGLE_POLLING,
    BDK_USB_TEST_USB2_LAST,
} bdk_usb_test_t;

typedef enum
{
    BDK_USB_CLOCK_SS_PAD_HS_PAD = 0x0,    /* Superspeed and high speed use PAD clock */
    BDK_USB_CLOCK_SS_REF0_HS_REF0 = 0x1,  /* Superspeed and high speed use DLM/QLM ref clock 0 */
    BDK_USB_CLOCK_SS_REF1_HS_REF1 = 0x2,  /* Superspeed and high speed use DLM/QLM ref clock 1 */
    BDK_USB_CLOCK_SS_PAD_HS_PLL = 0x3,    /* Superspeed uses PAD clock, high speed uses PLL ref clock */
    BDK_USB_CLOCK_SS_REF0_HS_PLL = 0x4,   /* Superspeed uses DLM/QLM ref clock 0, high speed uses PLL ref clock */
    BDK_USB_CLOCK_SS_REF1_HS_PLL = 0x5,   /* Superspeed uses DLM/QLM ref clock 1, high speed uses PLL ref clock */
} bdk_usb_clock_t;

/**
 * Initialize the clocks for USB such that it is ready for a generic XHCI driver
 *
 * @param node       Node to init
 * @param usb_port   Port to intialize
 * @param clock_type Type of clock connected to the usb port
 *
 * @return Zero on success, negative on failure
 */
int bdk_usb_initialize(bdk_node_t node, int usb_port, bdk_usb_clock_t clock_type);

/**
 * Put the USB port into a specific testing mode
 *
 * @param node      Node to use
 * @param usb_port  Port to use
 * @param test_mode USB test mode
 *
 * @return Zero on success, negative on failure
 */
int bdk_usb_test_mode(bdk_node_t node, int usb_port, bdk_usb_test_t test_mode);

/**
 * Convert a USB test enumeration into a string for display to the user
 *
 * @param node      Node to use
 * @param usb_port  Port to use
 * @param test_mode Mode to convert
 *
 * @return String name of test
 */
const char *bdk_usb_get_test_mode_string(bdk_node_t node, int usb_port, bdk_usb_test_t test_mode);

/** @} */
#endif
