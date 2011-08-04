/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_14 Power Management related stuff
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F14
 * @e \$Revision: 46836 $   @e \$Date: 2011-02-10 12:22:59 -0700 (Thu, 10 Feb 2011) $
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

#ifndef _CPUF14POWERMGMT_H_
#define _CPUF14POWERMGMT_H_

/*
 * Family 14h CPU Power Management MSR definitions
 *
 */

/* P-state Current Limit Register 0xC0010061 */
#define MSR_PSTATE_CURRENT_LIMIT 0xC0010061

/// Pstate Current Limit MSR Register
typedef struct {
  UINT64 CurPstateLimit:3;           ///< Current Pstate Limit
  UINT64 :1;                         ///< Reserved
  UINT64 PstateMaxVal:3;             ///< Pstate Max Value
  UINT64 :57;                        ///< Reserved
} PSTATE_CURLIM_MSR;


/* P-state Control Register 0xC0010062 */
#define MSR_PSTATE_CTL 0xC0010062

/// Pstate Control MSR Register
typedef struct {
  UINT64 PstateCmd:3;                ///< Pstate change command
  UINT64 :61;                        ///< Reserved
} PSTATE_CTRL_MSR;


/* P-state Status Register 0xC0010063 */
#define MSR_PSTATE_STS 0xC0010063

/// Pstate Status MSR Register
typedef struct {
  UINT64 CurPstate:3;                ///< Current Pstate
  UINT64 :61;                        ///< Reserved
} PSTATE_STS_MSR;


/* P-state Registers 0xC001006[B:4] */
#define MSR_PSTATE_0 0xC0010064
#define MSR_PSTATE_1 0xC0010065
#define MSR_PSTATE_2 0xC0010066
#define MSR_PSTATE_3 0xC0010067
#define MSR_PSTATE_4 0xC0010068
#define MSR_PSTATE_5 0xC0010069
#define MSR_PSTATE_6 0xC001006A
#define MSR_PSTATE_7 0xC001006B

#define PS_REG_BASE  MSR_PSTATE_0    /* P-state Register base */
#define PS_MAX_REG   MSR_PSTATE_7    /* Maximum P-State Register */
#define PS_MIN_REG   MSR_PSTATE_0    /* Minimum P-State Register */
#define NM_PS_REG    8               /* number of P-state MSR registers */

/// Pstate MSR
typedef struct {
  UINT64 CpuDidLSD:4;                ///< CPU core divisor identifier least significant digit
  UINT64 CpuDidMSD:5;                ///< CPU core divisor identifier most significant digit
  UINT64 CpuVid:7;                   ///< CPU core VID
  UINT64 :16;                        ///< Reserved
  UINT64 IddValue:8;                 ///< Current value field
  UINT64 IddDiv:2;                   ///< Current divisor field
  UINT64 :21;                        ///< Reserved
  UINT64 PsEnable:1;                 ///< P-state Enable
} PSTATE_MSR;


/* COFVID Control Register 0xC0010070 */
#define MSR_COFVID_CTL 0xC0010070

/// COFVID Control MSR Register
typedef struct {
  UINT64 CpuDid:4;                   ///< CPU core divisor identifier
  UINT64 CpuDidMSD:5;                ///< CPU core frequency identifier
  UINT64 CpuVid:7;                   ///< CPU core VID
  UINT64 PstateId:3;                 ///< P-state identifier
  UINT64 IgnoreFidVidDid:1;          ///< Ignore FID, VID, and DID
  UINT64 :44;                        ///< Reserved
} COFVID_CTRL_MSR;


/* COFVID Status Register 0xC0010071 */
#define MSR_COFVID_STS 0xC0010071

