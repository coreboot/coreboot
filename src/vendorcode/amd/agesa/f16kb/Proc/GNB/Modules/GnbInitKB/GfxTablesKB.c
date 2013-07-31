/* $NoKeywords:$ */
/**
 * @file
 *
 * GFx tables
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87698 $   @e \$Date: 2013-02-07 12:40:51 -0600 (Thu, 07 Feb 2013) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
* ***************************************************************************
*
*/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbRegistersKB.h"
#include  "cpuFamilyTranslation.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */

GNB_TABLE ROMDATA GfxGmcFeature1DisableKB [] = {
  //2.1 Disable clock-gating
  GNB_ENTRY_WR (0x12, 0x20C0, 0x00000C80),
  GNB_ENTRY_WR (0x12, 0x2478, 0x00000400),
  GNB_ENTRY_WR (0x12, 0x20B8, 0x00000400),
  GNB_ENTRY_WR (0x12, 0x20BC, 0x00000400),
  GNB_ENTRY_WR (0x12, 0x2648, 0x00000400),
  GNB_ENTRY_WR (0x12, 0x264C, 0x00000400),
  GNB_ENTRY_WR (0x12, 0x2650, 0x00000400),
  GNB_ENTRY_WR (0x12, 0x15C0, 0x00000400),
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GfxGmcInitTableKB [] = {
  GNB_ENTRY_RMW (D18F5x178_TYPE, D18F5x178_ADDRESS, D18F5x178_SwGfxDis_MASK, 0 << D18F5x178_SwGfxDis_OFFSET),
  //2.2 System memory address translation
  GNB_ENTRY_COPY (0x12, 0x2814,  0, 32, D18F2x40_dct0_TYPE, D18F2x40_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (0x12, 0x281C,  0, 32, D18F2x44_dct0_TYPE, D18F2x44_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (0x12, 0x2824,  0, 32, D18F2x48_dct0_TYPE, D18F2x48_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (0x12, 0x282C,  0, 32, D18F2x4C_dct0_TYPE, D18F2x4C_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (0x12, 0x2834,  0, 32, D18F2x60_dct0_TYPE, D18F2x60_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (0x12, 0x283C,  0, 32, D18F2x64_dct0_TYPE, D18F2x64_dct0_ADDRESS,  0, 32),
  // MC_FUS_DRAM0_BANK_ADDR_MAPPING
  GNB_ENTRY_COPY (0x12, 0x2844,  0,  8, D18F2x80_dct0_TYPE, D18F2x80_dct0_ADDRESS,  0,  8),
  GNB_ENTRY_COPY (0x12, 0x2844,  8,  1, D18F2x94_dct0_TYPE, D18F2x94_dct0_ADDRESS, 22,  1),
  GNB_ENTRY_COPY (0x12, 0x2844,  9,  1, D18F2xA8_dct0_TYPE, D18F2xA8_dct0_ADDRESS, 20,  1),
  // MC_FUS_DRAM0_CTL_BASE
  GNB_ENTRY_COPY (0x12, 0x284C,  0,  3, D18F1x200_TYPE, D18F1x200_ADDRESS,  4,  3),
  GNB_ENTRY_COPY (0x12, 0x284C,  3,  4, D18F1x204_TYPE, D18F1x204_ADDRESS,  0,  4),
  GNB_ENTRY_COPY (0x12, 0x284C,  7, 21, D18F1x200_TYPE, D18F1x200_ADDRESS, 11, 21),
  GNB_ENTRY_COPY (0x12, 0x284C, 28,  1, D18F1x200_TYPE, D18F1x200_ADDRESS,  3,  1),
  GNB_ENTRY_COPY (0x12, 0x284C, 29,  1, D18F1x200_TYPE, D18F1x200_ADDRESS,  0,  1),
  // MC_FUS_DRAM0_CTL_LIMIT
  GNB_ENTRY_COPY (0x12, 0x2854,  0, 21, D18F1x204_TYPE, D18F1x204_ADDRESS, 11, 21),
  GNB_ENTRY_COPY (0x12, 0x2854, 21,  1, D18F1xF0_TYPE,  D18F1xF0_ADDRESS,   1,  1),
  // MC_FUS_DRAM_MODE
  GNB_ENTRY_COPY (0x12, 0x2864,  3,  1, D18F2x78_dct0_TYPE, D18F2x78_dct0_ADDRESS,  8,  1),
  GNB_ENTRY_COPY (0x12, 0x2864,  4,  9, D18F1xF0_TYPE,      D18F1xF0_ADDRESS,       7,  9),
  GNB_ENTRY_COPY (0x12, 0x2864,  0,  2, D18F2x110_TYPE,     D18F2x110_ADDRESS,      6,  2),
  GNB_ENTRY_COPY (0x12, 0x2864,  2,  1, D18F2x114_TYPE,     D18F2x114_ADDRESS,      9,  1),
  // MC_FUS_DRAM_CTL_HIGH_01
  GNB_ENTRY_COPY (0x12, 0x285C,  0, 12, D18F1x240_TYPE, D18F1x240_ADDRESS, 11, 12),

  // 2.4 Sequencer model programming
  GNB_ENTRY_WR (0x12, 0x276C, 0x000003ff),

  //---------------------------------------------------------------------------
  // 2.5 Power gating init
  // Initializing PGFSMs
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x3538, 0x200010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x3538, 0x300010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x210000),
  GNB_ENTRY_WR (0x12, 0x3538, 0xa00010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x21003),
  GNB_ENTRY_WR (0x12, 0x3538, 0xb00010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x2b00),
  GNB_ENTRY_WR (0x12, 0x3538, 0xc00010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x3538, 0xd00010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x420000),
  GNB_ENTRY_WR (0x12, 0x3538, 0x100010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x120202),
  GNB_ENTRY_WR (0x12, 0x3538, 0x500010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x3e3e36),
  GNB_ENTRY_WR (0x12, 0x3538, 0x600010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x373f3e),
  GNB_ENTRY_WR (0x12, 0x3538, 0x700010ff),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x0),
  GNB_ENTRY_WR (0x12, 0x353C, 0x3e1332),
  GNB_ENTRY_WR (0x12, 0x3538, 0xe00010ff),
//---------------------------------------------------------------------------
// Initializing register engine
// opcode=1, body_cnt=0, mask=0xf, const=0xf: MC_CONFIG (0x800) MC_CONFIG (0x800)
  GNB_ENTRY_WR (0x12, 0x3500, 0x0),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10000800),
  GNB_ENTRY_WR (0x12, 0x3504, 0xf),
  GNB_ENTRY_WR (0x12, 0x3504, 0xf),
// opcode=1, body_cnt=0, mask=0x3f, const=0x3f: MC_CONFIG_MCD (0x828) MC_CONFIG_MCD (0x828)
  GNB_ENTRY_WR (0x12, 0x3500, 0x4),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10000828),
  GNB_ENTRY_WR (0x12, 0x3504, 0x3f),
  GNB_ENTRY_WR (0x12, 0x3504, 0x3f),
// opcode=1, body_cnt=0, mask=0xffff, const=0xffff: VM_INVALIDATE_REQUEST (0x51e) VM_INVALIDATE_REQUEST (0x51e)
  GNB_ENTRY_WR (0x12, 0x3500, 0x8),
  GNB_ENTRY_WR (0x12, 0x3504, 0x1000051e),
  GNB_ENTRY_WR (0x12, 0x3504, 0xffff),
  GNB_ENTRY_WR (0x12, 0x3504, 0xffff),
// opcode=0, body_cnt=2: VM_L2_CNTL (0x500) VM_L2_CNTL3 (0x502)
  GNB_ENTRY_WR (0x12, 0x3500, 0xc),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20500),
// opcode=0, body_cnt=4: VM_CONTEXT0_CNTL (0x504) VM_SECURE_FAULT_CNTL (0x508)
  GNB_ENTRY_WR (0x12, 0x3500, 0x10),
  GNB_ENTRY_WR (0x12, 0x3504, 0x40504),
// opcode=0, body_cnt=9: VM_CONTEXT0_CNTL2 (0x50c) VM_CONTEXT15_PAGE_TABLE_BASE_ADDR (0x515)
  GNB_ENTRY_WR (0x12, 0x3500, 0x16),
  GNB_ENTRY_WR (0x12, 0x3504, 0x9050c),
// opcode=0, body_cnt=9: VM_PRT_APERTURE0_LOW_ADDR (0x52c) VM_CONTEXTS_DISABLE (0x535)
  GNB_ENTRY_WR (0x12, 0x3500, 0x21),
  GNB_ENTRY_WR (0x12, 0x3504, 0x9052c),
// opcode=0, body_cnt=1: VM_CONTEXT0_PROTECTION_FAULT_DEFAULT_ADDR (0x546) VM_CONTEXT1_PROTECTION_FAULT_DEFAULT_ADDR (0x547)
  GNB_ENTRY_WR (0x12, 0x3500, 0x2c),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10546),
// opcode=0, body_cnt=10: VM_FAULT_CLIENT_ID (0x54e) VM_CONTEXT1_PAGE_TABLE_START_ADDR (0x558)
  GNB_ENTRY_WR (0x12, 0x3500, 0x2f),
  GNB_ENTRY_WR (0x12, 0x3504, 0xa054e),
// opcode=0, body_cnt=1: VM_CONTEXT0_PAGE_TABLE_END_ADDR (0x55f) VM_CONTEXT1_PAGE_TABLE_END_ADDR (0x560)
  GNB_ENTRY_WR (0x12, 0x3500, 0x3b),
  GNB_ENTRY_WR (0x12, 0x3504, 0x1055f),
// opcode=0, body_cnt=1: VM_DEBUG (0x56f) VM_L2_CG (0x570)
  GNB_ENTRY_WR (0x12, 0x3500, 0x3e),
  GNB_ENTRY_WR (0x12, 0x3504, 0x1056f),
// opcode=0, body_cnt=1: VM_L2_BANK_SELECT_MASKA (0x572) VM_L2_BANK_SELECT_MASKB (0x573)
  GNB_ENTRY_WR (0x12, 0x3500, 0x41),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10572),
