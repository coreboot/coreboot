/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Orochi Power Management related stuff
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/OR
 * @e \$Revision: 60669 $   @e \$Date: 2011-10-19 17:17:41 -0600 (Wed, 19 Oct 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_F15_OR_POWERMGMT_H_
#define _CPU_F15_OR_POWERMGMT_H_

/*
 * Family 15h Orochi CPU Power Management MSR definitions
 *
 */


/* Interrupt Pending and CMP-Halt MSR Register 0xC0010055 */
#define MSR_INTPEND 0xC0010055

/// Interrupt Pending and CMP-Halt MSR Register
typedef struct {
  UINT64 IoMsgAddr:16;               ///< IO message address
  UINT64 IoMsgData:8;                ///< IO message data
  UINT64 IntrPndMsgDis:1;            ///< Interrupt pending message disable
  UINT64 IntrPndMsg:1;               ///< Interrupt pending message
  UINT64 IoRd:1;                     ///< IO read
  UINT64 :2;                         ///< Reserved
  UINT64 BmStsClrOnHltEn:1;          ///< Clear BM status bit on server C1e entry
  UINT64 :34;                        ///< Reserved
} INTPEND_MSR;


/* P-state Registers 0xC001006[B:4] */

/// P-state MSR
typedef struct {
  UINT64 CpuFid:6;                   ///< CpuFid
  UINT64 CpuDid:3;                   ///< CpuDid
  UINT64 CpuVid:7;                   ///< CpuVid
  UINT64 :6;                         ///< Reserved
  UINT64 NbPstate:1;                 ///< NbPstate
  UINT64 :9;                         ///< Reserved
  UINT64 IddValue:8;                 ///< IddValue
  UINT64 IddDiv:2;                   ///< IddDiv
  UINT64 :21;                        ///< Reserved
  UINT64 PsEnable:1;                 ///< Pstate Enable
} PSTATE_MSR;


/* COFVID Control Register 0xC0010070 */
#define MSR_COFVID_CTL 0xC0010070

/// COFVID Control MSR Register
typedef struct {
  UINT64 CpuFid:6;                   ///< CpuFid
  UINT64 CpuDid:3;                   ///< CpuDid
  UINT64 CpuVid:7;                   ///< CpuVid
  UINT64 PstateId:3;                 ///< Pstate ID
  UINT64 :3;                         ///< Reserved
  UINT64 NbPstate:1;                 ///< Northbridge P-state
  UINT64 :41;                        ///< Reserved
} COFVID_CTRL_MSR;


/* COFVID Status Register 0xC0010071 */
#define MSR_COFVID_STS 0xC0010071

/// COFVID Status MSR Register
typedef struct {
  UINT64 CurCpuFid:6;                ///< Current CpuFid
  UINT64 CurCpuDid:3;                ///< Current CpuDid
  UINT64 CurCpuVid:7;                ///< Current CpuVid
  UINT64 CurPstate:3;                ///< Current Pstate
  UINT64 :3;                         ///< Reserved
  UINT64 CurNbDid:1;                 ///< Current NbDid
  UINT64 :2;                         ///< Reserved
  UINT64 CurNbVid:7;                 ///< Current NbVid
  UINT64 StartupPstate:3;            ///< Startup Pstate
  UINT64 MaxVid:7;                   ///< MaxVid
  UINT64 MinVid:7;                   ///< MinVid
  UINT64 MaxCpuCof:6;                ///< MaxCpuCof
  UINT64 :1;                         ///< Reserved
  UINT64 CurPstateLimit:3;           ///< Current Pstate Limit
  UINT64 MaxNbCof:5;                 ///< MaxNbCof
} COFVID_STS_MSR;

/* Floating Point Configuration Register 0xC0011028 */
#define MSR_FP_CFG 0xC0011028

/// Floating Point Configuration MSR Register
typedef struct {
  UINT64 :16;                        ///< Reserved
  UINT64 DiDtMode:1;                 ///< Di/Dt Mode
  UINT64 :1;                         ///< Reserved
  UINT64 DiDtCfg0:5;                 ///< Di/Dt Config 0
  UINT64 :2;                         ///< Reserved
  UINT64 AlwaysOnThrottle:2;         ///< AlwaysOnThrottle
  UINT64 DiDtCfg1:8;                 ///< Di/Dt Config 1
  UINT64 :5;                         ///< Reserved
  UINT64 Pipe3ThrottleDis:1;         ///< Pipe3ThrottleDis
  UINT64 :23;                        ///< Reserved
} FP_CFG_MSR;

/*
 * Family 15h Orochi CPU Power Management PCI definitions
 *
 */

/* Link transaction control register F0x68 */
#define LTC_REG 0x68

/// Link Transaction Control Register
typedef struct {
  UINT32 :12;                         ///< Reserved
  UINT32 ATMModeEn:1;                 ///< Accelerated transition to modified mode enable
  UINT32 :19;                         ///< Reserved
} LTC_REGISTER;

/* DRAM Configuration High Register F2x[1,0]94 */
#define DRAM_CFG_HI_REG0 0x94
#define DRAM_CFG_HI_REG1 0x194

/// DRAM Configuration High PCI Register
typedef struct {
  UINT32 MemClkFreq:5;               ///< Memory clock frequency
  UINT32 :2;                         ///< Reserved
  UINT32 MemClkFreqVal:1;            ///< Memory clock frequency valid
  UINT32 :2;                         ///< Reserved
  UINT32 ZqcsInterval:2;             ///< ZQ calibration short interval
  UINT32 :1;                         ///< Reserved
  UINT32 DisSimulRdWr:1;             ///< Disable simultaneous read and write
  UINT32 DisDramInterface:1;         ///< Disable the DRAM interface
  UINT32 PowerDownEn:1;              ///< Power down mode enable
  UINT32 PowerDownMode:1;            ///< Power down mode
  UINT32 FourRankRDimm1:1;           ///< Four rank registered DIMM 1
  UINT32 FourRankRDimm0:1;           ///< Four rank registered DIMM 0
  UINT32 DcqArbBypassEn:1;           ///< DRAM controller arbiter bypass enable
  UINT32 SlowAccessMode:1;           ///< Slow access mode
  UINT32 FreqChgInProg:1;            ///< Frequency change in progress
  UINT32 BankSwizzleMode:1;          ///< Bank swizzle mode
  UINT32 ProcOdtDis:1;               ///< Processor on-die termination disable
  UINT32 DcqBypassMax:4;             ///< DRAM controller queue bypass maximum
  UINT32 :4;                         ///< Reserved
} DRAM_CFG_HI_REGISTER;


/* Scrub Rate Control Register F3x58 */
#define SCRUB_RATE_CTRL_REG 0x58

/// Scrub Rate Control PCI Register
typedef struct {
  UINT32 DramScrub:5;                ///< DRAM scrub rate
  UINT32 :19;                        ///< Reserved
  UINT32 L3Scrub:5;                  ///< L3 cache scrub rate
  UINT32 :3;                         ///< Reserved
} SCRUB_RATE_CTRL_REGISTER;

/* DRAM Scrub Address Low Register F3x5C */
#define DRAM_SCRUB_ADDR_LOW_REG 0x5C

/// DRAM Scrub Address Low PCI Register
typedef struct {
  UINT32 ScrubReDirEn:1;             ///< DRAM scrubber redirect enable
  UINT32 :5;                         ///< Reserved
  UINT32 ScrubAddr:26;               ///< DRAM scrubber address bits[31:6]
} DRAM_SCRUB_ADDR_LOW_REGISTER;

/* Free List Buffer Count Register F3x7C */
#define FREE_LIST_BUFFER_COUNT_REG 0x7C

/// Free List Buffer Count PCI Register
typedef struct {
  UINT32 Xbar2SriFreeListCBC:5;      ///< XBAR to SRI free list command buffer count
  UINT32 :3;                         ///< Reserved
  UINT32 Sri2XbarFreeXreqCBC:4;      ///< SRI to XBAR free request and posted request command buffer count
  UINT32 Sri2XbarFreeRspCBC:4;       ///< SRI to XBAR free response command buffer count
  UINT32 Sri2XbarFreeXreqDBC:4;      ///< SRI to XBAR free request and posted request data buffer count
  UINT32 Sri2XbarFreeRspDBC:3;       ///< SRI to XBAR free response data buffer count
  UINT32 SrqExtFreeListBc:4;         ///< extend SRQ freelist tokens
  UINT32 :1;                         ///< Reserved
  UINT32 Xbar2SriFreeListCbInc:3;    ///< XBAR to SRI free list command buffer increment
  UINT32 :1;                         ///< Reserved
} FREE_LIST_BUFFER_COUNT_REGISTER;

/* ACPI Power State Control High F3x84 */
#define ACPI_PWR_STATE_CTRL_HI_REG 0x84

/// ACPI Power State Control High PCI Register
typedef struct {
  UINT32 CpuPrbEnSmafAct4:1;        ///< CPU direct probe enable
  UINT32 NbLowPwrEnSmafAct4:1;      ///< NB low-power enable
  UINT32 NbGateEnSmafAct4:1;        ///< NB gate enable
  UINT32 NbCofChgSmafAct4:1;        ///< NB FID change
  UINT32 :1;                        ///< Reserved
  UINT32 ClkDivisorSmafAct4:3;      ///< clock divisor
  UINT32 :8;                        ///< Reserved
  UINT32 CpuPrbEnSmafAct6:1;        ///< CPU direct probe enable
  UINT32 NbLowPwrEnSmafAct6:1;      ///< NB low-power enable
  UINT32 NbGateEnSmafAct6:1;        ///< NB gate enable
  UINT32 NbCofChgSmafAct6:1;        ///< NB FID change
  UINT32 :1;                        ///< Reserved
  UINT32 ClkDivisorSmafAct6:3;      ///< clock divisor
  UINT32 CpuPrbEnSmafAct7:1;        ///< CPU direct probe enable
  UINT32 NbLowPwrEnSmafAct7:1;      ///< NB low-power enable
  UINT32 NbGateEnSmafAct7:1;        ///< NB gate enable
  UINT32 NbCofChgSmafAct7:1;        ///< NB FID change
  UINT32 :1;                        ///< Reserved
  UINT32 ClkDivisorSmafAct7:3;      ///< clock divisor
} ACPI_PWR_STATE_CTRL_HI_REGISTER;

/* Power Control Miscellaneous Register F3xA0 */
#define PW_CTL_MISC_REG 0xA0

/// Power Control Miscellaneous PCI Register
typedef struct {
  UINT32 PsiVid:7;                   ///< PSI_L VID threshold
  UINT32 PsiVidEn:1;                 ///< PSI_L VID enable
  UINT32 :1;                         ///< Reserved
  UINT32 SviHighFreqSel:1;           ///< SVI high frequency select
  UINT32 IdleExitEn:1;               ///< IDLEEXIT_L Enable
  UINT32 PllLockTime:3;              ///< PLL synchronization lock time
  UINT32 :2;                         ///< Reserved
  UINT32 ConfigId:12;                ///< Configuration ID
  UINT32 NbPstateForce:1;            ///< NB P-state force on next LDTSTOP assertion
  UINT32 :2;                         ///< Reserved
  UINT32 CofVidProg:1;               ///< COF and VID of Pstate programmed
} POWER_CTRL_MISC_REGISTER;


/* Clock Power/Timing Control 0 Register F3xD4 */
#define CPTC0_REG 0xD4
#define CPTC0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC0_REG))

/// Clock Power Timing Control PCI Register
typedef struct {
  UINT32 NbFid:5;                    ///< NbFid
  UINT32 NbFidEn:1;                  ///< NbFidEn
  UINT32 :2;                         ///< Reserved
  UINT32 ClkRampHystSel:4;           ///< Clock Ramp Hysteresis Select
  UINT32 ClkRampHystCtl:1;           ///< Clock Ramp Hysteresis Control
  UINT32 MTC1eEn:1;                  ///< Message Triggered C1e Enable
  UINT32 CacheFlushImmOnAllHalt:1;   ///< Cache Flush Immediate on All Halt
  UINT32 StutterScrubEn:1;           ///< Stutter Mode Scrub Enable
  UINT32 LnkPllLock:2;               ///< Link PLL Lock
  UINT32 :2;                         ///< Reserved
  UINT32 PowerStepDown:4;            ///< Power Step Down
  UINT32 PowerStepUp:4;              ///< Power Step Up
  UINT32 NbClkDiv:3;                 ///< NbClkDiv
  UINT32 NbClkDivApplyAll:1;         ///< NbClkDivApplyAll
} CLK_PWR_TIMING_CTRL_REGISTER;


/* Clock Power/Timing Control 1 Register F3xD8 */
#define CPTC1_REG 0xD8
#define CPTC1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC1_REG))

/// Clock Power Timing Control 1 PCI Register
typedef struct {
  UINT32 :4;                         ///< Reserved
  UINT32 VSRampSlamTime:3;           ///< Voltage stabilization ramp time
  UINT32 :17;                        ///< Reserved
  UINT32 ReConDel:4;                 ///< Link reconnect delay
  UINT32 :4;                         ///< Reserved
} CLK_PWR_TIMING_CTRL1_REGISTER;


/* Northbridge Capabilities Register F3xE8 */
#define NB_CAPS_REG 0xE8

/// Northbridge Capabilities PCI Register
typedef struct {
  UINT32 :1;                         ///< Reserved
  UINT32 DualNode:1;                 ///< Dual-node multi-processor capable
  UINT32 EightNode:1;                ///< Eight-node multi-processor capable
  UINT32 Ecc:1;                      ///< ECC capable
  UINT32 Chipkill:1;                 ///< Chipkill ECC capable
  UINT32 :3;                         ///< Reserved
  UINT32 MctCap:1;                   ///< Memory controller capable
  UINT32 SvmCapable:1;               ///< SVM capable
  UINT32 HtcCapable:1;               ///< HTC capable
  UINT32 LnkRtryCap:1;               ///< Link error-retry capable
  UINT32 :2;                         ///< Reserved
  UINT32 MultVidPlane:1;             ///< Multiple VID plane capable
  UINT32 :1;                         ///< Reserved
  UINT32 MpCap:3;                    ///< MP capability
  UINT32 x2Apic:1;                   ///< x2Apic capability
  UINT32 UnGangEn:4;                 ///< Link unganging enabled
  UINT32 :1;                         ///< Reserved
  UINT32 L3Capable:1;                ///< L3 capable
  UINT32 :3;                         ///< Reserved
  UINT32 MultiNodeCpu:1;             ///< Multinode processor
  UINT32 IntNodeNum:2;               ///< Internal node number
} NB_CAPS_REGISTER;

/* L3 Buffer Count */
#define L3_BUFFER_COUNT_REG 0x1A0

/// L3 Buffer Count
typedef struct {
  UINT32 CpuCmdBufCnt:3;             ///< CPU to SRI command buffer count
  UINT32 :1;                         ///< Reserved
  UINT32 L3FreeListCBC:5;            ///< L3 free list command buffer counter for compute unit requests
  UINT32 :3;                         ///< Reserved
  UINT32 L3ToSriReqCBC:3;            ///< L3 cache to SRI request command buffer count
  UINT32 :1;                         ///< Reserved
  UINT32 CpuToNbFreeBufCnt:2;        ///< Cpu to Nb free buffer count
  UINT32 :14;                        ///< Reserved
} L3_BUFFER_COUNT_REGISTER;

/* L3 Control 1 */
#define L3_CONTROL_1_REG 0x1B8

///  L3 Control 1 Register
typedef struct {
  UINT32 :27;                       ///< Reserved
  UINT32 L3ATMModeEn:1;             ///< Enable Accelerated Transition to Modified protocol in L3
  UINT32 :4;                        ///< Reserved
} L3_CONTROL_1_REGISTER;

/* L3 Cache Parameter Register F3x1C4 */
#define L3_CACHE_PARAM_REG 0x1C4

/// L3 Cache Parameter PCI Register
typedef struct {
  UINT32 L3SubcacheSize0:4;          ///< L3 subcache size 0
  UINT32 L3SubcacheSize1:4;          ///< L3 subcache size 1
  UINT32 L3SubcacheSize2:4;          ///< L3 subcache size 2
  UINT32 L3SubcacheSize3:4;          ///< L3 subcache size 3
  UINT32 :15;                        ///< Reserved
  UINT32 L3TagInit:1;                ///< L3 tag initialization
} L3_CACHE_PARAM_REGISTER;


/* Probe Filter Control Register F3x1D4 */
#define PROBE_FILTER_CTRL_REG 0x1D4

/// Probe Filter Control PCI Register
typedef struct {
  UINT32 PFMode:2;                   ///< Probe Filter Mode
  UINT32 PFWayNum:2;                 ///< Probe Filter way number
  UINT32 PFSubCacheSize0:2;          ///< Probe filter subcache 0 size
  UINT32 PFSubCacheSize1:2;          ///< Probe filter subcache 1 size
  UINT32 PFSubCacheSize2:2;          ///< Probe filter subcache 2 size
  UINT32 PFSubCacheSize3:2;          ///< Probe filter subcache 3 size
  UINT32 PFSubCacheEn:4;             ///< Probe filter subcache enable
  UINT32 DisDirectedPrb:1;           ///< Disable directed probes
  UINT32 PFWayHashEn:1;              ///< Probe filter cache way hash enable
  UINT32 :1;                         ///< Reserved
  UINT32 PFInitDone:1;               ///< Probe filter initialization done
  UINT32 PFPreferredSORepl:2;        ///< PF preferredSO replacement mode
  UINT32 PFErrInt:2;                 ///< Probe filter error interrupt type
  UINT32 LvtOffset:4;                ///< Probe filter error interrupt LVT offset
  UINT32 PFEccError:1;               ///< Probe filter ECC error
  UINT32 PFLoIndexHashEn:1;          ///< Probe filter low index hash enable
  UINT32 DisPrbFilterInit:1;         ///< Disable probe filter initialization
  UINT32 SmallPFDirEn:1;             ///< Small probe filter directory enable
} PROBE_FILTER_CTRL_REGISTER;


/* Product Info Register F3x1FC */
#define PRCT_INFO_REG 0x1FC

/// Product Information PCI Register
typedef struct {
  UINT32 DiDtMode:1;                 ///< DiDtMode
  UINT32 DiDtCfg0:5;                 ///< DiDtCfg0
  UINT32 DiDtCfg1:8;                 ///< DiDtCfg1
  UINT32 AlwaysOnThrottle:2;         ///< AlwaysOnThrottle
  UINT32 Pipe3ThrottleDis:1;         ///< Pipe3ThrottleDis
  UINT32 :15;                        ///< Reserved
} PRODUCT_INFO_REGISTER;


/* C-state Control 1 Register D18F4x118 */
#define CSTATE_CTRL1_REG 0x118
#define CSTATE_CTRL1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_CTRL1_REG))

/// C-state Control 1 Register
typedef struct {
  UINT32 CpuPrbEnCstAct0:1;          ///< Core direct probe enable
  UINT32 CacheFlushEnCstAct0:1;      ///< Cache flush enable
  UINT32 CacheFlushTmrSelCstAct0:2;  ///< Cache flush timer select
  UINT32 :1;                         ///< Reserved
  UINT32 ClkDivisorCstAct0:3;        ///< Clock divisor
  UINT32 PwrGateEnCstAct0:1;         ///< Power gate enable
  UINT32 :1;                         ///< Reserved
  UINT32 :6;                         ///< Reserved
  UINT32 CpuPrbEnCstAct1:1;          ///< Core direct probe enable
  UINT32 CacheFlushEnCstAct1:1;      ///< Cache flush eable
  UINT32 CacheFlushTmrSelCstAct1:2;  ///< Cache flush timer select
  UINT32 :1;                         ///< Reserved
  UINT32 ClkDivisorCstAct1:3;        ///< Clock divisor
  UINT32 PwrGateEnCstAct1:1;         ///< Power gate enable
  UINT32 :1;                         ///< Reserved
  UINT32 :6;                         ///< Reserved
} CSTATE_CTRL1_REGISTER;


/* C-state Control 2 Register D18F4x11C */
#define CSTATE_CTRL2_REG 0x11C
#define CSTATE_CTRL2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_CTRL2_REG))

/// C-state Control 2 Register
typedef struct {
  UINT32 CpuPrbEnCstAct2:1;          ///< Core direct probe enable
  UINT32 CacheFlushEnCstAct2:1;      ///< Cache flush eable
  UINT32 CacheFlushTmrSelCstAct2:2;  ///< Cache flush timer select
  UINT32 AltvidEnCstAct2:1;          ///< Core altvid enable
  UINT32 ClkDivisorCstAct2:3;        ///< Clock divisor
  UINT32 PwrGateEnCstAct2:1;         ///< Power gate enable
  UINT32 PwrOffEnCstAct2:1;          ///< C-state action field 3
  UINT32 :22;                        ///< Reserved
} CSTATE_CTRL2_REGISTER;


/* Cstate Policy Control 1 Register D18F4x128 */
#define CSTATE_POLICY_CTRL1_REG 0x128
#define CSTATE_POLICY_CTRL1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_POLICY_CTRL1_REG))

/// Cstate Policy Control 1 Register
typedef struct {
  UINT32 CoreCStateMode:1;            ///< Specifies C-State actions
  UINT32 CoreCstatePolicy:1;          ///< Specified processor arbitration of voltage and frequency
  UINT32 HaltCstateIndex:3;           ///< Specifies the IO-based C-state that is invoked by a HLT instruction
  UINT32 CacheFlushTmr:7;             ///< Cache flush timer
  UINT32 CoreStateSaveDestnode:6;     ///< Core state save destination node
  UINT32 CacheFlushSucMonThreshold:3; ///< Cache flush success monitor threshold
  UINT32 :10;                         ///< Reserved
  UINT32 CstateMsgDis:1;              ///< C-state messaging disable
} CSTATE_POLICY_CTRL1_REGISTER;


/* Core Performance Boost Control Register D18F4x15C */

/// Core Performance Boost Control Register
typedef struct {
  UINT32 BoostSrc:2;                 ///< Boost source
  UINT32 NumBoostStates:3;           ///< Number of boosted states
  UINT32 :2;                         ///< Reserved
  UINT32 ApmMasterEn:1;              ///< APM master enable
  UINT32 :20;                        ///< Reserved
  UINT32 TdpLimitPstate:3;           ///< Highest performance pstate
  UINT32 BoostLock:1;                ///<
} CPB_CTRL_REGISTER;


/* Northbridge P-state [3:0] F5x1[6C:60]  */

/// Northbridge P-state Register
typedef struct {
  UINT32 NbPstateEn:1;               ///< NB P-state enable
  UINT32 NbFid:5;                    ///< NB frequency ID
  UINT32 :1;                         ///< Reserved
  UINT32 NbDid:1;                    ///< NB divisor ID
  UINT32 :2;                         ///< Reserved
  UINT32 NbVid:7;                    ///< NB VID
  UINT32 :15;                        ///< Reserved
} NB_PSTATE_REGISTER;


/* Northbridge P-state Status */
#define NB_PSTATE_CTRL 0x170
#define NB_PSTATE_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_CTRL))

/// Northbridge P-state Control Register
typedef struct {
  UINT32 NbPstateMaxVal:2;           ///< NB P-state maximum value
  UINT32 :1;                         ///< Reserved
  UINT32 NbPstateLo:2;               ///< NB P-state low
  UINT32 :1;                         ///< Reserved
  UINT32 NbPstateHi:2;               ///< NB P-state high
  UINT32 :1;                         ///< Reserved
  UINT32 NbPstateThreshold:3;        ///< NB P-state threshold
  UINT32 :1;                         ///< Reserved
  UINT32 NbPstateDisOnP0:1;          ///< NB P-state disable on P0
  UINT32 SwNbPstateLoDis:1;          ///< Software NB P-state low disable
  UINT32 :17;                        ///< Reserved
} NB_PSTATE_CTRL_REGISTER;


/* Northbridge P-state Status */
#define NB_PSTATE_STATUS 0x174
#define NB_PSTATE_STATUS_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_STATUS))

/// Northbridge P-state Status Register
typedef struct {
  UINT32 NbPstateDis:1;              ///< Nb pstate disable
  UINT32 StartupNbPstate:2;          ///< startup northbridge Pstate number
  UINT32 CurNbFid:5;                 ///< Current NB FID
  UINT32 :1;                         ///< Reserved
  UINT32 CurNbDid:1;                 ///< Current NB DID
  UINT32 :2;                         ///< Reserved
  UINT32 CurNbVid:7;                 ///< Current NB VID
  UINT32 CurNbPstate:2;              ///< Current NB Pstate
  UINT32 :11;                        ///< Reserved
} NB_PSTATE_STS_REGISTER;

#endif /* _CPU_F15_OR_POWERMGMT_H_ */