/// COFVID Status MSR Register
typedef struct {
  UINT64 CurCpuDid:4;                ///< Current CPU core divisor ID
  UINT64 CurCpuDidMSD:5;             ///< Current CPU core frequency ID
  UINT64 CurCpuVid:7;                ///< Current CPU core VID
  UINT64 CurPstate:3;                ///< Current P-state
  UINT64 :1;                         ///< Reserved
  UINT64 PstateInProgress:1;         ///< P-state change in progress
  UINT64 :4;                         ///< Reserved
  UINT64 CurNbVid:7;                 ///< Current northbridge VID
  UINT64 StartupPstate:3;            ///< Startup P-state number
  UINT64 MaxVid:7;                   ///< Maximum voltage
  UINT64 MinVid:7;                   ///< Minimum voltage
  UINT64 MainPllOpFreqIdMax:6;       ///< Main PLL operating frequency ID maximum
  UINT64 :1;                         ///< Reserved
  UINT64 CurPstateLimit:3;           ///< Current P-state Limit
  UINT64 :5;                         ///< Reserved
} COFVID_STS_MSR;


/* C-state Address Register 0xC0010073 */
#define MSR_CSTATE_ADDRESS 0xC0010073

/// C-state Address MSR Register
typedef struct {
  UINT64 CstateAddr:16;              ///< C-state address
  UINT64 :48;                        ///< Reserved
} CSTATE_ADDRESS_MSR;


/* CPU Watchdog Timer Register 0xC0010074 */
#define MSR_CPU_WDT 0xC0010074

/// CPU Watchdog Timer Register
typedef struct {
  UINT64 CpuWdtEn:1;                 ///< CPU watchdog timer enable
  UINT64 CpuWdtTimeBase:2;           ///< CPU watchdog timer time base
  UINT64 CpuWdtCountSel:4;           ///< CPU watchdog timer count select
  UINT64 :57;                        ///< Reserved
} CPU_WDT_MSR;


/*
 * Family 14h CPU Power Management PCI definitions
 *
 */

/* Memory controller configuration low register D18F2x118 */
#define MEM_CFG_LOW_REG 0x118
#define MEM_CFG_LOW_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_2, MEM_CFG_LOW_REG))

/// Memory Controller Configuration Low
typedef struct {
  UINT32 MctPriCpuRd:2;              ///< CPU read priority
  UINT32 MctPriCpuWr:2;              ///< CPU write priority
  UINT32 MctPriHiRd:2;               ///< High-priority VC set read priority
  UINT32 MctPriHiWr:2;               ///< High-priority VC set write priority
  UINT32 MctPriDefault:2;            ///< Default non-write priority
  UINT32 MctPriWr:2;                 ///< Default write priority
  UINT32 :7;                         ///< Reserved
  UINT32 C6DramLock:1;               ///< C6 DRAM lock
  UINT32 :8;                         ///< Reserved
  UINT32 MctVarPriCntLmt:4;          ///< Variable priority time limit
} MEM_CFG_LOW_REGISTER;


/* Hardware thermal control register D18F3x64 */
#define HTC_REG 0x64
#define HTC_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, HTC_REG))

/// Hardware Thermal Control PCI Register
typedef struct {
  UINT32 HtcEn:1;                    ///< HTC Enable
  UINT32 :3;                         ///< Reserved
  UINT32 HtcAct:1;                   ///< HTC Active State
  UINT32 HtcActSts:1;                ///< HTC Active Status
  UINT32 PslApicHiEn:1;              ///< P-state limit higher APIC int enable
  UINT32 PslApicLoEn:1;              ///< P-state limit lower APIC int enable
  UINT32 :8;                         ///< Reserved
  UINT32 HtcTmpLmt:7;                ///< HTC temperature limit
  UINT32 HtcSlewSel:1;               ///< HTC slew-controlled temp select
  UINT32 HtcHystLmt:4;               ///< HTC hysteresis
  UINT32 HtcPstateLimit:3;           ///< HTC P-state limit select
  UINT32 HtcLock:1;                  ///< HTC lock
} HTC_REGISTER;


/* Power Control Miscellaneous Register D18F3xA0 */
#define PW_CTL_MISC_REG 0xA0
#define PW_CTL_MISC_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, PW_CTL_MISC_REG))

/// Power Control Miscellaneous PCI Register
typedef struct {
  UINT32 PsiVid:7;                   ///< PSI_L VID threshold
  UINT32 PsiVidEn:1;                 ///< PSI_L VID enable
  UINT32 :1;                         ///< Reserved
  UINT32 SviHighFreqSel:1;           ///< SVI high frequency select
  UINT32 :6;                         ///< Reserved
  UINT32 ConfigId:12;                ///< Configuration Identifier
  UINT32 :3;                         ///< Reserved
  UINT32 CofVidProg:1;               ///< COF and VID of P-states programmed
} POWER_CTRL_MISC_REGISTER;


