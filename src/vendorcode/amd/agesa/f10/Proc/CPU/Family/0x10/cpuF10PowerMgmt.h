/**
 * @file
 *
 * AMD Family_10 Power Management related stuff
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPUF10POWERMGMT_H_
#define _CPUF10POWERMGMT_H_

/*
 * Family 10h CPU Power Management MSR definitions
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
  UINT64 SmiOnCmpHalt:1;             ///< SMI on chip multi-processing halt
  UINT64 C1eOnCmpHalt:1;             ///< C1E on chip multi-processing halt
  UINT64 BmStsClrOnHltEn:1;          ///< Clear BM status bit on server C1e entry
  UINT64 :34;                        ///< Reserved
} INTPEND_MSR;


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


/* P-state Registers 0xC001006[8:4] */
#define MSR_PSTATE_0 0xC0010064
#define MSR_PSTATE_1 0xC0010065
#define MSR_PSTATE_2 0xC0010066
#define MSR_PSTATE_3 0xC0010067
#define MSR_PSTATE_4 0xC0010068

#define PS_REG_BASE MSR_PSTATE_0     /* P-state Register base */
#define PS_MAX_REG  MSR_PSTATE_4     /* Maximum P-State Register */
#define PS_MIN_REG  MSR_PSTATE_0     /* Minimum P-State Register */
#define NM_PS_REG 5                  /* number of P-state MSR registers */

/// Pstate MSR
typedef struct {
  UINT64 CpuFid:6;                   ///< CpuFid
  UINT64 CpuDid:3;                   ///< CpuDid
  UINT64 CpuVid:7;                   ///< CpuVid
  UINT64 :6;                         ///< Reserved
  UINT64 NbDid:1;                    ///< NbDid
  UINT64 :2;                         ///< Reserved
  UINT64 NbVid:7;                    ///< NbVid
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
  UINT64 NbDid:1;                    ///< NbDid
  UINT64 :2;                         ///< Reserved
  UINT64 NbVid:7;                    ///< NbVid
  UINT64 :32;                        ///< Reserved
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
  UINT64 MaxNbFid:5;                 ///< MaxNbFid
} COFVID_STS_MSR;


/*
 * Family 10h CPU Power Management PCI definitions
 *
 */

/* DRAM Configuration High Register F2x[1,0]94 */
#define DRAM_CFG_HI_REG0 0x94
#define DRAM_CFG_HI_REG1 0x194

/// DRAM Configuration High PCI Register
typedef struct {
  UINT32 MemClkFreq:3;               ///< Memory clock frequency
  UINT32 MemClkFreqVal:1;            ///< Memory clock frequency valid
  UINT32 :4;                         ///< Reserved
  UINT32 Ddr3Mode:1;                 ///< DDR3 mode
  UINT32 LegacyBiosMode:1;           ///< Legacy BIOS mode
  UINT32 ZqcsInterval:2;             ///< ZQ calibration short interval
  UINT32 RDqsEn:1;                   ///< Read DQS enable
  UINT32 DisSimulRdWr:1;             ///< Disable simultaneous read and write
  UINT32 DisDramInterface:1;         ///< Disable the DRAM interface
  UINT32 PowerDownEn:1;              ///< Power down mode enable
  UINT32 PowerDownMode:1;            ///< Power down mode
  UINT32 :1;                         ///< Reserved
  UINT32 FourRankRDimm:1;            ///< Four rank registered DIMM connected
  UINT32 DcqArbBypassEn:1;           ///< DRAM controller arbiter bypass enable
  UINT32 SlowAccessMode:1;           ///< Slow access mode
  UINT32 FreqChgInProg:1;            ///< Frequency change in progress
  UINT32 BankSwizzleMode:1;          ///< Bank swizzle mode
  UINT32 ProcOdtDis:1;               ///< Processor on-die termination disable
  UINT32 DcqBypassMax:4;             ///< DRAM controller queue bypass maximum
  UINT32 FourActWindow:4;            ///< Four bank activate window
} DRAM_CFG_HI_REGISTER;


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


