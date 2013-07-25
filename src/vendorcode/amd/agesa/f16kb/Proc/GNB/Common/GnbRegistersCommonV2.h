/* $NoKeywords:$ */
/**
 * @file
 *
 *  Register definitions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87271 $   @e \$Date: 2013-01-31 10:11:23 -0600 (Thu, 31 Jan 2013) $
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

#ifndef _GNBREGISTERS_CV2_H_
#define _GNBREGISTERS_CV2_H_

#define  SMC_MSG_FIRMWARE_AUTH              0

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


// **** D0F0xFC_x01 Register Definition ****
// Address
#define D0F0xFC_x01_ADDRESS                                     0x01

// **** D0F0xFC_x02 Register Definition ****
// Address
#define D0F0xFC_x02_ADDRESS                                     0x02

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

// **** D0F0xB8 Register Definition ****
// Address
#define D0F0xB8_ADDRESS                                         0xb8

// **** DxF0xE4_xC1 Register Definition ****
// Address
#define D2FxxE4_xC1_ADDRESS                                     0xc1

// Type
#define D2FxxE4_xC1_TYPE                                        TYPE_D4F0xE4
// Field Data

#define D2FxxE4_xC1_StrapReverseLanes_OFFSET                    0
#define D2FxxE4_xC1_StrapReverseLanes_WIDTH                     1
#define D2FxxE4_xC1_StrapReverseLanes_MASK                      0x1
#define D2FxxE4_xC1_StrapE2EPrefixEn_OFFSET                     1
#define D2FxxE4_xC1_StrapE2EPrefixEn_WIDTH                      1
#define D2FxxE4_xC1_StrapE2EPrefixEn_MASK                       0x2
#define D2FxxE4_xC1_StrapExtendedFmtSupported_OFFSET            2
#define D2FxxE4_xC1_StrapExtendedFmtSupported_WIDTH             1
#define D2FxxE4_xC1_StrapExtendedFmtSupported_MASK              0x4
#define D2FxxE4_xC1_Reserved_31_3_OFFSET                        3
#define D2FxxE4_xC1_Reserved_31_3_WIDTH                         29
#define D2FxxE4_xC1_Reserved_31_3_MASK                          0xfffffff8

// Type
// Address
#define D0F0xE4_WRAP_0046_ADDRESS                               0x46

// Address
#define D0F0xE4_WRAP_8040_ADDRESS                               0x8040


// **** D0F0xE4_WRAP_8062 Register Definition ****
// Address
#define D0F0xE4_WRAP_8062_ADDRESS                               0x8062


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


// Type
#define D0F0xE4_WRAP_8040_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8040_OwnSlice_OFFSET                       0
#define D0F0xE4_WRAP_8040_OwnSlice_WIDTH                        1
#define D0F0xE4_WRAP_8040_OwnSlice_MASK                         0x1


// **** D0F0xBC_x1F630 Register Definition ****
// Address
#define D0F0xBC_x1F630_ADDRESS                                  0x1f630

// Type
#define D0F0xBC_x1F630_TYPE                                     TYPE_D0F0xBC
// Field Data
#define D0F0xBC_x1F630_RECONF_WRAPPER_OFFSET                    8
#define D0F0xBC_x1F630_RECONF_WRAPPER_WIDTH                     8
#define D0F0xBC_x1F630_RECONF_WRAPPER_MASK                      0x00ff00


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
#define D0F0xE4_WRAP_8011_Bitfield_16_16_OFFSET                 16
#define D0F0xE4_WRAP_8011_Bitfield_16_16_WIDTH                  1
#define D0F0xE4_WRAP_8011_Bitfield_16_16_MASK                   0x10000
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_OFFSET           17
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_MASK             0x7E0000
#define D0F0xE4_WRAP_8011_Reserved_23_23_OFFSET                 23
#define D0F0xE4_WRAP_8011_Reserved_23_23_WIDTH                  1
#define D0F0xE4_WRAP_8011_Reserved_23_23_MASK                   0x800000
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
    UINT32                                          Bitfield_16_16:1; ///<
    UINT32                                    TxclkPermGateLatency:6; ///<
    UINT32                                          Reserved_23_23:1; ///<
    UINT32                                     TxclkLcntGateEnable:1; ///<
    UINT32                                          Reserved_25_25:1; ///<
    UINT32                                          Reserved_31_26:6; ///<

  } Field;

  UINT32 Value;
} D0F0xE4_WRAP_8011_STRUCT;



// **** D0F0xE4_WRAP_8016 Register Definition ****
// Address
#define D0F0xE4_WRAP_8016_ADDRESS                               0x8016

// Type
#define D0F0xE4_WRAP_8016_TYPE                                  TYPE_D0F0xE4

/// D0F0xE4_WRAP_8016
typedef union {
  struct {                                                              ///<
    UINT32                                          CalibAckLatency:6 ; ///<
    UINT32                                            Reserved_15_6:10; ///<
    UINT32                                       LclkDynGateLatency:6 ; ///<
    UINT32                                             LclkGateFree:1 ; ///<
    UINT32                                        LclkDynGateEnable:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8016_STRUCT;


// **** D0F0xE4_CORE_0011 Register Definition ****
// Address
#define D0F0xE4_CORE_0011_ADDRESS                               0x11

#define D0F0xE4_CORE_0011_DynClkLatency_OFFSET                  0
#define D0F0xE4_CORE_0011_DynClkLatency_WIDTH                   4
#define D0F0xE4_CORE_0011_DynClkLatency_MASK                    0xf


// **** D0F0xE4_PIF_0010 Register Definition ****
// Address
#define D0F0xE4_PIF_0010_ADDRESS                                0x10

// Type
#define D0F0xE4_PIF_0010_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PIF_0010_Reserved_3_0_OFFSET                    0
#define D0F0xE4_PIF_0010_Reserved_3_0_WIDTH                     4
#define D0F0xE4_PIF_0010_Reserved_3_0_MASK                      0xf
#define D0F0xE4_PIF_0010_EiDetCycleMode_OFFSET                  4
#define D0F0xE4_PIF_0010_EiDetCycleMode_WIDTH                   1
#define D0F0xE4_PIF_0010_EiDetCycleMode_MASK                    0x10
#define D0F0xE4_PIF_0010_Reserved_5_5_OFFSET                    5
#define D0F0xE4_PIF_0010_Reserved_5_5_WIDTH                     1
#define D0F0xE4_PIF_0010_Reserved_5_5_MASK                      0x20
#define D0F0xE4_PIF_0010_RxDetectFifoResetMode_OFFSET           6
#define D0F0xE4_PIF_0010_RxDetectFifoResetMode_WIDTH            1
#define D0F0xE4_PIF_0010_RxDetectFifoResetMode_MASK             0x40
#define D0F0xE4_PIF_0010_RxDetectTxPwrMode_OFFSET               7
#define D0F0xE4_PIF_0010_RxDetectTxPwrMode_WIDTH                1
#define D0F0xE4_PIF_0010_RxDetectTxPwrMode_MASK                 0x80
#define D0F0xE4_PIF_0010_Reserved_16_8_OFFSET                   8
#define D0F0xE4_PIF_0010_Reserved_16_8_WIDTH                    9
#define D0F0xE4_PIF_0010_Reserved_16_8_MASK                     0x1ff00
#define D0F0xE4_PIF_0010_Ls2ExitTime_OFFSET                     17
#define D0F0xE4_PIF_0010_Ls2ExitTime_WIDTH                      3
#define D0F0xE4_PIF_0010_Ls2ExitTime_MASK                       0xe0000
#define D0F0xE4_PIF_0010_Reserved_31_23_OFFSET                  23
#define D0F0xE4_PIF_0010_Reserved_31_23_WIDTH                   9
#define D0F0xE4_PIF_0010_Reserved_31_23_MASK                    0xff800000

/// D0F0xE4_PIF_0010
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                           EiDetCycleMode:1 ; ///<
    UINT32                                             Reserved_5_5:1 ; ///<
    UINT32                                    RxDetectFifoResetMode:1 ; ///<
    UINT32                                        RxDetectTxPwrMode:1 ; ///<
    UINT32                                            Reserved_16_8:9 ; ///<
    UINT32                                              Ls2ExitTime:3 ; ///<
    UINT32                                                         :3 ; ///<
    UINT32                                           Reserved_31_23:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PIF_0010_STRUCT;

// **** D2FxxE4_xA4 Register Definition ****
// Address
#define D2FxxE4_xA4_ADDRESS                                     0xa4

// Type
#define D2FxxE4_xA4_TYPE                                        TYPE_D4F0xE4
// Field Data
#define D2FxxE4_xA4_LcGen2EnStrap_OFFSET                        0
#define D2FxxE4_xA4_LcGen2EnStrap_WIDTH                         1
#define D2FxxE4_xA4_LcGen2EnStrap_MASK                          0x1
#define D2FxxE4_xA4_Reserved_5_1_OFFSET                         1
#define D2FxxE4_xA4_Reserved_5_1_WIDTH                          5
#define D2FxxE4_xA4_Reserved_5_1_MASK                           0x3e
#define D2FxxE4_xA4_LcForceDisSwSpeedChange_OFFSET              6
#define D2FxxE4_xA4_LcForceDisSwSpeedChange_WIDTH               1
#define D2FxxE4_xA4_LcForceDisSwSpeedChange_MASK                0x40
#define D2FxxE4_xA4_Reserved_8_7_OFFSET                         7
#define D2FxxE4_xA4_Reserved_8_7_WIDTH                          2
#define D2FxxE4_xA4_Reserved_8_7_MASK                           0x180
#define D2FxxE4_xA4_LcInitiateLinkSpeedChange_OFFSET            9
#define D2FxxE4_xA4_LcInitiateLinkSpeedChange_WIDTH             1
#define D2FxxE4_xA4_LcInitiateLinkSpeedChange_MASK              0x200
#define D2FxxE4_xA4_Reserved_11_10_OFFSET                       10
#define D2FxxE4_xA4_Reserved_11_10_WIDTH                        2
#define D2FxxE4_xA4_Reserved_11_10_MASK                         0xc00
#define D2FxxE4_xA4_LcSpeedChangeAttemptFailed_OFFSET           12
#define D2FxxE4_xA4_LcSpeedChangeAttemptFailed_WIDTH            1
#define D2FxxE4_xA4_LcSpeedChangeAttemptFailed_MASK             0x1000
#define D2FxxE4_xA4_Reserved_18_13_OFFSET                       13
#define D2FxxE4_xA4_Reserved_18_13_WIDTH                        6
#define D2FxxE4_xA4_Reserved_18_13_MASK                         0x7e000
#define D2FxxE4_xA4_LcOtherSideSupportsGen2_OFFSET              19
#define D2FxxE4_xA4_LcOtherSideSupportsGen2_WIDTH               1
#define D2FxxE4_xA4_LcOtherSideSupportsGen2_MASK                0x80000
#define D2FxxE4_xA4_Reserved_26_20_OFFSET                       20
#define D2FxxE4_xA4_Reserved_26_20_WIDTH                        7
#define D2FxxE4_xA4_Reserved_26_20_MASK                         0x7f00000
#define D2FxxE4_xA4_LcMultUpstreamAutoSpdChngEn_OFFSET          27
#define D2FxxE4_xA4_LcMultUpstreamAutoSpdChngEn_WIDTH           1
#define D2FxxE4_xA4_LcMultUpstreamAutoSpdChngEn_MASK            0x8000000
#define D2FxxE4_xA4_Reserved_31_28_OFFSET                       28
#define D2FxxE4_xA4_Reserved_31_28_WIDTH                        4
#define D2FxxE4_xA4_Reserved_31_28_MASK                         0xf0000000

/// D2FxxE4_xA4
typedef union {
  struct {                                                              ///<
    UINT32                                            LcGen2EnStrap:1 ; ///<
    UINT32                                             Reserved_5_1:5 ; ///<
    UINT32                                  LcForceDisSwSpeedChange:1 ; ///<
    UINT32                                             Reserved_8_7:2 ; ///<
    UINT32                                LcInitiateLinkSpeedChange:1 ; ///<
    UINT32                                           Reserved_11_10:2 ; ///<
    UINT32                               LcSpeedChangeAttemptFailed:1 ; ///<
    UINT32                                           Reserved_18_13:6 ; ///<
    UINT32                                  LcOtherSideSupportsGen2:1 ; ///<
    UINT32                                           Reserved_26_20:7 ; ///<
    UINT32                              LcMultUpstreamAutoSpdChngEn:1 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D2FxxE4_xA4_STRUCT;


// **** D2FxxE4_xC0 Register Definition ****
// Address
#define D2FxxE4_xC0_ADDRESS                                     0xc0

// Type
#define D2FxxE4_xC0_TYPE                                        TYPE_D4F0xE4
// Field Data
#define D2FxxE4_xC0_Reserved_12_0_OFFSET                        0
#define D2FxxE4_xC0_Reserved_12_0_WIDTH                         13
#define D2FxxE4_xC0_Reserved_12_0_MASK                          0x1fff
#define D2FxxE4_xC0_StrapForceCompliance_OFFSET                 13
#define D2FxxE4_xC0_StrapForceCompliance_WIDTH                  1
#define D2FxxE4_xC0_StrapForceCompliance_MASK                   0x2000
#define D2FxxE4_xC0_Reserved_14_14_OFFSET                       14
#define D2FxxE4_xC0_Reserved_14_14_WIDTH                        1
#define D2FxxE4_xC0_Reserved_14_14_MASK                         0x4000
#define D2FxxE4_xC0_StrapAutoRcSpeedNegotiationDis_OFFSET       15
#define D2FxxE4_xC0_StrapAutoRcSpeedNegotiationDis_WIDTH        1
#define D2FxxE4_xC0_StrapAutoRcSpeedNegotiationDis_MASK         0x8000
#define D2FxxE4_xC0_Reserved_31_16_OFFSET                       16
#define D2FxxE4_xC0_Reserved_31_16_WIDTH                        16
#define D2FxxE4_xC0_Reserved_31_16_MASK                         0xffff0000

/// D2FxxE4_xC0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_12_0:13; ///<
    UINT32                                     StrapForceCompliance:1 ; ///<
    UINT32                                           Reserved_14_14:1 ; ///<
    UINT32                           StrapAutoRcSpeedNegotiationDis:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D2FxxE4_xC0_STRUCT;


// **** D2Fxx88 Register Definition ****
// Address
#define D2Fxx88_ADDRESS                                         0x88

// Type
#define D2Fxx88_TYPE                                            TYPE_D4F0
// Field Data
#define D2Fxx88_TargetLinkSpeed_OFFSET                          0
#define D2Fxx88_TargetLinkSpeed_WIDTH                           4
#define D2Fxx88_TargetLinkSpeed_MASK                            0xf
#define D2Fxx88_EnterCompliance_OFFSET                          4
#define D2Fxx88_EnterCompliance_WIDTH                           1
#define D2Fxx88_EnterCompliance_MASK                            0x10
#define D2Fxx88_HwAutonomousSpeedDisable_OFFSET                 5
#define D2Fxx88_HwAutonomousSpeedDisable_WIDTH                  1
#define D2Fxx88_HwAutonomousSpeedDisable_MASK                   0x20
#define D2Fxx88_SelectableDeemphasis_OFFSET                     6
#define D2Fxx88_SelectableDeemphasis_WIDTH                      1
#define D2Fxx88_SelectableDeemphasis_MASK                       0x40
#define D2Fxx88_XmitMargin_OFFSET                               7
#define D2Fxx88_XmitMargin_WIDTH                                3
#define D2Fxx88_XmitMargin_MASK                                 0x380
#define D2Fxx88_EnterModCompliance_OFFSET                       10
#define D2Fxx88_EnterModCompliance_WIDTH                        1
#define D2Fxx88_EnterModCompliance_MASK                         0x400
#define D2Fxx88_ComplianceSOS_OFFSET                            11
#define D2Fxx88_ComplianceSOS_WIDTH                             1
#define D2Fxx88_ComplianceSOS_MASK                              0x800
#define D2Fxx88_ComplianceDeemphasis_OFFSET                     12
#define D2Fxx88_ComplianceDeemphasis_WIDTH                      1
#define D2Fxx88_ComplianceDeemphasis_MASK                       0x1000
#define D2Fxx88_Reserved_15_13_OFFSET                           13
#define D2Fxx88_Reserved_15_13_WIDTH                            3
#define D2Fxx88_Reserved_15_13_MASK                             0xe000
#define D2Fxx88_CurDeemphasisLevel_OFFSET                       16
#define D2Fxx88_CurDeemphasisLevel_WIDTH                        1
#define D2Fxx88_CurDeemphasisLevel_MASK                         0x10000
#define D2Fxx88_Reserved_31_17_OFFSET                           17
#define D2Fxx88_Reserved_31_17_WIDTH                            15
#define D2Fxx88_Reserved_31_17_MASK                             0xfffe0000

/// D2Fxx88
typedef union {
  struct {                                                              ///<
    UINT32                                          TargetLinkSpeed:4 ; ///<
    UINT32                                          EnterCompliance:1 ; ///<
    UINT32                                 HwAutonomousSpeedDisable:1 ; ///<
    UINT32                                     SelectableDeemphasis:1 ; ///<
    UINT32                                               XmitMargin:3 ; ///<
    UINT32                                       EnterModCompliance:1 ; ///<
    UINT32                                            ComplianceSOS:1 ; ///<
    UINT32                                     ComplianceDeemphasis:1 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                       CurDeemphasisLevel:1 ; ///<
    UINT32                                           Reserved_31_17:15; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D2Fxx88_STRUCT;


// **** D0F0xE4_WRAP_0803 Register Definition ****
// Address
#define D0F0xE4_WRAP_0803_ADDRESS                               0x803

// Type
#define D0F0xE4_WRAP_0803_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_0803_Reserved_4_0_OFFSET                   0
#define D0F0xE4_WRAP_0803_Reserved_4_0_WIDTH                    5
#define D0F0xE4_WRAP_0803_Reserved_4_0_MASK                     0x1f
#define D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_OFFSET          5
#define D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_WIDTH           1
#define D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_MASK            0x20
#define D0F0xE4_WRAP_0803_Reserved_31_6_OFFSET                  6
#define D0F0xE4_WRAP_0803_Reserved_31_6_WIDTH                   26
#define D0F0xE4_WRAP_0803_Reserved_31_6_MASK                    0xffffffc0

/// D0F0xE4_WRAP_0803
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                    StrapBifDeemphasisSel:1 ; ///<
    UINT32                                            Reserved_31_6:26; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_0803_STRUCT;

/*
 *  Needed by GnbNbInitLibV4.c
 */

