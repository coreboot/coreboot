#ifndef __BDK_H__
#define __BDK_H__
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
 * Master include file for all BDK function.
 *
 * <hr>$Revision: 49448 $<hr>
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "../libbdk-arch/bdk-arch.h"
#include "../libbdk-os/bdk-os.h"
#include "../libfatfs/ff.h"
#include "../libfatfs/diskio.h"
#ifndef BDK_BUILD_HOST
#include "../libbdk-hal/bdk-hal.h"
#include "../libbdk-boot/bdk-boot.h"
#include "../libbdk-dram/bdk-dram.h"
#include "../libbdk-driver/bdk-driver.h"
#include "../libbdk-trust/bdk-trust.h"
#include "../libdram/libdram.h"
#include "bdk-functions.h"
#endif
#include "../libbdk-lua/bdk-lua.h"
#include "../libbdk-bist/bist.h"

/**
 * @mainpage
 *
 * This document goes through the internal details of the BDK.  Its purpose is
 * to serve as a API reference for people writing applications. Users of the
 * BDK's binary applications do not need these details.
 */

#endif
