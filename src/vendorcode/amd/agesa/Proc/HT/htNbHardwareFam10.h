/* $NoKeywords:$ */
/**
 * @file
 *
 * Northbridge hardware definitions for Family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
 *
 */
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
#ifndef _HT_NB_HARDWARE_FAM10_H_
#define _HT_NB_HARDWARE_FAM10_H_

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/* CPU Northbridge Functions */
#define CPU_HTNB_FUNC_00                  0
#define CPU_HTNB_FUNC_04                  4
#define CPU_ADDR_FUNC_01                  1
#define CPU_NB_FUNC_03                    3
#define CPU_NB_FUNC_05                    5

/* Function 0 registers */
#define REG_ROUTE0_0X40                   0x40
#define REG_ROUTE1_0X44                   0x44
#define REG_NODE_ID_0X60                  0x60
#define REG_UNIT_ID_0X64                  0x64
#define REG_LINK_TRANS_CONTROL_0X68       0x68
#define REG_LINK_INIT_CONTROL_0X6C        0x6C
#define REG_HT_CAP_BASE_0X80              0x80
#define REG_HT_LINK_CLUMPING0_0X110       0x110
#define REG_HT_LINK_RETRY0_0X130          0x130
#define REG_HT_EXTENDED_NODE_ID_F0X160    0x160
#define HTREG_NODE_CPUCNT_4_0             0x1F
#define HTREG_EXTNODE_CPUCNT_7_5          0xE0
#define REG_HT_TRAFFIC_DIST_0X164         0x164
#define REG_LINK_GLOBAL_EXT_CONTROL_0x16C 0x16C
#define REG_HT_LINK_EXT_CONTROL0_0X170    0x170
#define REG_HT_LINK_INITIALIZATION_0X1A0  0x1A0
#define PAIR_SELECT_OFFSET                8
#define REG_HT_LINK_PAIR_DIST_0X1E0       0x1E0

/* Function 1 registers */
#define REG_ADDR_CONFIG_MAP0_1XE0         0xE0
#define CPU_ADDR_NUM_CONFIG_MAPS          4

/* Function 3 registers */
#define REG_NB_SRI_XBAR_BUF_3X70          0x70
#define REG_NB_MCT_XBAR_BUF_3X78          0x78
#define REG_NB_FIFOPTR_3XDC               0xDC
#define REG_NB_CAPABILITY_3XE8            0xE8
#define REG_NB_CPUID_3XFC                 0xFC
#define REG_NB_LINK_XCS_TOKEN0_3X148      0x148
#define REG_NB_MCA_LINK_THRESHOLD_3X168   0x168
#define REG_NB_MCA_L3_THRESHOLD_3X170     0x170
#define REG_NB_DOWNCORE_3X190             0x190
#define REG_NB_SBI_CONTROL_3X1E4          0x1E4

/* Function 4 registers */

/* Function 5 registers */
#define REG_NB_COMPUTE_UNIT_5X80            0x80
#define REG_NB_CAPABILITY_2_5X84            0x84


/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

#endif /* _HT_NB_HARDWARE_FAM10_H_ */
