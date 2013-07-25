/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_16 Kabini Power Management related registers defination
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16/KB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_F16_KB_POWERMGMT_H_
#define _CPU_F16_KB_POWERMGMT_H_

/*
 * Family 16h Kabini CPU Power Management MSR definitions
 *
 */


/* NB Machine Check Misc 4 MSR Register 0x00000413 */
#define MC4_MISC0 0x00000413ul

/// Interrupt Pending and CMP-Halt MSR Register
typedef struct {
  UINT64 :24;                        ///< Reserved
  UINT64 BlkPtr:8;                   ///< Block pointer for additional MISC registers
  UINT64 ErrCnt:12;                  ///< Error counter
  UINT64 :4;                         ///< Reserved
  UINT64 Ovrflw:1;                   ///< Overflow
  UINT64 IntType:2;                  ///< Interrupt type
  UINT64 CntEn:1;                    ///< Counter enable
  UINT64 LvtOffset:4;                ///< LVT offset
  UINT64 :5;                         ///< Reserved
  UINT64 Locked:1;                   ///< Locked
  UINT64 CntP:1;                     ///< Counter present
  UINT64 Valid:1;                    ///< Valid
} MC4_MISC0_MSR;

/* P-state Registers 0xC00100[6B:64] */

/// P-state MSR
typedef struct {
  UINT64 CpuFid:6;                   ///< CpuFid
  UINT64 CpuDid:3;                   ///< CpuDid
  UINT64 CpuVid:8;                   ///< CpuVid
  UINT64 :5;                         ///< Reserved
  UINT64 NbPstate:1;                 ///< NbPstate
  UINT64 :9;                         ///< Reserved
  UINT64 IddValue:8;                 ///< IddValue
  UINT64 IddDiv:2;                   ///< IddDiv
  UINT64 :21;                        ///< Reserved
  UINT64 PsEnable:1;                 ///< Pstate Enable
} PSTATE_MSR;

#define GetF16KbCpuVid(PstateMsr) (((PSTATE_MSR *) PstateMsr)->CpuVid)


/* VID operation related macros */
#define ConvertVidInuV(Vid)   (1550000 - (6250 * Vid)) ///< Convert VID in uV.

/* COFVID Control Register 0xC0010070 */
#define MSR_COFVID_CTL 0xC0010070ul

/// COFVID Control MSR Register
typedef struct {
  UINT64 CpuFid:6;                   ///< CpuFid
  UINT64 CpuDid:3;                   ///< CpuDid
  UINT64 CpuVid_6_0:7;               ///< CpuVid[6:0]
  UINT64 PstateId:3;                 ///< Pstate ID
  UINT64 :1;                         ///< Reserved
  UINT64 CpuVid_7:1;                 ///< CpuVid[7]
  UINT64 :1;                         ///< Reserved
  UINT64 NbPstate:1;                 ///< Northbridge P-state
  UINT64 :1;                         ///< Reserved
  UINT64 NbVid:8;                    ///< NbVid
  UINT64 :32;                        ///< Reserved
} COFVID_CTRL_MSR;

#define COFVID_CTRL_MSR_CurCpuVid_6_0_OFFSET       9
#define COFVID_CTRL_MSR_CurCpuVid_6_0_WIDTH        7
#define COFVID_CTRL_MSR_CurCpuVid_6_0_MASK         0xfe00
#define COFVID_CTRL_MSR_CurCpuVid_7_OFFSET         20
#define COFVID_CTRL_MSR_CurCpuVid_7_WIDTH          1
#define COFVID_CTRL_MSR_CurCpuVid_7_MASK           0x100000ul

/* SVI VID Encoding */

///< Union structure of VID in SVI1/SVI2 modes
typedef union {
  UINT32     RawVid;                 ///< Raw VID value
  struct {                           ///< SVI2 mode VID structure
    UINT32   Vid_6_0:7;              ///< Vid[6:0] of SVI2 mode
    UINT32   Vid_7:1;                ///< Vid[7] of SVI2 mode
  } SVI2;
  struct {                           ///< SVI1 mode VID structure
    UINT32   Vid_LSB_Ignore:1;       ///< Ignored LSB of 8bit VID encoding in SVI1 mode
    UINT32   Vid_6_0:1;              ///< Vid[6:0] of SVI mode
  } SVI1;
} SVI_VID;


