/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Power Management related registers defination
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 52710 $   @e \$Date: 2011-05-10 15:58:53 -0600 (Tue, 10 May 2011) $
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

#ifndef _CPUF15POWERMGMT_H_
#define _CPUF15POWERMGMT_H_

/*
 * Family 15h CPU Power Management MSR definitions
 *
 */


/* Last Branch From IP Register 0x000001DB */
#define MSR_BR_FROM                 0x000001DB

/* P-state Current Limit Register 0xC0010061 */
#define MSR_PSTATE_CURRENT_LIMIT 0xC0010061  // F15 Shared

/// Pstate Current Limit MSR Register
typedef struct {
  UINT64 CurPstateLimit:3;           ///< Current Pstate Limit
  UINT64 :1;                         ///< Reserved
  UINT64 PstateMaxVal:3;             ///< Pstate Max Value
  UINT64 :57;                        ///< Reserved
} PSTATE_CURLIM_MSR;


/* P-state Control Register 0xC0010062 */
#define MSR_PSTATE_CTL 0xC0010062    // F15 Shared

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

#define PS_REG_BASE MSR_PSTATE_0     /* P-state Register base */
#define PS_MAX_REG  MSR_PSTATE_7     /* Maximum P-State Register */
#define PS_MIN_REG  MSR_PSTATE_0     /* Minimum P-State Register */
#define NM_PS_REG 8                  /* number of P-state MSR registers */

/// P-state MSR with common field
typedef struct {
  UINT64 :63;                        ///< CpuFid
  UINT64 PsEnable:1;                 ///< Pstate Enable
} F15_PSTATE_MSR;


/* C-state Address Register 0xC0010073 */
#define MSR_CSTATE_ADDRESS 0xC0010073

/// C-state Address MSR Register
typedef struct {
  UINT64 CstateAddr:16;              ///< C-state address
  UINT64 :48;                        ///< Reserved
} CSTATE_ADDRESS_MSR;


/*
 * Family 15h CPU Power Management PCI definitions
 *
 */

/* Extended Memory Controller Configuration Low Register F2x1B0 */
#define EXT_MEMCTRL_CFG_LOW_REG 0x1B0

/// Extended Memory Controller Configuration Low PCI Register
typedef struct {
  UINT32 AdapPrefMissRatio:2;        ///< Adaptive prefetch miss ratio
  UINT32 AdapPrefPositiveStep:2;     ///< Adaptive prefetch positive step
  UINT32 AdapPrefNegativeStep:2;     ///< Adaptive prefetch negative step
  UINT32 :2;                         ///< Reserved
  UINT32 CohPrefPrbLmt:3;            ///< Coherent prefetch probe limit
  UINT32 DisIoCohPref:1;             ///< Disable coherent prefetched for IO
  UINT32 EnSplitDctLimits:1;         ///< Split DCT write limits enable
  UINT32 SpecPrefDis:1;              ///< Speculative prefetch disable
  UINT32 SpecPrefMis:1;              ///< Speculative prefetch predict miss
  UINT32 SpecPrefThreshold:3;        ///< Speculative prefetch threshold
  UINT32 :4;                         ///< Reserved
  UINT32 PrefFourConf:3;             ///< Prefetch four-ahead confidence
  UINT32 PrefFiveConf:3;             ///< Prefetch five-ahead confidence
  UINT32 DcqBwThrotWm:4;             ///< Dcq bandwidth throttle watermark
} EXT_MEMCTRL_CFG_LOW_REGISTER;


/* Hardware thermal control register F3x64 */
#define HTC_REG 0x64

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
  UINT32 :1;                         ///< Reserved
} HTC_REGISTER;


/* Software P-state limit register F3x68 */
#define SW_PS_LIMIT_REG 0x68

/// Software P-state Limit PCI Register
typedef struct {
  UINT32 :5;                         ///< Reserved
  UINT32 SwPstateLimitEn:1;          ///< Software P-state limit enable
  UINT32 :22;                        ///< Reserved
  UINT32 SwPstateLimit:3;            ///< HTC P-state limit select
  UINT32 :1;                         ///< Reserved
} SW_PS_LIMIT_REGISTER;

/* ACPI Power State Control Registers F3x84:80 */