// opcode=0, body_cnt=2: VM_L2_CONTEXT1_IDENTITY_APERTURE_LOW_ADDR (0x575) VM_L2_CONTEXT_IDENTITY_PHYSICAL_OFFSET (0x577)
  GNB_ENTRY_WR (0x12, 0x3500, 0x44),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20575),
// opcode=0, body_cnt=53: MC_CITF_PERFCOUNTER_LO (0x7a0) ATC_PERFCOUNTER_RSLT_CNTL (0x7d5)
  GNB_ENTRY_WR (0x12, 0x3500, 0x48),
  GNB_ENTRY_WR (0x12, 0x3504, 0x3507a0),
// opcode=0, body_cnt=0: MC_ARB_PERF_MON_CNTL0_ECC (0x7db) MC_ARB_PERF_MON_CNTL0_ECC (0x7db)
  GNB_ENTRY_WR (0x12, 0x3500, 0x7f),
  GNB_ENTRY_WR (0x12, 0x3504, 0x7db),
// opcode=0, body_cnt=26: MC_SHARED_CHMAP (0x801) MC_VM_STEERING (0x81b)
  GNB_ENTRY_WR (0x12, 0x3500, 0x81),
  GNB_ENTRY_WR (0x12, 0x3504, 0x1a0801),