#define SetF16KbCpuVid(CofVidStsMsr, NewCpuVid) ( \
  ((COFVID_CTRL_MSR *) CofVidStsMsr)->CurCpuVid_6_0) = ((SVI_VID *) NewCpuVid)->SVI2.Vid_6_0; \
  ((COFVID_CTRL_MSR *) CofVidStsMsr)->CurCpuVid_7) = ((SVI_VID *) NewCpuVid)->SVI2.Vid_7; \
)


/* COFVID Status Register 0xC0010071 */
#define MSR_COFVID_STS 0xC0010071ul

/// COFVID Status MSR Register
typedef struct {
  UINT64 CurCpuFid:6;                ///< Current CpuFid
  UINT64 CurCpuDid:3;                ///< Current CpuDid
  UINT64 CurCpuVid_6_0:7;            ///< Current CpuVid[6:0]
  UINT64 CurPstate:3;                ///< Current Pstate
  UINT64 :1;                         ///< Reserved
  UINT64 CurCpuVid_7:1;              ///< Current CpuVid[7]
  UINT64 :2;                         ///< Reserved
  UINT64 NbPstateDis:1;              ///< NbPstate Disable
  UINT64 :8;
  UINT64 StartupPstate:3;            ///< Startup Pstate
  UINT64 :14;                        ///< Reserved
  UINT64 MaxCpuCof:6;                ///< MaxCpuCof
  UINT64 :1;                         ///< Reserved
  UINT64 CurPstateLimit:3;           ///< Current Pstate Limit
  UINT64 MaxNbCof:5;                 ///< MaxNbCof
} COFVID_STS_MSR;

#define COFVID_STS_MSR_CurCpuVid_6_0_OFFSET       9
#define COFVID_STS_MSR_CurCpuVid_6_0_WIDTH        7
#define COFVID_STS_MSR_CurCpuVid_6_0_MASK         0xfe00
#define COFVID_STS_MSR_CurCpuVid_7_OFFSET         20
#define COFVID_STS_MSR_CurCpuVid_7_WIDTH          1
#define COFVID_STS_MSR_CurCpuVid_7_MASK           0x100000ul

#define GetF16KbCurCpuVid(CofVidStsMsr) ( \
  (((COFVID_STS_MSR *) CofVidStsMsr)->CurCpuVid_7 << COFVID_STS_MSR_CurCpuVid_6_0_WIDTH) \
  | ((COFVID_STS_MSR *) CofVidStsMsr)->CurCpuVid_6_0)


/*
 * Family 16h Kabini CPU Power Management PCI definitions
 *
 */
/* Link Initialization Status D18F0x1A0 */
#define LINK_INIT_STATUS_REG 0x1A0
#define LINK_INIT_STATUS_REG_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_0, LINK_INIT_STATUS_REG))

/// Link Initialization Status
typedef struct {
  UINT32 InitComplete0:2;            ///< Initialization complete for link 0
  UINT32 InitComplete1:2;            ///< Initialization complete for link 1
  UINT32 :20;                        ///< Reserved
  UINT32 IntNbExtCap:4;              ///< Internal NB extneded capability
  UINT32 :3;                         ///< Reserved
  UINT32 InitStatusValid:1;          ///< Initialization status valid
} LINK_INIT_STATUS_REGISTER;

/* NB Configuration 1 Low F3x88 */
#define NB_CFG1_LOW_REG 0x88
#define NB_CFG1_LOW_REG_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, NB_CFG1_LOW_REG))

/// Power Control Miscellaneous PCI Register
typedef struct {
  UINT32 :18;                        ///< Reserved
  UINT32 DisCstateBoostBlockPstateUp:1; ///< DisCstateBoostBlockPstateUp
  UINT32 :12;                        ///< Reserved
  UINT32 DisCohLdtCfg:1;             ///< Disable coherent link configuration accesses
} NB_CFG1_LOW_REG_REGISTER;

