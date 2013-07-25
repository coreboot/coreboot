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
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
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
  RegDramBase1,                  ///< Register DramBase1
  RegDramLimit1,                 ///< Register DramLimit1
  RegDramBaseHi1,                ///< Register DramBaseHi1
  RegDramLimitHi1,               ///< Register DramLimitHi1
  RegDramBase2,                  ///< Register DramBase2
  RegDramLimit2,                 ///< Register DramLimit2
  RegDramBaseHi2,                ///< Register DramBaseHi2
  RegDramLimitHi2,               ///< Register DramLimitHi2
  RegDramBase3,                  ///< Register DramBase3
  RegDramLimit3,                 ///< Register DramLimit3
  RegDramBaseHi3,                ///< Register DramBaseHi3
  RegDramLimitHi3,               ///< Register DramLimitHi3
  RegDramBase4,                  ///< Register DramBase4
  RegDramLimit4,                 ///< Register DramLimit4
  RegDramBaseHi4,                ///< Register DramBaseHi4
  RegDramLimitHi4,               ///< Register DramLimitHi4
  RegDramBase5,                  ///< Register DramBase5
  RegDramLimit5,                 ///< Register DramLimit5
  RegDramBaseHi5,                ///< Register DramBaseHi5
  RegDramLimitHi5,               ///< Register DramLimitHi5
  RegDramBase6,                  ///< Register DramBase6
  RegDramLimit6,                 ///< Register DramLimit6
  RegDramBaseHi6,                ///< Register DramBaseHi6
  RegDramLimitHi6,               ///< Register DramLimitHi6
  RegDramBase7,                  ///< Register DramBase7
  RegDramLimit7,                 ///< Register DramLimit7
  RegDramBaseHi7,                ///< Register DramBaseHi7
  RegDramLimitHi7,               ///< Register DramLimitHi7

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
  RegDramCtlrMisc3,              ///< Register DramCtlrMisc3
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
  RegGmcToDctFifoCfg1,           ///< Register GMC to DCT FIFO Config 1

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
  RegPhyRODTCSLow ,              ///< Register RegPhyRODTCSLow
  RegPhyRODTCSHigh,              ///< Register RegPhyRODTCSHigh
  RegPhyWODTCSLow ,              ///< Register RegPhyWODTCSLow
  RegPhyWODTCSHigh,              ///< Register RegPhyWODTCSHigh
  RegDdr3DramPwrMng0,            ///< Register Ddr3DramPwrMng0
  RegDdr3DramPwrMng1,            ///< Register Ddr3DramPwrMng1
  RegDdr3DramOdtCtl,             ///< Register Ddr3DramOdtClt
  RegMemPsCtlSts,                ///< Register MemPsCtlSts
  RegMrsBuf0,                    ///< Register MRS Buffer 0
  RegMrsBuf1,                    ///< Register MRS Buffer 1

  RegDramLpbkTrnCtl,             ///< Register DramLpbkTrnCtl
  RegTargetABase,                ///< Register TargetABase
  RegDramCmd0,                   ///< Register DramCmd0
  RegDramCmd1,                   ///< Register DramCmd1
  RegDramCmd2,                   ///< Register DramCmd2
  RegDramPRBS,                   ///< Register DramPRBS
  RegDramStatus1,                ///< Register DramStatus1
  RegDramDqMaskLo,               ///< Register DramDqMaskLo
  RegDramDqMaskHi,               ///< Register DramDqMaskHi
  RegDramEccMask,                ///< Register DramEccMask
  RegDQErrLo,                    ///< Register DQErrLo
  RegDQErrHi,                    ///< Register DQErrHi

  RegGddr5DramTmg0,              ///< Register Gddr5DramTmg0
  RegGddr5DramTmg1,              ///< Register Gddr5DramTmg1
  RegGddr5DramTmg2,              ///< Register Gddr5DramTmg2
  RegGddr5DramTmg3,              ///< Register Gddr5DramTmg3
  RegGddr5DramTmg4,              ///< Register Gddr5DramTmg4
  RegGddr5DramTmg5,              ///< Register Gddr5DramTmg5
  RegGddr5DramTmg6,              ///< Register Gddr5DramTmg6
  RegGddr5DramTmg7,              ///< Register Gddr5DramTmg7
  RegGddr5DramTmg8,              ///< Register Gddr5DramTmg8
  RegGddr5DramPowerMng0,         ///< Register Gddr5DramPowerMng0
  RegGddr5DramPowerMng1,         ///< Register Gddr5DramPowerMng1
  RegGddr5DramCtrl0,             ///< Register Gddr5DramCtrl0
  RegGddr5DramNbPstate,          ///< Register Gddr5DramNbPstate

  RegNbCap2,                     ///< Register NbCap2
  RegNbPstateCtl,                ///< Register NbPstateCtl
  RegNbPstateStatus,             ///< Register NbPstateStatus
  RegNbPstate0,                  ///< Register NB Pstate 0
  RegNbPstate1,                  ///< Register NB Pstate 1
  RegNbPstate2,                  ///< Register NB Pstate 2
  RegNbPstate3,                  ///< Register NB Pstate 3
  RegNbFusionCfg,                ///< Register NB Fusion Configuration
  RegCUPwrStsLo,                 ///< Register Compute Unit Power Status Low
  RegCUPwrStsHi,                 ///< Register Compute Unit Power Status High

  RegCkeToCsMap,                 ///< Register Dram CKE to CS Map

  RegMcaNbCtl,                   ///< Register MCA NB Control
  RegMcaNbCfg,                   ///< Register MCA NB Configuration
  RegScrubRateCtl,               ///< Register Scrub Rate Control
  RegDramScrubAddrLo,            ///< Register DRAM Scrub Address Low
  RegDramScrubAddrHi,            ///< Register DRAM Scrub Address High
  RegNbCfg1Lo,                   ///< Register NB Configuration 1 Low
  RegCpuid,                      ///< Register CPUID Family/Model/Stepping
  RegExtMcaNbCfg,                ///< Register Extended NB MCA Configuration

  RegReserved01,                 ///< Reserved
  RegReserved02,                 ///< Reserved
  RegReserved03,                 ///< Reserved
  RegReserved04,                 ///< Reserved
  RegReserved05,                 ///< Reserved

  NbRegRangeEnd,                 ///< -------------------------- End of NB_REG range

                                 // DCT_PHY_REG identifiers
  DctPhyRegRangeStart,           ///< -------------------------- Start of DCT_PHY_REG range
  RegRxCtl1,                     ///< Register RxCtl1
  RegRdPtrInitVal,               ///< Register RdPtrInitVal
  RegDataRdPtrInitVal,           ///< Register DataRdPtrInitVal
  RegEdcRdPtrInitVal,            ///< Register EdcRdPtrInitVal
  RegDataRdPtrOffset,            ///< Register DataRdPtrOffset

  RegPmuRst,                     ///< Register PmuRst
  RegPwrStateCmd,                ///< Register Power State Command
  RegDsMbMsg,                    ///< Register DS Mailbox Message
  RegDsMbProtocol,               ///< Register DS Mailbox Protocol
  RegMbProtocolShadow,           ///< Register Mailbox Protocol Shadow
  RegUsMbMsg,                    ///< Register US Mailbox Message
  RegUsMbProtocol,               ///< Register US Mailbox Protocol
  RegUsMb2Msg,                   ///< Register US Mailbox 2 Message
  RegUsMb2Protocol,              ///< Register US Mailbox 2 Protocol
  RegSramMsgBlk,                 ///< Register SRAM Message Block
  RegGlobalCtl,                  ///< Register Global Control
  RegGlobalCtlSlaveAbyte,        ///< Register Global Control Slave ABYTE
  RegGlobalCtlSlaveDbyte,        ///< Register Global Control Slave DBYTE
  RegGlobalCtlSlaveAbit,         ///< Register Global Control Slave ABIT
  RegCalMisc2,                   ///< Register Cal Misc 2
  RegPllMemPs0,                  ///< Register PLL Mem Pstate 0
  RegPllMemPs1,                  ///< Register PLL Mem Pstate 1
  RegPmuClkDiv,                  ///< Register PMU Clock divider
  RegPllRegWaitTime,             ///< Register PllRegWaitTime
  RegPllLockTime,                ///< Register PllRegLockTime
  RegPhyCADCtl,                  ///< Register Phy CAD control
  RegMemResetCtl,                ///< Register Mem reset control
  RegDisCal,                     ///< Register Disable calibration

  RegCadTxSlewRate,              ///< Register CAD Tx Slew Rate
  RegDataTxSlewRate,             ///< Register DATA Tx Slew Rate

  RegAcsmCtrl13,                 ///< Register AcsmCtrl13
  RegX8DevIDCtl,                 ///< Register X8 DeviceID Control

  RegTxImpedance,                ///< Register for all TX impedance
  RegCadTxImpedance,             ///< Register CAD Tx Impedance
  RegDataTxImpedance,            ///< Register Data Tx Impedance
  RegDataRxImpedance,            ///< Register Data Rx Impedance

  RegAcsmOdtCtrl0,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl1,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl2,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl3,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl4,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl5,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl6,               ///< Register ACSM ODT Ctrl
  RegAcsmOdtCtrl7,               ///< Register ACSM ODT Ctrl

  RegVrefByteAbyte,              ///< Register Vref Byte
  RegVrefByteDbyte,              ///< Register Vref Byte
  RegVrefByteMaster,             ///< Register Vref Byte

  RegProcOdtTmg,                 ///< Register Proc ODT Timing
  RegPhyClkCtl,                  ///< Register Phy clock control

  RegTxDly,                      ///< Register TX delay
  RegDataRxDly,                  ///< Register Data Rx Delay
  RegDataTxDly,                  ///< Register Data Tx Delay
  RegDataTxEqHiImp,              ///< Register Data Tx EQ Hi Impedence
  RegDataTxEqLoImp,              ///< Register Data Tx EQ Lo Impedence
  RegDataTxEqBoostImp,           ///< Register Data Tx EQ Boost Impedence

  DctPhyRegRangeEnd,             ///< -------------------------- End of DCT_PHY_REG range

  RegIdLimit                     ///< Total number of register identifiers

} REG_BF_NAME;