// opcode=0, body_cnt=2: MC_CG_CONFIG_MCD (0x829) MC_SHARED_BLACKOUT_CNTL (0x82b)
  GNB_ENTRY_WR (0x12, 0x3500, 0x9d),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20829),
// opcode=0, body_cnt=4: MC_HUB_MISC_POWER (0x82d) MC_HUB_MISC_DBG (0x831)
  GNB_ENTRY_WR (0x12, 0x3500, 0xa1),
  GNB_ENTRY_WR (0x12, 0x3504, 0x4082d),
// opcode=0, body_cnt=4: MC_HUB_MISC_OVERRIDE (0x833) MC_HUB_WDP_BP (0x837)
  GNB_ENTRY_WR (0x12, 0x3500, 0xa7),
  GNB_ENTRY_WR (0x12, 0x3504, 0x40833),
// opcode=0, body_cnt=11: MC_HUB_RDREQ_CNTL (0x83b) MC_HUB_SHARED_DAGB_DLY (0x846)
  GNB_ENTRY_WR (0x12, 0x3500, 0xad),
  GNB_ENTRY_WR (0x12, 0x3504, 0xb083b),
// opcode=0, body_cnt=1: MC_HUB_RDREQ_DMIF_LIMIT (0x848) MC_HUB_RDREQ_ACPG_LIMIT (0x849)
  GNB_ENTRY_WR (0x12, 0x3500, 0xba),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10848),