/* Scrub Rate Control Register F3x58 */
#define SCRUB_RATE_CTRL_REG 0x58

/// Scrub Rate Control PCI Register
typedef struct {
  UINT32 DramScrub:5;                ///< DRAM scrub rate
  UINT32 :3;                         ///< Reserved
  UINT32 L2Scrub:5;                  ///< L2 cache scrub rate
  UINT32 :3;                         ///< Reserved
  UINT32 DcacheScrub:5;              ///< Data cache scrub rate
  UINT32 :3;                         ///< Reserved
  UINT32 L3Scrub:5;                  ///< L3 cache scrub rate
  UINT32 :3;                         ///< Reserved
} SCRUB_RATE_CTRL_REGISTER;

/* DRAM Scrub Address Low Register F3x5C */
#define DRAM_SCRUB_ADDR_LOW_REG 0x5C

/// DRAM Scrub Address Low PCI Register
typedef struct {
  UINT32 ScrubReDirEn:1;             ///< DRAM scrubber redirect enable
  UINT32 :5;                         ///< Reserved
  UINT32 ScrubAddrLo:26;             ///< DRAM scrubber address bits[31:6]
} DRAM_SCRUB_ADDR_LOW_REGISTER;


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


/* Software thermal control register F3x68 */
#define STC_REG 0x68

/// Software Thermal Control PCI Register
typedef struct {
  UINT32 StcSbcTmpHiEn:1;            ///< STC SBC temperature high enable
  UINT32 StcSbcTmpLoEn:1;            ///< STC SBC temperature low enable
  UINT32 StcApcTmpHiEn:1;            ///< STC APIC temperature high enable
  UINT32 StcApcTmpLoEn:1;            ///< STC APIC temperature low enable
  UINT32 :1;                         ///< Reserved
  UINT32 StcPstateEn:1;              ///< STC P-state enable
  UINT32 StcTmpHiSts:1;              ///< STC temperature high status
  UINT32 StcTmpLoSts:1;              ///< STC temperature low status
  UINT32 :8;                         ///< Reserved
  UINT32 StcTmpLmt:7;                ///< STC temperature limit
  UINT32 StcSlewSel:1;               ///< STC slew-controlled temp select
  UINT32 StcHystLmt:4;               ///< STC hysteresis
  UINT32 StcPstateLimit:3;           ///< STC P-state limit select
  UINT32 :1;                         ///< Reserved
} STC_REGISTER;

/* ACPI Power State Control Registers F3x84:80 */

/// System Management Action Field (SMAF) Register
typedef struct {
  UINT8 CpuPrbEn:1;                  ///< CPU direct probe enable
  UINT8 NbLowPwrEn:1;                ///< Northbridge low-power enable
  UINT8 NbGateEn:1;                  ///< Northbridge gate enable
  UINT8 :1;                          ///< Reserved
  UINT8 AltVidEn:1;                  ///< alternate VID enable
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
  ACPI_STATE_SMAF C3C1eLinkInit;     ///< [15:8]  SMAF Code 001b - C3, C1e or Link init
  ACPI_STATE_SMAF FidVidChg;         ///< [23:16] SMAF Code 010b - FIDVID Change
  ACPI_STATE_SMAF S1;                ///< [31:24] SMAF Code 011b - S1
} ACPI_PSC_0_REGISTER;

/// ACPI Power State Control Register F3x84
typedef struct {
  ACPI_STATE_SMAF S3;                 ///< [7:0]   SMAF Code 100b - S3
  ACPI_STATE_SMAF Throttling;         ///< [15:8]  SMAF Code 101b - Throttling
  ACPI_STATE_SMAF S4S5;               ///< [23:16] SMAF Code 110b - S4/S5
  ACPI_STATE_SMAF C1;                 ///< [31:24] SMAF Code 111b - C1
} ACPI_PSC_4_REGISTER;


/* Power Control Miscellaneous Register F3xA0 */
#define PW_CTL_MISC_REG 0xA0