/// System Management Action Field (SMAF) Register
typedef struct {
  UINT8 CpuPrbEn:1;                  ///< CPU direct probe enable
  UINT8 NbLowPwrEn:1;                ///< Northbridge low-power enable
  UINT8 NbGateEn:1;                  ///< Northbridge gate enable
  UINT8 Reserved:2;                  ///< Reserved
  UINT8 ClkDivisor:3;                ///< Clock divisor
} SMAF_REGISTER;

/// union type for ACPI State SMAF setting
typedef union {
  UINT8 SMAFValue;                   ///< SMAF raw value
  SMAF_REGISTER SMAF;                ///< SMAF structure
} ACPI_STATE_SMAF;

/// ACPI Power State Control Register F3x80
typedef struct {
  ACPI_STATE_SMAF C2;                ///< [7:0]   SMAF Code 000b - C2
  ACPI_STATE_SMAF C1eLinkInit;       ///< [15:8]  SMAF Code 001b - C1e or Link init
  ACPI_STATE_SMAF SmafAct2;          ///< [23:16] SMAF Code 010b
  ACPI_STATE_SMAF S1;                ///< [31:24] SMAF Code 011b - S1
} ACPI_PSC_0_REGISTER;

/// ACPI Power State Control Register F3x84
typedef struct {
  ACPI_STATE_SMAF S3;                 ///< [7:0]   SMAF Code 100b - S3
  ACPI_STATE_SMAF Throttling;         ///< [15:8]  SMAF Code 101b - Throttling
  ACPI_STATE_SMAF S4S5;               ///< [23:16] SMAF Code 110b - S4/S5
  ACPI_STATE_SMAF C1;                 ///< [31:24] SMAF Code 111b - C1
} ACPI_PSC_4_REGISTER;


/* Popup P-state Register F3xA8 */
#define POPUP_PSTATE_REG 0xA8
#define POPUP_PSTATE_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, POPUP_PSTATE_REG))

/// Popup P-state Register
typedef struct {
  UINT32 :29;                        ///< Reserved
  UINT32 PopDownPstate:3;            ///< PopDownPstate
} POPUP_PSTATE_REGISTER;


/* Clock Power/Timing Control 2 Register F3xDC */
#define CPTC2_REG 0xDC
#define CPTC2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_3, CPTC2_REG))

/// Clock Power Timing Control 2 PCI Register
typedef struct {
  UINT32 :8;                         ///< Reserved
  UINT32 PstateMaxVal:3;             ///< P-state maximum value
  UINT32 :1;                         ///< Reserved
  UINT32 NbsynPtrAdj:3;              ///< NB/Core sync FIFO ptr adjust
  UINT32 :1;                         ///< Reserved
  UINT32 CacheFlushOnHaltCtl:3;      ///< Cache flush on halt control
  UINT32 CacheFlushOnHaltTmr:7;      ///< Cache flush on halt timer
  UINT32 IgnCpuPrbEn:1;              ///< ignore CPU probe enable
  UINT32 :5;                         ///< Reserved
} CLK_PWR_TIMING_CTRL2_REGISTER;


/* Core Performance Boost Control Register D18F4x15C */
#define CPB_CTRL_REG 0x15C
#define CPB_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_4, CPB_CTRL_REG))

/// Core Performance Boost Control Register of Family 15h common aceess
typedef struct {
  UINT32 BoostSrc:2;                 ///< Boost source
  UINT32 NumBoostStates:3;           ///< Number of boosted states
  UINT32 :2;                         ///< Reserved
  UINT32 ApmMasterEn:1;              ///< APM master enable
  UINT32 :23;                        ///< Reserved
  UINT32 BoostLock:1;                ///<
} F15_CPB_CTRL_REGISTER;


#define NM_NB_PS_REG 4               /* Number of NB P-state registers */

/* Northbridge P-state */
#define NB_PSTATE_0 0x160
#define NB_PSTATE_0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_0))

#define NB_PSTATE_1 0x164
#define NB_PSTATE_1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_1))

#define NB_PSTATE_2 0x168
#define NB_PSTATE_2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_2))

#define NB_PSTATE_3 0x16C
#define NB_PSTATE_3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, NB_PSTATE_3))


/* Northbridge P-state Status */
#define F15_NB_PSTATE_CTRL 0x170
#define F15_NB_PSTATE_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FUNC_5, F15_NB_PSTATE_CTRL))

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
} F15_NB_PSTATE_CTRL_REGISTER;


#endif /* _CPUF15POWERMGMT_H */