// opcode=0, body_cnt=63: MC_HUB_WDP_SH2 (0x84d) MC_HUB_WDP_SAM (0x88c)
  GNB_ENTRY_WR (0x12, 0x3500, 0xbd),
  GNB_ENTRY_WR (0x12, 0x3504, 0x3f084d),
// opcode=0, body_cnt=0: MC_VM_MB_L1_TLB0_DEBUG (0x891) MC_VM_MB_L1_TLB0_DEBUG (0x891)
  GNB_ENTRY_WR (0x12, 0x3500, 0xfe),
  GNB_ENTRY_WR (0x12, 0x3504, 0x891),
// opcode=0, body_cnt=0: MC_VM_MB_L1_TLB2_DEBUG (0x893) MC_VM_MB_L1_TLB2_DEBUG (0x893)
  GNB_ENTRY_WR (0x12, 0x3500, 0x100),
  GNB_ENTRY_WR (0x12, 0x3504, 0x893),
// opcode=0, body_cnt=0: MC_VM_MB_L2ARBITER_L2_CREDITS (0x8a1) MC_VM_MB_L2ARBITER_L2_CREDITS (0x8a1)
  GNB_ENTRY_WR (0x12, 0x3500, 0x102),
  GNB_ENTRY_WR (0x12, 0x3504, 0x8a1),
// opcode=0, body_cnt=0: MC_VM_MB_L1_TLB3_DEBUG (0x8a5) MC_VM_MB_L1_TLB3_DEBUG (0x8a5)
  GNB_ENTRY_WR (0x12, 0x3500, 0x104),
  GNB_ENTRY_WR (0x12, 0x3504, 0x8a5),
// opcode=0, body_cnt=51: MC_XPB_RTR_SRC_APRTR0 (0x8cd) MC_XPB_UNC_THRESH_SID (0x900)
  GNB_ENTRY_WR (0x12, 0x3500, 0x106),
  GNB_ENTRY_WR (0x12, 0x3504, 0x3308cd),
// opcode=0, body_cnt=1: MC_XPB_WCB_CFG (0x902) MC_XPB_P2P_BAR_CFG (0x903)
  GNB_ENTRY_WR (0x12, 0x3500, 0x13b),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10902),
// opcode=0, body_cnt=19: MC_XPB_P2P_BAR_SETUP (0x90c) MC_XPB_INTF_CFG (0x91f)
  GNB_ENTRY_WR (0x12, 0x3500, 0x13e),
  GNB_ENTRY_WR (0x12, 0x3504, 0x13090c),
// opcode=0, body_cnt=0: MC_XPB_SUB_CTRL (0x922) MC_XPB_SUB_CTRL (0x922)
  GNB_ENTRY_WR (0x12, 0x3500, 0x153),
  GNB_ENTRY_WR (0x12, 0x3504, 0x922),
// opcode=0, body_cnt=0: MC_XPB_PERF_KNOBS (0x924) MC_XPB_PERF_KNOBS (0x924)
  GNB_ENTRY_WR (0x12, 0x3500, 0x155),
  GNB_ENTRY_WR (0x12, 0x3504, 0x924),
// opcode=0, body_cnt=20: MC_XPB_STICKY_W1C (0x926) MC_XPB_CLG_CFG36 (0x93a)
  GNB_ENTRY_WR (0x12, 0x3500, 0x157),
  GNB_ENTRY_WR (0x12, 0x3504, 0x140926),
// opcode=2, body_cnt=0: MC_RPB_CID_QUEUE_EX (0x95a)
  GNB_ENTRY_WR (0x12, 0x3500, 0x16d),
  GNB_ENTRY_WR (0x12, 0x3504, 0x2000095a),
  GNB_ENTRY_WR (0x12, 0x3504, 0x1),
// opcode=3, body_cnt=31: MC_RPB_CID_QUEUE_EX_DATA (0x95b)
  GNB_ENTRY_WR (0x12, 0x3500, 0x16f),
  GNB_ENTRY_WR (0x12, 0x3504, 0x301f095b),
// opcode=0, body_cnt=12: MC_RPB_CONF (0x94d) MC_RPB_PERF_COUNTER_STATUS (0x959)
  GNB_ENTRY_WR (0x12, 0x3500, 0x190),
  GNB_ENTRY_WR (0x12, 0x3504, 0xc094d),
// opcode=0, body_cnt=16: MC_CITF_XTRA_ENABLE (0x96d) MC_CITF_INT_CREDITS_WR (0x97d)
  GNB_ENTRY_WR (0x12, 0x3500, 0x19e),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10096d),