/// Power Control Miscellaneous PCI Register
typedef struct {
  UINT32 PsiVid:7;                   ///< PSI_L VID threshold
  UINT32 PsiVidEn:1;                 ///< PSI_L VID enable
  UINT32 PviMode:1;                  ///< Parallel VID interface mode
  UINT32 SviHighFreqSel:1;           ///< SVI high frequency select
  UINT32 IdleExitEn:1;               ///< IDLEEXIT_L Enable
  UINT32 PllLockTime:3;              ///< PLL synchronization lock time
  UINT32 BpPinsTriEn:1;              ///< Breakpoint pins tristate enable
  UINT32 :1;                         ///< Reserved
  UINT32 PstateId:12;                ///< Pstate ID
  UINT32 :1;                         ///< Reserved
  UINT32 SlamVidMode:1;              ///< Slam voltage ID mode
  UINT32 :1;                         ///< Reserved
  UINT32 CofVidProg:1;               ///< COF and VID of Pstate programmed
} POWER_CTRL_MISC_REGISTER;

/* Popup P-state Register F3xA8 */
#define POPUP_PSTATE_REG 0xA8

/// Popup P-state Register
typedef struct {
  UINT32 PopupEn:1;                  ///< Popup enable
  UINT32 :1;                         ///< Reserved
  UINT32 PopupPstate:3;              ///< Popup P-state
  UINT32 PopupCpuVid:7;              ///< Popup core VID
  UINT32 PopupCpuFid:6;              ///< Popup core FID
  UINT32 PopupCpuDid:3;              ///< Popup core DID
  UINT32 :11;                        ///< Reserved
} POPUP_PSTATE_REGISTER;

/* Clock Power/Timing Control 0 Register F3xD4 */
#define CPTC0_REG 0xD4

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

/// Clock Power Timing Control 1 PCI Register
typedef struct {
  UINT32 VSSlamTime:3;               ///< Voltage stabilization slam time
  UINT32 :1;                         ///< Reserved
  UINT32 VSRampTime:3;               ///< Voltage stabilization ramp time
  UINT32 :1;                         ///< Reserved
  UINT32 TdpVid:7;                   ///< Thermal design power VID
  UINT32 :1;                         ///< Reserved
  UINT32 AltVidStart:7;              ///< Alternate VID start limit
  UINT32 :1;                         ///< Reserved
  UINT32 ReConDel:4;                 ///< Link reconnect delay
  UINT32 PwrPlanes:1;                ///< Power planes
  UINT32 :3;                         ///< Reserved
} CLK_PWR_TIMING_CTRL1_REGISTER;


/* Clock Power/Timing Control 2 Register F3xDC */
#define CPTC2_REG 0xDC

/// Clock Power Timing Control 2 PCI Register
typedef struct {
  UINT32 AltVid:7;                   ///< Alternate VID
  UINT32 :1;                         ///< Reserved
  UINT32 PstateMaxVal:3;             ///< P-state maximum value
  UINT32 :1;                         ///< Reserved
  UINT32 NbsynPtrAdj:3;              ///< NB/Core sync FIFO ptr adjust
  UINT32 :1;                         ///< Reserved
  UINT32 CacheFlushOnHaltCtl:3;      ///< Cache flush on halt control
  UINT32 CacheFlushOnHaltTmr:7;      ///< Cache flush on halt timer
  UINT32 :1;                         ///< Reserved
  UINT32 SlamTimeMode:2;             ///< Slam time mode
  UINT32 AltvidVSSlamTime:3;         ///< Altvid voltage stabilization slam time
} CLK_PWR_TIMING_CTRL2_REGISTER;


/* Northbridge Capabilities Register F3xE8 */
#define NB_CAPS_REG 0xE8