/* Clock Power/Timing Control 0 Register D18F3xD4 */
#define CPTC0_REG 0xD4
#define CPTC0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC0_REG))

/// Clock Power Timing Control PCI Register
typedef struct {
  UINT32 MainPllOpFreqId:6;          ///< Main PLL Fid
  UINT32 MainPllOpFreqIdEn:1;        ///< Main PLL Fid Enable
  UINT32 :1;                         ///< Reserved
  UINT32 ClkRampHystSel:4;           ///< Clock Ramp Hysteresis Select
  UINT32 OnionOutHyst:4;             ///< ONION outbound hysteresis
  UINT32 DisNclkGatingIdle:1;        ///< Disable NCLK gating when idle
  UINT32 ClkGatingEnDram:1;          ///< Clock gating enable DRAM
  UINT32 :1;                         ///< Reserved
  UINT32 PstateSpecFuseSel:8;        ///< P-State Specification Fuse Select
  UINT32 :5;                         ///< Reserved
} CLK_PWR_TIMING_CTRL_REGISTER;


/* Clock Power/Timing Control 1 Register D18F3xD8 */
#define CPTC1_REG 0xD8
#define CPTC1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC1_REG))

/// Clock Power Timing Control 1 PCI Register
typedef struct {
  UINT32 :4;                         ///< Reserved
  UINT32 VSRampSlamTime:3;           ///< Voltage stabilization slam time
  UINT32 :22;                        ///< Reserved
  UINT32 SlamModeSelect:1;           ///< Voltage slam mode select
  UINT32 :2;                         ///< Reserved
} CLK_PWR_TIMING_CTRL1_REGISTER;


/* Clock Power/Timing Control 2 Register D18F3xDC */
#define CPTC2_REG 0xDC
#define CPTC2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC2_REG))

/// Clock Power Timing Control 2 PCI Register
typedef struct {
  UINT32 :8;                         ///< Reserved
  UINT32 PstateMaxVal:3;             ///< P-state maximum value
  UINT32 :1;                         ///< Reserved
  UINT32 NbPs0Vid:7;                 ///< NB VID
  UINT32 NclkFreqDone:1;             ///< NCLK frequency change done
  UINT32 NbPs0NclkDiv:7;             ///< NCLK divisor
  UINT32 NbClockGateHyst:3;          ///< Northbridge clock gating hysteresis
  UINT32 NbClockGateEn:1;            ///< Northbridge clock gating enable
  UINT32 CnbCifClockGateEn:1;        ///< CNB CIF clock gating enable
} CLK_PWR_TIMING_CTRL2_REGISTER;


/* Northbridge Capabilities Register D18F3xE8 */
#define NB_CAPS_REG 0xE8
#define NB_CAPS_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, NB_CAPS_REG))

/// Northbridge Capabilities PCI Register
typedef struct {
  UINT32 DctDualCap:1;               ///< Two-channel DRAM capable
  UINT32 :4;                         ///< Reserved
  UINT32 DdrMaxRate:3;               ///< Maximum DRAM data rate
  UINT32 MctCap:1;                   ///< Memory controller capable
  UINT32 SvmCapable:1;               ///< SVM capable
  UINT32 HtcCapable:1;               ///< HTC capable
  UINT32 :1;                         ///< Reserved
  UINT32 CmpCap:2;                   ///< CMP capable
  UINT32 :14;                        ///< Reserved
  UINT32 LHtcCapable:1;              ///< LHTC capable
  UINT32 :3;                         ///< Reserved
} NB_CAPS_REGISTER;


/* Clock Power/Timing Control 3 Register D18F3x128 */
#define CPTC3_REG 0x128
#define CPTC3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC3_REG))