/// Bit field location
typedef struct {
  UINT32  LoBit: 6;         ///< Low bit of the bit field
  UINT32  HiBit: 6;         ///< High bit of the bit field
  UINT32  RegIndex: 12;     ///< Register that the bit field is on
  UINT32  Reserved: 7;      ///< Reserved
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
  ASSERT ((LowerBfIndex < BFEndOfList) || (LowerBfIndex > RegIdLimit)); \
  ASSERT ((HigherBfIndex < BFEndOfList) || (HigherBfIndex > RegIdLimit)); \
  ASSERT (LowerBfIndex == (HigherBfIndex - 1)) ; \
  TableName[LowerBfIndex] |= ((UINT32) 1) << 31; \
}

/*----------------------------------------------------------------------------
 *                           CHIP TO PAD TRANSLATION
 *
 *----------------------------------------------------------------------------
 */

/// Pad group names
typedef enum {
  PAD_ALL = 0,  ///< broadcast to all pads
  PAD_CKE,      ///< pad CKE
  PAD_ADDR_CMD, ///< pad ADDR_CMD
  PAD_CLK,      ///< pad CLK
  PAD_CS,       ///< pad CS
  PAD_ODT,      ///< pad ODT
  PAD_DQ_L,     ///< pad DQ_L
  PAD_DQ_H,     ///< pad DQ_H
  PAD_DQS_DM,   ///< pad DQS_DM
  PAD_DQS_H,    ///< pad DQS_H
  PAD_PROC_ODT, ///< pad PROC_ODT
  PAD_DQ,       ///< pad DQ
  PAD_DQS,      ///< pad DQS
} PAD_NAME;

