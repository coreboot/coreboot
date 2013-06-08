/* $NoKeywords:$ */
/**
 * @file
 *
 * mnreg.h
 *
 * Definitions for whole register tokens
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 61256 $ @e \$Date: 2011-11-03 15:06:11 -0500 (Thu, 03 Nov 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _MNREG_H_
#define _MNREG_H_

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

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Registers used in memory initialization
typedef enum {
  // NB_REG identifiers
  NbRegRangeStart = BFEndOfList, ///< -------------------------- Start of NB_REG range
  RegDramBase0,                  ///< Register DramBase0
  RegDramLimit0,                 ///< Register DramLimit0
  RegDramBaseHi0,                ///< Register DramBaseHi0
  RegDramLimitHi0,               ///< Register DramLimitHi0
  RegDramHoleAddr,               ///< Register DramHoleAddr
  RegDctCfgSel,                  ///< Register DctCfgSel
  RegDramBaseSysAddr,            ///< Register DramBaseSysAddr
  RegDramLimitSysAddr,           ///< Register DramLimitSysAddr

  RegDramCtlrBase0,              ///< Register DramCtlrBase0
  RegDramCtlrBase1,              ///< Register DramCtlrBase1
  RegDramCtlrBase2,              ///< Register DramCtlrBase2
  RegDramCtlrBase3,              ///< Register DramCtlrBase3

  RegDramCtlrLimit0,             ///< Register DramCtlrLimit0
  RegDramCtlrLimit1,             ///< Register DramCtlrLimit1
  RegDramCtlrLimit2,             ///< Register DramCtlrLimit2
  RegDramCtlrLimit3,             ///< Register DramCtlrLimit3

  RegDctHiAddrOffset0,           ///< Register DctHiAddrOffset0
  RegDctHiAddrOffset1,           ///< Register DctHiAddrOffset1
  RegDctHiAddrOffset2,           ///< Register DctHiAddrOffset2
  RegDctHiAddrOffset3,           ///< Register DctHiAddrOffset3

  RegDramCtl,                    ///< Register DramCtl
  RegDramInit,                   ///< Register DramInit
  RegDramBankAddr,               ///< Register DramBankAddr
  RegDramMRS,                    ///< Register DramMRS
  RegDramTmgLo,                  ///< Register DramTmgLo
  RegDramTmgHi,                  ///< Register DramTmgHi
  RegDramConfigLo,               ///< Register DramConfigLo
  RegDramConfigHi,               ///< Register DramConfigHi

  RegDctAddlOffset,              ///< Register DctAddlOffset
  RegDctAddlData,                ///< Register DctAddlData
  RegDctAddlOffsetBrdcst,        ///< Register DctAddlOffset for broadcast
  RegDctAddlDataBrdcst,          ///< Register DctAddlData for broadcast

  RegDctTempThrottle,            ///< Register DctTempThrottle
  RegDramCtlrMisc2,              ///< Register DramCtlrMisc2
  RegTraceBufferCtlr,            ///< Register TraceBufferCtlr
  RegSwapIntLvRgn,               ///< Register SwapIntLvRgn
  RegMctCfgLo,                   ///< Register MctCfgLo
  RegMctCfgHi,                   ///< Register MctCfgHi
  RegExtMctCfgLo,                ///< Register ExtMctCfgLo
  RegExtMctCfgHi,                ///< Register ExtMctCfgHi
  RegDramNbPstate,               ///< Register DramNbPstate

  RegGmcToDctCtl0,               ///< Register GmcToDctCtl0
  RegGmcToDctCtl1,               ///< Register GmcToDctCtl1
  RegGmcToDctCtl2,               ///< Register GmcToDctCtl2

  RegCSBaseAddr0,                ///< Register CSBaseAddr0
  RegCSBaseAddr1,                ///< Register CSBaseAddr1
  RegCSBaseAddr2,                ///< Register CSBaseAddr2
  RegCSBaseAddr3,                ///< Register CSBaseAddr3
  RegCSMask0,                    ///< Register CSMask0
  RegCSMask1,                    ///< Register CSMask1
  RegDramCtlrSelLo,              ///< Register DramCtlrSelLo
  RegDramCtlrSelHi,              ///< Register DramCtlrSelHi

  RegDdr3DramTmg0,               ///< Register Ddr3DramTmg0
  RegDdr3DramTmg1,               ///< Register Ddr3DramTmg1
  RegDdr3DramTmg2,               ///< Register Ddr3DramTmg2
  RegDdr3DramTmg3,               ///< Register Ddr3DramTmg3
  RegDdr3DramTmg4,               ///< Register Ddr3DramTmg4
  RegDdr3DramTmg5,               ///< Register Ddr3DramTmg5
  RegDdr3DramTmg6,               ///< Register Ddr3DramTmg6
  RegDdr3DramTmg7,               ///< Register Ddr3DramTmg7
  RegDdr3DramTmg8,               ///< Register Ddr3DramTmg8
  RegDdr3DramTmg9,               ///< Register Ddr3DramTmg9
  RegDdr3DramTmg10,              ///< Register Ddr3DramTmg10
  RegDdr3DramPwrMng1,            ///< Register Ddr3DramPwrMng1
  RegDdr3DramOdtCtl,             ///< Register Ddr3DramOdtClt
  RegMemPsCtlSts,                ///< Register MemPsCtlSts

  RegDramLpbkTrnCtl,             ///< Register DramLpbkTrnCtl
  RegTargetABase,                ///< Register TargetABase
  RegDramCmd0,                   ///< Register DramCmd0
  RegDramCmd1,                   ///< Register DramCmd1
  RegDramCmd2,                   ///< Register DramCmd2
  RegDramPRBS,                   ///< Register DramPRBS
  RegDramStatus1,                ///< Register DramStatus1
  RegDramDqMaskLo,               ///< Register DramDqMaskLo
  RegDramDqMaskHi,               ///< Register DramDqMaskHi
  RegDQErrLo,                    ///< Register DQErrLo
  RegDQErrHi,                    ///< Register DQErrHi

  RegDramControl,                ///< Register DRAM Control


  RegNbCap2,                     ///< Register NbCap2
  RegNbPstateCtl,                ///< Register NbPstateCtl
  RegNbPstateStatus,             ///< Register NbPstateStatus
  RegNbPstate0,                  ///< Register NB Pstate 0
  RegNbPstate1,                  ///< Register NB Pstate 1
  RegNbPstate2,                  ///< Register NB Pstate 2
  RegNbPstate3,                  ///< Register NB Pstate 3

  NbRegRangeEnd,                 ///< -------------------------- End of NB_REG range

                                 // DCT_PHY_REG identifiers
  DctPhyRegRangeStart,           ///< -------------------------- Start of DCT_PHY_REG range
  RegRxCtl1,                     ///< Register RxCtl1
  RegDqDqsRxCtl,                 ///< Register DqDqsRxCtl
  RegRdPtrInitVal,               ///< Register RdPtrInitVal
  RegDataRdPtrInitVal,           ///< Register DataRdPtrInitVal
  RegDataRdPtrOffset,            ///< Register DataRdPtrOffset


  DctPhyRegRangeEnd,             ///< -------------------------- End of DCT_PHY_REG range

  RegIdLimit                     ///< Total number of register identifiers

} REG_BF_NAME;

/// Bit field location
typedef struct {
  UINT32  LoBit: 6;         ///< Low bit of the bit field
  UINT32  HiBit: 6;         ///< High bit of the bit field
  UINT32  RegIndex: 10;     ///< Register that the bit field is on
  UINT32  Reserved: 9;      ///< Reserved
  UINT32  Linked: 1;        ///< 1: The bit field has high bits defined in the very next Bf identifier
  ///< 0: The bit field has no further extension
} BF_LOCATION;

/**
  REG_DEF(TableName, RegIndex, Addr)

  @param[in]    TableName
  @param[in]    RegIndex
  @param[in]    Addr

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define REG_DEF(TableName, RegIndex, Addr) \
  TableName[RegIndex] = Addr

/**
  _BF_DEF(TableName, RegIndex, BfIndex, Hi, Lo)

  @param[in]    TableName
  @param[in]    RegIndex
  @param[in]    BfIndex
  @param[in]    Hi
  @param[in]    Lo

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define _BF_DEF(TableName, RegIndex, BfIndex, Hi, Lo) \
  TableName[BfIndex] = ( \
    ((UINT32) RegIndex << 12) | ( \
      (Hi > Lo) ? (\
        (((UINT32) Hi << 6) | (UINT32) Lo) \
      ) : ( \
        (((UINT32) Lo << 6) | (UINT32) Hi) \
      ) \
    ) \
  )

/**
  LINK_BF(TableName, LowerBfIndex, HigherBfIndex):
  This is one way link: any write to LowerBfIndex would write to HigherBfIndex,
                        but NOT the other way around.
  Requirement: LowerBfIndex must be declared *right* before HigherBfIndex.

  @param[in]    TableName
  @param[in]    LowerBfIndex
  @param[in]    HigherBfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define LINK_BF(TableName, LowerBfIndex, HigherBfIndex) { \
  ASSERT (LowerBfIndex < BFEndOfList); \
  ASSERT (HigherBfIndex < BFEndOfList); \
  ASSERT (LowerBfIndex == (HigherBfIndex - 1)) ; \
  TableName[LowerBfIndex] |= ((UINT32) 1) << 31; \
}

/**
  S3_SAVE_PRE_ESR(RegBfIndex)
  Specifies that RegBfIndex should be saved for S3 resume

  @param[in]    RegBfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define S3_SAVE_PRE_ESR(RegBfIndex)    ((RegBfIndex) | ((UINT32) 1 << 31))
#define GET_PRE_ESR_FLAG(RegBfIndex)   (((RegBfIndex) >> 31) & 1)

/**
  S3_SAVE_POST_ESR(RegBfIndex)
  Specifies that RegBfIndex should be saved for S3 resume

  @param[in]    RegBfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define S3_SAVE_POST_ESR(RegBfIndex)   ((RegBfIndex) | ((UINT32) 1 << 30))
#define GET_POST_ESR_FLAG(RegBfIndex)  (((RegBfIndex) >> 30) & 1)

/**
  VOLATILE_BF(RegBfIndex)
  Specifies that RegBfIndex cannot be cached, but must be accessed right away.

  @param[in]    RegBfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define VOLATILE_BF(RegBfIndex)           ((RegBfIndex) | ((UINT32) 1 << 28))
#define GET_VOLATILE_BF_FLAG(RegBfIndex)  (((RegBfIndex) >> 28) & 1)

#define BC_MEM_PS   0x4
#define BC_NB_PS    0x1
#define BC_DIS      0xF
#define GET_BROADCAST_FLAG(BfIndex)   (((BfIndex) >> 20) & 0xF)
#define GET_BROADCAST_OVERRIDE(BfIndex)  (((BfIndex) >> 16) & 0xF)

/**
  SINGLE_INST(BfIndex)
  Specifies that set function should only broadcast to Mem Pstate p of bit field BfIndex

  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define SINGLE_INST(BfIndex) ((BfIndex) | ((UINT32) BC_DIS << 20))

/**
  PER_MEM_PS(p, BfIndex)
  Specifies that set function should only broadcast to Mem Pstate p of bit field BfIndex

  @param[in]    p
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_MEM_PS(p, BfIndex) ((BfIndex) | ((UINT32) (p) << 16) | ((UINT32) BC_MEM_PS << 20))

/**
  PER_NB_PS(p, BfIndex)
  Specifies that set function should only broadcast to NB Pstate p of bit field BfIndex

  @param[in]    p
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_NB_PS(p, BfIndex) ((BfIndex) | ((UINT32) (p) << 16) | ((UINT32) BC_NB_PS << 20))
#define NB_PS_PMU   8


/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif  /* _MNREG_H_ */