// opcode=0, body_cnt=12: MC_CITF_WTM_RD_CNTL (0x97f) MC_WR_GRP_LCL (0x98b)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1b0),
  GNB_ENTRY_WR (0x12, 0x3504, 0xc097f),
// opcode=0, body_cnt=0: MC_CITF_PERF_MON_CNTL2 (0x98e) MC_CITF_PERF_MON_CNTL2 (0x98e)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1be),
  GNB_ENTRY_WR (0x12, 0x3504, 0x98e),
// opcode=0, body_cnt=2: MC_CITF_MISC_RD_CG (0x992) MC_CITF_MISC_VM_CG (0x994)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1c0),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20992),
// opcode=0, body_cnt=2: MC_VM_MD_L1_TLB0_DEBUG (0x998) MC_VM_MD_L1_TLB2_DEBUG (0x99a)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1c4),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20998),
// opcode=0, body_cnt=0: MC_VM_MD_L2ARBITER_L2_CREDITS (0x9a4) MC_VM_MD_L2ARBITER_L2_CREDITS (0x9a4)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1c8),
  GNB_ENTRY_WR (0x12, 0x3504, 0x9a4),
// opcode=0, body_cnt=0: MC_VM_MD_L1_TLB3_DEBUG (0x9a7) MC_VM_MD_L1_TLB3_DEBUG (0x9a7)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1ca),
  GNB_ENTRY_WR (0x12, 0x3504, 0x9a7),
// opcode=0, body_cnt=6: MC_ARB_AGE_CNTL (0x9bf) MC_ARB_GECC2_DEBUG2 (0x9c5)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1cc),
  GNB_ENTRY_WR (0x12, 0x3504, 0x609bf),
// opcode=0, body_cnt=45: MC_ARB_GECC2 (0x9c9) MC_ARB_MAX_LAT_CID (0x9f6)
  GNB_ENTRY_WR (0x12, 0x3500, 0x1d4),
  GNB_ENTRY_WR (0x12, 0x3504, 0x2d09c9),
// opcode=0, body_cnt=3: MC_ARB_SSM (0x9f9) MC_ARB_DRAM_TIMING_1 (0x9fc)
  GNB_ENTRY_WR (0x12, 0x3500, 0x203),
  GNB_ENTRY_WR (0x12, 0x3504, 0x309f9),
// opcode=0, body_cnt=0: MC_ARB_DRAM_TIMING2_1 (0x9ff) MC_ARB_DRAM_TIMING2_1 (0x9ff)
  GNB_ENTRY_WR (0x12, 0x3500, 0x208),
  GNB_ENTRY_WR (0x12, 0x3504, 0x9ff),
// opcode=0, body_cnt=2: MC_ARB_BURST_TIME (0xa02) MC_ARB_SCRAMBLE_KEY1 (0xa04)
  GNB_ENTRY_WR (0x12, 0x3500, 0x20a),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20a02),
// opcode=0, body_cnt=6: MC_FUS_DRAM0_CTL_BASE (0xa13) MC_FUS_DRAM_MODE (0xa19)
  GNB_ENTRY_WR (0x12, 0x3500, 0x20e),
  GNB_ENTRY_WR (0x12, 0x3504, 0x60a13),
// opcode=0, body_cnt=3: MC_FUS_ARB_GARLIC_ISOC_PRI (0xa1f) MC_FUS_ARB_GARLIC_WR_PRI2 (0xa22)
  GNB_ENTRY_WR (0x12, 0x3500, 0x216),
  GNB_ENTRY_WR (0x12, 0x3504, 0x30a1f),
// opcode=1, body_cnt=0, mask=0x3f, const=0x0: ATC_ATS_FAULT_CNTL (0xccd) ATC_ATS_FAULT_CNTL (0xccd)
  GNB_ENTRY_WR (0x12, 0x3500, 0x21b),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10000ccd),
  GNB_ENTRY_WR (0x12, 0x3504, 0x3f),
  GNB_ENTRY_WR (0x12, 0x3504, 0x0),
// opcode=0, body_cnt=7: ATC_VM_APERTURE0_LOW_ADDR (0xcc0) ATC_VM_APERTURE1_CNTL2 (0xcc7)
  GNB_ENTRY_WR (0x12, 0x3500, 0x21f),
  GNB_ENTRY_WR (0x12, 0x3504, 0x70cc0),
