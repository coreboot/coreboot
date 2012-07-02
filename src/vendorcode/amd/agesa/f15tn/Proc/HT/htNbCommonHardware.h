/* $NoKeywords:$ */
/**
 * @file
 *
 * Northbridge hardware definitions for Family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