/// Clock Power Timing Control 3 PCI Register
typedef struct {
  UINT32 C6Vid:7;                    ///< C6 VID
  UINT32 :1;                         ///< Reserved
  UINT32 NbPsiVid:7;                 ///< NB PSI_L VID threshold
  UINT32 NbPsiVidEn:1;               ///< NB PSI_L enable
  UINT32 :16;                        ///< Reserved
} CLK_PWR_TIMING_CTRL3_REGISTER;

/* Local hardware thermal control register D18F3x138 */
#define LHTC_REG 0x138
#define LHTC_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, LHTC_REG))

/// Local Hardware Thermal Control PCI Register
typedef struct {
  UINT32 LHtcEn:1;                   ///< Local HTC Enable
  UINT32 :7;                         ///< Reserved
  UINT32 LHtcAct:2;                  ///< Local HTC Active State
  UINT32 :2;                         ///< Reserved
  UINT32 LHtcActSts:2;               ///< Local HTC Active Status
  UINT32 :2;                         ///< Reserved
  UINT32 LHtcTmpLmt:7;               ///< Local HTC temperature limit
  UINT32 LHtcSlewSel:1;              ///< Local HTC slew-controlled temp select
  UINT32 LHtcHystLmt:4;              ///< Local HTC hysteresis
  UINT32 LHtcPstateLimit:3;          ///< Local HTC P-state limit select
  UINT32 LHtcLock:1;                 ///< HTC lock
} LHTC_REGISTER;

/* Product Information Register D18F3x1FC */
#define PRODUCT_INFO_REG 0x1FC
#define PRODUCT_INFO_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, PRODUCT_INFO_REG))

/// Product Information PCI Register
typedef struct {
  UINT32 :2;                         ///< Reserved
  UINT32 LowPowerDefault:1;          ///< Low Power Default
  UINT32 :29;                        ///< Reserved
} PRODUCT_INFO_REGISTER;


/* C-state Control 1 Register D18F4x118 */
#define CSTATE_CTRL1_REG 0x118
#define CSTATE_CTRL1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_CTRL1_REG))

/// C-state Control 1 Register
typedef struct {
  UINT32 CstAct0:3;                  ///< C-state action field 0
  UINT32 :5;                         ///< Reserved
  UINT32 CstAct1:3;                  ///< C-state action field 1
  UINT32 :5;                         ///< Reserved
  UINT32 CstAct2:3;                  ///< C-state action field 2
  UINT32 :5;                         ///< Reserved
  UINT32 CstAct3:3;                  ///< C-state action field 3
  UINT32 :5;                         ///< Reserved
} CSTATE_CTRL1_REGISTER;


/* C-state Control 2 Register D18F4x11C */
#define CSTATE_CTRL2_REG 0x11C
#define CSTATE_CTRL2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_CTRL2_REG))

/// C-state Control 2 Register
typedef struct {
  UINT32 CstAct4:3;                  ///< C-state action field 4
  UINT32 :5;                         ///< Reserved
  UINT32 CstAct5:3;                  ///< C-state action field 5
  UINT32 :5;                         ///< Reserved
  UINT32 CstAct6:3;                  ///< C-state action field 6
  UINT32 :5;                         ///< Reserved
  UINT32 CstAct7:3;                  ///< C-state action field 7
  UINT32 :5;                         ///< Reserved
} CSTATE_CTRL2_REGISTER;


/* C-state Monitor Control 3 Register D18F4x134 */
#define CSTATE_MON_CTRL3_REG 0x134
#define CSTATE_MON_CTRL3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CSTATE_MON_CTRL3_REG))

/// C-state Monitor Control 3 Register
typedef struct {
  UINT32 IntRatePkgC6MaxDepth:4;     ///< Interrupt rate monitor PC6 maximum counter depth
  UINT32 IntRatePkgC6Threshold:4;    ///< Interrupt rate monitor PC6 threshold
  UINT32 IntRatePkgC6BurstLen:3;     ///< Interrupt rate monitor PC6 burst length
  UINT32 IntRatePkgC6DecrRate:5;     ///< Interrupt rate monitor PC6 decrement rate
  UINT32 IntRateCC6MaxDepth:4;       ///< Interrupt rate monitor CC6 maximum counter depth
  UINT32 IntRateCC6Threshold:4;      ///< Interrupt rate monitor CC6 threshold
  UINT32 IntRateCC6BurstLen:3;       ///< Interrupt rate monitor CC6 burst length
  UINT32 IntRateCC6DecrRate:5;       ///< Interrupt rate monitor CC6 decrement rate
} CSTATE_MON_CTRL3_REGISTER;

