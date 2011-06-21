/* $NoKeywords:$ */
/**
 * @file
 *
 * mtspd2.h
 *
 * Technology SPD support for DDR2
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR2)
 * @e \$Revision: 34897 $ @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
 *
 **/
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * ***************************************************************************
 *
 */

#ifndef _MTSPD2_H_
#define _MTSPD2_H_

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
 *   Jedec DDR II
 *===============================================================================
 */
#define SPD_TYPE         2              /* SPD byte read location */
#define JED_DDR_SDRAM     7              /* Jedec defined bit field */
#define JED_DDR2_SDRAM    8              /* Jedec defined bit field */

#define SPD_DIMM_TYPE    20
#define SPD_ATTRIB       21
#define JED_DIF_CK_MSK   0x20            /* Differential Clock Input */
#define JED_REG_ADC_MSK  0x11            /* Registered Address/Control */
#define JED_PROBE_MSK    0x40            /* Analysis Probe installed */
#define JED_SODIMM       0x04            /* SO-DIMM */
#define SPD_DEV_ATTRIB   22
#define SPD_EDC_TYPE     11
#define JED_ECC          2
#define JED_ADRC_PAR     4
#define SPD_ROW_SZ       3
#define SPD_COL_SZ       4
#define SPD_L_BANKS      17              /* number of [logical] banks on each device */
#define SPD_DM_BANKS     5               /* number of physical banks on dimm */
#define SP_DPL_BIT       4               /*  Dram package bit */
#define SPD_BANK_SZ      31              /* capacity of physical bank */
#define SPD_DEV_WIDTH    13
#define SPD_CAS_LAT      18
#define SPD_TRP         27
#define SPD_TRRD        28
#define SPD_TRCD        29
#define SPD_TRAS        30
#define SPD_TWR         36
#define SPD_TWTR        37
#define SPD_TRTP        38
#define SPD_TRC         41
#define SPD_TRFC        42
#define SPD_CHECKSUM    63
#define SPD_MAN_DATE_YR   93             /* Module Manufacturing Year (BCD) */

#define SPD_MAN_DATE_WK   94             /* Module Manufacturing Week (BCD) */

/*-----------------------------
 * Jedec DDR II related equates
 *-----------------------------
 */
#define M_YEAR_06         0x06           /*  Manufacturing Year BCD encoding of 2006 - 06d */
#define M_WEEK_24         0x24           /*  Manufacturing Week BCD encoding of June - 24d */

#define J_MIN           0              /* j loop constraint. 1=CL 2.0 T */
#define J_MAX           5              /* j loop constraint. 5=CL 7.0 T */
#define K_MIN           1              /* k loop constraint. 1=200 MHz */
#define K_MAX           5              /* k loop constraint. 5=533 MHz */
#define CL_DEF          2              /* Default value for failsafe operation. 2=CL 4.0 T */
#define T_DEF           1              /* Default value for failsafe operation. 1=5ns (cycle time) */


#define BIAS_TCL_T       1
#define BIAS_TRP_T       3              /* bias to convert bus clocks to bit field value */
#define BIAS_TRRD_T      2
#define BIAS_TRCD_T      3
#define BIAS_TRAS_T      3
#define BIAS_TRC_T       11
#define BIAS_TRTP_T      1
#define BIAS_TWR_T       3
#define BIAS_TWTR_T      0
#define BIAS_TFAW_T      7

#define MIN_TRP_T        3                   /* min programmable value in busclocks */
#define MAX_TRP_T        6                   /* max programmable value in busclocks */
#define MIN_TRRD_T       2
#define MAX_TRRD_T       5
#define MIN_TRCD_T       3
#define MAX_TRCD_T       6
#define MIN_TRAS_T       5
#define MAX_TRAS_T       18
#define MIN_TRC_T        11
#define MAX_TRC_T        26
#define MIN_TRTP_T       2
#define MAX_TRTP_T       4
#define MIN_TWR_T        3
#define MAX_TWR_T        6
#define MIN_TWTR_T       1
#define MAX_TWTR_T       3

/* DDR2-1066 support */
#define BIAS_TRCD_T_1066      5
#define BIAS_TRAS_T_1066      15
#define BIAS_TRRD_T_1066      4
#define BIAS_TWR_T_1066       4
#define BIAS_TRP_T_1066       5
#define BIAS_TWTR_T_1066      4

#define MIN_TRCD_T_1066       5
#define MAX_TRCD_T_1066       12
#define MIN_TRAS_T_1066       15
#define MAX_TRAS_T_1066       30
#define MIN_TRC_T_1066        11
#define MAX_TRC_T_1066        42
#define MIN_TRRD_T_1066       4
#define MAX_TRRD_T_1066       7
#define MIN_TWR_T_1066        5
#define MAX_TWR_T_1066        8
#define MIN_TRP_T_1066        5
#define MAX_TRP_T_1066        12
#define MIN_TWTR_T_1066       4
#define MAX_TWTR_T_1066       7


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


#endif  /* _MTSPD2_H_ */