// **** D0F0x60 Register Definition ****
// Address
#define D0F0x60_ADDRESS                                         0x60

// **** D0F0x90 Register Definition ****
// Address
#define D0F0x90_ADDRESS                                         0x90

// **** D0F0x64_x19 Register Definition ****
// Address
#define D0F0x64_x19_ADDRESS                                     0x19

// **** D0F0x64_x1A Register Definition ****
// Address
#define D0F0x64_x1A_ADDRESS                                     0x1a





/// D0F0xBC_x80010000
typedef union {
UINT32 Value;                                                         ///<
} GRC1_STRUCT;






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
#define D0F0xE4_CORE_0010_Reserved_8_1_MASK                     0x1fe
#define D0F0xE4_CORE_0010_UmiNpMemWrite_OFFSET                  9
#define D0F0xE4_CORE_0010_UmiNpMemWrite_WIDTH                   1
#define D0F0xE4_CORE_0010_UmiNpMemWrite_MASK                    0x200
#define D0F0xE4_CORE_0010_RxSbAdjPayloadSize_OFFSET             10
#define D0F0xE4_CORE_0010_RxSbAdjPayloadSize_WIDTH              3
#define D0F0xE4_CORE_0010_RxSbAdjPayloadSize_MASK               0x1c00
#define D0F0xE4_CORE_0010_Reserved_31_13_OFFSET                 13
#define D0F0xE4_CORE_0010_Reserved_31_13_WIDTH                  19
#define D0F0xE4_CORE_0010_Reserved_31_13_MASK                   0xffffe000