/* LPMV Scalar 2 Register D18F4x14C */
#define LPMV_SCALAR2_REG 0x14C
#define LPMV_SCALAR2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, LPMV_SCALAR2_REG))

/// LPMV Scalar 2 Register
typedef struct {
  UINT32 :24;                        ///< Reserved
  UINT32 ApmCstExtPol:2;             ///< Number of boosted states
  UINT32 :6;                         ///< Reserved
} LPMV_SCALAR2_REGISTER;

/* Core Performance Boost Control Register D18F4x15C */
#define CPB_CTRL_REG 0x15C
#define CPB_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CPB_CTRL_REG))

/// Core Performance Boost Control Register
typedef struct {
  UINT32 BoostSrc:2;                 ///< Boost source
  UINT32 NumBoostStates:3;           ///< Number of boosted states
  UINT32 :24;                        ///< Reserved
  UINT32 BoostEnAllCores:1;          ///< Boost enable all cores
  UINT32 :2;                         ///< Reserved
} CPB_CTRL_REGISTER;


/* CPU State Power Management Dynamic Control 0 Register D18F4x1A8 */
#define CPU_STATE_PM_CTRL0_REG 0x1A8
#define CPU_STATE_PM_CTRL0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CPU_STATE_PM_CTRL0_REG))

/// CPU State Power Management Dynamic Control 0 Register
typedef struct {
  UINT32 SingleHaltCpuDid:5;         ///< Single hlt CPU DID
  UINT32 AllHaltCpuDid:5;            ///< All hlt CPU DID
  UINT32 :5;                         ///< Reserved
  UINT32 CpuProbEn:1;                ///< CPU probe enable
  UINT32 :1;                         ///< Reserved
  UINT32 PService:3;                 ///< Service P-state
  UINT32 PServiceTmr:3;              ///< Service P-state timer
  UINT32 PServiceTmrEn:1;            ///< Service P-state timer enable
  UINT32 DramSrEn:1;                 ///< DRAM self-refresh enable
  UINT32 MemTriStateEn:1;            ///< Memory clock tri-state enable
  UINT32 DramSrHyst:3;               ///< DRAM self-refresh hysteresis time
  UINT32 DramSrHystEnable:1;         ///< DRAM self-refresh hysteresis enable
  UINT32 :2;                         ///< Reserved
} CPU_STATE_PM_CTRL0_REGISTER;


/* CPU State Power Management Dynamic Control 1 Register D18F4x1AC */
#define CPU_STATE_PM_CTRL1_REG 0x1AC
#define CPU_STATE_PM_CTRL1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CPU_STATE_PM_CTRL1_REG))

/// CPU State Power Management Dynamic Control 1 Register
typedef struct {
  UINT32 :5;                         ///< Reserved
  UINT32 C6Did:5;                    ///< CC6 divisor
  UINT32 :6;                         ///< Reserved
  UINT32 PstateIdCoreOffExit:3;      ///< P-state ID core-off exit
  UINT32 :7;                         ///< Reserved
  UINT32 PkgC6Cap:1;                 ///< Package C6 capable
  UINT32 CoreC6Cap:1;                ///< Core C6 capable
  UINT32 PkgC6Dis:1;                 ///< Package C6 disable
  UINT32 CoreC6Dis:1;                ///< Core C6 disable
  UINT32 CstPminEn:1;                ///< C-state Pmin enable
  UINT32 :1;                         ///< Reserved
} CPU_STATE_PM_CTRL1_REGISTER;


/* C6 Base Register D18F4x12C */
#define C6_BASE_REG 0x12C
#define C6_BASE_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, C6_BASE_REG))

/// C6 Base Register
typedef struct {
  UINT32 C6Base:12;                  ///< C6 base[35:24]
  UINT32 :20;                        ///< Reserved
} C6_BASE_REGISTER;