#define  MEMCKE0    PAD_CKE
#define  MEMADD0    PAD_ADDR_CMD
#define  MEMBANK0   PAD_ADDR_CMD
#define  MEMPAR0    PAD_ADDR_CMD
#define  MEMCLK0_L  PAD_CLK
#define  MEMCLK0_H  PAD_CLK
#define  MEMCS0_L   PAD_CS
#define  MEMCAS0_L  PAD_ADDR_CMD
#define  MEMWE0_L   PAD_ADDR_CMD
#define  MEMRAS0_L  PAD_ADDR_CMD
#define  MEMODT0    PAD_ODT
#define  MEMDQ_L    PAD_DQ_L
#define  MEMDQ_H    PAD_DQ_H
#define  MEMDQSDM   PAD_DQS_DM
#define  MEMDQS_H   PAD_DQS_H
#define  MEMDQ      PAD_DQ
#define  MEMDQS     PAD_DQS

/// DDR Chip to pad entry
typedef struct {
  UINT16 PadName: 4;   ///< Pad name
  UINT16 Instance: 4;  ///< Pad instance
  UINT16 Chiplet: 4;   ///< Chiplet number
  UINT8  Group: 4;     ///< Group number
  UINT8  PadNum: 4;    ///< Pad number
} DDR_CHIP_TO_PAD_ENTRY;