// opcode=0, body_cnt=2: ATC_ATS_CNTL (0xcc9) ATC_ATS_FAULT_DEBUG (0xccb)
  GNB_ENTRY_WR (0x12, 0x3500, 0x228),
  GNB_ENTRY_WR (0x12, 0x3504, 0x20cc9),
// opcode=0, body_cnt=1: ATC_ATS_DEFAULT_PAGE_LOW (0xcd0) ATC_ATS_DEFAULT_PAGE_CNTL (0xcd1)
  GNB_ENTRY_WR (0x12, 0x3500, 0x22c),
  GNB_ENTRY_WR (0x12, 0x3504, 0x10cd0),
// opcode=0, body_cnt=4: ATC_MISC_CG (0xcd4) ATC_L2_DEBUG2 (0xcd8)
  GNB_ENTRY_WR (0x12, 0x3500, 0x22f),
  GNB_ENTRY_WR (0x12, 0x3504, 0x40cd4),
// opcode=0, body_cnt=5: ATC_L1_CNTL (0xcdc) ATC_L1WR_STATUS (0xce1)
  GNB_ENTRY_WR (0x12, 0x3500, 0x235),
  GNB_ENTRY_WR (0x12, 0x3504, 0x50cdc),
// opcode=0, body_cnt=16: ATC_VMID_PASID_MAPPING_UPDATE_STATUS (0xce6) ATC_VMID15_PASID_MAPPING (0xcf6)
  GNB_ENTRY_WR (0x12, 0x3500, 0x23c),
  GNB_ENTRY_WR (0x12, 0x3504, 0x100ce6),
// opcode=0, body_cnt=0: ATC_ATS_FAULT_CNTL (0xccd) ATC_ATS_FAULT_CNTL (0xccd)
  GNB_ENTRY_WR (0x12, 0x3500, 0x24e),
  GNB_ENTRY_WR (0x12, 0x3504, 0xccd),
// opcode=0, body_cnt=23: MC_ARB_HARSH_EN_RD (0xdc0) MC_ARB_HARSH_CTL_WR (0xdd7)
  GNB_ENTRY_WR (0x12, 0x3500, 0x250),
  GNB_ENTRY_WR (0x12, 0x3504, 0x170dc0),
// opcode=0, body_cnt=0: MC_CONFIG (0x800) MC_CONFIG (0x800)
  GNB_ENTRY_WR (0x12, 0x3500, 0x269),
  GNB_ENTRY_WR (0x12, 0x3504, 0x800),
// opcode=0, body_cnt=0: MC_CONFIG_MCD (0x828) MC_CONFIG_MCD (0x828)
  GNB_ENTRY_WR (0x12, 0x3500, 0x26b),
  GNB_ENTRY_WR (0x12, 0x3504, 0x828),
//---------------------------------------------------------------------------
// Setting up end pointers
  GNB_ENTRY_RMW (0x12, 0x3508, 0xfffff000, 0x9b26c000),