/// Northbridge Capabilities PCI Register
typedef struct {
  UINT32 DctDualCap:1;               ///< Two-channel DRAM capable
  UINT32 DualNodeCap:1;              ///< Dual-node multi-processor capable
  UINT32 EightNodeCap:1;             ///< Eight-node multi-processor capable
  UINT32 EccCapable:1;               ///< ECC capable
  UINT32 ChipkillCapable:1;          ///< Chipkill ECC capable
  UINT32 DdrMaxRate:3;               ///< Maximum DRAM data rate
  UINT32 MctCap:1;                   ///< Memory controller capable
  UINT32 SvmCapable:1;               ///< SVM capable
  UINT32 HtcCapable:1;               ///< HTC capable
  UINT32 LnkRtryCap:1;               ///< Link error-retry capable
  UINT32 CmpCapLo:2;                 ///< CMP capable[1:0]
  UINT32 MultiVidPlaneCap:1;         ///< Multiple VID plane capable
  UINT32 CmpCapHi:1;                 ///< CMP capable[2]
  UINT32 MpCap:3;                    ///< MP capability
  UINT32 :1;                         ///< Reserved
  UINT32 UnGangEn:4;                 ///< Link unganging enabled
  UINT32 :1;                         ///< Reserved
  UINT32 L3Capable:1;                ///< L3 capable
  UINT32 HtAcCapable:1;              ///< HT AC capable
  UINT32 :2;                         ///< Reserved
  UINT32 MultiNodeCpu:1;             ///< Multinode processor
  UINT32 IntNodeNum:2;               ///< Internal node number
} NB_CAPS_REGISTER;


/* NB Extended Configuration Low Register F3x188 */
#define NB_EXT_CFG_LO_REG 0x188

/// Northbridge Extended Configuration Low PCI Register
typedef struct {
  UINT32 :4;                         ///< Reserved
  UINT32 EnStpGntOnFlushMaskWakeup:1; ///< Enable stop grant on flush mask wakeup
  UINT32 :27;                        ///< Reserved
} NB_EXT_CFG_LO_REGISTER;


/* L3 Cache Parameter Register F3x1C4 */
#define L3_CACHE_PARAM_REG 0x1C4

/// L3 Cache Parameter PCI Register
typedef struct {
  UINT32 L3SubcacheSize0:1;          ///< L3 subcache size 0
  UINT32 :3;                         ///< Reserved
  UINT32 L3SubcacheSize1:1;          ///< L3 subcache size 1
  UINT32 :3;                         ///< Reserved
  UINT32 L3SubcacheSize2:2;          ///< L3 subcache size 2
  UINT32 :2;                         ///< Reserved
  UINT32 L3SubcacheSize3:2;          ///< L3 subcache size 3
  UINT32 :17;                        ///< Reserved
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
  UINT32 :3;                         ///< Reserved
  UINT32 PFInitDone:1;               ///< Probe filter initialization done
  UINT32 PFPreferredSORepl:2;        ///< PF preferredSO replacement mode
  UINT32 PFErrInt:2;                 ///< Probe filter error interrupt type
  UINT32 PFErrIntLvtOff:4;           ///< Probe filter error interrupt LVT offset
  UINT32 PFEccError:1;               ///< Probe filter ECC error
  UINT32 PFLoIndexHashEn:1;          ///< Probe filter low index hash enable
  UINT32 :2;                         ///< Reserved
} PROBE_FILTER_CTRL_REGISTER;


/* Product Info Register F3x1FC */
#define PRCT_INFO_REG 0x1FC

/// Product Information PCI Register
typedef struct {
  UINT32 NbCofVidUpdate:1;           ///< NbCofVidUpdate
  UINT32 NbVidUpdateAll:1;           ///< NbVidUpdateAll
  UINT32 SinglePlaneNbFid:5;         ///< SinglePlaneNbFid
  UINT32 SinglePlaneNbVid:7;         ///< SinglePlaneNbVid
  UINT32 DualPlaneNbFidOff:3;        ///< DualPlaneNbFidOff
  UINT32 DualPlaneNbVidOff:5;        ///< DualPlaneNbVidOff
  UINT32 SinglePlaneNbIdd:4;         ///< SinglePlaneNbIdd
} PRODUCT_INFO_REGISTER;


#endif /* _CPUF10POWERMGMT_H */
