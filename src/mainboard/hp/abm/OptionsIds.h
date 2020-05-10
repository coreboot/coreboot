/* SPDX-License-Identifier: GPL-2.0-only */

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
 * @e \$Revision: 12067 $   @e \$Date: 2009-04-11 04:34:13 +0800 (Sat, 11 Apr 2009) $
 */
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
 *    IDSOPT_ERROR_TRAP_ENABLED
 *    IDSOPT_CONTROL_ENABLED
 *    IDSOPT_TRACING_ENABLED
 *    IDSOPT_PERF_ANALYSIS
 *    IDSOPT_ASSERT_ENABLED
 *    IDS_DEBUG_PORT
 *    IDSOPT_CAR_CORRUPTION_CHECK_ENABLED
 *
 **/

#define IDSOPT_IDS_ENABLED     TRUE
//#define IDSOPT_CONTROL_ENABLED TRUE
//#define IDSOPT_TRACING_ENABLED TRUE
#define IDSOPT_TRACING_CONSOLE_SERIALPORT TRUE
//#define IDSOPT_PERF_ANALYSIS   TRUE
#define IDSOPT_ASSERT_ENABLED  TRUE
//#undef IDSOPT_DEBUG_ENABLED
//#define IDSOPT_DEBUG_ENABLED  FALSE
//#undef IDSOPT_HOST_SIMNOW
//#define IDSOPT_HOST_SIMNOW    FALSE
//#undef IDSOPT_HOST_HDT
//#define IDSOPT_HOST_HDT       FALSE
//#define IDS_DEBUG_PORT    0x80

#endif /* _OPTION_IDS_H_ */