/* Power Control Miscellaneous Register F3xA0 */
#define PW_CTL_MISC_REG 0xA0
#define PW_CTL_MISC_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, PW_CTL_MISC_REG))

/// Power Control Miscellaneous PCI Register
typedef struct {
  UINT32 PsiVid:7;                   ///< PSI_L VID threshold VID[6:0]
  UINT32 PsiVidEn:1;                 ///< PSI_L VID enable
  UINT32 PsiVid_7:1;                 ///< PSI_L VID threshold VID[7]
  UINT32 :1;                         ///< Reserved
  UINT32 IdleExitEn:1;               ///< Idle exit enable
  UINT32 PllLockTime:3;              ///< PLL synchronization lock time
  UINT32 Svi2HighFreqSel:1;          ///< SVI2 high frequency select
  UINT32 :1;                         ///< Reserved
  UINT32 ConfigId:12;                ///< Configuration ID
  UINT32 :3;                         ///< Reserved
  UINT32 CofVidProg:1;               ///< COF and VID of Pstate programmed
} POWER_CTRL_MISC_REGISTER;


/* Clock Power/Timing Control 0 Register F3xD4 */
#define CPTC0_REG 0xD4
#define CPTC0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC0_REG))

/// Clock Power Timing Control PCI Register
typedef struct {
  UINT32 MaxSwPstateCpuCof:6;        ///< Maximum software P-state core COF
  UINT32 :2;                         ///< Reserved
  UINT32 ClkRampHystSel:4;           ///< Clock Ramp Hysteresis Select
  UINT32 ClkRampHystCtl:1;           ///< Clock Ramp Hysteresis Control
  UINT32 :1;                         ///< Reserved
  UINT32 CacheFlushImmOnAllHalt:1;   ///< Cache Flush Immediate on All Halt
  UINT32 :5;                         ///< Reserved
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
  UINT32 :25;                        ///< Reserved
} CLK_PWR_TIMING_CTRL1_REGISTER;


/* Northbridge Capabilities Register F3xE8 */
#define NB_CAPS_REG 0xE8
#define NB_CAPS_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, NB_CAPS_REG))

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
  UINT32 :3;                         ///< Reserved
  UINT32 MultVidPlane:1;             ///< Multiple VID plane capable
  UINT32 :4;                         ///< Reserved
  UINT32 x2Apic:1;                   ///< x2Apic capability
  UINT32 :4;                         ///< Reserved
  UINT32 MemPstateCap:1;             ///< Memory P-state capable
  UINT32 :3;                         ///< Reserved
  UINT32 Succor:1;                   ///< SUCCOR
  UINT32 :3;                         ///< Reserved
} NB_CAPS_REGISTER;

/* Sample and Residency Timers Register D18F4x110 */
#define SAMPLE_RESIDENCY_TIMER_REG 0x110
#define SAMPLE_RESIDENCY_TIMER_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, SAMPLE_RESIDENCY_TIMER_REG))

