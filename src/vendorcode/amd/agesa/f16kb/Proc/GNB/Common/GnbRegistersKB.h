/* $NoKeywords:$ */
/**
 * @file
 *
 * AGESA gnb file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  GNB
 * @e \$Revision: 88282 $   @e \$Date: 2013-02-19 11:20:56 -0600 (Tue, 19 Feb 2013) $
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
#ifndef _GNBREGISTERSKB_H_
#define _GNBREGISTERSKB_H_
#define  TYPE_D0F0                  0x1
#define  TYPE_D0F0x64               0x2
#define  TYPE_D0F0x98               0x3
#define  TYPE_D0F0xBC               0x4
#define  TYPE_D18F1                 0xb
#define  TYPE_D18F2                 0xc
#define  TYPE_D18F3                 0xd
#define  TYPE_D18F4                 0xe
#define  TYPE_D18F5                 0xf
#define  TYPE_MSR                   0x10
#define  TYPE_D1F0                  0x11
#define  TYPE_D18F2x9C_dct0         0x13
#define  TYPE_D18F2x9C_dct0_mp0     0x14
#define  TYPE_D18F2x9C_dct0_mp1     0x15
#define  TYPE_D18F2x9C_dct1         0x16
#define  TYPE_D18F2x9C_dct1_mp0     0x17
#define  TYPE_D18F2x9C_dct1_mp1     0x18
#define  TYPE_D18F2_dct0            0x19
#define  TYPE_D18F2_dct1            0x1a
#define  TYPE_D18F2_dct0_mp0        0x1b
#define  TYPE_D18F2_dct0_mp1        0x1c
#define  TYPE_D1F1                  0x1d
#define  TYPE_D18F2_dct1_mp0        0x1e
#define  TYPE_D18F2_dct1_mp1        0x1f
#define  TYPE_CGIND                 0x20
#define  TYPE_SMU_MSG               0x21
#define  TYPE_D0F0xD4               0x22
#define  TYPE_D18F0                 0x24

#ifndef WRAP_SPACE
  #define  WRAP_SPACE(w, x)   (0x01300000 | (w << 16) | (x))
#endif
#ifndef CORE_SPACE
  #define  CORE_SPACE(c, x)   (0x00010000 | (c << 24) | (x))
#endif
#ifndef PHY_SPACE
  #define  PHY_SPACE(w, p, x) (0x00200000 | ((p + 1) << 24) | (w << 16) | (x))
#endif
#ifndef PIF_SPACE
  #define  PIF_SPACE(w, p, x) (0x00100000 | ((p + 1) << 24) | (w << 16) | (x))
#endif

#define  INVALID_SMU_MSG                    0xFF
#define  SMU_MSG_TYPE                       TYPE_SMU_MSG
#define  SMC_MSG_TEST                       INVALID_SMU_MSG // 1
#define  SMC_MSG_PHY_LN_OFF                 INVALID_SMU_MSG // 2
#define  SMC_MSG_PHY_LN_ON                  INVALID_SMU_MSG // 3
#define  SMC_MSG_DDI_PHY_OFF                INVALID_SMU_MSG // 4
#define  SMC_MSG_DDI_PHY_ON                 INVALID_SMU_MSG // 5
#define  SMC_MSG_CASCADE_PLL_OFF            INVALID_SMU_MSG // 6
#define  SMC_MSG_CASCADE_PLL_ON             INVALID_SMU_MSG // 7
#define  SMC_MSG_PWR_OFF_x16                INVALID_SMU_MSG // 8
#define  SMC_MSG_FLUSH_DATA_CACHE           INVALID_SMU_MSG // 10
#define  SMC_MSG_FLUSH_INSTRUCTION_CACHE    INVALID_SMU_MSG // 11
#define  SMC_MSG_CONFIG_VPC_ACCUMULATOR     12
#define  SMC_MSG_RECONFIGURE_SB             25
#define  SMC_MSG_UPDATE_BAPM                INVALID_SMU_MSG // 26
#define  SMC_MSG_PCIE_PLLSWITCH             INVALID_SMU_MSG // 27
#define  SMC_MSG_FORCE_LCLK_DPM_STATE       INVALID_SMU_MSG // 28
#define  SMC_MSG_UNFORCE_LCLK_DPM_STATE     INVALID_SMU_MSG // 29
#define  SMC_MSG_LCLK_DPM_ENABLE            30
#define  SMC_MSG_LCLK_DPM_DISABLE           31
#define  SMC_MSG_DDI_PHY_IDLE               INVALID_SMU_MSG // 34
#define  SMC_MSG_DDI_PHY_BUSY               INVALID_SMU_MSG // 35
#define  SMC_MSG_DSMU                       INVALID_SMU_MSG // 36
#define  SMC_MSG_NBDPM_DISABLE              INVALID_SMU_MSG // 37
#define  SMC_MSG_NBDPM_FORCENOMINAL         INVALID_SMU_MSG // 38
#define  SMC_MSG_NBDPM_FORCEPERFORMANCE     INVALID_SMU_MSG // 39
#define  SMC_MSG_NBDPM_UNFORCE              INVALID_SMU_MSG // 40
#define  SMC_MSG_RESET                      INVALID_SMU_MSG // 41
#define  SMC_MSG_AVS_ENABLE                 INVALID_SMU_MSG // 42
#define  SMC_MSG_AVS_DISABLE                INVALID_SMU_MSG // 43
#define  SMC_MSG_LPMX_ENABLE                INVALID_SMU_MSG // 44
#define  SMC_MSG_LPMX_DISABLE               INVALID_SMU_MSG // 45
#define  SMC_MSG_TDC_LIMIT_DISABLE          47
#define  SMC_MSG_BAPM_DISABLE               49
#define  SMC_MSG_TDP_LIMIT_ENABLE           INVALID_SMU_MSG // 50
#define  SMC_MSG_TDP_LIMIT_DISABLE          INVALID_SMU_MSG // 51
#define  SMC_MSG_SPMI_ENABLE                INVALID_SMU_MSG // 52
#define  SMC_MSG_SPMI_DISABLE               INVALID_SMU_MSG // 53
#define  SMC_MSG_SPMI_TEST                  INVALID_SMU_MSG // 54
#define  SMC_MSG_EXT_API                    INVALID_SMU_MSG // 55
#define  SMC_MSG_THERMAL_CNTL_DISABLE       57
#define  SMC_MSG_VDDNB_REQUEST              58
#define  SMC_MSG_VOLTAGE_CNTL_ENABLE        59
#define  SMC_MSG_VOLTAGE_CNTL_DISABLE       60
#define  SMC_MSG_NBDPM_ENABLE               INVALID_SMU_MSG
#define  SMC_MSG_MCARB_UPDATE_ENABLE        INVALID_SMU_MSG
#define  SMC_MSG_MCARB_UPDATE_DISABLE       INVALID_SMU_MSG
#define  SMC_MSG_LHTC_LIMIT_DISABLE         71
#define  SMC_MSG_PWR_OFF_IOMMU              INVALID_SMU_MSG
#define  SMC_MSG_PWR_ON_IOMMU               INVALID_SMU_MSG
#define  SMC_MSG_MONITOR_PORT80_ENABLE      INVALID_SMU_MSG
#define  SMC_MSG_MONITOR_PORT80_DISABLE     INVALID_SMU_MSG
#define  SMC_MSG_LCLKDPM_SETENABLEMASK      INVALID_SMU_MSG
#define  SMC_MSG_READ_SCS                   80
#define  SMC_MSG_ENABLE_PKGPWRLIMIT         81
#define  SMC_MSG_DISABLE_PKGPWRLIMIT        82
#define  SMC_MSG_ENABLE_ALLCLK_MONITOR      83
#define  SMC_MSG_DISABLE_ALLCLK_MONITOR     INVALID_SMU_MSG
#define  SMC_MSG_LHTC_LIMIT_SetLimit        90 // 0x5A

#define  L1_SEL_PPD                         0
#define  L1_SEL_BIF                         1
#define  L1_SEL_INTGEN                      2

// **** D18F0x110 Register Definition ****
// Address
#define D18F0x110_ADDRESS                                       0x110

// Type
#define D18F0x110_TYPE                                          TYPE_D18F0

// **** D18F1x44 Register Definition ****
// Address
#define D18F1x44_ADDRESS                                        0x44
// Type
#define D18F1x44_TYPE                                           TYPE_D18F1

// Field Data
#define D18F1x44_DstNode_OFFSET                                 0
#define D18F1x44_DstNode_WIDTH                                  3
#define D18F1x44_DstNode_MASK                                   0x7
#define D18F1x44_Reserved_7_3_OFFSET                            3
#define D18F1x44_Reserved_7_3_WIDTH                             5
#define D18F1x44_Reserved_7_3_MASK                              0xF8
#define D18F1x44_Reserved_10_8_OFFSET                           8
#define D18F1x44_Reserved_10_8_WIDTH                            3
#define D18F1x44_Reserved_10_8_MASK                             0x700
#define D18F1x44_Reserved_15_11_OFFSET                          11
#define D18F1x44_Reserved_15_11_WIDTH                           5
#define D18F1x44_Reserved_15_11_MASK                            0xF800
#define D18F1x44_DramLimit_39_24_OFFSET                         16
#define D18F1x44_DramLimit_39_24_WIDTH                          16
#define D18F1x44_DramLimit_39_24_MASK                           0xFFFF0000

/// D18F1x44
typedef union {
  struct {                                                            ///<
    UINT32                                                 DstNode:3; ///<
    UINT32                                            Reserved_7_3:5; ///<
    UINT32                                           Reserved_10_8:3; ///<
    UINT32                                          Reserved_15_11:5; ///<
    UINT32                                         DramLimit_39_24:16; ///<

  } Field;

  UINT32 Value;
} D18F1x44_STRUCT;



// **** D18F2x90_dct0 Register Definition ****
// Address
#define D18F2x90_dct0_ADDRESS                                   0x90
// Type
#define D18F2x90_dct0_TYPE                                      TYPE_D18F2

// Field Data
#define D18F2x90_dct0_ExitSelfRef_OFFSET                        1
#define D18F2x90_dct0_ExitSelfRef_WIDTH                         1
#define D18F2x90_dct0_ExitSelfRef_MASK                          0x2
#define D18F2x90_dct0_Reserved_7_2_OFFSET                       2
#define D18F2x90_dct0_Reserved_7_2_WIDTH                        6
#define D18F2x90_dct0_Reserved_7_2_MASK                         0xfc
#define D18F2x90_dct0_Reserved_8_8_OFFSET                       8
#define D18F2x90_dct0_Reserved_8_8_WIDTH                        1
#define D18F2x90_dct0_Reserved_8_8_MASK                         0x100
#define D18F2x90_dct0_Reserved_10_9_OFFSET                      9
#define D18F2x90_dct0_Reserved_10_9_WIDTH                       2
#define D18F2x90_dct0_Reserved_10_9_MASK                        0x600
#define D18F2x90_dct0_Reserved_11_11_OFFSET                     11
#define D18F2x90_dct0_Reserved_11_11_WIDTH                      1
#define D18F2x90_dct0_Reserved_11_11_MASK                       0x800
#define D18F2x90_dct0_Reserved_15_12_OFFSET                     12
#define D18F2x90_dct0_Reserved_15_12_WIDTH                      4
#define D18F2x90_dct0_Reserved_15_12_MASK                       0xf000
#define D18F2x90_dct0_UnbuffDimm_OFFSET                         16
#define D18F2x90_dct0_UnbuffDimm_WIDTH                          1
#define D18F2x90_dct0_UnbuffDimm_MASK                           0x10000
#define D18F2x90_dct0_UnbuffDimm_VALUE                          0x1
#define D18F2x90_dct0_EnterSelfRef_OFFSET                       17
#define D18F2x90_dct0_EnterSelfRef_WIDTH                        1
#define D18F2x90_dct0_EnterSelfRef_MASK                         0x20000
#define D18F2x90_dct0_PendRefPayback_OFFSET                     18
#define D18F2x90_dct0_PendRefPayback_WIDTH                      1
#define D18F2x90_dct0_PendRefPayback_MASK                       0x40000
#define D18F2x90_dct0_PendRefPayback_VALUE                      0x0
#define D18F2x90_dct0_DimmEccEn_OFFSET                          19
#define D18F2x90_dct0_DimmEccEn_WIDTH                           1
#define D18F2x90_dct0_DimmEccEn_MASK                            0x80000
#define D18F2x90_dct0_DynPageCloseEn_OFFSET                     20
#define D18F2x90_dct0_DynPageCloseEn_WIDTH                      1
#define D18F2x90_dct0_DynPageCloseEn_MASK                       0x100000
#define D18F2x90_dct0_IdleCycLowLimit_OFFSET                    21
#define D18F2x90_dct0_IdleCycLowLimit_WIDTH                     2
#define D18F2x90_dct0_IdleCycLowLimit_MASK                      0x600000
#define D18F2x90_dct0_ForceAutoPchg_OFFSET                      23
#define D18F2x90_dct0_ForceAutoPchg_WIDTH                       1
#define D18F2x90_dct0_ForceAutoPchg_MASK                        0x800000
#define D18F2x90_dct0_StagRefEn_OFFSET                          24
#define D18F2x90_dct0_StagRefEn_WIDTH                           1
#define D18F2x90_dct0_StagRefEn_MASK                            0x1000000
#define D18F2x90_dct0_StagRefEn_VALUE                           0x1
#define D18F2x90_dct0_PendRefPaybackS3En_OFFSET                 25
#define D18F2x90_dct0_PendRefPaybackS3En_WIDTH                  1
#define D18F2x90_dct0_PendRefPaybackS3En_MASK                   0x2000000
#define D18F2x90_dct0_PendRefPaybackS3En_VALUE                  0x1
#define D18F2x90_dct0_Reserved_26_26_OFFSET                     26
#define D18F2x90_dct0_Reserved_26_26_WIDTH                      1
#define D18F2x90_dct0_Reserved_26_26_MASK                       0x4000000
#define D18F2x90_dct0_DisDllShutdownSR_OFFSET                   27
#define D18F2x90_dct0_DisDllShutdownSR_WIDTH                    1
#define D18F2x90_dct0_DisDllShutdownSR_MASK                     0x8000000
#define D18F2x90_dct0_IdleCycLimit_OFFSET                       28
#define D18F2x90_dct0_IdleCycLimit_WIDTH                        4
#define D18F2x90_dct0_IdleCycLimit_MASK                         0xf0000000
#define D18F2x90_dct0_IdleCycLimit_VALUE                        0x8

/// D18F2x90_dct0
typedef union {
  struct {                                                              ///<
    UINT32                                              ExitSelfRef:1 ; ///<
    UINT32                                             Reserved_7_2:6 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                            Reserved_10_9:2 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                               UnbuffDimm:1 ; ///<
    UINT32                                             EnterSelfRef:1 ; ///<
    UINT32                                           PendRefPayback:1 ; ///<
    UINT32                                                DimmEccEn:1 ; ///<
    UINT32                                           DynPageCloseEn:1 ; ///<
    UINT32                                          IdleCycLowLimit:2 ; ///<
    UINT32                                            ForceAutoPchg:1 ; ///<
    UINT32                                                StagRefEn:1 ; ///<
    UINT32                                       PendRefPaybackS3En:1 ; ///<
    UINT32                                           Reserved_26_26:1 ; ///<
    UINT32                                         DisDllShutdownSR:1 ; ///<
    UINT32                                             IdleCycLimit:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x90_dct0_STRUCT;


// **** D18F2x94_dct0 Register Definition ****
// Address
#define D18F2x94_dct0_ADDRESS                                   0x94
// Type
#define D18F2x94_dct0_TYPE                                      TYPE_D18F2_dct0

// Field Data
#define D18F2x94_dct0_MemClkFreq_OFFSET                         0
#define D18F2x94_dct0_MemClkFreq_WIDTH                          5
#define D18F2x94_dct0_MemClkFreq_MASK                           0x1f
#define D18F2x94_dct0_Reserved_6_5_OFFSET                       5
#define D18F2x94_dct0_Reserved_6_5_WIDTH                        2
#define D18F2x94_dct0_Reserved_6_5_MASK                         0x60
#define D18F2x94_dct0_MemClkFreqVal_OFFSET                      7
#define D18F2x94_dct0_MemClkFreqVal_WIDTH                       1
#define D18F2x94_dct0_MemClkFreqVal_MASK                        0x80
#define D18F2x94_dct0_Reserved_9_8_OFFSET                       8
#define D18F2x94_dct0_Reserved_9_8_WIDTH                        2
#define D18F2x94_dct0_Reserved_9_8_MASK                         0x300
#define D18F2x94_dct0_ZqcsInterval_OFFSET                       10
#define D18F2x94_dct0_ZqcsInterval_WIDTH                        2
#define D18F2x94_dct0_ZqcsInterval_MASK                         0xc00
#define D18F2x94_dct0_RDqsEn_OFFSET                             12
#define D18F2x94_dct0_RDqsEn_WIDTH                              1
#define D18F2x94_dct0_RDqsEn_MASK                               0x1000
#define D18F2x94_dct0_Reserved_13_13_OFFSET                     13
#define D18F2x94_dct0_Reserved_13_13_WIDTH                      1
#define D18F2x94_dct0_Reserved_13_13_MASK                       0x2000
#define D18F2x94_dct0_DisDramInterface_OFFSET                   14
#define D18F2x94_dct0_DisDramInterface_WIDTH                    1
#define D18F2x94_dct0_DisDramInterface_MASK                     0x4000
#define D18F2x94_dct0_PowerDownEn_OFFSET                        15
#define D18F2x94_dct0_PowerDownEn_WIDTH                         1
#define D18F2x94_dct0_PowerDownEn_MASK                          0x8000
#define D18F2x94_dct0_PowerDownMode_OFFSET                      16
#define D18F2x94_dct0_PowerDownMode_WIDTH                       1
#define D18F2x94_dct0_PowerDownMode_MASK                        0x10000
#define D18F2x94_dct0_PowerDownMode_VALUE                       0x1
#define D18F2x94_dct0_Reserved_17_17_OFFSET                     17
#define D18F2x94_dct0_Reserved_17_17_WIDTH                      1
#define D18F2x94_dct0_Reserved_17_17_MASK                       0x20000
#define D18F2x94_dct0_Reserved_18_18_OFFSET                     18
#define D18F2x94_dct0_Reserved_18_18_WIDTH                      1
#define D18F2x94_dct0_Reserved_18_18_MASK                       0x40000
#define D18F2x94_dct0_Reserved_19_19_OFFSET                     19
#define D18F2x94_dct0_Reserved_19_19_WIDTH                      1
#define D18F2x94_dct0_Reserved_19_19_MASK                       0x80000
#define D18F2x94_dct0_SlowAccessMode_OFFSET                     20
#define D18F2x94_dct0_SlowAccessMode_WIDTH                      1
#define D18F2x94_dct0_SlowAccessMode_MASK                       0x100000
#define D18F2x94_dct0_FreqChgInProg_OFFSET                      21
#define D18F2x94_dct0_FreqChgInProg_WIDTH                       1
#define D18F2x94_dct0_FreqChgInProg_MASK                        0x200000
#define D18F2x94_dct0_BankSwizzleMode_OFFSET                    22
#define D18F2x94_dct0_BankSwizzleMode_WIDTH                     1
#define D18F2x94_dct0_BankSwizzleMode_MASK                      0x400000
#define D18F2x94_dct0_ProcOdtDis_OFFSET                         23
#define D18F2x94_dct0_ProcOdtDis_WIDTH                          1
#define D18F2x94_dct0_ProcOdtDis_MASK                           0x800000
#define D18F2x94_dct0_DcqBypassMax_OFFSET                       24
#define D18F2x94_dct0_DcqBypassMax_WIDTH                        5
#define D18F2x94_dct0_DcqBypassMax_MASK                         0x1f000000
#define D18F2x94_dct0_Reserved_30_29_OFFSET                     29
#define D18F2x94_dct0_Reserved_30_29_WIDTH                      2
#define D18F2x94_dct0_Reserved_30_29_MASK                       0x60000000
#define D18F2x94_dct0_DphyMemPsSelEn_OFFSET                     31
#define D18F2x94_dct0_DphyMemPsSelEn_WIDTH                      1
#define D18F2x94_dct0_DphyMemPsSelEn_MASK                       0x80000000
#define D18F2x94_dct0_DphyMemPsSelEn_VALUE                      0x1

/// D18F2x94_dct0
typedef union {
  struct {                                                              ///<
    UINT32                                               MemClkFreq:5 ; ///<
    UINT32                                             Reserved_6_5:2 ; ///<
    UINT32                                            MemClkFreqVal:1 ; ///<
    UINT32                                             Reserved_9_8:2 ; ///<
    UINT32                                             ZqcsInterval:2 ; ///<
    UINT32                                                   RDqsEn:1 ; ///<
    UINT32                                           Reserved_13_13:1 ; ///<
    UINT32                                         DisDramInterface:1 ; ///<
    UINT32                                              PowerDownEn:1 ; ///<
    UINT32                                            PowerDownMode:1 ; ///<
    UINT32                                           Reserved_17_17:1 ; ///<
    UINT32                                           Reserved_18_18:1 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                           SlowAccessMode:1 ; ///<
    UINT32                                            FreqChgInProg:1 ; ///<
    UINT32                                          BankSwizzleMode:1 ; ///<
    UINT32                                               ProcOdtDis:1 ; ///<
    UINT32                                             DcqBypassMax:5 ; ///<
    UINT32                                           Reserved_30_29:2 ; ///<
    UINT32                                           DphyMemPsSelEn:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x94_dct0_STRUCT;

#define D18F2xA8_dct0_MemPhyPllPdMode_OFFSET                    16
#define D18F2xA8_dct0_MemPhyPllPdMode_WIDTH                     2

// **** D18F2x2E0_dct0 Register Definition ****
// Address
#define D18F2x2E0_dct0_ADDRESS                                  0x2e0

// Type
#define D18F2x2E0_dct0_TYPE                                     TYPE_D18F2
// Field Data

#define D18F2x2E0_dct0_M1MemClkFreq_OFFSET                      24
#define D18F2x2E0_dct0_M1MemClkFreq_WIDTH                       5

/// D18F2x2E0_dct0
typedef union {
  struct {                                                              ///<
    UINT32                                               Reserved_1:1 ; ///<
    UINT32                                            Reserved_19_1:19; ///<
    UINT32                                           Reserved_22_20:3 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                             M1MemClkFreq:5 ; ///<
    UINT32                                           Reserved_29_29:1 ; ///<
    UINT32                                           Reserved_30_30:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x2E0_dct0_STRUCT;

// **** D18F4x110 Register Definition ****
// Address
#define D18F4x110_ADDRESS                                       0x110

// Type
#define D18F4x110_TYPE                                          TYPE_D18F4
// Field Data
#define D18F4x110_CSampleTimer_OFFSET                           0
#define D18F4x110_CSampleTimer_WIDTH                            12
#define D18F4x110_CSampleTimer_MASK                             0xfff
#define D18F4x110_CSampleTimer_VALUE                            0x2
#define D18F4x110_FastCSampleTimer_OFFSET                       12
#define D18F4x110_FastCSampleTimer_WIDTH                        1
#define D18F4x110_FastCSampleTimer_MASK                         0x1000
#define D18F4x110_MinResTmr_OFFSET                              13
#define D18F4x110_MinResTmr_WIDTH                               8
#define D18F4x110_MinResTmr_MASK                                0x1fe000
#define D18F4x110_Reserved_31_21_OFFSET                         21
#define D18F4x110_Reserved_31_21_WIDTH                          11
#define D18F4x110_Reserved_31_21_MASK                           0xffe00000

/// D18F4x110
typedef union {
  struct {                                                              ///<
    UINT32                                             CSampleTimer:12; ///<
    UINT32                                         FastCSampleTimer:1 ; ///<
    UINT32                                                MinResTmr:8 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F4x110_STRUCT;

// **** D18F5xE0 Register Definition ****
// Address
#define D18F5xE0_ADDRESS                                        0xe0

// Type
#define D18F5xE0_TYPE                                           TYPE_D18F5
// Field Data
#define D18F5xE0_RunAvgRange_OFFSET                             0
#define D18F5xE0_RunAvgRange_WIDTH                              4
#define D18F5xE0_RunAvgRange_MASK                               0xf
#define D18F5xE0_RunAvgRange_VALUE                              0x2
#define D18F5xE0_Reserved_31_4_OFFSET                           4
#define D18F5xE0_Reserved_31_4_WIDTH                            28
#define D18F5xE0_Reserved_31_4_MASK                             0xfffffff0

/// D18F5xE0
typedef union {
  struct {                                                              ///<
    UINT32                                              RunAvgRange:4 ; ///<
    UINT32                                           Reserved_31_4:28 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F5xE0_STRUCT;

// **** D18F5x160 Register Definition ****
// Address
#define D18F5x160_ADDRESS                                       0x160

// Type
#define D18F5x160_TYPE                                          TYPE_D18F5
#define D18F5x160_NbPstateEn_OFFSET                             0
#define D18F5x160_NbPstateEn_WIDTH                              1

#define D18F5x160_MemPstate_OFFSET                              18
#define D18F5x160_MemPstate_WIDTH                               1
#define D18F5x160_NbFid_OFFSET                                  1
#define D18F5x160_NbFid_WIDTH                                   6
#define D18F5x160_NbDid_OFFSET                                  7
#define D18F5x160_NbDid_WIDTH                                   1
#define D18F5x160_NbVid_6_0_OFFSET                             10
#define D18F5x160_NbVid_6_0_WIDTH                              7
#define D18F5x160_NbVid_7_OFFSET                               21
#define D18F5x160_NbVid_7_WIDTH                                1

#define D18F5x160_NbVid_7__OFFSET                               21
#define D18F5x160_NbVid_7__WIDTH                                1

/// D18F5x160
typedef union {
  struct {                                                              ///<
    UINT32                                               NbPstateEn:1 ; ///<
    UINT32                                                    NbFid:6 ; ///<
    UINT32                                                    NbDid:1 ; ///<
    UINT32                                             Reserved_9_8:2 ; ///<
    UINT32                                               NbVid_6_0_:7 ; ///<
    UINT32                                           Reserved_17_17:1 ; ///<
    UINT32                                                MemPstate:1 ; ///<
    UINT32                                           Reserved_20_19:2 ; ///<
    UINT32                                                 NbVid_7_:1 ; ///<
    UINT32                                           Reserved_23_22:2 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F5x160_STRUCT;

// **** D18F5x164 Register Definition ****
// Address
#define D18F5x164_ADDRESS                                       0x164

// Type
#define D18F5x164_TYPE                                          TYPE_D18F5
#define D18F5x164_NbPstateEn_OFFSET                             0
#define D18F5x164_NbPstateEn_WIDTH                              1
#define D18F5x164_MemPstate_OFFSET                              18
#define D18F5x164_MemPstate_WIDTH                               1
#define D18F5x164_NbFid_OFFSET                                  1
#define D18F5x164_NbFid_WIDTH                                   6
#define D18F5x164_NbDid_OFFSET                                  7
#define D18F5x164_NbDid_WIDTH                                   1
#define D18F5x164_NbVid_6_0_OFFSET                             10
#define D18F5x164_NbVid_6_0_WIDTH                              7
#define D18F5x164_NbVid_7_OFFSET                               21
#define D18F5x164_NbVid_7_WIDTH                                1

// **** D18F5x168 Register Definition ****
// Address
#define D18F5x168_ADDRESS                                       0x168

// Type
#define D18F5x168_TYPE                                          TYPE_D18F5
#define D18F5x168_NbPstateEn_OFFSET                             0
#define D18F5x168_NbPstateEn_WIDTH                              1
#define D18F5x168_NbFid_OFFSET                                  1
#define D18F5x168_NbFid_WIDTH                                   6
#define D18F5x168_NbDid_OFFSET                                  7
#define D18F5x168_NbDid_WIDTH                                   1
#define D18F5x168_NbVid_6_0_OFFSET                             10
#define D18F5x168_NbVid_6_0_WIDTH                              7
#define D18F5x168_MemPstate_OFFSET                              18
#define D18F5x168_MemPstate_WIDTH                               1
#define D18F5x168_NbVid_7_OFFSET                               21
#define D18F5x168_NbVid_7_WIDTH                                1

// **** D18F5x16C Register Definition ****
// Address
#define D18F5x16C_ADDRESS                                       0x16c

// Type
#define D18F5x16C_TYPE                                          TYPE_D18F5
#define D18F5x16C_NbPstateEn_OFFSET                             0
#define D18F5x16C_NbPstateEn_WIDTH                              1
#define D18F5x16C_NbFid_OFFSET                                  1
#define D18F5x16C_NbFid_WIDTH                                   6
#define D18F5x16C_NbDid_OFFSET                                  7
#define D18F5x16C_NbDid_WIDTH                                   1
#define D18F5x16C_NbVid_6_0_OFFSET                             10
#define D18F5x16C_NbVid_6_0_WIDTH                              7
#define D18F5x16C_MemPstate_OFFSET                              18
#define D18F5x16C_MemPstate_WIDTH                               1
#define D18F5x16C_NbVid_7_OFFSET                               21
#define D18F5x16C_NbVid_7_WIDTH                                1

// **** D18F5x170 Register Definition ****
// Address
#define D18F5x170_ADDRESS                                       0x170

// Type
#define D18F5x170_TYPE                                          TYPE_D18F5
// Field Data
#define D18F5x170_NbPstateMaxVal_OFFSET                         0
#define D18F5x170_NbPstateMaxVal_WIDTH                          2
#define D18F5x170_NbPstateMaxVal_MASK                           0x3
#define D18F5x170_Reserved_2_2_OFFSET                           2
#define D18F5x170_Reserved_2_2_WIDTH                            1
#define D18F5x170_Reserved_2_2_MASK                             0x4
#define D18F5x170_NbPstateLo_OFFSET                             3
#define D18F5x170_NbPstateLo_WIDTH                              2
#define D18F5x170_NbPstateLo_MASK                               0x18
#define D18F5x170_Reserved_5_5_OFFSET                           5
#define D18F5x170_Reserved_5_5_WIDTH                            1
#define D18F5x170_Reserved_5_5_MASK                             0x20
#define D18F5x170_NbPstateHi_OFFSET                             6
#define D18F5x170_NbPstateHi_WIDTH                              2
#define D18F5x170_NbPstateHi_MASK                               0xc0
#define D18F5x170_Reserved_8_8_OFFSET                           8
#define D18F5x170_Reserved_8_8_WIDTH                            1
#define D18F5x170_Reserved_8_8_MASK                             0x100
#define D18F5x170_NbPstateThreshold_OFFSET                      9
#define D18F5x170_NbPstateThreshold_WIDTH                       4
#define D18F5x170_NbPstateThreshold_MASK                        0x1e00
#define D18F5x170_Reserved_12_12_OFFSET                         12
#define D18F5x170_Reserved_12_12_WIDTH                          1
#define D18F5x170_Reserved_12_12_MASK                           0x1000
#define D18F5x170_NbPstateDisOnP0_OFFSET                        13
#define D18F5x170_NbPstateDisOnP0_WIDTH                         1
#define D18F5x170_NbPstateDisOnP0_MASK                          0x2000
#define D18F5x170_SwNbPstateLoDis_OFFSET                        14
#define D18F5x170_SwNbPstateLoDis_WIDTH                         1
#define D18F5x170_SwNbPstateLoDis_MASK                          0x4000
#define D18F5x170_Reserved_22_15_OFFSET                         15
#define D18F5x170_Reserved_22_15_WIDTH                          8
#define D18F5x170_Reserved_22_15_MASK                           0x3f8000
#define D18F5x170_NbPstateGnbSlowDis_OFFSET                     23
#define D18F5x170_NbPstateGnbSlowDis_WIDTH                      1
#define D18F5x170_NbPstateGnbSlowDis_MASK                       0x800000
#define D18F5x170_NbPstateLoRes_OFFSET                          24
#define D18F5x170_NbPstateLoRes_WIDTH                           3
#define D18F5x170_NbPstateLoRes_MASK                            0x7000000
#define D18F5x170_NbPstateHiRes_OFFSET                          27
#define D18F5x170_NbPstateHiRes_WIDTH                           3
#define D18F5x170_NbPstateHiRes_MASK                            0x38000000
#define D18F5x170_NbPstateFidVidSbcEn_OFFSET                    30
#define D18F5x170_NbPstateFidVidSbcEn_WIDTH                     1
#define D18F5x170_NbPstateFidVidSbcEn_MASK                      0x40000000
#define D18F5x170_MemPstateDis_OFFSET                           31
#define D18F5x170_MemPstateDis_WIDTH                            1
#define D18F5x170_MemPstateDis_MASK                             0x80000000

/// D18F5x170
typedef union {
  struct {                                                              ///<
    UINT32                                           NbPstateMaxVal:2 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                               NbPstateLo:2 ; ///<
    UINT32                                             Reserved_5_5:1 ; ///<
    UINT32                                               NbPstateHi:2 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                        NbPstateThreshold:4 ; ///<
    UINT32                                          NbPstateDisOnP0:1 ; ///<
    UINT32                                          SwNbPstateLoDis:1 ; ///<
    UINT32                                           Reserved_22_15:8 ; ///<
    UINT32                                       NbPstateGnbSlowDis:1 ; ///<
    UINT32                                            NbPstateLoRes:3 ; ///<
    UINT32                                            NbPstateHiRes:3 ; ///<
    UINT32                                      NbPstateFidVidSbcEn:1 ; ///<
    UINT32                                             MemPstateDis:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F5x170_STRUCT;

// **** D18F2x118 Register Definition ****
// Address
#define D18F2x118_ADDRESS                                       0x118
// Type
#define D18F2x118_TYPE                                          TYPE_D18F2

// Field Data
#define D18F2x118_MctPriCpuRd_OFFSET                            0
#define D18F2x118_MctPriCpuRd_WIDTH                             2
#define D18F2x118_MctPriCpuRd_MASK                              0x3
#define D18F2x118_MctPriCpuWr_OFFSET                            2
#define D18F2x118_MctPriCpuWr_WIDTH                             2
#define D18F2x118_MctPriCpuWr_MASK                              0xC
#define D18F2x118_MctPriIsocRd_OFFSET                           4
#define D18F2x118_MctPriIsocRd_WIDTH                            2
#define D18F2x118_MctPriIsocRd_MASK                             0x30
#define D18F2x118_MctPriIsocWr_OFFSET                           6
#define D18F2x118_MctPriIsocWr_WIDTH                            2
#define D18F2x118_MctPriIsocWr_MASK                             0xC0
#define D18F2x118_MctPriDefault_OFFSET                          8
#define D18F2x118_MctPriDefault_WIDTH                           2
#define D18F2x118_MctPriDefault_MASK                            0x300
#define D18F2x118_MctPriWr_OFFSET                               10
#define D18F2x118_MctPriWr_WIDTH                                2
#define D18F2x118_MctPriWr_MASK                                 0xC00
#define D18F2x118_MctPriIsoc_OFFSET                             12
#define D18F2x118_MctPriIsoc_WIDTH                              2
#define D18F2x118_MctPriIsoc_MASK                               0x3000
#define D18F2x118_MctPriTrace_OFFSET                            14
#define D18F2x118_MctPriTrace_WIDTH                             2
#define D18F2x118_MctPriTrace_MASK                              0xC000
#define D18F2x118_MctPriScrub_OFFSET                            16
#define D18F2x118_MctPriScrub_WIDTH                             2
#define D18F2x118_MctPriScrub_MASK                              0x30000
#define D18F2x118_CC6SaveEn_OFFSET                              18
#define D18F2x118_CC6SaveEn_WIDTH                               1
#define D18F2x118_CC6SaveEn_MASK                                0x40000
#define D18F2x118_LockDramCfg_OFFSET                            19
#define D18F2x118_LockDramCfg_WIDTH                             1
#define D18F2x118_LockDramCfg_MASK                              0x80000
#define D18F2x118_McqMedPriByPassMax_OFFSET                     20
#define D18F2x118_McqMedPriByPassMax_WIDTH                      3
#define D18F2x118_McqMedPriByPassMax_MASK                       0x700000
#define D18F2x118_Reserved_23_23_OFFSET                         23
#define D18F2x118_Reserved_23_23_WIDTH                          1
#define D18F2x118_Reserved_23_23_MASK                           0x800000
#define D18F2x118_McqHiPriByPassMax_OFFSET                      24
#define D18F2x118_McqHiPriByPassMax_WIDTH                       3
#define D18F2x118_McqHiPriByPassMax_MASK                        0x7000000
#define D18F2x118_Reserved_27_27_OFFSET                         27
#define D18F2x118_Reserved_27_27_WIDTH                          1
#define D18F2x118_Reserved_27_27_MASK                           0x8000000
#define D18F2x118_MctVarPriCntLmt_OFFSET                        28
#define D18F2x118_MctVarPriCntLmt_WIDTH                         4
#define D18F2x118_MctVarPriCntLmt_MASK                          0xF0000000

/// D18F2x118
typedef union {
  struct {                                                            ///<
    UINT32                                             MctPriCpuRd:2; ///<
    UINT32                                             MctPriCpuWr:2; ///<
    UINT32                                            MctPriIsocRd:2; ///<
    UINT32                                            MctPriIsocWr:2; ///<
    UINT32                                           MctPriDefault:2; ///<
    UINT32                                                MctPriWr:2; ///<
    UINT32                                              MctPriIsoc:2; ///<
    UINT32                                             MctPriTrace:2; ///<
    UINT32                                             MctPriScrub:2; ///<
    UINT32                                               CC6SaveEn:1; ///<
    UINT32                                             LockDramCfg:1; ///<
    UINT32                                      McqMedPriByPassMax:3; ///<
    UINT32                                          Reserved_23_23:1; ///<
    UINT32                                       McqHiPriByPassMax:3; ///<
    UINT32                                          Reserved_27_27:1; ///<
    UINT32                                         MctVarPriCntLmt:4; ///<

  } Field;

  UINT32 Value;
} D18F2x118_STRUCT;


// **** D18F3x44 Register Definition ****
// Address
#define D18F3x44_ADDRESS                                        0x44

// Type
#define D18F3x44_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x44_Reserved_0_0_OFFSET                            0
#define D18F3x44_Reserved_0_0_WIDTH                             1
#define D18F3x44_Reserved_0_0_MASK                              0x1
#define D18F3x44_CpuRdDatErrEn_OFFSET                           1
#define D18F3x44_CpuRdDatErrEn_WIDTH                            1
#define D18F3x44_CpuRdDatErrEn_MASK                             0x2
#define D18F3x44_SyncFloodOnDramUcEcc_OFFSET                    2
#define D18F3x44_SyncFloodOnDramUcEcc_WIDTH                     1
#define D18F3x44_SyncFloodOnDramUcEcc_MASK                      0x4
#define D18F3x44_SyncFloodOnDramUcEcc_VALUE                     0x1
#define D18F3x44_SyncPktGenDis_OFFSET                           3
#define D18F3x44_SyncPktGenDis_WIDTH                            1
#define D18F3x44_SyncPktGenDis_MASK                             0x8
#define D18F3x44_SyncPktGenDis_VALUE                            0x0
#define D18F3x44_SyncPktPropDis_OFFSET                          4
#define D18F3x44_SyncPktPropDis_WIDTH                           1
#define D18F3x44_SyncPktPropDis_MASK                            0x10
#define D18F3x44_SyncPktPropDis_VALUE                           0x0
#define D18F3x44_IoMstAbortDis_OFFSET                           5
#define D18F3x44_IoMstAbortDis_WIDTH                            1
#define D18F3x44_IoMstAbortDis_MASK                             0x20
#define D18F3x44_CpuErrDis_OFFSET                               6
#define D18F3x44_CpuErrDis_WIDTH                                1
#define D18F3x44_CpuErrDis_MASK                                 0x40
#define D18F3x44_CpuErrDis_VALUE                                0x1
#define D18F3x44_IoErrDis_OFFSET                                7
#define D18F3x44_IoErrDis_WIDTH                                 1
#define D18F3x44_IoErrDis_MASK                                  0x80
#define D18F3x44_WDTDis_OFFSET                                  8
#define D18F3x44_WDTDis_WIDTH                                   1
#define D18F3x44_WDTDis_MASK                                    0x100
#define D18F3x44_WDTCntSel_2_0__OFFSET                          9
#define D18F3x44_WDTCntSel_2_0__WIDTH                           3
#define D18F3x44_WDTCntSel_2_0__MASK                            0xe00
#define D18F3x44_WDTBaseSel_OFFSET                              12
#define D18F3x44_WDTBaseSel_WIDTH                               2
#define D18F3x44_WDTBaseSel_MASK                                0x3000
#define D18F3x44_GenLinkSel_OFFSET                              14
#define D18F3x44_GenLinkSel_WIDTH                               2
#define D18F3x44_GenLinkSel_MASK                                0xc000
#define D18F3x44_GenCrcErrByte0_OFFSET                          16
#define D18F3x44_GenCrcErrByte0_WIDTH                           1
#define D18F3x44_GenCrcErrByte0_MASK                            0x10000
#define D18F3x44_GenCrcErrByte1_OFFSET                          17
#define D18F3x44_GenCrcErrByte1_WIDTH                           1
#define D18F3x44_GenCrcErrByte1_MASK                            0x20000
#define D18F3x44_GenSubLinkSel_OFFSET                           18
#define D18F3x44_GenSubLinkSel_WIDTH                            2
#define D18F3x44_GenSubLinkSel_MASK                             0xc0000
#define D18F3x44_SyncFloodOnWDT_OFFSET                          20
#define D18F3x44_SyncFloodOnWDT_WIDTH                           1
#define D18F3x44_SyncFloodOnWDT_MASK                            0x100000
#define D18F3x44_SyncFloodOnWDT_VALUE                           0x1
#define D18F3x44_SyncFloodOnAnyUcErr_OFFSET                     21
#define D18F3x44_SyncFloodOnAnyUcErr_WIDTH                      1
#define D18F3x44_SyncFloodOnAnyUcErr_MASK                       0x200000
#define D18F3x44_SyncFloodOnAnyUcErr_VALUE                      0x1
#define D18F3x44_DramEccEn_OFFSET                               22
#define D18F3x44_DramEccEn_WIDTH                                1
#define D18F3x44_DramEccEn_MASK                                 0x400000
#define D18F3x44_ChipKillEccCap_OFFSET                          23
#define D18F3x44_ChipKillEccCap_WIDTH                           1
#define D18F3x44_ChipKillEccCap_MASK                            0x800000
#define D18F3x44_IoRdDatErrEn_OFFSET                            24
#define D18F3x44_IoRdDatErrEn_WIDTH                             1
#define D18F3x44_IoRdDatErrEn_MASK                              0x1000000
#define D18F3x44_DisPciCfgCpuErrRsp_OFFSET                      25
#define D18F3x44_DisPciCfgCpuErrRsp_WIDTH                       1
#define D18F3x44_DisPciCfgCpuErrRsp_MASK                        0x2000000
#define D18F3x44_FlagMcaCorrErr_OFFSET                          26
#define D18F3x44_FlagMcaCorrErr_WIDTH                           1
#define D18F3x44_FlagMcaCorrErr_MASK                            0x4000000
#define D18F3x44_NbMcaToMstCpuEn_OFFSET                         27
#define D18F3x44_NbMcaToMstCpuEn_WIDTH                          1
#define D18F3x44_NbMcaToMstCpuEn_MASK                           0x8000000
#define D18F3x44_NbMcaToMstCpuEn_VALUE                          0x1
#define D18F3x44_DisTgtAbortCpuErrRsp_OFFSET                    28
#define D18F3x44_DisTgtAbortCpuErrRsp_WIDTH                     1
#define D18F3x44_DisTgtAbortCpuErrRsp_MASK                      0x10000000
#define D18F3x44_DisMstAbortCpuErrRsp_OFFSET                    29
#define D18F3x44_DisMstAbortCpuErrRsp_WIDTH                     1
#define D18F3x44_DisMstAbortCpuErrRsp_MASK                      0x20000000
#define D18F3x44_SyncFloodOnDramAdrParErr_OFFSET                30
#define D18F3x44_SyncFloodOnDramAdrParErr_WIDTH                 1
#define D18F3x44_SyncFloodOnDramAdrParErr_MASK                  0x40000000
#define D18F3x44_SyncFloodOnDramAdrParErr_VALUE                 0x1
#define D18F3x44_NbMcaLogEn_OFFSET                              31
#define D18F3x44_NbMcaLogEn_WIDTH                               1
#define D18F3x44_NbMcaLogEn_MASK                                0x80000000

/// D18F3x44
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                            CpuRdDatErrEn:1 ; ///<
    UINT32                                     SyncFloodOnDramUcEcc:1 ; ///<
    UINT32                                            SyncPktGenDis:1 ; ///<
    UINT32                                           SyncPktPropDis:1 ; ///<
    UINT32                                            IoMstAbortDis:1 ; ///<
    UINT32                                                CpuErrDis:1 ; ///<
    UINT32                                                 IoErrDis:1 ; ///<
    UINT32                                                   WDTDis:1 ; ///<
    UINT32                                           WDTCntSel_2_0_:3 ; ///<
    UINT32                                               WDTBaseSel:2 ; ///<
    UINT32                                               GenLinkSel:2 ; ///<
    UINT32                                           GenCrcErrByte0:1 ; ///<
    UINT32                                           GenCrcErrByte1:1 ; ///<
    UINT32                                            GenSubLinkSel:2 ; ///<
    UINT32                                           SyncFloodOnWDT:1 ; ///<
    UINT32                                      SyncFloodOnAnyUcErr:1 ; ///<
    UINT32                                                DramEccEn:1 ; ///<
    UINT32                                           ChipKillEccCap:1 ; ///<
    UINT32                                             IoRdDatErrEn:1 ; ///<
    UINT32                                       DisPciCfgCpuErrRsp:1 ; ///<
    UINT32                                           FlagMcaCorrErr:1 ; ///<
    UINT32                                          NbMcaToMstCpuEn:1 ; ///<
    UINT32                                     DisTgtAbortCpuErrRsp:1 ; ///<
    UINT32                                     DisMstAbortCpuErrRsp:1 ; ///<
    UINT32                                 SyncFloodOnDramAdrParErr:1 ; ///<
    UINT32                                               NbMcaLogEn:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x44_STRUCT;

// **** D18F3x64 Register Definition ****
// Address
#define D18F3x64_ADDRESS                                        0x64

// Type
#define D18F3x64_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x64_HtcEn_OFFSET                                   0
#define D18F3x64_HtcEn_WIDTH                                    1
#define D18F3x64_HtcEn_MASK                                     0x1
#define D18F3x64_HtcEn_VALUE                                    0x0
#define D18F3x64_HtcAct_OFFSET                                  4
#define D18F3x64_HtcAct_WIDTH                                   1
#define D18F3x64_HtcAct_MASK                                    0x10
#define D18F3x64_HtcActSts_OFFSET                               5
#define D18F3x64_HtcActSts_WIDTH                                1
#define D18F3x64_HtcActSts_MASK                                 0x20
#define D18F3x64_PslApicHiEn_OFFSET                             6
#define D18F3x64_PslApicHiEn_WIDTH                              1
#define D18F3x64_PslApicHiEn_MASK                               0x40
#define D18F3x64_PslApicLoEn_OFFSET                             7
#define D18F3x64_PslApicLoEn_WIDTH                              1
#define D18F3x64_PslApicLoEn_MASK                               0x80
#define D18F3x64_HtcTmpLmt_OFFSET                               16
#define D18F3x64_HtcTmpLmt_WIDTH                                7
#define D18F3x64_HtcTmpLmt_MASK                                 0x7f0000
#define D18F3x64_HtcSlewSel_OFFSET                              23
#define D18F3x64_HtcSlewSel_WIDTH                               1
#define D18F3x64_HtcSlewSel_MASK                                0x800000
#define D18F3x64_HtcHystLmt_OFFSET                              24
#define D18F3x64_HtcHystLmt_WIDTH                               4
#define D18F3x64_HtcHystLmt_MASK                                0xf000000
#define D18F3x64_HtcPstateLimit_OFFSET                          28
#define D18F3x64_HtcPstateLimit_WIDTH                           3
#define D18F3x64_HtcPstateLimit_MASK                            0x70000000
#define D18F3x64_Reserved_31_31_OFFSET                          31
#define D18F3x64_Reserved_31_31_WIDTH                           1
#define D18F3x64_Reserved_31_31_MASK                            0x80000000

/// D18F3x64
typedef union {
  struct {                                                              ///<
    UINT32                                                    HtcEn:1 ; ///<
    UINT32                                         bit1:1;
    UINT32                                         bit2:1;
    UINT32                                             Reserved_3_3:1 ; ///<
    UINT32                                                   HtcAct:1 ; ///<
    UINT32                                                HtcActSts:1 ; ///<
    UINT32                                              PslApicHiEn:1 ; ///<
    UINT32                                              PslApicLoEn:1 ; ///<
    UINT32                                         bit10_8:3;
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                         bit14_12:3 ;
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                                HtcTmpLmt:7 ; ///<
    UINT32                                               HtcSlewSel:1 ; ///<
    UINT32                                               HtcHystLmt:4 ; ///<
    UINT32                                           HtcPstateLimit:3 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x64_STRUCT;


// **** D18F3xA0 Register Definition ****
// Address
#define D18F3xA0_ADDRESS                                        0xa0

// Type
#define D18F3xA0_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xA0_PsiVid_6_0__OFFSET                             0
#define D18F3xA0_PsiVid_6_0__WIDTH                              7
#define D18F3xA0_PsiVid_6_0__MASK                               0x7f
#define D18F3xA0_PsiVidEn_OFFSET                                7
#define D18F3xA0_PsiVidEn_WIDTH                                 1
#define D18F3xA0_PsiVidEn_MASK                                  0x80
#define D18F3xA0_PsiVid_7__OFFSET                               8
#define D18F3xA0_PsiVid_7__WIDTH                                1
#define D18F3xA0_PsiVid_7__MASK                                 0x100
#define D18F3xA0_Reserved_9_9_OFFSET                            9
#define D18F3xA0_Reserved_9_9_WIDTH                             1
#define D18F3xA0_Reserved_9_9_MASK                              0x200
#define D18F3xA0_IdleExitEn_OFFSET                              10
#define D18F3xA0_IdleExitEn_WIDTH                               1
#define D18F3xA0_IdleExitEn_MASK                                0x400
#define D18F3xA0_PllLockTime_OFFSET                             11
#define D18F3xA0_PllLockTime_WIDTH                              3
#define D18F3xA0_PllLockTime_MASK                               0x3800
#define D18F3xA0_PllLockTime_VALUE                              0x1
#define D18F3xA0_Svi2HighFreqSel_OFFSET                         14
#define D18F3xA0_Svi2HighFreqSel_WIDTH                          1
#define D18F3xA0_Svi2HighFreqSel_MASK                           0x4000
#define D18F3xA0_Svi2HighFreqSel_VALUE                          0x1
#define D18F3xA0_ConfigId_OFFSET                                16
#define D18F3xA0_ConfigId_WIDTH                                 12
#define D18F3xA0_ConfigId_MASK                                  0xfff0000
#define D18F3xA0_Reserved_29_29_OFFSET                          29
#define D18F3xA0_Reserved_29_29_WIDTH                           1
#define D18F3xA0_Reserved_29_29_MASK                            0x20000000
#define D18F3xA0_CofVidProg_OFFSET                              31
#define D18F3xA0_CofVidProg_WIDTH                               1
#define D18F3xA0_CofVidProg_MASK                                0x80000000

/// D18F3xA0
typedef union {
  struct {                                                              ///<
    UINT32                                              PsiVid_6_0_:7 ; ///<
    UINT32                                                 PsiVidEn:1 ; ///<
    UINT32                                                PsiVid_7_:1 ; ///<
    UINT32                                             Reserved_9_9:1 ; ///<
    UINT32                                               IdleExitEn:1 ; ///<
    UINT32                                              PllLockTime:3 ; ///<
    UINT32                                          Svi2HighFreqSel:1 ; ///<
    UINT32                                                         :1 ; ///<
    UINT32                                                 ConfigId:12; ///<
    UINT32                                                         :1 ; ///<
    UINT32                                           Reserved_29_29:1 ; ///<
    UINT32                                                         :1 ; ///<
    UINT32                                               CofVidProg:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xA0_STRUCT;


// **** D18F3xA8 Register Definition ****
// Address
#define D18F3xA8_ADDRESS                                        0xA8
// Type
#define D18F3xA8_TYPE                                           TYPE_D18F3

// Field Data
#define D18F3xA8_Reserved_28_0_OFFSET                           0
#define D18F3xA8_Reserved_28_0_WIDTH                            29
#define D18F3xA8_Reserved_28_0_MASK                             0x1FFFFFFF
#define D18F3xA8_PopDownPstate_OFFSET                           29
#define D18F3xA8_PopDownPstate_WIDTH                            3
#define D18F3xA8_PopDownPstate_MASK                             0xE0000000

/// D18F3xA8
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_28_0:29; ///<
    UINT32                                           PopDownPstate:3; ///<

  } Field;

  UINT32 Value;
} D18F3xA8_STRUCT;

// **** D18F5x12C Register Definition ****
// Address
#define D18F5x12C_ADDRESS                                       0x12C
// Type
#define D18F5x12C_TYPE                                          TYPE_D18F5

// Field Data
#define D18F5x12C_CoreOffsetTrim_OFFSET                         0
#define D18F5x12C_CoreOffsetTrim_WIDTH                          2
#define D18F5x12C_CoreOffsetTrim_MASK                           0x3
#define D18F5x12C_CoreLoadLineTrim_OFFSET                       2
#define D18F5x12C_CoreLoadLineTrim_WIDTH                        3
#define D18F5x12C_CoreLoadLineTrim_MASK                         0x1C
#define D18F5x12C_CorePsi1En_OFFSET                             5
#define D18F5x12C_CorePsi1En_WIDTH                              1
#define D18F5x12C_CorePsi1En_MASK                               0x20
#define D18F5x12C_RAZ_29_7_OFFSET                               7
#define D18F5x12C_RAZ_29_7_WIDTH                                23
#define D18F5x12C_RAZ_29_7_MASK                                 0x3FFFFF80
#define D18F5x12C_WaitVidCompDis_OFFSET                         30
#define D18F5x12C_WaitVidCompDis_WIDTH                          1
#define D18F5x12C_WaitVidCompDis_MASK                           0x40000000
#define D18F5x12C_Svi2CmdBusy_OFFSET                            31
#define D18F5x12C_Svi2CmdBusy_WIDTH                             1
#define D18F5x12C_Svi2CmdBusy_MASK                              0x80000000

/// D18F5x12C
typedef union {
  struct {                                                            ///<
    UINT32                                          CoreOffsetTrim:2; ///<
    UINT32                                        CoreLoadLineTrim:3; ///<
    UINT32                                              CorePsi1En:1; ///<
    UINT32                                                        :1; ///<
    UINT32                                                RAZ_29_7:23; ///<
    UINT32                                          WaitVidCompDis:1; ///<
    UINT32                                             Svi2CmdBusy:1; ///<

  } Field;

  UINT32 Value;
} D18F5x12C_STRUCT;

// **** D18F5x178 Register Definition ****
// Address
#define D18F5x178_ADDRESS                                       0x178
// Type
#define D18F5x178_TYPE                                          TYPE_D18F5

// Field Data
#define D18F5x178_Reserved_1_0_OFFSET                           0
#define D18F5x178_Reserved_1_0_WIDTH                            2
#define D18F5x178_Reserved_1_0_MASK                             0x3
#define D18F5x178_CstateFusionDis_OFFSET                        2
#define D18F5x178_CstateFusionDis_WIDTH                         1
#define D18F5x178_CstateFusionDis_MASK                          0x4
#define D18F5x178_CstateThreeWayHsEn_OFFSET                     3
#define D18F5x178_CstateThreeWayHsEn_WIDTH                      1
#define D18F5x178_CstateThreeWayHsEn_MASK                       0x8
#define D18F5x178_Reserved_17_4_OFFSET                          4
#define D18F5x178_Reserved_17_4_WIDTH                           14
#define D18F5x178_Reserved_17_4_MASK                            0x3FFF0
#define D18F5x178_CstateFusionHsDis_OFFSET                      18
#define D18F5x178_CstateFusionHsDis_WIDTH                       1
#define D18F5x178_CstateFusionHsDis_MASK                        0x40000
#define D18F5x178_SwGfxDis_OFFSET                               19
#define D18F5x178_SwGfxDis_WIDTH                                1
#define D18F5x178_SwGfxDis_MASK                                 0x80000
#define D18F5x178_Reserved_31_20_OFFSET                         20
#define D18F5x178_Reserved_31_20_WIDTH                          12
#define D18F5x178_Reserved_31_20_MASK                           0xFFF00000

/// D18F5x178
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_1_0:2; ///<
    UINT32                                         CstateFusionDis:1; ///<
    UINT32                                      CstateThreeWayHsEn:1; ///<
    UINT32                                           Reserved_17_4:14; ///<
    UINT32                                       CstateFusionHsDis:1; ///<
    UINT32                                                SwGfxDis:1; ///<
    UINT32                                          Reserved_31_20:12; ///<

  } Field;

  UINT32 Value;
} D18F5x178_STRUCT;

// **** D18F5x188 Register Definition ****
// Address
#define D18F5x188_ADDRESS                                       0x188
// Type
#define D18F5x188_TYPE                                          TYPE_D18F5

// Field Data
#define D18F5x188_NbOffsetTrim_OFFSET                           0
#define D18F5x188_NbOffsetTrim_WIDTH                            2
#define D18F5x188_NbOffsetTrim_MASK                             0x3
#define D18F5x188_NbLoadLineTrim_OFFSET                         2
#define D18F5x188_NbLoadLineTrim_WIDTH                          3
#define D18F5x188_NbLoadLineTrim_MASK                           0x1C
#define D18F5x188_NbPsi1_OFFSET                                 5
#define D18F5x188_NbPsi1_WIDTH                                  1
#define D18F5x188_NbPsi1_MASK                                   0x20
#define D18F5x188_RAZ_31_7_OFFSET                               7
#define D18F5x188_RAZ_31_7_WIDTH                                25
#define D18F5x188_RAZ_31_7_MASK                                 0xFFFFFF80

/// D18F5x188
typedef union {
  struct {                                                            ///<
    UINT32                                            NbOffsetTrim:2; ///<
    UINT32                                          NbLoadLineTrim:3; ///<
    UINT32                                                  NbPsi1:1; ///<
    UINT32                                                        :1; ///<
    UINT32                                                RAZ_31_7:25; ///<

  } Field;

  UINT32 Value;
} D18F5x188_STRUCT;

// **** D0F0x04 Register Definition ****
// Address
#define D0F0x04_ADDRESS                                         0x4
// Type
#define D0F0x04_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0x04_IoAccessEn_OFFSET                               0
#define D0F0x04_IoAccessEn_WIDTH                                1
#define D0F0x04_IoAccessEn_MASK                                 0x1
#define D0F0x04_MemAccessEn_OFFSET                              1
#define D0F0x04_MemAccessEn_WIDTH                               1
#define D0F0x04_MemAccessEn_MASK                                0x2
#define D0F0x04_BusMasterEn_OFFSET                              2
#define D0F0x04_BusMasterEn_WIDTH                               1
#define D0F0x04_BusMasterEn_MASK                                0x4
#define D0F0x04_Reserved_19_3_OFFSET                            3
#define D0F0x04_Reserved_19_3_WIDTH                             17
#define D0F0x04_Reserved_19_3_MASK                              0xFFFF8
#define D0F0x04_CapList_OFFSET                                  20
#define D0F0x04_CapList_WIDTH                                   1
#define D0F0x04_CapList_MASK                                    0x100000
#define D0F0x04_Reserved_31_21_OFFSET                           21
#define D0F0x04_Reserved_31_21_WIDTH                            11
#define D0F0x04_Reserved_31_21_MASK                             0xFFE00000

/// D0F0x04
typedef union {
  struct {                                                            ///<
    UINT32                                              IoAccessEn:1; ///<
    UINT32                                             MemAccessEn:1; ///<
    UINT32                                             BusMasterEn:1; ///<
    UINT32                                           Reserved_19_3:17; ///<
    UINT32                                                 CapList:1; ///<
    UINT32                                          Reserved_31_21:11; ///<

  } Field;

  UINT32 Value;
} D0F0x04_STRUCT;

// **** D0F0x64 Register Definition ****
// Address
#define D0F0x64_ADDRESS                                         0x64
// Type
#define D0F0x64_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0x64_MiscIndData_OFFSET                              0
#define D0F0x64_MiscIndData_WIDTH                               32
#define D0F0x64_MiscIndData_MASK                                0xFFFFFFFF

/// D0F0x64
typedef union {
  struct {                                                            ///<
    UINT32                                             MiscIndData:32; ///<

  } Field;

  UINT32 Value;
} D0F0x64_STRUCT;

// **** DxF0x3C Register Definition ****
// Address
#define DxF0x3C_ADDRESS                                         0x3c

// Type
#define DxF0x3C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x3C_IntLine_OFFSET                                  0
#define DxF0x3C_IntLine_WIDTH                                   8
#define DxF0x3C_IntLine_MASK                                    0xff
#define DxF0x3C_IntPin_OFFSET                                   8
#define DxF0x3C_IntPin_WIDTH                                    3
#define DxF0x3C_IntPin_MASK                                     0x700
#define DxF0x3C_IntPinR_OFFSET                                  11
#define DxF0x3C_IntPinR_WIDTH                                   5
#define DxF0x3C_IntPinR_MASK                                    0xf800
#define DxF0x3C_ParityResponseEn_OFFSET                         16
#define DxF0x3C_ParityResponseEn_WIDTH                          1
#define DxF0x3C_ParityResponseEn_MASK                           0x10000
#define DxF0x3C_SerrEn_OFFSET                                   17
#define DxF0x3C_SerrEn_WIDTH                                    1
#define DxF0x3C_SerrEn_MASK                                     0x20000
#define DxF0x3C_IsaEn_OFFSET                                    18
#define DxF0x3C_IsaEn_WIDTH                                     1
#define DxF0x3C_IsaEn_MASK                                      0x40000
#define DxF0x3C_VgaEn_OFFSET                                    19
#define DxF0x3C_VgaEn_WIDTH                                     1
#define DxF0x3C_VgaEn_MASK                                      0x80000
#define DxF0x3C_Vga16En_OFFSET                                  20
#define DxF0x3C_Vga16En_WIDTH                                   1
#define DxF0x3C_Vga16En_MASK                                    0x100000
#define DxF0x3C_MasterAbortMode_OFFSET                          21
#define DxF0x3C_MasterAbortMode_WIDTH                           1
#define DxF0x3C_MasterAbortMode_MASK                            0x200000
#define DxF0x3C_SecondaryBusReset_OFFSET                        22
#define DxF0x3C_SecondaryBusReset_WIDTH                         1
#define DxF0x3C_SecondaryBusReset_MASK                          0x400000
#define DxF0x3C_FastB2BCap_OFFSET                               23
#define DxF0x3C_FastB2BCap_WIDTH                                1
#define DxF0x3C_FastB2BCap_MASK                                 0x800000
#define DxF0x3C_Reserved_31_24_OFFSET                           24
#define DxF0x3C_Reserved_31_24_WIDTH                            8
#define DxF0x3C_Reserved_31_24_MASK                             0xff000000

/// DxF0x3C
typedef union {
  struct {                                                              ///<
    UINT32                                                  IntLine:8 ; ///<
    UINT32                                                   IntPin:3 ; ///<
    UINT32                                                  IntPinR:5 ; ///<
    UINT32                                         ParityResponseEn:1 ; ///<
    UINT32                                                   SerrEn:1 ; ///<
    UINT32                                                    IsaEn:1 ; ///<
    UINT32                                                    VgaEn:1 ; ///<
    UINT32                                                  Vga16En:1 ; ///<
    UINT32                                          MasterAbortMode:1 ; ///<
    UINT32                                        SecondaryBusReset:1 ; ///<
    UINT32                                               FastB2BCap:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x3C_STRUCT;

// **** DxF0x58 Register Definition ****
// Address
#define DxF0x58_ADDRESS                                         0x58

// Type
#define DxF0x58_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x58_CapID_OFFSET                                    0
#define DxF0x58_CapID_WIDTH                                     8
#define DxF0x58_CapID_MASK                                      0xff
#define DxF0x58_NextPtr_OFFSET                                  8
#define DxF0x58_NextPtr_WIDTH                                   8
#define DxF0x58_NextPtr_MASK                                    0xff00
#define DxF0x58_Version_OFFSET                                  16
#define DxF0x58_Version_WIDTH                                   4
#define DxF0x58_Version_MASK                                    0xf0000
#define DxF0x58_DeviceType_OFFSET                               20
#define DxF0x58_DeviceType_WIDTH                                4
#define DxF0x58_DeviceType_MASK                                 0xf00000
#define DxF0x58_SlotImplemented_OFFSET                          24
#define DxF0x58_SlotImplemented_WIDTH                           1
#define DxF0x58_SlotImplemented_MASK                            0x1000000
#define DxF0x58_IntMessageNum_OFFSET                            25
#define DxF0x58_IntMessageNum_WIDTH                             5
#define DxF0x58_IntMessageNum_MASK                              0x3e000000
#define DxF0x58_Reserved_31_30_OFFSET                           30
#define DxF0x58_Reserved_31_30_WIDTH                            2
#define DxF0x58_Reserved_31_30_MASK                             0xc0000000

/// DxF0x58
typedef union {
  struct {                                                              ///<
    UINT32                                                    CapID:8 ; ///<
    UINT32                                                  NextPtr:8 ; ///<
    UINT32                                                  Version:4 ; ///<
    UINT32                                               DeviceType:4 ; ///<
    UINT32                                          SlotImplemented:1 ; ///<
    UINT32                                            IntMessageNum:5 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x58_STRUCT;

// **** DxFxx68 Register Definition ****
// Address
#define DxFxx68_ADDRESS                                         0x68
// Type
#define DxFxx68_TYPE                                            TYPE_D2F1

// Field Data
#define DxFxx68_PmControl_OFFSET                                0
#define DxFxx68_PmControl_WIDTH                                 2
#define DxFxx68_PmControl_MASK                                  0x3
#define DxFxx68_Reserved_2_2_OFFSET                             2
#define DxFxx68_Reserved_2_2_WIDTH                              1
#define DxFxx68_Reserved_2_2_MASK                               0x4
#define DxFxx68_ReadCplBoundary_OFFSET                          3
#define DxFxx68_ReadCplBoundary_WIDTH                           1
#define DxFxx68_ReadCplBoundary_MASK                            0x8
#define DxFxx68_LinkDis_OFFSET                                  4
#define DxFxx68_LinkDis_WIDTH                                   1
#define DxFxx68_LinkDis_MASK                                    0x10
#define DxFxx68_RetrainLink_OFFSET                              5
#define DxFxx68_RetrainLink_WIDTH                               1
#define DxFxx68_RetrainLink_MASK                                0x20
#define DxFxx68_CommonClockCfg_OFFSET                           6
#define DxFxx68_CommonClockCfg_WIDTH                            1
#define DxFxx68_CommonClockCfg_MASK                             0x40
#define DxFxx68_ExtendedSync_OFFSET                             7
#define DxFxx68_ExtendedSync_WIDTH                              1
#define DxFxx68_ExtendedSync_MASK                               0x80
#define DxFxx68_ClockPowerManagementEn_OFFSET                   8
#define DxFxx68_ClockPowerManagementEn_WIDTH                    1
#define DxFxx68_ClockPowerManagementEn_MASK                     0x100
#define DxFxx68_HWAutonomousWidthDisable_OFFSET                 9
#define DxFxx68_HWAutonomousWidthDisable_WIDTH                  1
#define DxFxx68_HWAutonomousWidthDisable_MASK                   0x200
#define DxFxx68_LinkBWManagementEn_OFFSET                       10
#define DxFxx68_LinkBWManagementEn_WIDTH                        1
#define DxFxx68_LinkBWManagementEn_MASK                         0x400
#define DxFxx68_LinkAutonomousBWIntEn_OFFSET                    11
#define DxFxx68_LinkAutonomousBWIntEn_WIDTH                     1
#define DxFxx68_LinkAutonomousBWIntEn_MASK                      0x800
#define DxFxx68_Reserved_15_12_OFFSET                           12
#define DxFxx68_Reserved_15_12_WIDTH                            4
#define DxFxx68_Reserved_15_12_MASK                             0xF000
#define DxFxx68_LinkSpeed_OFFSET                                16
#define DxFxx68_LinkSpeed_WIDTH                                 4
#define DxFxx68_LinkSpeed_MASK                                  0xF0000
#define DxFxx68_NegotiatedLinkWidth_OFFSET                      20
#define DxFxx68_NegotiatedLinkWidth_WIDTH                       6
#define DxFxx68_NegotiatedLinkWidth_MASK                        0x3F00000
#define DxFxx68_Reserved_26_26_OFFSET                           26
#define DxFxx68_Reserved_26_26_WIDTH                            1
#define DxFxx68_Reserved_26_26_MASK                             0x4000000
#define DxFxx68_LinkTraining_OFFSET                             27
#define DxFxx68_LinkTraining_WIDTH                              1
#define DxFxx68_LinkTraining_MASK                               0x8000000
#define DxFxx68_SlotClockCfg_OFFSET                             28
#define DxFxx68_SlotClockCfg_WIDTH                              1
#define DxFxx68_SlotClockCfg_MASK                               0x10000000
#define DxFxx68_DlActive_OFFSET                                 29
#define DxFxx68_DlActive_WIDTH                                  1
#define DxFxx68_DlActive_MASK                                   0x20000000
#define DxFxx68_LinkBWManagementStatus_OFFSET                   30
#define DxFxx68_LinkBWManagementStatus_WIDTH                    1
#define DxFxx68_LinkBWManagementStatus_MASK                     0x40000000
#define DxFxx68_LinkAutonomousBWStatus_OFFSET                   31
#define DxFxx68_LinkAutonomousBWStatus_WIDTH                    1
#define DxFxx68_LinkAutonomousBWStatus_MASK                     0x80000000

/// DxFxx68
typedef union {
  struct {                                                            ///<
    UINT32                                               PmControl:2; ///<
    UINT32                                            Reserved_2_2:1; ///<
    UINT32                                         ReadCplBoundary:1; ///<
    UINT32                                                 LinkDis:1; ///<
    UINT32                                             RetrainLink:1; ///<
    UINT32                                          CommonClockCfg:1; ///<
    UINT32                                            ExtendedSync:1; ///<
    UINT32                                  ClockPowerManagementEn:1; ///<
    UINT32                                HWAutonomousWidthDisable:1; ///<
    UINT32                                      LinkBWManagementEn:1; ///<
    UINT32                                   LinkAutonomousBWIntEn:1; ///<
    UINT32                                          Reserved_15_12:4; ///<
    UINT32                                               LinkSpeed:4; ///<
    UINT32                                     NegotiatedLinkWidth:6; ///<
    UINT32                                          Reserved_26_26:1; ///<
    UINT32                                            LinkTraining:1; ///<
    UINT32                                            SlotClockCfg:1; ///<
    UINT32                                                DlActive:1; ///<
    UINT32                                  LinkBWManagementStatus:1; ///<
    UINT32                                  LinkAutonomousBWStatus:1; ///<

  } Field;

  UINT32 Value;
} DxFxx68_STRUCT;

// **** D0F0x7C Register Definition ****
// Address
#define D0F0x7C_ADDRESS                                         0x7C
// Type
#define D0F0x7C_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0x7C_ForceIntGfxDisable_OFFSET                       0
#define D0F0x7C_ForceIntGfxDisable_WIDTH                        1
#define D0F0x7C_ForceIntGfxDisable_MASK                         0x1
#define D0F0x7C_Reserved_31_1_OFFSET                            1
#define D0F0x7C_Reserved_31_1_WIDTH                             31
#define D0F0x7C_Reserved_31_1_MASK                              0xFFFFFFFE

/// D0F0x7C
typedef union {
  struct {                                                            ///<
    UINT32                                      ForceIntGfxDisable:1; ///<
    UINT32                                           Reserved_31_1:31; ///<

  } Field;

  UINT32 Value;
} D0F0x7C_STRUCT;

// **** D0F0x98 Register Definition ****
// Address
#define D0F0x98_ADDRESS                                         0x98
// Type
#define D0F0x98_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0x98_OrbIndData_OFFSET                               0
#define D0F0x98_OrbIndData_WIDTH                                32
#define D0F0x98_OrbIndData_MASK                                 0xFFFFFFFF

/// D0F0x98
typedef union {
  struct {                                                            ///<
    UINT32                                              OrbIndData:32; ///<

  } Field;

  UINT32 Value;
} D0F0x98_STRUCT;

// **** D0F0xBC Register Definition ****
// Address
#define D0F0xBC_ADDRESS                                         0xBC
// Type
#define D0F0xBC_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0xBC_NbSmuIndData_OFFSET                             0
#define D0F0xBC_NbSmuIndData_WIDTH                              32
#define D0F0xBC_NbSmuIndData_MASK                               0xFFFFFFFF

/// D0F0xBC
typedef union {
  struct {                                                            ///<
    UINT32                                            NbSmuIndData:32; ///<

  } Field;

  UINT32 Value;
} D0F0xBC_STRUCT;

// **** D0F0xC8 Register Definition ****
// Address
#define D0F0xC8_ADDRESS                                         0xC8
// Type
#define D0F0xC8_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0xC8_NbDevIndAddr_OFFSET                             0
#define D0F0xC8_NbDevIndAddr_WIDTH                              7
#define D0F0xC8_NbDevIndAddr_MASK                               0x7F
#define D0F0xC8_Reserved_15_7_OFFSET                            7
#define D0F0xC8_Reserved_15_7_WIDTH                             9
#define D0F0xC8_Reserved_15_7_MASK                              0xFF80
#define D0F0xC8_NbDevIndSel_OFFSET                              16
#define D0F0xC8_NbDevIndSel_WIDTH                               8
#define D0F0xC8_NbDevIndSel_MASK                                0xFF0000
#define D0F0xC8_Reserved_31_24_OFFSET                           24
#define D0F0xC8_Reserved_31_24_WIDTH                            8
#define D0F0xC8_Reserved_31_24_MASK                             0xFF000000

/// D0F0xC8
typedef union {
  struct {                                                            ///<
    UINT32                                            NbDevIndAddr:7; ///<
    UINT32                                           Reserved_15_7:9; ///<
    UINT32                                             NbDevIndSel:8; ///<
    UINT32                                          Reserved_31_24:8; ///<

  } Field;

  UINT32 Value;
} D0F0xC8_STRUCT;

// **** D0F0xCC Register Definition ****
// Address
#define D0F0xCC_ADDRESS                                         0xCC
// Type
#define D0F0xCC_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0xCC_NbDevIndData_OFFSET                             0
#define D0F0xCC_NbDevIndData_WIDTH                              32
#define D0F0xCC_NbDevIndData_MASK                               0xFFFFFFFF

/// D0F0xCC
typedef union {
  struct {                                                            ///<
    UINT32                                            NbDevIndData:32; ///<

  } Field;

  UINT32 Value;
} D0F0xCC_STRUCT;

// **** D0F0xE4 Register Definition ****
// Address
#define D0F0xE4_ADDRESS                                         0xE4
// Type
#define D0F0xE4_TYPE                                            TYPE_D0F0

// Field Data
#define D0F0xE4_PcieIndxData_OFFSET                             0
#define D0F0xE4_PcieIndxData_WIDTH                              32
#define D0F0xE4_PcieIndxData_MASK                               0xFFFFFFFF

/// D0F0xE4
typedef union {
  struct {                                                            ///<
    UINT32                                            PcieIndxData:32; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_STRUCT;

// **** DxFxxE4_x6A Register Definition ****
// Address
#define DxFxxE4_x6A_ADDRESS                                     0x6A
// Type
#define DxFxxE4_x6A_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_x6A_ErrReportingDis_OFFSET                          0
#define DxFxxE4_x6A_ErrReportingDis_WIDTH                           1
#define DxFxxE4_x6A_ErrReportingDis_MASK                            0x1

// **** DxFxxE4_xA2 Register Definition ****
// Address
#define DxFxxE4_xA2_ADDRESS                                     0xA2
// Type
#define DxFxxE4_xA2_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_xA2_LcLinkWidth_OFFSET                          0
#define DxFxxE4_xA2_LcLinkWidth_WIDTH                           3
#define DxFxxE4_xA2_LcLinkWidth_MASK                            0x7
#define DxFxxE4_xA2_Reserved_3_3_OFFSET                         3
#define DxFxxE4_xA2_Reserved_3_3_WIDTH                          1
#define DxFxxE4_xA2_Reserved_3_3_MASK                           0x8
#define DxFxxE4_xA2_LcLinkWidthRd_OFFSET                        4
#define DxFxxE4_xA2_LcLinkWidthRd_WIDTH                         3
#define DxFxxE4_xA2_LcLinkWidthRd_MASK                          0x70
#define DxFxxE4_xA2_LcReconfigArcMissingEscape_OFFSET           7
#define DxFxxE4_xA2_LcReconfigArcMissingEscape_WIDTH            1
#define DxFxxE4_xA2_LcReconfigArcMissingEscape_MASK             0x80
#define DxFxxE4_xA2_LcReconfigNow_OFFSET                        8
#define DxFxxE4_xA2_LcReconfigNow_WIDTH                         1
#define DxFxxE4_xA2_LcReconfigNow_MASK                          0x100
#define DxFxxE4_xA2_LcRenegotiationSupport_OFFSET               9
#define DxFxxE4_xA2_LcRenegotiationSupport_WIDTH                1
#define DxFxxE4_xA2_LcRenegotiationSupport_MASK                 0x200
#define DxFxxE4_xA2_LcRenegotiateEn_OFFSET                      10
#define DxFxxE4_xA2_LcRenegotiateEn_WIDTH                       1
#define DxFxxE4_xA2_LcRenegotiateEn_MASK                        0x400
#define DxFxxE4_xA2_LcShortReconfigEn_OFFSET                    11
#define DxFxxE4_xA2_LcShortReconfigEn_WIDTH                     1
#define DxFxxE4_xA2_LcShortReconfigEn_MASK                      0x800
#define DxFxxE4_xA2_LcUpconfigureSupport_OFFSET                 12
#define DxFxxE4_xA2_LcUpconfigureSupport_WIDTH                  1
#define DxFxxE4_xA2_LcUpconfigureSupport_MASK                   0x1000
#define DxFxxE4_xA2_LcUpconfigureDis_OFFSET                     13
#define DxFxxE4_xA2_LcUpconfigureDis_WIDTH                      1
#define DxFxxE4_xA2_LcUpconfigureDis_MASK                       0x2000
#define DxFxxE4_xA2_Reserved_19_14_OFFSET                       14
#define DxFxxE4_xA2_Reserved_19_14_WIDTH                        6
#define DxFxxE4_xA2_Reserved_19_14_MASK                         0xFC000
#define DxFxxE4_xA2_LcUpconfigCapable_OFFSET                    20
#define DxFxxE4_xA2_LcUpconfigCapable_WIDTH                     1
#define DxFxxE4_xA2_LcUpconfigCapable_MASK                      0x100000
#define DxFxxE4_xA2_LcDynLanesPwrState_OFFSET                   21
#define DxFxxE4_xA2_LcDynLanesPwrState_WIDTH                    2
#define DxFxxE4_xA2_LcDynLanesPwrState_MASK                     0x600000
#define DxFxxE4_xA2_Reserved_31_23_OFFSET                       23
#define DxFxxE4_xA2_Reserved_31_23_WIDTH                        9
#define DxFxxE4_xA2_Reserved_31_23_MASK                         0xFF800000

/// DxFxxE4_xA2
typedef union {
  struct {                                                            ///<
    UINT32                                             LcLinkWidth:3; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                           LcLinkWidthRd:3; ///<
    UINT32                              LcReconfigArcMissingEscape:1; ///<
    UINT32                                           LcReconfigNow:1; ///<
    UINT32                                  LcRenegotiationSupport:1; ///<
    UINT32                                         LcRenegotiateEn:1; ///<
    UINT32                                       LcShortReconfigEn:1; ///<
    UINT32                                    LcUpconfigureSupport:1; ///<
    UINT32                                        LcUpconfigureDis:1; ///<
    UINT32                                          Reserved_19_14:6; ///<
    UINT32                                       LcUpconfigCapable:1; ///<
    UINT32                                      LcDynLanesPwrState:2; ///<
    UINT32                                          Reserved_31_23:9; ///<

  } Field;

  UINT32 Value;
} DxFxxE4_xA2_STRUCT;

// **** DxFxx128 Register Definition ****
// Address
#define DxFxx128_ADDRESS                                        0x128
// Type
#define DxFxx128_TYPE                                           TYPE_D2F1

// Field Data
#define DxFxx128_Reserved_15_0_OFFSET                           0
#define DxFxx128_Reserved_15_0_WIDTH                            16
#define DxFxx128_Reserved_15_0_MASK                             0xFFFF
#define DxFxx128_PortArbTableStatus_OFFSET                      16
#define DxFxx128_PortArbTableStatus_WIDTH                       1
#define DxFxx128_PortArbTableStatus_MASK                        0x10000
#define DxFxx128_VcNegotiationPending_OFFSET                    17
#define DxFxx128_VcNegotiationPending_WIDTH                     1
#define DxFxx128_VcNegotiationPending_MASK                      0x20000
#define DxFxx128_Reserved_31_18_OFFSET                          18
#define DxFxx128_Reserved_31_18_WIDTH                           14
#define DxFxx128_Reserved_31_18_MASK                            0xFFFC0000

/// DxFxx128
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_15_0:16; ///<
    UINT32                                      PortArbTableStatus:1; ///<
    UINT32                                    VcNegotiationPending:1; ///<
    UINT32                                          Reserved_31_18:14; ///<

  } Field;

  UINT32 Value;
} DxFxx128_STRUCT;

// **** D0F0x64_x0D Register Definition ****
// Address
#define D0F0x64_x0D_ADDRESS                                     0xD
// Type
#define D0F0x64_x0D_TYPE                                        TYPE_D0F0x64

// Field Data
#define D0F0x64_x0D_PciDev0Fn2RegEn_OFFSET                      0
#define D0F0x64_x0D_PciDev0Fn2RegEn_WIDTH                       1
#define D0F0x64_x0D_PciDev0Fn2RegEn_MASK                        0x1
#define D0F0x64_x0D_Reserved_30_1_OFFSET                        1
#define D0F0x64_x0D_Reserved_30_1_WIDTH                         30
#define D0F0x64_x0D_Reserved_30_1_MASK                          0x7FFFFFFE
#define D0F0x64_x0D_IommuDis_OFFSET                             31
#define D0F0x64_x0D_IommuDis_WIDTH                              1
#define D0F0x64_x0D_IommuDis_MASK                               0x80000000

/// D0F0x64_x0D
typedef union {
  struct {                                                            ///<
    UINT32                                         PciDev0Fn2RegEn:1; ///<
    UINT32                                           Reserved_30_1:30; ///<
    UINT32                                                IommuDis:1; ///<

  } Field;

  UINT32 Value;
} D0F0x64_x0D_STRUCT;

// **** D0F0x64_x16 Register Definition ****
// Address
#define D0F0x64_x16_ADDRESS                                     0x16

// Type
#define D0F0x64_x16_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x16_AerUrMsgEn_OFFSET                           0
#define D0F0x64_x16_AerUrMsgEn_WIDTH                            1
#define D0F0x64_x16_AerUrMsgEn_MASK                             0x1
#define D0F0x64_x16_Reserved_31_1_OFFSET                        1
#define D0F0x64_x16_Reserved_31_1_WIDTH                         31
#define D0F0x64_x16_Reserved_31_1_MASK                          0xfffffffe

/// D0F0x64_x16
typedef union {
  struct {                                                              ///<
    UINT32                                               AerUrMsgEn:1 ; ///<
    UINT32                                            Reserved_31_1:31; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x16_STRUCT;

// **** D0F0x64_x1D Register Definition ****
// Address
#define D0F0x64_x1D_ADDRESS                                     0x1D
// Type
#define D0F0x64_x1D_TYPE                                        TYPE_D0F0x64

// Field Data
#define D0F0x64_x1D_Reserved_0_0_OFFSET                         0
#define D0F0x64_x1D_Reserved_0_0_WIDTH                          1
#define D0F0x64_x1D_Reserved_0_0_MASK                           0x1
#define D0F0x64_x1D_VgaEn_OFFSET                                1
#define D0F0x64_x1D_VgaEn_WIDTH                                 1
#define D0F0x64_x1D_VgaEn_MASK                                  0x2
#define D0F0x64_x1D_Reserved_2_2_OFFSET                         2
#define D0F0x64_x1D_Reserved_2_2_WIDTH                          1
#define D0F0x64_x1D_Reserved_2_2_MASK                           0x4
#define D0F0x64_x1D_Vga16En_OFFSET                              3
#define D0F0x64_x1D_Vga16En_WIDTH                               1
#define D0F0x64_x1D_Vga16En_MASK                                0x8
#define D0F0x64_x1D_Reserved_31_4_OFFSET                        4
#define D0F0x64_x1D_Reserved_31_4_WIDTH                         28
#define D0F0x64_x1D_Reserved_31_4_MASK                          0xFFFFFFF0

/// D0F0x64_x1D
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_0_0:1; ///<
    UINT32                                                   VgaEn:1; ///<
    UINT32                                            Reserved_2_2:1; ///<
    UINT32                                                 Vga16En:1; ///<
    UINT32                                           Reserved_31_4:28; ///<

  } Field;

  UINT32 Value;
} D0F0x64_x1D_STRUCT;

// **** D0F0x64_x22 Register Definition ****
// Address
#define D0F0x64_x22_ADDRESS                                     0x22
// Type
#define D0F0x64_x22_TYPE                                        TYPE_D0F0x64


// **** D0F0x64_x23 Register Definition ****
// Address
#define D0F0x64_x23_ADDRESS                                     0x23
// Type
#define D0F0x64_x23_TYPE                                        TYPE_D0F0x64


// **** D0F0x64_x30 Register Definition ****
// Address
#define D0F0x64_x30_ADDRESS                                     0x30
// Type
#define D0F0x64_x30_TYPE                                        TYPE_D0F0x64

// Field Data
#define D0F0x64_x30_DevFnMap_OFFSET                             0
#define D0F0x64_x30_DevFnMap_WIDTH                              8
#define D0F0x64_x30_DevFnMap_MASK                               0xFF
#define D0F0x64_x30_Reserved_31_8_OFFSET                        8
#define D0F0x64_x30_Reserved_31_8_WIDTH                         24
#define D0F0x64_x30_Reserved_31_8_MASK                          0xFFFFFF00

/// D0F0x64_x30
typedef union {
  struct {                                                            ///<
    UINT32                                                DevFnMap:8; ///<
    UINT32                                           Reserved_31_8:24; ///<

  } Field;

  UINT32 Value;
} D0F0x64_x30_STRUCT;

// **** D0F0x64_x46 Register Definition ****
// Address
#define D0F0x64_x46_ADDRESS                                     0x46
// Type
#define D0F0x64_x46_TYPE                                        TYPE_D0F0x64


// **** D0F0x94 Register Definition ****
// Address
#define D0F0x94_ADDRESS                                         0x94

// Type
#define D0F0x94_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x94_OrbIndAddr_OFFSET                               0
#define D0F0x94_OrbIndAddr_WIDTH                                7
#define D0F0x94_OrbIndAddr_MASK                                 0x7f
#define D0F0x94_Reserved_7_7_OFFSET                             7
#define D0F0x94_Reserved_7_7_WIDTH                              1
#define D0F0x94_Reserved_7_7_MASK                               0x80
#define D0F0x94_Reserved_31_9_OFFSET                            9
#define D0F0x94_Reserved_31_9_WIDTH                             23
#define D0F0x94_Reserved_31_9_MASK                              0xfffffe00

/// D0F0x94
typedef union {
  struct {                                                              ///<
    UINT32                                               OrbIndAddr:7 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x94_STRUCT;

// **** D0F0x98_x07 Register Definition ****
// Address
#define D0F0x98_x07_ADDRESS                                     0x7
// Type
#define D0F0x98_x07_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x07_IocBwOptEn_OFFSET                           0
#define D0F0x98_x07_IocBwOptEn_WIDTH                            1
#define D0F0x98_x07_IocBwOptEn_MASK                             0x1
#define D0F0x98_x07_IocWrROMapDis_OFFSET                        1
#define D0F0x98_x07_IocWrROMapDis_WIDTH                         1
#define D0F0x98_x07_IocWrROMapDis_MASK                          0x2
#define D0F0x98_x07_IocRdROMapDis_OFFSET                        2
#define D0F0x98_x07_IocRdROMapDis_WIDTH                         1
#define D0F0x98_x07_IocRdROMapDis_MASK                          0x4
#define D0F0x98_x07_Reserved_3_3_OFFSET                         3
#define D0F0x98_x07_Reserved_3_3_WIDTH                          1
#define D0F0x98_x07_Reserved_3_3_MASK                           0x8
#define D0F0x98_x07_IommuBwOptEn_OFFSET                         4
#define D0F0x98_x07_IommuBwOptEn_WIDTH                          1
#define D0F0x98_x07_IommuBwOptEn_MASK                           0x10
#define D0F0x98_x07_Reserved_5_5_OFFSET                         5
#define D0F0x98_x07_Reserved_5_5_WIDTH                          1
#define D0F0x98_x07_Reserved_5_5_MASK                           0x20
#define D0F0x98_x07_DmaReqRespPassPWMode_OFFSET                 6
#define D0F0x98_x07_DmaReqRespPassPWMode_WIDTH                  1
#define D0F0x98_x07_DmaReqRespPassPWMode_MASK                   0x40
#define D0F0x98_x07_IommuIsocPassPWMode_OFFSET                  7
#define D0F0x98_x07_IommuIsocPassPWMode_WIDTH                   1
#define D0F0x98_x07_IommuIsocPassPWMode_MASK                    0x80
#define D0F0x98_x07_Reserved_14_8_OFFSET                        8
#define D0F0x98_x07_Reserved_14_8_WIDTH                         7
#define D0F0x98_x07_Reserved_14_8_MASK                          0x7F00
#define D0F0x98_x07_DropZeroMaskWrEn_OFFSET                     15
#define D0F0x98_x07_DropZeroMaskWrEn_WIDTH                      1
#define D0F0x98_x07_DropZeroMaskWrEn_MASK                       0x8000
#define D0F0x98_x07_SyncFloodOnParityErr_OFFSET                 16
#define D0F0x98_x07_SyncFloodOnParityErr_WIDTH                  1
#define D0F0x98_x07_SyncFloodOnParityErr_MASK                   0x10000
#define D0F0x98_x07_Reserved_30_17_OFFSET                       17
#define D0F0x98_x07_Reserved_30_17_WIDTH                        14
#define D0F0x98_x07_Reserved_30_17_MASK                         0x7FFE0000
#define D0F0x98_x07_SMUCsrIsocEn_OFFSET                         31
#define D0F0x98_x07_SMUCsrIsocEn_WIDTH                          1
#define D0F0x98_x07_SMUCsrIsocEn_MASK                           0x80000000

/// D0F0x98_x07
typedef union {
  struct {                                                            ///<
    UINT32                                              IocBwOptEn:1; ///<
    UINT32                                           IocWrROMapDis:1; ///<
    UINT32                                           IocRdROMapDis:1; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                            IommuBwOptEn:1; ///<
    UINT32                                            Reserved_5_5:1; ///<
    UINT32                                    DmaReqRespPassPWMode:1; ///<
    UINT32                                     IommuIsocPassPWMode:1; ///<
    UINT32                                           Reserved_14_8:7; ///<
    UINT32                                        DropZeroMaskWrEn:1; ///<
    UINT32                                    SyncFloodOnParityErr:1; ///<
    UINT32                                          Reserved_30_17:14; ///<
    UINT32                                            SMUCsrIsocEn:1; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x07_STRUCT;

// **** D0F0x98_x08 Register Definition ****
// Address
#define D0F0x98_x08_ADDRESS                                     0x8
// Type
#define D0F0x98_x08_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x08_NpWrrLenA_OFFSET                            0
#define D0F0x98_x08_NpWrrLenA_WIDTH                             8
#define D0F0x98_x08_NpWrrLenA_MASK                              0xFF
#define D0F0x98_x08_NpWrrLenB_OFFSET                            8
#define D0F0x98_x08_NpWrrLenB_WIDTH                             8
#define D0F0x98_x08_NpWrrLenB_MASK                              0xFF00
#define D0F0x98_x08_NpWrrLenC_OFFSET                            16
#define D0F0x98_x08_NpWrrLenC_WIDTH                             8
#define D0F0x98_x08_NpWrrLenC_MASK                              0xFF0000
#define D0F0x98_x08_Reserved_31_24_OFFSET                       24
#define D0F0x98_x08_Reserved_31_24_WIDTH                        8
#define D0F0x98_x08_Reserved_31_24_MASK                         0xFF000000

/// D0F0x98_x08
typedef union {
  struct {                                                            ///<
    UINT32                                               NpWrrLenA:8; ///<
    UINT32                                               NpWrrLenB:8; ///<
    UINT32                                               NpWrrLenC:8; ///<
    UINT32                                          Reserved_31_24:8; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x08_STRUCT;

// **** D0F0x98_x0C Register Definition ****
// Address
#define D0F0x98_x0C_ADDRESS                                     0xC
// Type
#define D0F0x98_x0C_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x0C_GcmWrrLenA_OFFSET                           0
#define D0F0x98_x0C_GcmWrrLenA_WIDTH                            8
#define D0F0x98_x0C_GcmWrrLenA_MASK                             0xFF
#define D0F0x98_x0C_GcmWrrLenB_OFFSET                           8
#define D0F0x98_x0C_GcmWrrLenB_WIDTH                            8
#define D0F0x98_x0C_GcmWrrLenB_MASK                             0xFF00
#define D0F0x98_x0C_Reserved_29_16_OFFSET                       16
#define D0F0x98_x0C_Reserved_29_16_WIDTH                        14
#define D0F0x98_x0C_Reserved_29_16_MASK                         0x3FFF0000
#define D0F0x98_x0C_Reserved_31_31_OFFSET                       31
#define D0F0x98_x0C_Reserved_31_31_WIDTH                        1
#define D0F0x98_x0C_Reserved_31_31_MASK                         0x80000000

/// D0F0x98_x0C
typedef union {
  struct {                                                            ///<
    UINT32                                              GcmWrrLenA:8; ///<
    UINT32                                              GcmWrrLenB:8; ///<
    UINT32                                          Reserved_29_16:14; ///<
    UINT32                                          Reserved_30_30:1; ///<
    UINT32                                          Reserved_31_31:1; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x0C_STRUCT;

// **** D0F0x98_x1E Register Definition ****
// Address
#define D0F0x98_x1E_ADDRESS                                     0x1E
// Type
#define D0F0x98_x1E_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x1E_Reserved_0_0_OFFSET                         0
#define D0F0x98_x1E_Reserved_0_0_WIDTH                          1
#define D0F0x98_x1E_Reserved_0_0_MASK                           0x1
#define D0F0x98_x1E_HiPriEn_OFFSET                              1
#define D0F0x98_x1E_HiPriEn_WIDTH                               1
#define D0F0x98_x1E_HiPriEn_MASK                                0x2
#define D0F0x98_x1E_Reserved_23_2_OFFSET                        2
#define D0F0x98_x1E_Reserved_23_2_WIDTH                         22
#define D0F0x98_x1E_Reserved_23_2_MASK                          0xFFFFFC
#define D0F0x98_x1E_RxErrStatusDelay_OFFSET                     24
#define D0F0x98_x1E_RxErrStatusDelay_WIDTH                      8
#define D0F0x98_x1E_RxErrStatusDelay_MASK                       0xFF000000

/// D0F0x98_x1E
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_0_0:1; ///<
    UINT32                                                 HiPriEn:1; ///<
    UINT32                                           Reserved_23_2:22; ///<
    UINT32                                        RxErrStatusDelay:8; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x1E_STRUCT;

// **** D0F0x98_x28 Register Definition ****
// Address
#define D0F0x98_x28_ADDRESS                                     0x28
// Type
#define D0F0x98_x28_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x28_Reserved_0_0_OFFSET                         0
#define D0F0x98_x28_Reserved_0_0_WIDTH                          1
#define D0F0x98_x28_Reserved_0_0_MASK                           0x1
#define D0F0x98_x28_ForceCoherentIntr_OFFSET                    1
#define D0F0x98_x28_ForceCoherentIntr_WIDTH                     1
#define D0F0x98_x28_ForceCoherentIntr_MASK                      0x2
#define D0F0x98_x28_Reserved_31_2_OFFSET                        2
#define D0F0x98_x28_Reserved_31_2_WIDTH                         30
#define D0F0x98_x28_Reserved_31_2_MASK                          0xFFFFFFFC

/// D0F0x98_x28
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_0_0:1; ///<
    UINT32                                       ForceCoherentIntr:1; ///<
    UINT32                                           Reserved_31_2:30; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x28_STRUCT;

// **** D0F0x98_x2C Register Definition ****
// Address
#define D0F0x98_x2C_ADDRESS                                     0x2C
// Type
#define D0F0x98_x2C_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x2C_Reserved_0_0_OFFSET                         0
#define D0F0x98_x2C_Reserved_0_0_WIDTH                          1
#define D0F0x98_x2C_Reserved_0_0_MASK                           0x1
#define D0F0x98_x2C_DynWakeEn_OFFSET                            1
#define D0F0x98_x2C_DynWakeEn_WIDTH                             1
#define D0F0x98_x2C_DynWakeEn_MASK                              0x2
#define D0F0x98_x2C_CgttLclkOverride_OFFSET                     2
#define D0F0x98_x2C_CgttLclkOverride_WIDTH                      1
#define D0F0x98_x2C_CgttLclkOverride_MASK                       0x4
#define D0F0x98_x2C_Reserved_8_3_OFFSET                         3
#define D0F0x98_x2C_Reserved_8_3_WIDTH                          6
#define D0F0x98_x2C_Reserved_8_3_MASK                           0x1F8
#define D0F0x98_x2C_SBDmaActiveMask_OFFSET                      9
#define D0F0x98_x2C_SBDmaActiveMask_WIDTH                       1
#define D0F0x98_x2C_SBDmaActiveMask_MASK                        0x200
#define D0F0x98_x2C_Reserved_15_10_OFFSET                       10
#define D0F0x98_x2C_Reserved_15_10_WIDTH                        6
#define D0F0x98_x2C_Reserved_15_10_MASK                         0xFC00
#define D0F0x98_x2C_WakeHysteresis_OFFSET                       16
#define D0F0x98_x2C_WakeHysteresis_WIDTH                        16
#define D0F0x98_x2C_WakeHysteresis_MASK                         0xFFFF0000

/// D0F0x98_x2C
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_0_0:1; ///<
    UINT32                                               DynWakeEn:1; ///<
    UINT32                                        CgttLclkOverride:1; ///<
    UINT32                                            Reserved_8_3:6; ///<
    UINT32                                         SBDmaActiveMask:1; ///<
    UINT32                                          Reserved_15_10:6; ///<
    UINT32                                          WakeHysteresis:16; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x2C_STRUCT;

// **** D0F0x98_x3A Register Definition ****
// Address
#define D0F0x98_x3A_ADDRESS                                     0x3A
// Type
#define D0F0x98_x3A_TYPE                                        TYPE_D0F0x98

// **** D0F0x98_x49 Register Definition ****
// Address
#define D0F0x98_x49_ADDRESS                                     0x49
// Type
#define D0F0x98_x49_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x49_Reserved_23_0_OFFSET                        0
#define D0F0x98_x49_Reserved_23_0_WIDTH                         24
#define D0F0x98_x49_Reserved_23_0_MASK                          0xFFFFFF
#define D0F0x98_x49_SoftOverrideClk6_OFFSET                     24
#define D0F0x98_x49_SoftOverrideClk6_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk6_MASK                       0x1000000
#define D0F0x98_x49_SoftOverrideClk5_OFFSET                     25
#define D0F0x98_x49_SoftOverrideClk5_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk5_MASK                       0x2000000
#define D0F0x98_x49_SoftOverrideClk4_OFFSET                     26
#define D0F0x98_x49_SoftOverrideClk4_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk4_MASK                       0x4000000
#define D0F0x98_x49_SoftOverrideClk3_OFFSET                     27
#define D0F0x98_x49_SoftOverrideClk3_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk3_MASK                       0x8000000
#define D0F0x98_x49_SoftOverrideClk2_OFFSET                     28
#define D0F0x98_x49_SoftOverrideClk2_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk2_MASK                       0x10000000
#define D0F0x98_x49_SoftOverrideClk1_OFFSET                     29
#define D0F0x98_x49_SoftOverrideClk1_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk1_MASK                       0x20000000
#define D0F0x98_x49_SoftOverrideClk0_OFFSET                     30
#define D0F0x98_x49_SoftOverrideClk0_WIDTH                      1
#define D0F0x98_x49_SoftOverrideClk0_MASK                       0x40000000
#define D0F0x98_x49_Reserved_31_31_OFFSET                       31
#define D0F0x98_x49_Reserved_31_31_WIDTH                        1
#define D0F0x98_x49_Reserved_31_31_MASK                         0x80000000

/// D0F0x98_x49
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                                        SoftOverrideClk6:1; ///<
    UINT32                                        SoftOverrideClk5:1; ///<
    UINT32                                        SoftOverrideClk4:1; ///<
    UINT32                                        SoftOverrideClk3:1; ///<
    UINT32                                        SoftOverrideClk2:1; ///<
    UINT32                                        SoftOverrideClk1:1; ///<
    UINT32                                        SoftOverrideClk0:1; ///<
    UINT32                                          Reserved_31_31:1; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x49_STRUCT;

// **** D0F0x98_x4A Register Definition ****
// Address
#define D0F0x98_x4A_ADDRESS                                     0x4A
// Type
#define D0F0x98_x4A_TYPE                                        TYPE_D0F0x98

// Field Data
#define D0F0x98_x4A_Reserved_23_0_OFFSET                        0
#define D0F0x98_x4A_Reserved_23_0_WIDTH                         24
#define D0F0x98_x4A_Reserved_23_0_MASK                          0xFFFFFF
#define D0F0x98_x4A_SoftOverrideClk6_OFFSET                     24
#define D0F0x98_x4A_SoftOverrideClk6_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk6_MASK                       0x1000000
#define D0F0x98_x4A_SoftOverrideClk5_OFFSET                     25
#define D0F0x98_x4A_SoftOverrideClk5_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk5_MASK                       0x2000000
#define D0F0x98_x4A_SoftOverrideClk4_OFFSET                     26
#define D0F0x98_x4A_SoftOverrideClk4_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk4_MASK                       0x4000000
#define D0F0x98_x4A_SoftOverrideClk3_OFFSET                     27
#define D0F0x98_x4A_SoftOverrideClk3_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk3_MASK                       0x8000000
#define D0F0x98_x4A_SoftOverrideClk2_OFFSET                     28
#define D0F0x98_x4A_SoftOverrideClk2_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk2_MASK                       0x10000000
#define D0F0x98_x4A_SoftOverrideClk1_OFFSET                     29
#define D0F0x98_x4A_SoftOverrideClk1_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk1_MASK                       0x20000000
#define D0F0x98_x4A_SoftOverrideClk0_OFFSET                     30
#define D0F0x98_x4A_SoftOverrideClk0_WIDTH                      1
#define D0F0x98_x4A_SoftOverrideClk0_MASK                       0x40000000
#define D0F0x98_x4A_Reserved_31_31_OFFSET                       31
#define D0F0x98_x4A_Reserved_31_31_WIDTH                        1
#define D0F0x98_x4A_Reserved_31_31_MASK                         0x80000000

/// D0F0x98_x4A
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                                        SoftOverrideClk6:1; ///<
    UINT32                                        SoftOverrideClk5:1; ///<
    UINT32                                        SoftOverrideClk4:1; ///<
    UINT32                                        SoftOverrideClk3:1; ///<
    UINT32                                        SoftOverrideClk2:1; ///<
    UINT32                                        SoftOverrideClk1:1; ///<
    UINT32                                        SoftOverrideClk0:1; ///<
    UINT32                                          Reserved_31_31:1; ///<

  } Field;

  UINT32 Value;
} D0F0x98_x4A_STRUCT;











/// D0F0xBC_x3FA04
typedef union {
  struct {                                                            ///<
    UINT32                                           Bitfield_15_0:16; ///<
    UINT32                                          Bitfield_31_16:16; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB4753_STRUCT;

// **** D0F0xE4_PHY_0004 Register Definition ****
// Address
#define D0F0xE4_PHY_0004_ADDRESS                                0x0004
// Type
#define D0F0xE4_PHY_0004_TYPE                                   TYPE_D0F0xE4

// Field Data
#define D0F0xE4_PHY_0004_Reserved_15_0_OFFSET                   0
#define D0F0xE4_PHY_0004_Reserved_15_0_WIDTH                    16
#define D0F0xE4_PHY_0004_Reserved_15_0_MASK                     0xFFFF
#define D0F0xE4_PHY_0004_CfgIdleDetTh_OFFSET                    16
#define D0F0xE4_PHY_0004_CfgIdleDetTh_WIDTH                     2
#define D0F0xE4_PHY_0004_CfgIdleDetTh_MASK                      0x30000
#define D0F0xE4_PHY_0004_Reserved_31_18_OFFSET                  18
#define D0F0xE4_PHY_0004_Reserved_31_18_WIDTH                   14
#define D0F0xE4_PHY_0004_Reserved_31_18_MASK                    0xFFFC0000

/// D0F0xE4_PHY_0004
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_15_0:16; ///<
    UINT32                                            CfgIdleDetTh:2 ; ///<
    UINT32                                          Reserved_31_18:14; ///<
  } Field;
  UINT32 Value;
} D0F0xE4_PHY_0004_STRUCT;

// **** D0F0xE4_PHY_4440 Register Definition ****
// Address
#define D0F0xE4_PHY_4440_ADDRESS                                0x4440
// Type
#define D0F0xE4_PHY_4440_TYPE                                   TYPE_D0F0xE4

// Field Data
#define D0F0xE4_PHY_4440_Reserved_12_0_OFFSET                   0
#define D0F0xE4_PHY_4440_Reserved_12_0_WIDTH                    13
#define D0F0xE4_PHY_4440_Reserved_12_0_MASK                     0x1FFF
#define D0F0xE4_PHY_4440_PllDbgRoIPFDResetCntrl_OFFSET          13
#define D0F0xE4_PHY_4440_PllDbgRoIPFDResetCntrl_WIDTH           2
#define D0F0xE4_PHY_4440_PllDbgRoIPFDResetCntrl_MASK            0x6000
#define D0F0xE4_PHY_4440_Reserved_31_15_OFFSET                  15
#define D0F0xE4_PHY_4440_Reserved_31_15_WIDTH                   17
#define D0F0xE4_PHY_4440_Reserved_31_15_MASK                    0xFFFF1000

/// D0F0xE4_PHY_4440
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_12_0:13; ///<
    UINT32                                  PllDbgRoIPFDResetCntrl:2 ; ///<
    UINT32                                          Reserved_31_15:17; ///<
  } Field;
  UINT32 Value;
} D0F0xE4_PHY_4440_STRUCT;

// **** D0F0xE4_PHY_4450 Register Definition ****
// Address
#define D0F0xE4_PHY_4450_ADDRESS                                0x4450
// Type
#define D0F0xE4_PHY_4450_TYPE                                   TYPE_D0F0xE4

// Field Data
#define D0F0xE4_PHY_4450_PllCfgROBWCntrlOvrdVal0_OFFSET         0
#define D0F0xE4_PHY_4450_PllCfgROBWCntrlOvrdVal0_WIDTH          8
#define D0F0xE4_PHY_4450_PllCfgROBWCntrlOvrdVal0_MASK           0xFF
#define D0F0xE4_PHY_4450_Reserved_29_8_OFFSET                   8
#define D0F0xE4_PHY_4450_Reserved_29_8_WIDTH                    22
#define D0F0xE4_PHY_4450_Reserved_29_8_MASK                     0x3FFFFF00
#define D0F0xE4_PHY_4450_PllCfgROVTOIBiasCntrlOvrdVal0_OFFSET   30
#define D0F0xE4_PHY_4450_PllCfgROVTOIBiasCntrlOvrdVal0_WIDTH    1
#define D0F0xE4_PHY_4450_PllCfgROVTOIBiasCntrlOvrdVal0_MASK     0x40000000
#define D0F0xE4_PHY_4450_Reserved_31_31_OFFSET                  31
#define D0F0xE4_PHY_4450_Reserved_31_31_WIDTH                   1
#define D0F0xE4_PHY_4450_Reserved_31_31_MASK                    0x80000000

/// D0F0xE4_PHY_4450
typedef union {
  struct {                                                            ///<
    UINT32                                 PllCfgROBWCntrlOvrdVal0:8 ; ///<
    UINT32                                           Reserved_29_8:22; ///<
    UINT32                           PllCfgROVTOIBiasCntrlOvrdVal0:1 ; ///<
    UINT32                                          Reserved_31_31:1 ; ///<
  } Field;
  UINT32 Value;
} D0F0xE4_PHY_4450_STRUCT;

// **** D0F0xE4_WRAP_0800 Register Definition ****
// Address
#define D0F0xE4_WRAP_0800_ADDRESS                               0x800
// Type
#define D0F0xE4_WRAP_0800_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_0800_HoldTraining_OFFSET                   0
#define D0F0xE4_WRAP_0800_HoldTraining_WIDTH                    1
#define D0F0xE4_WRAP_0800_HoldTraining_MASK                     0x1
#define D0F0xE4_WRAP_0800_Reserved_31_1_OFFSET                  1
#define D0F0xE4_WRAP_0800_Reserved_31_1_WIDTH                   31
#define D0F0xE4_WRAP_0800_Reserved_31_1_MASK                    0xFFFFFFFE

/// D0F0xE4_WRAP_0800
typedef union {
  struct {                                                            ///<
    UINT32                                            HoldTraining:1; ///<
    UINT32                                           Reserved_31_1:31; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_0800_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                             bit_31_24;

  } Field;

  UINT32 Value;
} GnbRegistersKB4915_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                             bit_31_24:8; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB4940_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                               bit_31_24:8; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB4965_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                               bit_31_24:8; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB4990_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_23_0:24; ///<
    UINT32                               bit_31_24:8; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB5015_STRUCT;

// **** D0F0xE4_WRAP_8011 Register Definition ****
// Address
#define D0F0xE4_WRAP_8011_ADDRESS                               0x8011
// Type
#define D0F0xE4_WRAP_8011_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8011_TxclkDynGateLatency_OFFSET            0
#define D0F0xE4_WRAP_8011_TxclkDynGateLatency_WIDTH             6
#define D0F0xE4_WRAP_8011_TxclkDynGateLatency_MASK              0x3F
#define D0F0xE4_WRAP_8011_TxclkPermGateEven_OFFSET              6
#define D0F0xE4_WRAP_8011_TxclkPermGateEven_WIDTH               1
#define D0F0xE4_WRAP_8011_TxclkPermGateEven_MASK                0x40
#define D0F0xE4_WRAP_8011_TxclkDynGateEnable_OFFSET             7
#define D0F0xE4_WRAP_8011_TxclkDynGateEnable_WIDTH              1
#define D0F0xE4_WRAP_8011_TxclkDynGateEnable_MASK               0x80
#define D0F0xE4_WRAP_8011_TxclkPermStop_OFFSET                  8
#define D0F0xE4_WRAP_8011_TxclkPermStop_WIDTH                   1
#define D0F0xE4_WRAP_8011_TxclkPermStop_MASK                    0x100
#define D0F0xE4_WRAP_8011_TxclkRegsGateEnable_OFFSET            9
#define D0F0xE4_WRAP_8011_TxclkRegsGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8011_TxclkRegsGateEnable_MASK              0x200
#define D0F0xE4_WRAP_8011_TxclkRegsGateLatency_OFFSET           10
#define D0F0xE4_WRAP_8011_TxclkRegsGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8011_TxclkRegsGateLatency_MASK             0xFC00
#define D0F0xE4_WRAP_8011_Reserved_16_16_OFFSET                 16
#define D0F0xE4_WRAP_8011_Reserved_16_16_WIDTH                  1
#define D0F0xE4_WRAP_8011_Reserved_16_16_MASK                   0x10000
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_OFFSET           17
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_MASK             0x7E0000
#define D0F0xE4_WRAP_8011_Bitfield_23_23_OFFSET                 23
#define D0F0xE4_WRAP_8011_Bitfield_23_23_WIDTH                  1
#define D0F0xE4_WRAP_8011_Bitfield_23_23_MASK                   0x800000
#define D0F0xE4_WRAP_8011_TxclkLcntGateEnable_OFFSET            24
#define D0F0xE4_WRAP_8011_TxclkLcntGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8011_TxclkLcntGateEnable_MASK              0x1000000
#define D0F0xE4_WRAP_8011_Reserved_25_25_OFFSET                 25
#define D0F0xE4_WRAP_8011_Reserved_25_25_WIDTH                  1
#define D0F0xE4_WRAP_8011_Reserved_25_25_MASK                   0x2000000
#define D0F0xE4_WRAP_8011_Reserved_31_26_OFFSET                 26
#define D0F0xE4_WRAP_8011_Reserved_31_26_WIDTH                  6
#define D0F0xE4_WRAP_8011_Reserved_31_26_MASK                   0xFC000000

/// D0F0xE4_WRAP_8011
typedef union {
  struct {                                                            ///<
    UINT32                                     TxclkDynGateLatency:6; ///<
    UINT32                                       TxclkPermGateEven:1; ///<
    UINT32                                      TxclkDynGateEnable:1; ///<
    UINT32                                           TxclkPermStop:1; ///<
    UINT32                                     TxclkRegsGateEnable:1; ///<
    UINT32                                    TxclkRegsGateLatency:6; ///<
    UINT32                                          Reserved_16_16:1; ///<
    UINT32                                    TxclkPermGateLatency:6; ///<
    UINT32                                          Bitfield_23_23:1; ///<
    UINT32                                     TxclkLcntGateEnable:1; ///<
    UINT32                                          Reserved_25_25:1; ///<
    UINT32                                          Reserved_31_26:6; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8011_STRUCT;

// **** D0F0xE4_WRAP_8012 Register Definition ****
// Address
#define D0F0xE4_WRAP_8012_ADDRESS                               0x8012
// Type
#define D0F0xE4_WRAP_8012_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8012_Pif1xIdleGateLatency_OFFSET           0
#define D0F0xE4_WRAP_8012_Pif1xIdleGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8012_Pif1xIdleGateLatency_MASK             0x3F
#define D0F0xE4_WRAP_8012_Reserved_6_6_OFFSET                   6
#define D0F0xE4_WRAP_8012_Reserved_6_6_WIDTH                    1
#define D0F0xE4_WRAP_8012_Reserved_6_6_MASK                     0x40
#define D0F0xE4_WRAP_8012_Pif1xIdleGateEnable_OFFSET            7
#define D0F0xE4_WRAP_8012_Pif1xIdleGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8012_Pif1xIdleGateEnable_MASK              0x80
#define D0F0xE4_WRAP_8012_Pif1xIdleResumeLatency_OFFSET         8
#define D0F0xE4_WRAP_8012_Pif1xIdleResumeLatency_WIDTH          6
#define D0F0xE4_WRAP_8012_Pif1xIdleResumeLatency_MASK           0x3F00
#define D0F0xE4_WRAP_8012_Reserved_31_14_OFFSET                 14
#define D0F0xE4_WRAP_8012_Reserved_31_14_WIDTH                  18
#define D0F0xE4_WRAP_8012_Reserved_31_14_MASK                   0xFFFFC000

/// D0F0xE4_WRAP_8012
typedef union {
  struct {                                                            ///<
    UINT32                                    Pif1xIdleGateLatency:6; ///<
    UINT32                                            Reserved_6_6:1; ///<
    UINT32                                     Pif1xIdleGateEnable:1; ///<
    UINT32                                  Pif1xIdleResumeLatency:6; ///<
    UINT32                                          Reserved_31_14:18; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8012_STRUCT;

// **** D0F0xE4_WRAP_8013 Register Definition ****
// Address
#define D0F0xE4_WRAP_8013_ADDRESS                               0x8013
// Type
#define D0F0xE4_WRAP_8013_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8013_MasterPciePllA_OFFSET                 0
#define D0F0xE4_WRAP_8013_MasterPciePllA_WIDTH                  1
#define D0F0xE4_WRAP_8013_MasterPciePllA_MASK                   0x1
#define D0F0xE4_WRAP_8013_MasterPciePllB_OFFSET                 1
#define D0F0xE4_WRAP_8013_MasterPciePllB_WIDTH                  1
#define D0F0xE4_WRAP_8013_MasterPciePllB_MASK                   0x2
#define D0F0xE4_WRAP_8013_Reserved_2_2_OFFSET                   2
#define D0F0xE4_WRAP_8013_Reserved_2_2_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_2_2_MASK                     0x4
#define D0F0xE4_WRAP_8013_Reserved_3_3_OFFSET                   3
#define D0F0xE4_WRAP_8013_Reserved_3_3_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_3_3_MASK                     0x8
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideA_OFFSET       4
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideA_WIDTH        1
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideA_MASK         0x10
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideB_OFFSET       5
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideB_WIDTH        1
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideB_MASK         0x20
#define D0F0xE4_WRAP_8013_Reserved_6_6_OFFSET                   6
#define D0F0xE4_WRAP_8013_Reserved_6_6_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_6_6_MASK                     0x40
#define D0F0xE4_WRAP_8013_Reserved_7_7_OFFSET                   7
#define D0F0xE4_WRAP_8013_Reserved_7_7_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_7_7_MASK                     0x80
#define D0F0xE4_WRAP_8013_TxclkSelCoreOverride_OFFSET           8
#define D0F0xE4_WRAP_8013_TxclkSelCoreOverride_WIDTH            1
#define D0F0xE4_WRAP_8013_TxclkSelCoreOverride_MASK             0x100
#define D0F0xE4_WRAP_8013_TxclkSelPifAOverride_OFFSET           9
#define D0F0xE4_WRAP_8013_TxclkSelPifAOverride_WIDTH            1
#define D0F0xE4_WRAP_8013_TxclkSelPifAOverride_MASK             0x200
#define D0F0xE4_WRAP_8013_TxclkSelPifBOverride_OFFSET           10
#define D0F0xE4_WRAP_8013_TxclkSelPifBOverride_WIDTH            1
#define D0F0xE4_WRAP_8013_TxclkSelPifBOverride_MASK             0x400
#define D0F0xE4_WRAP_8013_Reserved_11_11_OFFSET                 11
#define D0F0xE4_WRAP_8013_Reserved_11_11_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_11_11_MASK                   0x800
#define D0F0xE4_WRAP_8013_Reserved_12_12_OFFSET                 12
#define D0F0xE4_WRAP_8013_Reserved_12_12_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_12_12_MASK                   0x1000
#define D0F0xE4_WRAP_8013_PhyRxIsoDis_OFFSET                    13
#define D0F0xE4_WRAP_8013_PhyRxIsoDis_WIDTH                     2
#define D0F0xE4_WRAP_8013_PhyRxIsoDis_MASK                      0x6000
#define D0F0xE4_WRAP_8013_Reserved_31_15_OFFSET                 15
#define D0F0xE4_WRAP_8013_Reserved_31_15_WIDTH                  17
#define D0F0xE4_WRAP_8013_Reserved_31_15_MASK                   0xFFFF8000

/// D0F0xE4_WRAP_8013
typedef union {
  struct {                                                            ///<
    UINT32                                          MasterPciePllA:1; ///<
    UINT32                                          MasterPciePllB:1; ///<
    UINT32                                            Reserved_2_2:1; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                ClkDividerResetOverrideA:1; ///<
    UINT32                                ClkDividerResetOverrideB:1; ///<
    UINT32                                            Reserved_6_6:1; ///<
    UINT32                                            Reserved_7_7:1; ///<
    UINT32                                    TxclkSelCoreOverride:1; ///<
    UINT32                                    TxclkSelPifAOverride:1; ///<
    UINT32                                    TxclkSelPifBOverride:1; ///<
    UINT32                                          Reserved_11_11:1; ///<
    UINT32                                          Reserved_12_12:1; ///<
    UINT32                                             PhyRxIsoDis:2; ///<
    UINT32                                          Reserved_31_15:17; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8013_STRUCT;

// **** D0F0xE4_WRAP_8014 Register Definition ****
// Address
#define D0F0xE4_WRAP_8014_ADDRESS                               0x8014
// Type
#define D0F0xE4_WRAP_8014_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8014_TxclkPermGateEnable_OFFSET            0
#define D0F0xE4_WRAP_8014_TxclkPermGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8014_TxclkPermGateEnable_MASK              0x1
#define D0F0xE4_WRAP_8014_TxclkPrbsGateEnable_OFFSET            1
#define D0F0xE4_WRAP_8014_TxclkPrbsGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8014_TxclkPrbsGateEnable_MASK              0x2
#define D0F0xE4_WRAP_8014_Reserved_2_2_OFFSET                   2
#define D0F0xE4_WRAP_8014_Reserved_2_2_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_2_2_MASK                     0x4
#define D0F0xE4_WRAP_8014_Reserved_3_3_OFFSET                   3
#define D0F0xE4_WRAP_8014_Reserved_3_3_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_3_3_MASK                     0x8
#define D0F0xE4_WRAP_8014_Reserved_4_4_OFFSET                   4
#define D0F0xE4_WRAP_8014_Reserved_4_4_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_4_4_MASK                     0x10
#define D0F0xE4_WRAP_8014_Reserved_5_5_OFFSET                   5
#define D0F0xE4_WRAP_8014_Reserved_5_5_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_5_5_MASK                     0x20
#define D0F0xE4_WRAP_8014_Reserved_11_6_OFFSET                  6
#define D0F0xE4_WRAP_8014_Reserved_11_6_WIDTH                   6
#define D0F0xE4_WRAP_8014_Reserved_11_6_MASK                    0xFC0
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_OFFSET           12
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_MASK             0x1000
#define D0F0xE4_WRAP_8014_PcieGatePifB1xEnable_OFFSET           13
#define D0F0xE4_WRAP_8014_PcieGatePifB1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifB1xEnable_MASK             0x2000
#define D0F0xE4_WRAP_8014_Reserved_14_14_OFFSET                 14
#define D0F0xE4_WRAP_8014_Reserved_14_14_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_14_14_MASK                   0x4000
#define D0F0xE4_WRAP_8014_Reserved_15_15_OFFSET                 15
#define D0F0xE4_WRAP_8014_Reserved_15_15_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_15_15_MASK                   0x8000
#define D0F0xE4_WRAP_8014_Reserved_19_16_OFFSET                 16
#define D0F0xE4_WRAP_8014_Reserved_19_16_WIDTH                  4
#define D0F0xE4_WRAP_8014_Reserved_19_16_MASK                   0xF0000
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_OFFSET  20
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_WIDTH   1
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_MASK    0x100000
#define D0F0xE4_WRAP_8014_Reserved_23_21_OFFSET                 21
#define D0F0xE4_WRAP_8014_Reserved_23_21_WIDTH                  3
#define D0F0xE4_WRAP_8014_Reserved_23_21_MASK                   0xE00000
#define D0F0xE4_WRAP_8014_Reserved_24_24_OFFSET                 24
#define D0F0xE4_WRAP_8014_Reserved_24_24_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_24_24_MASK                   0x1000000
#define D0F0xE4_WRAP_8014_Reserved_25_25_OFFSET                 25
#define D0F0xE4_WRAP_8014_Reserved_25_25_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_25_25_MASK                   0x2000000
#define D0F0xE4_WRAP_8014_Reserved_26_26_OFFSET                 26
#define D0F0xE4_WRAP_8014_Reserved_26_26_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_26_26_MASK                   0x4000000
#define D0F0xE4_WRAP_8014_Reserved_27_27_OFFSET                 27
#define D0F0xE4_WRAP_8014_Reserved_27_27_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_27_27_MASK                   0x8000000
#define D0F0xE4_WRAP_8014_SpareRegRw_OFFSET                     28
#define D0F0xE4_WRAP_8014_SpareRegRw_WIDTH                      4
#define D0F0xE4_WRAP_8014_SpareRegRw_MASK                       0xF0000000

/// D0F0xE4_WRAP_8014
typedef union {
  struct {                                                            ///<
    UINT32                                     TxclkPermGateEnable:1; ///<
    UINT32                                     TxclkPrbsGateEnable:1; ///<
    UINT32                                            Reserved_2_2:1; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                            Reserved_4_4:1; ///<
    UINT32                                            Reserved_5_5:1; ///<
    UINT32                                           Reserved_11_6:6; ///<
    UINT32                                    PcieGatePifA1xEnable:1; ///<
    UINT32                                    PcieGatePifB1xEnable:1; ///<
    UINT32                                          Reserved_14_14:1; ///<
    UINT32                                          Reserved_15_15:1; ///<
    UINT32                                          Reserved_19_16:4; ///<
    UINT32                           TxclkPermGateOnlyWhenPllPwrDn:1; ///<
    UINT32                                          Reserved_23_21:3; ///<
    UINT32                                          Reserved_24_24:1; ///<
    UINT32                                          Reserved_25_25:1; ///<
    UINT32                                          Reserved_26_26:1; ///<
    UINT32                                          Reserved_27_27:1; ///<
    UINT32                                              SpareRegRw:4; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8014_STRUCT;

// **** D0F0xE4_WRAP_8015 Register Definition ****
// Address
#define D0F0xE4_WRAP_8015_ADDRESS                               0x8015
// Type
#define D0F0xE4_WRAP_8015_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8015_Bitfield_0_0_OFFSET                   0
#define D0F0xE4_WRAP_8015_Bitfield_0_0_WIDTH                    1
#define D0F0xE4_WRAP_8015_Bitfield_0_0_MASK                     0x1
#define D0F0xE4_WRAP_8015_Reserved_1_1_OFFSET                   1
#define D0F0xE4_WRAP_8015_Reserved_1_1_WIDTH                    1
#define D0F0xE4_WRAP_8015_Reserved_1_1_MASK                     0x2
#define D0F0xE4_WRAP_8015_Reserved_7_2_OFFSET                   2
#define D0F0xE4_WRAP_8015_Reserved_7_2_WIDTH                    6
#define D0F0xE4_WRAP_8015_Reserved_7_2_MASK                     0xFC
#define D0F0xE4_WRAP_8015_Reserved_8_8_OFFSET                   8
#define D0F0xE4_WRAP_8015_Reserved_8_8_WIDTH                    1
#define D0F0xE4_WRAP_8015_Reserved_8_8_MASK                     0x100
#define D0F0xE4_WRAP_8015_Bitfield_9_9_OFFSET                   9
#define D0F0xE4_WRAP_8015_Bitfield_9_9_WIDTH                    1
#define D0F0xE4_WRAP_8015_Bitfield_9_9_MASK                     0x200
#define D0F0xE4_WRAP_8015_Bitfield_10_10_OFFSET                 10
#define D0F0xE4_WRAP_8015_Bitfield_10_10_WIDTH                  1
#define D0F0xE4_WRAP_8015_Bitfield_10_10_MASK                   0x400
#define D0F0xE4_WRAP_8015_Bitfield_11_11_OFFSET                 11
#define D0F0xE4_WRAP_8015_Bitfield_11_11_WIDTH                  1
#define D0F0xE4_WRAP_8015_Bitfield_11_11_MASK                   0x800
#define D0F0xE4_WRAP_8015_Reserved_13_12_OFFSET                 12
#define D0F0xE4_WRAP_8015_Reserved_13_12_WIDTH                  2
#define D0F0xE4_WRAP_8015_Reserved_13_12_MASK                   0x3000
#define D0F0xE4_WRAP_8015_Bitfield_15_14_OFFSET                 14
#define D0F0xE4_WRAP_8015_Bitfield_15_14_WIDTH                  2
#define D0F0xE4_WRAP_8015_Bitfield_15_14_MASK                   0xC000
#define D0F0xE4_WRAP_8015_RefclkRegsGateLatency_OFFSET          16
#define D0F0xE4_WRAP_8015_RefclkRegsGateLatency_WIDTH           6
#define D0F0xE4_WRAP_8015_RefclkRegsGateLatency_MASK            0x3F0000
#define D0F0xE4_WRAP_8015_Reserved_22_22_OFFSET                 22
#define D0F0xE4_WRAP_8015_Reserved_22_22_WIDTH                  1
#define D0F0xE4_WRAP_8015_Reserved_22_22_MASK                   0x400000
#define D0F0xE4_WRAP_8015_RefclkRegsGateEnable_OFFSET           23
#define D0F0xE4_WRAP_8015_RefclkRegsGateEnable_WIDTH            1
#define D0F0xE4_WRAP_8015_RefclkRegsGateEnable_MASK             0x800000
#define D0F0xE4_WRAP_8015_Reserved_31_24_OFFSET                 24
#define D0F0xE4_WRAP_8015_Reserved_31_24_WIDTH                  8
#define D0F0xE4_WRAP_8015_Reserved_31_24_MASK                   0xFF000000

/// D0F0xE4_WRAP_8015
typedef union {
  struct {                                                            ///<
    UINT32                                            Bitfield_0_0:1; ///<
    UINT32                                            Reserved_1_1:1; ///<
    UINT32                                            Reserved_7_2:6; ///<
    UINT32                                            Reserved_8_8:1; ///<
    UINT32                                            Bitfield_9_9:1; ///<
    UINT32                                          Bitfield_10_10:1; ///<
    UINT32                                          Bitfield_11_11:1; ///<
    UINT32                                          Reserved_13_12:2; ///<
    UINT32                                          Bitfield_15_14:2; ///<
    UINT32                                   RefclkRegsGateLatency:6; ///<
    UINT32                                          Reserved_22_22:1; ///<
    UINT32                                    RefclkRegsGateEnable:1; ///<
    UINT32                                          Reserved_31_24:8; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8015_STRUCT;

// **** D0F0xE4_WRAP_8016 Register Definition ****
// Address
#define D0F0xE4_WRAP_8016_ADDRESS                               0x8016
// Type
#define D0F0xE4_WRAP_8016_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8016_CalibAckLatency_OFFSET                0
#define D0F0xE4_WRAP_8016_CalibAckLatency_WIDTH                 6
#define D0F0xE4_WRAP_8016_CalibAckLatency_MASK                  0x3F
#define D0F0xE4_WRAP_8016_Reserved_15_6_OFFSET                  6
#define D0F0xE4_WRAP_8016_Reserved_15_6_WIDTH                   10
#define D0F0xE4_WRAP_8016_Reserved_15_6_MASK                    0xFFC0
#define D0F0xE4_WRAP_8016_LclkDynGateLatency_OFFSET             16
#define D0F0xE4_WRAP_8016_LclkDynGateLatency_WIDTH              6
#define D0F0xE4_WRAP_8016_LclkDynGateLatency_MASK               0x3F0000
#define D0F0xE4_WRAP_8016_LclkGateFree_OFFSET                   22
#define D0F0xE4_WRAP_8016_LclkGateFree_WIDTH                    1
#define D0F0xE4_WRAP_8016_LclkGateFree_MASK                     0x400000
#define D0F0xE4_WRAP_8016_LclkDynGateEnable_OFFSET              23
#define D0F0xE4_WRAP_8016_LclkDynGateEnable_WIDTH               1
#define D0F0xE4_WRAP_8016_LclkDynGateEnable_MASK                0x800000
#define D0F0xE4_WRAP_8016_Reserved_31_24_OFFSET                 24
#define D0F0xE4_WRAP_8016_Reserved_31_24_WIDTH                  8
#define D0F0xE4_WRAP_8016_Reserved_31_24_MASK                   0xFF000000

/// D0F0xE4_WRAP_8016
typedef union {
  struct {                                                            ///<
    UINT32                                         CalibAckLatency:6; ///<
    UINT32                                           Reserved_15_6:10; ///<
    UINT32                                      LclkDynGateLatency:6; ///<
    UINT32                                            LclkGateFree:1; ///<
    UINT32                                       LclkDynGateEnable:1; ///<
    UINT32                                          Reserved_31_24:8; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8016_STRUCT;

// **** D0F0xE4_WRAP_8029 Register Definition ****
// Address
#define D0F0xE4_WRAP_8029_ADDRESS                               0x8029
// Type
#define D0F0xE4_WRAP_8029_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_WRAP_8029_LaneEnable_OFFSET                     0
#define D0F0xE4_WRAP_8029_LaneEnable_WIDTH                      16
#define D0F0xE4_WRAP_8029_LaneEnable_MASK                       0xFFFF
#define D0F0xE4_WRAP_8029_Reserved_31_16_OFFSET                 16
#define D0F0xE4_WRAP_8029_Reserved_31_16_WIDTH                  16
#define D0F0xE4_WRAP_8029_Reserved_31_16_MASK                   0xFFFF0000

/// D0F0xE4_WRAP_8029
typedef union {
  struct {                                                            ///<
    UINT32                                              LaneEnable:16; ///<
    UINT32                                          Reserved_31_16:16; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8029_STRUCT;

// **** D0F0xE4_WRAP_8062 Register Definition ****
// Address
#define D0F0xE4_WRAP_8062_ADDRESS                               0x8062

// Type
#define D0F0xE4_WRAP_8062_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8062_ReconfigureEn_OFFSET                  0
#define D0F0xE4_WRAP_8062_ReconfigureEn_WIDTH                   1
#define D0F0xE4_WRAP_8062_ReconfigureEn_MASK                    0x1
#define D0F0xE4_WRAP_8062_Reserved_1_1_OFFSET                   1
#define D0F0xE4_WRAP_8062_Reserved_1_1_WIDTH                    1
#define D0F0xE4_WRAP_8062_Reserved_1_1_MASK                     0x2
#define D0F0xE4_WRAP_8062_ResetPeriod_OFFSET                    2
#define D0F0xE4_WRAP_8062_ResetPeriod_WIDTH                     3
#define D0F0xE4_WRAP_8062_ResetPeriod_MASK                      0x1c
#define D0F0xE4_WRAP_8062_Reserved_9_5_OFFSET                   5
#define D0F0xE4_WRAP_8062_Reserved_9_5_WIDTH                    5
#define D0F0xE4_WRAP_8062_Reserved_9_5_MASK                     0x3e0
#define D0F0xE4_WRAP_8062_BlockOnIdle_OFFSET                    10
#define D0F0xE4_WRAP_8062_BlockOnIdle_WIDTH                     1
#define D0F0xE4_WRAP_8062_BlockOnIdle_MASK                      0x400
#define D0F0xE4_WRAP_8062_ConfigXferMode_OFFSET                 11
#define D0F0xE4_WRAP_8062_ConfigXferMode_WIDTH                  1
#define D0F0xE4_WRAP_8062_ConfigXferMode_MASK                   0x800
#define D0F0xE4_WRAP_8062_Reserved_31_12_OFFSET                 12
#define D0F0xE4_WRAP_8062_Reserved_31_12_WIDTH                  20
#define D0F0xE4_WRAP_8062_Reserved_31_12_MASK                   0xfffff000

/// D0F0xE4_WRAP_8062
typedef union {
  struct {                                                              ///<
    UINT32                                            ReconfigureEn:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                              ResetPeriod:3 ; ///<
    UINT32                                             Reserved_9_5:5 ; ///<
    UINT32                                              BlockOnIdle:1 ; ///<
    UINT32                                           ConfigXferMode:1 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8062_STRUCT;

// **** D0F0xE4_PIF_0011 Register Definition ****
// Address
#define D0F0xE4_PIF_0011_ADDRESS                                0x11
// Type
#define D0F0xE4_PIF_0011_TYPE                                   TYPE_D0F0xE4

// Field Data
#define D0F0xE4_PIF_0011_X2Lane10_OFFSET                        0
#define D0F0xE4_PIF_0011_X2Lane10_WIDTH                         1
#define D0F0xE4_PIF_0011_X2Lane10_MASK                          0x1
#define D0F0xE4_PIF_0011_X2Lane32_OFFSET                        1
#define D0F0xE4_PIF_0011_X2Lane32_WIDTH                         1
#define D0F0xE4_PIF_0011_X2Lane32_MASK                          0x2
#define D0F0xE4_PIF_0011_X2Lane54_OFFSET                        2
#define D0F0xE4_PIF_0011_X2Lane54_WIDTH                         1
#define D0F0xE4_PIF_0011_X2Lane54_MASK                          0x4
#define D0F0xE4_PIF_0011_X2Lane76_OFFSET                        3
#define D0F0xE4_PIF_0011_X2Lane76_WIDTH                         1
#define D0F0xE4_PIF_0011_X2Lane76_MASK                          0x8
#define D0F0xE4_PIF_0011_X2Lane98_OFFSET                        4
#define D0F0xE4_PIF_0011_X2Lane98_WIDTH                         1
#define D0F0xE4_PIF_0011_X2Lane98_MASK                          0x10
#define D0F0xE4_PIF_0011_X2Lane1110_OFFSET                      5
#define D0F0xE4_PIF_0011_X2Lane1110_WIDTH                       1
#define D0F0xE4_PIF_0011_X2Lane1110_MASK                        0x20
#define D0F0xE4_PIF_0011_X2Lane1312_OFFSET                      6
#define D0F0xE4_PIF_0011_X2Lane1312_WIDTH                       1
#define D0F0xE4_PIF_0011_X2Lane1312_MASK                        0x40
#define D0F0xE4_PIF_0011_X2Lane1514_OFFSET                      7
#define D0F0xE4_PIF_0011_X2Lane1514_WIDTH                       1
#define D0F0xE4_PIF_0011_X2Lane1514_MASK                        0x80
#define D0F0xE4_PIF_0011_X4Lane30_OFFSET                        8
#define D0F0xE4_PIF_0011_X4Lane30_WIDTH                         1
#define D0F0xE4_PIF_0011_X4Lane30_MASK                          0x100
#define D0F0xE4_PIF_0011_X4Lane74_OFFSET                        9
#define D0F0xE4_PIF_0011_X4Lane74_WIDTH                         1
#define D0F0xE4_PIF_0011_X4Lane74_MASK                          0x200
#define D0F0xE4_PIF_0011_X4Lane118_OFFSET                       10
#define D0F0xE4_PIF_0011_X4Lane118_WIDTH                        1
#define D0F0xE4_PIF_0011_X4Lane118_MASK                         0x400
#define D0F0xE4_PIF_0011_X4Lane1512_OFFSET                      11
#define D0F0xE4_PIF_0011_X4Lane1512_WIDTH                       1
#define D0F0xE4_PIF_0011_X4Lane1512_MASK                        0x800
#define D0F0xE4_PIF_0011_Reserved_15_12_OFFSET                  12
#define D0F0xE4_PIF_0011_Reserved_15_12_WIDTH                   4
#define D0F0xE4_PIF_0011_Reserved_15_12_MASK                    0xF000
#define D0F0xE4_PIF_0011_X8Lane70_OFFSET                        16
#define D0F0xE4_PIF_0011_X8Lane70_WIDTH                         1
#define D0F0xE4_PIF_0011_X8Lane70_MASK                          0x10000
#define D0F0xE4_PIF_0011_X8Lane158_OFFSET                       17
#define D0F0xE4_PIF_0011_X8Lane158_WIDTH                        1
#define D0F0xE4_PIF_0011_X8Lane158_MASK                         0x20000
#define D0F0xE4_PIF_0011_Reserved_19_18_OFFSET                  18
#define D0F0xE4_PIF_0011_Reserved_19_18_WIDTH                   2
#define D0F0xE4_PIF_0011_Reserved_19_18_MASK                    0xC0000
#define D0F0xE4_PIF_0011_X16Lane150_OFFSET                      20
#define D0F0xE4_PIF_0011_X16Lane150_WIDTH                       1
#define D0F0xE4_PIF_0011_X16Lane150_MASK                        0x100000
#define D0F0xE4_PIF_0011_Reserved_24_21_OFFSET                  21
#define D0F0xE4_PIF_0011_Reserved_24_21_WIDTH                   4
#define D0F0xE4_PIF_0011_Reserved_24_21_MASK                    0x1E00000
#define D0F0xE4_PIF_0011_MultiPif_OFFSET                        25
#define D0F0xE4_PIF_0011_MultiPif_WIDTH                         1
#define D0F0xE4_PIF_0011_MultiPif_MASK                          0x2000000
#define D0F0xE4_PIF_0011_Reserved_31_26_OFFSET                  26
#define D0F0xE4_PIF_0011_Reserved_31_26_WIDTH                   6
#define D0F0xE4_PIF_0011_Reserved_31_26_MASK                    0xFC000000

/// D0F0xE4_PIF_0011
typedef union {
  struct {                                                            ///<
    UINT32                                                X2Lane10:1; ///<
    UINT32                                                X2Lane32:1; ///<
    UINT32                                                X2Lane54:1; ///<
    UINT32                                                X2Lane76:1; ///<
    UINT32                                                X2Lane98:1; ///<
    UINT32                                              X2Lane1110:1; ///<
    UINT32                                              X2Lane1312:1; ///<
    UINT32                                              X2Lane1514:1; ///<
    UINT32                                                X4Lane30:1; ///<
    UINT32                                                X4Lane74:1; ///<
    UINT32                                               X4Lane118:1; ///<
    UINT32                                              X4Lane1512:1; ///<
    UINT32                                          Reserved_15_12:4; ///<
    UINT32                                                X8Lane70:1; ///<
    UINT32                                               X8Lane158:1; ///<
    UINT32                                          Reserved_19_18:2; ///<
    UINT32                                              X16Lane150:1; ///<
    UINT32                                          Reserved_24_21:4; ///<
    UINT32                                                MultiPif:1; ///<
    UINT32                                          Reserved_31_26:6; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_PIF_0011_STRUCT;

// **** D0F0xE4_PIF_0012 Register Definition ****
// Address
#define D0F0xE4_PIF_0012_ADDRESS                                0x12
// Type
#define D0F0xE4_PIF_0012_TYPE                                   TYPE_D0F0xE4

// Field Data
#define D0F0xE4_PIF_0012_TxPowerStateInTxs2_OFFSET              0
#define D0F0xE4_PIF_0012_TxPowerStateInTxs2_WIDTH               3
#define D0F0xE4_PIF_0012_TxPowerStateInTxs2_MASK                0x7
#define D0F0xE4_PIF_0012_ForceRxEnInL0s_OFFSET                  3
#define D0F0xE4_PIF_0012_ForceRxEnInL0s_WIDTH                   1
#define D0F0xE4_PIF_0012_ForceRxEnInL0s_MASK                    0x8
#define D0F0xE4_PIF_0012_RxPowerStateInRxs2_OFFSET              4
#define D0F0xE4_PIF_0012_RxPowerStateInRxs2_WIDTH               3
#define D0F0xE4_PIF_0012_RxPowerStateInRxs2_MASK                0x70
#define D0F0xE4_PIF_0012_PllPowerStateInTxs2_OFFSET             7
#define D0F0xE4_PIF_0012_PllPowerStateInTxs2_WIDTH              3
#define D0F0xE4_PIF_0012_PllPowerStateInTxs2_MASK               0x380
#define D0F0xE4_PIF_0012_PllPowerStateInOff_OFFSET              10
#define D0F0xE4_PIF_0012_PllPowerStateInOff_WIDTH               3
#define D0F0xE4_PIF_0012_PllPowerStateInOff_MASK                0x1C00
#define D0F0xE4_PIF_0012_Reserved_15_13_OFFSET                  13
#define D0F0xE4_PIF_0012_Reserved_15_13_WIDTH                   3
#define D0F0xE4_PIF_0012_Reserved_15_13_MASK                    0xE000
#define D0F0xE4_PIF_0012_Tx2p5clkClockGatingEn_OFFSET           16
#define D0F0xE4_PIF_0012_Tx2p5clkClockGatingEn_WIDTH            1
#define D0F0xE4_PIF_0012_Tx2p5clkClockGatingEn_MASK             0x10000
#define D0F0xE4_PIF_0012_Reserved_23_17_OFFSET                  17
#define D0F0xE4_PIF_0012_Reserved_23_17_WIDTH                   7
#define D0F0xE4_PIF_0012_Reserved_23_17_MASK                    0xFE0000
#define D0F0xE4_PIF_0012_PllRampUpTime_OFFSET                   24
#define D0F0xE4_PIF_0012_PllRampUpTime_WIDTH                    3
#define D0F0xE4_PIF_0012_PllRampUpTime_MASK                     0x7000000
#define D0F0xE4_PIF_0012_Reserved_27_27_OFFSET                  27
#define D0F0xE4_PIF_0012_Reserved_27_27_WIDTH                   1
#define D0F0xE4_PIF_0012_Reserved_27_27_MASK                    0x8000000
#define D0F0xE4_PIF_0012_PllPwrOverrideEn_OFFSET                28
#define D0F0xE4_PIF_0012_PllPwrOverrideEn_WIDTH                 1
#define D0F0xE4_PIF_0012_PllPwrOverrideEn_MASK                  0x10000000
#define D0F0xE4_PIF_0012_PllPwrOverrideVal_OFFSET               29
#define D0F0xE4_PIF_0012_PllPwrOverrideVal_WIDTH                3
#define D0F0xE4_PIF_0012_PllPwrOverrideVal_MASK                 0xE0000000

/// D0F0xE4_PIF_0012
typedef union {
  struct {                                                            ///<
    UINT32                                      TxPowerStateInTxs2:3; ///<
    UINT32                                          ForceRxEnInL0s:1; ///<
    UINT32                                      RxPowerStateInRxs2:3; ///<
    UINT32                                     PllPowerStateInTxs2:3; ///<
    UINT32                                      PllPowerStateInOff:3; ///<
    UINT32                                          Reserved_15_13:3; ///<
    UINT32                                   Tx2p5clkClockGatingEn:1; ///<
    UINT32                                          Reserved_23_17:7; ///<
    UINT32                                           PllRampUpTime:3; ///<
    UINT32                                          Reserved_27_27:1; ///<
    UINT32                                        PllPwrOverrideEn:1; ///<
    UINT32                                       PllPwrOverrideVal:3; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_PIF_0012_STRUCT;

// **** D0F0xE4_CORE_0002 Register Definition ****
// Address
#define D0F0xE4_CORE_0002_ADDRESS                               0x0002
// Type
#define D0F0xE4_CORE_0002_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_0002_HwDebug_0_OFFSET                      0
#define D0F0xE4_CORE_0002_HwDebug_0_WIDTH                       1
#define D0F0xE4_CORE_0002_HwDebug_0_MASK                        0x1
#define D0F0xE4_CORE_0002_Reserved_31_1_OFFSET                  1
#define D0F0xE4_CORE_0002_Reserved_31_1_WIDTH                   31
#define D0F0xE4_CORE_0002_Reserved_31_1_MASK                    0xFFFFFFFE

/// D0F0xE4_CORE_0002
typedef union {
  struct {                                                            ///<
    UINT32                                               HwDebug_0:1; ///<
    UINT32                                           Reserved_31_1:31; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_0002_STRUCT;

// **** D0F0xE4_CORE_0010 Register Definition ****
// Address
#define D0F0xE4_CORE_0010_ADDRESS                               0x10
// Type
#define D0F0xE4_CORE_0010_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_0010_HwInitWrLock_OFFSET                   0
#define D0F0xE4_CORE_0010_HwInitWrLock_WIDTH                    1
#define D0F0xE4_CORE_0010_HwInitWrLock_MASK                     0x1
#define D0F0xE4_CORE_0010_Reserved_8_1_OFFSET                   1
#define D0F0xE4_CORE_0010_Reserved_8_1_WIDTH                    8
#define D0F0xE4_CORE_0010_Reserved_8_1_MASK                     0x1FE
#define D0F0xE4_CORE_0010_UmiNpMemWrite_OFFSET                  9
#define D0F0xE4_CORE_0010_UmiNpMemWrite_WIDTH                   1
#define D0F0xE4_CORE_0010_UmiNpMemWrite_MASK                    0x200
#define D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_OFFSET            10
#define D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_WIDTH             3
#define D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_MASK              0x1C00
#define D0F0xE4_CORE_0010_Reserved_31_13_OFFSET                 13
#define D0F0xE4_CORE_0010_Reserved_31_13_WIDTH                  19
#define D0F0xE4_CORE_0010_Reserved_31_13_MASK                   0xFFFFE000

/// D0F0xE4_CORE_0010
typedef union {
  struct {                                                            ///<
    UINT32                                            HwInitWrLock:1; ///<
    UINT32                                            Reserved_8_1:8; ///<
    UINT32                                           UmiNpMemWrite:1; ///<
    UINT32                                     RxUmiAdjPayloadSize:3; ///<
    UINT32                                          Reserved_31_13:19; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_0010_STRUCT;

// **** D0F0xE4_CORE_0011 Register Definition ****
// Address
#define D0F0xE4_CORE_0011_ADDRESS                               0x11
// Type
#define D0F0xE4_CORE_0011_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_0011_DynClkLatency_OFFSET                  0
#define D0F0xE4_CORE_0011_DynClkLatency_WIDTH                   4
#define D0F0xE4_CORE_0011_DynClkLatency_MASK                    0xF
#define D0F0xE4_CORE_0011_Reserved_31_4_OFFSET                  4
#define D0F0xE4_CORE_0011_Reserved_31_4_WIDTH                   28
#define D0F0xE4_CORE_0011_Reserved_31_4_MASK                    0xFFFFFFF0

/// D0F0xE4_CORE_0011
typedef union {
  struct {                                                            ///<
    UINT32                                           DynClkLatency:4; ///<
    UINT32                                           Reserved_31_4:28; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_0011_STRUCT;

// **** D0F0xE4_CORE_001C Register Definition ****
// Address
#define D0F0xE4_CORE_001C_ADDRESS                               0x1C
// Type
#define D0F0xE4_CORE_001C_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_001C_TxArbRoundRobinEn_OFFSET              0
#define D0F0xE4_CORE_001C_TxArbRoundRobinEn_WIDTH               1
#define D0F0xE4_CORE_001C_TxArbRoundRobinEn_MASK                0x1
#define D0F0xE4_CORE_001C_TxArbSlvLimit_OFFSET                  1
#define D0F0xE4_CORE_001C_TxArbSlvLimit_WIDTH                   5
#define D0F0xE4_CORE_001C_TxArbSlvLimit_MASK                    0x3E
#define D0F0xE4_CORE_001C_TxArbMstLimit_OFFSET                  6
#define D0F0xE4_CORE_001C_TxArbMstLimit_WIDTH                   5
#define D0F0xE4_CORE_001C_TxArbMstLimit_MASK                    0x7C0
#define D0F0xE4_CORE_001C_Reserved_31_11_OFFSET                 11
#define D0F0xE4_CORE_001C_Reserved_31_11_WIDTH                  21
#define D0F0xE4_CORE_001C_Reserved_31_11_MASK                   0xFFFFF800

/// D0F0xE4_CORE_001C
typedef union {
  struct {                                                            ///<
    UINT32                                       TxArbRoundRobinEn:1; ///<
    UINT32                                           TxArbSlvLimit:5; ///<
    UINT32                                           TxArbMstLimit:5; ///<
    UINT32                                          Reserved_31_11:21; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_001C_STRUCT;

// **** D0F0xE4_CORE_0020 Register Definition ****
// Address
#define D0F0xE4_CORE_0020_ADDRESS                               0x0020
// Type
#define D0F0xE4_CORE_0020_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_0020_Reserved_7_0_OFFSET                   0
#define D0F0xE4_CORE_0020_Reserved_7_0_WIDTH                    8
#define D0F0xE4_CORE_0020_Reserved_7_0_MASK                     0xFF
#define D0F0xE4_CORE_0020_CiSlvOrderingDis_OFFSET               8
#define D0F0xE4_CORE_0020_CiSlvOrderingDis_WIDTH                1
#define D0F0xE4_CORE_0020_CiSlvOrderingDis_MASK                 0x100
#define D0F0xE4_CORE_0020_CiRcOrderingDis_OFFSET                9
#define D0F0xE4_CORE_0020_CiRcOrderingDis_WIDTH                 1
#define D0F0xE4_CORE_0020_CiRcOrderingDis_MASK                  0x200
#define D0F0xE4_CORE_0020_Reserved_31_10_OFFSET                 10
#define D0F0xE4_CORE_0020_Reserved_31_10_WIDTH                  22
#define D0F0xE4_CORE_0020_Reserved_31_10_MASK                   0xFFFFFC00

/// D0F0xE4_CORE_0020
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_7_0:8; ///<
    UINT32                                        CiSlvOrderingDis:1; ///<
    UINT32                                         CiRcOrderingDis:1; ///<
    UINT32                                          Reserved_31_10:22; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_0020_STRUCT;

// **** D0F0xE4_CORE_0040 Register Definition ****
// Address
#define D0F0xE4_CORE_0040_ADDRESS                               0x40
// Type
#define D0F0xE4_CORE_0040_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_0040_Reserved_13_0_OFFSET                  0
#define D0F0xE4_CORE_0040_Reserved_13_0_WIDTH                   14
#define D0F0xE4_CORE_0040_Reserved_13_0_MASK                    0x3FFF
#define D0F0xE4_CORE_0040_PElecIdleMode_OFFSET                  14
#define D0F0xE4_CORE_0040_PElecIdleMode_WIDTH                   2
#define D0F0xE4_CORE_0040_PElecIdleMode_MASK                    0xC000
#define D0F0xE4_CORE_0040_Reserved_31_16_OFFSET                 16
#define D0F0xE4_CORE_0040_Reserved_31_16_WIDTH                  16
#define D0F0xE4_CORE_0040_Reserved_31_16_MASK                   0xFFFF0000

/// D0F0xE4_CORE_0040
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_13_0:14; ///<
    UINT32                                           PElecIdleMode:2; ///<
    UINT32                                          Reserved_31_16:16; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_0040_STRUCT;

// **** D0F0xE4_CORE_00B0 Register Definition ****
// Address
#define D0F0xE4_CORE_00B0_ADDRESS                               0xB0
// Type
#define D0F0xE4_CORE_00B0_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_00B0_Reserved_1_0_OFFSET                   0
#define D0F0xE4_CORE_00B0_Reserved_1_0_WIDTH                    2
#define D0F0xE4_CORE_00B0_Reserved_1_0_MASK                     0x3
#define D0F0xE4_CORE_00B0_StrapF0MsiEn_OFFSET                   2
#define D0F0xE4_CORE_00B0_StrapF0MsiEn_WIDTH                    1
#define D0F0xE4_CORE_00B0_StrapF0MsiEn_MASK                     0x4
#define D0F0xE4_CORE_00B0_Reserved_4_3_OFFSET                   3
#define D0F0xE4_CORE_00B0_Reserved_4_3_WIDTH                    2
#define D0F0xE4_CORE_00B0_Reserved_4_3_MASK                     0x18
#define D0F0xE4_CORE_00B0_StrapF0AerEn_OFFSET                   5
#define D0F0xE4_CORE_00B0_StrapF0AerEn_WIDTH                    1
#define D0F0xE4_CORE_00B0_StrapF0AerEn_MASK                     0x20
#define D0F0xE4_CORE_00B0_Reserved_31_6_OFFSET                  6
#define D0F0xE4_CORE_00B0_Reserved_31_6_WIDTH                   26
#define D0F0xE4_CORE_00B0_Reserved_31_6_MASK                    0xFFFFFFC0

/// D0F0xE4_CORE_00B0
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_1_0:2; ///<
    UINT32                                            StrapF0MsiEn:1; ///<
    UINT32                                            Reserved_4_3:2; ///<
    UINT32                                            StrapF0AerEn:1; ///<
    UINT32                                           Reserved_31_6:26; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_00B0_STRUCT;

// **** D0F0xE4_CORE_00C1 Register Definition ****
// Address
#define D0F0xE4_CORE_00C1_ADDRESS                               0xC1
// Type
#define D0F0xE4_CORE_00C1_TYPE                                  TYPE_D0F0xE4

// Field Data
#define D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_OFFSET   0
#define D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_WIDTH    1
#define D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_MASK     0x1
#define D0F0xE4_CORE_00C1_StrapGen2Compliance_OFFSET            1
#define D0F0xE4_CORE_00C1_StrapGen2Compliance_WIDTH             1
#define D0F0xE4_CORE_00C1_StrapGen2Compliance_MASK              0x2
#define D0F0xE4_CORE_00C1_Reserved_31_2_OFFSET                  2
#define D0F0xE4_CORE_00C1_Reserved_31_2_WIDTH                   30
#define D0F0xE4_CORE_00C1_Reserved_31_2_MASK                    0xFFFFFFFC

/// D0F0xE4_CORE_00C1
typedef union {
  struct {                                                            ///<
    UINT32                            StrapLinkBwNotificationCapEn:1; ///<
    UINT32                                     StrapGen2Compliance:1; ///<
    UINT32                                           Reserved_31_2:30; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_CORE_00C1_STRUCT;

// **** DxFxxE4_x70 Register Definition ****
// Address
#define DxFxxE4_x70_ADDRESS                                     0x70
// Type
#define DxFxxE4_x70_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_x70_Reserved_15_0_OFFSET                        0
#define DxFxxE4_x70_Reserved_15_0_WIDTH                         16
#define DxFxxE4_x70_Reserved_15_0_MASK                          0xFFFF
#define DxFxxE4_x70_RxRcbCplTimeout_OFFSET                      16
#define DxFxxE4_x70_RxRcbCplTimeout_WIDTH                       3
#define DxFxxE4_x70_RxRcbCplTimeout_MASK                        0x70000
#define DxFxxE4_x70_RxRcbCplTimeoutMode_OFFSET                  19
#define DxFxxE4_x70_RxRcbCplTimeoutMode_WIDTH                   1
#define DxFxxE4_x70_RxRcbCplTimeoutMode_MASK                    0x80000
#define DxFxxE4_x70_Reserved_31_20_OFFSET                       20
#define DxFxxE4_x70_Reserved_31_20_WIDTH                        12
#define DxFxxE4_x70_Reserved_31_20_MASK                         0xFFF00000

/// DxFxxE4_x70
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_15_0:16; ///<
    UINT32                                         RxRcbCplTimeout:3; ///<
    UINT32                                     RxRcbCplTimeoutMode:1; ///<
    UINT32                                          Reserved_31_20:12; ///<

  } Field;

  UINT32 Value;
} DxFxxE4_x70_STRUCT;

// **** DxFxxE4_xA0 Register Definition ****
// Address
#define DxFxxE4_xA0_ADDRESS                                     0xA0
// Type
#define DxFxxE4_xA0_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_xA0_Reserved_3_0_OFFSET                         0
#define DxFxxE4_xA0_Reserved_3_0_WIDTH                          4
#define DxFxxE4_xA0_Reserved_3_0_MASK                           0xF
#define DxFxxE4_xA0_Lc16xClearTxPipe_OFFSET                     4
#define DxFxxE4_xA0_Lc16xClearTxPipe_WIDTH                      4
#define DxFxxE4_xA0_Lc16xClearTxPipe_MASK                       0xF0
#define DxFxxE4_xA0_LcL0sInactivity_OFFSET                      8
#define DxFxxE4_xA0_LcL0sInactivity_WIDTH                       4
#define DxFxxE4_xA0_LcL0sInactivity_MASK                        0xF00
#define DxFxxE4_xA0_LcL1Inactivity_OFFSET                       12
#define DxFxxE4_xA0_LcL1Inactivity_WIDTH                        4
#define DxFxxE4_xA0_LcL1Inactivity_MASK                         0xF000
#define DxFxxE4_xA0_Reserved_22_16_OFFSET                       16
#define DxFxxE4_xA0_Reserved_22_16_WIDTH                        7
#define DxFxxE4_xA0_Reserved_22_16_MASK                         0x7F0000
#define DxFxxE4_xA0_LcL1ImmediateAck_OFFSET                     23
#define DxFxxE4_xA0_LcL1ImmediateAck_WIDTH                      1
#define DxFxxE4_xA0_LcL1ImmediateAck_MASK                       0x800000
#define DxFxxE4_xA0_Reserved_24_24_OFFSET                       24
#define DxFxxE4_xA0_Reserved_24_24_WIDTH                        1
#define DxFxxE4_xA0_Reserved_24_24_MASK                         0x01000000
#define DxFxxE4_xA0_Reserved_26_25_OFFSET                       25
#define DxFxxE4_xA0_Reserved_26_25_WIDTH                        2
#define DxFxxE4_xA0_Reserved_26_25_MASK                         0x06000000
#define DxFxxE4_xA0_Reserved_27_27_OFFSET                       27
#define DxFxxE4_xA0_Reserved_27_27_WIDTH                        1
#define DxFxxE4_xA0_Reserved_27_27_MASK                         0x08000000
#define DxFxxE4_xA0_Reserved_28_28_OFFSET                       28
#define DxFxxE4_xA0_Reserved_28_28_WIDTH                        1
#define DxFxxE4_xA0_Reserved_28_28_MASK                         0x10000000
#define DxFxxE4_xA0_Reserved_31_29_OFFSET                       29
#define DxFxxE4_xA0_Reserved_31_29_WIDTH                        3
#define DxFxxE4_xA0_Reserved_31_29_MASK                         0xE0000000

/// DxFxxE4_xA0
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_3_0:4; ///<
    UINT32                                        Lc16xClearTxPipe:4; ///<
    UINT32                                         LcL0sInactivity:4; ///<
    UINT32                                          LcL1Inactivity:4; ///<
    UINT32                                          Reserved_22_16:7; ///<
    UINT32                                        LcL1ImmediateAck:1; ///<
    UINT32                                          Reserved_24_24:1; ///<
    UINT32                                          Reserved_26_25:2; ///<
    UINT32                                          Reserved_27_27:1; ///<
    UINT32                                          Reserved_28_28:1; ///<
    UINT32                                          Reserved_31_29:3; ///<

  } Field;

  UINT32 Value;
} DxFxxE4_xA0_STRUCT;

// **** DxFxxE4_xA1 Register Definition ****
// Address
#define DxFxxE4_xA1_ADDRESS                                     0xA1
// Type
#define DxFxxE4_xA1_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_xA1_Reserved_10_0_OFFSET                        0
#define DxFxxE4_xA1_Reserved_10_0_WIDTH                         11
#define DxFxxE4_xA1_Reserved_10_0_MASK                          0x7FF
#define DxFxxE4_xA1_LcDontGotoL0sifL1Armed_OFFSET               11
#define DxFxxE4_xA1_LcDontGotoL0sifL1Armed_WIDTH                1
#define DxFxxE4_xA1_LcDontGotoL0sifL1Armed_MASK                 0x800
#define DxFxxE4_xA1_Reserved_31_12_OFFSET                       12
#define DxFxxE4_xA1_Reserved_31_12_WIDTH                        20
#define DxFxxE4_xA1_Reserved_31_12_MASK                         0xFFFFF000

/// DxFxxE4_xA1
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_10_0:11; ///<
    UINT32                                  LcDontGotoL0sifL1Armed:1; ///<
    UINT32                                          Reserved_31_12:20; ///<

  } Field;

  UINT32 Value;
} DxFxxE4_xA1_STRUCT;

// **** DxFxxE4_xA3 Register Definition ****
// Address
#define DxFxxE4_xA3_ADDRESS                                     0xA3
// Type
#define DxFxxE4_xA3_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_xA3_Reserved_8_0_OFFSET                         0
#define DxFxxE4_xA3_Reserved_8_0_WIDTH                          9
#define DxFxxE4_xA3_Reserved_8_0_MASK                           0x1FF
#define DxFxxE4_xA3_LcXmitFtsBeforeRecovery_OFFSET              9
#define DxFxxE4_xA3_LcXmitFtsBeforeRecovery_WIDTH               1
#define DxFxxE4_xA3_LcXmitFtsBeforeRecovery_MASK                0x200
#define DxFxxE4_xA3_Reserved_31_10_OFFSET                       10
#define DxFxxE4_xA3_Reserved_31_10_WIDTH                        22
#define DxFxxE4_xA3_Reserved_31_10_MASK                         0xFFFFFC00

/// DxFxxE4_xA3
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_8_0:9; ///<
    UINT32                                 LcXmitFtsBeforeRecovery:1; ///<
    UINT32                                          Reserved_31_10:22; ///<

  } Field;

  UINT32 Value;
} DxFxxE4_xA3_STRUCT;

// **** DxFxxE4_xB1 Register Definition ****
// Address
#define DxFxxE4_xB1_ADDRESS                                     0xB1
// Type
#define DxFxxE4_xB1_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_xB1_Reserved_13_0_OFFSET                        0
#define DxFxxE4_xB1_Reserved_13_0_WIDTH                         14
#define DxFxxE4_xB1_Reserved_13_0_MASK                          0x3FFF
#define DxFxxE4_xB1_LcElecIdleMode_OFFSET                       14
#define DxFxxE4_xB1_LcElecIdleMode_WIDTH                        2
#define DxFxxE4_xB1_LcElecIdleMode_MASK                         0xc000
#define DxFxxE4_xB1_Reserved_18_16_OFFSET                       16
#define DxFxxE4_xB1_Reserved_18_16_WIDTH                        3
#define DxFxxE4_xB1_Reserved_18_16_MASK                         0x70000
#define DxFxxE4_xB1_LcDeassertRxEnInL0s_OFFSET                  19
#define DxFxxE4_xB1_LcDeassertRxEnInL0s_WIDTH                   1
#define DxFxxE4_xB1_LcDeassertRxEnInL0s_MASK                    0x80000
#define DxFxxE4_xB1_LcBlockElIdleinL0_OFFSET                    20
#define DxFxxE4_xB1_LcBlockElIdleinL0_WIDTH                     1
#define DxFxxE4_xB1_LcBlockElIdleinL0_MASK                      0x100000
#define DxFxxE4_xB1_Reserved_31_21_OFFSET                       21
#define DxFxxE4_xB1_Reserved_31_21_WIDTH                        11
#define DxFxxE4_xB1_Reserved_31_21_MASK                         0xFFE00000

/// DxFxxE4_xB1
typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_13_0:14; ///<
    UINT32                                          LcElecIdleMode:2 ; ///<
    UINT32                                          Reserved_18_16:3 ; ///<
    UINT32                                     LcDeassertRxEnInL0s:1; ///<
    UINT32                                       LcBlockElIdleinL0:1; ///<
    UINT32                                          Reserved_31_21:11; ///<

  } Field;
  UINT32 Value;
} DxFxxE4_xB1_STRUCT;

// **** DxFxxE4_xC0 Register Definition ****
// Address
// Type
#define DxFxxE4_xC0_TYPE                                        TYPE_D2F1xE4

// Field Data
#define DxFxxE4_xC0_Reserved_3_0_OFFSET                         0
#define DxFxxE4_xC0_Reserved_3_0_WIDTH                          4
#define DxFxxE4_xC0_Reserved_3_0_MASK                           0xF
#define DxFxxE4_xC0_Reserved_12_6_OFFSET                        6
#define DxFxxE4_xC0_Reserved_12_6_WIDTH                         7
#define DxFxxE4_xC0_Reserved_12_6_MASK                          0x1FC0
#define DxFxxE4_xC0_StrapForceCompliance_OFFSET                 13
#define DxFxxE4_xC0_StrapForceCompliance_WIDTH                  1
#define DxFxxE4_xC0_StrapForceCompliance_MASK                   0x2000
#define DxFxxE4_xC0_Reserved_14_14_OFFSET                       14
#define DxFxxE4_xC0_Reserved_14_14_WIDTH                        1
#define DxFxxE4_xC0_Reserved_14_14_MASK                         0x4000
#define DxFxxE4_xC0_StrapAutoRcSpeedNegotiationDis_OFFSET       15
#define DxFxxE4_xC0_StrapAutoRcSpeedNegotiationDis_WIDTH        1
#define DxFxxE4_xC0_StrapAutoRcSpeedNegotiationDis_MASK         0x8000
#define DxFxxE4_xC0_Reserved_31_19_OFFSET                       19
#define DxFxxE4_xC0_Reserved_31_19_WIDTH                        13
#define DxFxxE4_xC0_Reserved_31_19_MASK                         0xfff80000

/// DxFxxE4_xC0
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_3_0:4 ; ///<
    UINT32                                       StrapMedyTSxCount:2 ; ///<
    UINT32                                           Reserved_12_6:7 ; ///<
    UINT32                                    StrapForceCompliance:1 ; ///<
    UINT32                                          Reserved_14_14:1 ; ///<
    UINT32                          StrapAutoRcSpeedNegotiationDis:1 ; ///<
    UINT32                                    StrapLaneNegotiation:3 ; ///<
    UINT32                                          Reserved_31_19:13; ///<
  } Field;
  UINT32 Value;
} DxFxxE4_xC0_STRUCT;




typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_1_0:2; ///<
    UINT32                                             bit2:1; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                            Reserved_7_4:4; ///<
    UINT32                                            Reserved_8_8:1; ///<
    UINT32                                           Reserved_31_9:23; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB7208_STRUCT;


typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_16_0:17; ///<
    UINT32                                       bit17:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GnbRegistersKB7236_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                           Reserved_15_0:16; ///<
    UINT32                                      bit16:1; ///<
    UINT32                                          Reserved_17_17:1; ///<
    UINT32                                          Reserved_31_18:14; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB7269_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_7_0:8; ///<
    UINT32                           StrapBifF0LegacyDeviceTypeDis:1; ///<
    UINT32                                            Reserved_9_9:1; ///<
    UINT32                           bita:1; ///<
    UINT32                                          Reserved_12_11:2; ///<
    UINT32                                         bit13:1; ///<
    UINT32                                          Reserved_31_14:18; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB7314_STRUCT;

typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                    bit_20:1; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GnbRegistersKB7341_STRUCT;


// **** D0F0xD4_x010914E1 Register Definition ****
// Address
#define D0F0xD4_x010914E1_ADDRESS                               0x10914E1
// Type
#define D0F0xD4_x010914E1_TYPE                                  TYPE_D0F0xD4

// Field Data
#define D0F0xD4_x010914E1_Reserved_0_0_OFFSET                   0
#define D0F0xD4_x010914E1_Reserved_0_0_WIDTH                    1
#define D0F0xD4_x010914E1_Reserved_0_0_MASK                     0x1
#define D0F0xD4_x010914E1_StrapBifRegApSize_OFFSET              1
#define D0F0xD4_x010914E1_StrapBifRegApSize_WIDTH               2
#define D0F0xD4_x010914E1_StrapBifRegApSize_MASK                0x6
#define D0F0xD4_x010914E1_StrapBifMemApSize_OFFSET              3
#define D0F0xD4_x010914E1_StrapBifMemApSize_WIDTH               3
#define D0F0xD4_x010914E1_StrapBifMemApSize_MASK                0x38
#define D0F0xD4_x010914E1_Reserved_11_6_OFFSET                  6
#define D0F0xD4_x010914E1_Reserved_11_6_WIDTH                   6
#define D0F0xD4_x010914E1_Reserved_11_6_MASK                    0xFC0
#define D0F0xD4_x010914E1_StrapBifDoorbellBarDis_OFFSET         12
#define D0F0xD4_x010914E1_StrapBifDoorbellBarDis_WIDTH          1
#define D0F0xD4_x010914E1_StrapBifDoorbellBarDis_MASK           0x1000
#define D0F0xD4_x010914E1_Bitfield_13_13_OFFSET                 13
#define D0F0xD4_x010914E1_Bitfield_13_13_WIDTH                  1
#define D0F0xD4_x010914E1_Bitfield_13_13_MASK                   0x2000
#define D0F0xD4_x010914E1_Bitfield_15_14_OFFSET                 14
#define D0F0xD4_x010914E1_Bitfield_15_14_WIDTH                  2
#define D0F0xD4_x010914E1_Bitfield_15_14_MASK                   0xC000
#define D0F0xD4_x010914E1_Reserved_31_13_OFFSET                 16
#define D0F0xD4_x010914E1_Reserved_31_13_WIDTH                  16
#define D0F0xD4_x010914E1_Reserved_31_13_MASK                   0xFFFF0000

/// D0F0xD4_x010914E1
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_0_0:1; ///<
    UINT32                                       StrapBifRegApSize:2; ///<
    UINT32                                       StrapBifMemApSize:3; ///<
    UINT32                                           Reserved_11_6:6; ///<
    UINT32                                  StrapBifDoorbellBarDis:1; ///<
    UINT32                                          Bitfield_13_13:1; ///<
    UINT32                                          Bitfield_15_14:2; ///<
    UINT32                                          Reserved_31_13:16; ///<

  } Field;

  UINT32 Value;
} D0F0xD4_x010914E1_STRUCT;

// **** D0F0xD4_x010914E2 Register Definition ****
// Address
#define D0F0xD4_x010914E2_ADDRESS                               0x10914E2
// Type
#define D0F0xD4_x010914E2_TYPE                                  TYPE_D0F0xD4

// Field Data
#define D0F0xD4_x010914E2_Reserved_0_0_OFFSET                   0
#define D0F0xD4_x010914E2_Reserved_0_0_WIDTH                    1
#define D0F0xD4_x010914E2_Reserved_0_0_MASK                     0x1
#define D0F0xD4_x010914E2_StrapBifIoBarDis_OFFSET               1
#define D0F0xD4_x010914E2_StrapBifIoBarDis_WIDTH                1
#define D0F0xD4_x010914E2_StrapBifIoBarDis_MASK                 0x2
#define D0F0xD4_x010914E2_StrapBifF064BarDisA_OFFSET            3
#define D0F0xD4_x010914E2_StrapBifF064BarDisA_WIDTH             1
#define D0F0xD4_x010914E2_StrapBifF064BarDisA_MASK              0x8

/// D0F0xD4_x010914E2
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_0_0:1; ///<
    UINT32                                        StrapBifIoBarDis:1; ///<
    UINT32                                            Reserved_2_2:1; ///<
    UINT32                                     StrapBifF064BarDisA:1; ///<
    UINT32                                            Reserved_7_4:4; ///<
    UINT32                           bit8:1; ///<
    UINT32                                            Reserved_9_9:1; ///<
    UINT32                           bita:1; ///<
    UINT32                                          Reserved_12_11:2; ///<
    UINT32                                         bit13:1; ///<
    UINT32                                          Reserved_31_14:18; ///<

  } Field;

  UINT32 Value;
} D0F0xD4_x010914E2_STRUCT;

// **** D0F0xD4_x01091507 Register Definition ****
// Address
#define D0F0xD4_x01091507_ADDRESS                               0x1091507
// Type
#define D0F0xD4_x01091507_TYPE                                  TYPE_D0F0xD4

// Field Data
#define D0F0xD4_x01091507_StrapBifMemApSizePin_OFFSET           5
#define D0F0xD4_x01091507_StrapBifMemApSizePin_WIDTH            3
#define D0F0xD4_x01091507_StrapBifMemApSizePin_MASK             0xE0

/// D0F0xD4_x01091507
typedef union {
  struct {                                                            ///<
    UINT32                                            Reserved_4_0:5; ///<
    UINT32                                    StrapBifMemApSizePin:3; ///<
    UINT32                                           Reserved_15_8:8; ///<
    UINT32                                      bit16:1; ///<
    UINT32                                          Reserved_31_17:15; ///<

  } Field;

  UINT32 Value;
} D0F0xD4_x01091507_STRUCT;

typedef union {
  struct {                                                            ///<
    UINT32                                             bit0:1; ///<
    UINT32                                           Reserved_31_1:31; ///<

  } Field;

  UINT32 Value;
} GnbRegistersKB7514_STRUCT;


// **** D0F0xFC_x00 Register Definition ****
// Address
#define D0F0xFC_x00_ADDRESS                                     0x0
// Type
#define D0F0xFC_x00_TYPE                                        TYPE_D0F0xFC

// Field Data
#define D0F0xFC_x00_IoapicEnable_OFFSET                         0
#define D0F0xFC_x00_IoapicEnable_WIDTH                          1
#define D0F0xFC_x00_IoapicEnable_MASK                           0x1
#define D0F0xFC_x00_Reserved_1_1_OFFSET                         1
#define D0F0xFC_x00_Reserved_1_1_WIDTH                          1
#define D0F0xFC_x00_Reserved_1_1_MASK                           0x2
#define D0F0xFC_x00_IoapicIdExtEn_OFFSET                        2
#define D0F0xFC_x00_IoapicIdExtEn_WIDTH                         1
#define D0F0xFC_x00_IoapicIdExtEn_MASK                          0x4
#define D0F0xFC_x00_Reserved_3_3_OFFSET                         3
#define D0F0xFC_x00_Reserved_3_3_WIDTH                          1
#define D0F0xFC_x00_Reserved_3_3_MASK                           0x8
#define D0F0xFC_x00_IoapicSbFeatureEn_OFFSET                    4
#define D0F0xFC_x00_IoapicSbFeatureEn_WIDTH                     1
#define D0F0xFC_x00_IoapicSbFeatureEn_MASK                      0x10
#define D0F0xFC_x00_Reserved_31_5_OFFSET                        5
#define D0F0xFC_x00_Reserved_31_5_WIDTH                         27
#define D0F0xFC_x00_Reserved_31_5_MASK                          0xFFFFFFE0

/// D0F0xFC_x00
typedef union {
  struct {                                                            ///<
    UINT32                                            IoapicEnable:1; ///<
    UINT32                                            Reserved_1_1:1; ///<
    UINT32                                           IoapicIdExtEn:1; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                       IoapicSbFeatureEn:1; ///<
    UINT32                                           Reserved_31_5:27; ///<

  } Field;

  UINT32 Value;
} D0F0xFC_x00_STRUCT;

// **** D0F0xFC_x0F Register Definition ****
// Address
#define D0F0xFC_x0F_ADDRESS                                     0x0F
// Type
#define D0F0xFC_x0F_TYPE                                        TYPE_D0F0xFC

// Field Data
#define D0F0xFC_x0F_GBIFExtIntrGrp_OFFSET                         0
#define D0F0xFC_x0F_GBIFExtIntrGrp_WIDTH                          3
#define D0F0xFC_x0F_GBIFExtIntrGrp_MASK                           0x7
#define D0F0xFC_x0F_Reserved_3_3_OFFSET                           3
#define D0F0xFC_x0F_Reserved_3_3_WIDTH                            1
#define D0F0xFC_x0F_Reserved_3_3_MASK                             0x8
#define D0F0xFC_x0F_GBIFExtIntrSwz_OFFSET                         4
#define D0F0xFC_x0F_GBIFExtIntrSwz_WIDTH                          2
#define D0F0xFC_x0F_GBIFExtIntrSwz_MASK                           0x30
#define D0F0xFC_x0F_Reserved_31_6_OFFSET                          6
#define D0F0xFC_x0F_Reserved_31_6_WIDTH                           26
#define D0F0xFC_x0F_Reserved_31_6_MASK                            0xFFFFFFC0

/// D0F0xFC_xOF
typedef union {
  struct {                                                            ///<
    UINT32                                            GBIFExtIntrGrp:3; ///<
    UINT32                                              Reserved_3_3:1; ///<
    UINT32                                            GBIFExtIntrSwz:2; ///<
    UINT32                                             Reserved_31_6:26; ///<
  } Field;

  UINT32 Value;
} D0F0xFC_x0F_STRUCT;

// **** D0F0xFC_x10 Register Definition ****
// Address
#define D0F0xFC_x10_ADDRESS                                     0x10
// Type
#define D0F0xFC_x10_TYPE                                        TYPE_D0F0xFC

// Field Data
#define D0F0xFC_x10_BrExtIntrGrp_OFFSET                         0
#define D0F0xFC_x10_BrExtIntrGrp_WIDTH                          3
#define D0F0xFC_x10_BrExtIntrGrp_MASK                           0x7
#define D0F0xFC_x10_Reserved_3_3_OFFSET                         3
#define D0F0xFC_x10_Reserved_3_3_WIDTH                          1
#define D0F0xFC_x10_Reserved_3_3_MASK                           0x8
#define D0F0xFC_x10_BrExtIntrSwz_OFFSET                         4
#define D0F0xFC_x10_BrExtIntrSwz_WIDTH                          2
#define D0F0xFC_x10_BrExtIntrSwz_MASK                           0x30
#define D0F0xFC_x10_Reserved_15_6_OFFSET                        6
#define D0F0xFC_x10_Reserved_15_6_WIDTH                         10
#define D0F0xFC_x10_Reserved_15_6_MASK                          0xFFC0
#define D0F0xFC_x10_BrIntIntrMap_OFFSET                         16
#define D0F0xFC_x10_BrIntIntrMap_WIDTH                          5
#define D0F0xFC_x10_BrIntIntrMap_MASK                           0x1F0000
#define D0F0xFC_x10_Reserved_31_21_OFFSET                       21
#define D0F0xFC_x10_Reserved_31_21_WIDTH                        11
#define D0F0xFC_x10_Reserved_31_21_MASK                         0xFFE00000

/// D0F0xFC_x10
typedef union {
  struct {                                                            ///<
    UINT32                                            BrExtIntrGrp:3; ///<
    UINT32                                            Reserved_3_3:1; ///<
    UINT32                                            BrExtIntrSwz:2; ///<
    UINT32                                           Reserved_15_6:10; ///<
    UINT32                                            BrIntIntrMap:5; ///<
    UINT32                                          Reserved_31_21:11; ///<

  } Field;

  UINT32 Value;
} D0F0xFC_x10_STRUCT;


// **** D0F0x90 Register Definition ****
// Address
#define D0F0x90_ADDRESS                                         0x90
// Type
#define D0F0x90_TYPE                                            TYPE_D0F0

// **** D0F0x94 Register Definition ****
// Address
#define D0F0x94_ADDRESS                                         0x94
// Type
#define D0F0x94_TYPE                                            TYPE_D0F0


// **** D18F1xF0 Register Definition ****
// Address
#define D18F1xF0_ADDRESS                                        0xF0
// Type
#define D18F1xF0_TYPE                                           TYPE_D18F1

// **** D18F1x200 Register Definition ****
// Address
#define D18F1x200_ADDRESS                                       0x200
// Type
#define D18F1x200_TYPE                                          TYPE_D18F1

// **** D18F1x204 Register Definition ****
// Address
#define D18F1x204_ADDRESS                                       0x204
// Type
#define D18F1x204_TYPE                                          TYPE_D18F1

// **** D18F1x240 Register Definition ****
// Address
#define D18F1x240_ADDRESS                                       0x240
// Type
#define D18F1x240_TYPE                                          TYPE_D18F1

// **** D18F2x40_dct0 Register Definition ****
// Address
#define D18F2x40_dct0_ADDRESS                                   0x40
// Type
#define D18F2x40_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x44_dct0 Register Definition ****
// Address
#define D18F2x44_dct0_ADDRESS                                   0x44
// Type
#define D18F2x44_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x48_dct0 Register Definition ****
// Address
#define D18F2x48_dct0_ADDRESS                                   0x48
// Type
#define D18F2x48_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x4C_dct0 Register Definition ****
// Address
#define D18F2x4C_dct0_ADDRESS                                   0x4C
// Type
#define D18F2x4C_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x60_dct0 Register Definition ****
// Address
#define D18F2x60_dct0_ADDRESS                                   0x60
// Type
#define D18F2x60_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x64_dct0 Register Definition ****
// Address
#define D18F2x64_dct0_ADDRESS                                   0x64
// Type
#define D18F2x64_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x80_dct0 Register Definition ****
// Address
#define D18F2x80_dct0_ADDRESS                                   0x80
// Type
#define D18F2x80_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2xA8_dct0 Register Definition ****
// Address
#define D18F2xA8_dct0_ADDRESS                                   0xA8
// Type
#define D18F2xA8_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x78_dct0 Register Definition ****
// Address
#define D18F2x78_dct0_ADDRESS                                   0x78
// Type
#define D18F2x78_dct0_TYPE                                      TYPE_D18F2_dct0

// **** D18F2x110 Register Definition ****
// Address
#define D18F2x110_ADDRESS                                       0x110
// Type
#define D18F2x110_TYPE                                          TYPE_D18F2

// **** D18F2x114 Register Definition ****
// Address
#define D18F2x114_ADDRESS                                       0x114
// Type
#define D18F2x114_TYPE                                          TYPE_D18F2

// **** DxFxx18 Register Definition ****
// Address
#define DxFxx18_ADDRESS                                         0x18
// Type
#define DxFxx18_TYPE                                            TYPE_D2F1

// **** DxFxx20 Register Definition ****
// Address
#define DxFxx20_ADDRESS                                         0x20
// Type
#define DxFxx20_TYPE                                            TYPE_D2F1

// **** DxFxx24 Register Definition ****
// Address
#define DxFxx24_ADDRESS                                         0x24
// Type
#define DxFxx24_TYPE                                            TYPE_D2F1

// **** D0F0x60 Register Definition ****
// Address
#define D0F0x60_ADDRESS                                         0x60
// Type
#define D0F0x60_TYPE                                            TYPE_D0F0

// **** D0F0xB8 Register Definition ****
// Address
#define D0F0xB8_ADDRESS                                         0xB8
// Type
#define D0F0xB8_TYPE                                            TYPE_D0F0

// **** D0F0xE0 Register Definition ****
// Address
#define D0F0xE0_ADDRESS                                         0xE0
// Type
#define D0F0xE0_TYPE                                            TYPE_D0F0
// **** D0F0x64_x1F Register Definition ****
// Address
#define D0F0x64_x1F_ADDRESS                                     0x1F
// Type
#define D0F0x64_x1F_TYPE                                        TYPE_D0F0x64

// **** D0F0xE4_PIF_0017 Register Definition ****
// Address
#define D0F0xE4_PIF_0017_ADDRESS                                0x17
// Type
#define D0F0xE4_PIF_0017_TYPE                                   TYPE_D0F0xE4

// **** D0F2xF4_x49 Register Definition ****
// Address
// Type


// **** D0F0xE4_WRAP_8021 Register Definition ****
// Address
#define D0F0xE4_WRAP_8021_ADDRESS                               0x8021
// Type
#define D0F0xE4_WRAP_8021_TYPE                                  TYPE_D0F0xE4

// **** D0F0xE4_WRAP_8022 Register Definition ****
// Address
#define D0F0xE4_WRAP_8022_ADDRESS                               0x8022
// Type
#define D0F0xE4_WRAP_8022_TYPE                                  TYPE_D0F0xE4

// **** D0F0xE4_WRAP_8025 Register Definition ****
// Address
#define D0F0xE4_WRAP_8025_ADDRESS                               0x8025
// Type
#define D0F0xE4_WRAP_8025_TYPE                                  TYPE_D0F0xE4

// **** D0F0xE4_WRAP_8026 Register Definition ****
// Address
#define D0F0xE4_WRAP_8026_ADDRESS                               0x8026
// Type
#define D0F0xE4_WRAP_8026_TYPE                                  TYPE_D0F0xE4

// **** D0F0xF8 Register Definition ****
// Address
#define D0F0xF8_ADDRESS                                         0xF8
// Type
#define D0F0xF8_TYPE                                            TYPE_D0F0

// **** D0F0x64_x19 Register Definition ****
// Address
#define D0F0x64_x19_ADDRESS                                     0x19
// Type
#define D0F0x64_x19_TYPE                                        TYPE_D0F0x64

// **** D0F0x64_x1A Register Definition ****
// Address
#define D0F0x64_x1A_ADDRESS                                     0x1A
// Type
#define D0F0x64_x1A_TYPE                                        TYPE_D0F0x64

// **** D0F0xBC_x20000 Register Definition ****
// Address
#define D0F0xBC_x20000_ADDRESS                                  0x20000
// Type
#define D0F0xBC_x20000_TYPE                                     TYPE_D0F0xBC

// **** D0F0xBC_x0 Register Definition ****
// Address
#define D0F0xBC_x0_ADDRESS                                      0x0
// Type
#define D0F0xBC_x0_TYPE                                         TYPE_D0F0xBC



// **** D0F0xBC_xC210003C Register Definition ****
// Address
#define D0F0xBC_xC210003C_ADDRESS                               0xC210003C
// Type
#define D0F0xBC_xC210003C_TYPE                                  TYPE_D0F0xBC

// **** D0F0xD4_x010914C3 Register Definition ****
// Address
#define D0F0xD4_x010914C3_ADDRESS                               0x010914C3
// Type
#define D0F0xD4_x010914C3_TYPE                                  TYPE_D0F0xD4

#endif
