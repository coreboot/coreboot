#ifndef __BDK_HAL_H__
#define __BDK_HAL_H__
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
 * Master include file for hardware support. Use bdk.h instead
 * of including this file directly.
 *
 * <hr>$Revision: 49448 $<hr>
 *
 * @defgroup hal Hardware abstraction layer
 */

/* Global define to control if the BDK configures units to send
    don't write back requests for freed buffers. Set to 1 to enable
    DWB, 0 to disable them. As the BDK normally fits inside L2, sending
    DWB just causes more L2 operations without benefit */
#define BDK_USE_DWB 0

#include "bdk-access.h"
#include "bdk-utils.h"
#include "bdk-config.h"
#include "bdk-atomic.h"
#include "bdk-spinlock.h"
#include "bdk-rvu.h"
#include "bdk-clock.h"
#include "bdk-crc.h"
#include "bdk-error-report.h"
#include "bdk-gpio.h"
#include "device/bdk-device.h"
#include "if/bdk-if.h"
#include "usb/bdk-usb-xhci-intf.h"
#include "bdk-ecam.h"
#include "bdk-fpa.h"
#include "bdk-pbus-flash.h"
#include "bdk-pki.h"
#include "bdk-pko.h"
#include "bdk-power-burn.h"
#include "bdk-sso.h"
#include "bdk-nic.h"
#include "bdk-nix.h"
#include "bdk-key.h"
#include "bdk-l2c.h"
#include "bdk-mdio.h"
#include "bdk-mpi.h"
#include "bdk-mmc.h"
#include "bdk-pcie.h"
#include "bdk-pcie-flash.h"
#include "bdk-qlm.h"
#include "qlm/bdk-qlm-errata-cn8xxx.h"
#include "bdk-rng.h"
#include "bdk-sata.h"
#include "bdk-twsi.h"
#include "bdk-usb.h"
#include "bdk-access-native.h"
#include "bdk-tns.h"
#include "bdk-vrm.h"
#include "aq_api/bdk-aqr-support.h"
#endif