/// Sample and Residency Timers Register
typedef struct {
  UINT32 CSampleTimer:12;            ///< CSampleTimer
  UINT32 FastCSampleTimer:1;         ///< FastCSampleTimer
  UINT32 MinResTmr:8;                ///< Minimum residency timer
  UINT32 :11;                        ///< Reserved
} SAMPLE_RESIDENCY_TIMER_REGISTER;

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
  UINT32 PwrOffEnCstAct0:1;          ///< C-state action field 3
  UINT32 NbPwrGate0:1;               ///< NB power-gating 0
  UINT32 NbClkGate0:1;               ///< NB clock-gating 0
  UINT32 SelfRefr0:1;                ///< Self-refresh 0
  UINT32 SelfRefrEarly0:1;           ///< Allow early self-refresh 0
  UINT32 :2;                         ///< Reserved
  UINT32 CpuPrbEnCstAct1:1;          ///< Core direct probe enable
  UINT32 CacheFlushEnCstAct1:1;      ///< Cache flush eable
  UINT32 CacheFlushTmrSelCstAct1:2;  ///< Cache flush timer select
  UINT32 :1;                         ///< Reserved
  UINT32 ClkDivisorCstAct1:3;        ///< Clock divisor
  UINT32 PwrGateEnCstAct1:1;         ///< Power gate enable
  UINT32 PwrOffEnCstAct1:1;          ///< C-state action field 3
  UINT32 NbPwrGate1:1;               ///< NB power-gating 1
  UINT32 NbClkGate1:1;               ///< NB clock-gating 1
  UINT32 SelfRefr1:1;                ///< Self-refresh 1
  UINT32 SelfRefrEarly1:1;           ///< Allow early self-refresh 1
  UINT32 :2;                         ///< Reserved
} CSTATE_CTRL1_REGISTER;


/* C-state Control 2 Register D18F4x11C */
#define CSTATE_CTRL2_REG 0x11C
#define CSTATE_CTRL2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_CTRL2_REG))

/// C-state Control 2 Register
typedef struct {
  UINT32 CpuPrbEnCstAct2:1;          ///< Core direct probe enable
  UINT32 CacheFlushEnCstAct2:1;      ///< Cache flush eable
  UINT32 CacheFlushTmrSelCstAct2:2;  ///< Cache flush timer select
  UINT32 :1;                         ///< Reserved
  UINT32 ClkDivisorCstAct2:3;        ///< Clock divisor
  UINT32 PwrGateEnCstAct2:1;         ///< Power gate enable
  UINT32 PwrOffEnCstAct2:1;          ///< C-state action field 3
  UINT32 NbPwrGate2:1;               ///< NB power-gating 2
  UINT32 NbClkGate2:1;               ///< NB clock-gating 2
  UINT32 SelfRefr2:1;                ///< Self-refresh 2
  UINT32 SelfRefrEarly2:1;           ///< Allow early self-refresh 2
  UINT32 :18;                        ///< Reserved
} CSTATE_CTRL2_REGISTER;


/* Cstate Policy Control 1 Register D18F4x128 */
#define CSTATE_POLICY_CTRL1_REG 0x128
#define CSTATE_POLICY_CTRL1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_POLICY_CTRL1_REG))

/// Cstate Policy Control 1 Register
typedef struct {
  UINT32 :2;                          ///< Reserved
  UINT32 HaltCstateIndex:3;           ///< Specifies the IO-based C-state that is invoked by a HLT instruction
  UINT32 CacheFlushTmr:7;             ///< Cache flush timer
  UINT32 :6;                          ///< Reserved
  UINT32 CacheFlushSucMonThreshold:3; ///< Cache flush success monitor threshold
  UINT32 CacheFlushSucMonTmrSel:2;    ///< Cache flush success monitor timer select
  UINT32 CacheFlushSucMonMispredictAct:2; ///< Cache flush success monitor mispredict action
  UINT32 :6;                          ///< Reserved
  UINT32 CstateMsgDis:1;              ///< C-state messaging disable
} CSTATE_POLICY_CTRL1_REGISTER;


/* Core Performance Boost Control Register D18F4x15C */

/// Core Performance Boost Control Register
typedef struct {
  UINT32 BoostSrc:2;                 ///< Boost source
  UINT32 NumBoostStates:3;           ///< Number of boosted states
  UINT32 :2;                         ///< Reserved
  UINT32 ApmMasterEn:1;              ///< APM master enable
  UINT32 CstatePowerEn:1;            ///< C-state Power Enable
  UINT32 :22;                        ///< Reserved
  UINT32 BoostLock:1;                ///<
} CPB_CTRL_REGISTER;


/* Northbridge Capabilities 2  F5x84*/
#define NB_CAPS_REG2 0x84
#define NB_CAPS_REG2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_CAPS_REG2))

