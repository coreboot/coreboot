/* $NoKeywords:$ */
/**
 * @file
 *
 * mrtspd3.h
 *
 * Technology SPD support for DDR3 Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#define SPD_TYPE          2              /* SPD byte read location */
#define JED_DDR_SDRAM     7              /* Jedec defined bit field */
#define JED_DDR2_SDRAM    8              /* Jedec defined bit field */
#define JED_DDR3SDRAM   0xB              /* Jedec defined bit field */

#define SPD_DIMM_TYPE    3
#define SPD_ATTRIB      21
#define JED_DIF_CK_MSK    0x20            /* Differential Clock Input */
#define JED_RDIMM       1
#define JED_MINIRDIMM   5
#define JED_UDIMM       2
#define JED_SODIMM       3

#define SPD_L_BANKS      4               /* [7:4] number of [logical] banks on each device */
#define SPD_DENSITY     4               /* bit 3:0 */
#define SPD_ROW_SZ       5               /* bit 5:3 */
#define SPD_COL_SZ       5               /* bit 2:0 */
#define SPD_MNVVDD    6
#define SPD_RANKS       7               /* bit 5:3 */
#define SPD_DEV_WIDTH    7               /* bit 2:0 */
#define SPD_ECCBITS     8               /* bit 4:3 */
#define JED_ECC         8
#define SPD_RAWCARD     62              /* bit 2:0 */
#define SPD_ADDRMAP     63              /* bit 0 */

#define SPD_CTLWRD03     70              /* bit 7:4 */
#define SPD_CTLWRD04     71              /* bit 3:0 */
#define SPD_CTLWRD05     71              /* bit 7:4 */

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


