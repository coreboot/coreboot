/* $NoKeywords:$ */
/**
 * @file
 *
 * mtspd3.h
 *
 * Technology SPD support for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
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
  * ***************************************************************************
  *
 */

#ifndef _MTSPD3_H_
#define _MTSPD3_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*===============================================================================
 *   Jedec DDR III
 *===============================================================================
 */
#define SPD_BYTE_USED     0
#define SPD_TYPE         2              /* SPD byte read location */
#define JED_DDR_SDRAM     7              /* Jedec defined bit field */
#define JED_DDR2_SDRAM    8              /* Jedec defined bit field */
#define JED_DDR3SDRAM    0xB            /* Jedec defined bit field */

#define SPD_DIMM_TYPE    3
#define SPD_ATTRIB      21
#define JED_DIF_CK_MSK    0x20            /* Differential Clock Input */
#define JED_RDIMM       1
#define JED_MINIRDIMM   5
#define JED_UDIMM       2
#define JED_SODIMM       3
#define JED_LRDIMM      0xB
#define JED_UNDEFINED   0                /* Undefined value */

#define SPD_L_BANKS      4               /* [7:4] number of [logical] banks on each device */
#define SPD_DENSITY     4               /* bit 3:0 */
#define SPD_ROW_SZ       5               /* bit 5:3 */
#define SPD_COL_SZ       5               /* bit 2:0 */
#define SPD_RANKS       7               /* bit 5:3 */
#define SPD_DEV_WIDTH    7               /* bit 2:0 */
#define SPD_ECCBITS     8               /* bit 4:3 */
#define JED_ECC         8
#define SPD_RAWCARD     62              /* bit 2:0 */
#define SPD_ADDRMAP     63              /* bit 0 */

#define SPD_CTLWRD03     70              /* bit 7:4 */
#define SPD_CTLWRD04     71              /* bit 3:0 */
#define SPD_CTLWRD05     71              /* bit 7:4 */

#define SPD_FTB          9

#define SPD_DIVIDENT    10
#define SPD_DIVISOR     11

#define SPD_TCK         12
#define SPD_CASLO       14
#define SPD_CASHI       15
#define SPD_TAA         16

#define SPD_TRP         20
#define SPD_TRRD        19
#define SPD_TRCD        18
#define SPD_TRAS        22
#define SPD_TWR         17
#define SPD_TWTR        26
#define SPD_TRTP        27
#define SPD_TRC         23
#define SPD_UPPER_TRC   21              /* bit 7:4 */
#define SPD_UPPER_TRAS  21              /* bit 3:0 */
#define SPD_TFAW        29
#define SPD_UPPER_TFAW  28              /* bit 3:0 */

#define SPD_TCK_FTB     34
#define SPD_TAA_FTB     35
#define SPD_TRCD_FTB    36
#define SPD_TRP_FTB     37
#define SPD_TRC_FTB     38

/*-----------------------------
 * Jedec DDR II related equates
 *-----------------------------
 */

#define CL_DEF          4              /* Default value for failsafe operation. 4=CL 6.0 T */
#define T_DEF           4              /* Default value for failsafe operation. 4=2.5ns (cycle time) */

#define BIAS_TRTP_T      4
#define BIAS_TRCD_T      5
#define BIAS_TRAS_T      15
#define BIAS_TRC_T       11
#define BIAS_TRRD_T      4
#define BIAS_TWR_T       4
#define BIAS_TRP_T       5
#define BIAS_TWTR_T      4
#define BIAS_TFAW_T      14

#define MIN_TRTP_T       4
#define MAX_TRTP_T       7
#define MIN_TRCD_T       5
#define MAX_TRCD_T       12
#define MIN_TRAS_T       15
#define MAX_TRAS_T       30
#define MIN_TRC_T        11
#define MAX_TRC_T        42
#define MIN_TRRD_T       4
#define MAX_TRRD_T       7
#define MIN_TWR_T        5
#define MAX_TWR_T        12
#define MIN_TRP_T        5
#define MAX_TRP_T        12
#define MIN_TWTR_T       4
#define MAX_TWTR_T       7
#define MIN_TFAW_T       16
#define MAX_TFAW_T       32

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */


#endif  /* _MTSPD3_H_ */