/// Northbridge Capabilities 2 PCI Register
typedef struct {
  UINT32 CmpCap:8;                   ///< CMP capable
  UINT32 :4;                         ///< Reserved
  UINT32 DctEn:4;                    ///< DCT enabled
  UINT32 DdrMaxRate:5;               ///< maximum DDR rate
  UINT32 :3;                         ///< Reserved
  UINT32 DdrMaxRateEnf:5;            ///< enforced maximum DDR rate:
  UINT32 :3;                         ///< Reserved
} NB_CAPS_2_REGISTER;

/* Northbridge Configuration 4  F5x88*/
#define NB_CFG_REG4 0x88
#define NB_CFG_REG4_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_CFG_REG4))

/// Northbridge Configuration 4 PCI Register
typedef struct {
  UINT32 CC6PstateWakeUpDis:1;       ///< CC6PstateWakeUpDis
  UINT32 :17;                        ///< Reserved
  UINT32 EnCstateBoostBlockCC6Exit:1;///< EnCstateBoostBlockCC6Exit
  UINT32 :5;                         ///< Reserved
  UINT32 DisHbNpReqBusLock:1;        ///< DisHbNpReqBusLock
  UINT32 :7;                         ///< Reserved
} NB_CFG_4_REGISTER;

typedef struct {
  UINT32 Bits_19_0:20;               ///< Bits[19:0]
  UINT32 Bits_31_20:12;              ///< Reserved
} pmgmt_str0;

typedef struct {
  UINT32 Bits_19_0:20;               ///< Bits[19:0]
  UINT32 Bits_31_20:12;              ///< Reserved
} pmgmt_str1;

/* Northbridge P-state [3:0] F5x1[6C:60]  */

/// Northbridge P-state Register
typedef struct {
  UINT32 NbPstateEn:1;               ///< NB P-state enable
  UINT32 NbFid:6;                    ///< NB frequency ID
  UINT32 NbDid:1;                    ///< NB divisor ID
  UINT32 :2;                         ///< Reserved
  UINT32 NbVid_6_0:7;                ///< NB VID[6:0]
  UINT32 :1;                         ///< Reserved
  UINT32 MemPstate:1;                ///< Memory P-State
  UINT32 :2;                         ///< Reserved
  UINT32 NbVid_7:1;                  ///< NB VID[7]
  UINT32 NbIddDiv:2;                 ///< northbridge current divisor
  UINT32 NbIddValue:8;               ///< northbridge current value
} NB_PSTATE_REGISTER;

#define NB_PSTATE_REGISTER_NbVid_6_0_OFFSET      10
#define NB_PSTATE_REGISTER_NbVid_6_0_WIDTH       7
#define NB_PSTATE_REGISTER_NbVid_6_0_MASK        0x0001FC00ul
#define NB_PSTATE_REGISTER_NbVid_7_OFFSET        21
#define NB_PSTATE_REGISTER_NbVid_7_WIDTH         1
#define NB_PSTATE_REGISTER_NbVid_7_MASK          0x00200000ul

#define GetF16KbNbVid(NbPstateRegister) ( \
  (((NB_PSTATE_REGISTER *) NbPstateRegister)->NbVid_7 << NB_PSTATE_REGISTER_NbVid_6_0_WIDTH) \
  | ((NB_PSTATE_REGISTER *) NbPstateRegister)->NbVid_6_0)

#define SetF16KbNbVid(NbPstateRegister, NewNbVid) { \
  ((NB_PSTATE_REGISTER *) NbPstateRegister)->NbVid_6_0 = ((SVI_VID *) NewNbVid)->SVI2.Vid_6_0; \
  ((NB_PSTATE_REGISTER *) NbPstateRegister)->NbVid_7 = ((SVI_VID *) NewNbVid)->SVI2.Vid_7; \
}

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
  UINT32 NbPstateThreshold:4;        ///< NB P-state threshold
  UINT32 NbPstateDisOnP0:1;          ///< NB P-state disable on P0
  UINT32 SwNbPstateLoDis:1;          ///< Software NB P-state low disable
  UINT32 :8;                         ///< Reserved
  UINT32 NbPstateGnbSlowDis:1;       ///< NbPstateGnbSlowDis
  UINT32 NbPstateLoRes:3;            ///< NB P-state low residency timer
  UINT32 NbPstateHiRes:3;            ///< NB P-state high residency timer
  UINT32 NbPstateFidVidSbcEn:1;      ///< NbPstateFidVidSbcEn
  UINT32 MemPstateDis:1;             ///< Memory P-state disable
} NB_PSTATE_CTRL_REGISTER;


