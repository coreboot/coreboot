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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 *
 ***************************************************************************/
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
 *      IDSOPT_CUSTOMIZE_TRACING_SERVICE
 *
 *      IDSOPT_TRACING_CONSOLE_HDTOUT
 *      IDSOPT_TRACING_CONSOLE_SERIALPORT
 *        IDSOPT_SERIAL_PORT (default 0x3F8)
 *      IDSOPT_TRACING_CONSOLE_REDIRECT_IO
 *        IDSOPT_DEBUG_PRINT_IO_PORT (default 0x80)
 *    IDSOPT_TRACE_BLD_CFG
 *    IDSOPT_PERF_ANALYSIS
 *    IDSOPT_ASSERT_ENABLED
 *    IDS_DEBUG_PORT
 *    IDSOPT_CAR_CORRUPTION_CHECK_ENABLED
 *    IDSOPT_DEBUG_CODE_ENABLED
 *    IDSOPT_IDT_EXCEPTION_TRAP
 *    IDSOPT_C_OPTIMIZATION_DISABLED
 *
 **/

#endif