/* NB P-state Config Low Register D18F6x90 */
#define NB_PSTATE_CFG_LOW_REG 0x90
#define NB_PSTATE_CFG_LOW_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_6, NB_PSTATE_CFG_LOW_REG))

/// NB P-state Config Low Register
typedef struct {
  UINT32 NbPs1NclkDiv:7;             ///< NBP1 NCLK divisor
  UINT32 :1;                         ///< Reserved
  UINT32 NbPs1Vid:7;                 ///< NBP1 NCLK VID
  UINT32 :1;                         ///< Reserved
  UINT32 NbPs1GnbSlowIgn:1;          ///< NB P-state ignore GNB slow signal
  UINT32 :3;                         ///< Reserved
  UINT32 NbPsLock:1;                 ///< NB P-state lock
  UINT32 :7;                         ///< Reserved
  UINT32 NbPsForceReq:1;             ///< NB P-state force request
  UINT32 NbPsForceSel:1;             ///< NB P-state force selection
  UINT32 NbPsCtrlDis:1;              ///< NB P-state control disable
  UINT32 NbPsCap:1;                  ///< NB P-state capable
} NB_PSTATE_CFG_LOW_REGISTER;


/* NB P-state Config High Register D18F6x94 */
#define NB_PSTATE_CFG_HIGH_REG 0x94
#define NB_PSTATE_CFG_HIGH_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_6, NB_PSTATE_CFG_HIGH_REG))

/// NB P-state Config High Register
typedef struct {
  UINT32 CpuPstateThr:3;             ///< CPU P-state threshold
  UINT32 CpuPstateThrEn:1;           ///< CPU P-state threshold enable
  UINT32 NbPs1NoTransOnDma:1;        ///< NB P-state no transitions on DMA
  UINT32 :15;                        ///< Reserved
  UINT32 NbPsNonC0Timer:3;           ///< NB P-state non-C0 timer
  UINT32 NbPsC0Timer:3;              ///< NB P-state C0 timer
  UINT32 NbPs1ResTmrMin:3;           ///< NBP1 minimum residency timer
  UINT32 NbPs0ResTmrMin:3;           ///< NBP0 minimum residency timer
} NB_PSTATE_CFG_HIGH_REGISTER;


/* NB P-state Control and Status Register D18F6x98 */
#define NB_PSTATE_CTRL_STS_REG 0x98
#define NB_PSTATE_CTRL_STS_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_6, NB_PSTATE_CTRL_STS_REG))

/// NB P-state Control and Status Register
typedef struct {
  UINT32 NbPsTransInFlight:1;        ///< NB P-state transition in flight
  UINT32 NbPs1ActSts:1;              ///< NB P-state 1 active status
  UINT32 NbPs1Act:1;                 ///< NB P-state 1 active
  UINT32 :27;                        ///< Reserved
  UINT32 NbPsCsrAccSel:1;            ///< NB P-state register accessibility select
  UINT32 NbPsDbgEn:1;                ///< NB P-state debug enable
} NB_PSTATE_CTRL_STS_REGISTER;

/* NCLK Reduction Control D18F6x9C */
#define NCLK_REDUCTION_CTRL_REG 0x9C
#define NCLK_REDUCTION_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_6, NCLK_REDUCTION_CTRL_REG))

/// NCLK Reduction Control
typedef struct {
  UINT32 NclkRedDiv:7;              ///< NCLK reduction divisor
  UINT32 NclkRedSelfRefrAlways:1;   ///< NCLK reduction always self refresh
  UINT32 NclkRampWithDllRelock:1;   ///< NCLK ramp mode
  UINT32 :23;                       ///< Reserved
} NCLK_REDUCTION_CTRL_REGISTER;

/// enum for DSM workaround control
typedef enum {
  CC6_DSM_WORK_AROUND_DISABLE = 0, ///< work around disable
  CC6_DSM_WORK_AROUND_NORMAL_TRAFFIC, ///< work around With Normal Traffic
  CC6_DSM_WORK_AROUND_HIGH_PRIORITY_CHANNEL, ///< work around With High Priority Channel
} CC6_DSM_WORK_AROUND;

#endif /* _CPUF14POWERMGMT_H */