/* Northbridge P-state Status */
#define NB_PSTATE_STATUS 0x174
#define NB_PSTATE_STATUS_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_STATUS))

/// Northbridge P-state Status Register
typedef struct {
  UINT32 NbPstateDis:1;              ///< Nb pstate disable
  UINT32 StartupNbPstate:2;          ///< startup northbridge Pstate number
  UINT32 CurNbFid:6;                 ///< Current NB FID
  UINT32 CurNbDid:1;                 ///< Current NB DID
  UINT32 :2;                         ///< Reserved
  UINT32 CurNbVid_6_0:7;             ///< Current NB VID[6:0]
  UINT32 CurNbPstate:2;              ///< Current NB Pstate
  UINT32 :1;                         ///< Reserved
  UINT32 CurNbPstateLo:1;            ///< Current NB Pstate maps to Lo or Hi
  UINT32 CurNbVid_7:1;               ///< Current NB VID[7]
  UINT32 CurMemPstate:1;             ///< Current memory P-state
  UINT32 :7;                         ///< Reserved
} NB_PSTATE_STS_REGISTER;

#define NB_PSTATE_STS_REGISTER_CurNbVid_6_0_OFFSET      12
#define NB_PSTATE_STS_REGISTER_CurNbVid_6_0_WIDTH       7
#define NB_PSTATE_STS_REGISTER_CurNbVid_6_0_MASK        0x0007F000ul
#define NB_PSTATE_STS_REGISTER_CurNbVid_7_OFFSET        23
#define NB_PSTATE_STS_REGISTER_CurNbVid_7_WIDTH         1
#define NB_PSTATE_STS_REGISTER_CurNbVid_7_MASK          0x00800000ul

#define GetF16KbCurNbVid(NbPstateStsRegister) ( \
  (((NB_PSTATE_STS_REGISTER *) NbPstateStsRegister)->CurNbVid_7 << NB_PSTATE_STS_REGISTER_CurNbVid_6_0_WIDTH) \
  | ((NB_PSTATE_STS_REGISTER *) NbPstateStsRegister)->CurNbVid_6_0)

/* Miscellaneous Voltages */
#define MISC_VOLTAGES 0x17C
#define MISC_VOLTAGES_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, MISC_VOLTAGES))

/// Miscellaneous Voltages Register
typedef struct {
  UINT32 MaxVid:8;                   ///< Maximum Voltage
  UINT32 :2;                         ///< Reserved
  UINT32 MinVid:8;                   ///< Minimum Voltage
  UINT32 :5;                         ///< Reserved
  UINT32 NbPsi0Vid:8;                ///< Northbridge PSI0_L VID threshold
  UINT32 NbPsi0VidEn:1;              ///< Northbridge PSI0_L VID enable
} MISC_VOLTAGE_REGISTER;


/* Clock Power/Timing Control 5 Register F5x188 */
#define CPTC5_REG 0x188
#define CPTC5_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, CPTC5_REG))

/// Clock Power Timing Control 5 Register
typedef struct {
  UINT32 NbOffsetTrim:2;             ///< Northbridge offset trim
  UINT32 NbLoadLineTrim:3;           ///< Northbridge load line trim
  UINT32 NbPsi1:1;                   ///< Northbridge PSI1_L
  UINT32 :1;                    ///< Northbridge telemetry functionality.
  UINT32 :25;                        ///< Reserved
} CLK_PWR_TIMING_CTRL_5_REGISTER;

#endif /* _CPU_F16_KB_POWERMGMT_H_ */
