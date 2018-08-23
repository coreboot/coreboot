/* $NoKeywords:$ */
/**
 * @file
 *
 * IDS Option File
 *
 * This file is used to switch on/off IDS features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 192403 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2017, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/

#include <check_for_wrapper.h>

#include "Ids.h"

#ifndef _OPTION_IDS_H_
#define _OPTION_IDS_H_

/**
 *
 *  This file generates the defaults tables for the Integrated Debug Support
 * Module. The documented build options are imported from a user controlled
 * file for processing. The build options for the Integrated Debug Support
 * Module are listed below:
 *
 *    IDSOPT_IDS_ENABLED
 *    IDSOPT_CONTROL_ENABLED
 *
 *      Warning: When you enable the IDSOPT_CONTROL_NV_TO_CMOS feature.
 *      please make the cmos region defined by IDS_OPT_CMOS_REGION_START &
 *      IDS_OPT_CMOS_REGION_END can be touched between IDS HOOK point
 *      IDS_CPU_Early_Override and IDS_BEFORE_AP_EARLY_HALT of BSP
 *
 *      IDSOPT_CONTROL_NV_TO_CMOS
 *        IDS_OPT_CMOS_INDEX_PORT
 *        IDS_OPT_CMOS_DATA_PORT
 *        IDS_OPT_CMOS_REGION_START
 *        IDS_OPT_CMOS_REGION_END
 *
 *    IDSOPT_TRACING_ENABLED
 *      IDSOPT_TRACING_CONSOLE_HDTOUT
 *      IDSOPT_TRACING_CONSOLE_SERIALPORT
 *        IDSOPT_SERIAL_PORT (default 0x3F8)
 *      IDSOPT_TRACING_CONSOLE_REDIRECT_IO
 *        IDSOPT_DEBUG_PRINT_IO_PORT (default 0x80)
 *      IDSOPT_TRACING_CONSOLE_RAM
 *        IDSOPT_DPRAM_BASE
 *        IDSOPT_DPRAM_SIZE
 *        IDSOPT_DPRAM_STOP_LOGGING_WHEN_BUFFER_FULL (default FALSE)
 *      IDSOPT_CUSTOMIZE_TRACING_SERVICE
 *      IDSOPT_CUSTOMIZE_TRACING_SERVICE_INIT
 *      IDSOPT_CUSTOMIZE_TRACING_SERVICE_EXIT
 *
 *    IDSOPT_TRACE_BLD_CFG
 *    IDSOPT_PERF_ANALYSIS
 *    IDSOPT_ASSERT_ENABLED
 *    IDS_DEBUG_PORT
 *    IDS_DEBUG_PORT_SIZE_IN_BYTES
 *    IDS_DEBUG_TP_PREFIX
 *    IDSOPT_CAR_CORRUPTION_CHECK_ENABLED
 *    IDSOPT_DEBUG_CODE_ENABLED
 *    IDSOPT_IDT_EXCEPTION_TRAP
 *    IDSOPT_C_OPTIMIZATION_DISABLED
 *
 **/

#define IDS_MMAP_SERIAL_PORT

#ifdef DEBUG
#define IDSOPT_IDS_ENABLED                    TRUE
//#define IDSOPT_CONTROL_ENABLED                FALSE
#define IDSOPT_TRACING_ENABLED                TRUE
#define IDSOPT_TRACING_CONSOLE_SERIALPORT     TRUE
#ifdef IDS_DEBUG_PRINT_MASK
#undef IDS_DEBUG_PRINT_MASK
#endif
#define IDS_DEBUG_PRINT_MASK                  (GNB_TRACE_ALL | GFX_MISC | CPU_TRACE_ALL | MEM_STATUS | TOPO_TRACE_ALL | FCH_TRACE_ALL | MAIN_FLOW | IDS_TRACE_DEFAULT | TEST_POINT)
#ifdef IDS_MMAP_SERIAL_PORT
#define IDSOPT_SERIAL_PORT                    0xfedc6000
#else
#define IDSOPT_SERIAL_PORT                    0x3F8
#endif
#define IDSOPT_HEAP_CHECKING                  TRUE
#define IDSOPT_TRACE_BLD_CFG                  TRUE
#define IDSOPT_CAR_CORRUPTION_CHECK_ENABLED   FALSE
#define IDSOPT_DEBUG_CODE_ENABLED             TRUE
#define IDSOPT_C_OPTIMIZATION_DISABLED        TRUE
//#define IDSOPT_ASSERT_ENABLED                 TRUE
#else
#define IDSOPT_IDS_ENABLED                    FALSE
//#define IDSOPT_CONTROL_ENABLED                FALSE
#define IDSOPT_TRACING_ENABLED                FALSE
#define IDSOPT_TRACING_CONSOLE_SERIALPORT     TRUE
#define IDS_DEBUG_PRINT_MASK                  (0)
#endif

#endif