//---------------------------------------------------------------------------
// Enabling light sleep
  GNB_ENTRY_RMW (0x12, 0x3544, 0x6000000, 0x2000000),

  // 2.7 Performance tuning
  GNB_ENTRY_WR (0x12, 0x27D0, 0x10724847),
  GNB_ENTRY_WR (0x12, 0x27C0, 0x00C32008),
  GNB_ENTRY_WR (0x12, 0x27C4, 0x00C32006),
  GNB_ENTRY_WR (0x12, 0x277C, 0x00000007),
  GNB_ENTRY_WR (0x12, 0x218C, 0x000021b1),
  GNB_ENTRY_WR (0x12, 0x201C, 0x47773337),
  GNB_ENTRY_WR (0x12, 0x2020, 0x73773337),
  GNB_ENTRY_WR (0x12, 0x2018, 0x66334303),
  GNB_ENTRY_WR (0x12, 0x2014, 0x66300333),
  GNB_ENTRY_WR (0x12, 0x2794, 0xfcfcfdfc),
  GNB_ENTRY_WR (0x12, 0x2798, 0xfcfcfdfc),
  GNB_ENTRY_WR (0x12, 0x27A4, 0x00ffffff),
  GNB_ENTRY_WR (0x12, 0x27A8, 0x00ffffff),
  GNB_ENTRY_WR (0x12, 0x278C, 0x00000008),
  GNB_ENTRY_WR (0x12, 0x2790, 0x00000008),
  GNB_ENTRY_WR (0x12, 0x2628, 0x55111000),
  GNB_ENTRY_WR (0x12, 0x25E0, 0x00000025),
  GNB_ENTRY_WR (0x12, 0x262C, 0x10555111),
  GNB_ENTRY_WR (0x12, 0x25E4, 0x00000025),
  GNB_ENTRY_WR (0x12, 0x25C8, 0x0080685F),
  GNB_ENTRY_WR (0x12, 0x25CC, 0x0000807F),
  GNB_ENTRY_WR (0x12, 0x2144, 0x50A1421D),
  GNB_ENTRY_WR (0x12, 0x20EC, 0x0000001C),
  GNB_ENTRY_WR (0x12, 0x2184, 0x0000A1F1),
  GNB_ENTRY_WR (0x12, 0x21E0, 0x0000A1F1),
  GNB_ENTRY_WR (0x12, 0x217C, 0x0000A1F1),
  GNB_ENTRY_WR (0x12, 0x21C0, 0x0000A1F1),
  GNB_ENTRY_WR (0x12, 0x2214, 0x000021B1),
  GNB_ENTRY_WR (0x12, 0x2220, 0x000021B1),
  GNB_ENTRY_WR (0x12, 0x3758, 0x00000800),
  GNB_ENTRY_WR (0x12, 0x375C, 0x00000800),
  GNB_ENTRY_WR (0x12, 0x3700, 0xD1000000),
  GNB_ENTRY_WR (0x12, 0x3704, 0xD0000000),
  GNB_ENTRY_WR (0x12, 0x3748, 0x0000007F),
  GNB_ENTRY_WR (0x12, 0x3750, 0x18201015),
  GNB_ENTRY_WR (0x12, 0x3754, 0x18201015),
  GNB_ENTRY_WR (0x12, 0x253C, 0x000000B4),
  GNB_ENTRY_WR (0x12, 0x2550, 0x000000B3),
  GNB_ENTRY_WR (0x12, 0x2558, 0x00002077),
  GNB_ENTRY_WR (0x12, 0x2558, 0x000020B7),
  GNB_ENTRY_WR (0x12, 0x2558, 0x00002A3D),
  GNB_ENTRY_WR (0x12, 0x2558, 0x00002010),
  GNB_ENTRY_WR (0x12, 0x2558, 0x00002A20),
  GNB_ENTRY_WR (0x12, 0x2544, 0x00040108),

  GNB_ENTRY_WR (0x12, 0x287C, 0x0c000911),

  //2.9 Display latency
  GNB_ENTRY_WR (0x12, 0x2114, 0x00000015),

  GNB_ENTRY_WR (0x12, 0x2880, 0xFCFE8000),

  //2.12 Remove blackout
  GNB_ENTRY_WR (0x12, 0x20AC, 0x00000000),

  // VCE Optimization
  GNB_ENTRY_RMW (0x12, 0x2108, 0xff00, (1 << 8)),
  GNB_ENTRY_WR (0x12, 0x21fc, 0xA1F1),
  GNB_ENTRY_WR (0x12, 0x2198, 0xA1F1),

  // STCTRL_IGNORE_PROTECTION_FAULT
  GNB_ENTRY_RMW (0x12, 0x350C, 0x01000000, (1 << 24)),

  GNB_ENTRY_RMW (0x12, 0x5490, 0x2 | 0x1, (1 << 0) | (1 << 1)),

  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GfxGmcFeature1EnableKB [] = {
  GNB_ENTRY_WR (0x12, 0x20C0, 0x000c0c80),
  GNB_ENTRY_WR (0x12, 0x2478, 0x000c0400),
  GNB_ENTRY_WR (0x12, 0x20B8, 0x000c0400),
  GNB_ENTRY_WR (0x12, 0x20BC, 0x000c0400),
  GNB_ENTRY_WR (0x12, 0x2648, 0x00080400),
  GNB_ENTRY_WR (0x12, 0x264C, 0x000c0400),
  GNB_ENTRY_WR (0x12, 0x2650, 0x000c0400),
  GNB_ENTRY_WR (0x12, 0x15C0, 0x000c0400),
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GfxEnvInitTableKB [] = {
  GNB_ENTRY_TERMINATE
};