/*----------------------------------------------------------------------------
 *                           BIT FIELD TOKEN ENCODING
 *
 *----------------------------------------------------------------------------
 */

/// Sub-structure used to parse a BF token
typedef struct {
  UINT32  Index: 12;       ///< Index into NBRegTable table
  UINT32  Reserved: 4;     ///< Reserved
  UINT32  PadName: 5;      ///< ie CKE, ODT, ADDR,...
  UINT32  PadIdxPlus1: 5;  ///< Pad instance of a certain type
  UINT32  DLNPlus1: 4;     ///< DIMM/DLL/NBPs number
  UINT32  MemPsPlus1: 2;   ///< MemPs number
} BF_TOKEN_PARSE;

/// Bit field name encoding
typedef union {
  BIT_FIELD_NAME  FieldName; ///< BF token
  BF_TOKEN_PARSE  Parse;     ///< BF token parse
} BF_NAME_ENC;


/**
  SW_BC_DIS(BfIndex)
  Specifies that set function to not attempt SW broadcast

  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define SW_BC_DIS(BfIndex) ((BfIndex) | (0xFFFF0000ul))

/**
  PER_MEM_PS(p, BfIndex)
  Specifies that set function to only broadcast to Mem Pstate p of bit field BfIndex

  @param[in]    p
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_MEM_PS(p, BfIndex) ((BfIndex) | ((UINT32) (((p) + 1) & 0x3) << 30))
#define ALL_MEM_PS  2

/**
  PER_NB_PS(p, BfIndex)
  Specifies that set function to only broadcast to NB Pstate p of bit field BfIndex

  @param[in]    p
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_NB_PS(p, BfIndex) ((BfIndex) | ((UINT32) (((p) + 1) & 0xF) << 26))
#define NB_PS_PMU   8
#define ALL_NB_PS   0xFF

/**
  PER_DIMM(p, BfIndex)
  Specifies that set function to only broadcast to NB Pstate p of bit field BfIndex

  @param[in]    d
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_DIMM(d, BfIndex) ((BfIndex) | ((UINT32) (((d) + 1) & 0xF) << 26))
#ifndef ALL_DIMMS
  #define ALL_DIMMS  0xFF
#endif


/**
  PER_CAD_PAD(t, i, BfIndex)
  Specifies that set function to only broadcast a certain pad (ie MEMCKE[3])

  @param[in]    t - pad type (ie MEMCKE)
  @param[in]    i - pad index (ie 3)
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_CAD_PAD(t, i, BfIndex) ((BfIndex) | ((UINT32) (t) << 16) | ((UINT32) (((i) + 1) & 0x1F) << 21))
#define ALL_PADS   0xFF

/**
  PER_DATA_BYTE(t, b, BfIndex)
  Specifies that set function to only broadcast a certain data pad (ie DQ byte 3)

  @param[in]    t - data pin type (ie DQ)
  @param[in]    b - data byte (ie byte 3)
  @param[in]    BfIndex

  @return   REG_BF_ENC  Access params encrypted in REG_BF_ENC format.
--*/
#define PER_DATA_BYTE(t, b, BfIndex) ((BfIndex) | ((UINT32) (t) << 16) | ((UINT32) (((b) + 1) & 0x1F) << 21))
#define ECC_BYTE  8
#define ALL_BYTES 0xFF

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif  /* _MNREG_H_ */