/// D0F0xE4_CORE_0010
typedef union {
  struct {                                                              ///<
    UINT32                                             HwInitWrLock:1 ; ///<
    UINT32                                             Reserved_8_1:8 ; ///<
    UINT32                                            UmiNpMemWrite:1 ; ///<
    UINT32                                       RxSbAdjPayloadSize:3 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0010_STRUCT;


// **** D0F0x94 Register Definition ****
// Address
#define D0F0x94_ADDRESS                                         0x94


// **** D0F0x98_x3A Register Definition ****
// Address
#define D0F0x98_x3A_ADDRESS                                     0x3a


// **** D18F0x110 Register Definition ****
// Address
#define D18F0x110_ADDRESS                                       0x110


// **** D0F0x98_x06 Register Definition ****
// Address
#define D0F0x98_x06_ADDRESS                                     0x6

#define D0F0x98_x06_UmiNpMemWrEn_OFFSET                         26
#define D0F0x98_x06_UmiNpMemWrEn_WIDTH                          1
#define D0F0x98_x06_UmiNpMemWrEn_MASK                           0x4000000

/*
 *  Needed by GnbNbInitLibV4.c - end
 */

/*
 *  Needed by GnbIommuScratch.c
 */




/*
 *  Needed by GnbIommuScratch.c - end
 */

/*
 *  Needed by GnbIoapic.c
 */


// **** D0F0xF8 Register Definition ****
// Address
#define D0F0xF8_ADDRESS                                         0xf8

// **** D0F0xFC_x00 Register Definition ****
// Address
#define D0F0xFC_x00_ADDRESS                                     0x0

// Type
#define D0F0xFC_x00_TYPE                                        TYPE_D0F0xFC
// Field Data
#define D0F0xFC_x00_IoapicEnable_OFFSET                         0
#define D0F0xFC_x00_IoapicEnable_WIDTH                          1
#define D0F0xFC_x00_IoapicEnable_MASK                           0x01
#define D0F0xFC_x00_Reserved_1_1_OFFSET                         2
#define D0F0xFC_x00_Reserved_1_1_WIDTH                          1
#define D0F0xFC_x00_Reserved_1_1_MASK                           0x02
#define D0F0xFC_x00_IoapicIdExtEn_OFFSET                        3
#define D0F0xFC_x00_IoapicIdExtEn_WIDTH                         1
#define D0F0xFC_x00_IoapicIdExtEn_MASK                          0x04
#define D0F0xFC_x00_Reserved_3_3_OFFSET                         4
#define D0F0xFC_x00_Reserved_3_3_WIDTH                          1
#define D0F0xFC_x00_Reserved_3_3_MASK                           0x08
#define D0F0xFC_x00_IoapicSbFeatureEn_OFFSET                    4
#define D0F0xFC_x00_IoapicSbFeatureEn_WIDTH                     1
#define D0F0xFC_x00_IoapicSbFeatureEn_MASK                      0x10
#define D0F0xFC_x00_Reserved_31_5_OFFSET                        5
#define D0F0xFC_x00_Reserved_31_5_WIDTH                         27
#define D0F0xFC_x00_Reserved_31_5_MASK                          0xffffffe0

/// D0F0xFC_x00
typedef union {
  struct {                                                              ///<
    UINT32                                             IoapicEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                            IoapicIdExtEn:1 ; ///<
    UINT32                                             Reserved_3_3:1 ; ///<
    UINT32                                        IoapicSbFeatureEn:1 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xFC_x00_STRUCT;

/*
 * Needed by GnbIoapic.c - end
 */

/*
 * Needed by GnbPcieInitLibV5
 */


// **** D0F0xE4_WRAP_8021 Register Definition ****
// Address
#define D0F0xE4_WRAP_8021_ADDRESS                               0x8021


// **** D0F0xCC_x01 Register Definition ****
// Address
#define D0F0xCC_x01_ADDRESS                                      0x1

// Type
#define D0F0xCC_x01_TYPE                                         TYPE_D0F0xCC
// Field Data
#define D0F0xCC_x01_BridgeDis_OFFSET                             0
#define D0F0xCC_x01_BridgeDis_WIDTH                              1
#define D0F0xCC_x01_BridgeDis_MASK                               0x1
#define D0F0xCC_x01_Reserved_1_1_OFFSET                          1
#define D0F0xCC_x01_Reserved_1_1_WIDTH                           1
#define D0F0xCC_x01_Reserved_1_1_MASK                            0x2
#define D0F0xCC_x01_CfgDis_OFFSET                                2
#define D0F0xCC_x01_CfgDis_WIDTH                                 1
#define D0F0xCC_x01_CfgDis_MASK                                  0x4
#define D0F0xCC_x01_P2pDis_OFFSET                                3
#define D0F0xCC_x01_P2pDis_WIDTH                                 1
#define D0F0xCC_x01_P2pDis_MASK                                  0x8
#define D0F0xCC_x01_Reserved_15_4_OFFSET                         4
#define D0F0xCC_x01_Reserved_15_4_WIDTH                          12
#define D0F0xCC_x01_Reserved_15_4_MASK                           0xfff0
#define D0F0xCC_x01_ExtDevPlug_OFFSET                            16
#define D0F0xCC_x01_ExtDevPlug_WIDTH                             1
#define D0F0xCC_x01_ExtDevPlug_MASK                              0x10000
#define D0F0xCC_x01_ExtDevCsrEn_OFFSET                           17
#define D0F0xCC_x01_ExtDevCsrEn_WIDTH                            1
#define D0F0xCC_x01_ExtDevCsrEn_MASK                             0x20000
#define D0F0xCC_x01_CsrEnable_OFFSET                             18
#define D0F0xCC_x01_CsrEnable_WIDTH                              1
#define D0F0xCC_x01_CsrEnable_MASK                               0x40000
#define D0F0xCC_x01_SetPowEn_OFFSET                              20
#define D0F0xCC_x01_SetPowEn_WIDTH                               1
#define D0F0xCC_x01_SetPowEn_MASK                                0x100000
#define D0F0xCC_x01_Reserved_22_21_OFFSET                        21
#define D0F0xCC_x01_Reserved_22_21_WIDTH                         2
#define D0F0xCC_x01_Reserved_22_21_MASK                          0x600000
#define D0F0xCC_x01_ApicEnable_OFFSET                            23
#define D0F0xCC_x01_ApicEnable_WIDTH                             1
#define D0F0xCC_x01_ApicEnable_MASK                              0x800000
#define D0F0xCC_x01_ApicRange_OFFSET                             24
#define D0F0xCC_x01_ApicRange_WIDTH                              8
#define D0F0xCC_x01_ApicRange_MASK                               0xff000000

/// D0F0xCC_x1
typedef union {
  struct {                                                              ///<
    UINT32                                                BridgeDis:1 ; ///<
    UINT32                                             Reserved_1_1:2 ; ///<
    UINT32                                                   CfgDis:1 ; ///<
    UINT32                                                   P2pDis:1 ; ///<
    UINT32                                            Reserved_15_4:12; ///<
    UINT32                                               ExtDevPlug:1 ; ///<
    UINT32                                              ExtDevCsrEn:1 ; ///<
    UINT32                                                CsrEnable:1 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_22_21:2 ; ///<
    UINT32                                               ApicEnable:1 ; ///<
    UINT32                                                ApicRange:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xCC_x01_STRUCT;


// **** D0F0xC8 Register Definition ****
// Address
#define D0F0xC8_ADDRESS                                         0xc8

// Type
#define D0F0xC8_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0xC8_NB_DEV_IND_ADDR_OFFSET                          0
#define D0F0xC8_NB_DEV_IND_ADDR_WIDTH                           7
#define D0F0xC8_NB_DEV_IND_ADDR_MASK                            0x7F
#define D0F0xC8_Reserved_15_7_OFFSET                            7
#define D0F0xC8_Reserved_15_7_WIDTH                             9
#define D0F0xC8_Reserved_15_7_MASK                              0xFF80
#define D0F0xC8_NB_DEV_IND_SEL_OFFSET                           16
#define D0F0xC8_NB_DEV_IND_SEL_WIDTH                            8
#define D0F0xC8_NB_DEV_IND_SEL_MASK                             0xFF0000
#define D0F0xC8_Reserved_31_24_OFFSET                           24
#define D0F0xC8_Reserved_31_24_WIDTH                            8
#define D0F0xC8_Reserved_31_24_MASK                             0xFF000000

/// D0F0xC8
typedef union {
  struct {                                                              ///<
    UINT32                                         NB_DEV_IND_ADDR:7; ///<
    UINT32                                           Reserved_15_7:9; ///<
    UINT32                                          NB_DEV_IND_SEL:8; ///<
    UINT32                                          Reserved_31_24:8; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xC8_STRUCT;



#ifdef _GNBPCIEINITLIBV4_H_

// **** D0F0xE4_WRAP_8014 Register Definition ****
// Address
#define D0F0xE4_WRAP_8014_ADDRESS                               0x8014


/// D0F0xE4_WRAP_8014
typedef union {
  struct {                                                              ///<
    UINT32                                      TxclkPermGateEnable:1 ; ///<
    UINT32                                      TxclkPrbsGateEnable:1 ; ///<
    UINT32                                      DdiGatePifA1xEnable:1 ; ///<
    UINT32                                      DdiGatePifB1xEnable:1 ; ///<
    UINT32                                      DdiGatePifC1xEnable:1 ; ///<
    UINT32                                      DdiGatePifD1xEnable:1 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                    DdiGatePifA2p5xEnable:1 ; ///<
    UINT32                                    DdiGatePifB2p5xEnable:1 ; ///<
    UINT32                                    DdiGatePifC2p5xEnable:1 ; ///<
    UINT32                                    DdiGatePifD2p5xEnable:1 ; ///<
    UINT32                                     PcieGatePifA1xEnable:1 ; ///<
    UINT32                                     PcieGatePifB1xEnable:1 ; ///<
    UINT32                                     PcieGatePifC1xEnable:1 ; ///<
    UINT32                                     PcieGatePifD1xEnable:1 ; ///<
    UINT32                                   PcieGatePifA2p5xEnable:1 ; ///<
    UINT32                                   PcieGatePifB2p5xEnable:1 ; ///<
    UINT32                                   PcieGatePifC2p5xEnable:1 ; ///<
    UINT32                                   PcieGatePifD2p5xEnable:1 ; ///<
    UINT32                            TxclkPermGateOnlyWhenPllPwrDn:1 ; ///<
    UINT32                                           Reserved_23_21:3 ; ///<
    UINT32                                        DdiGateDigAEnable:1 ; ///<
    UINT32                                        DdiGateDigBEnable:1 ; ///<
    UINT32                                        DdiGateDigCEnable:1 ; ///<
    UINT32                                        DdiGateDigDEnable:1 ; ///<
    UINT32                                               SpareRegRw:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8014_STRUCT;

#endif

#ifdef _GNBPCIEINITLIBV5_H_ // KV version of WRAP_8014

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
#define D0F0xE4_WRAP_8014_DdiPifa1xGateEnable_OFFSET            2
#define D0F0xE4_WRAP_8014_DdiPifa1xGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8014_DdiPifa1xGateEnable_MASK              0x4
#define D0F0xE4_WRAP_8014_DdiPifb1xGateEnable_OFFSET            3
#define D0F0xE4_WRAP_8014_DdiPifb1xGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8014_DdiPifb1xGateEnable_MASK              0x8
#define D0F0xE4_WRAP_8014_DdiPifc1xGateEnable_OFFSET            4
#define D0F0xE4_WRAP_8014_DdiPifc1xGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8014_DdiPifc1xGateEnable_MASK              0x10
#define D0F0xE4_WRAP_8014_DdiPifd1xGateEnable_OFFSET            5
#define D0F0xE4_WRAP_8014_DdiPifd1xGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8014_DdiPifd1xGateEnable_MASK              0x20
#define D0F0xE4_WRAP_8014_Reserved_11_6_OFFSET                  6
#define D0F0xE4_WRAP_8014_Reserved_11_6_WIDTH                   6
#define D0F0xE4_WRAP_8014_Reserved_11_6_MASK                    0xfc0
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_OFFSET           12
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_MASK             0x1000
#define D0F0xE4_WRAP_8014_PcieGatePifB1xEnable_OFFSET           13
#define D0F0xE4_WRAP_8014_PcieGatePifB1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifB1xEnable_MASK             0x2000
#define D0F0xE4_WRAP_8014_PcieGatePifC1xEnable_OFFSET           14
#define D0F0xE4_WRAP_8014_PcieGatePifC1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifC1xEnable_MASK             0x4000
#define D0F0xE4_WRAP_8014_PcieGatePifD1xEnable_OFFSET           15
#define D0F0xE4_WRAP_8014_PcieGatePifD1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifD1xEnable_MASK             0x8000
#define D0F0xE4_WRAP_8014_Reserved_19_16_OFFSET                 16
#define D0F0xE4_WRAP_8014_Reserved_19_16_WIDTH                  4
#define D0F0xE4_WRAP_8014_Reserved_19_16_MASK                   0xf0000
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_OFFSET  20
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_WIDTH   1
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_MASK    0x100000
#define D0F0xE4_WRAP_8014_Reserved_23_21_OFFSET                 21
#define D0F0xE4_WRAP_8014_Reserved_23_21_WIDTH                  3
#define D0F0xE4_WRAP_8014_Reserved_23_21_MASK                   0xe00000
#define D0F0xE4_WRAP_8014_DdiDigaGateEnable_OFFSET              24
#define D0F0xE4_WRAP_8014_DdiDigaGateEnable_WIDTH               1
#define D0F0xE4_WRAP_8014_DdiDigaGateEnable_MASK                0x1000000
#define D0F0xE4_WRAP_8014_DdiDigbGateEnable_OFFSET              25
#define D0F0xE4_WRAP_8014_DdiDigbGateEnable_WIDTH               1
#define D0F0xE4_WRAP_8014_DdiDigbGateEnable_MASK                0x2000000
#define D0F0xE4_WRAP_8014_DdiDigcGateEnable_OFFSET              26
#define D0F0xE4_WRAP_8014_DdiDigcGateEnable_WIDTH               1
#define D0F0xE4_WRAP_8014_DdiDigcGateEnable_MASK                0x4000000
#define D0F0xE4_WRAP_8014_DdiDigdGateEnable_OFFSET              27
#define D0F0xE4_WRAP_8014_DdiDigdGateEnable_WIDTH               1
#define D0F0xE4_WRAP_8014_DdiDigdGateEnable_MASK                0x8000000
#define D0F0xE4_WRAP_8014_SpareRegRw_OFFSET                     28
#define D0F0xE4_WRAP_8014_SpareRegRw_WIDTH                      4
#define D0F0xE4_WRAP_8014_SpareRegRw_MASK                       0xf0000000

/// D0F0xE4_WRAP_8014
typedef union {
  struct {                                                              ///<
    UINT32                                      TxclkPermGateEnable:1 ; ///<
    UINT32                                      TxclkPrbsGateEnable:1 ; ///<
    UINT32                                      DdiPifa1xGateEnable:1 ; ///<
    UINT32                                      DdiPifb1xGateEnable:1 ; ///<
    UINT32                                      DdiPifc1xGateEnable:1 ; ///<
    UINT32                                      DdiPifd1xGateEnable:1 ; ///<
    UINT32                                            Reserved_11_6:6 ; ///<
    UINT32                                     PcieGatePifA1xEnable:1 ; ///<
    UINT32                                     PcieGatePifB1xEnable:1 ; ///<
    UINT32                                     PcieGatePifC1xEnable:1 ; ///<
    UINT32                                     PcieGatePifD1xEnable:1 ; ///<
    UINT32                                           Reserved_19_16:4 ; ///<
    UINT32                            TxclkPermGateOnlyWhenPllPwrDn:1 ; ///<
    UINT32                                           Reserved_23_21:3 ; ///<
    UINT32                                        DdiDigaGateEnable:1 ; ///<
    UINT32                                        DdiDigbGateEnable:1 ; ///<
    UINT32                                        DdiDigcGateEnable:1 ; ///<
    UINT32                                        DdiDigdGateEnable:1 ; ///<
    UINT32                                               SpareRegRw:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8014_STRUCT;

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

#endif

#ifdef _GNBPCIETRAININGV2_H_

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

#endif

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
#define D0F0xE4_PIF_0011_Reserved_15_12_MASK                    0xf000
#define D0F0xE4_PIF_0011_X8Lane70_OFFSET                        16
#define D0F0xE4_PIF_0011_X8Lane70_WIDTH                         1
#define D0F0xE4_PIF_0011_X8Lane70_MASK                          0x10000
#define D0F0xE4_PIF_0011_X8Lane158_OFFSET                       17
#define D0F0xE4_PIF_0011_X8Lane158_WIDTH                        1
#define D0F0xE4_PIF_0011_X8Lane158_MASK                         0x20000
#define D0F0xE4_PIF_0011_Reserved_19_18_OFFSET                  18
#define D0F0xE4_PIF_0011_Reserved_19_18_WIDTH                   2
#define D0F0xE4_PIF_0011_Reserved_19_18_MASK                    0xc0000
#define D0F0xE4_PIF_0011_X16Lane150_OFFSET                      20
#define D0F0xE4_PIF_0011_X16Lane150_WIDTH                       1
#define D0F0xE4_PIF_0011_X16Lane150_MASK                        0x100000
#define D0F0xE4_PIF_0011_Reserved_24_21_OFFSET                  21
#define D0F0xE4_PIF_0011_Reserved_24_21_WIDTH                   4
#define D0F0xE4_PIF_0011_Reserved_24_21_MASK                    0x1e00000
#define D0F0xE4_PIF_0011_MultiPif_OFFSET                        25
#define D0F0xE4_PIF_0011_MultiPif_WIDTH                         1
#define D0F0xE4_PIF_0011_MultiPif_MASK                          0x2000000
#define D0F0xE4_PIF_0011_Reserved_31_26_OFFSET                  26
#define D0F0xE4_PIF_0011_Reserved_31_26_WIDTH                   6
#define D0F0xE4_PIF_0011_Reserved_31_26_MASK                    0xfc000000

/// D0F0xE4_PIF_0011
typedef union {
  struct {                                                              ///<
    UINT32                                                 X2Lane10:1 ; ///<
    UINT32                                                 X2Lane32:1 ; ///<
    UINT32                                                 X2Lane54:1 ; ///<
    UINT32                                                 X2Lane76:1 ; ///<
    UINT32                                                 X2Lane98:1 ; ///<
    UINT32                                               X2Lane1110:1 ; ///<
    UINT32                                               X2Lane1312:1 ; ///<
    UINT32                                               X2Lane1514:1 ; ///<
    UINT32                                                 X4Lane30:1 ; ///<
    UINT32                                                 X4Lane74:1 ; ///<
    UINT32                                                X4Lane118:1 ; ///<
    UINT32                                               X4Lane1512:1 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                                 X8Lane70:1 ; ///<
    UINT32                                                X8Lane158:1 ; ///<
    UINT32                                           Reserved_19_18:2 ; ///<
    UINT32                                               X16Lane150:1 ; ///<
    UINT32                                           Reserved_24_21:4 ; ///<
    UINT32                                                 MultiPif:1 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
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
#define D0F0xE4_PIF_0012_PllPowerStateInOff_MASK                0x1c00
#define D0F0xE4_PIF_0012_Reserved_15_13_OFFSET                  13
#define D0F0xE4_PIF_0012_Reserved_15_13_WIDTH                   3
#define D0F0xE4_PIF_0012_Reserved_15_13_MASK                    0xe000
#define D0F0xE4_PIF_0012_Tx2p5clkClockGatingEn_OFFSET           16
#define D0F0xE4_PIF_0012_Tx2p5clkClockGatingEn_WIDTH            1
#define D0F0xE4_PIF_0012_Tx2p5clkClockGatingEn_MASK             0x10000
#define D0F0xE4_PIF_0012_Reserved_23_17_OFFSET                  17
#define D0F0xE4_PIF_0012_Reserved_23_17_WIDTH                   7
#define D0F0xE4_PIF_0012_Reserved_23_17_MASK                    0xfe0000
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
#define D0F0xE4_PIF_0012_PllPwrOverrideVal_MASK                 0xe0000000

/// D0F0xE4_PIF_0012
typedef union {
  struct {                                                              ///<
    UINT32                                       TxPowerStateInTxs2:3 ; ///<
    UINT32                                           ForceRxEnInL0s:1 ; ///<
    UINT32                                       RxPowerStateInRxs2:3 ; ///<
    UINT32                                      PllPowerStateInTxs2:3 ; ///<
    UINT32                                       PllPowerStateInOff:3 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                    Tx2p5clkClockGatingEn:1 ; ///<
    UINT32                                           Reserved_23_17:7 ; ///<
    UINT32                                            PllRampUpTime:3 ; ///<
    UINT32                                           Reserved_27_27:1 ; ///<
    UINT32                                         PllPwrOverrideEn:1 ; ///<
    UINT32                                        PllPwrOverrideVal:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PIF_0012_STRUCT;


// **** D0F0xE4_PIF_0017 Register Definition ****
// Address
#define D0F0xE4_PIF_0017_ADDRESS                                0x17

// Type
#define D0F0xE4_PIF_0017_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PIF_0017_TxPowerStateInTxs2_OFFSET              0
#define D0F0xE4_PIF_0017_TxPowerStateInTxs2_WIDTH               3
#define D0F0xE4_PIF_0017_TxPowerStateInTxs2_MASK                0x7
#define D0F0xE4_PIF_0017_ForceRxEnInL0s_OFFSET                  3
#define D0F0xE4_PIF_0017_ForceRxEnInL0s_WIDTH                   1
#define D0F0xE4_PIF_0017_ForceRxEnInL0s_MASK                    0x8
#define D0F0xE4_PIF_0017_RxPowerStateInRxs2_OFFSET              4
#define D0F0xE4_PIF_0017_RxPowerStateInRxs2_WIDTH               3
#define D0F0xE4_PIF_0017_RxPowerStateInRxs2_MASK                0x70
#define D0F0xE4_PIF_0017_PllPowerStateInTxs2_OFFSET             7
#define D0F0xE4_PIF_0017_PllPowerStateInTxs2_WIDTH              3
#define D0F0xE4_PIF_0017_PllPowerStateInTxs2_MASK               0x380
#define D0F0xE4_PIF_0017_PllPowerStateInOff_OFFSET              10
#define D0F0xE4_PIF_0017_PllPowerStateInOff_WIDTH               3
#define D0F0xE4_PIF_0017_PllPowerStateInOff_MASK                0x1c00
#define D0F0xE4_PIF_0017_Reserved_15_13_OFFSET                  13
#define D0F0xE4_PIF_0017_Reserved_15_13_WIDTH                   3
#define D0F0xE4_PIF_0017_Reserved_15_13_MASK                    0xe000
#define D0F0xE4_PIF_0017_Tx2p5clkClockGatingEn_OFFSET           16
#define D0F0xE4_PIF_0017_Tx2p5clkClockGatingEn_WIDTH            1
#define D0F0xE4_PIF_0017_Tx2p5clkClockGatingEn_MASK             0x10000
#define D0F0xE4_PIF_0017_Reserved_23_17_OFFSET                  17
#define D0F0xE4_PIF_0017_Reserved_23_17_WIDTH                   7
#define D0F0xE4_PIF_0017_Reserved_23_17_MASK                    0xfe0000
#define D0F0xE4_PIF_0017_PllRampUpTime_OFFSET                   24
#define D0F0xE4_PIF_0017_PllRampUpTime_WIDTH                    3
#define D0F0xE4_PIF_0017_PllRampUpTime_MASK                     0x7000000
#define D0F0xE4_PIF_0017_Reserved_27_27_OFFSET                  27
#define D0F0xE4_PIF_0017_Reserved_27_27_WIDTH                   1
#define D0F0xE4_PIF_0017_Reserved_27_27_MASK                    0x8000000
#define D0F0xE4_PIF_0017_PllPwrOverrideEn_OFFSET                28
#define D0F0xE4_PIF_0017_PllPwrOverrideEn_WIDTH                 1
#define D0F0xE4_PIF_0017_PllPwrOverrideEn_MASK                  0x10000000
#define D0F0xE4_PIF_0017_PllPwrOverrideVal_OFFSET               29
#define D0F0xE4_PIF_0017_PllPwrOverrideVal_WIDTH                3
#define D0F0xE4_PIF_0017_PllPwrOverrideVal_MASK                 0xe0000000

/// D0F0xE4_PIF_0017
typedef union {
  struct {                                                              ///<
    UINT32                                       TxPowerStateInTxs2:3 ; ///<
    UINT32                                           ForceRxEnInL0s:1 ; ///<
    UINT32                                       RxPowerStateInRxs2:3 ; ///<
    UINT32                                      PllPowerStateInTxs2:3 ; ///<
    UINT32                                       PllPowerStateInOff:3 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                    Tx2p5clkClockGatingEn:1 ; ///<
    UINT32                                           Reserved_23_17:7 ; ///<
    UINT32                                            PllRampUpTime:3 ; ///<
    UINT32                                           Reserved_27_27:1 ; ///<
    UINT32                                         PllPwrOverrideEn:1 ; ///<
    UINT32                                        PllPwrOverrideVal:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PIF_0017_STRUCT;


// **** D0F0xE4_CORE_00B0 Register Definition ****
// Address
#define D0F0xE4_CORE_00B0_ADDRESS                               0xb0

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
#define D0F0xE4_CORE_00B0_Bitfield_6_6_MASK                     0x40
#define D0F0xE4_CORE_00B0_Reserved_31_7_OFFSET                  7
#define D0F0xE4_CORE_00B0_Reserved_31_7_WIDTH                   25
#define D0F0xE4_CORE_00B0_Reserved_31_7_MASK                    0xffffff80

/// D0F0xE4_CORE_00B0
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                             StrapF0MsiEn:1 ; ///<
    UINT32                                             Reserved_4_3:2 ; ///<
    UINT32                                             StrapF0AerEn:1 ; ///<
    UINT32                                             Bitfield_6_6:1 ; ///<
    UINT32                                            Reserved_31_7:25; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_00B0_STRUCT;
typedef union {
  struct {                                                              ///<
    UINT32                                             Bitfield_0_0:1;  ///<
    UINT32                                             Bitfield_1_1:1 ; ///<
    UINT32                                            Reserved_15_2:14; ///<
    UINT32                                           Bitfield_16_16:1;  ///<
    UINT32                                           Bitfield_17_17:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GRC2_STRUCT;



#endif


