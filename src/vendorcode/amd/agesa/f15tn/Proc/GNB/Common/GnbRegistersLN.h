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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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

#ifndef _GNBREGISTERSLN_H_
#define _GNBREGISTERSLN_H_
#define  TYPE_D0F0                  0x1
#define  TYPE_D0F0x64               0x2
#define  TYPE_D0F0x98               0x3
#define  TYPE_D0F0xE4               0x5
#define  TYPE_DxF0                  0x6
#define  TYPE_DxF0xE4               0x7
#define  TYPE_D18F1                 0xb
#define  TYPE_D18F2                 0xc
#define  TYPE_D18F3                 0xd
#define  TYPE_MSR                   0x10
#define  TYPE_D1F0                  0x11
#define  TYPE_GMM                   0x12
#define  D18F2x9C  0xe
#define  GMM       0x11
#ifndef WRAP_SPACE
  #define  WRAP_SPACE(w, x)   (0x01300000 | (w << 16) | (x))
#endif
#ifndef CORE_SPACE
  #define  CORE_SPACE(c, x)   (0x00010000 | (c << 24) | (x))
#endif
#ifndef PHY_SPACE
  #define  PHY_SPACE(w, p, x) (0x00200000 | ((p + 1) << 24) | (w << 16) | (x))
#endif
#ifndef  PIF_SPACE
  #define  PIF_SPACE(w, p, x) (0x00100000 | ((p + 1) << 24) | (w << 16) | (x))
#endif
// **** D0F0x00 Register Definition ****
// Address
#define D0F0x00_ADDRESS                                         0x0

// Type
#define D0F0x00_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x00_VendorID_OFFSET                                 0
#define D0F0x00_VendorID_WIDTH                                  16
#define D0F0x00_VendorID_MASK                                   0xffff
#define D0F0x00_DeviceID_OFFSET                                 16
#define D0F0x00_DeviceID_WIDTH                                  16
#define D0F0x00_DeviceID_MASK                                   0xffff0000

/// D0F0x00
typedef union {
  struct {                                                              ///<
    UINT32                                                 VendorID:16; ///<
    UINT32                                                 DeviceID:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x00_STRUCT;

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
#define D0F0x04_SpecialCycleEn_OFFSET                           3
#define D0F0x04_SpecialCycleEn_WIDTH                            1
#define D0F0x04_SpecialCycleEn_MASK                             0x8
#define D0F0x04_MemWriteInvalidateEn_OFFSET                     4
#define D0F0x04_MemWriteInvalidateEn_WIDTH                      1
#define D0F0x04_MemWriteInvalidateEn_MASK                       0x10
#define D0F0x04_PalSnoopEn_OFFSET                               5
#define D0F0x04_PalSnoopEn_WIDTH                                1
#define D0F0x04_PalSnoopEn_MASK                                 0x20
#define D0F0x04_ParityErrorEn_OFFSET                            6
#define D0F0x04_ParityErrorEn_WIDTH                             1
#define D0F0x04_ParityErrorEn_MASK                              0x40
#define D0F0x04_Reserved_7_7_OFFSET                             7
#define D0F0x04_Reserved_7_7_WIDTH                              1
#define D0F0x04_Reserved_7_7_MASK                               0x80
#define D0F0x04_SerrEn_OFFSET                                   8
#define D0F0x04_SerrEn_WIDTH                                    1
#define D0F0x04_SerrEn_MASK                                     0x100
#define D0F0x04_FastB2BEn_OFFSET                                9
#define D0F0x04_FastB2BEn_WIDTH                                 1
#define D0F0x04_FastB2BEn_MASK                                  0x200
#define D0F0x04_Reserved_19_10_OFFSET                           10
#define D0F0x04_Reserved_19_10_WIDTH                            10
#define D0F0x04_Reserved_19_10_MASK                             0xffc00
#define D0F0x04_CapList_OFFSET                                  20
#define D0F0x04_CapList_WIDTH                                   1
#define D0F0x04_CapList_MASK                                    0x100000
#define D0F0x04_PCI66En_OFFSET                                  21
#define D0F0x04_PCI66En_WIDTH                                   1
#define D0F0x04_PCI66En_MASK                                    0x200000
#define D0F0x04_Reserved_22_22_OFFSET                           22
#define D0F0x04_Reserved_22_22_WIDTH                            1
#define D0F0x04_Reserved_22_22_MASK                             0x400000
#define D0F0x04_FastBackCapable_OFFSET                          23
#define D0F0x04_FastBackCapable_WIDTH                           1
#define D0F0x04_FastBackCapable_MASK                            0x800000
#define D0F0x04_Reserved_24_24_OFFSET                           24
#define D0F0x04_Reserved_24_24_WIDTH                            1
#define D0F0x04_Reserved_24_24_MASK                             0x1000000
#define D0F0x04_DevselTiming_OFFSET                             25
#define D0F0x04_DevselTiming_WIDTH                              2
#define D0F0x04_DevselTiming_MASK                               0x6000000
#define D0F0x04_SignalTargetAbort_OFFSET                        27
#define D0F0x04_SignalTargetAbort_WIDTH                         1
#define D0F0x04_SignalTargetAbort_MASK                          0x8000000
#define D0F0x04_ReceivedTargetAbort_OFFSET                      28
#define D0F0x04_ReceivedTargetAbort_WIDTH                       1
#define D0F0x04_ReceivedTargetAbort_MASK                        0x10000000
#define D0F0x04_ReceivedMasterAbort_OFFSET                      29
#define D0F0x04_ReceivedMasterAbort_WIDTH                       1
#define D0F0x04_ReceivedMasterAbort_MASK                        0x20000000
#define D0F0x04_SignaledSystemError_OFFSET                      30
#define D0F0x04_SignaledSystemError_WIDTH                       1
#define D0F0x04_SignaledSystemError_MASK                        0x40000000
#define D0F0x04_ParityErrorDetected_OFFSET                      31
#define D0F0x04_ParityErrorDetected_WIDTH                       1
#define D0F0x04_ParityErrorDetected_MASK                        0x80000000

/// D0F0x04
typedef union {
  struct {                                                              ///<
    UINT32                                               IoAccessEn:1 ; ///<
    UINT32                                              MemAccessEn:1 ; ///<
    UINT32                                              BusMasterEn:1 ; ///<
    UINT32                                           SpecialCycleEn:1 ; ///<
    UINT32                                     MemWriteInvalidateEn:1 ; ///<
    UINT32                                               PalSnoopEn:1 ; ///<
    UINT32                                            ParityErrorEn:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                                   SerrEn:1 ; ///<
    UINT32                                                FastB2BEn:1 ; ///<
    UINT32                                           Reserved_19_10:10; ///<
    UINT32                                                  CapList:1 ; ///<
    UINT32                                                  PCI66En:1 ; ///<
    UINT32                                           Reserved_22_22:1 ; ///<
    UINT32                                          FastBackCapable:1 ; ///<
    UINT32                                           Reserved_24_24:1 ; ///<
    UINT32                                             DevselTiming:2 ; ///<
    UINT32                                        SignalTargetAbort:1 ; ///<
    UINT32                                      ReceivedTargetAbort:1 ; ///<
    UINT32                                      ReceivedMasterAbort:1 ; ///<
    UINT32                                      SignaledSystemError:1 ; ///<
    UINT32                                      ParityErrorDetected:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x04_STRUCT;

// **** D0F0x08 Register Definition ****
// Address
#define D0F0x08_ADDRESS                                         0x8

// Type
#define D0F0x08_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x08_RevID_OFFSET                                    0
#define D0F0x08_RevID_WIDTH                                     8
#define D0F0x08_RevID_MASK                                      0xff
#define D0F0x08_ClassCode_OFFSET                                8
#define D0F0x08_ClassCode_WIDTH                                 24
#define D0F0x08_ClassCode_MASK                                  0xffffff00

/// D0F0x08
typedef union {
  struct {                                                              ///<
    UINT32                                                    RevID:8 ; ///<
    UINT32                                                ClassCode:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x08_STRUCT;

// **** D0F0x0C Register Definition ****
// Address
#define D0F0x0C_ADDRESS                                         0xc

// Type
#define D0F0x0C_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x0C_CacheLineSize_OFFSET                            0
#define D0F0x0C_CacheLineSize_WIDTH                             8
#define D0F0x0C_CacheLineSize_MASK                              0xff
#define D0F0x0C_LatencyTimer_OFFSET                             8
#define D0F0x0C_LatencyTimer_WIDTH                              8
#define D0F0x0C_LatencyTimer_MASK                               0xff00
#define D0F0x0C_HeaderTypeReg_OFFSET                            16
#define D0F0x0C_HeaderTypeReg_WIDTH                             8
#define D0F0x0C_HeaderTypeReg_MASK                              0xff0000
#define D0F0x0C_BIST_OFFSET                                     24
#define D0F0x0C_BIST_WIDTH                                      8
#define D0F0x0C_BIST_MASK                                       0xff000000

/// D0F0x0C
typedef union {
  struct {                                                              ///<
    UINT32                                            CacheLineSize:8 ; ///<
    UINT32                                             LatencyTimer:8 ; ///<
    UINT32                                            HeaderTypeReg:8 ; ///<
    UINT32                                                     BIST:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x0C_STRUCT;

// **** D0F0x2C Register Definition ****
// Address
#define D0F0x2C_ADDRESS                                         0x2c

// Type
#define D0F0x2C_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x2C_SubsystemVendorID_OFFSET                        0
#define D0F0x2C_SubsystemVendorID_WIDTH                         16
#define D0F0x2C_SubsystemVendorID_MASK                          0xffff
#define D0F0x2C_SubsystemID_OFFSET                              16
#define D0F0x2C_SubsystemID_WIDTH                               16
#define D0F0x2C_SubsystemID_MASK                                0xffff0000

/// D0F0x2C
typedef union {
  struct {                                                              ///<
    UINT32                                        SubsystemVendorID:16; ///<
    UINT32                                              SubsystemID:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x2C_STRUCT;

// **** D0F0x34 Register Definition ****
// Address
#define D0F0x34_ADDRESS                                         0x34

// Type
#define D0F0x34_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x34_CapPtr_OFFSET                                   0
#define D0F0x34_CapPtr_WIDTH                                    8
#define D0F0x34_CapPtr_MASK                                     0xff
#define D0F0x34_Reserved_31_8_OFFSET                            8
#define D0F0x34_Reserved_31_8_WIDTH                             24
#define D0F0x34_Reserved_31_8_MASK                              0xffffff00

/// D0F0x34
typedef union {
  struct {                                                              ///<
    UINT32                                                   CapPtr:8 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x34_STRUCT;

// **** D0F0x4C Register Definition ****
// Address
#define D0F0x4C_ADDRESS                                         0x4c

// Type
#define D0F0x4C_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x4C_Function1Enable_OFFSET                          0
#define D0F0x4C_Function1Enable_WIDTH                           1
#define D0F0x4C_Function1Enable_MASK                            0x1
#define D0F0x4C_ApicEnable_OFFSET                               1
#define D0F0x4C_ApicEnable_WIDTH                                1
#define D0F0x4C_ApicEnable_MASK                                 0x2
#define D0F0x4C_Reserved_2_2_OFFSET                             2
#define D0F0x4C_Reserved_2_2_WIDTH                              1
#define D0F0x4C_Reserved_2_2_MASK                               0x4
#define D0F0x4C_Cf8Dis_OFFSET                                   3
#define D0F0x4C_Cf8Dis_WIDTH                                    1
#define D0F0x4C_Cf8Dis_MASK                                     0x8
#define D0F0x4C_PMEDis_OFFSET                                   4
#define D0F0x4C_PMEDis_WIDTH                                    1
#define D0F0x4C_PMEDis_MASK                                     0x10
#define D0F0x4C_SerrDis_OFFSET                                  5
#define D0F0x4C_SerrDis_WIDTH                                   1
#define D0F0x4C_SerrDis_MASK                                    0x20
#define D0F0x4C_Reserved_10_6_OFFSET                            6
#define D0F0x4C_Reserved_10_6_WIDTH                             5
#define D0F0x4C_Reserved_10_6_MASK                              0x7c0
#define D0F0x4C_CRS_OFFSET                                      11
#define D0F0x4C_CRS_WIDTH                                       1
#define D0F0x4C_CRS_MASK                                        0x800
#define D0F0x4C_CfgRdTime_OFFSET                                12
#define D0F0x4C_CfgRdTime_WIDTH                                 3
#define D0F0x4C_CfgRdTime_MASK                                  0x7000
#define D0F0x4C_Reserved_22_15_OFFSET                           15
#define D0F0x4C_Reserved_22_15_WIDTH                            8
#define D0F0x4C_Reserved_22_15_MASK                             0x7f8000
#define D0F0x4C_MMIOEnable_OFFSET                               23
#define D0F0x4C_MMIOEnable_WIDTH                                1
#define D0F0x4C_MMIOEnable_MASK                                 0x800000
#define D0F0x4C_Reserved_25_24_OFFSET                           24
#define D0F0x4C_Reserved_25_24_WIDTH                            2
#define D0F0x4C_Reserved_25_24_MASK                             0x3000000
#define D0F0x4C_HPDis_OFFSET                                    26
#define D0F0x4C_HPDis_WIDTH                                     1
#define D0F0x4C_HPDis_MASK                                      0x4000000
#define D0F0x4C_Reserved_31_27_OFFSET                           27
#define D0F0x4C_Reserved_31_27_WIDTH                            5
#define D0F0x4C_Reserved_31_27_MASK                             0xf8000000

/// D0F0x4C
typedef union {
  struct {                                                              ///<
    UINT32                                          Function1Enable:1 ; ///<
    UINT32                                               ApicEnable:1 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                                   Cf8Dis:1 ; ///<
    UINT32                                                   PMEDis:1 ; ///<
    UINT32                                                  SerrDis:1 ; ///<
    UINT32                                            Reserved_10_6:5 ; ///<
    UINT32                                                      CRS:1 ; ///<
    UINT32                                                CfgRdTime:3 ; ///<
    UINT32                                           Reserved_22_15:8 ; ///<
    UINT32                                               MMIOEnable:1 ; ///<
    UINT32                                           Reserved_25_24:2 ; ///<
    UINT32                                                    HPDis:1 ; ///<
    UINT32                                           Reserved_31_27:5 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x4C_STRUCT;

// **** D0F0x60 Register Definition ****
// Address
#define D0F0x60_ADDRESS                                         0x60

// Type
#define D0F0x60_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x60_MiscIndAddr_OFFSET                              0
#define D0F0x60_MiscIndAddr_WIDTH                               7
#define D0F0x60_MiscIndAddr_MASK                                0x7f
#define D0F0x60_MiscIndWrEn_OFFSET                              7
#define D0F0x60_MiscIndWrEn_WIDTH                               1
#define D0F0x60_MiscIndWrEn_MASK                                0x80
#define D0F0x60_Reserved_31_8_OFFSET                            8
#define D0F0x60_Reserved_31_8_WIDTH                             24
#define D0F0x60_Reserved_31_8_MASK                              0xffffff00

/// D0F0x60
typedef union {
  struct {                                                              ///<
    UINT32                                              MiscIndAddr:7 ; ///<
    UINT32                                              MiscIndWrEn:1 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x60_STRUCT;

// **** D0F0x64 Register Definition ****
// Address
#define D0F0x64_ADDRESS                                         0x64

// Type
#define D0F0x64_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x64_MiscIndData_OFFSET                              0
#define D0F0x64_MiscIndData_WIDTH                               32
#define D0F0x64_MiscIndData_MASK                                0xffffffff

/// D0F0x64
typedef union {
  struct {                                                              ///<
    UINT32                                              MiscIndData:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_STRUCT;


/// D0F0x78
typedef union {
  struct {                                                              ///<
    UINT32                                                  Scratch:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x78_STRUCT;

// **** D0F0x7C Register Definition ****
// Address
#define D0F0x7C_ADDRESS                                         0x7c

// Type
#define D0F0x7C_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x7C_ForceIntGFXDisable_OFFSET                       0
#define D0F0x7C_ForceIntGFXDisable_WIDTH                        1
#define D0F0x7C_ForceIntGFXDisable_MASK                         0x1
#define D0F0x7C_Reserved_31_1_OFFSET                            1
#define D0F0x7C_Reserved_31_1_WIDTH                             31
#define D0F0x7C_Reserved_31_1_MASK                              0xfffffffe

/// D0F0x7C
typedef union {
  struct {                                                              ///<
    UINT32                                       ForceIntGFXDisable:1 ; ///<
    UINT32                                            Reserved_31_1:31; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x7C_STRUCT;

// **** D0F0x84 Register Definition ****
// Address
#define D0F0x84_ADDRESS                                         0x84

// Type
#define D0F0x84_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x84_Reserved_3_0_OFFSET                             0
#define D0F0x84_Reserved_3_0_WIDTH                              4
#define D0F0x84_Reserved_3_0_MASK                               0xf
#define D0F0x84_Ev6Mode_OFFSET                                  4
#define D0F0x84_Ev6Mode_WIDTH                                   1
#define D0F0x84_Ev6Mode_MASK                                    0x10
#define D0F0x84_Reserved_7_5_OFFSET                             5
#define D0F0x84_Reserved_7_5_WIDTH                              3
#define D0F0x84_Reserved_7_5_MASK                               0xe0
#define D0F0x84_PmeMode_OFFSET                                  8
#define D0F0x84_PmeMode_WIDTH                                   1
#define D0F0x84_PmeMode_MASK                                    0x100
#define D0F0x84_PmeTurnOff_OFFSET                               9
#define D0F0x84_PmeTurnOff_WIDTH                                1
#define D0F0x84_PmeTurnOff_MASK                                 0x200
#define D0F0x84_Reserved_31_10_OFFSET                           10
#define D0F0x84_Reserved_31_10_WIDTH                            22
#define D0F0x84_Reserved_31_10_MASK                             0xfffffc00

/// D0F0x84
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                                  Ev6Mode:1 ; ///<
    UINT32                                             Reserved_7_5:3 ; ///<
    UINT32                                                  PmeMode:1 ; ///<
    UINT32                                               PmeTurnOff:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x84_STRUCT;

// **** D0F0x90 Register Definition ****
// Address
#define D0F0x90_ADDRESS                                         0x90

// Type
#define D0F0x90_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x90_Reserved_22_0_OFFSET                            0
#define D0F0x90_Reserved_22_0_WIDTH                             23
#define D0F0x90_Reserved_22_0_MASK                              0x7fffff
#define D0F0x90_TopOfDram_OFFSET                                23
#define D0F0x90_TopOfDram_WIDTH                                 9
#define D0F0x90_TopOfDram_MASK                                  0xff800000

/// D0F0x90
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_22_0:23; ///<
    UINT32                                                TopOfDram:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x90_STRUCT;

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
#define D0F0x94_OrbIndWrEn_OFFSET                               8
#define D0F0x94_OrbIndWrEn_WIDTH                                1
#define D0F0x94_OrbIndWrEn_MASK                                 0x100
#define D0F0x94_Reserved_31_9_OFFSET                            9
#define D0F0x94_Reserved_31_9_WIDTH                             23
#define D0F0x94_Reserved_31_9_MASK                              0xfffffe00

/// D0F0x94
typedef union {
  struct {                                                              ///<
    UINT32                                               OrbIndAddr:7 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                               OrbIndWrEn:1 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x94_STRUCT;

// **** D0F0x98 Register Definition ****
// Address
#define D0F0x98_ADDRESS                                         0x98

// Type
#define D0F0x98_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x98_OrbIndData_OFFSET                               0
#define D0F0x98_OrbIndData_WIDTH                                32
#define D0F0x98_OrbIndData_MASK                                 0xffffffff

/// D0F0x98
typedef union {
  struct {                                                              ///<
    UINT32                                               OrbIndData:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_STRUCT;

// **** D0F0xE0 Register Definition ****
// Address
#define D0F0xE0_ADDRESS                                         0xe0

// Type
#define D0F0xE0_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0xE0_PcieIndxAddr_OFFSET                             0
#define D0F0xE0_PcieIndxAddr_WIDTH                              16
#define D0F0xE0_PcieIndxAddr_MASK                               0xffff
#define D0F0xE0_FrameType_OFFSET                                16
#define D0F0xE0_FrameType_WIDTH                                 8
#define D0F0xE0_FrameType_MASK                                  0xff0000
#define D0F0xE0_BlockSelect_OFFSET                              24
#define D0F0xE0_BlockSelect_WIDTH                               8
#define D0F0xE0_BlockSelect_MASK                                0xff000000

/// D0F0xE0
typedef union {
  struct {                                                              ///<
    UINT32                                             PcieIndxAddr:16; ///<
    UINT32                                                FrameType:8 ; ///<
    UINT32                                              BlockSelect:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE0_STRUCT;

// **** D0F0xE4 Register Definition ****
// Address
#define D0F0xE4_ADDRESS                                         0xe4

// Type
#define D0F0xE4_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0xE4_PcieIndxData_OFFSET                             0
#define D0F0xE4_PcieIndxData_WIDTH                              32
#define D0F0xE4_PcieIndxData_MASK                               0xffffffff

/// D0F0xE4
typedef union {
  struct {                                                              ///<
    UINT32                                             PcieIndxData:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_STRUCT;

// **** D18F1xF0 Register Definition ****
// Address
#define D18F1xF0_ADDRESS                                        0xf0

// Type
#define D18F1xF0_TYPE                                           TYPE_D18F1
// Field Data
#define D18F1xF0_DramHoleValid_OFFSET                           0
#define D18F1xF0_DramHoleValid_WIDTH                            1
#define D18F1xF0_DramHoleValid_MASK                             0x1
#define D18F1xF0_Reserved_6_1_OFFSET                            1
#define D18F1xF0_Reserved_6_1_WIDTH                             6
#define D18F1xF0_Reserved_6_1_MASK                              0x7e
#define D18F1xF0_DramHoleOffset_31_23__OFFSET                   7
#define D18F1xF0_DramHoleOffset_31_23__WIDTH                    9
#define D18F1xF0_DramHoleOffset_31_23__MASK                     0xff80
#define D18F1xF0_Reserved_23_16_OFFSET                          16
#define D18F1xF0_Reserved_23_16_WIDTH                           8
#define D18F1xF0_Reserved_23_16_MASK                            0xff0000
#define D18F1xF0_DramHoleBase_31_24__OFFSET                     24
#define D18F1xF0_DramHoleBase_31_24__WIDTH                      8
#define D18F1xF0_DramHoleBase_31_24__MASK                       0xff000000

/// D18F1xF0
typedef union {
  struct {                                                              ///<
    UINT32                                            DramHoleValid:1 ; ///<
    UINT32                                             Reserved_6_1:6 ; ///<
    UINT32                                    DramHoleOffset_31_23_:9 ; ///<
    UINT32                                           Reserved_23_16:8 ; ///<
    UINT32                                      DramHoleBase_31_24_:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F1xF0_STRUCT;

// **** D18F2x00 Register Definition ****
// Address
#define D18F2x00_ADDRESS                                        0x0

// Type
#define D18F2x00_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x00_VendorID_OFFSET                                0
#define D18F2x00_VendorID_WIDTH                                 16
#define D18F2x00_VendorID_MASK                                  0xffff
#define D18F2x00_DeviceID_OFFSET                                16
#define D18F2x00_DeviceID_WIDTH                                 16
#define D18F2x00_DeviceID_MASK                                  0xffff0000

/// D18F2x00
typedef union {
  struct {                                                              ///<
    UINT32                                                 VendorID:16; ///<
    UINT32                                                 DeviceID:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x00_STRUCT;


// **** D18F2x08 Register Definition ****
// Address
#define D18F2x08_ADDRESS                                        0x8

// Type
#define D18F2x08_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x08_RevID_OFFSET                                   0
#define D18F2x08_RevID_WIDTH                                    8
#define D18F2x08_RevID_MASK                                     0xff
#define D18F2x08_ClassCode_OFFSET                               8
#define D18F2x08_ClassCode_WIDTH                                24
#define D18F2x08_ClassCode_MASK                                 0xffffff00

/// D18F2x08
typedef union {
  struct {                                                              ///<
    UINT32                                                    RevID:8 ; ///<
    UINT32                                                ClassCode:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x08_STRUCT;

// **** D18F2x0C Register Definition ****
// Address
#define D18F2x0C_ADDRESS                                        0xc

// Type
#define D18F2x0C_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x0C_HeaderTypeReg_OFFSET                           0
#define D18F2x0C_HeaderTypeReg_WIDTH                            32
#define D18F2x0C_HeaderTypeReg_MASK                             0xffffffff

/// D18F2x0C
typedef union {
  struct {                                                              ///<
    UINT32                                            HeaderTypeReg:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x0C_STRUCT;


// **** D18F2x040 Register Definition ****
// Address
#define D18F2x040_ADDRESS                                       0x40

// Type
#define D18F2x040_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x040_CSEnable_OFFSET                               0
#define D18F2x040_CSEnable_WIDTH                                1
#define D18F2x040_CSEnable_MASK                                 0x1
#define D18F2x040_Reserved_1_1_OFFSET                           1
#define D18F2x040_Reserved_1_1_WIDTH                            1
#define D18F2x040_Reserved_1_1_MASK                             0x2
#define D18F2x040_TestFail_OFFSET                               2
#define D18F2x040_TestFail_WIDTH                                1
#define D18F2x040_TestFail_MASK                                 0x4
#define D18F2x040_OnDimmMirror_OFFSET                           3
#define D18F2x040_OnDimmMirror_WIDTH                            1
#define D18F2x040_OnDimmMirror_MASK                             0x8
#define D18F2x040_Reserved_4_4_OFFSET                           4
#define D18F2x040_Reserved_4_4_WIDTH                            1
#define D18F2x040_Reserved_4_4_MASK                             0x10
#define D18F2x040_Reserved_31_29_OFFSET                         29
#define D18F2x040_Reserved_31_29_WIDTH                          3
#define D18F2x040_Reserved_31_29_MASK                           0xe0000000

/// D18F2x040
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          :9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          :10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x040_STRUCT;

// **** D18F2x044 Register Definition ****
// Address
#define D18F2x044_ADDRESS                                       0x44

// Type
#define D18F2x044_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x044_CSEnable_OFFSET                               0
#define D18F2x044_CSEnable_WIDTH                                1
#define D18F2x044_CSEnable_MASK                                 0x1
#define D18F2x044_Reserved_1_1_OFFSET                           1
#define D18F2x044_Reserved_1_1_WIDTH                            1
#define D18F2x044_Reserved_1_1_MASK                             0x2
#define D18F2x044_TestFail_OFFSET                               2
#define D18F2x044_TestFail_WIDTH                                1
#define D18F2x044_TestFail_MASK                                 0x4
#define D18F2x044_OnDimmMirror_OFFSET                           3
#define D18F2x044_OnDimmMirror_WIDTH                            1
#define D18F2x044_OnDimmMirror_MASK                             0x8
#define D18F2x044_Reserved_4_4_OFFSET                           4
#define D18F2x044_Reserved_4_4_WIDTH                            1
#define D18F2x044_Reserved_4_4_MASK                             0x10
#define D18F2x044_Reserved_31_29_OFFSET                         29
#define D18F2x044_Reserved_31_29_WIDTH                          3
#define D18F2x044_Reserved_31_29_MASK                           0xe0000000

/// D18F2x044
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          :9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          :10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x044_STRUCT;

// **** D18F2x048 Register Definition ****
// Address
#define D18F2x048_ADDRESS                                       0x48

// Type
#define D18F2x048_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x048_CSEnable_OFFSET                               0
#define D18F2x048_CSEnable_WIDTH                                1
#define D18F2x048_CSEnable_MASK                                 0x1
#define D18F2x048_Reserved_1_1_OFFSET                           1
#define D18F2x048_Reserved_1_1_WIDTH                            1
#define D18F2x048_Reserved_1_1_MASK                             0x2
#define D18F2x048_TestFail_OFFSET                               2
#define D18F2x048_TestFail_WIDTH                                1
#define D18F2x048_TestFail_MASK                                 0x4
#define D18F2x048_OnDimmMirror_OFFSET                           3
#define D18F2x048_OnDimmMirror_WIDTH                            1
#define D18F2x048_OnDimmMirror_MASK                             0x8
#define D18F2x048_Reserved_4_4_OFFSET                           4
#define D18F2x048_Reserved_4_4_WIDTH                            1
#define D18F2x048_Reserved_4_4_MASK                             0x10
#define D18F2x048_Reserved_31_29_OFFSET                         29
#define D18F2x048_Reserved_31_29_WIDTH                          3
#define D18F2x048_Reserved_31_29_MASK                           0xe0000000

/// D18F2x048
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          :9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          :10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x048_STRUCT;

// **** D18F2x04C Register Definition ****
// Address
#define D18F2x04C_ADDRESS                                       0x4c

// Type
#define D18F2x04C_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x04C_CSEnable_OFFSET                               0
#define D18F2x04C_CSEnable_WIDTH                                1
#define D18F2x04C_CSEnable_MASK                                 0x1
#define D18F2x04C_Reserved_1_1_OFFSET                           1
#define D18F2x04C_Reserved_1_1_WIDTH                            1
#define D18F2x04C_Reserved_1_1_MASK                             0x2
#define D18F2x04C_TestFail_OFFSET                               2
#define D18F2x04C_TestFail_WIDTH                                1
#define D18F2x04C_TestFail_MASK                                 0x4
#define D18F2x04C_OnDimmMirror_OFFSET                           3
#define D18F2x04C_OnDimmMirror_WIDTH                            1
#define D18F2x04C_OnDimmMirror_MASK                             0x8
#define D18F2x04C_Reserved_4_4_OFFSET                           4
#define D18F2x04C_Reserved_4_4_WIDTH                            1
#define D18F2x04C_Reserved_4_4_MASK                             0x10
#define D18F2x04C_Reserved_31_29_OFFSET                         29
#define D18F2x04C_Reserved_31_29_WIDTH                          3
#define D18F2x04C_Reserved_31_29_MASK                           0xe0000000

/// D18F2x04C
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          :9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          :10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x04C_STRUCT;

// **** D18F2x060 Register Definition ****
// Address
#define D18F2x060_ADDRESS                                       0x60

// Type
#define D18F2x060_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x060_Reserved_4_0_OFFSET                           0
#define D18F2x060_Reserved_4_0_WIDTH                            5
#define D18F2x060_Reserved_4_0_MASK                             0x1f
#define D18F2x060_Reserved_31_29_OFFSET                         29
#define D18F2x060_Reserved_31_29_WIDTH                          3
#define D18F2x060_Reserved_31_29_MASK                           0xe0000000

/// D18F2x060
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          :9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          :10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x060_STRUCT;

// **** D18F2x064 Register Definition ****
// Address
#define D18F2x064_ADDRESS                                       0x64

// Type
#define D18F2x064_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x064_Reserved_4_0_OFFSET                           0
#define D18F2x064_Reserved_4_0_WIDTH                            5
#define D18F2x064_Reserved_4_0_MASK                             0x1f
#define D18F2x064_Reserved_31_29_OFFSET                         29
#define D18F2x064_Reserved_31_29_WIDTH                          3
#define D18F2x064_Reserved_31_29_MASK                           0xe0000000

/// D18F2x064
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          :9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          :10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x064_STRUCT;

// **** D18F2x078 Register Definition ****
// Address
#define D18F2x078_ADDRESS                                       0x78

// Type
#define D18F2x078_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x078_Reserved_14_14_OFFSET                         14
#define D18F2x078_Reserved_14_14_WIDTH                          1
#define D18F2x078_Reserved_14_14_MASK                           0x4000
#define D18F2x078_Reserved_15_15_OFFSET                         15
#define D18F2x078_Reserved_15_15_WIDTH                          1
#define D18F2x078_Reserved_15_15_MASK                           0x8000
#define D18F2x078_Reserved_16_16_OFFSET                         16
#define D18F2x078_Reserved_16_16_WIDTH                          1
#define D18F2x078_Reserved_16_16_MASK                           0x10000
#define D18F2x078_AddrCmdTriEn_OFFSET                           17
#define D18F2x078_AddrCmdTriEn_WIDTH                            1
#define D18F2x078_AddrCmdTriEn_MASK                             0x20000
#define D18F2x078_Reserved_18_18_OFFSET                         18
#define D18F2x078_Reserved_18_18_WIDTH                          1
#define D18F2x078_Reserved_18_18_MASK                           0x40000
#define D18F2x078_Reserved_19_19_OFFSET                         19
#define D18F2x078_Reserved_19_19_WIDTH                          1
#define D18F2x078_Reserved_19_19_MASK                           0x80000

/// D18F2x078
typedef union {
  struct {                                                              ///<
    UINT32                                                :4 ; ///<
    UINT32                                             :2 ; ///<
    UINT32                                             :1 ; ///<
    UINT32                                             :1 ; ///<
    UINT32                                               :2 ; ///<
    UINT32                                               :2 ; ///<
    UINT32                                               :2 ; ///<
    UINT32                                           Reserved_14_14:1 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                           Reserved_16_16:1 ; ///<
    UINT32                                             AddrCmdTriEn:1 ; ///<
    UINT32                                           Reserved_18_18:1 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                        :1 ; ///<
    UINT32                                             :10; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x078_STRUCT;



// **** D18F2x084 Register Definition ****
// Address
#define D18F2x084_ADDRESS                                       0x84

// Type
#define D18F2x084_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x084_BurstCtrl_OFFSET                              0
#define D18F2x084_BurstCtrl_WIDTH                               2
#define D18F2x084_BurstCtrl_MASK                                0x3
#define D18F2x084_Reserved_3_2_OFFSET                           2
#define D18F2x084_Reserved_3_2_WIDTH                            2
#define D18F2x084_Reserved_3_2_MASK                             0xc
#define D18F2x084_Reserved_19_7_OFFSET                          7
#define D18F2x084_Reserved_19_7_WIDTH                           13
#define D18F2x084_Reserved_19_7_MASK                            0xfff80
#define D18F2x084_PchgPDModeSel_OFFSET                          23
#define D18F2x084_PchgPDModeSel_WIDTH                           1
#define D18F2x084_PchgPDModeSel_MASK                            0x800000
#define D18F2x084_Reserved_31_24_OFFSET                         24
#define D18F2x084_Reserved_31_24_WIDTH                          8
#define D18F2x084_Reserved_31_24_MASK                           0xff000000

/// D18F2x084
typedef union {
  struct {                                                              ///<
    UINT32                                                BurstCtrl:2 ; ///<
    UINT32                                             Reserved_3_2:2 ; ///<
    UINT32                                                      :3 ; ///<
    UINT32                                            Reserved_19_7:13; ///<
    UINT32                                                     :3 ; ///<
    UINT32                                            PchgPDModeSel:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x084_STRUCT;

// **** D18F2x088 Register Definition ****
// Address
#define D18F2x088_ADDRESS                                       0x88

// Type
#define D18F2x088_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x088_Reserved_23_4_OFFSET                          4
#define D18F2x088_Reserved_23_4_WIDTH                           20
#define D18F2x088_Reserved_23_4_MASK                            0xfffff0

/// D18F2x088
typedef union {
  struct {                                                              ///<
    UINT32                                                      :4 ; ///<
    UINT32                                            Reserved_23_4:20; ///<
    UINT32                                                :8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x088_STRUCT;




// **** D18F2x098 Register Definition ****
// Address
#define D18F2x098_ADDRESS                                       0x98

// Type
#define D18F2x098_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x098_DctOffset_OFFSET                              0
#define D18F2x098_DctOffset_WIDTH                               30
#define D18F2x098_DctOffset_MASK                                0x3fffffff
#define D18F2x098_DctAccessWrite_OFFSET                         30
#define D18F2x098_DctAccessWrite_WIDTH                          1
#define D18F2x098_DctAccessWrite_MASK                           0x40000000
#define D18F2x098_Reserved_31_31_OFFSET                         31
#define D18F2x098_Reserved_31_31_WIDTH                          1
#define D18F2x098_Reserved_31_31_MASK                           0x80000000

/// D18F2x098
typedef union {
  struct {                                                              ///<
    UINT32                                                DctOffset:30; ///<
    UINT32                                           DctAccessWrite:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x098_STRUCT;

// **** D18F2x09C Register Definition ****
// Address
#define D18F2x09C_ADDRESS                                       0x9c

// Type
#define D18F2x09C_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x09C_DctDataPort_OFFSET                            0
#define D18F2x09C_DctDataPort_WIDTH                             32
#define D18F2x09C_DctDataPort_MASK                              0xffffffff

/// D18F2x09C
typedef union {
  struct {                                                              ///<
    UINT32                                              DctDataPort:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_STRUCT;


// **** D18F2xA4 Register Definition ****
// Address
#define D18F2xA4_ADDRESS                                        0xa4

// Type
#define D18F2xA4_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xA4_DoubleTrefRateEn_OFFSET                        0
#define D18F2xA4_DoubleTrefRateEn_WIDTH                         1


// **** D18F2xAC Register Definition ****
// Address
#define D18F2xAC_ADDRESS                                        0xac

// Type
#define D18F2xAC_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xAC_Reserved_31_1_OFFSET                           1
#define D18F2xAC_Reserved_31_1_WIDTH                            31
#define D18F2xAC_Reserved_31_1_MASK                             0xfffffffe

/// D18F2xAC
typedef union {
  struct {                                                              ///<
    UINT32                                               MemTempHot:1 ; ///<
    UINT32                                            Reserved_31_1:31; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2xAC_STRUCT;



// **** D18F2x114 Register Definition ****
// Address
#define D18F2x114_ADDRESS                                       0x114

// Type
#define D18F2x114_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x114_Reserved_8_0_OFFSET                           0
#define D18F2x114_Reserved_8_0_WIDTH                            9
#define D18F2x114_Reserved_8_0_MASK                             0x1ff
#define D18F2x114_DctSelIntLvAddr_2__OFFSET                     9
#define D18F2x114_DctSelIntLvAddr_2__WIDTH                      1
#define D18F2x114_DctSelIntLvAddr_2__MASK                       0x200

/// D18F2x114
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                       DctSelIntLvAddr_2_:1 ; ///<
    UINT32                                  :14; ///<
    UINT32                                           :8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x114_STRUCT;


// **** D18F3x00 Register Definition ****
// Address
#define D18F3x00_ADDRESS                                        0x0

// Type
#define D18F3x00_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x00_VendorID_OFFSET                                0
#define D18F3x00_VendorID_WIDTH                                 16
#define D18F3x00_VendorID_MASK                                  0xffff
#define D18F3x00_DeviceID_OFFSET                                16
#define D18F3x00_DeviceID_WIDTH                                 16
#define D18F3x00_DeviceID_MASK                                  0xffff0000

/// D18F3x00
typedef union {
  struct {                                                              ///<
    UINT32                                                 VendorID:16; ///<
    UINT32                                                 DeviceID:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x00_STRUCT;

// **** D18F3x04 Register Definition ****
// Address
#define D18F3x04_ADDRESS                                        0x4

// Type
#define D18F3x04_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x04_Command_OFFSET                                 0
#define D18F3x04_Command_WIDTH                                  16
#define D18F3x04_Command_MASK                                   0xffff
#define D18F3x04_Status_OFFSET                                  16
#define D18F3x04_Status_WIDTH                                   16
#define D18F3x04_Status_MASK                                    0xffff0000

/// D18F3x04
typedef union {
  struct {                                                              ///<
    UINT32                                                  Command:16; ///<
    UINT32                                                   Status:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x04_STRUCT;

// **** D18F3x08 Register Definition ****
// Address
#define D18F3x08_ADDRESS                                        0x8

// Type
#define D18F3x08_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x08_RevID_OFFSET                                   0
#define D18F3x08_RevID_WIDTH                                    8
#define D18F3x08_RevID_MASK                                     0xff
#define D18F3x08_ClassCode_OFFSET                               8
#define D18F3x08_ClassCode_WIDTH                                24
#define D18F3x08_ClassCode_MASK                                 0xffffff00

/// D18F3x08
typedef union {
  struct {                                                              ///<
    UINT32                                                    RevID:8 ; ///<
    UINT32                                                ClassCode:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x08_STRUCT;

// **** D18F3x0C Register Definition ****
// Address
#define D18F3x0C_ADDRESS                                        0xc

// Type
#define D18F3x0C_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x0C_HeaderTypeReg_OFFSET                           0
#define D18F3x0C_HeaderTypeReg_WIDTH                            32
#define D18F3x0C_HeaderTypeReg_MASK                             0xffffffff

/// D18F3x0C
typedef union {
  struct {                                                              ///<
    UINT32                                            HeaderTypeReg:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x0C_STRUCT;

// **** D18F3x34 Register Definition ****
// Address
#define D18F3x34_ADDRESS                                        0x34

// Type
#define D18F3x34_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x34_CapPtr_OFFSET                                  0
#define D18F3x34_CapPtr_WIDTH                                   8
#define D18F3x34_CapPtr_MASK                                    0xff
#define D18F3x34_Reserved_31_8_OFFSET                           8
#define D18F3x34_Reserved_31_8_WIDTH                            24
#define D18F3x34_Reserved_31_8_MASK                             0xffffff00

/// D18F3x34
typedef union {
  struct {                                                              ///<
    UINT32                                                   CapPtr:8 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x34_STRUCT;


// **** D18F3x48 Register Definition ****
// Address
#define D18F3x48_ADDRESS                                        0x48

// Type
#define D18F3x48_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x48_ErrorCode_OFFSET                               0
#define D18F3x48_ErrorCode_WIDTH                                16
#define D18F3x48_ErrorCode_MASK                                 0xffff
#define D18F3x48_ErrorCodeExt_OFFSET                            16
#define D18F3x48_ErrorCodeExt_WIDTH                             5
#define D18F3x48_ErrorCodeExt_MASK                              0x1f0000
#define D18F3x48_Reserved_31_21_OFFSET                          21
#define D18F3x48_Reserved_31_21_WIDTH                           11
#define D18F3x48_Reserved_31_21_MASK                            0xffe00000

/// D18F3x48
typedef union {
  struct {                                                              ///<
    UINT32                                                ErrorCode:16; ///<
    UINT32                                             ErrorCodeExt:5 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x48_STRUCT;


// **** D18F3x64 Register Definition ****
// Address
#define D18F3x64_ADDRESS                                        0x64

// Type
#define D18F3x64_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x64_HtcEn_OFFSET                                   0
#define D18F3x64_HtcEn_WIDTH                                    1
#define D18F3x64_HtcEn_MASK                                     0x1
#define D18F3x64_Reserved_3_1_OFFSET                            1
#define D18F3x64_Reserved_3_1_WIDTH                             3
#define D18F3x64_Reserved_3_1_MASK                              0xe
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
#define D18F3x64_Reserved_15_8_OFFSET                           8
#define D18F3x64_Reserved_15_8_WIDTH                            8
#define D18F3x64_Reserved_15_8_MASK                             0xff00
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

/// D18F3x64
typedef union {
  struct {                                                              ///<
    UINT32                                                    HtcEn:1 ; ///<
    UINT32                                             Reserved_3_1:3 ; ///<
    UINT32                                                   HtcAct:1 ; ///<
    UINT32                                                HtcActSts:1 ; ///<
    UINT32                                              PslApicHiEn:1 ; ///<
    UINT32                                              PslApicLoEn:1 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                                HtcTmpLmt:7 ; ///<
    UINT32                                               HtcSlewSel:1 ; ///<
    UINT32                                               HtcHystLmt:4 ; ///<
    UINT32                                           HtcPstateLimit:3 ; ///<
    UINT32                                                  :1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x64_STRUCT;


// **** D18F3x88 Register Definition ****
// Address
#define D18F3x88_ADDRESS                                        0x88

// Type
#define D18F3x88_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x88_Reserved_31_0_OFFSET                           0
#define D18F3x88_Reserved_31_0_WIDTH                            32
#define D18F3x88_Reserved_31_0_MASK                             0xffffffff

/// D18F3x88
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x88_STRUCT;


// **** D18F3xE4 Register Definition ****
// Address
#define D18F3xE4_ADDRESS                                        0xe4

// Type
#define D18F3xE4_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xE4_Reserved_0_0_OFFSET                            0
#define D18F3xE4_Reserved_0_0_WIDTH                             1
#define D18F3xE4_Reserved_0_0_MASK                              0x1
#define D18F3xE4_Thermtp_OFFSET                                 1
#define D18F3xE4_Thermtp_WIDTH                                  1
#define D18F3xE4_Thermtp_MASK                                   0x2
#define D18F3xE4_Reserved_2_2_OFFSET                            2
#define D18F3xE4_Reserved_2_2_WIDTH                             1
#define D18F3xE4_Reserved_2_2_MASK                              0x4
#define D18F3xE4_ThermtpSense_OFFSET                            3
#define D18F3xE4_ThermtpSense_WIDTH                             1
#define D18F3xE4_ThermtpSense_MASK                              0x8
#define D18F3xE4_Reserved_4_4_OFFSET                            4
#define D18F3xE4_Reserved_4_4_WIDTH                             1
#define D18F3xE4_Reserved_4_4_MASK                              0x10
#define D18F3xE4_ThermtpEn_OFFSET                               5
#define D18F3xE4_ThermtpEn_WIDTH                                1
#define D18F3xE4_ThermtpEn_MASK                                 0x20
#define D18F3xE4_Reserved_7_6_OFFSET                            6
#define D18F3xE4_Reserved_7_6_WIDTH                             2
#define D18F3xE4_Reserved_7_6_MASK                              0xc0
#define D18F3xE4_Reserved_30_8_OFFSET                           8
#define D18F3xE4_Reserved_30_8_WIDTH                            23
#define D18F3xE4_Reserved_30_8_MASK                             0x7fffff00
#define D18F3xE4_SwThermtp_OFFSET                               31
#define D18F3xE4_SwThermtp_WIDTH                                1
#define D18F3xE4_SwThermtp_MASK                                 0x80000000

/// D18F3xE4
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                                  Thermtp:1 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                             ThermtpSense:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                                ThermtpEn:1 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                            Reserved_30_8:23; ///<
    UINT32                                                SwThermtp:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xE4_STRUCT;


// **** D18F3xF0 Register Definition ****
// Address
#define D18F3xF0_ADDRESS                                        0xf0

// Type
#define D18F3xF0_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xF0_Reserved_31_0_OFFSET                           0
#define D18F3xF0_Reserved_31_0_WIDTH                            32
#define D18F3xF0_Reserved_31_0_MASK                             0xffffffff

/// D18F3xF0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xF0_STRUCT;

// **** D18F3xF4 Register Definition ****
// Address
#define D18F3xF4_ADDRESS                                        0xf4

// Type
#define D18F3xF4_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xF4_Reserved_31_0_OFFSET                           0
#define D18F3xF4_Reserved_31_0_WIDTH                            32
#define D18F3xF4_Reserved_31_0_MASK                             0xffffffff

/// D18F3xF4
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xF4_STRUCT;

// **** D18F3xF8 Register Definition ****
// Address
#define D18F3xF8_ADDRESS                                        0xf8

// Type
#define D18F3xF8_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xF8_Reserved_31_0_OFFSET                           0
#define D18F3xF8_Reserved_31_0_WIDTH                            32
#define D18F3xF8_Reserved_31_0_MASK                             0xffffffff

/// D18F3xF8
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xF8_STRUCT;

// **** D18F3xFC Register Definition ****
// Address
#define D18F3xFC_ADDRESS                                        0xfc

// Type
#define D18F3xFC_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xFC_Stepping_OFFSET                                0
#define D18F3xFC_Stepping_WIDTH                                 4
#define D18F3xFC_Stepping_MASK                                  0xf
#define D18F3xFC_BaseModel_OFFSET                               4
#define D18F3xFC_BaseModel_WIDTH                                4
#define D18F3xFC_BaseModel_MASK                                 0xf0
#define D18F3xFC_BaseFamily_OFFSET                              8
#define D18F3xFC_BaseFamily_WIDTH                               4
#define D18F3xFC_BaseFamily_MASK                                0xf00
#define D18F3xFC_Reserved_15_12_OFFSET                          12
#define D18F3xFC_Reserved_15_12_WIDTH                           4
#define D18F3xFC_Reserved_15_12_MASK                            0xf000
#define D18F3xFC_ExtModel_OFFSET                                16
#define D18F3xFC_ExtModel_WIDTH                                 4
#define D18F3xFC_ExtModel_MASK                                  0xf0000
#define D18F3xFC_ExtFamily_OFFSET                               20
#define D18F3xFC_ExtFamily_WIDTH                                8
#define D18F3xFC_ExtFamily_MASK                                 0xff00000
#define D18F3xFC_Reserved_31_28_OFFSET                          28
#define D18F3xFC_Reserved_31_28_WIDTH                           4
#define D18F3xFC_Reserved_31_28_MASK                            0xf0000000

/// D18F3xFC
typedef union {
  struct {                                                              ///<
    UINT32                                                 Stepping:4 ; ///<
    UINT32                                                BaseModel:4 ; ///<
    UINT32                                               BaseFamily:4 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                                 ExtModel:4 ; ///<
    UINT32                                                ExtFamily:8 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xFC_STRUCT;







// **** D18F3x1CC Register Definition ****
// Address
#define D18F3x1CC_ADDRESS                                       0x1cc

// Type
#define D18F3x1CC_TYPE                                          TYPE_D18F3
// Field Data
#define D18F3x1CC_LvtOffset_OFFSET                              0
#define D18F3x1CC_LvtOffset_WIDTH                               4
#define D18F3x1CC_LvtOffset_MASK                                0xf
#define D18F3x1CC_Reserved_7_4_OFFSET                           4
#define D18F3x1CC_Reserved_7_4_WIDTH                            4
#define D18F3x1CC_Reserved_7_4_MASK                             0xf0
#define D18F3x1CC_LvtOffsetVal_OFFSET                           8
#define D18F3x1CC_LvtOffsetVal_WIDTH                            1
#define D18F3x1CC_LvtOffsetVal_MASK                             0x100
#define D18F3x1CC_Reserved_31_9_OFFSET                          9
#define D18F3x1CC_Reserved_31_9_WIDTH                           23
#define D18F3x1CC_Reserved_31_9_MASK                            0xfffffe00

/// D18F3x1CC
typedef union {
  struct {                                                              ///<
    UINT32                                                LvtOffset:4 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                             LvtOffsetVal:1 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x1CC_STRUCT;











// **** DxF0x00 Register Definition ****
// Address
#define DxF0x00_ADDRESS                                         0x0

// Type
#define DxF0x00_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x00_VendorID_OFFSET                                 0
#define DxF0x00_VendorID_WIDTH                                  16
#define DxF0x00_VendorID_MASK                                   0xffff
#define DxF0x00_DeviceID_OFFSET                                 16
#define DxF0x00_DeviceID_WIDTH                                  16
#define DxF0x00_DeviceID_MASK                                   0xffff0000

/// DxF0x00
typedef union {
  struct {                                                              ///<
    UINT32                                                 VendorID:16; ///<
    UINT32                                                 DeviceID:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x00_STRUCT;


// **** DxF0x08 Register Definition ****
// Address
#define DxF0x08_ADDRESS                                         0x8

// Type
#define DxF0x08_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x08_RevID_OFFSET                                    0
#define DxF0x08_RevID_WIDTH                                     8
#define DxF0x08_RevID_MASK                                      0xff
#define DxF0x08_ClassCode_OFFSET                                8
#define DxF0x08_ClassCode_WIDTH                                 24
#define DxF0x08_ClassCode_MASK                                  0xffffff00

/// DxF0x08
typedef union {
  struct {                                                              ///<
    UINT32                                                    RevID:8 ; ///<
    UINT32                                                ClassCode:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x08_STRUCT;

// **** DxF0x0C Register Definition ****
// Address
#define DxF0x0C_ADDRESS                                         0xc

// Type
#define DxF0x0C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x0C_CacheLineSize_OFFSET                            0
#define DxF0x0C_CacheLineSize_WIDTH                             8
#define DxF0x0C_CacheLineSize_MASK                              0xff
#define DxF0x0C_LatencyTimer_OFFSET                             8
#define DxF0x0C_LatencyTimer_WIDTH                              8
#define DxF0x0C_LatencyTimer_MASK                               0xff00
#define DxF0x0C_HeaderTypeReg_OFFSET                            16
#define DxF0x0C_HeaderTypeReg_WIDTH                             8
#define DxF0x0C_HeaderTypeReg_MASK                              0xff0000
#define DxF0x0C_BIST_OFFSET                                     24
#define DxF0x0C_BIST_WIDTH                                      8
#define DxF0x0C_BIST_MASK                                       0xff000000

/// DxF0x0C
typedef union {
  struct {                                                              ///<
    UINT32                                            CacheLineSize:8 ; ///<
    UINT32                                             LatencyTimer:8 ; ///<
    UINT32                                            HeaderTypeReg:8 ; ///<
    UINT32                                                     BIST:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x0C_STRUCT;

// **** DxF0x18 Register Definition ****
// Address
#define DxF0x18_ADDRESS                                         0x18

// Type
#define DxF0x18_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x18_PrimaryBus_OFFSET                               0
#define DxF0x18_PrimaryBus_WIDTH                                8
#define DxF0x18_PrimaryBus_MASK                                 0xff
#define DxF0x18_SecondaryBus_OFFSET                             8
#define DxF0x18_SecondaryBus_WIDTH                              8
#define DxF0x18_SecondaryBus_MASK                               0xff00
#define DxF0x18_SubBusNumber_OFFSET                             16
#define DxF0x18_SubBusNumber_WIDTH                              8
#define DxF0x18_SubBusNumber_MASK                               0xff0000
#define DxF0x18_SecondaryLatencyTimer_OFFSET                    24
#define DxF0x18_SecondaryLatencyTimer_WIDTH                     8
#define DxF0x18_SecondaryLatencyTimer_MASK                      0xff000000

/// DxF0x18
typedef union {
  struct {                                                              ///<
    UINT32                                               PrimaryBus:8 ; ///<
    UINT32                                             SecondaryBus:8 ; ///<
    UINT32                                             SubBusNumber:8 ; ///<
    UINT32                                    SecondaryLatencyTimer:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x18_STRUCT;


// **** DxF0x20 Register Definition ****
// Address
#define DxF0x20_ADDRESS                                         0x20

// Type
#define DxF0x20_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x20_Reserved_3_0_OFFSET                             0
#define DxF0x20_Reserved_3_0_WIDTH                              4
#define DxF0x20_Reserved_3_0_MASK                               0xf
#define DxF0x20_MemBase_OFFSET                                  4
#define DxF0x20_MemBase_WIDTH                                   12
#define DxF0x20_MemBase_MASK                                    0xfff0
#define DxF0x20_Reserved_19_16_OFFSET                           16
#define DxF0x20_Reserved_19_16_WIDTH                            4
#define DxF0x20_Reserved_19_16_MASK                             0xf0000
#define DxF0x20_MemLimit_OFFSET                                 20
#define DxF0x20_MemLimit_WIDTH                                  12
#define DxF0x20_MemLimit_MASK                                   0xfff00000

/// DxF0x20
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                                  MemBase:12; ///<
    UINT32                                           Reserved_19_16:4 ; ///<
    UINT32                                                 MemLimit:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x20_STRUCT;

// **** DxF0x24 Register Definition ****
// Address
#define DxF0x24_ADDRESS                                         0x24

// Type
#define DxF0x24_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x24_PrefMemBaseR_OFFSET                             0
#define DxF0x24_PrefMemBaseR_WIDTH                              4
#define DxF0x24_PrefMemBaseR_MASK                               0xf
#define DxF0x24_PrefMemBase_31_20__OFFSET                       4
#define DxF0x24_PrefMemBase_31_20__WIDTH                        12
#define DxF0x24_PrefMemBase_31_20__MASK                         0xfff0
#define DxF0x24_PrefMemLimitR_OFFSET                            16
#define DxF0x24_PrefMemLimitR_WIDTH                             4
#define DxF0x24_PrefMemLimitR_MASK                              0xf0000
#define DxF0x24_PrefMemLimit_OFFSET                             20
#define DxF0x24_PrefMemLimit_WIDTH                              12
#define DxF0x24_PrefMemLimit_MASK                               0xfff00000

/// DxF0x24
typedef union {
  struct {                                                              ///<
    UINT32                                             PrefMemBaseR:4 ; ///<
    UINT32                                       PrefMemBase_31_20_:12; ///<
    UINT32                                            PrefMemLimitR:4 ; ///<
    UINT32                                             PrefMemLimit:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x24_STRUCT;

// **** DxF0x28 Register Definition ****
// Address
#define DxF0x28_ADDRESS                                         0x28

// Type
#define DxF0x28_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x28_PrefMemBase_63_32__OFFSET                       0
#define DxF0x28_PrefMemBase_63_32__WIDTH                        32
#define DxF0x28_PrefMemBase_63_32__MASK                         0xffffffff

/// DxF0x28
typedef union {
  struct {                                                              ///<
    UINT32                                       PrefMemBase_63_32_:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x28_STRUCT;

// **** DxF0x2C Register Definition ****
// Address
#define DxF0x2C_ADDRESS                                         0x2c

// Type
#define DxF0x2C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x2C_PrefMemLimit_63_32__OFFSET                      0
#define DxF0x2C_PrefMemLimit_63_32__WIDTH                       32
#define DxF0x2C_PrefMemLimit_63_32__MASK                        0xffffffff

/// DxF0x2C
typedef union {
  struct {                                                              ///<
    UINT32                                      PrefMemLimit_63_32_:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x2C_STRUCT;

// **** DxF0x30 Register Definition ****
// Address
#define DxF0x30_ADDRESS                                         0x30

// Type
#define DxF0x30_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x30_IOBase_31_16__OFFSET                            0
#define DxF0x30_IOBase_31_16__WIDTH                             16
#define DxF0x30_IOBase_31_16__MASK                              0xffff
#define DxF0x30_IOLimit_31_16__OFFSET                           16
#define DxF0x30_IOLimit_31_16__WIDTH                            16
#define DxF0x30_IOLimit_31_16__MASK                             0xffff0000

/// DxF0x30
typedef union {
  struct {                                                              ///<
    UINT32                                            IOBase_31_16_:16; ///<
    UINT32                                           IOLimit_31_16_:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x30_STRUCT;

// **** DxF0x34 Register Definition ****
// Address
#define DxF0x34_ADDRESS                                         0x34

// Type
#define DxF0x34_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x34_CapPtr_OFFSET                                   0
#define DxF0x34_CapPtr_WIDTH                                    8
#define DxF0x34_CapPtr_MASK                                     0xff
#define DxF0x34_Reserved_31_8_OFFSET                            8
#define DxF0x34_Reserved_31_8_WIDTH                             24
#define DxF0x34_Reserved_31_8_MASK                              0xffffff00

/// DxF0x34
typedef union {
  struct {                                                              ///<
    UINT32                                                   CapPtr:8 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x34_STRUCT;

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

// **** DxF0x50 Register Definition ****
// Address
#define DxF0x50_ADDRESS                                         0x50

// Type
#define DxF0x50_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x50_CapID_OFFSET                                    0
#define DxF0x50_CapID_WIDTH                                     8
#define DxF0x50_CapID_MASK                                      0xff
#define DxF0x50_NextPtr_OFFSET                                  8
#define DxF0x50_NextPtr_WIDTH                                   8
#define DxF0x50_NextPtr_MASK                                    0xff00
#define DxF0x50_Version_OFFSET                                  16
#define DxF0x50_Version_WIDTH                                   3
#define DxF0x50_Version_MASK                                    0x70000
#define DxF0x50_PmeClock_OFFSET                                 19
#define DxF0x50_PmeClock_WIDTH                                  1
#define DxF0x50_PmeClock_MASK                                   0x80000
#define DxF0x50_Reserved_20_20_OFFSET                           20
#define DxF0x50_Reserved_20_20_WIDTH                            1
#define DxF0x50_Reserved_20_20_MASK                             0x100000
#define DxF0x50_DevSpecificInit_OFFSET                          21
#define DxF0x50_DevSpecificInit_WIDTH                           1
#define DxF0x50_DevSpecificInit_MASK                            0x200000
#define DxF0x50_AuxCurrent_OFFSET                               22
#define DxF0x50_AuxCurrent_WIDTH                                3
#define DxF0x50_AuxCurrent_MASK                                 0x1c00000
#define DxF0x50_D1Support_OFFSET                                25
#define DxF0x50_D1Support_WIDTH                                 1
#define DxF0x50_D1Support_MASK                                  0x2000000
#define DxF0x50_D2Support_OFFSET                                26
#define DxF0x50_D2Support_WIDTH                                 1
#define DxF0x50_D2Support_MASK                                  0x4000000
#define DxF0x50_PmeSupport_OFFSET                               27
#define DxF0x50_PmeSupport_WIDTH                                5
#define DxF0x50_PmeSupport_MASK                                 0xf8000000

/// DxF0x50
typedef union {
  struct {                                                              ///<
    UINT32                                                    CapID:8 ; ///<
    UINT32                                                  NextPtr:8 ; ///<
    UINT32                                                  Version:3 ; ///<
    UINT32                                                 PmeClock:1 ; ///<
    UINT32                                           Reserved_20_20:1 ; ///<
    UINT32                                          DevSpecificInit:1 ; ///<
    UINT32                                               AuxCurrent:3 ; ///<
    UINT32                                                D1Support:1 ; ///<
    UINT32                                                D2Support:1 ; ///<
    UINT32                                               PmeSupport:5 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x50_STRUCT;

// **** DxF0x54 Register Definition ****
// Address
#define DxF0x54_ADDRESS                                         0x54

// Type
#define DxF0x54_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x54_PowerState_OFFSET                               0
#define DxF0x54_PowerState_WIDTH                                2
#define DxF0x54_PowerState_MASK                                 0x3
#define DxF0x54_Reserved_2_2_OFFSET                             2
#define DxF0x54_Reserved_2_2_WIDTH                              1
#define DxF0x54_Reserved_2_2_MASK                               0x4
#define DxF0x54_NoSoftReset_OFFSET                              3
#define DxF0x54_NoSoftReset_WIDTH                               1
#define DxF0x54_NoSoftReset_MASK                                0x8
#define DxF0x54_Reserved_7_4_OFFSET                             4
#define DxF0x54_Reserved_7_4_WIDTH                              4
#define DxF0x54_Reserved_7_4_MASK                               0xf0
#define DxF0x54_PmeEn_OFFSET                                    8
#define DxF0x54_PmeEn_WIDTH                                     1
#define DxF0x54_PmeEn_MASK                                      0x100
#define DxF0x54_DataSelect_OFFSET                               9
#define DxF0x54_DataSelect_WIDTH                                4
#define DxF0x54_DataSelect_MASK                                 0x1e00
#define DxF0x54_DataScale_OFFSET                                13
#define DxF0x54_DataScale_WIDTH                                 2
#define DxF0x54_DataScale_MASK                                  0x6000
#define DxF0x54_PmeStatus_OFFSET                                15
#define DxF0x54_PmeStatus_WIDTH                                 1
#define DxF0x54_PmeStatus_MASK                                  0x8000
#define DxF0x54_Reserved_21_16_OFFSET                           16
#define DxF0x54_Reserved_21_16_WIDTH                            6
#define DxF0x54_Reserved_21_16_MASK                             0x3f0000
#define DxF0x54_B2B3Support_OFFSET                              22
#define DxF0x54_B2B3Support_WIDTH                               1
#define DxF0x54_B2B3Support_MASK                                0x400000
#define DxF0x54_BusPwrEn_OFFSET                                 23
#define DxF0x54_BusPwrEn_WIDTH                                  1
#define DxF0x54_BusPwrEn_MASK                                   0x800000
#define DxF0x54_PmeData_OFFSET                                  24
#define DxF0x54_PmeData_WIDTH                                   8
#define DxF0x54_PmeData_MASK                                    0xff000000

/// DxF0x54
typedef union {
  struct {                                                              ///<
    UINT32                                               PowerState:2 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                              NoSoftReset:1 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                                    PmeEn:1 ; ///<
    UINT32                                               DataSelect:4 ; ///<
    UINT32                                                DataScale:2 ; ///<
    UINT32                                                PmeStatus:1 ; ///<
    UINT32                                           Reserved_21_16:6 ; ///<
    UINT32                                              B2B3Support:1 ; ///<
    UINT32                                                 BusPwrEn:1 ; ///<
    UINT32                                                  PmeData:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x54_STRUCT;

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

// **** DxF0x5C Register Definition ****
// Address
#define DxF0x5C_ADDRESS                                         0x5c

// Type
#define DxF0x5C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x5C_MaxPayloadSupport_OFFSET                        0
#define DxF0x5C_MaxPayloadSupport_WIDTH                         3
#define DxF0x5C_MaxPayloadSupport_MASK                          0x7
#define DxF0x5C_PhantomFunc_OFFSET                              3
#define DxF0x5C_PhantomFunc_WIDTH                               2
#define DxF0x5C_PhantomFunc_MASK                                0x18
#define DxF0x5C_ExtendedTag_OFFSET                              5
#define DxF0x5C_ExtendedTag_WIDTH                               1
#define DxF0x5C_ExtendedTag_MASK                                0x20
#define DxF0x5C_L0SAcceptableLatency_OFFSET                     6
#define DxF0x5C_L0SAcceptableLatency_WIDTH                      3
#define DxF0x5C_L0SAcceptableLatency_MASK                       0x1c0
#define DxF0x5C_L1AcceptableLatency_OFFSET                      9
#define DxF0x5C_L1AcceptableLatency_WIDTH                       3
#define DxF0x5C_L1AcceptableLatency_MASK                        0xe00
#define DxF0x5C_Reserved_14_12_OFFSET                           12
#define DxF0x5C_Reserved_14_12_WIDTH                            3
#define DxF0x5C_Reserved_14_12_MASK                             0x7000
#define DxF0x5C_RoleBasedErrReporting_OFFSET                    15
#define DxF0x5C_RoleBasedErrReporting_WIDTH                     1
#define DxF0x5C_RoleBasedErrReporting_MASK                      0x8000
#define DxF0x5C_Reserved_17_16_OFFSET                           16
#define DxF0x5C_Reserved_17_16_WIDTH                            2
#define DxF0x5C_Reserved_17_16_MASK                             0x30000
#define DxF0x5C_CapturedSlotPowerLimit_OFFSET                   18
#define DxF0x5C_CapturedSlotPowerLimit_WIDTH                    8
#define DxF0x5C_CapturedSlotPowerLimit_MASK                     0x3fc0000
#define DxF0x5C_CapturedSlotPowerScale_OFFSET                   26
#define DxF0x5C_CapturedSlotPowerScale_WIDTH                    2
#define DxF0x5C_CapturedSlotPowerScale_MASK                     0xc000000
#define DxF0x5C_FlrCapable_OFFSET                               28
#define DxF0x5C_FlrCapable_WIDTH                                1
#define DxF0x5C_FlrCapable_MASK                                 0x10000000
#define DxF0x5C_Reserved_31_29_OFFSET                           29
#define DxF0x5C_Reserved_31_29_WIDTH                            3
#define DxF0x5C_Reserved_31_29_MASK                             0xe0000000

/// DxF0x5C
typedef union {
  struct {                                                              ///<
    UINT32                                        MaxPayloadSupport:3 ; ///<
    UINT32                                              PhantomFunc:2 ; ///<
    UINT32                                              ExtendedTag:1 ; ///<
    UINT32                                     L0SAcceptableLatency:3 ; ///<
    UINT32                                      L1AcceptableLatency:3 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                    RoleBasedErrReporting:1 ; ///<
    UINT32                                           Reserved_17_16:2 ; ///<
    UINT32                                   CapturedSlotPowerLimit:8 ; ///<
    UINT32                                   CapturedSlotPowerScale:2 ; ///<
    UINT32                                               FlrCapable:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x5C_STRUCT;

// **** DxF0x60 Register Definition ****
// Address
#define DxF0x60_ADDRESS                                         0x60

// Type
#define DxF0x60_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x60_CorrErrEn_OFFSET                                0
#define DxF0x60_CorrErrEn_WIDTH                                 1
#define DxF0x60_CorrErrEn_MASK                                  0x1
#define DxF0x60_NonFatalErrEn_OFFSET                            1
#define DxF0x60_NonFatalErrEn_WIDTH                             1
#define DxF0x60_NonFatalErrEn_MASK                              0x2
#define DxF0x60_FatalErrEn_OFFSET                               2
#define DxF0x60_FatalErrEn_WIDTH                                1
#define DxF0x60_FatalErrEn_MASK                                 0x4
#define DxF0x60_UsrReportEn_OFFSET                              3
#define DxF0x60_UsrReportEn_WIDTH                               1
#define DxF0x60_UsrReportEn_MASK                                0x8
#define DxF0x60_RelaxedOrdEn_OFFSET                             4
#define DxF0x60_RelaxedOrdEn_WIDTH                              1
#define DxF0x60_RelaxedOrdEn_MASK                               0x10
#define DxF0x60_MaxPayloadSize_OFFSET                           5
#define DxF0x60_MaxPayloadSize_WIDTH                            3
#define DxF0x60_MaxPayloadSize_MASK                             0xe0
#define DxF0x60_ExtendedTagEn_OFFSET                            8
#define DxF0x60_ExtendedTagEn_WIDTH                             1
#define DxF0x60_ExtendedTagEn_MASK                              0x100
#define DxF0x60_PhantomFuncEn_OFFSET                            9
#define DxF0x60_PhantomFuncEn_WIDTH                             1
#define DxF0x60_PhantomFuncEn_MASK                              0x200
#define DxF0x60_AuxPowerPmEn_OFFSET                             10
#define DxF0x60_AuxPowerPmEn_WIDTH                              1
#define DxF0x60_AuxPowerPmEn_MASK                               0x400
#define DxF0x60_NoSnoopEnable_OFFSET                            11
#define DxF0x60_NoSnoopEnable_WIDTH                             1
#define DxF0x60_NoSnoopEnable_MASK                              0x800
#define DxF0x60_MaxRequestSize_OFFSET                           12
#define DxF0x60_MaxRequestSize_WIDTH                            3
#define DxF0x60_MaxRequestSize_MASK                             0x7000
#define DxF0x60_BridgeCfgRetryEn_OFFSET                         15
#define DxF0x60_BridgeCfgRetryEn_WIDTH                          1
#define DxF0x60_BridgeCfgRetryEn_MASK                           0x8000
#define DxF0x60_CorrErr_OFFSET                                  16
#define DxF0x60_CorrErr_WIDTH                                   1
#define DxF0x60_CorrErr_MASK                                    0x10000
#define DxF0x60_NonFatalErr_OFFSET                              17
#define DxF0x60_NonFatalErr_WIDTH                               1
#define DxF0x60_NonFatalErr_MASK                                0x20000
#define DxF0x60_FatalErr_OFFSET                                 18
#define DxF0x60_FatalErr_WIDTH                                  1
#define DxF0x60_FatalErr_MASK                                   0x40000
#define DxF0x60_UsrDetected_OFFSET                              19
#define DxF0x60_UsrDetected_WIDTH                               1
#define DxF0x60_UsrDetected_MASK                                0x80000
#define DxF0x60_AuxPwr_OFFSET                                   20
#define DxF0x60_AuxPwr_WIDTH                                    1
#define DxF0x60_AuxPwr_MASK                                     0x100000
#define DxF0x60_TransactionsPending_OFFSET                      21
#define DxF0x60_TransactionsPending_WIDTH                       1
#define DxF0x60_TransactionsPending_MASK                        0x200000
#define DxF0x60_Reserved_31_22_OFFSET                           22
#define DxF0x60_Reserved_31_22_WIDTH                            10
#define DxF0x60_Reserved_31_22_MASK                             0xffc00000

/// DxF0x60
typedef union {
  struct {                                                              ///<
    UINT32                                                CorrErrEn:1 ; ///<
    UINT32                                            NonFatalErrEn:1 ; ///<
    UINT32                                               FatalErrEn:1 ; ///<
    UINT32                                              UsrReportEn:1 ; ///<
    UINT32                                             RelaxedOrdEn:1 ; ///<
    UINT32                                           MaxPayloadSize:3 ; ///<
    UINT32                                            ExtendedTagEn:1 ; ///<
    UINT32                                            PhantomFuncEn:1 ; ///<
    UINT32                                             AuxPowerPmEn:1 ; ///<
    UINT32                                            NoSnoopEnable:1 ; ///<
    UINT32                                           MaxRequestSize:3 ; ///<
    UINT32                                         BridgeCfgRetryEn:1 ; ///<
    UINT32                                                  CorrErr:1 ; ///<
    UINT32                                              NonFatalErr:1 ; ///<
    UINT32                                                 FatalErr:1 ; ///<
    UINT32                                              UsrDetected:1 ; ///<
    UINT32                                                   AuxPwr:1 ; ///<
    UINT32                                      TransactionsPending:1 ; ///<
    UINT32                                           Reserved_31_22:10; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x60_STRUCT;


// **** DxF0x68 Register Definition ****
// Address
#define DxF0x68_ADDRESS                                         0x68

// Type
#define DxF0x68_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x68_PmControl_OFFSET                                0
#define DxF0x68_PmControl_WIDTH                                 2
#define DxF0x68_PmControl_MASK                                  0x3
#define DxF0x68_Reserved_2_2_OFFSET                             2
#define DxF0x68_Reserved_2_2_WIDTH                              1
#define DxF0x68_Reserved_2_2_MASK                               0x4
#define DxF0x68_ReadCplBoundary_OFFSET                          3
#define DxF0x68_ReadCplBoundary_WIDTH                           1
#define DxF0x68_ReadCplBoundary_MASK                            0x8
#define DxF0x68_LinkDis_OFFSET                                  4
#define DxF0x68_LinkDis_WIDTH                                   1
#define DxF0x68_LinkDis_MASK                                    0x10
#define DxF0x68_RetrainLink_OFFSET                              5
#define DxF0x68_RetrainLink_WIDTH                               1
#define DxF0x68_RetrainLink_MASK                                0x20
#define DxF0x68_CommonClockCfg_OFFSET                           6
#define DxF0x68_CommonClockCfg_WIDTH                            1
#define DxF0x68_CommonClockCfg_MASK                             0x40
#define DxF0x68_ExtendedSync_OFFSET                             7
#define DxF0x68_ExtendedSync_WIDTH                              1
#define DxF0x68_ExtendedSync_MASK                               0x80
#define DxF0x68_ClockPowerManagementEn_OFFSET                   8
#define DxF0x68_ClockPowerManagementEn_WIDTH                    1
#define DxF0x68_ClockPowerManagementEn_MASK                     0x100
#define DxF0x68_HWAutonomousWidthDisable_OFFSET                 9
#define DxF0x68_HWAutonomousWidthDisable_WIDTH                  1
#define DxF0x68_HWAutonomousWidthDisable_MASK                   0x200
#define DxF0x68_LinkBWManagementEn_OFFSET                       10
#define DxF0x68_LinkBWManagementEn_WIDTH                        1
#define DxF0x68_LinkBWManagementEn_MASK                         0x400
#define DxF0x68_LinkAutonomousBWIntEn_OFFSET                    11
#define DxF0x68_LinkAutonomousBWIntEn_WIDTH                     1
#define DxF0x68_LinkAutonomousBWIntEn_MASK                      0x800
#define DxF0x68_Reserved_15_12_OFFSET                           12
#define DxF0x68_Reserved_15_12_WIDTH                            4
#define DxF0x68_Reserved_15_12_MASK                             0xf000
#define DxF0x68_LinkSpeed_OFFSET                                16
#define DxF0x68_LinkSpeed_WIDTH                                 4
#define DxF0x68_LinkSpeed_MASK                                  0xf0000
#define DxF0x68_NegotiatedLinkWidth_OFFSET                      20
#define DxF0x68_NegotiatedLinkWidth_WIDTH                       6
#define DxF0x68_NegotiatedLinkWidth_MASK                        0x3f00000
#define DxF0x68_Reserved_26_26_OFFSET                           26
#define DxF0x68_Reserved_26_26_WIDTH                            1
#define DxF0x68_Reserved_26_26_MASK                             0x4000000
#define DxF0x68_LinkTraining_OFFSET                             27
#define DxF0x68_LinkTraining_WIDTH                              1
#define DxF0x68_LinkTraining_MASK                               0x8000000
#define DxF0x68_SlotClockCfg_OFFSET                             28
#define DxF0x68_SlotClockCfg_WIDTH                              1
#define DxF0x68_SlotClockCfg_MASK                               0x10000000
#define DxF0x68_DlActive_OFFSET                                 29
#define DxF0x68_DlActive_WIDTH                                  1
#define DxF0x68_DlActive_MASK                                   0x20000000
#define DxF0x68_LinkBWManagementStatus_OFFSET                   30
#define DxF0x68_LinkBWManagementStatus_WIDTH                    1
#define DxF0x68_LinkBWManagementStatus_MASK                     0x40000000
#define DxF0x68_LinkAutonomousBWStatus_OFFSET                   31
#define DxF0x68_LinkAutonomousBWStatus_WIDTH                    1
#define DxF0x68_LinkAutonomousBWStatus_MASK                     0x80000000

/// DxF0x68
typedef union {
  struct {                                                              ///<
    UINT32                                                PmControl:2 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                          ReadCplBoundary:1 ; ///<
    UINT32                                                  LinkDis:1 ; ///<
    UINT32                                              RetrainLink:1 ; ///<
    UINT32                                           CommonClockCfg:1 ; ///<
    UINT32                                             ExtendedSync:1 ; ///<
    UINT32                                   ClockPowerManagementEn:1 ; ///<
    UINT32                                 HWAutonomousWidthDisable:1 ; ///<
    UINT32                                       LinkBWManagementEn:1 ; ///<
    UINT32                                    LinkAutonomousBWIntEn:1 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                                LinkSpeed:4 ; ///<
    UINT32                                      NegotiatedLinkWidth:6 ; ///<
    UINT32                                           Reserved_26_26:1 ; ///<
    UINT32                                             LinkTraining:1 ; ///<
    UINT32                                             SlotClockCfg:1 ; ///<
    UINT32                                                 DlActive:1 ; ///<
    UINT32                                   LinkBWManagementStatus:1 ; ///<
    UINT32                                   LinkAutonomousBWStatus:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x68_STRUCT;

// **** DxF0x6C Register Definition ****
// Address
#define DxF0x6C_ADDRESS                                         0x6c

// Type
#define DxF0x6C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x6C_AttnButtonPresent_OFFSET                        0
#define DxF0x6C_AttnButtonPresent_WIDTH                         1
#define DxF0x6C_AttnButtonPresent_MASK                          0x1
#define DxF0x6C_PwrControllerPresent_OFFSET                     1
#define DxF0x6C_PwrControllerPresent_WIDTH                      1
#define DxF0x6C_PwrControllerPresent_MASK                       0x2
#define DxF0x6C_MrlSensorPresent_OFFSET                         2
#define DxF0x6C_MrlSensorPresent_WIDTH                          1
#define DxF0x6C_MrlSensorPresent_MASK                           0x4
#define DxF0x6C_AttnIndicatorPresent_OFFSET                     3
#define DxF0x6C_AttnIndicatorPresent_WIDTH                      1
#define DxF0x6C_AttnIndicatorPresent_MASK                       0x8
#define DxF0x6C_PwrIndicatorPresent_OFFSET                      4
#define DxF0x6C_PwrIndicatorPresent_WIDTH                       1
#define DxF0x6C_PwrIndicatorPresent_MASK                        0x10
#define DxF0x6C_HotplugSurprise_OFFSET                          5
#define DxF0x6C_HotplugSurprise_WIDTH                           1
#define DxF0x6C_HotplugSurprise_MASK                            0x20
#define DxF0x6C_HotplugCapable_OFFSET                           6
#define DxF0x6C_HotplugCapable_WIDTH                            1
#define DxF0x6C_HotplugCapable_MASK                             0x40
#define DxF0x6C_SlotPwrLimitValue_OFFSET                        7
#define DxF0x6C_SlotPwrLimitValue_WIDTH                         8
#define DxF0x6C_SlotPwrLimitValue_MASK                          0x7f80
#define DxF0x6C_SlotPwrLimitScale_OFFSET                        15
#define DxF0x6C_SlotPwrLimitScale_WIDTH                         2
#define DxF0x6C_SlotPwrLimitScale_MASK                          0x18000
#define DxF0x6C_ElecMechIlPresent_OFFSET                        17
#define DxF0x6C_ElecMechIlPresent_WIDTH                         1
#define DxF0x6C_ElecMechIlPresent_MASK                          0x20000
#define DxF0x6C_NoCmdCplSupport_OFFSET                          18
#define DxF0x6C_NoCmdCplSupport_WIDTH                           1
#define DxF0x6C_NoCmdCplSupport_MASK                            0x40000
#define DxF0x6C_PhysicalSlotNumber_OFFSET                       19
#define DxF0x6C_PhysicalSlotNumber_WIDTH                        13
#define DxF0x6C_PhysicalSlotNumber_MASK                         0xfff80000

/// DxF0x6C
typedef union {
  struct {                                                              ///<
    UINT32                                        AttnButtonPresent:1 ; ///<
    UINT32                                     PwrControllerPresent:1 ; ///<
    UINT32                                         MrlSensorPresent:1 ; ///<
    UINT32                                     AttnIndicatorPresent:1 ; ///<
    UINT32                                      PwrIndicatorPresent:1 ; ///<
    UINT32                                          HotplugSurprise:1 ; ///<
    UINT32                                           HotplugCapable:1 ; ///<
    UINT32                                        SlotPwrLimitValue:8 ; ///<
    UINT32                                        SlotPwrLimitScale:2 ; ///<
    UINT32                                        ElecMechIlPresent:1 ; ///<
    UINT32                                          NoCmdCplSupport:1 ; ///<
    UINT32                                       PhysicalSlotNumber:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x6C_STRUCT;

// **** DxF0x70 Register Definition ****
// Address
#define DxF0x70_ADDRESS                                         0x70

// Type
#define DxF0x70_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x70_AttnButtonPressedEn_OFFSET                      0
#define DxF0x70_AttnButtonPressedEn_WIDTH                       1
#define DxF0x70_AttnButtonPressedEn_MASK                        0x1
#define DxF0x70_PwrFaultDetectedEn_OFFSET                       1
#define DxF0x70_PwrFaultDetectedEn_WIDTH                        1
#define DxF0x70_PwrFaultDetectedEn_MASK                         0x2
#define DxF0x70_MrlSensorChangedEn_OFFSET                       2
#define DxF0x70_MrlSensorChangedEn_WIDTH                        1
#define DxF0x70_MrlSensorChangedEn_MASK                         0x4
#define DxF0x70_PresenceDetectChangedEn_OFFSET                  3
#define DxF0x70_PresenceDetectChangedEn_WIDTH                   1
#define DxF0x70_PresenceDetectChangedEn_MASK                    0x8
#define DxF0x70_CmdCplIntrEn_OFFSET                             4
#define DxF0x70_CmdCplIntrEn_WIDTH                              1
#define DxF0x70_CmdCplIntrEn_MASK                               0x10
#define DxF0x70_HotplugIntrEn_OFFSET                            5
#define DxF0x70_HotplugIntrEn_WIDTH                             1
#define DxF0x70_HotplugIntrEn_MASK                              0x20
#define DxF0x70_AttnIndicatorControl_OFFSET                     6
#define DxF0x70_AttnIndicatorControl_WIDTH                      2
#define DxF0x70_AttnIndicatorControl_MASK                       0xc0
#define DxF0x70_PwrIndicatorCntl_OFFSET                         8
#define DxF0x70_PwrIndicatorCntl_WIDTH                          2
#define DxF0x70_PwrIndicatorCntl_MASK                           0x300
#define DxF0x70_PwrControllerCntl_OFFSET                        10
#define DxF0x70_PwrControllerCntl_WIDTH                         1
#define DxF0x70_PwrControllerCntl_MASK                          0x400
#define DxF0x70_ElecMechIlCntl_OFFSET                           11
#define DxF0x70_ElecMechIlCntl_WIDTH                            1
#define DxF0x70_ElecMechIlCntl_MASK                             0x800
#define DxF0x70_DlStateChangedEn_OFFSET                         12
#define DxF0x70_DlStateChangedEn_WIDTH                          1
#define DxF0x70_DlStateChangedEn_MASK                           0x1000
#define DxF0x70_Reserved_15_13_OFFSET                           13
#define DxF0x70_Reserved_15_13_WIDTH                            3
#define DxF0x70_Reserved_15_13_MASK                             0xe000
#define DxF0x70_AttnButtonPressed_OFFSET                        16
#define DxF0x70_AttnButtonPressed_WIDTH                         1
#define DxF0x70_AttnButtonPressed_MASK                          0x10000
#define DxF0x70_PwrFaultDetected_OFFSET                         17
#define DxF0x70_PwrFaultDetected_WIDTH                          1
#define DxF0x70_PwrFaultDetected_MASK                           0x20000
#define DxF0x70_MrlSensorChanged_OFFSET                         18
#define DxF0x70_MrlSensorChanged_WIDTH                          1
#define DxF0x70_MrlSensorChanged_MASK                           0x40000
#define DxF0x70_PresenceDetectChanged_OFFSET                    19
#define DxF0x70_PresenceDetectChanged_WIDTH                     1
#define DxF0x70_PresenceDetectChanged_MASK                      0x80000
#define DxF0x70_CmdCpl_OFFSET                                   20
#define DxF0x70_CmdCpl_WIDTH                                    1
#define DxF0x70_CmdCpl_MASK                                     0x100000
#define DxF0x70_MrlSensorState_OFFSET                           21
#define DxF0x70_MrlSensorState_WIDTH                            1
#define DxF0x70_MrlSensorState_MASK                             0x200000
#define DxF0x70_PresenceDetectState_OFFSET                      22
#define DxF0x70_PresenceDetectState_WIDTH                       1
#define DxF0x70_PresenceDetectState_MASK                        0x400000
#define DxF0x70_ElecMechIlSts_OFFSET                            23
#define DxF0x70_ElecMechIlSts_WIDTH                             1
#define DxF0x70_ElecMechIlSts_MASK                              0x800000
#define DxF0x70_DlStateChanged_OFFSET                           24
#define DxF0x70_DlStateChanged_WIDTH                            1
#define DxF0x70_DlStateChanged_MASK                             0x1000000
#define DxF0x70_Reserved_31_25_OFFSET                           25
#define DxF0x70_Reserved_31_25_WIDTH                            7
#define DxF0x70_Reserved_31_25_MASK                             0xfe000000

/// DxF0x70
typedef union {
  struct {                                                              ///<
    UINT32                                      AttnButtonPressedEn:1 ; ///<
    UINT32                                       PwrFaultDetectedEn:1 ; ///<
    UINT32                                       MrlSensorChangedEn:1 ; ///<
    UINT32                                  PresenceDetectChangedEn:1 ; ///<
    UINT32                                             CmdCplIntrEn:1 ; ///<
    UINT32                                            HotplugIntrEn:1 ; ///<
    UINT32                                     AttnIndicatorControl:2 ; ///<
    UINT32                                         PwrIndicatorCntl:2 ; ///<
    UINT32                                        PwrControllerCntl:1 ; ///<
    UINT32                                           ElecMechIlCntl:1 ; ///<
    UINT32                                         DlStateChangedEn:1 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                        AttnButtonPressed:1 ; ///<
    UINT32                                         PwrFaultDetected:1 ; ///<
    UINT32                                         MrlSensorChanged:1 ; ///<
    UINT32                                    PresenceDetectChanged:1 ; ///<
    UINT32                                                   CmdCpl:1 ; ///<
    UINT32                                           MrlSensorState:1 ; ///<
    UINT32                                      PresenceDetectState:1 ; ///<
    UINT32                                            ElecMechIlSts:1 ; ///<
    UINT32                                           DlStateChanged:1 ; ///<
    UINT32                                           Reserved_31_25:7 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x70_STRUCT;

// **** DxF0x74 Register Definition ****
// Address
#define DxF0x74_ADDRESS                                         0x74

// Type
#define DxF0x74_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x74_SerrOnCorrErrEn_OFFSET                          0
#define DxF0x74_SerrOnCorrErrEn_WIDTH                           1
#define DxF0x74_SerrOnCorrErrEn_MASK                            0x1
#define DxF0x74_SerrOnNonFatalErrEn_OFFSET                      1
#define DxF0x74_SerrOnNonFatalErrEn_WIDTH                       1
#define DxF0x74_SerrOnNonFatalErrEn_MASK                        0x2
#define DxF0x74_SerrOnFatalErrEn_OFFSET                         2
#define DxF0x74_SerrOnFatalErrEn_WIDTH                          1
#define DxF0x74_SerrOnFatalErrEn_MASK                           0x4
#define DxF0x74_PmIntEn_OFFSET                                  3
#define DxF0x74_PmIntEn_WIDTH                                   1
#define DxF0x74_PmIntEn_MASK                                    0x8
#define DxF0x74_CrsSoftVisibilityEn_OFFSET                      4
#define DxF0x74_CrsSoftVisibilityEn_WIDTH                       1
#define DxF0x74_CrsSoftVisibilityEn_MASK                        0x10
#define DxF0x74_Reserved_15_5_OFFSET                            5
#define DxF0x74_Reserved_15_5_WIDTH                             11
#define DxF0x74_Reserved_15_5_MASK                              0xffe0
#define DxF0x74_CrsSoftVisibility_OFFSET                        16
#define DxF0x74_CrsSoftVisibility_WIDTH                         1
#define DxF0x74_CrsSoftVisibility_MASK                          0x10000
#define DxF0x74_Reserved_31_17_OFFSET                           17
#define DxF0x74_Reserved_31_17_WIDTH                            15
#define DxF0x74_Reserved_31_17_MASK                             0xfffe0000

/// DxF0x74
typedef union {
  struct {                                                              ///<
    UINT32                                          SerrOnCorrErrEn:1 ; ///<
    UINT32                                      SerrOnNonFatalErrEn:1 ; ///<
    UINT32                                         SerrOnFatalErrEn:1 ; ///<
    UINT32                                                  PmIntEn:1 ; ///<
    UINT32                                      CrsSoftVisibilityEn:1 ; ///<
    UINT32                                            Reserved_15_5:11; ///<
    UINT32                                        CrsSoftVisibility:1 ; ///<
    UINT32                                           Reserved_31_17:15; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x74_STRUCT;

// **** DxF0x78 Register Definition ****
// Address
#define DxF0x78_ADDRESS                                         0x78

// Type
#define DxF0x78_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x78_PmeRequestorId_OFFSET                           0
#define DxF0x78_PmeRequestorId_WIDTH                            16
#define DxF0x78_PmeRequestorId_MASK                             0xffff
#define DxF0x78_PmeStatus_OFFSET                                16
#define DxF0x78_PmeStatus_WIDTH                                 1
#define DxF0x78_PmeStatus_MASK                                  0x10000
#define DxF0x78_PmePending_OFFSET                               17
#define DxF0x78_PmePending_WIDTH                                1
#define DxF0x78_PmePending_MASK                                 0x20000
#define DxF0x78_Reserved_31_18_OFFSET                           18
#define DxF0x78_Reserved_31_18_WIDTH                            14
#define DxF0x78_Reserved_31_18_MASK                             0xfffc0000

/// DxF0x78
typedef union {
  struct {                                                              ///<
    UINT32                                           PmeRequestorId:16; ///<
    UINT32                                                PmeStatus:1 ; ///<
    UINT32                                               PmePending:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x78_STRUCT;

// **** DxF0x7C Register Definition ****
// Address
#define DxF0x7C_ADDRESS                                         0x7c

// Type
#define DxF0x7C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x7C_CplTimeoutRangeSup_OFFSET                       0
#define DxF0x7C_CplTimeoutRangeSup_WIDTH                        4
#define DxF0x7C_CplTimeoutRangeSup_MASK                         0xf
#define DxF0x7C_CplTimeoutDisSup_OFFSET                         4
#define DxF0x7C_CplTimeoutDisSup_WIDTH                          1
#define DxF0x7C_CplTimeoutDisSup_MASK                           0x10
#define DxF0x7C_AriForwardingSupported_OFFSET                   5
#define DxF0x7C_AriForwardingSupported_WIDTH                    1
#define DxF0x7C_AriForwardingSupported_MASK                     0x20
#define DxF0x7C_Reserved_31_6_OFFSET                            6
#define DxF0x7C_Reserved_31_6_WIDTH                             26
#define DxF0x7C_Reserved_31_6_MASK                              0xffffffc0

/// DxF0x7C
typedef union {
  struct {                                                              ///<
    UINT32                                       CplTimeoutRangeSup:4 ; ///<
    UINT32                                         CplTimeoutDisSup:1 ; ///<
    UINT32                                   AriForwardingSupported:1 ; ///<
    UINT32                                            Reserved_31_6:26; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x7C_STRUCT;

// **** DxF0x80 Register Definition ****
// Address
#define DxF0x80_ADDRESS                                         0x80

// Type
#define DxF0x80_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x80_CplTimeoutValue_OFFSET                          0
#define DxF0x80_CplTimeoutValue_WIDTH                           4
#define DxF0x80_CplTimeoutValue_MASK                            0xf
#define DxF0x80_CplTimeoutDis_OFFSET                            4
#define DxF0x80_CplTimeoutDis_WIDTH                             1
#define DxF0x80_CplTimeoutDis_MASK                              0x10
#define DxF0x80_AriForwardingEn_OFFSET                          5
#define DxF0x80_AriForwardingEn_WIDTH                           1
#define DxF0x80_AriForwardingEn_MASK                            0x20
#define DxF0x80_Reserved_31_6_OFFSET                            6
#define DxF0x80_Reserved_31_6_WIDTH                             26
#define DxF0x80_Reserved_31_6_MASK                              0xffffffc0

/// DxF0x80
typedef union {
  struct {                                                              ///<
    UINT32                                          CplTimeoutValue:4 ; ///<
    UINT32                                            CplTimeoutDis:1 ; ///<
    UINT32                                          AriForwardingEn:1 ; ///<
    UINT32                                            Reserved_31_6:26; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x80_STRUCT;

// **** DxF0x84 Register Definition ****
// Address
#define DxF0x84_ADDRESS                                         0x84

// Type
#define DxF0x84_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x84_Reserved_31_0_OFFSET                            0
#define DxF0x84_Reserved_31_0_WIDTH                             32
#define DxF0x84_Reserved_31_0_MASK                              0xffffffff

/// DxF0x84
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x84_STRUCT;

// **** DxF0x88 Register Definition ****
// Address
#define DxF0x88_ADDRESS                                         0x88

// Type
#define DxF0x88_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x88_TargetLinkSpeed_OFFSET                          0
#define DxF0x88_TargetLinkSpeed_WIDTH                           4
#define DxF0x88_TargetLinkSpeed_MASK                            0xf
#define DxF0x88_EnterCompliance_OFFSET                          4
#define DxF0x88_EnterCompliance_WIDTH                           1
#define DxF0x88_EnterCompliance_MASK                            0x10
#define DxF0x88_HwAutonomousSpeedDisable_OFFSET                 5
#define DxF0x88_HwAutonomousSpeedDisable_WIDTH                  1
#define DxF0x88_HwAutonomousSpeedDisable_MASK                   0x20
#define DxF0x88_SelectableDeemphasis_OFFSET                     6
#define DxF0x88_SelectableDeemphasis_WIDTH                      1
#define DxF0x88_SelectableDeemphasis_MASK                       0x40
#define DxF0x88_XmitMargin_OFFSET                               7
#define DxF0x88_XmitMargin_WIDTH                                3
#define DxF0x88_XmitMargin_MASK                                 0x380
#define DxF0x88_EnterModCompliance_OFFSET                       10
#define DxF0x88_EnterModCompliance_WIDTH                        1
#define DxF0x88_EnterModCompliance_MASK                         0x400
#define DxF0x88_ComplianceSOS_OFFSET                            11
#define DxF0x88_ComplianceSOS_WIDTH                             1
#define DxF0x88_ComplianceSOS_MASK                              0x800
#define DxF0x88_ComplianceDeemphasis_OFFSET                     12
#define DxF0x88_ComplianceDeemphasis_WIDTH                      1
#define DxF0x88_ComplianceDeemphasis_MASK                       0x1000
#define DxF0x88_Reserved_15_13_OFFSET                           13
#define DxF0x88_Reserved_15_13_WIDTH                            3
#define DxF0x88_Reserved_15_13_MASK                             0xe000
#define DxF0x88_CurDeemphasisLevel_OFFSET                       16
#define DxF0x88_CurDeemphasisLevel_WIDTH                        1
#define DxF0x88_CurDeemphasisLevel_MASK                         0x10000
#define DxF0x88_Reserved_31_17_OFFSET                           17
#define DxF0x88_Reserved_31_17_WIDTH                            15
#define DxF0x88_Reserved_31_17_MASK                             0xfffe0000

/// DxF0x88
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
} DxF0x88_STRUCT;

// **** DxF0x8C Register Definition ****
// Address
#define DxF0x8C_ADDRESS                                         0x8c

// Type
#define DxF0x8C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x8C_Reserved_31_0_OFFSET                            0
#define DxF0x8C_Reserved_31_0_WIDTH                             32
#define DxF0x8C_Reserved_31_0_MASK                              0xffffffff

/// DxF0x8C
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x8C_STRUCT;

// **** DxF0x90 Register Definition ****
// Address
#define DxF0x90_ADDRESS                                         0x90

// Type
#define DxF0x90_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x90_Reserved_31_0_OFFSET                            0
#define DxF0x90_Reserved_31_0_WIDTH                             32
#define DxF0x90_Reserved_31_0_MASK                              0xffffffff

/// DxF0x90
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x90_STRUCT;

// **** DxF0x128 Register Definition ****
// Address
#define DxF0x128_ADDRESS                                        0x128

// Type
#define DxF0x128_TYPE                                           TYPE_D4F0
// Field Data
#define DxF0x128_Reserved_15_0_OFFSET                           0
#define DxF0x128_Reserved_15_0_WIDTH                            16
#define DxF0x128_Reserved_15_0_MASK                             0xffff
#define DxF0x128_PortArbTableStatus_OFFSET                      16
#define DxF0x128_PortArbTableStatus_WIDTH                       1
#define DxF0x128_PortArbTableStatus_MASK                        0x10000
#define DxF0x128_VcNegotiationPending_OFFSET                    17
#define DxF0x128_VcNegotiationPending_WIDTH                     1
#define DxF0x128_VcNegotiationPending_MASK                      0x20000
#define DxF0x128_Reserved_31_18_OFFSET                          18
#define DxF0x128_Reserved_31_18_WIDTH                           14
#define DxF0x128_Reserved_31_18_MASK                            0xfffc0000

/// DxF0x128
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_15_0:16; ///<
    UINT32                                       PortArbTableStatus:1 ; ///<
    UINT32                                     VcNegotiationPending:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x128_STRUCT;
















// **** D0F0x64_x00 Register Definition ****
// Address
#define D0F0x64_x00_ADDRESS                                     0x0

// Type
#define D0F0x64_x00_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x00_Reserved_5_0_OFFSET                         0
#define D0F0x64_x00_Reserved_5_0_WIDTH                          6
#define D0F0x64_x00_Reserved_5_0_MASK                           0x3f
#define D0F0x64_x00_NbFchCfgEn_OFFSET                           6
#define D0F0x64_x00_NbFchCfgEn_WIDTH                            1
#define D0F0x64_x00_NbFchCfgEn_MASK                             0x40
#define D0F0x64_x00_HwInitWrLock_OFFSET                         7
#define D0F0x64_x00_HwInitWrLock_WIDTH                          1
#define D0F0x64_x00_HwInitWrLock_MASK                           0x80
#define D0F0x64_x00_Reserved_31_8_OFFSET                        8
#define D0F0x64_x00_Reserved_31_8_WIDTH                         24
#define D0F0x64_x00_Reserved_31_8_MASK                          0xffffff00

/// D0F0x64_x00
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_5_0:6 ; ///<
    UINT32                                               NbFchCfgEn:1 ; ///<
    UINT32                                             HwInitWrLock:1 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x00_STRUCT;

// **** D0F0x64_x0B Register Definition ****
// Address
#define D0F0x64_x0B_ADDRESS                                     0xb

// Type
#define D0F0x64_x0B_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x0B_Reserved_19_0_OFFSET                        0
#define D0F0x64_x0B_Reserved_19_0_WIDTH                         20
#define D0F0x64_x0B_Reserved_19_0_MASK                          0xfffff
#define D0F0x64_x0B_SetPowEn_OFFSET                             20
#define D0F0x64_x0B_SetPowEn_WIDTH                              1
#define D0F0x64_x0B_SetPowEn_MASK                               0x100000
#define D0F0x64_x0B_IocFchSetPowEn_OFFSET                       21
#define D0F0x64_x0B_IocFchSetPowEn_WIDTH                        1
#define D0F0x64_x0B_IocFchSetPowEn_MASK                         0x200000
#define D0F0x64_x0B_Reserved_22_22_OFFSET                       22
#define D0F0x64_x0B_Reserved_22_22_WIDTH                        1
#define D0F0x64_x0B_Reserved_22_22_MASK                         0x400000
#define D0F0x64_x0B_IocFchSetPmeTurnOffEn_OFFSET                23
#define D0F0x64_x0B_IocFchSetPmeTurnOffEn_WIDTH                 1
#define D0F0x64_x0B_IocFchSetPmeTurnOffEn_MASK                  0x800000
#define D0F0x64_x0B_Reserved_31_24_OFFSET                       24
#define D0F0x64_x0B_Reserved_31_24_WIDTH                        8
#define D0F0x64_x0B_Reserved_31_24_MASK                         0xff000000

/// D0F0x64_x0B
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           IocFchSetPowEn:1 ; ///<
    UINT32                                           Reserved_22_22:1 ; ///<
    UINT32                                    IocFchSetPmeTurnOffEn:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x0B_STRUCT;

// **** D0F0x64_x0C Register Definition ****
// Address
#define D0F0x64_x0C_ADDRESS                                     0xc

// Type
#define D0F0x64_x0C_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x0C_Reserved_1_0_OFFSET                         0
#define D0F0x64_x0C_Reserved_1_0_WIDTH                          2
#define D0F0x64_x0C_Reserved_1_0_MASK                           0x3
#define D0F0x64_x0C_Dev2BridgeDis_OFFSET                        2
#define D0F0x64_x0C_Dev2BridgeDis_WIDTH                         1
#define D0F0x64_x0C_Dev2BridgeDis_MASK                          0x4
#define D0F0x64_x0C_Dev3BridgeDis_OFFSET                        3
#define D0F0x64_x0C_Dev3BridgeDis_WIDTH                         1
#define D0F0x64_x0C_Dev3BridgeDis_MASK                          0x8
#define D0F0x64_x0C_Dev4BridgeDis_OFFSET                        4
#define D0F0x64_x0C_Dev4BridgeDis_WIDTH                         1
#define D0F0x64_x0C_Dev4BridgeDis_MASK                          0x10
#define D0F0x64_x0C_Dev5BridgeDis_OFFSET                        5
#define D0F0x64_x0C_Dev5BridgeDis_WIDTH                         1
#define D0F0x64_x0C_Dev5BridgeDis_MASK                          0x20
#define D0F0x64_x0C_Dev6BridgeDis_OFFSET                        6
#define D0F0x64_x0C_Dev6BridgeDis_WIDTH                         1
#define D0F0x64_x0C_Dev6BridgeDis_MASK                          0x40
#define D0F0x64_x0C_Dev7BridgeDis_OFFSET                        7
#define D0F0x64_x0C_Dev7BridgeDis_WIDTH                         1
#define D0F0x64_x0C_Dev7BridgeDis_MASK                          0x80
#define D0F0x64_x0C_Reserved_31_8_OFFSET                        8
#define D0F0x64_x0C_Reserved_31_8_WIDTH                         24
#define D0F0x64_x0C_Reserved_31_8_MASK                          0xffffff00

/// D0F0x64_x0C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                            Dev2BridgeDis:1 ; ///<
    UINT32                                            Dev3BridgeDis:1 ; ///<
    UINT32                                            Dev4BridgeDis:1 ; ///<
    UINT32                                            Dev5BridgeDis:1 ; ///<
    UINT32                                            Dev6BridgeDis:1 ; ///<
    UINT32                                            Dev7BridgeDis:1 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x0C_STRUCT;

// **** D0F0x64_x19 Register Definition ****
// Address
#define D0F0x64_x19_ADDRESS                                     0x19

// Type
#define D0F0x64_x19_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x19_TomEn_OFFSET                                0
#define D0F0x64_x19_TomEn_WIDTH                                 1
#define D0F0x64_x19_TomEn_MASK                                  0x1
#define D0F0x64_x19_Reserved_22_1_OFFSET                        1
#define D0F0x64_x19_Reserved_22_1_WIDTH                         22
#define D0F0x64_x19_Reserved_22_1_MASK                          0x7ffffe
#define D0F0x64_x19_Tom2_31_23__OFFSET                          23
#define D0F0x64_x19_Tom2_31_23__WIDTH                           9
#define D0F0x64_x19_Tom2_31_23__MASK                            0xff800000

/// D0F0x64_x19
typedef union {
  struct {                                                              ///<
    UINT32                                                    TomEn:1 ; ///<
    UINT32                                            Reserved_22_1:22; ///<
    UINT32                                              Tom2_31_23_:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x19_STRUCT;

// **** D0F0x64_x1A Register Definition ****
// Address
#define D0F0x64_x1A_ADDRESS                                     0x1a

// Type
#define D0F0x64_x1A_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x1A_Tom2_39_32__OFFSET                          0
#define D0F0x64_x1A_Tom2_39_32__WIDTH                           8
#define D0F0x64_x1A_Tom2_39_32__MASK                            0xff
#define D0F0x64_x1A_Reserved_31_8_OFFSET                        8
#define D0F0x64_x1A_Reserved_31_8_WIDTH                         24
#define D0F0x64_x1A_Reserved_31_8_MASK                          0xffffff00

/// D0F0x64_x1A
typedef union {
  struct {                                                              ///<
    UINT32                                              Tom2_39_32_:8 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x1A_STRUCT;

// **** D0F0x64_x1D Register Definition ****
// Address
#define D0F0x64_x1D_ADDRESS                                     0x1d

// Type
#define D0F0x64_x1D_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x1D_IntGfxAsPcieEn_OFFSET                       0
#define D0F0x64_x1D_IntGfxAsPcieEn_WIDTH                        1
#define D0F0x64_x1D_IntGfxAsPcieEn_MASK                         0x1
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
#define D0F0x64_x1D_Reserved_31_4_MASK                          0xfffffff0

/// D0F0x64_x1D
typedef union {
  struct {                                                              ///<
    UINT32                                           IntGfxAsPcieEn:1 ; ///<
    UINT32                                                    VgaEn:1 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                                  Vga16En:1 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x1D_STRUCT;








// **** D0F0x64_x53 Register Definition ****
// Address
#define D0F0x64_x53_ADDRESS                                     0x53

// Type
#define D0F0x64_x53_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x53_Reserved_19_0_OFFSET                        0
#define D0F0x64_x53_Reserved_19_0_WIDTH                         20
#define D0F0x64_x53_Reserved_19_0_MASK                          0xfffff
#define D0F0x64_x53_SetPowEn_OFFSET                             20
#define D0F0x64_x53_SetPowEn_WIDTH                              1
#define D0F0x64_x53_SetPowEn_MASK                               0x100000
#define D0F0x64_x53_Reserved_31_21_OFFSET                       21
#define D0F0x64_x53_Reserved_31_21_WIDTH                        11
#define D0F0x64_x53_Reserved_31_21_MASK                         0xffe00000

/// D0F0x64_x53
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x53_STRUCT;

// **** D0F0x64_x55 Register Definition ****
// Address
#define D0F0x64_x55_ADDRESS                                     0x55

// Type
#define D0F0x64_x55_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x55_Reserved_19_0_OFFSET                        0
#define D0F0x64_x55_Reserved_19_0_WIDTH                         20
#define D0F0x64_x55_Reserved_19_0_MASK                          0xfffff
#define D0F0x64_x55_SetPowEn_OFFSET                             20
#define D0F0x64_x55_SetPowEn_WIDTH                              1
#define D0F0x64_x55_SetPowEn_MASK                               0x100000
#define D0F0x64_x55_Reserved_31_21_OFFSET                       21
#define D0F0x64_x55_Reserved_31_21_WIDTH                        11
#define D0F0x64_x55_Reserved_31_21_MASK                         0xffe00000

/// D0F0x64_x55
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x55_STRUCT;

// **** D0F0x64_x57 Register Definition ****
// Address
#define D0F0x64_x57_ADDRESS                                     0x57

// Type
#define D0F0x64_x57_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x57_Reserved_19_0_OFFSET                        0
#define D0F0x64_x57_Reserved_19_0_WIDTH                         20
#define D0F0x64_x57_Reserved_19_0_MASK                          0xfffff
#define D0F0x64_x57_SetPowEn_OFFSET                             20
#define D0F0x64_x57_SetPowEn_WIDTH                              1
#define D0F0x64_x57_SetPowEn_MASK                               0x100000
#define D0F0x64_x57_Reserved_31_21_OFFSET                       21
#define D0F0x64_x57_Reserved_31_21_WIDTH                        11
#define D0F0x64_x57_Reserved_31_21_MASK                         0xffe00000

/// D0F0x64_x57
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x57_STRUCT;

// **** D0F0x64_x59 Register Definition ****
// Address
#define D0F0x64_x59_ADDRESS                                     0x59

// Type
#define D0F0x64_x59_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x59_Reserved_19_0_OFFSET                        0
#define D0F0x64_x59_Reserved_19_0_WIDTH                         20
#define D0F0x64_x59_Reserved_19_0_MASK                          0xfffff
#define D0F0x64_x59_SetPowEn_OFFSET                             20
#define D0F0x64_x59_SetPowEn_WIDTH                              1
#define D0F0x64_x59_SetPowEn_MASK                               0x100000
#define D0F0x64_x59_Reserved_31_21_OFFSET                       21
#define D0F0x64_x59_Reserved_31_21_WIDTH                        11
#define D0F0x64_x59_Reserved_31_21_MASK                         0xffe00000

/// D0F0x64_x59
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x59_STRUCT;

// **** D0F0x64_x5B Register Definition ****
// Address
#define D0F0x64_x5B_ADDRESS                                     0x5b

// Type
#define D0F0x64_x5B_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x5B_Reserved_19_0_OFFSET                        0
#define D0F0x64_x5B_Reserved_19_0_WIDTH                         20
#define D0F0x64_x5B_Reserved_19_0_MASK                          0xfffff
#define D0F0x64_x5B_SetPowEn_OFFSET                             20
#define D0F0x64_x5B_SetPowEn_WIDTH                              1
#define D0F0x64_x5B_SetPowEn_MASK                               0x100000
#define D0F0x64_x5B_Reserved_31_21_OFFSET                       21
#define D0F0x64_x5B_Reserved_31_21_WIDTH                        11
#define D0F0x64_x5B_Reserved_31_21_MASK                         0xffe00000

/// D0F0x64_x5B
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x5B_STRUCT;

/// D0F0x64_x6A
typedef union {
  struct {                                                              ///<
    UINT32                                           VoltageForceEn:1 ; ///<
    UINT32                                          VoltageChangeEn:1 ; ///<
    UINT32                                         VoltageChangeReq:1 ; ///<
    UINT32                                             VoltageLevel:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex488_STRUCT;

/// D0F0x64_x6B
typedef union {
  struct {                                                              ///<
    UINT32                                         VoltageChangeAck:1 ; ///<
    UINT32                                      CurrentVoltageLevel:2 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex489_STRUCT;

// **** D0F0x98_x06 Register Definition ****
// Address
#define D0F0x98_x06_ADDRESS                                     0x6

// Type
#define D0F0x98_x06_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x06_Reserved_25_0_OFFSET                        0
#define D0F0x98_x06_Reserved_25_0_WIDTH                         26
#define D0F0x98_x06_Reserved_25_0_MASK                          0x3ffffff
#define D0F0x98_x06_UmiNpMemWrEn_OFFSET                         26
#define D0F0x98_x06_UmiNpMemWrEn_WIDTH                          1
#define D0F0x98_x06_UmiNpMemWrEn_MASK                           0x4000000
#define D0F0x98_x06_Reserved_31_27_OFFSET                       27
#define D0F0x98_x06_Reserved_31_27_WIDTH                        5
#define D0F0x98_x06_Reserved_31_27_MASK                         0xf8000000

/// D0F0x98_x06
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_25_0:26; ///<
    UINT32                                             UmiNpMemWrEn:1 ; ///<
    UINT32                                           Reserved_31_27:5 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x06_STRUCT;





// **** D0F0x98_x1E Register Definition ****
// Address
#define D0F0x98_x1E_ADDRESS                                     0x1e

// Type
#define D0F0x98_x1E_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x1E_Reserved_0_0_OFFSET                         0
#define D0F0x98_x1E_Reserved_0_0_WIDTH                          1
#define D0F0x98_x1E_Reserved_0_0_MASK                           0x1
#define D0F0x98_x1E_HiPriEn_OFFSET                              1
#define D0F0x98_x1E_HiPriEn_WIDTH                               1
#define D0F0x98_x1E_HiPriEn_MASK                                0x2
#define D0F0x98_x1E_Reserved_31_2_OFFSET                        2
#define D0F0x98_x1E_Reserved_31_2_WIDTH                         30
#define D0F0x98_x1E_Reserved_31_2_MASK                          0xfffffffc

/// D0F0x98_x1E
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                                  HiPriEn:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x1E_STRUCT;


/// D0F0x98_x2C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32           ex495_1:1;
    UINT32                                            Reserved_15_2:14; ///<
    UINT32           ex495_3:16;
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex495_STRUCT;


// **** D0F0x98_x49 Register Definition ****
// Address
#define D0F0x98_x49_ADDRESS                                     0x49

// Type
#define D0F0x98_x49_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x49_Reserved_3_0_OFFSET                         0
#define D0F0x98_x49_Reserved_3_0_WIDTH                          4
#define D0F0x98_x49_Reserved_3_0_MASK                           0xf
#define D0F0x98_x49_Reserved_23_12_OFFSET                       12
#define D0F0x98_x49_Reserved_23_12_WIDTH                        12
#define D0F0x98_x49_Reserved_23_12_MASK                         0xfff000
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
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                            :8 ; ///<
    UINT32                                           Reserved_23_12:12; ///<
    UINT32                                         SoftOverrideClk6:1 ; ///<
    UINT32                                         SoftOverrideClk5:1 ; ///<
    UINT32                                         SoftOverrideClk4:1 ; ///<
    UINT32                                         SoftOverrideClk3:1 ; ///<
    UINT32                                         SoftOverrideClk2:1 ; ///<
    UINT32                                         SoftOverrideClk1:1 ; ///<
    UINT32                                         SoftOverrideClk0:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x49_STRUCT;

// **** D0F0x98_x4A Register Definition ****
// Address
#define D0F0x98_x4A_ADDRESS                                     0x4a

// Type
#define D0F0x98_x4A_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x4A_Reserved_3_0_OFFSET                         0
#define D0F0x98_x4A_Reserved_3_0_WIDTH                          4
#define D0F0x98_x4A_Reserved_3_0_MASK                           0xf
#define D0F0x98_x4A_Reserved_23_12_OFFSET                       12
#define D0F0x98_x4A_Reserved_23_12_WIDTH                        12
#define D0F0x98_x4A_Reserved_23_12_MASK                         0xfff000
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
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                            :8 ; ///<
    UINT32                                           Reserved_23_12:12; ///<
    UINT32                                         SoftOverrideClk6:1 ; ///<
    UINT32                                         SoftOverrideClk5:1 ; ///<
    UINT32                                         SoftOverrideClk4:1 ; ///<
    UINT32                                         SoftOverrideClk3:1 ; ///<
    UINT32                                         SoftOverrideClk2:1 ; ///<
    UINT32                                         SoftOverrideClk1:1 ; ///<
    UINT32                                         SoftOverrideClk0:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x4A_STRUCT;


// **** D0F0xE4_WRAP_0080 Register Definition ****
// Address
#define D0F0xE4_WRAP_0080_ADDRESS                               0x80

// Type
#define D0F0xE4_WRAP_0080_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_0080_StrapBifLinkConfig_OFFSET             0
#define D0F0xE4_WRAP_0080_StrapBifLinkConfig_WIDTH              4
#define D0F0xE4_WRAP_0080_StrapBifLinkConfig_MASK               0xf
#define D0F0xE4_WRAP_0080_Reserved_31_4_OFFSET                  4
#define D0F0xE4_WRAP_0080_Reserved_31_4_WIDTH                   28
#define D0F0xE4_WRAP_0080_Reserved_31_4_MASK                    0xfffffff0

/// D0F0xE4_WRAP_0080
typedef union {
  struct {                                                              ///<
    UINT32                                       StrapBifLinkConfig:4 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_0080_STRUCT;

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
#define D0F0xE4_WRAP_0800_Reserved_31_1_MASK                    0xfffffffe

/// D0F0xE4_WRAP_0800
typedef union {
  struct {                                                              ///<
    UINT32                                             HoldTraining:1 ; ///<
    UINT32                                            Reserved_31_1:31; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_0800_STRUCT;

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

// **** D0F0xE4_WRAP_0903 Register Definition ****
// Address
#define D0F0xE4_WRAP_0903_ADDRESS                               0x903

// Type
#define D0F0xE4_WRAP_0903_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_0903_Reserved_4_0_OFFSET                   0
#define D0F0xE4_WRAP_0903_Reserved_4_0_WIDTH                    5
#define D0F0xE4_WRAP_0903_Reserved_4_0_MASK                     0x1f
#define D0F0xE4_WRAP_0903_StrapBifDeemphasisSel_OFFSET          5
#define D0F0xE4_WRAP_0903_StrapBifDeemphasisSel_WIDTH           1
#define D0F0xE4_WRAP_0903_StrapBifDeemphasisSel_MASK            0x20
#define D0F0xE4_WRAP_0903_Reserved_31_6_OFFSET                  6
#define D0F0xE4_WRAP_0903_Reserved_31_6_WIDTH                   26
#define D0F0xE4_WRAP_0903_Reserved_31_6_MASK                    0xffffffc0

/// D0F0xE4_WRAP_0903
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                    StrapBifDeemphasisSel:1 ; ///<
    UINT32                                            Reserved_31_6:26; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_0903_STRUCT;


/// D0F0xE4_WRAP_8011
typedef union {
  struct {                                                              ///<
    UINT32                                      TxclkDynGateLatency:6 ; ///<
    UINT32                                        TxclkPermGateEven:1 ; ///<
    UINT32                                       TxclkDynGateEnable:1 ; ///<
    UINT32                                            TxclkPermStop:1 ; ///<
    UINT32                                      TxclkRegsGateEnable:1 ; ///<
    UINT32                                     TxclkRegsGateLatency:6 ; ///<
    UINT32                                         RcvrDetClkEnable:1 ; ///<
    UINT32                                     TxclkPermGateLatency:6 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                      TxclkLcntGateEnable:1 ; ///<
    UINT32                                           Reserved_30_25:6 ; ///<
    UINT32                                            StrapBifValid:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex501_STRUCT;

// **** D0F0xE4_WRAP_8012 Register Definition ****
// Address
#define D0F0xE4_WRAP_8012_ADDRESS                               0x8012

// Type
#define D0F0xE4_WRAP_8012_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8012_Pif1xIdleGateLatency_OFFSET           0
#define D0F0xE4_WRAP_8012_Pif1xIdleGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8012_Pif1xIdleGateLatency_MASK             0x3f
#define D0F0xE4_WRAP_8012_Reserved_6_6_OFFSET                   6
#define D0F0xE4_WRAP_8012_Reserved_6_6_WIDTH                    1
#define D0F0xE4_WRAP_8012_Reserved_6_6_MASK                     0x40
#define D0F0xE4_WRAP_8012_Pif1xIdleGateEnable_OFFSET            7
#define D0F0xE4_WRAP_8012_Pif1xIdleGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8012_Pif1xIdleGateEnable_MASK              0x80
#define D0F0xE4_WRAP_8012_Pif1xIdleResumeLatency_OFFSET         8
#define D0F0xE4_WRAP_8012_Pif1xIdleResumeLatency_WIDTH          6
#define D0F0xE4_WRAP_8012_Pif1xIdleResumeLatency_MASK           0x3f00
#define D0F0xE4_WRAP_8012_Reserved_15_14_OFFSET                 14
#define D0F0xE4_WRAP_8012_Reserved_15_14_WIDTH                  2
#define D0F0xE4_WRAP_8012_Reserved_15_14_MASK                   0xc000
#define D0F0xE4_WRAP_8012_Pif2p5xIdleGateLatency_OFFSET         16
#define D0F0xE4_WRAP_8012_Pif2p5xIdleGateLatency_WIDTH          6
#define D0F0xE4_WRAP_8012_Pif2p5xIdleGateLatency_MASK           0x3f0000
#define D0F0xE4_WRAP_8012_Reserved_22_22_OFFSET                 22
#define D0F0xE4_WRAP_8012_Reserved_22_22_WIDTH                  1
#define D0F0xE4_WRAP_8012_Reserved_22_22_MASK                   0x400000
#define D0F0xE4_WRAP_8012_Pif2p5xIdleGateEnable_OFFSET          23
#define D0F0xE4_WRAP_8012_Pif2p5xIdleGateEnable_WIDTH           1
#define D0F0xE4_WRAP_8012_Pif2p5xIdleGateEnable_MASK            0x800000
#define D0F0xE4_WRAP_8012_Pif2p5xIdleResumeLatency_OFFSET       24
#define D0F0xE4_WRAP_8012_Pif2p5xIdleResumeLatency_WIDTH        6
#define D0F0xE4_WRAP_8012_Pif2p5xIdleResumeLatency_MASK         0x3f000000
#define D0F0xE4_WRAP_8012_Reserved_31_30_OFFSET                 30
#define D0F0xE4_WRAP_8012_Reserved_31_30_WIDTH                  2
#define D0F0xE4_WRAP_8012_Reserved_31_30_MASK                   0xc0000000

/// D0F0xE4_WRAP_8012
typedef union {
  struct {                                                              ///<
    UINT32                                     Pif1xIdleGateLatency:6 ; ///<
    UINT32                                             Reserved_6_6:1 ; ///<
    UINT32                                      Pif1xIdleGateEnable:1 ; ///<
    UINT32                                   Pif1xIdleResumeLatency:6 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                   Pif2p5xIdleGateLatency:6 ; ///<
    UINT32                                           Reserved_22_22:1 ; ///<
    UINT32                                    Pif2p5xIdleGateEnable:1 ; ///<
    UINT32                                 Pif2p5xIdleResumeLatency:6 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8012_STRUCT;


// **** D0F0xE4_WRAP_8021 Register Definition ****
// Address
#define D0F0xE4_WRAP_8021_ADDRESS                               0x8021

// Type
#define D0F0xE4_WRAP_8021_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8021_Lanes10_OFFSET                        0
#define D0F0xE4_WRAP_8021_Lanes10_WIDTH                         4
#define D0F0xE4_WRAP_8021_Lanes10_MASK                          0xf
#define D0F0xE4_WRAP_8021_Lanes32_OFFSET                        4
#define D0F0xE4_WRAP_8021_Lanes32_WIDTH                         4
#define D0F0xE4_WRAP_8021_Lanes32_MASK                          0xf0
#define D0F0xE4_WRAP_8021_Lanes54_OFFSET                        8
#define D0F0xE4_WRAP_8021_Lanes54_WIDTH                         4
#define D0F0xE4_WRAP_8021_Lanes54_MASK                          0xf00
#define D0F0xE4_WRAP_8021_Lanes76_OFFSET                        12
#define D0F0xE4_WRAP_8021_Lanes76_WIDTH                         4
#define D0F0xE4_WRAP_8021_Lanes76_MASK                          0xf000
#define D0F0xE4_WRAP_8021_Lanes98_OFFSET                        16
#define D0F0xE4_WRAP_8021_Lanes98_WIDTH                         4
#define D0F0xE4_WRAP_8021_Lanes98_MASK                          0xf0000
#define D0F0xE4_WRAP_8021_Lanes1110_OFFSET                      20
#define D0F0xE4_WRAP_8021_Lanes1110_WIDTH                       4
#define D0F0xE4_WRAP_8021_Lanes1110_MASK                        0xf00000
#define D0F0xE4_WRAP_8021_Lanes1312_OFFSET                      24
#define D0F0xE4_WRAP_8021_Lanes1312_WIDTH                       4
#define D0F0xE4_WRAP_8021_Lanes1312_MASK                        0xf000000
#define D0F0xE4_WRAP_8021_Lanes1514_OFFSET                      28
#define D0F0xE4_WRAP_8021_Lanes1514_WIDTH                       4
#define D0F0xE4_WRAP_8021_Lanes1514_MASK                        0xf0000000

/// D0F0xE4_WRAP_8021
typedef union {
  struct {                                                              ///<
    UINT32                                                  Lanes10:4 ; ///<
    UINT32                                                  Lanes32:4 ; ///<
    UINT32                                                  Lanes54:4 ; ///<
    UINT32                                                  Lanes76:4 ; ///<
    UINT32                                                  Lanes98:4 ; ///<
    UINT32                                                Lanes1110:4 ; ///<
    UINT32                                                Lanes1312:4 ; ///<
    UINT32                                                Lanes1514:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8021_STRUCT;

// **** D0F0xE4_WRAP_8022 Register Definition ****
// Address
#define D0F0xE4_WRAP_8022_ADDRESS                               0x8022

// Type
#define D0F0xE4_WRAP_8022_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8022_Lanes10_OFFSET                        0
#define D0F0xE4_WRAP_8022_Lanes10_WIDTH                         4
#define D0F0xE4_WRAP_8022_Lanes10_MASK                          0xf
#define D0F0xE4_WRAP_8022_Lanes32_OFFSET                        4
#define D0F0xE4_WRAP_8022_Lanes32_WIDTH                         4
#define D0F0xE4_WRAP_8022_Lanes32_MASK                          0xf0
#define D0F0xE4_WRAP_8022_Lanes54_OFFSET                        8
#define D0F0xE4_WRAP_8022_Lanes54_WIDTH                         4
#define D0F0xE4_WRAP_8022_Lanes54_MASK                          0xf00
#define D0F0xE4_WRAP_8022_Lanes76_OFFSET                        12
#define D0F0xE4_WRAP_8022_Lanes76_WIDTH                         4
#define D0F0xE4_WRAP_8022_Lanes76_MASK                          0xf000
#define D0F0xE4_WRAP_8022_Lanes98_OFFSET                        16
#define D0F0xE4_WRAP_8022_Lanes98_WIDTH                         4
#define D0F0xE4_WRAP_8022_Lanes98_MASK                          0xf0000
#define D0F0xE4_WRAP_8022_Lanes1110_OFFSET                      20
#define D0F0xE4_WRAP_8022_Lanes1110_WIDTH                       4
#define D0F0xE4_WRAP_8022_Lanes1110_MASK                        0xf00000
#define D0F0xE4_WRAP_8022_Lanes1312_OFFSET                      24
#define D0F0xE4_WRAP_8022_Lanes1312_WIDTH                       4
#define D0F0xE4_WRAP_8022_Lanes1312_MASK                        0xf000000
#define D0F0xE4_WRAP_8022_Lanes1514_OFFSET                      28
#define D0F0xE4_WRAP_8022_Lanes1514_WIDTH                       4
#define D0F0xE4_WRAP_8022_Lanes1514_MASK                        0xf0000000

/// D0F0xE4_WRAP_8022
typedef union {
  struct {                                                              ///<
    UINT32                                                  Lanes10:4 ; ///<
    UINT32                                                  Lanes32:4 ; ///<
    UINT32                                                  Lanes54:4 ; ///<
    UINT32                                                  Lanes76:4 ; ///<
    UINT32                                                  Lanes98:4 ; ///<
    UINT32                                                Lanes1110:4 ; ///<
    UINT32                                                Lanes1312:4 ; ///<
    UINT32                                                Lanes1514:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8022_STRUCT;

// **** D0F0xE4_WRAP_8023 Register Definition ****
// Address
#define D0F0xE4_WRAP_8023_ADDRESS                               0x8023

// Type
#define D0F0xE4_WRAP_8023_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8023_LaneEnable_OFFSET                     0
#define D0F0xE4_WRAP_8023_LaneEnable_WIDTH                      16
#define D0F0xE4_WRAP_8023_LaneEnable_MASK                       0xffff
#define D0F0xE4_WRAP_8023_Reserved_31_16_OFFSET                 16
#define D0F0xE4_WRAP_8023_Reserved_31_16_WIDTH                  16
#define D0F0xE4_WRAP_8023_Reserved_31_16_MASK                   0xffff0000

/// D0F0xE4_WRAP_8023
typedef union {
  struct {                                                              ///<
    UINT32                                               LaneEnable:16; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8023_STRUCT;

// **** D0F0xE4_WRAP_8025 Register Definition ****
// Address
#define D0F0xE4_WRAP_8025_ADDRESS                               0x8025

// Type
#define D0F0xE4_WRAP_8025_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8025_LMTxPhyCmd0_OFFSET                    0
#define D0F0xE4_WRAP_8025_LMTxPhyCmd0_WIDTH                     3
#define D0F0xE4_WRAP_8025_LMTxPhyCmd0_MASK                      0x7
#define D0F0xE4_WRAP_8025_LMRxPhyCmd0_OFFSET                    3
#define D0F0xE4_WRAP_8025_LMRxPhyCmd0_WIDTH                     2
#define D0F0xE4_WRAP_8025_LMRxPhyCmd0_MASK                      0x18
#define D0F0xE4_WRAP_8025_LMLinkSpeed0_OFFSET                   5
#define D0F0xE4_WRAP_8025_LMLinkSpeed0_WIDTH                    1
#define D0F0xE4_WRAP_8025_LMLinkSpeed0_MASK                     0x20
#define D0F0xE4_WRAP_8025_Reserved_7_6_OFFSET                   6
#define D0F0xE4_WRAP_8025_Reserved_7_6_WIDTH                    2
#define D0F0xE4_WRAP_8025_Reserved_7_6_MASK                     0xc0
#define D0F0xE4_WRAP_8025_LMTxPhyCmd1_OFFSET                    8
#define D0F0xE4_WRAP_8025_LMTxPhyCmd1_WIDTH                     3
#define D0F0xE4_WRAP_8025_LMTxPhyCmd1_MASK                      0x700
#define D0F0xE4_WRAP_8025_LMRxPhyCmd1_OFFSET                    11
#define D0F0xE4_WRAP_8025_LMRxPhyCmd1_WIDTH                     2
#define D0F0xE4_WRAP_8025_LMRxPhyCmd1_MASK                      0x1800
#define D0F0xE4_WRAP_8025_LMLinkSpeed1_OFFSET                   13
#define D0F0xE4_WRAP_8025_LMLinkSpeed1_WIDTH                    1
#define D0F0xE4_WRAP_8025_LMLinkSpeed1_MASK                     0x2000
#define D0F0xE4_WRAP_8025_Reserved_15_14_OFFSET                 14
#define D0F0xE4_WRAP_8025_Reserved_15_14_WIDTH                  2
#define D0F0xE4_WRAP_8025_Reserved_15_14_MASK                   0xc000
#define D0F0xE4_WRAP_8025_LMTxPhyCmd2_OFFSET                    16
#define D0F0xE4_WRAP_8025_LMTxPhyCmd2_WIDTH                     3
#define D0F0xE4_WRAP_8025_LMTxPhyCmd2_MASK                      0x70000
#define D0F0xE4_WRAP_8025_LMRxPhyCmd2_OFFSET                    19
#define D0F0xE4_WRAP_8025_LMRxPhyCmd2_WIDTH                     2
#define D0F0xE4_WRAP_8025_LMRxPhyCmd2_MASK                      0x180000
#define D0F0xE4_WRAP_8025_LMLinkSpeed2_OFFSET                   21
#define D0F0xE4_WRAP_8025_LMLinkSpeed2_WIDTH                    1
#define D0F0xE4_WRAP_8025_LMLinkSpeed2_MASK                     0x200000
#define D0F0xE4_WRAP_8025_Reserved_23_22_OFFSET                 22
#define D0F0xE4_WRAP_8025_Reserved_23_22_WIDTH                  2
#define D0F0xE4_WRAP_8025_Reserved_23_22_MASK                   0xc00000
#define D0F0xE4_WRAP_8025_LMTxPhyCmd3_OFFSET                    24
#define D0F0xE4_WRAP_8025_LMTxPhyCmd3_WIDTH                     3
#define D0F0xE4_WRAP_8025_LMTxPhyCmd3_MASK                      0x7000000
#define D0F0xE4_WRAP_8025_LMRxPhyCmd3_OFFSET                    27
#define D0F0xE4_WRAP_8025_LMRxPhyCmd3_WIDTH                     2
#define D0F0xE4_WRAP_8025_LMRxPhyCmd3_MASK                      0x18000000
#define D0F0xE4_WRAP_8025_LMLinkSpeed3_OFFSET                   29
#define D0F0xE4_WRAP_8025_LMLinkSpeed3_WIDTH                    1
#define D0F0xE4_WRAP_8025_LMLinkSpeed3_MASK                     0x20000000
#define D0F0xE4_WRAP_8025_Reserved_31_30_OFFSET                 30
#define D0F0xE4_WRAP_8025_Reserved_31_30_WIDTH                  2
#define D0F0xE4_WRAP_8025_Reserved_31_30_MASK                   0xc0000000

/// D0F0xE4_WRAP_8025
typedef union {
  struct {                                                              ///<
    UINT32                                              LMTxPhyCmd0:3 ; ///<
    UINT32                                              LMRxPhyCmd0:2 ; ///<
    UINT32                                             LMLinkSpeed0:1 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                              LMTxPhyCmd1:3 ; ///<
    UINT32                                              LMRxPhyCmd1:2 ; ///<
    UINT32                                             LMLinkSpeed1:1 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                              LMTxPhyCmd2:3 ; ///<
    UINT32                                              LMRxPhyCmd2:2 ; ///<
    UINT32                                             LMLinkSpeed2:1 ; ///<
    UINT32                                           Reserved_23_22:2 ; ///<
    UINT32                                              LMTxPhyCmd3:3 ; ///<
    UINT32                                              LMRxPhyCmd3:2 ; ///<
    UINT32                                             LMLinkSpeed3:1 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8025_STRUCT;

// **** D0F0xE4_WRAP_8031 Register Definition ****
// Address
#define D0F0xE4_WRAP_8031_ADDRESS                               0x8031

// Type
#define D0F0xE4_WRAP_8031_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8031_LnCntBandwidth_OFFSET                 0
#define D0F0xE4_WRAP_8031_LnCntBandwidth_WIDTH                  10
#define D0F0xE4_WRAP_8031_LnCntBandwidth_MASK                   0x3ff
#define D0F0xE4_WRAP_8031_Reserved_15_10_OFFSET                 10
#define D0F0xE4_WRAP_8031_Reserved_15_10_WIDTH                  6
#define D0F0xE4_WRAP_8031_Reserved_15_10_MASK                   0xfc00
#define D0F0xE4_WRAP_8031_LnCntValid_OFFSET                     16
#define D0F0xE4_WRAP_8031_LnCntValid_WIDTH                      1
#define D0F0xE4_WRAP_8031_LnCntValid_MASK                       0x10000
#define D0F0xE4_WRAP_8031_Reserved_31_17_OFFSET                 17
#define D0F0xE4_WRAP_8031_Reserved_31_17_WIDTH                  15
#define D0F0xE4_WRAP_8031_Reserved_31_17_MASK                   0xfffe0000

/// D0F0xE4_WRAP_8031
typedef union {
  struct {                                                              ///<
    UINT32                                           LnCntBandwidth:10; ///<
    UINT32                                           Reserved_15_10:6 ; ///<
    UINT32                                               LnCntValid:1 ; ///<
    UINT32                                           Reserved_31_17:15; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8031_STRUCT;

/// D0F0xE4_WRAP_8040
typedef union {
  struct {                                                              ///<
    UINT32                                                  OwnPhyA:1 ; ///<
    UINT32                                                  OwnPhyB:1 ; ///<
    UINT32                                                  OwnPhyC:1 ; ///<
    UINT32                                                  OwnPhyD:1 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                           DigaPwrdnValue:3 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                           DigbPwrdnValue:3 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                                  CntPhyA:1 ; ///<
    UINT32                                                  CntPhyB:1 ; ///<
    UINT32                                                  CntPhyC:1 ; ///<
    UINT32                                                  CntPhyD:1 ; ///<
    UINT32                                                  CntDigA:1 ; ///<
    UINT32                                                  CntDigB:1 ; ///<
    UINT32                                              ChangeLnSpd:1 ; ///<
    UINT32                                           Reserved_31_23:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex502_STRUCT;

// **** D0F0xE4_WRAP_8060 Register Definition ****
// Address
#define D0F0xE4_WRAP_8060_ADDRESS                               0x8060

// Type
#define D0F0xE4_WRAP_8060_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8060_Reconfigure_OFFSET                    0
#define D0F0xE4_WRAP_8060_Reconfigure_WIDTH                     1
#define D0F0xE4_WRAP_8060_Reconfigure_MASK                      0x1
#define D0F0xE4_WRAP_8060_Reserved_1_1_OFFSET                   1
#define D0F0xE4_WRAP_8060_Reserved_1_1_WIDTH                    1
#define D0F0xE4_WRAP_8060_Reserved_1_1_MASK                     0x2
#define D0F0xE4_WRAP_8060_ResetComplete_OFFSET                  2
#define D0F0xE4_WRAP_8060_ResetComplete_WIDTH                   1
#define D0F0xE4_WRAP_8060_ResetComplete_MASK                    0x4
#define D0F0xE4_WRAP_8060_Reserved_15_3_OFFSET                  3
#define D0F0xE4_WRAP_8060_Reserved_15_3_WIDTH                   13
#define D0F0xE4_WRAP_8060_Reserved_15_3_MASK                    0xfff8
#define D0F0xE4_WRAP_8060_Reserved_31_18_OFFSET                 18
#define D0F0xE4_WRAP_8060_Reserved_31_18_WIDTH                  14
#define D0F0xE4_WRAP_8060_Reserved_31_18_MASK                   0xfffc0000

/// D0F0xE4_WRAP_8060
typedef union {
  struct {                                                              ///<
    UINT32                                              Reconfigure:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                            ResetComplete:1 ; ///<
    UINT32                                            Reserved_15_3:13; ///<
    UINT32                                           :1 ; ///<
    UINT32                                      :1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8060_STRUCT;

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

// **** D0F0xE4_WRAP_80F0 Register Definition ****
// Address
#define D0F0xE4_WRAP_80F0_ADDRESS                               0x80f0

// Type
#define D0F0xE4_WRAP_80F0_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_80F0_MicroSeconds_OFFSET                   0
#define D0F0xE4_WRAP_80F0_MicroSeconds_WIDTH                    32
#define D0F0xE4_WRAP_80F0_MicroSeconds_MASK                     0xffffffff

/// D0F0xE4_WRAP_80F0
typedef union {
  struct {                                                              ///<
    UINT32                                             MicroSeconds:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_80F0_STRUCT;

// **** D0F0xE4_WRAP_80F1 Register Definition ****
// Address
#define D0F0xE4_WRAP_80F1_ADDRESS                               0x80f1

// Type
#define D0F0xE4_WRAP_80F1_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_80F1_ClockRate_OFFSET                      0
#define D0F0xE4_WRAP_80F1_ClockRate_WIDTH                       8
#define D0F0xE4_WRAP_80F1_ClockRate_MASK                        0xff
#define D0F0xE4_WRAP_80F1_Reserved_31_8_OFFSET                  8
#define D0F0xE4_WRAP_80F1_Reserved_31_8_WIDTH                   24
#define D0F0xE4_WRAP_80F1_Reserved_31_8_MASK                    0xffffff00

/// D0F0xE4_WRAP_80F1
typedef union {
  struct {                                                              ///<
    UINT32                                                ClockRate:8 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_80F1_STRUCT;

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
    UINT32                                           :3 ; ///<
    UINT32                                           Reserved_31_23:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PIF_0010_STRUCT;

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
#define D0F0xE4_PIF_0011_Reserved_7_4_OFFSET                    4
#define D0F0xE4_PIF_0011_Reserved_7_4_WIDTH                     4
#define D0F0xE4_PIF_0011_Reserved_7_4_MASK                      0xf0
#define D0F0xE4_PIF_0011_X4Lane30_OFFSET                        8
#define D0F0xE4_PIF_0011_X4Lane30_WIDTH                         1
#define D0F0xE4_PIF_0011_X4Lane30_MASK                          0x100
#define D0F0xE4_PIF_0011_X4Lane74_OFFSET                        9
#define D0F0xE4_PIF_0011_X4Lane74_WIDTH                         1
#define D0F0xE4_PIF_0011_X4Lane74_MASK                          0x200
#define D0F0xE4_PIF_0011_Reserved_11_10_OFFSET                  10
#define D0F0xE4_PIF_0011_Reserved_11_10_WIDTH                   2
#define D0F0xE4_PIF_0011_Reserved_11_10_MASK                    0xc00
#define D0F0xE4_PIF_0011_X4Lane52_OFFSET                        12
#define D0F0xE4_PIF_0011_X4Lane52_WIDTH                         1
#define D0F0xE4_PIF_0011_X4Lane52_MASK                          0x1000
#define D0F0xE4_PIF_0011_Reserved_15_13_OFFSET                  13
#define D0F0xE4_PIF_0011_Reserved_15_13_WIDTH                   3
#define D0F0xE4_PIF_0011_Reserved_15_13_MASK                    0xe000
#define D0F0xE4_PIF_0011_X8Lane70_OFFSET                        16
#define D0F0xE4_PIF_0011_X8Lane70_WIDTH                         1
#define D0F0xE4_PIF_0011_X8Lane70_MASK                          0x10000
#define D0F0xE4_PIF_0011_Reserved_24_17_OFFSET                  17
#define D0F0xE4_PIF_0011_Reserved_24_17_WIDTH                   8
#define D0F0xE4_PIF_0011_Reserved_24_17_MASK                    0x1fe0000
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
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                                 X4Lane30:1 ; ///<
    UINT32                                                 X4Lane74:1 ; ///<
    UINT32                                           Reserved_11_10:2 ; ///<
    UINT32                                                 X4Lane52:1 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                                 X8Lane70:1 ; ///<
    UINT32                                           Reserved_24_17:8 ; ///<
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

// **** D0F0xE4_PIF_0013 Register Definition ****
// Address
#define D0F0xE4_PIF_0013_ADDRESS                                0x13

// Type
#define D0F0xE4_PIF_0013_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PIF_0013_TxPowerStateInTxs2_OFFSET              0
#define D0F0xE4_PIF_0013_TxPowerStateInTxs2_WIDTH               3
#define D0F0xE4_PIF_0013_TxPowerStateInTxs2_MASK                0x7
#define D0F0xE4_PIF_0013_ForceRxEnInL0s_OFFSET                  3
#define D0F0xE4_PIF_0013_ForceRxEnInL0s_WIDTH                   1
#define D0F0xE4_PIF_0013_ForceRxEnInL0s_MASK                    0x8
#define D0F0xE4_PIF_0013_RxPowerStateInRxs2_OFFSET              4
#define D0F0xE4_PIF_0013_RxPowerStateInRxs2_WIDTH               3
#define D0F0xE4_PIF_0013_RxPowerStateInRxs2_MASK                0x70
#define D0F0xE4_PIF_0013_PllPowerStateInTxs2_OFFSET             7
#define D0F0xE4_PIF_0013_PllPowerStateInTxs2_WIDTH              3
#define D0F0xE4_PIF_0013_PllPowerStateInTxs2_MASK               0x380
#define D0F0xE4_PIF_0013_PllPowerStateInOff_OFFSET              10
#define D0F0xE4_PIF_0013_PllPowerStateInOff_WIDTH               3
#define D0F0xE4_PIF_0013_PllPowerStateInOff_MASK                0x1c00
#define D0F0xE4_PIF_0013_Reserved_15_13_OFFSET                  13
#define D0F0xE4_PIF_0013_Reserved_15_13_WIDTH                   3
#define D0F0xE4_PIF_0013_Reserved_15_13_MASK                    0xe000
#define D0F0xE4_PIF_0013_Tx2p5clkClockGatingEn_OFFSET           16
#define D0F0xE4_PIF_0013_Tx2p5clkClockGatingEn_WIDTH            1
#define D0F0xE4_PIF_0013_Tx2p5clkClockGatingEn_MASK             0x10000
#define D0F0xE4_PIF_0013_Reserved_23_17_OFFSET                  17
#define D0F0xE4_PIF_0013_Reserved_23_17_WIDTH                   7
#define D0F0xE4_PIF_0013_Reserved_23_17_MASK                    0xfe0000
#define D0F0xE4_PIF_0013_PllRampUpTime_OFFSET                   24
#define D0F0xE4_PIF_0013_PllRampUpTime_WIDTH                    3
#define D0F0xE4_PIF_0013_PllRampUpTime_MASK                     0x7000000
#define D0F0xE4_PIF_0013_Reserved_27_27_OFFSET                  27
#define D0F0xE4_PIF_0013_Reserved_27_27_WIDTH                   1
#define D0F0xE4_PIF_0013_Reserved_27_27_MASK                    0x8000000
#define D0F0xE4_PIF_0013_PllPwrOverrideEn_OFFSET                28
#define D0F0xE4_PIF_0013_PllPwrOverrideEn_WIDTH                 1
#define D0F0xE4_PIF_0013_PllPwrOverrideEn_MASK                  0x10000000
#define D0F0xE4_PIF_0013_PllPwrOverrideVal_OFFSET               29
#define D0F0xE4_PIF_0013_PllPwrOverrideVal_WIDTH                3
#define D0F0xE4_PIF_0013_PllPwrOverrideVal_MASK                 0xe0000000

/// D0F0xE4_PIF_0013
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
} D0F0xE4_PIF_0013_STRUCT;

// **** D0F0xE4_PIF_0015 Register Definition ****
// Address
#define D0F0xE4_PIF_0015_ADDRESS                                0x15

// Type
#define D0F0xE4_PIF_0015_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PIF_0015_TxPhyStatus00_OFFSET                   0
#define D0F0xE4_PIF_0015_TxPhyStatus00_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus00_MASK                     0x1
#define D0F0xE4_PIF_0015_TxPhyStatus01_OFFSET                   1
#define D0F0xE4_PIF_0015_TxPhyStatus01_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus01_MASK                     0x2
#define D0F0xE4_PIF_0015_TxPhyStatus02_OFFSET                   2
#define D0F0xE4_PIF_0015_TxPhyStatus02_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus02_MASK                     0x4
#define D0F0xE4_PIF_0015_TxPhyStatus03_OFFSET                   3
#define D0F0xE4_PIF_0015_TxPhyStatus03_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus03_MASK                     0x8
#define D0F0xE4_PIF_0015_TxPhyStatus04_OFFSET                   4
#define D0F0xE4_PIF_0015_TxPhyStatus04_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus04_MASK                     0x10
#define D0F0xE4_PIF_0015_TxPhyStatus05_OFFSET                   5
#define D0F0xE4_PIF_0015_TxPhyStatus05_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus05_MASK                     0x20
#define D0F0xE4_PIF_0015_TxPhyStatus06_OFFSET                   6
#define D0F0xE4_PIF_0015_TxPhyStatus06_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus06_MASK                     0x40
#define D0F0xE4_PIF_0015_TxPhyStatus07_OFFSET                   7
#define D0F0xE4_PIF_0015_TxPhyStatus07_WIDTH                    1
#define D0F0xE4_PIF_0015_TxPhyStatus07_MASK                     0x80
#define D0F0xE4_PIF_0015_Reserved_31_8_OFFSET                   8
#define D0F0xE4_PIF_0015_Reserved_31_8_WIDTH                    24
#define D0F0xE4_PIF_0015_Reserved_31_8_MASK                     0xffffff00

/// D0F0xE4_PIF_0015
typedef union {
  struct {                                                              ///<
    UINT32                                            TxPhyStatus00:1 ; ///<
    UINT32                                            TxPhyStatus01:1 ; ///<
    UINT32                                            TxPhyStatus02:1 ; ///<
    UINT32                                            TxPhyStatus03:1 ; ///<
    UINT32                                            TxPhyStatus04:1 ; ///<
    UINT32                                            TxPhyStatus05:1 ; ///<
    UINT32                                            TxPhyStatus06:1 ; ///<
    UINT32                                            TxPhyStatus07:1 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PIF_0015_STRUCT;

// **** D0F0xE4_CORE_0002 Register Definition ****
// Address
#define D0F0xE4_CORE_0002_ADDRESS                               0x2

// Type
#define D0F0xE4_CORE_0002_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_0002_HwDebug_0__OFFSET                     0
#define D0F0xE4_CORE_0002_HwDebug_0__WIDTH                      1
#define D0F0xE4_CORE_0002_HwDebug_0__MASK                       0x1
#define D0F0xE4_CORE_0002_Reserved_31_1_OFFSET                  1
#define D0F0xE4_CORE_0002_Reserved_31_1_WIDTH                   31
#define D0F0xE4_CORE_0002_Reserved_31_1_MASK                    0xfffffffe

/// D0F0xE4_CORE_0002
typedef union {
  struct {                                                              ///<
    UINT32                                               HwDebug_0_:1 ; ///<
    UINT32                                            Reserved_31_1:31; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0002_STRUCT;



// **** D0F0xE4_CORE_001C Register Definition ****
// Address
#define D0F0xE4_CORE_001C_ADDRESS                               0x1c

// Type
#define D0F0xE4_CORE_001C_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_001C_TxArbRoundRobinEn_OFFSET              0
#define D0F0xE4_CORE_001C_TxArbRoundRobinEn_WIDTH               1
#define D0F0xE4_CORE_001C_TxArbRoundRobinEn_MASK                0x1
#define D0F0xE4_CORE_001C_TxArbSlvLimit_OFFSET                  1
#define D0F0xE4_CORE_001C_TxArbSlvLimit_WIDTH                   5
#define D0F0xE4_CORE_001C_TxArbSlvLimit_MASK                    0x3e
#define D0F0xE4_CORE_001C_TxArbMstLimit_OFFSET                  6
#define D0F0xE4_CORE_001C_TxArbMstLimit_WIDTH                   5
#define D0F0xE4_CORE_001C_TxArbMstLimit_MASK                    0x7c0
#define D0F0xE4_CORE_001C_Reserved_31_11_OFFSET                 11
#define D0F0xE4_CORE_001C_Reserved_31_11_WIDTH                  21
#define D0F0xE4_CORE_001C_Reserved_31_11_MASK                   0xfffff800

/// D0F0xE4_CORE_001C
typedef union {
  struct {                                                              ///<
    UINT32                                        TxArbRoundRobinEn:1 ; ///<
    UINT32                                            TxArbSlvLimit:5 ; ///<
    UINT32                                            TxArbMstLimit:5 ; ///<
    UINT32                                           Reserved_31_11:21; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_001C_STRUCT;

// **** D0F0xE4_CORE_0040 Register Definition ****
// Address
#define D0F0xE4_CORE_0040_ADDRESS                               0x40

// Type
#define D0F0xE4_CORE_0040_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_0040_Reserved_13_0_OFFSET                  0
#define D0F0xE4_CORE_0040_Reserved_13_0_WIDTH                   14
#define D0F0xE4_CORE_0040_Reserved_13_0_MASK                    0x3fff
#define D0F0xE4_CORE_0040_PElecIdleMode_OFFSET                  14
#define D0F0xE4_CORE_0040_PElecIdleMode_WIDTH                   2
#define D0F0xE4_CORE_0040_PElecIdleMode_MASK                    0xc000
#define D0F0xE4_CORE_0040_Reserved_31_16_OFFSET                 16
#define D0F0xE4_CORE_0040_Reserved_31_16_WIDTH                  16
#define D0F0xE4_CORE_0040_Reserved_31_16_MASK                   0xffff0000

/// D0F0xE4_CORE_0040
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_13_0:14; ///<
    UINT32                                            PElecIdleMode:2 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0040_STRUCT;

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
#define D0F0xE4_CORE_00B0_Reserved_31_6_OFFSET                  6
#define D0F0xE4_CORE_00B0_Reserved_31_6_WIDTH                   26
#define D0F0xE4_CORE_00B0_Reserved_31_6_MASK                    0xffffffc0

/// D0F0xE4_CORE_00B0
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                             StrapF0MsiEn:1 ; ///<
    UINT32                                             Reserved_4_3:2 ; ///<
    UINT32                                             StrapF0AerEn:1 ; ///<
    UINT32                                            Reserved_31_6:26; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_00B0_STRUCT;


// **** D0F0xE4_CORE_00C1 Register Definition ****
// Address
#define D0F0xE4_CORE_00C1_ADDRESS                               0xc1

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
#define D0F0xE4_CORE_00C1_Reserved_31_2_MASK                    0xfffffffc

/// D0F0xE4_CORE_00C1
typedef union {
  struct {                                                              ///<
    UINT32                             StrapLinkBwNotificationCapEn:1 ; ///<
    UINT32                                      StrapGen2Compliance:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_00C1_STRUCT;

// **** D0F0xE4_PHY_0009 Register Definition ****
// Address
#define D0F0xE4_PHY_0009_ADDRESS                                0x9

// Type
#define D0F0xE4_PHY_0009_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_0009_Reserved_23_0_OFFSET                   0
#define D0F0xE4_PHY_0009_Reserved_23_0_WIDTH                    24
#define D0F0xE4_PHY_0009_Reserved_23_0_MASK                     0xffffff
#define D0F0xE4_PHY_0009_ClkOff_OFFSET                          24
#define D0F0xE4_PHY_0009_ClkOff_WIDTH                           1
#define D0F0xE4_PHY_0009_ClkOff_MASK                            0x1000000
#define D0F0xE4_PHY_0009_DisplayStream_OFFSET                   25
#define D0F0xE4_PHY_0009_DisplayStream_WIDTH                    1
#define D0F0xE4_PHY_0009_DisplayStream_MASK                     0x2000000
#define D0F0xE4_PHY_0009_Reserved_27_26_OFFSET                  26
#define D0F0xE4_PHY_0009_Reserved_27_26_WIDTH                   2
#define D0F0xE4_PHY_0009_Reserved_27_26_MASK                    0xc000000
#define D0F0xE4_PHY_0009_CascadedPllSel_OFFSET                  28
#define D0F0xE4_PHY_0009_CascadedPllSel_WIDTH                   1
#define D0F0xE4_PHY_0009_CascadedPllSel_MASK                    0x10000000
#define D0F0xE4_PHY_0009_Reserved_30_29_OFFSET                  29
#define D0F0xE4_PHY_0009_Reserved_30_29_WIDTH                   2
#define D0F0xE4_PHY_0009_Reserved_30_29_MASK                    0x60000000
#define D0F0xE4_PHY_0009_PCIePllSel_OFFSET                      31
#define D0F0xE4_PHY_0009_PCIePllSel_WIDTH                       1
#define D0F0xE4_PHY_0009_PCIePllSel_MASK                        0x80000000

/// D0F0xE4_PHY_0009
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_23_0:24; ///<
    UINT32                                                   ClkOff:1 ; ///<
    UINT32                                            DisplayStream:1 ; ///<
    UINT32                                           Reserved_27_26:2 ; ///<
    UINT32                                           CascadedPllSel:1 ; ///<
    UINT32                                           Reserved_30_29:2 ; ///<
    UINT32                                               PCIePllSel:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_0009_STRUCT;

// **** D0F0xE4_PHY_000A Register Definition ****
// Address
#define D0F0xE4_PHY_000A_ADDRESS                                0xa

// Type
#define D0F0xE4_PHY_000A_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_000A_Reserved_23_0_OFFSET                   0
#define D0F0xE4_PHY_000A_Reserved_23_0_WIDTH                    24
#define D0F0xE4_PHY_000A_Reserved_23_0_MASK                     0xffffff
#define D0F0xE4_PHY_000A_ClkOff_OFFSET                          24
#define D0F0xE4_PHY_000A_ClkOff_WIDTH                           1
#define D0F0xE4_PHY_000A_ClkOff_MASK                            0x1000000
#define D0F0xE4_PHY_000A_DisplayStream_OFFSET                   25
#define D0F0xE4_PHY_000A_DisplayStream_WIDTH                    1
#define D0F0xE4_PHY_000A_DisplayStream_MASK                     0x2000000
#define D0F0xE4_PHY_000A_Reserved_27_26_OFFSET                  26
#define D0F0xE4_PHY_000A_Reserved_27_26_WIDTH                   2
#define D0F0xE4_PHY_000A_Reserved_27_26_MASK                    0xc000000
#define D0F0xE4_PHY_000A_CascadedPllSel_OFFSET                  28
#define D0F0xE4_PHY_000A_CascadedPllSel_WIDTH                   1
#define D0F0xE4_PHY_000A_CascadedPllSel_MASK                    0x10000000
#define D0F0xE4_PHY_000A_Reserved_30_29_OFFSET                  29
#define D0F0xE4_PHY_000A_Reserved_30_29_WIDTH                   2
#define D0F0xE4_PHY_000A_Reserved_30_29_MASK                    0x60000000
#define D0F0xE4_PHY_000A_PCIePllSel_OFFSET                      31
#define D0F0xE4_PHY_000A_PCIePllSel_WIDTH                       1
#define D0F0xE4_PHY_000A_PCIePllSel_MASK                        0x80000000

/// D0F0xE4_PHY_000A
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_23_0:24; ///<
    UINT32                                                   ClkOff:1 ; ///<
    UINT32                                            DisplayStream:1 ; ///<
    UINT32                                           Reserved_27_26:2 ; ///<
    UINT32                                           CascadedPllSel:1 ; ///<
    UINT32                                           Reserved_30_29:2 ; ///<
    UINT32                                               PCIePllSel:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_000A_STRUCT;

// **** D0F0xE4_PHY_000B Register Definition ****
// Address
#define D0F0xE4_PHY_000B_ADDRESS                                0xb

// Type
#define D0F0xE4_PHY_000B_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_000B_TxPwrSbiEn_OFFSET                      0
#define D0F0xE4_PHY_000B_TxPwrSbiEn_WIDTH                       1
#define D0F0xE4_PHY_000B_TxPwrSbiEn_MASK                        0x1
#define D0F0xE4_PHY_000B_RxPwrSbiEn_OFFSET                      1
#define D0F0xE4_PHY_000B_RxPwrSbiEn_WIDTH                       1
#define D0F0xE4_PHY_000B_RxPwrSbiEn_MASK                        0x2
#define D0F0xE4_PHY_000B_PcieModeSbiEn_OFFSET                   2
#define D0F0xE4_PHY_000B_PcieModeSbiEn_WIDTH                    1
#define D0F0xE4_PHY_000B_PcieModeSbiEn_MASK                     0x4
#define D0F0xE4_PHY_000B_FreqDivSbiEn_OFFSET                    3
#define D0F0xE4_PHY_000B_FreqDivSbiEn_WIDTH                     1
#define D0F0xE4_PHY_000B_FreqDivSbiEn_MASK                      0x8
#define D0F0xE4_PHY_000B_DllLockSbiEn_OFFSET                    4
#define D0F0xE4_PHY_000B_DllLockSbiEn_WIDTH                     1
#define D0F0xE4_PHY_000B_DllLockSbiEn_MASK                      0x10
#define D0F0xE4_PHY_000B_OffsetCancelSbiEn_OFFSET               5
#define D0F0xE4_PHY_000B_OffsetCancelSbiEn_WIDTH                1
#define D0F0xE4_PHY_000B_OffsetCancelSbiEn_MASK                 0x20
#define D0F0xE4_PHY_000B_SkipBitSbiEn_OFFSET                    6
#define D0F0xE4_PHY_000B_SkipBitSbiEn_WIDTH                     1
#define D0F0xE4_PHY_000B_SkipBitSbiEn_MASK                      0x40
#define D0F0xE4_PHY_000B_IncoherentClkSbiEn_OFFSET              7
#define D0F0xE4_PHY_000B_IncoherentClkSbiEn_WIDTH               1
#define D0F0xE4_PHY_000B_IncoherentClkSbiEn_MASK                0x80
#define D0F0xE4_PHY_000B_EiDetSbiEn_OFFSET                      8
#define D0F0xE4_PHY_000B_EiDetSbiEn_WIDTH                       1
#define D0F0xE4_PHY_000B_EiDetSbiEn_MASK                        0x100
#define D0F0xE4_PHY_000B_Reserved_13_9_OFFSET                   9
#define D0F0xE4_PHY_000B_Reserved_13_9_WIDTH                    5
#define D0F0xE4_PHY_000B_Reserved_13_9_MASK                     0x3e00
#define D0F0xE4_PHY_000B_MargPktSbiEn_OFFSET                    14
#define D0F0xE4_PHY_000B_MargPktSbiEn_WIDTH                     1
#define D0F0xE4_PHY_000B_MargPktSbiEn_MASK                      0x4000
#define D0F0xE4_PHY_000B_PllCmpPktSbiEn_OFFSET                  15
#define D0F0xE4_PHY_000B_PllCmpPktSbiEn_WIDTH                   1
#define D0F0xE4_PHY_000B_PllCmpPktSbiEn_MASK                    0x8000
#define D0F0xE4_PHY_000B_Reserved_31_16_OFFSET                  16
#define D0F0xE4_PHY_000B_Reserved_31_16_WIDTH                   16
#define D0F0xE4_PHY_000B_Reserved_31_16_MASK                    0xffff0000

/// D0F0xE4_PHY_000B
typedef union {
  struct {                                                              ///<
    UINT32                                               TxPwrSbiEn:1 ; ///<
    UINT32                                               RxPwrSbiEn:1 ; ///<
    UINT32                                            PcieModeSbiEn:1 ; ///<
    UINT32                                             FreqDivSbiEn:1 ; ///<
    UINT32                                             DllLockSbiEn:1 ; ///<
    UINT32                                        OffsetCancelSbiEn:1 ; ///<
    UINT32                                             SkipBitSbiEn:1 ; ///<
    UINT32                                       IncoherentClkSbiEn:1 ; ///<
    UINT32                                               EiDetSbiEn:1 ; ///<
    UINT32                                            Reserved_13_9:5 ; ///<
    UINT32                                             MargPktSbiEn:1 ; ///<
    UINT32                                           PllCmpPktSbiEn:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_000B_STRUCT;

// **** D0F0xE4_PHY_2000 Register Definition ****
// Address
#define D0F0xE4_PHY_2000_ADDRESS                                0x2000

// Type
#define D0F0xE4_PHY_2000_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_2000_PllPowerDownEn_OFFSET                  0
#define D0F0xE4_PHY_2000_PllPowerDownEn_WIDTH                   3
#define D0F0xE4_PHY_2000_PllPowerDownEn_MASK                    0x7
#define D0F0xE4_PHY_2000_PllAutoPwrDownDis_OFFSET               3
#define D0F0xE4_PHY_2000_PllAutoPwrDownDis_WIDTH                1
#define D0F0xE4_PHY_2000_PllAutoPwrDownDis_MASK                 0x8
#define D0F0xE4_PHY_2000_Reserved_31_4_OFFSET                   4
#define D0F0xE4_PHY_2000_Reserved_31_4_WIDTH                    28
#define D0F0xE4_PHY_2000_Reserved_31_4_MASK                     0xfffffff0

/// D0F0xE4_PHY_2000
typedef union {
  struct {                                                              ///<
    UINT32                                           PllPowerDownEn:3 ; ///<
    UINT32                                        PllAutoPwrDownDis:1 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_2000_STRUCT;


// **** D0F0xE4_PHY_2005 Register Definition ****
// Address
#define D0F0xE4_PHY_2005_ADDRESS                                0x2005

// Type
#define D0F0xE4_PHY_2005_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_2005_PllClkFreq_OFFSET                      0
#define D0F0xE4_PHY_2005_PllClkFreq_WIDTH                       4
#define D0F0xE4_PHY_2005_PllClkFreq_MASK                        0xf
#define D0F0xE4_PHY_2005_Reserved_8_4_OFFSET                    4
#define D0F0xE4_PHY_2005_Reserved_8_4_WIDTH                     5
#define D0F0xE4_PHY_2005_Reserved_8_4_MASK                      0x1f0
#define D0F0xE4_PHY_2005_PllClkFreqExt_OFFSET                   9
#define D0F0xE4_PHY_2005_PllClkFreqExt_WIDTH                    2
#define D0F0xE4_PHY_2005_PllClkFreqExt_MASK                     0x600
#define D0F0xE4_PHY_2005_Reserved_12_11_OFFSET                  11
#define D0F0xE4_PHY_2005_Reserved_12_11_WIDTH                   2
#define D0F0xE4_PHY_2005_Reserved_12_11_MASK                    0x1800
#define D0F0xE4_PHY_2005_PllMode_OFFSET                         13
#define D0F0xE4_PHY_2005_PllMode_WIDTH                          2
#define D0F0xE4_PHY_2005_PllMode_MASK                           0x6000
#define D0F0xE4_PHY_2005_Reserved_31_15_OFFSET                  15
#define D0F0xE4_PHY_2005_Reserved_31_15_WIDTH                   17
#define D0F0xE4_PHY_2005_Reserved_31_15_MASK                    0xffff8000

/// D0F0xE4_PHY_2005
typedef union {
  struct {                                                              ///<
    UINT32                                               PllClkFreq:4 ; ///<
    UINT32                                             Reserved_8_4:5 ; ///<
    UINT32                                            PllClkFreqExt:2 ; ///<
    UINT32                                           Reserved_12_11:2 ; ///<
    UINT32                                                  PllMode:2 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_2005_STRUCT;

// **** D0F0xE4_PHY_2008 Register Definition ****
// Address
#define D0F0xE4_PHY_2008_ADDRESS                                0x2008

// Type
#define D0F0xE4_PHY_2008_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_2008_PllControlUpdate_OFFSET                0
#define D0F0xE4_PHY_2008_PllControlUpdate_WIDTH                 1
#define D0F0xE4_PHY_2008_PllControlUpdate_MASK                  0x1
#define D0F0xE4_PHY_2008_Reserved_22_1_OFFSET                   1
#define D0F0xE4_PHY_2008_Reserved_22_1_WIDTH                    22
#define D0F0xE4_PHY_2008_Reserved_22_1_MASK                     0x7ffffe
#define D0F0xE4_PHY_2008_MeasCycCntVal_2_0__OFFSET              23
#define D0F0xE4_PHY_2008_MeasCycCntVal_2_0__WIDTH               3
#define D0F0xE4_PHY_2008_MeasCycCntVal_2_0__MASK                0x3800000
#define D0F0xE4_PHY_2008_Reserved_28_26_OFFSET                  26
#define D0F0xE4_PHY_2008_Reserved_28_26_WIDTH                   3
#define D0F0xE4_PHY_2008_Reserved_28_26_MASK                    0x1c000000
#define D0F0xE4_PHY_2008_VdDetectEn_OFFSET                      29
#define D0F0xE4_PHY_2008_VdDetectEn_WIDTH                       1
#define D0F0xE4_PHY_2008_VdDetectEn_MASK                        0x20000000
#define D0F0xE4_PHY_2008_Reserved_31_30_OFFSET                  30
#define D0F0xE4_PHY_2008_Reserved_31_30_WIDTH                   2
#define D0F0xE4_PHY_2008_Reserved_31_30_MASK                    0xc0000000

/// D0F0xE4_PHY_2008
typedef union {
  struct {                                                              ///<
    UINT32                                         PllControlUpdate:1 ; ///<
    UINT32                                            Reserved_22_1:22; ///<
    UINT32                                       MeasCycCntVal_2_0_:3 ; ///<
    UINT32                                           Reserved_28_26:3 ; ///<
    UINT32                                               VdDetectEn:1 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_2008_STRUCT;

// **** D0F0xE4_PHY_4001 Register Definition ****
// Address
#define D0F0xE4_PHY_4001_ADDRESS                                0x4001

// Type
#define D0F0xE4_PHY_4001_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_4001_Reserved_14_0_OFFSET                   0
#define D0F0xE4_PHY_4001_Reserved_14_0_WIDTH                    15
#define D0F0xE4_PHY_4001_Reserved_14_0_MASK                     0x7fff
#define D0F0xE4_PHY_4001_ForceDccRecalc_OFFSET                  15
#define D0F0xE4_PHY_4001_ForceDccRecalc_WIDTH                   1
#define D0F0xE4_PHY_4001_ForceDccRecalc_MASK                    0x8000
#define D0F0xE4_PHY_4001_Reserved_31_16_OFFSET                  16
#define D0F0xE4_PHY_4001_Reserved_31_16_WIDTH                   16
#define D0F0xE4_PHY_4001_Reserved_31_16_MASK                    0xffff0000

/// D0F0xE4_PHY_4001
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_14_0:15; ///<
    UINT32                                           ForceDccRecalc:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_4001_STRUCT;

// **** D0F0xE4_PHY_4002 Register Definition ****
// Address
#define D0F0xE4_PHY_4002_ADDRESS                                0x4002

// Type
#define D0F0xE4_PHY_4002_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_4002_Reserved_2_0_OFFSET                    0
#define D0F0xE4_PHY_4002_Reserved_2_0_WIDTH                     3
#define D0F0xE4_PHY_4002_Reserved_2_0_MASK                      0x7
#define D0F0xE4_PHY_4002_SamClkPiOffsetSign_OFFSET              3
#define D0F0xE4_PHY_4002_SamClkPiOffsetSign_WIDTH               1
#define D0F0xE4_PHY_4002_SamClkPiOffsetSign_MASK                0x8
#define D0F0xE4_PHY_4002_SamClkPiOffset_OFFSET                  4
#define D0F0xE4_PHY_4002_SamClkPiOffset_WIDTH                   3
#define D0F0xE4_PHY_4002_SamClkPiOffset_MASK                    0x70
#define D0F0xE4_PHY_4002_SamClkPiOffsetEn_OFFSET                7
#define D0F0xE4_PHY_4002_SamClkPiOffsetEn_WIDTH                 1
#define D0F0xE4_PHY_4002_SamClkPiOffsetEn_MASK                  0x80
#define D0F0xE4_PHY_4002_Reserved_13_8_OFFSET                   8
#define D0F0xE4_PHY_4002_Reserved_13_8_WIDTH                    6
#define D0F0xE4_PHY_4002_Reserved_13_8_MASK                     0x3f00
#define D0F0xE4_PHY_4002_LfcMin_OFFSET                          14
#define D0F0xE4_PHY_4002_LfcMin_WIDTH                           8
#define D0F0xE4_PHY_4002_LfcMin_MASK                            0x3fc000
#define D0F0xE4_PHY_4002_LfcMax_OFFSET                          22
#define D0F0xE4_PHY_4002_LfcMax_WIDTH                           8
#define D0F0xE4_PHY_4002_LfcMax_MASK                            0x3fc00000
#define D0F0xE4_PHY_4002_Reserved_31_30_OFFSET                  30
#define D0F0xE4_PHY_4002_Reserved_31_30_WIDTH                   2
#define D0F0xE4_PHY_4002_Reserved_31_30_MASK                    0xc0000000

/// D0F0xE4_PHY_4002
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                       SamClkPiOffsetSign:1 ; ///<
    UINT32                                           SamClkPiOffset:3 ; ///<
    UINT32                                         SamClkPiOffsetEn:1 ; ///<
    UINT32                                            Reserved_13_8:6 ; ///<
    UINT32                                                   LfcMin:8 ; ///<
    UINT32                                                   LfcMax:8 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_4002_STRUCT;

// **** D0F0xE4_PHY_4005 Register Definition ****
// Address
#define D0F0xE4_PHY_4005_ADDRESS                                0x4005

// Type
#define D0F0xE4_PHY_4005_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_4005_Reserved_8_0_OFFSET                    0
#define D0F0xE4_PHY_4005_Reserved_8_0_WIDTH                     9
#define D0F0xE4_PHY_4005_Reserved_8_0_MASK                      0x1ff
#define D0F0xE4_PHY_4005_JitterInjHold_OFFSET                   9
#define D0F0xE4_PHY_4005_JitterInjHold_WIDTH                    1
#define D0F0xE4_PHY_4005_JitterInjHold_MASK                     0x200
#define D0F0xE4_PHY_4005_JitterInjOffCnt_OFFSET                 10
#define D0F0xE4_PHY_4005_JitterInjOffCnt_WIDTH                  6
#define D0F0xE4_PHY_4005_JitterInjOffCnt_MASK                   0xfc00
#define D0F0xE4_PHY_4005_Reserved_22_16_OFFSET                  16
#define D0F0xE4_PHY_4005_Reserved_22_16_WIDTH                   7
#define D0F0xE4_PHY_4005_Reserved_22_16_MASK                    0x7f0000
#define D0F0xE4_PHY_4005_JitterInjOnCnt_OFFSET                  23
#define D0F0xE4_PHY_4005_JitterInjOnCnt_WIDTH                   6
#define D0F0xE4_PHY_4005_JitterInjOnCnt_MASK                    0x1f800000
#define D0F0xE4_PHY_4005_JitterInjDir_OFFSET                    29
#define D0F0xE4_PHY_4005_JitterInjDir_WIDTH                     1
#define D0F0xE4_PHY_4005_JitterInjDir_MASK                      0x20000000
#define D0F0xE4_PHY_4005_JitterInjEn_OFFSET                     30
#define D0F0xE4_PHY_4005_JitterInjEn_WIDTH                      1
#define D0F0xE4_PHY_4005_JitterInjEn_MASK                       0x40000000
#define D0F0xE4_PHY_4005_Reserved_31_31_OFFSET                  31
#define D0F0xE4_PHY_4005_Reserved_31_31_WIDTH                   1
#define D0F0xE4_PHY_4005_Reserved_31_31_MASK                    0x80000000

/// D0F0xE4_PHY_4005
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                            JitterInjHold:1 ; ///<
    UINT32                                          JitterInjOffCnt:6 ; ///<
    UINT32                                           Reserved_22_16:7 ; ///<
    UINT32                                           JitterInjOnCnt:6 ; ///<
    UINT32                                             JitterInjDir:1 ; ///<
    UINT32                                              JitterInjEn:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_4005_STRUCT;

// **** D0F0xE4_PHY_4006 Register Definition ****
// Address
#define D0F0xE4_PHY_4006_ADDRESS                                0x4006

// Type
#define D0F0xE4_PHY_4006_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_4006_Reserved_4_0_OFFSET                    0
#define D0F0xE4_PHY_4006_Reserved_4_0_WIDTH                     5
#define D0F0xE4_PHY_4006_Reserved_4_0_MASK                      0x1f
#define D0F0xE4_PHY_4006_DfeVoltage_OFFSET                      5
#define D0F0xE4_PHY_4006_DfeVoltage_WIDTH                       2
#define D0F0xE4_PHY_4006_DfeVoltage_MASK                        0x60
#define D0F0xE4_PHY_4006_DfeEn_OFFSET                           7
#define D0F0xE4_PHY_4006_DfeEn_WIDTH                            1
#define D0F0xE4_PHY_4006_DfeEn_MASK                             0x80
#define D0F0xE4_PHY_4006_Reserved_31_8_OFFSET                   8
#define D0F0xE4_PHY_4006_Reserved_31_8_WIDTH                    24
#define D0F0xE4_PHY_4006_Reserved_31_8_MASK                     0xffffff00

/// D0F0xE4_PHY_4006
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                               DfeVoltage:2 ; ///<
    UINT32                                                    DfeEn:1 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_4006_STRUCT;

// **** D0F0xE4_PHY_400A Register Definition ****
// Address
#define D0F0xE4_PHY_400A_ADDRESS                                0x400a

// Type
#define D0F0xE4_PHY_400A_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_400A_EnCoreLoopFirst_OFFSET                 0
#define D0F0xE4_PHY_400A_EnCoreLoopFirst_WIDTH                  1
#define D0F0xE4_PHY_400A_EnCoreLoopFirst_MASK                   0x1
#define D0F0xE4_PHY_400A_Reserved_3_1_OFFSET                    1
#define D0F0xE4_PHY_400A_Reserved_3_1_WIDTH                     3
#define D0F0xE4_PHY_400A_Reserved_3_1_MASK                      0xe
#define D0F0xE4_PHY_400A_LockDetOnLs2Exit_OFFSET                4
#define D0F0xE4_PHY_400A_LockDetOnLs2Exit_WIDTH                 1
#define D0F0xE4_PHY_400A_LockDetOnLs2Exit_MASK                  0x10
#define D0F0xE4_PHY_400A_Reserved_6_5_OFFSET                    5
#define D0F0xE4_PHY_400A_Reserved_6_5_WIDTH                     2
#define D0F0xE4_PHY_400A_Reserved_6_5_MASK                      0x60
#define D0F0xE4_PHY_400A_BiasDisInLs2_OFFSET                    7
#define D0F0xE4_PHY_400A_BiasDisInLs2_WIDTH                     1
#define D0F0xE4_PHY_400A_BiasDisInLs2_MASK                      0x80
#define D0F0xE4_PHY_400A_Reserved_12_8_OFFSET                   8
#define D0F0xE4_PHY_400A_Reserved_12_8_WIDTH                    5
#define D0F0xE4_PHY_400A_Reserved_12_8_MASK                     0x1f00
#define D0F0xE4_PHY_400A_AnalogWaitTime_OFFSET                  13
#define D0F0xE4_PHY_400A_AnalogWaitTime_WIDTH                   2
#define D0F0xE4_PHY_400A_AnalogWaitTime_MASK                    0x6000
#define D0F0xE4_PHY_400A_Reserved_16_15_OFFSET                  15
#define D0F0xE4_PHY_400A_Reserved_16_15_WIDTH                   2
#define D0F0xE4_PHY_400A_Reserved_16_15_MASK                    0x18000
#define D0F0xE4_PHY_400A_DllLockFastModeEn_OFFSET               17
#define D0F0xE4_PHY_400A_DllLockFastModeEn_WIDTH                1
#define D0F0xE4_PHY_400A_DllLockFastModeEn_MASK                 0x20000
#define D0F0xE4_PHY_400A_Reserved_28_18_OFFSET                  18
#define D0F0xE4_PHY_400A_Reserved_28_18_WIDTH                   11
#define D0F0xE4_PHY_400A_Reserved_28_18_MASK                    0x1ffc0000
#define D0F0xE4_PHY_400A_Ls2ExitTime_OFFSET                     29
#define D0F0xE4_PHY_400A_Ls2ExitTime_WIDTH                      3
#define D0F0xE4_PHY_400A_Ls2ExitTime_MASK                       0xe0000000

/// D0F0xE4_PHY_400A
typedef union {
  struct {                                                              ///<
    UINT32                                          EnCoreLoopFirst:1 ; ///<
    UINT32                                             Reserved_3_1:3 ; ///<
    UINT32                                         LockDetOnLs2Exit:1 ; ///<
    UINT32                                             Reserved_6_5:2 ; ///<
    UINT32                                             BiasDisInLs2:1 ; ///<
    UINT32                                            Reserved_12_8:5 ; ///<
    UINT32                                           AnalogWaitTime:2 ; ///<
    UINT32                                           Reserved_16_15:2 ; ///<
    UINT32                                        DllLockFastModeEn:1 ; ///<
    UINT32                                           Reserved_28_18:11; ///<
    UINT32                                              Ls2ExitTime:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_400A_STRUCT;

// **** D0F0xE4_PHY_6005 Register Definition ****
// Address
#define D0F0xE4_PHY_6005_ADDRESS                                0x6005

// Type
#define D0F0xE4_PHY_6005_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_6005_Reserved_28_0_OFFSET                   0
#define D0F0xE4_PHY_6005_Reserved_28_0_WIDTH                    29
#define D0F0xE4_PHY_6005_Reserved_28_0_MASK                     0x1fffffff
#define D0F0xE4_PHY_6005_IsOwnMstr_OFFSET                       29
#define D0F0xE4_PHY_6005_IsOwnMstr_WIDTH                        1
#define D0F0xE4_PHY_6005_IsOwnMstr_MASK                         0x20000000
#define D0F0xE4_PHY_6005_Reserved_30_30_OFFSET                  30
#define D0F0xE4_PHY_6005_Reserved_30_30_WIDTH                   1
#define D0F0xE4_PHY_6005_Reserved_30_30_MASK                    0x40000000
#define D0F0xE4_PHY_6005_GangedModeEn_OFFSET                    31
#define D0F0xE4_PHY_6005_GangedModeEn_WIDTH                     1
#define D0F0xE4_PHY_6005_GangedModeEn_MASK                      0x80000000

/// D0F0xE4_PHY_6005
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_28_0:29; ///<
    UINT32                                                IsOwnMstr:1 ; ///<
    UINT32                                           Reserved_30_30:1 ; ///<
    UINT32                                             GangedModeEn:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_6005_STRUCT;

// **** D18F2x09C_x0000_0000 Register Definition ****
// Address
#define D18F2x09C_x0000_0000_ADDRESS                            0x0

// Type
#define D18F2x09C_x0000_0000_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0000_CkeDrvStren_OFFSET                 0
#define D18F2x09C_x0000_0000_CkeDrvStren_WIDTH                  3
#define D18F2x09C_x0000_0000_CkeDrvStren_MASK                   0x7
#define D18F2x09C_x0000_0000_Reserved_3_3_OFFSET                3
#define D18F2x09C_x0000_0000_Reserved_3_3_WIDTH                 1
#define D18F2x09C_x0000_0000_Reserved_3_3_MASK                  0x8
#define D18F2x09C_x0000_0000_CsOdtDrvStren_OFFSET               4
#define D18F2x09C_x0000_0000_CsOdtDrvStren_WIDTH                3
#define D18F2x09C_x0000_0000_CsOdtDrvStren_MASK                 0x70
#define D18F2x09C_x0000_0000_Reserved_7_7_OFFSET                7
#define D18F2x09C_x0000_0000_Reserved_7_7_WIDTH                 1
#define D18F2x09C_x0000_0000_Reserved_7_7_MASK                  0x80
#define D18F2x09C_x0000_0000_AddrCmdDrvStren_OFFSET             8
#define D18F2x09C_x0000_0000_AddrCmdDrvStren_WIDTH              3
#define D18F2x09C_x0000_0000_AddrCmdDrvStren_MASK               0x700
#define D18F2x09C_x0000_0000_Reserved_11_11_OFFSET              11
#define D18F2x09C_x0000_0000_Reserved_11_11_WIDTH               1
#define D18F2x09C_x0000_0000_Reserved_11_11_MASK                0x800
#define D18F2x09C_x0000_0000_ClkDrvStren_OFFSET                 12
#define D18F2x09C_x0000_0000_ClkDrvStren_WIDTH                  3
#define D18F2x09C_x0000_0000_ClkDrvStren_MASK                   0x7000
#define D18F2x09C_x0000_0000_Reserved_15_15_OFFSET              15
#define D18F2x09C_x0000_0000_Reserved_15_15_WIDTH               1
#define D18F2x09C_x0000_0000_Reserved_15_15_MASK                0x8000
#define D18F2x09C_x0000_0000_DataDrvStren_OFFSET                16
#define D18F2x09C_x0000_0000_DataDrvStren_WIDTH                 3
#define D18F2x09C_x0000_0000_DataDrvStren_MASK                  0x70000
#define D18F2x09C_x0000_0000_Reserved_19_19_OFFSET              19
#define D18F2x09C_x0000_0000_Reserved_19_19_WIDTH               1
#define D18F2x09C_x0000_0000_Reserved_19_19_MASK                0x80000
#define D18F2x09C_x0000_0000_DqsDrvStren_OFFSET                 20
#define D18F2x09C_x0000_0000_DqsDrvStren_WIDTH                  3
#define D18F2x09C_x0000_0000_DqsDrvStren_MASK                   0x700000
#define D18F2x09C_x0000_0000_Reserved_27_23_OFFSET              23
#define D18F2x09C_x0000_0000_Reserved_27_23_WIDTH               5
#define D18F2x09C_x0000_0000_Reserved_27_23_MASK                0xf800000
#define D18F2x09C_x0000_0000_ProcOdt_OFFSET                     28
#define D18F2x09C_x0000_0000_ProcOdt_WIDTH                      3
#define D18F2x09C_x0000_0000_ProcOdt_MASK                       0x70000000
#define D18F2x09C_x0000_0000_Reserved_31_31_OFFSET              31
#define D18F2x09C_x0000_0000_Reserved_31_31_WIDTH               1
#define D18F2x09C_x0000_0000_Reserved_31_31_MASK                0x80000000

/// D18F2x09C_x0000_0000
typedef union {
  struct {                                                              ///<
    UINT32                                              CkeDrvStren:3 ; ///<
    UINT32                                             Reserved_3_3:1 ; ///<
    UINT32                                            CsOdtDrvStren:3 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                          AddrCmdDrvStren:3 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                              ClkDrvStren:3 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                             DataDrvStren:3 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                              DqsDrvStren:3 ; ///<
    UINT32                                           Reserved_27_23:5 ; ///<
    UINT32                                                  ProcOdt:3 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0000_STRUCT;

// **** D18F2x09C_x0000_0001 Register Definition ****
// Address
#define D18F2x09C_x0000_0001_ADDRESS                            0x1

// Type
#define D18F2x09C_x0000_0001_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte0_OFFSET          0
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte0_WIDTH           5
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte0_MASK            0x1f
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte0_OFFSET         5
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte0_WIDTH          3
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte0_MASK           0xe0
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte1_OFFSET          8
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte1_WIDTH           5
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte1_MASK            0x1f00
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte1_OFFSET         13
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte1_WIDTH          3
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte1_MASK           0xe000
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte2_OFFSET          16
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte2_WIDTH           5
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte2_MASK            0x1f0000
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte2_OFFSET         21
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte2_WIDTH          3
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte2_MASK           0xe00000
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte3_OFFSET          24
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte3_WIDTH           5
#define D18F2x09C_x0000_0001_WrDatFineDly_Byte3_MASK            0x1f000000
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte3_OFFSET         29
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte3_WIDTH          3
#define D18F2x09C_x0000_0001_WrDatGrossDly_Byte3_MASK           0xe0000000

/// D18F2x09C_x0000_0001
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDatFineDly_Byte0:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte0:3 ; ///<
    UINT32                                       WrDatFineDly_Byte1:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte1:3 ; ///<
    UINT32                                       WrDatFineDly_Byte2:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte2:3 ; ///<
    UINT32                                       WrDatFineDly_Byte3:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte3:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0001_STRUCT;

// **** D18F2x09C_x0000_0002 Register Definition ****
// Address
#define D18F2x09C_x0000_0002_ADDRESS                            0x2

// Type
#define D18F2x09C_x0000_0002_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte4_OFFSET          0
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte4_WIDTH           5
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte4_MASK            0x1f
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte4_OFFSET         5
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte4_WIDTH          3
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte4_MASK           0xe0
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte5_OFFSET          8
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte5_WIDTH           5
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte5_MASK            0x1f00
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte5_OFFSET         13
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte5_WIDTH          3
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte5_MASK           0xe000
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte6_OFFSET          16
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte6_WIDTH           5
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte6_MASK            0x1f0000
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte6_OFFSET         21
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte6_WIDTH          3
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte6_MASK           0xe00000
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte7_OFFSET          24
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte7_WIDTH           5
#define D18F2x09C_x0000_0002_WrDatFineDly_Byte7_MASK            0x1f000000
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte7_OFFSET         29
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte7_WIDTH          3
#define D18F2x09C_x0000_0002_WrDatGrossDly_Byte7_MASK           0xe0000000

/// D18F2x09C_x0000_0002
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDatFineDly_Byte4:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte4:3 ; ///<
    UINT32                                       WrDatFineDly_Byte5:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte5:3 ; ///<
    UINT32                                       WrDatFineDly_Byte6:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte6:3 ; ///<
    UINT32                                       WrDatFineDly_Byte7:5 ; ///<
    UINT32                                      WrDatGrossDly_Byte7:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0002_STRUCT;

// **** D18F2x09C_x0000_0004 Register Definition ****
// Address
#define D18F2x09C_x0000_0004_ADDRESS                            0x4

// Type
#define D18F2x09C_x0000_0004_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0004_CkeFineDelay_OFFSET                0
#define D18F2x09C_x0000_0004_CkeFineDelay_WIDTH                 5
#define D18F2x09C_x0000_0004_CkeFineDelay_MASK                  0x1f
#define D18F2x09C_x0000_0004_CkeSetup_OFFSET                    5
#define D18F2x09C_x0000_0004_CkeSetup_WIDTH                     1
#define D18F2x09C_x0000_0004_CkeSetup_MASK                      0x20
#define D18F2x09C_x0000_0004_Reserved_7_6_OFFSET                6
#define D18F2x09C_x0000_0004_Reserved_7_6_WIDTH                 2
#define D18F2x09C_x0000_0004_Reserved_7_6_MASK                  0xc0
#define D18F2x09C_x0000_0004_CsOdtFineDelay_OFFSET              8
#define D18F2x09C_x0000_0004_CsOdtFineDelay_WIDTH               5
#define D18F2x09C_x0000_0004_CsOdtFineDelay_MASK                0x1f00
#define D18F2x09C_x0000_0004_CsOdtSetup_OFFSET                  13
#define D18F2x09C_x0000_0004_CsOdtSetup_WIDTH                   1
#define D18F2x09C_x0000_0004_CsOdtSetup_MASK                    0x2000
#define D18F2x09C_x0000_0004_Reserved_15_14_OFFSET              14
#define D18F2x09C_x0000_0004_Reserved_15_14_WIDTH               2
#define D18F2x09C_x0000_0004_Reserved_15_14_MASK                0xc000
#define D18F2x09C_x0000_0004_AddrCmdFineDelay_OFFSET            16
#define D18F2x09C_x0000_0004_AddrCmdFineDelay_WIDTH             5
#define D18F2x09C_x0000_0004_AddrCmdFineDelay_MASK              0x1f0000
#define D18F2x09C_x0000_0004_AddrCmdSetup_OFFSET                21
#define D18F2x09C_x0000_0004_AddrCmdSetup_WIDTH                 1
#define D18F2x09C_x0000_0004_AddrCmdSetup_MASK                  0x200000
#define D18F2x09C_x0000_0004_Reserved_31_22_OFFSET              22
#define D18F2x09C_x0000_0004_Reserved_31_22_WIDTH               10
#define D18F2x09C_x0000_0004_Reserved_31_22_MASK                0xffc00000

/// D18F2x09C_x0000_0004
typedef union {
  struct {                                                              ///<
    UINT32                                             CkeFineDelay:5 ; ///<
    UINT32                                                 CkeSetup:1 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                           CsOdtFineDelay:5 ; ///<
    UINT32                                               CsOdtSetup:1 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                         AddrCmdFineDelay:5 ; ///<
    UINT32                                             AddrCmdSetup:1 ; ///<
    UINT32                                           Reserved_31_22:10; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0004_STRUCT;

// **** D18F2x09C_x0000_0005 Register Definition ****
// Address
#define D18F2x09C_x0000_0005_ADDRESS                            0x5

// Type
#define D18F2x09C_x0000_0005_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0005_Reserved_0_0_OFFSET                0
#define D18F2x09C_x0000_0005_Reserved_0_0_WIDTH                 1
#define D18F2x09C_x0000_0005_Reserved_0_0_MASK                  0x1
#define D18F2x09C_x0000_0005_RdDqsTime_Byte0_OFFSET             1
#define D18F2x09C_x0000_0005_RdDqsTime_Byte0_WIDTH              5
#define D18F2x09C_x0000_0005_RdDqsTime_Byte0_MASK               0x3e
#define D18F2x09C_x0000_0005_Reserved_8_6_OFFSET                6
#define D18F2x09C_x0000_0005_Reserved_8_6_WIDTH                 3
#define D18F2x09C_x0000_0005_Reserved_8_6_MASK                  0x1c0
#define D18F2x09C_x0000_0005_RdDqsTime_Byte1_OFFSET             9
#define D18F2x09C_x0000_0005_RdDqsTime_Byte1_WIDTH              5
#define D18F2x09C_x0000_0005_RdDqsTime_Byte1_MASK               0x3e00
#define D18F2x09C_x0000_0005_Reserved_16_14_OFFSET              14
#define D18F2x09C_x0000_0005_Reserved_16_14_WIDTH               3
#define D18F2x09C_x0000_0005_Reserved_16_14_MASK                0x1c000
#define D18F2x09C_x0000_0005_RdDqsTime_Byte2_OFFSET             17
#define D18F2x09C_x0000_0005_RdDqsTime_Byte2_WIDTH              5
#define D18F2x09C_x0000_0005_RdDqsTime_Byte2_MASK               0x3e0000
#define D18F2x09C_x0000_0005_Reserved_24_22_OFFSET              22
#define D18F2x09C_x0000_0005_Reserved_24_22_WIDTH               3
#define D18F2x09C_x0000_0005_Reserved_24_22_MASK                0x1c00000
#define D18F2x09C_x0000_0005_RdDqsTime_Byte3_OFFSET             25
#define D18F2x09C_x0000_0005_RdDqsTime_Byte3_WIDTH              5
#define D18F2x09C_x0000_0005_RdDqsTime_Byte3_MASK               0x3e000000
#define D18F2x09C_x0000_0005_Reserved_31_30_OFFSET              30
#define D18F2x09C_x0000_0005_Reserved_31_30_WIDTH               2
#define D18F2x09C_x0000_0005_Reserved_31_30_MASK                0xc0000000

/// D18F2x09C_x0000_0005
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                          RdDqsTime_Byte0:5 ; ///<
    UINT32                                             Reserved_8_6:3 ; ///<
    UINT32                                          RdDqsTime_Byte1:5 ; ///<
    UINT32                                           Reserved_16_14:3 ; ///<
    UINT32                                          RdDqsTime_Byte2:5 ; ///<
    UINT32                                           Reserved_24_22:3 ; ///<
    UINT32                                          RdDqsTime_Byte3:5 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0005_STRUCT;

// **** D18F2x09C_x0000_0006 Register Definition ****
// Address
#define D18F2x09C_x0000_0006_ADDRESS                            0x6

// Type
#define D18F2x09C_x0000_0006_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0006_Reserved_0_0_OFFSET                0
#define D18F2x09C_x0000_0006_Reserved_0_0_WIDTH                 1
#define D18F2x09C_x0000_0006_Reserved_0_0_MASK                  0x1
#define D18F2x09C_x0000_0006_RdDqsTime_Byte4_OFFSET             1
#define D18F2x09C_x0000_0006_RdDqsTime_Byte4_WIDTH              5
#define D18F2x09C_x0000_0006_RdDqsTime_Byte4_MASK               0x3e
#define D18F2x09C_x0000_0006_Reserved_8_6_OFFSET                6
#define D18F2x09C_x0000_0006_Reserved_8_6_WIDTH                 3
#define D18F2x09C_x0000_0006_Reserved_8_6_MASK                  0x1c0
#define D18F2x09C_x0000_0006_RdDqsTime_Byte5_OFFSET             9
#define D18F2x09C_x0000_0006_RdDqsTime_Byte5_WIDTH              5
#define D18F2x09C_x0000_0006_RdDqsTime_Byte5_MASK               0x3e00
#define D18F2x09C_x0000_0006_Reserved_16_14_OFFSET              14
#define D18F2x09C_x0000_0006_Reserved_16_14_WIDTH               3
#define D18F2x09C_x0000_0006_Reserved_16_14_MASK                0x1c000
#define D18F2x09C_x0000_0006_RdDqsTime_Byte6_OFFSET             17
#define D18F2x09C_x0000_0006_RdDqsTime_Byte6_WIDTH              5
#define D18F2x09C_x0000_0006_RdDqsTime_Byte6_MASK               0x3e0000
#define D18F2x09C_x0000_0006_Reserved_24_22_OFFSET              22
#define D18F2x09C_x0000_0006_Reserved_24_22_WIDTH               3
#define D18F2x09C_x0000_0006_Reserved_24_22_MASK                0x1c00000
#define D18F2x09C_x0000_0006_RdDqsTime_Byte7_OFFSET             25
#define D18F2x09C_x0000_0006_RdDqsTime_Byte7_WIDTH              5
#define D18F2x09C_x0000_0006_RdDqsTime_Byte7_MASK               0x3e000000
#define D18F2x09C_x0000_0006_Reserved_31_30_OFFSET              30
#define D18F2x09C_x0000_0006_Reserved_31_30_WIDTH               2
#define D18F2x09C_x0000_0006_Reserved_31_30_MASK                0xc0000000

/// D18F2x09C_x0000_0006
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                          RdDqsTime_Byte4:5 ; ///<
    UINT32                                             Reserved_8_6:3 ; ///<
    UINT32                                          RdDqsTime_Byte5:5 ; ///<
    UINT32                                           Reserved_16_14:3 ; ///<
    UINT32                                          RdDqsTime_Byte6:5 ; ///<
    UINT32                                           Reserved_24_22:3 ; ///<
    UINT32                                          RdDqsTime_Byte7:5 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0006_STRUCT;



// **** D18F2x09C_x0000_000D Register Definition ****
// Address
#define D18F2x09C_x0000_000D_ADDRESS                            0xd

// Type
#define D18F2x09C_x0000_000D_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_000D_TxMaxDurDllNoLock_OFFSET           0
#define D18F2x09C_x0000_000D_TxMaxDurDllNoLock_WIDTH            4
#define D18F2x09C_x0000_000D_TxMaxDurDllNoLock_MASK             0xf
#define D18F2x09C_x0000_000D_TxCPUpdPeriod_OFFSET               4
#define D18F2x09C_x0000_000D_TxCPUpdPeriod_WIDTH                3
#define D18F2x09C_x0000_000D_TxCPUpdPeriod_MASK                 0x70
#define D18F2x09C_x0000_000D_Reserved_7_7_OFFSET                7
#define D18F2x09C_x0000_000D_Reserved_7_7_WIDTH                 1
#define D18F2x09C_x0000_000D_Reserved_7_7_MASK                  0x80
#define D18F2x09C_x0000_000D_TxDLLWakeupTime_OFFSET             8
#define D18F2x09C_x0000_000D_TxDLLWakeupTime_WIDTH              2
#define D18F2x09C_x0000_000D_TxDLLWakeupTime_MASK               0x300
#define D18F2x09C_x0000_000D_Reserved_15_10_OFFSET              10
#define D18F2x09C_x0000_000D_Reserved_15_10_WIDTH               6
#define D18F2x09C_x0000_000D_Reserved_15_10_MASK                0xfc00
#define D18F2x09C_x0000_000D_RxMaxDurDllNoLock_OFFSET           16
#define D18F2x09C_x0000_000D_RxMaxDurDllNoLock_WIDTH            4
#define D18F2x09C_x0000_000D_RxMaxDurDllNoLock_MASK             0xf0000
#define D18F2x09C_x0000_000D_RxCPUpdPeriod_OFFSET               20
#define D18F2x09C_x0000_000D_RxCPUpdPeriod_WIDTH                3
#define D18F2x09C_x0000_000D_RxCPUpdPeriod_MASK                 0x700000
#define D18F2x09C_x0000_000D_Reserved_23_23_OFFSET              23
#define D18F2x09C_x0000_000D_Reserved_23_23_WIDTH               1
#define D18F2x09C_x0000_000D_Reserved_23_23_MASK                0x800000
#define D18F2x09C_x0000_000D_RxDLLWakeupTime_OFFSET             24
#define D18F2x09C_x0000_000D_RxDLLWakeupTime_WIDTH              2
#define D18F2x09C_x0000_000D_RxDLLWakeupTime_MASK               0x3000000
#define D18F2x09C_x0000_000D_Reserved_31_26_OFFSET              26
#define D18F2x09C_x0000_000D_Reserved_31_26_WIDTH               6
#define D18F2x09C_x0000_000D_Reserved_31_26_MASK                0xfc000000

/// D18F2x09C_x0000_000D
typedef union {
  struct {                                                              ///<
    UINT32                                        TxMaxDurDllNoLock:4 ; ///<
    UINT32                                            TxCPUpdPeriod:3 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                          TxDLLWakeupTime:2 ; ///<
    UINT32                                           Reserved_15_10:6 ; ///<
    UINT32                                        RxMaxDurDllNoLock:4 ; ///<
    UINT32                                            RxCPUpdPeriod:3 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                          RxDLLWakeupTime:2 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_000D_STRUCT;


// **** D18F2x09C_x0000_0030 Register Definition ****
// Address
#define D18F2x09C_x0000_0030_ADDRESS                            0x30

// Type
#define D18F2x09C_x0000_0030_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0030_WrDqsFineDly_Byte0_OFFSET          0
#define D18F2x09C_x0000_0030_WrDqsFineDly_Byte0_WIDTH           5
#define D18F2x09C_x0000_0030_WrDqsFineDly_Byte0_MASK            0x1f
#define D18F2x09C_x0000_0030_WrDqsGrossDly_Byte0_OFFSET         5
#define D18F2x09C_x0000_0030_WrDqsGrossDly_Byte0_WIDTH          3
#define D18F2x09C_x0000_0030_WrDqsGrossDly_Byte0_MASK           0xe0
#define D18F2x09C_x0000_0030_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0030_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0030_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0030_WrDqsFineDly_Byte1_OFFSET          16
#define D18F2x09C_x0000_0030_WrDqsFineDly_Byte1_WIDTH           5
#define D18F2x09C_x0000_0030_WrDqsFineDly_Byte1_MASK            0x1f0000
#define D18F2x09C_x0000_0030_WrDqsGrossDly_Byte1_OFFSET         21
#define D18F2x09C_x0000_0030_WrDqsGrossDly_Byte1_WIDTH          3
#define D18F2x09C_x0000_0030_WrDqsGrossDly_Byte1_MASK           0xe00000
#define D18F2x09C_x0000_0030_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0030_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0030_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0030
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte0:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte0:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte1:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte1:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0030_STRUCT;

// **** D18F2x09C_x0000_0031 Register Definition ****
// Address
#define D18F2x09C_x0000_0031_ADDRESS                            0x31

// Type
#define D18F2x09C_x0000_0031_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0031_WrDqsFineDly_Byte2_OFFSET          0
#define D18F2x09C_x0000_0031_WrDqsFineDly_Byte2_WIDTH           5
#define D18F2x09C_x0000_0031_WrDqsFineDly_Byte2_MASK            0x1f
#define D18F2x09C_x0000_0031_WrDqsGrossDly_Byte2_OFFSET         5
#define D18F2x09C_x0000_0031_WrDqsGrossDly_Byte2_WIDTH          3
#define D18F2x09C_x0000_0031_WrDqsGrossDly_Byte2_MASK           0xe0
#define D18F2x09C_x0000_0031_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0031_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0031_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0031_WrDqsFineDly_Byte3_OFFSET          16
#define D18F2x09C_x0000_0031_WrDqsFineDly_Byte3_WIDTH           5
#define D18F2x09C_x0000_0031_WrDqsFineDly_Byte3_MASK            0x1f0000
#define D18F2x09C_x0000_0031_WrDqsGrossDly_Byte3_OFFSET         21
#define D18F2x09C_x0000_0031_WrDqsGrossDly_Byte3_WIDTH          3
#define D18F2x09C_x0000_0031_WrDqsGrossDly_Byte3_MASK           0xe00000
#define D18F2x09C_x0000_0031_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0031_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0031_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0031
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte2:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte2:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte3:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte3:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0031_STRUCT;

// **** D18F2x09C_x0000_0033 Register Definition ****
// Address
#define D18F2x09C_x0000_0033_ADDRESS                            0x33

// Type
#define D18F2x09C_x0000_0033_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0033_WrDqsFineDly_Byte0_OFFSET          0
#define D18F2x09C_x0000_0033_WrDqsFineDly_Byte0_WIDTH           5
#define D18F2x09C_x0000_0033_WrDqsFineDly_Byte0_MASK            0x1f
#define D18F2x09C_x0000_0033_WrDqsGrossDly_Byte0_OFFSET         5
#define D18F2x09C_x0000_0033_WrDqsGrossDly_Byte0_WIDTH          3
#define D18F2x09C_x0000_0033_WrDqsGrossDly_Byte0_MASK           0xe0
#define D18F2x09C_x0000_0033_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0033_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0033_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0033_WrDqsFineDly_Byte1_OFFSET          16
#define D18F2x09C_x0000_0033_WrDqsFineDly_Byte1_WIDTH           5
#define D18F2x09C_x0000_0033_WrDqsFineDly_Byte1_MASK            0x1f0000
#define D18F2x09C_x0000_0033_WrDqsGrossDly_Byte1_OFFSET         21
#define D18F2x09C_x0000_0033_WrDqsGrossDly_Byte1_WIDTH          3
#define D18F2x09C_x0000_0033_WrDqsGrossDly_Byte1_MASK           0xe00000
#define D18F2x09C_x0000_0033_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0033_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0033_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0033
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte0:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte0:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte1:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte1:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0033_STRUCT;

// **** D18F2x09C_x0000_0034 Register Definition ****
// Address
#define D18F2x09C_x0000_0034_ADDRESS                            0x34

// Type
#define D18F2x09C_x0000_0034_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0034_WrDqsFineDly_Byte2_OFFSET          0
#define D18F2x09C_x0000_0034_WrDqsFineDly_Byte2_WIDTH           5
#define D18F2x09C_x0000_0034_WrDqsFineDly_Byte2_MASK            0x1f
#define D18F2x09C_x0000_0034_WrDqsGrossDly_Byte2_OFFSET         5
#define D18F2x09C_x0000_0034_WrDqsGrossDly_Byte2_WIDTH          3
#define D18F2x09C_x0000_0034_WrDqsGrossDly_Byte2_MASK           0xe0
#define D18F2x09C_x0000_0034_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0034_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0034_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0034_WrDqsFineDly_Byte3_OFFSET          16
#define D18F2x09C_x0000_0034_WrDqsFineDly_Byte3_WIDTH           5
#define D18F2x09C_x0000_0034_WrDqsFineDly_Byte3_MASK            0x1f0000
#define D18F2x09C_x0000_0034_WrDqsGrossDly_Byte3_OFFSET         21
#define D18F2x09C_x0000_0034_WrDqsGrossDly_Byte3_WIDTH          3
#define D18F2x09C_x0000_0034_WrDqsGrossDly_Byte3_MASK           0xe00000
#define D18F2x09C_x0000_0034_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0034_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0034_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0034
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte2:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte2:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte3:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte3:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0034_STRUCT;

// **** D18F2x09C_x0000_0040 Register Definition ****
// Address
#define D18F2x09C_x0000_0040_ADDRESS                            0x40

// Type
#define D18F2x09C_x0000_0040_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0040_WrDqsFineDly_Byte4_OFFSET          0
#define D18F2x09C_x0000_0040_WrDqsFineDly_Byte4_WIDTH           5
#define D18F2x09C_x0000_0040_WrDqsFineDly_Byte4_MASK            0x1f
#define D18F2x09C_x0000_0040_WrDqsGrossDly_Byte4_OFFSET         5
#define D18F2x09C_x0000_0040_WrDqsGrossDly_Byte4_WIDTH          3
#define D18F2x09C_x0000_0040_WrDqsGrossDly_Byte4_MASK           0xe0
#define D18F2x09C_x0000_0040_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0040_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0040_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0040_WrDqsFineDly_Byte5_OFFSET          16
#define D18F2x09C_x0000_0040_WrDqsFineDly_Byte5_WIDTH           5
#define D18F2x09C_x0000_0040_WrDqsFineDly_Byte5_MASK            0x1f0000
#define D18F2x09C_x0000_0040_WrDqsGrossDly_Byte5_OFFSET         21
#define D18F2x09C_x0000_0040_WrDqsGrossDly_Byte5_WIDTH          3
#define D18F2x09C_x0000_0040_WrDqsGrossDly_Byte5_MASK           0xe00000
#define D18F2x09C_x0000_0040_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0040_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0040_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0040
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte4:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte4:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte5:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte5:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0040_STRUCT;

// **** D18F2x09C_x0000_0041 Register Definition ****
// Address
#define D18F2x09C_x0000_0041_ADDRESS                            0x41

// Type
#define D18F2x09C_x0000_0041_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0041_WrDqsFineDly_Byte6_OFFSET          0
#define D18F2x09C_x0000_0041_WrDqsFineDly_Byte6_WIDTH           5
#define D18F2x09C_x0000_0041_WrDqsFineDly_Byte6_MASK            0x1f
#define D18F2x09C_x0000_0041_WrDqsGrossDly_Byte6_OFFSET         5
#define D18F2x09C_x0000_0041_WrDqsGrossDly_Byte6_WIDTH          3
#define D18F2x09C_x0000_0041_WrDqsGrossDly_Byte6_MASK           0xe0
#define D18F2x09C_x0000_0041_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0041_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0041_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0041_WrDqsFineDly_Byte7_OFFSET          16
#define D18F2x09C_x0000_0041_WrDqsFineDly_Byte7_WIDTH           5
#define D18F2x09C_x0000_0041_WrDqsFineDly_Byte7_MASK            0x1f0000
#define D18F2x09C_x0000_0041_WrDqsGrossDly_Byte7_OFFSET         21
#define D18F2x09C_x0000_0041_WrDqsGrossDly_Byte7_WIDTH          3
#define D18F2x09C_x0000_0041_WrDqsGrossDly_Byte7_MASK           0xe00000
#define D18F2x09C_x0000_0041_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0041_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0041_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0041
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte6:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte6:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte7:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte7:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0041_STRUCT;

// **** D18F2x09C_x0000_0043 Register Definition ****
// Address
#define D18F2x09C_x0000_0043_ADDRESS                            0x43

// Type
#define D18F2x09C_x0000_0043_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0043_WrDqsFineDly_Byte4_OFFSET          0
#define D18F2x09C_x0000_0043_WrDqsFineDly_Byte4_WIDTH           5
#define D18F2x09C_x0000_0043_WrDqsFineDly_Byte4_MASK            0x1f
#define D18F2x09C_x0000_0043_WrDqsGrossDly_Byte4_OFFSET         5
#define D18F2x09C_x0000_0043_WrDqsGrossDly_Byte4_WIDTH          3
#define D18F2x09C_x0000_0043_WrDqsGrossDly_Byte4_MASK           0xe0
#define D18F2x09C_x0000_0043_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0043_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0043_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0043_WrDqsFineDly_Byte5_OFFSET          16
#define D18F2x09C_x0000_0043_WrDqsFineDly_Byte5_WIDTH           5
#define D18F2x09C_x0000_0043_WrDqsFineDly_Byte5_MASK            0x1f0000
#define D18F2x09C_x0000_0043_WrDqsGrossDly_Byte5_OFFSET         21
#define D18F2x09C_x0000_0043_WrDqsGrossDly_Byte5_WIDTH          3
#define D18F2x09C_x0000_0043_WrDqsGrossDly_Byte5_MASK           0xe00000
#define D18F2x09C_x0000_0043_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0043_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0043_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0043
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte4:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte4:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte5:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte5:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0043_STRUCT;

// **** D18F2x09C_x0000_0044 Register Definition ****
// Address
#define D18F2x09C_x0000_0044_ADDRESS                            0x44

// Type
#define D18F2x09C_x0000_0044_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0044_WrDqsFineDly_Byte6_OFFSET          0
#define D18F2x09C_x0000_0044_WrDqsFineDly_Byte6_WIDTH           5
#define D18F2x09C_x0000_0044_WrDqsFineDly_Byte6_MASK            0x1f
#define D18F2x09C_x0000_0044_WrDqsGrossDly_Byte6_OFFSET         5
#define D18F2x09C_x0000_0044_WrDqsGrossDly_Byte6_WIDTH          3
#define D18F2x09C_x0000_0044_WrDqsGrossDly_Byte6_MASK           0xe0
#define D18F2x09C_x0000_0044_Reserved_15_8_OFFSET               8
#define D18F2x09C_x0000_0044_Reserved_15_8_WIDTH                8
#define D18F2x09C_x0000_0044_Reserved_15_8_MASK                 0xff00
#define D18F2x09C_x0000_0044_WrDqsFineDly_Byte7_OFFSET          16
#define D18F2x09C_x0000_0044_WrDqsFineDly_Byte7_WIDTH           5
#define D18F2x09C_x0000_0044_WrDqsFineDly_Byte7_MASK            0x1f0000
#define D18F2x09C_x0000_0044_WrDqsGrossDly_Byte7_OFFSET         21
#define D18F2x09C_x0000_0044_WrDqsGrossDly_Byte7_WIDTH          3
#define D18F2x09C_x0000_0044_WrDqsGrossDly_Byte7_MASK           0xe00000
#define D18F2x09C_x0000_0044_Reserved_31_24_OFFSET              24
#define D18F2x09C_x0000_0044_Reserved_31_24_WIDTH               8
#define D18F2x09C_x0000_0044_Reserved_31_24_MASK                0xff000000

/// D18F2x09C_x0000_0044
typedef union {
  struct {                                                              ///<
    UINT32                                       WrDqsFineDly_Byte6:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte6:3 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                       WrDqsFineDly_Byte7:5 ; ///<
    UINT32                                      WrDqsGrossDly_Byte7:3 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0044_STRUCT;

// **** D18F2x09C_x0000_0050 Register Definition ****
// Address
#define D18F2x09C_x0000_0050_ADDRESS                            0x50

// Type
#define D18F2x09C_x0000_0050_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte0_OFFSET          0
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte0_WIDTH           5
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte0_MASK            0x1f
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte0_OFFSET         5
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte0_WIDTH          2
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte0_MASK           0x60
#define D18F2x09C_x0000_0050_Reserved_7_7_OFFSET                7
#define D18F2x09C_x0000_0050_Reserved_7_7_WIDTH                 1
#define D18F2x09C_x0000_0050_Reserved_7_7_MASK                  0x80
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte1_OFFSET          8
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte1_WIDTH           5
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte1_MASK            0x1f00
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte1_OFFSET         13
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte1_WIDTH          2
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte1_MASK           0x6000
#define D18F2x09C_x0000_0050_Reserved_15_15_OFFSET              15
#define D18F2x09C_x0000_0050_Reserved_15_15_WIDTH               1
#define D18F2x09C_x0000_0050_Reserved_15_15_MASK                0x8000
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte2_OFFSET          16
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte2_WIDTH           5
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte2_MASK            0x1f0000
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte2_OFFSET         21
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte2_WIDTH          2
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte2_MASK           0x600000
#define D18F2x09C_x0000_0050_Reserved_23_23_OFFSET              23
#define D18F2x09C_x0000_0050_Reserved_23_23_WIDTH               1
#define D18F2x09C_x0000_0050_Reserved_23_23_MASK                0x800000
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte3_OFFSET          24
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte3_WIDTH           5
#define D18F2x09C_x0000_0050_PhRecFineDly_Byte3_MASK            0x1f000000
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte3_OFFSET         29
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte3_WIDTH          2
#define D18F2x09C_x0000_0050_PhRecGrossDly_Byte3_MASK           0x60000000
#define D18F2x09C_x0000_0050_Reserved_31_31_OFFSET              31
#define D18F2x09C_x0000_0050_Reserved_31_31_WIDTH               1
#define D18F2x09C_x0000_0050_Reserved_31_31_MASK                0x80000000

/// D18F2x09C_x0000_0050
typedef union {
  struct {                                                              ///<
    UINT32                                       PhRecFineDly_Byte0:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte0:2 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                       PhRecFineDly_Byte1:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte1:2 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                       PhRecFineDly_Byte2:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte2:2 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                       PhRecFineDly_Byte3:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte3:2 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0050_STRUCT;

// **** D18F2x09C_x0000_0051 Register Definition ****
// Address
#define D18F2x09C_x0000_0051_ADDRESS                            0x51

// Type
#define D18F2x09C_x0000_0051_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte4_OFFSET          0
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte4_WIDTH           5
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte4_MASK            0x1f
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte4_OFFSET         5
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte4_WIDTH          2
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte4_MASK           0x60
#define D18F2x09C_x0000_0051_Reserved_7_7_OFFSET                7
#define D18F2x09C_x0000_0051_Reserved_7_7_WIDTH                 1
#define D18F2x09C_x0000_0051_Reserved_7_7_MASK                  0x80
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte5_OFFSET          8
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte5_WIDTH           5
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte5_MASK            0x1f00
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte5_OFFSET         13
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte5_WIDTH          2
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte5_MASK           0x6000
#define D18F2x09C_x0000_0051_Reserved_15_15_OFFSET              15
#define D18F2x09C_x0000_0051_Reserved_15_15_WIDTH               1
#define D18F2x09C_x0000_0051_Reserved_15_15_MASK                0x8000
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte6_OFFSET          16
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte6_WIDTH           5
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte6_MASK            0x1f0000
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte6_OFFSET         21
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte6_WIDTH          2
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte6_MASK           0x600000
#define D18F2x09C_x0000_0051_Reserved_23_23_OFFSET              23
#define D18F2x09C_x0000_0051_Reserved_23_23_WIDTH               1
#define D18F2x09C_x0000_0051_Reserved_23_23_MASK                0x800000
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte7_OFFSET          24
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte7_WIDTH           5
#define D18F2x09C_x0000_0051_PhRecFineDly_Byte7_MASK            0x1f000000
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte7_OFFSET         29
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte7_WIDTH          2
#define D18F2x09C_x0000_0051_PhRecGrossDly_Byte7_MASK           0x60000000
#define D18F2x09C_x0000_0051_Reserved_31_31_OFFSET              31
#define D18F2x09C_x0000_0051_Reserved_31_31_WIDTH               1
#define D18F2x09C_x0000_0051_Reserved_31_31_MASK                0x80000000

/// D18F2x09C_x0000_0051
typedef union {
  struct {                                                              ///<
    UINT32                                       PhRecFineDly_Byte4:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte4:2 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                       PhRecFineDly_Byte5:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte5:2 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                       PhRecFineDly_Byte6:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte6:2 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                       PhRecFineDly_Byte7:5 ; ///<
    UINT32                                      PhRecGrossDly_Byte7:2 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0000_0051_STRUCT;





// **** D18F2x09C_x0D0F_0002 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0002_ADDRESS                            0xd0f0002

// Type
#define D18F2x09C_x0D0F_0002_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0002_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0002_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0002_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0002_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0002_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0002_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0002_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0002_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0002_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0002_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0002_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0002_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0002_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0002_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0002_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0002
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0002_STRUCT;

// **** D18F2x09C_x0D0F_0006 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0006_ADDRESS                            0xd0f0006

// Type
#define D18F2x09C_x0D0F_0006_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0006_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0006_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0006_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0006_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0006_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0006_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0006_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0006_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0006_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0006
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0006_STRUCT;

// **** D18F2x09C_x0D0F_000A Register Definition ****
// Address
#define D18F2x09C_x0D0F_000A_ADDRESS                            0xd0f000a

// Type
#define D18F2x09C_x0D0F_000A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_000A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_000A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_000A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_000A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_000A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_000A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_000A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_000A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_000A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_000A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_000A_STRUCT;

// **** D18F2x09C_x0D0F_000F Register Definition ****
// Address
#define D18F2x09C_x0D0F_000F_ADDRESS                            0xd0f000f

// Type
#define D18F2x09C_x0D0F_000F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_000F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_000F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_000F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_000F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_000F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_000F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_000F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_000F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_000F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_000F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_000F_STRUCT;

// **** D18F2x09C_x0D0F_0010 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0010_ADDRESS                            0xd0f0010

// Type
#define D18F2x09C_x0D0F_0010_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0010_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0010_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0010_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0010_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0010_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0010_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0010_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0010_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0010_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0010
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0010_STRUCT;




// **** D18F2x09C_x0D0F_0102 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0102_ADDRESS                            0xd0f0102

// Type
#define D18F2x09C_x0D0F_0102_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0102_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0102_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0102_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0102_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0102_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0102_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0102_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0102_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0102_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0102_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0102_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0102_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0102_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0102_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0102_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0102
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0102_STRUCT;

// **** D18F2x09C_x0D0F_0106 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0106_ADDRESS                            0xd0f0106

// Type
#define D18F2x09C_x0D0F_0106_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0106_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0106_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0106_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0106_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0106_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0106_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0106_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0106_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0106_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0106
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0106_STRUCT;

// **** D18F2x09C_x0D0F_010A Register Definition ****
// Address
#define D18F2x09C_x0D0F_010A_ADDRESS                            0xd0f010a

// Type
#define D18F2x09C_x0D0F_010A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_010A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_010A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_010A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_010A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_010A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_010A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_010A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_010A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_010A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_010A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_010A_STRUCT;

// **** D18F2x09C_x0D0F_010F Register Definition ****
// Address
#define D18F2x09C_x0D0F_010F_ADDRESS                            0xd0f010f

// Type
#define D18F2x09C_x0D0F_010F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_010F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_010F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_010F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_010F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_010F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_010F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_010F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_010F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_010F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_010F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_010F_STRUCT;

// **** D18F2x09C_x0D0F_0110 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0110_ADDRESS                            0xd0f0110

// Type
#define D18F2x09C_x0D0F_0110_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0110_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0110_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0110_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0110_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0110_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0110_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0110_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0110_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0110_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0110
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0110_STRUCT;

// **** D18F2x09C_x0D0F_011F Register Definition ****
// Address
#define D18F2x09C_x0D0F_011F_ADDRESS                            0xd0f011f

// Type
#define D18F2x09C_x0D0F_011F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_011F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_011F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_011F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_011F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_011F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_011F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_011F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_011F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_011F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_011F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_011F_STRUCT;



// **** D18F2x09C_x0D0F_0202 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0202_ADDRESS                            0xd0f0202

// Type
#define D18F2x09C_x0D0F_0202_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0202_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0202_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0202_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0202_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0202_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0202_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0202_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0202_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0202_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0202_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0202_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0202_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0202_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0202_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0202_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0202
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0202_STRUCT;

// **** D18F2x09C_x0D0F_0206 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0206_ADDRESS                            0xd0f0206

// Type
#define D18F2x09C_x0D0F_0206_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0206_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0206_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0206_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0206_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0206_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0206_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0206_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0206_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0206_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0206
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0206_STRUCT;

// **** D18F2x09C_x0D0F_020A Register Definition ****
// Address
#define D18F2x09C_x0D0F_020A_ADDRESS                            0xd0f020a

// Type
#define D18F2x09C_x0D0F_020A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_020A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_020A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_020A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_020A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_020A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_020A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_020A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_020A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_020A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_020A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_020A_STRUCT;

// **** D18F2x09C_x0D0F_020F Register Definition ****
// Address
#define D18F2x09C_x0D0F_020F_ADDRESS                            0xd0f020f

// Type
#define D18F2x09C_x0D0F_020F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_020F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_020F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_020F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_020F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_020F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_020F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_020F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_020F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_020F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_020F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_020F_STRUCT;

// **** D18F2x09C_x0D0F_0210 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0210_ADDRESS                            0xd0f0210

// Type
#define D18F2x09C_x0D0F_0210_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0210_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0210_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0210_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0210_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0210_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0210_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0210_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0210_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0210_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0210
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0210_STRUCT;

// **** D18F2x09C_x0D0F_021F Register Definition ****
// Address
#define D18F2x09C_x0D0F_021F_ADDRESS                            0xd0f021f

// Type
#define D18F2x09C_x0D0F_021F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_021F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_021F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_021F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_021F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_021F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_021F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_021F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_021F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_021F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_021F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_021F_STRUCT;



// **** D18F2x09C_x0D0F_0302 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0302_ADDRESS                            0xd0f0302

// Type
#define D18F2x09C_x0D0F_0302_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0302_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0302_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0302_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0302_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0302_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0302_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0302_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0302_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0302_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0302_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0302_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0302_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0302_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0302_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0302_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0302
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0302_STRUCT;

// **** D18F2x09C_x0D0F_0306 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0306_ADDRESS                            0xd0f0306

// Type
#define D18F2x09C_x0D0F_0306_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0306_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0306_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0306_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0306_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0306_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0306_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0306_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0306_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0306_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0306
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0306_STRUCT;

// **** D18F2x09C_x0D0F_030A Register Definition ****
// Address
#define D18F2x09C_x0D0F_030A_ADDRESS                            0xd0f030a

// Type
#define D18F2x09C_x0D0F_030A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_030A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_030A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_030A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_030A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_030A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_030A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_030A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_030A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_030A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_030A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_030A_STRUCT;

// **** D18F2x09C_x0D0F_030F Register Definition ****
// Address
#define D18F2x09C_x0D0F_030F_ADDRESS                            0xd0f030f

// Type
#define D18F2x09C_x0D0F_030F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_030F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_030F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_030F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_030F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_030F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_030F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_030F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_030F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_030F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_030F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_030F_STRUCT;

// **** D18F2x09C_x0D0F_0310 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0310_ADDRESS                            0xd0f0310

// Type
#define D18F2x09C_x0D0F_0310_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0310_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0310_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0310_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0310_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0310_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0310_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0310_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0310_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0310_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0310
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0310_STRUCT;

// **** D18F2x09C_x0D0F_031F Register Definition ****
// Address
#define D18F2x09C_x0D0F_031F_ADDRESS                            0xd0f031f

// Type
#define D18F2x09C_x0D0F_031F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_031F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_031F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_031F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_031F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_031F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_031F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_031F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_031F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_031F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_031F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_031F_STRUCT;



// **** D18F2x09C_x0D0F_0402 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0402_ADDRESS                            0xd0f0402

// Type
#define D18F2x09C_x0D0F_0402_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0402_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0402_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0402_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0402_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0402_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0402_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0402_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0402_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0402_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0402_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0402_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0402_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0402_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0402_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0402_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0402
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0402_STRUCT;

// **** D18F2x09C_x0D0F_0406 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0406_ADDRESS                            0xd0f0406

// Type
#define D18F2x09C_x0D0F_0406_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0406_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0406_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0406_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0406_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0406_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0406_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0406_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0406_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0406_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0406
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0406_STRUCT;

// **** D18F2x09C_x0D0F_040A Register Definition ****
// Address
#define D18F2x09C_x0D0F_040A_ADDRESS                            0xd0f040a

// Type
#define D18F2x09C_x0D0F_040A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_040A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_040A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_040A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_040A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_040A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_040A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_040A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_040A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_040A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_040A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_040A_STRUCT;

// **** D18F2x09C_x0D0F_040F Register Definition ****
// Address
#define D18F2x09C_x0D0F_040F_ADDRESS                            0xd0f040f

// Type
#define D18F2x09C_x0D0F_040F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_040F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_040F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_040F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_040F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_040F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_040F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_040F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_040F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_040F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_040F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_040F_STRUCT;

// **** D18F2x09C_x0D0F_0410 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0410_ADDRESS                            0xd0f0410

// Type
#define D18F2x09C_x0D0F_0410_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0410_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0410_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0410_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0410_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0410_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0410_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0410_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0410_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0410_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0410
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0410_STRUCT;

// **** D18F2x09C_x0D0F_041F Register Definition ****
// Address
#define D18F2x09C_x0D0F_041F_ADDRESS                            0xd0f041f

// Type
#define D18F2x09C_x0D0F_041F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_041F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_041F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_041F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_041F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_041F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_041F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_041F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_041F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_041F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_041F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_041F_STRUCT;



// **** D18F2x09C_x0D0F_0502 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0502_ADDRESS                            0xd0f0502

// Type
#define D18F2x09C_x0D0F_0502_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0502_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0502_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0502_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0502_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0502_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0502_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0502_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0502_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0502_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0502_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0502_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0502_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0502_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0502_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0502_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0502
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0502_STRUCT;

// **** D18F2x09C_x0D0F_0506 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0506_ADDRESS                            0xd0f0506

// Type
#define D18F2x09C_x0D0F_0506_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0506_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0506_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0506_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0506_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0506_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0506_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0506_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0506_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0506_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0506
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0506_STRUCT;

// **** D18F2x09C_x0D0F_050A Register Definition ****
// Address
#define D18F2x09C_x0D0F_050A_ADDRESS                            0xd0f050a

// Type
#define D18F2x09C_x0D0F_050A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_050A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_050A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_050A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_050A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_050A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_050A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_050A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_050A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_050A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_050A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_050A_STRUCT;

// **** D18F2x09C_x0D0F_050F Register Definition ****
// Address
#define D18F2x09C_x0D0F_050F_ADDRESS                            0xd0f050f

// Type
#define D18F2x09C_x0D0F_050F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_050F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_050F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_050F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_050F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_050F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_050F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_050F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_050F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_050F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_050F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_050F_STRUCT;

// **** D18F2x09C_x0D0F_0510 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0510_ADDRESS                            0xd0f0510

// Type
#define D18F2x09C_x0D0F_0510_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0510_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0510_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0510_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0510_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0510_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0510_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0510_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0510_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0510_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0510
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0510_STRUCT;

// **** D18F2x09C_x0D0F_051F Register Definition ****
// Address
#define D18F2x09C_x0D0F_051F_ADDRESS                            0xd0f051f

// Type
#define D18F2x09C_x0D0F_051F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_051F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_051F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_051F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_051F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_051F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_051F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_051F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_051F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_051F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_051F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_051F_STRUCT;



// **** D18F2x09C_x0D0F_0602 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0602_ADDRESS                            0xd0f0602

// Type
#define D18F2x09C_x0D0F_0602_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0602_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0602_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0602_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0602_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0602_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0602_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0602_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0602_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0602_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0602_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0602_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0602_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0602_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0602_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0602_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0602
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0602_STRUCT;

// **** D18F2x09C_x0D0F_0606 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0606_ADDRESS                            0xd0f0606

// Type
#define D18F2x09C_x0D0F_0606_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0606_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0606_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0606_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0606_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0606_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0606_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0606_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0606_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0606_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0606
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0606_STRUCT;

// **** D18F2x09C_x0D0F_060A Register Definition ****
// Address
#define D18F2x09C_x0D0F_060A_ADDRESS                            0xd0f060a

// Type
#define D18F2x09C_x0D0F_060A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_060A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_060A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_060A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_060A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_060A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_060A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_060A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_060A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_060A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_060A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_060A_STRUCT;

// **** D18F2x09C_x0D0F_060F Register Definition ****
// Address
#define D18F2x09C_x0D0F_060F_ADDRESS                            0xd0f060f

// Type
#define D18F2x09C_x0D0F_060F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_060F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_060F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_060F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_060F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_060F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_060F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_060F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_060F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_060F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_060F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_060F_STRUCT;

// **** D18F2x09C_x0D0F_0610 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0610_ADDRESS                            0xd0f0610

// Type
#define D18F2x09C_x0D0F_0610_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0610_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0610_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0610_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0610_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0610_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0610_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0610_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0610_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0610_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0610
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0610_STRUCT;

// **** D18F2x09C_x0D0F_061F Register Definition ****
// Address
#define D18F2x09C_x0D0F_061F_ADDRESS                            0xd0f061f

// Type
#define D18F2x09C_x0D0F_061F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_061F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_061F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_061F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_061F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_061F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_061F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_061F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_061F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_061F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_061F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_061F_STRUCT;



// **** D18F2x09C_x0D0F_0702 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0702_ADDRESS                            0xd0f0702

// Type
#define D18F2x09C_x0D0F_0702_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0702_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0702_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0702_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0702_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0702_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0702_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0702_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0702_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0702_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0702_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0702_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0702_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0702_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0702_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0702_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0702
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0702_STRUCT;

// **** D18F2x09C_x0D0F_0706 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0706_ADDRESS                            0xd0f0706

// Type
#define D18F2x09C_x0D0F_0706_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0706_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0706_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0706_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0706_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0706_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0706_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0706_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0706_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0706_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0706
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0706_STRUCT;

// **** D18F2x09C_x0D0F_070A Register Definition ****
// Address
#define D18F2x09C_x0D0F_070A_ADDRESS                            0xd0f070a

// Type
#define D18F2x09C_x0D0F_070A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_070A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_070A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_070A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_070A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_070A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_070A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_070A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_070A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_070A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_070A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_070A_STRUCT;

// **** D18F2x09C_x0D0F_070F Register Definition ****
// Address
#define D18F2x09C_x0D0F_070F_ADDRESS                            0xd0f070f

// Type
#define D18F2x09C_x0D0F_070F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_070F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_070F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_070F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_070F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_070F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_070F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_070F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_070F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_070F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_070F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_070F_STRUCT;

// **** D18F2x09C_x0D0F_0710 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0710_ADDRESS                            0xd0f0710

// Type
#define D18F2x09C_x0D0F_0710_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0710_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0710_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0710_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0710_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0710_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0710_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0710_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0710_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0710_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0710
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0710_STRUCT;

// **** D18F2x09C_x0D0F_071F Register Definition ****
// Address
#define D18F2x09C_x0D0F_071F_ADDRESS                            0xd0f071f

// Type
#define D18F2x09C_x0D0F_071F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_071F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_071F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_071F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_071F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_071F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_071F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_071F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_071F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_071F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_071F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_071F_STRUCT;



// **** D18F2x09C_x0D0F_0F02 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F02_ADDRESS                            0xd0f0f02

// Type
#define D18F2x09C_x0D0F_0F02_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F02_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0F02_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0F02_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0F02_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0F02_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0F02_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0F02_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_0F02_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_0F02_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_0F02_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_0F02_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_0F02_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_0F02_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_0F02_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_0F02_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_0F02
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F02_STRUCT;

// **** D18F2x09C_x0D0F_0F06 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F06_ADDRESS                            0xd0f0f06

// Type
#define D18F2x09C_x0D0F_0F06_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F06_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0F06_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0F06_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0F06_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0F06_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0F06_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0F06_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0F06_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0F06_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0F06
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F06_STRUCT;

// **** D18F2x09C_x0D0F_0F0A Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F0A_ADDRESS                            0xd0f0f0a

// Type
#define D18F2x09C_x0D0F_0F0A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F0A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_0F0A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_0F0A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_0F0A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_0F0A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_0F0A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_0F0A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_0F0A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_0F0A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_0F0A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F0A_STRUCT;

// **** D18F2x09C_x0D0F_0F0F Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F0F_ADDRESS                            0xd0f0f0f

// Type
#define D18F2x09C_x0D0F_0F0F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F0F_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0F0F_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0F0F_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0F0F_AlwaysEnDllClks_OFFSET             12
#define D18F2x09C_x0D0F_0F0F_AlwaysEnDllClks_WIDTH              3
#define D18F2x09C_x0D0F_0F0F_AlwaysEnDllClks_MASK               0x7000
#define D18F2x09C_x0D0F_0F0F_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_0F0F_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_0F0F_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_0F0F
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                          AlwaysEnDllClks:3 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F0F_STRUCT;

// **** D18F2x09C_x0D0F_0F10 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F10_ADDRESS                            0xd0f0f10

// Type
#define D18F2x09C_x0D0F_0F10_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F10_Reserved_11_0_OFFSET               0
#define D18F2x09C_x0D0F_0F10_Reserved_11_0_WIDTH                12
#define D18F2x09C_x0D0F_0F10_Reserved_11_0_MASK                 0xfff
#define D18F2x09C_x0D0F_0F10_EnRxPadStandby_OFFSET              12
#define D18F2x09C_x0D0F_0F10_EnRxPadStandby_WIDTH               1
#define D18F2x09C_x0D0F_0F10_EnRxPadStandby_MASK                0x1000
#define D18F2x09C_x0D0F_0F10_Reserved_31_13_OFFSET              13
#define D18F2x09C_x0D0F_0F10_Reserved_31_13_WIDTH               19
#define D18F2x09C_x0D0F_0F10_Reserved_31_13_MASK                0xffffe000

/// D18F2x09C_x0D0F_0F10
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_11_0:12; ///<
    UINT32                                           EnRxPadStandby:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F10_STRUCT;

// **** D18F2x09C_x0D0F_0F13 Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F13_ADDRESS                            0xd0f0f13

// Type
#define D18F2x09C_x0D0F_0F13_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F13_DllDisEarlyL_OFFSET                0
#define D18F2x09C_x0D0F_0F13_DllDisEarlyL_WIDTH                 1
#define D18F2x09C_x0D0F_0F13_DllDisEarlyL_MASK                  0x1
#define D18F2x09C_x0D0F_0F13_DllDisEarlyU_OFFSET                1
#define D18F2x09C_x0D0F_0F13_DllDisEarlyU_WIDTH                 1
#define D18F2x09C_x0D0F_0F13_DllDisEarlyU_MASK                  0x2
#define D18F2x09C_x0D0F_0F13_Reserved_6_2_OFFSET                2
#define D18F2x09C_x0D0F_0F13_Reserved_6_2_WIDTH                 5
#define D18F2x09C_x0D0F_0F13_Reserved_6_2_MASK                  0x7c
#define D18F2x09C_x0D0F_0F13_RxDqsUDllPowerDown_OFFSET          7
#define D18F2x09C_x0D0F_0F13_RxDqsUDllPowerDown_WIDTH           1
#define D18F2x09C_x0D0F_0F13_RxDqsUDllPowerDown_MASK            0x80
#define D18F2x09C_x0D0F_0F13_Reserved_13_8_OFFSET               8
#define D18F2x09C_x0D0F_0F13_Reserved_13_8_WIDTH                6
#define D18F2x09C_x0D0F_0F13_Reserved_13_8_MASK                 0x3f00
#define D18F2x09C_x0D0F_0F13_ProcOdtAdv_OFFSET                  14
#define D18F2x09C_x0D0F_0F13_ProcOdtAdv_WIDTH                   1
#define D18F2x09C_x0D0F_0F13_ProcOdtAdv_MASK                    0x4000
#define D18F2x09C_x0D0F_0F13_Reserved_31_15_OFFSET              15
#define D18F2x09C_x0D0F_0F13_Reserved_31_15_WIDTH               17
#define D18F2x09C_x0D0F_0F13_Reserved_31_15_MASK                0xffff8000

/// D18F2x09C_x0D0F_0F13
typedef union {
  struct {                                                              ///<
    UINT32                                             DllDisEarlyL:1 ; ///<
    UINT32                                             DllDisEarlyU:1 ; ///<
    UINT32                                             Reserved_6_2:5 ; ///<
    UINT32                                       RxDqsUDllPowerDown:1 ; ///<
    UINT32                                            Reserved_13_8:6 ; ///<
    UINT32                                               ProcOdtAdv:1 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F13_STRUCT;

// **** D18F2x09C_x0D0F_0F1F Register Definition ****
// Address
#define D18F2x09C_x0D0F_0F1F_ADDRESS                            0xd0f0f1f

// Type
#define D18F2x09C_x0D0F_0F1F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_0F1F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_0F1F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_0F1F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_0F1F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_0F1F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_0F1F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_0F1F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_0F1F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_0F1F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_0F1F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_0F1F_STRUCT;



// **** D18F2x09C_x0D0F_2002 Register Definition ****
// Address
#define D18F2x09C_x0D0F_2002_ADDRESS                            0xd0f2002

// Type
#define D18F2x09C_x0D0F_2002_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_2002_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_2002_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_2002_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_2002_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_2002_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_2002_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_2002_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_2002_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_2002_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_2002_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_2002_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_2002_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_2002_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_2002_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_2002_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_2002
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_2002_STRUCT;

// **** D18F2x09C_x0D0F_201F Register Definition ****
// Address
#define D18F2x09C_x0D0F_201F_ADDRESS                            0xd0f201f

// Type
#define D18F2x09C_x0D0F_201F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_201F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_201F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_201F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_201F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_201F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_201F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_201F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_201F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_201F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_201F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_201F_STRUCT;


// **** D18F2x09C_x0D0F_2030 Register Definition ****
// Address
#define D18F2x09C_x0D0F_2030_ADDRESS                            0xd0f2030

// Type
#define D18F2x09C_x0D0F_2030_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_2030_Reserved_3_0_OFFSET                0
#define D18F2x09C_x0D0F_2030_Reserved_3_0_WIDTH                 4
#define D18F2x09C_x0D0F_2030_Reserved_3_0_MASK                  0xf
#define D18F2x09C_x0D0F_2030_PwrDn_OFFSET                       4
#define D18F2x09C_x0D0F_2030_PwrDn_WIDTH                        1
#define D18F2x09C_x0D0F_2030_PwrDn_MASK                         0x10
#define D18F2x09C_x0D0F_2030_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_2030_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_2030_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_2030
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                                    PwrDn:1 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_2030_STRUCT;


// **** D18F2x09C_x0D0F_2102 Register Definition ****
// Address
#define D18F2x09C_x0D0F_2102_ADDRESS                            0xd0f2102

// Type
#define D18F2x09C_x0D0F_2102_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_2102_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_2102_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_2102_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_2102_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_2102_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_2102_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_2102_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_2102_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_2102_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_2102_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_2102_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_2102_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_2102_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_2102_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_2102_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_2102
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_2102_STRUCT;

// **** D18F2x09C_x0D0F_211F Register Definition ****
// Address
#define D18F2x09C_x0D0F_211F_ADDRESS                            0xd0f211f

// Type
#define D18F2x09C_x0D0F_211F_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_211F_Reserved_2_0_OFFSET                0
#define D18F2x09C_x0D0F_211F_Reserved_2_0_WIDTH                 3
#define D18F2x09C_x0D0F_211F_Reserved_2_0_MASK                  0x7
#define D18F2x09C_x0D0F_211F_RxVioLvl_OFFSET                    3
#define D18F2x09C_x0D0F_211F_RxVioLvl_WIDTH                     2
#define D18F2x09C_x0D0F_211F_RxVioLvl_MASK                      0x18
#define D18F2x09C_x0D0F_211F_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_211F_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_211F_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_211F
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                 RxVioLvl:2 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_211F_STRUCT;


// **** D18F2x09C_x0D0F_2130 Register Definition ****
// Address
#define D18F2x09C_x0D0F_2130_ADDRESS                            0xd0f2130

// Type
#define D18F2x09C_x0D0F_2130_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_2130_Reserved_3_0_OFFSET                0
#define D18F2x09C_x0D0F_2130_Reserved_3_0_WIDTH                 4
#define D18F2x09C_x0D0F_2130_Reserved_3_0_MASK                  0xf
#define D18F2x09C_x0D0F_2130_PwrDn_OFFSET                       4
#define D18F2x09C_x0D0F_2130_PwrDn_WIDTH                        1
#define D18F2x09C_x0D0F_2130_PwrDn_MASK                         0x10
#define D18F2x09C_x0D0F_2130_Reserved_31_5_OFFSET               5
#define D18F2x09C_x0D0F_2130_Reserved_31_5_WIDTH                27
#define D18F2x09C_x0D0F_2130_Reserved_31_5_MASK                 0xffffffe0

/// D18F2x09C_x0D0F_2130
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                                    PwrDn:1 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_2130_STRUCT;

// Field Data
#define D18F2x09C_x0D0F_4009_Reserved_1_0_OFFSET                0
#define D18F2x09C_x0D0F_4009_Reserved_1_0_WIDTH                 2
#define D18F2x09C_x0D0F_4009_Reserved_1_0_MASK                  0x3
#define D18F2x09C_x0D0F_4009_ComparatorAdjust_OFFSET            2
#define D18F2x09C_x0D0F_4009_ComparatorAdjust_WIDTH             2
#define D18F2x09C_x0D0F_4009_ComparatorAdjust_MASK              0xc
#define D18F2x09C_x0D0F_4009_Reserved_13_4_OFFSET               4
#define D18F2x09C_x0D0F_4009_Reserved_13_4_WIDTH                10
#define D18F2x09C_x0D0F_4009_Reserved_13_4_MASK                 0x3ff0
#define D18F2x09C_x0D0F_4009_CmpVioLvl_OFFSET                   14
#define D18F2x09C_x0D0F_4009_CmpVioLvl_WIDTH                    2
#define D18F2x09C_x0D0F_4009_CmpVioLvl_MASK                     0xc000
#define D18F2x09C_x0D0F_4009_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_4009_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_4009_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_4009
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                         ComparatorAdjust:2 ; ///<
    UINT32                                            Reserved_13_4:10; ///<
    UINT32                                                CmpVioLvl:2 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_4009_STRUCT;

// **** D18F2x09C_x0D0F_8002 Register Definition ****
// Address
#define D18F2x09C_x0D0F_8002_ADDRESS                            0xd0f8002

// Type
#define D18F2x09C_x0D0F_8002_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_8002_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_8002_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_8002_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_8002_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_8002_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_8002_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_8002_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_8002_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_8002_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_8002_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_8002_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_8002_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_8002_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_8002_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_8002_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_8002
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_8002_STRUCT;

// **** D18F2x09C_x0D0F_8006 Register Definition ****
// Address
#define D18F2x09C_x0D0F_8006_ADDRESS                            0xd0f8006

// Type
#define D18F2x09C_x0D0F_8006_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_8006_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_8006_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_8006_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_8006_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_8006_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_8006_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_8006_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_8006_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_8006_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_8006
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_8006_STRUCT;

// **** D18F2x09C_x0D0F_800A Register Definition ****
// Address
#define D18F2x09C_x0D0F_800A_ADDRESS                            0xd0f800a

// Type
#define D18F2x09C_x0D0F_800A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_800A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_800A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_800A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_800A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_800A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_800A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_800A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_800A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_800A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_800A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_800A_STRUCT;



// **** D18F2x09C_x0D0F_8102 Register Definition ****
// Address
#define D18F2x09C_x0D0F_8102_ADDRESS                            0xd0f8102

// Type
#define D18F2x09C_x0D0F_8102_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_8102_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_8102_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_8102_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_8102_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_8102_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_8102_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_8102_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_8102_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_8102_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_8102_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_8102_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_8102_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_8102_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_8102_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_8102_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_8102
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_8102_STRUCT;

// **** D18F2x09C_x0D0F_8106 Register Definition ****
// Address
#define D18F2x09C_x0D0F_8106_ADDRESS                            0xd0f8106

// Type
#define D18F2x09C_x0D0F_8106_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_8106_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_8106_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_8106_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_8106_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_8106_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_8106_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_8106_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_8106_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_8106_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_8106
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_8106_STRUCT;

// **** D18F2x09C_x0D0F_810A Register Definition ****
// Address
#define D18F2x09C_x0D0F_810A_ADDRESS                            0xd0f810a

// Type
#define D18F2x09C_x0D0F_810A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_810A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_810A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_810A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_810A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_810A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_810A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_810A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_810A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_810A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_810A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_810A_STRUCT;




// **** D18F2x09C_x0D0F_C000 Register Definition ****
// Address
#define D18F2x09C_x0D0F_C000_ADDRESS                            0xd0fc000

// Type
#define D18F2x09C_x0D0F_C000_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_C000_Reserved_7_0_OFFSET                0
#define D18F2x09C_x0D0F_C000_Reserved_7_0_WIDTH                 8
#define D18F2x09C_x0D0F_C000_Reserved_7_0_MASK                  0xff
#define D18F2x09C_x0D0F_C000_LowPowerDrvStrengthEn_OFFSET       8
#define D18F2x09C_x0D0F_C000_LowPowerDrvStrengthEn_WIDTH        1
#define D18F2x09C_x0D0F_C000_LowPowerDrvStrengthEn_MASK         0x100
#define D18F2x09C_x0D0F_C000_Reserved_31_9_OFFSET               9
#define D18F2x09C_x0D0F_C000_Reserved_31_9_WIDTH                23
#define D18F2x09C_x0D0F_C000_Reserved_31_9_MASK                 0xfffffe00

/// D18F2x09C_x0D0F_C000
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_7_0:8 ; ///<
    UINT32                                    LowPowerDrvStrengthEn:1 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_C000_STRUCT;

// **** D18F2x09C_x0D0F_C002 Register Definition ****
// Address
#define D18F2x09C_x0D0F_C002_ADDRESS                            0xd0fc002

// Type
#define D18F2x09C_x0D0F_C002_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_C002_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_C002_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_C002_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_C002_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_C002_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_C002_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_C002_Reserved_14_12_OFFSET              12
#define D18F2x09C_x0D0F_C002_Reserved_14_12_WIDTH               3
#define D18F2x09C_x0D0F_C002_Reserved_14_12_MASK                0x7000
#define D18F2x09C_x0D0F_C002_ValidTxAndPre_OFFSET               15
#define D18F2x09C_x0D0F_C002_ValidTxAndPre_WIDTH                1
#define D18F2x09C_x0D0F_C002_ValidTxAndPre_MASK                 0x8000
#define D18F2x09C_x0D0F_C002_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_C002_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_C002_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_C002
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_14_12:3 ; ///<
    UINT32                                            ValidTxAndPre:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_C002_STRUCT;

// **** D18F2x09C_x0D0F_C006 Register Definition ****
// Address
#define D18F2x09C_x0D0F_C006_ADDRESS                            0xd0fc006

// Type
#define D18F2x09C_x0D0F_C006_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_C006_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_C006_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_C006_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_C006_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_C006_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_C006_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_C006_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_C006_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_C006_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_C006
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_C006_STRUCT;

// **** D18F2x09C_x0D0F_C00A Register Definition ****
// Address
#define D18F2x09C_x0D0F_C00A_ADDRESS                            0xd0fc00a

// Type
#define D18F2x09C_x0D0F_C00A_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_C00A_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_C00A_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_C00A_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_C00A_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_C00A_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_C00A_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_C00A_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_C00A_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_C00A_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_C00A
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_C00A_STRUCT;

// **** D18F2x09C_x0D0F_C00E Register Definition ****
// Address
#define D18F2x09C_x0D0F_C00E_ADDRESS                            0xd0fc00e

// Type
#define D18F2x09C_x0D0F_C00E_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_C00E_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_C00E_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_C00E_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_C00E_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_C00E_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_C00E_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_C00E_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_C00E_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_C00E_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_C00E
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_C00E_STRUCT;

// **** D18F2x09C_x0D0F_C012 Register Definition ****
// Address
#define D18F2x09C_x0D0F_C012_ADDRESS                            0xd0fc012

// Type
#define D18F2x09C_x0D0F_C012_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_C012_TxPreN_OFFSET                      0
#define D18F2x09C_x0D0F_C012_TxPreN_WIDTH                       6
#define D18F2x09C_x0D0F_C012_TxPreN_MASK                        0x3f
#define D18F2x09C_x0D0F_C012_TxPreP_OFFSET                      6
#define D18F2x09C_x0D0F_C012_TxPreP_WIDTH                       6
#define D18F2x09C_x0D0F_C012_TxPreP_MASK                        0xfc0
#define D18F2x09C_x0D0F_C012_Reserved_31_12_OFFSET              12
#define D18F2x09C_x0D0F_C012_Reserved_31_12_WIDTH               20
#define D18F2x09C_x0D0F_C012_Reserved_31_12_MASK                0xfffff000

/// D18F2x09C_x0D0F_C012
typedef union {
  struct {                                                              ///<
    UINT32                                                   TxPreN:6 ; ///<
    UINT32                                                   TxPreP:6 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_C012_STRUCT;




// **** D18F2x09C_x0D0F_E006 Register Definition ****
// Address
#define D18F2x09C_x0D0F_E006_ADDRESS                            0xd0fe006

// Type
#define D18F2x09C_x0D0F_E006_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_E006_PllLockTime_OFFSET                 0
#define D18F2x09C_x0D0F_E006_PllLockTime_WIDTH                  16
#define D18F2x09C_x0D0F_E006_PllLockTime_MASK                   0xffff
#define D18F2x09C_x0D0F_E006_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_E006_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_E006_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_E006
typedef union {
  struct {                                                              ///<
    UINT32                                              PllLockTime:16; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_E006_STRUCT;


// **** D18F2x09C_x0D0F_E013 Register Definition ****
// Address
#define D18F2x09C_x0D0F_E013_ADDRESS                            0xd0fe013

// Type
#define D18F2x09C_x0D0F_E013_TYPE                               TYPE_D18F2x09C
// Field Data
#define D18F2x09C_x0D0F_E013_PllRegWaitTime_OFFSET              0
#define D18F2x09C_x0D0F_E013_PllRegWaitTime_WIDTH               16
#define D18F2x09C_x0D0F_E013_PllRegWaitTime_MASK                0xffff
#define D18F2x09C_x0D0F_E013_Reserved_31_16_OFFSET              16
#define D18F2x09C_x0D0F_E013_Reserved_31_16_WIDTH               16
#define D18F2x09C_x0D0F_E013_Reserved_31_16_MASK                0xffff0000

/// D18F2x09C_x0D0F_E013
typedef union {
  struct {                                                              ///<
    UINT32                                           PllRegWaitTime:16; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0F_E013_STRUCT;


// **** DxF0xE4_x02 Register Definition ****
// Address
#define DxF0xE4_x02_ADDRESS                                     0x2

// Type
#define DxF0xE4_x02_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_x02_Reserved_14_0_OFFSET                        0
#define DxF0xE4_x02_Reserved_14_0_WIDTH                         15
#define DxF0xE4_x02_Reserved_14_0_MASK                          0x7fff
#define DxF0xE4_x02_RegsLcAllowTxL1Control_OFFSET               15
#define DxF0xE4_x02_RegsLcAllowTxL1Control_WIDTH                1
#define DxF0xE4_x02_RegsLcAllowTxL1Control_MASK                 0x8000
#define DxF0xE4_x02_Reserved_31_16_OFFSET                       16
#define DxF0xE4_x02_Reserved_31_16_WIDTH                        16
#define DxF0xE4_x02_Reserved_31_16_MASK                         0xffff0000

/// DxF0xE4_x02
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_14_0:15; ///<
    UINT32                                   RegsLcAllowTxL1Control:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_x02_STRUCT;

// **** DxF0xE4_x20 Register Definition ****
// Address
#define DxF0xE4_x20_ADDRESS                                     0x20

// Type
#define DxF0xE4_x20_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_x20_Reserved_14_0_OFFSET                        0
#define DxF0xE4_x20_Reserved_14_0_WIDTH                         15
#define DxF0xE4_x20_Reserved_14_0_MASK                          0x7fff
#define DxF0xE4_x20_TxFlushTlpDis_OFFSET                        15
#define DxF0xE4_x20_TxFlushTlpDis_WIDTH                         1
#define DxF0xE4_x20_TxFlushTlpDis_MASK                          0x8000
#define DxF0xE4_x20_Reserved_31_16_OFFSET                       16
#define DxF0xE4_x20_Reserved_31_16_WIDTH                        16
#define DxF0xE4_x20_Reserved_31_16_MASK                         0xffff0000

/// DxF0xE4_x20
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_14_0:15; ///<
    UINT32                                            TxFlushTlpDis:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_x20_STRUCT;

// **** DxF0xE4_x50 Register Definition ****
// Address
#define DxF0xE4_x50_ADDRESS                                     0x50

// Type
#define DxF0xE4_x50_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_x50_PortLaneReversal_OFFSET                     0
#define DxF0xE4_x50_PortLaneReversal_WIDTH                      1
#define DxF0xE4_x50_PortLaneReversal_MASK                       0x1
#define DxF0xE4_x50_PhyLinkWidth_OFFSET                         1
#define DxF0xE4_x50_PhyLinkWidth_WIDTH                          6
#define DxF0xE4_x50_PhyLinkWidth_MASK                           0x7e
#define DxF0xE4_x50_Reserved_31_7_OFFSET                        7
#define DxF0xE4_x50_Reserved_31_7_WIDTH                         25
#define DxF0xE4_x50_Reserved_31_7_MASK                          0xffffff80

/// DxF0xE4_x50
typedef union {
  struct {                                                              ///<
    UINT32                                         PortLaneReversal:1 ; ///<
    UINT32                                             PhyLinkWidth:6 ; ///<
    UINT32                                            Reserved_31_7:25; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_x50_STRUCT;

// **** DxF0xE4_x70 Register Definition ****
// Address
#define DxF0xE4_x70_ADDRESS                                     0x70

// Type
#define DxF0xE4_x70_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_x70_Reserved_15_0_OFFSET                        0
#define DxF0xE4_x70_Reserved_15_0_WIDTH                         16
#define DxF0xE4_x70_Reserved_15_0_MASK                          0xffff
#define DxF0xE4_x70_RxRcbCplTimeout_OFFSET                      16
#define DxF0xE4_x70_RxRcbCplTimeout_WIDTH                       3
#define DxF0xE4_x70_RxRcbCplTimeout_MASK                        0x70000
#define DxF0xE4_x70_RxRcbCplTimeoutMode_OFFSET                  19
#define DxF0xE4_x70_RxRcbCplTimeoutMode_WIDTH                   1
#define DxF0xE4_x70_RxRcbCplTimeoutMode_MASK                    0x80000
#define DxF0xE4_x70_Reserved_31_20_OFFSET                       20
#define DxF0xE4_x70_Reserved_31_20_WIDTH                        12
#define DxF0xE4_x70_Reserved_31_20_MASK                         0xfff00000

/// DxF0xE4_x70
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_15_0:16; ///<
    UINT32                                          RxRcbCplTimeout:3 ; ///<
    UINT32                                      RxRcbCplTimeoutMode:1 ; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_x70_STRUCT;

// **** DxF0xE4_xA0 Register Definition ****
// Address
#define DxF0xE4_xA0_ADDRESS                                     0xa0

// Type
#define DxF0xE4_xA0_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xA0_Reserved_3_0_OFFSET                         0
#define DxF0xE4_xA0_Reserved_3_0_WIDTH                          4
#define DxF0xE4_xA0_Reserved_3_0_MASK                           0xf
#define DxF0xE4_xA0_Lc16xClearTxPipe_OFFSET                     4
#define DxF0xE4_xA0_Lc16xClearTxPipe_WIDTH                      4
#define DxF0xE4_xA0_Lc16xClearTxPipe_MASK                       0xf0
#define DxF0xE4_xA0_LcL0sInactivity_OFFSET                      8
#define DxF0xE4_xA0_LcL0sInactivity_WIDTH                       4
#define DxF0xE4_xA0_LcL0sInactivity_MASK                        0xf00
#define DxF0xE4_xA0_LcL1Inactivity_OFFSET                       12
#define DxF0xE4_xA0_LcL1Inactivity_WIDTH                        4
#define DxF0xE4_xA0_LcL1Inactivity_MASK                         0xf000
#define DxF0xE4_xA0_Reserved_22_16_OFFSET                       16
#define DxF0xE4_xA0_Reserved_22_16_WIDTH                        7
#define DxF0xE4_xA0_Reserved_22_16_MASK                         0x7f0000
#define DxF0xE4_xA0_LcL1ImmediateAck_OFFSET                     23
#define DxF0xE4_xA0_LcL1ImmediateAck_WIDTH                      1
#define DxF0xE4_xA0_LcL1ImmediateAck_MASK                       0x800000
#define DxF0xE4_xA0_Reserved_31_24_OFFSET                       24
#define DxF0xE4_xA0_Reserved_31_24_WIDTH                        8
#define DxF0xE4_xA0_Reserved_31_24_MASK                         0xff000000

/// DxF0xE4_xA0
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                         Lc16xClearTxPipe:4 ; ///<
    UINT32                                          LcL0sInactivity:4 ; ///<
    UINT32                                           LcL1Inactivity:4 ; ///<
    UINT32                                           Reserved_22_16:7 ; ///<
    UINT32                                         LcL1ImmediateAck:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xA0_STRUCT;

// **** DxF0xE4_xA1 Register Definition ****
// Address
#define DxF0xE4_xA1_ADDRESS                                     0xa1

// Type
#define DxF0xE4_xA1_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xA1_Reserved_10_0_OFFSET                        0
#define DxF0xE4_xA1_Reserved_10_0_WIDTH                         11
#define DxF0xE4_xA1_Reserved_10_0_MASK                          0x7ff
#define DxF0xE4_xA1_LcDontGotoL0sifL1Armed_OFFSET               11
#define DxF0xE4_xA1_LcDontGotoL0sifL1Armed_WIDTH                1
#define DxF0xE4_xA1_LcDontGotoL0sifL1Armed_MASK                 0x800
#define DxF0xE4_xA1_LcInitSpdChgWithCsrEn_OFFSET                12
#define DxF0xE4_xA1_LcInitSpdChgWithCsrEn_WIDTH                 1
#define DxF0xE4_xA1_LcInitSpdChgWithCsrEn_MASK                  0x1000
#define DxF0xE4_xA1_Reserved_31_13_OFFSET                       13
#define DxF0xE4_xA1_Reserved_31_13_WIDTH                        19
#define DxF0xE4_xA1_Reserved_31_13_MASK                         0xffffe000

/// DxF0xE4_xA1
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_10_0:11; ///<
    UINT32                                   LcDontGotoL0sifL1Armed:1 ; ///<
    UINT32                                    LcInitSpdChgWithCsrEn:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xA1_STRUCT;

// **** DxF0xE4_xA2 Register Definition ****
// Address
#define DxF0xE4_xA2_ADDRESS                                     0xa2

// Type
#define DxF0xE4_xA2_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xA2_LcLinkWidth_OFFSET                          0
#define DxF0xE4_xA2_LcLinkWidth_WIDTH                           3
#define DxF0xE4_xA2_LcLinkWidth_MASK                            0x7
#define DxF0xE4_xA2_Reserved_3_3_OFFSET                         3
#define DxF0xE4_xA2_Reserved_3_3_WIDTH                          1
#define DxF0xE4_xA2_Reserved_3_3_MASK                           0x8
#define DxF0xE4_xA2_LcLinkWidthRd_OFFSET                        4
#define DxF0xE4_xA2_LcLinkWidthRd_WIDTH                         3
#define DxF0xE4_xA2_LcLinkWidthRd_MASK                          0x70
#define DxF0xE4_xA2_LcReconfigArcMissingEscape_OFFSET           7
#define DxF0xE4_xA2_LcReconfigArcMissingEscape_WIDTH            1
#define DxF0xE4_xA2_LcReconfigArcMissingEscape_MASK             0x80
#define DxF0xE4_xA2_LcReconfigNow_OFFSET                        8
#define DxF0xE4_xA2_LcReconfigNow_WIDTH                         1
#define DxF0xE4_xA2_LcReconfigNow_MASK                          0x100
#define DxF0xE4_xA2_LcRenegotiationSupport_OFFSET               9
#define DxF0xE4_xA2_LcRenegotiationSupport_WIDTH                1
#define DxF0xE4_xA2_LcRenegotiationSupport_MASK                 0x200
#define DxF0xE4_xA2_LcRenegotiateEn_OFFSET                      10
#define DxF0xE4_xA2_LcRenegotiateEn_WIDTH                       1
#define DxF0xE4_xA2_LcRenegotiateEn_MASK                        0x400
#define DxF0xE4_xA2_LcShortReconfigEn_OFFSET                    11
#define DxF0xE4_xA2_LcShortReconfigEn_WIDTH                     1
#define DxF0xE4_xA2_LcShortReconfigEn_MASK                      0x800
#define DxF0xE4_xA2_LcUpconfigureSupport_OFFSET                 12
#define DxF0xE4_xA2_LcUpconfigureSupport_WIDTH                  1
#define DxF0xE4_xA2_LcUpconfigureSupport_MASK                   0x1000
#define DxF0xE4_xA2_LcUpconfigureDis_OFFSET                     13
#define DxF0xE4_xA2_LcUpconfigureDis_WIDTH                      1
#define DxF0xE4_xA2_LcUpconfigureDis_MASK                       0x2000
#define DxF0xE4_xA2_Reserved_19_14_OFFSET                       14
#define DxF0xE4_xA2_Reserved_19_14_WIDTH                        6
#define DxF0xE4_xA2_Reserved_19_14_MASK                         0xfc000
#define DxF0xE4_xA2_LcUpconfigCapable_OFFSET                    20
#define DxF0xE4_xA2_LcUpconfigCapable_WIDTH                     1
#define DxF0xE4_xA2_LcUpconfigCapable_MASK                      0x100000
#define DxF0xE4_xA2_LcDynLanesPwrState_OFFSET                   21
#define DxF0xE4_xA2_LcDynLanesPwrState_WIDTH                    2
#define DxF0xE4_xA2_LcDynLanesPwrState_MASK                     0x600000
#define DxF0xE4_xA2_Reserved_31_23_OFFSET                       23
#define DxF0xE4_xA2_Reserved_31_23_WIDTH                        9
#define DxF0xE4_xA2_Reserved_31_23_MASK                         0xff800000

/// DxF0xE4_xA2
typedef union {
  struct {                                                              ///<
    UINT32                                              LcLinkWidth:3 ; ///<
    UINT32                                             Reserved_3_3:1 ; ///<
    UINT32                                            LcLinkWidthRd:3 ; ///<
    UINT32                               LcReconfigArcMissingEscape:1 ; ///<
    UINT32                                            LcReconfigNow:1 ; ///<
    UINT32                                   LcRenegotiationSupport:1 ; ///<
    UINT32                                          LcRenegotiateEn:1 ; ///<
    UINT32                                        LcShortReconfigEn:1 ; ///<
    UINT32                                     LcUpconfigureSupport:1 ; ///<
    UINT32                                         LcUpconfigureDis:1 ; ///<
    UINT32                                           Reserved_19_14:6 ; ///<
    UINT32                                        LcUpconfigCapable:1 ; ///<
    UINT32                                       LcDynLanesPwrState:2 ; ///<
    UINT32                                           Reserved_31_23:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xA2_STRUCT;

// **** DxF0xE4_xA3 Register Definition ****
// Address
#define DxF0xE4_xA3_ADDRESS                                     0xa3

// Type
#define DxF0xE4_xA3_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xA3_Reserved_8_0_OFFSET                         0
#define DxF0xE4_xA3_Reserved_8_0_WIDTH                          9
#define DxF0xE4_xA3_Reserved_8_0_MASK                           0x1ff
#define DxF0xE4_xA3_LcXmitFtsBeforeRecovery_OFFSET              9
#define DxF0xE4_xA3_LcXmitFtsBeforeRecovery_WIDTH               1
#define DxF0xE4_xA3_LcXmitFtsBeforeRecovery_MASK                0x200
#define DxF0xE4_xA3_Reserved_31_10_OFFSET                       10
#define DxF0xE4_xA3_Reserved_31_10_WIDTH                        22
#define DxF0xE4_xA3_Reserved_31_10_MASK                         0xfffffc00

/// DxF0xE4_xA3
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                  LcXmitFtsBeforeRecovery:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xA3_STRUCT;

/// DxF0xE4_xA4
typedef union {
  struct {                                                              ///<
    UINT32                                            LcGen2EnStrap:1 ; ///<
    UINT32                                             Reserved_3_1:3 ; ///<
    UINT32                                  LcForceDisSwSpeedChange:1 ; ///<
    UINT32                                             Reserved_6_5:2 ; ///<
    UINT32                                LcInitiateLinkSpeedChange:1 ; ///<
    UINT32                                             Reserved_9_8:2 ; ///<
    UINT32                               LcSpeedChangeAttemptFailed:1 ; ///<
    UINT32                                           Reserved_17_11:7 ; ///<
    UINT32                                           LcGoToRecovery:1 ; ///<
    UINT32                                           Reserved_23_19:5 ; ///<
    UINT32                                  LcOtherSideSupportsGen2:1 ; ///<
    UINT32                                           Reserved_28_25:4 ; ///<
    UINT32                              LcMultUpstreamAutoSpdChngEn:1 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex548_STRUCT;

// **** DxF0xE4_xA5 Register Definition ****
// Address
#define DxF0xE4_xA5_ADDRESS                                     0xa5

// Type
#define DxF0xE4_xA5_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xA5_LcCurrentState_OFFSET                       0
#define DxF0xE4_xA5_LcCurrentState_WIDTH                        6
#define DxF0xE4_xA5_LcCurrentState_MASK                         0x3f
#define DxF0xE4_xA5_Reserved_7_6_OFFSET                         6
#define DxF0xE4_xA5_Reserved_7_6_WIDTH                          2
#define DxF0xE4_xA5_Reserved_7_6_MASK                           0xc0
#define DxF0xE4_xA5_LcPrevState1_OFFSET                         8
#define DxF0xE4_xA5_LcPrevState1_WIDTH                          6
#define DxF0xE4_xA5_LcPrevState1_MASK                           0x3f00
#define DxF0xE4_xA5_Reserved_15_14_OFFSET                       14
#define DxF0xE4_xA5_Reserved_15_14_WIDTH                        2
#define DxF0xE4_xA5_Reserved_15_14_MASK                         0xc000
#define DxF0xE4_xA5_LcPrevState2_OFFSET                         16
#define DxF0xE4_xA5_LcPrevState2_WIDTH                          6
#define DxF0xE4_xA5_LcPrevState2_MASK                           0x3f0000
#define DxF0xE4_xA5_Reserved_23_22_OFFSET                       22
#define DxF0xE4_xA5_Reserved_23_22_WIDTH                        2
#define DxF0xE4_xA5_Reserved_23_22_MASK                         0xc00000
#define DxF0xE4_xA5_LcPrevState3_OFFSET                         24
#define DxF0xE4_xA5_LcPrevState3_WIDTH                          6
#define DxF0xE4_xA5_LcPrevState3_MASK                           0x3f000000
#define DxF0xE4_xA5_Reserved_31_30_OFFSET                       30
#define DxF0xE4_xA5_Reserved_31_30_WIDTH                        2
#define DxF0xE4_xA5_Reserved_31_30_MASK                         0xc0000000

/// DxF0xE4_xA5
typedef union {
  struct {                                                              ///<
    UINT32                                           LcCurrentState:6 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                             LcPrevState1:6 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                             LcPrevState2:6 ; ///<
    UINT32                                           Reserved_23_22:2 ; ///<
    UINT32                                             LcPrevState3:6 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xA5_STRUCT;

// **** DxF0xE4_xB1 Register Definition ****
// Address
#define DxF0xE4_xB1_ADDRESS                                     0xb1

// Type
#define DxF0xE4_xB1_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xB1_Reserved_18_0_OFFSET                        0
#define DxF0xE4_xB1_Reserved_18_0_WIDTH                         19
#define DxF0xE4_xB1_Reserved_18_0_MASK                          0x7ffff
#define DxF0xE4_xB1_LcDeassertRxEnInL0s_OFFSET                  19
#define DxF0xE4_xB1_LcDeassertRxEnInL0s_WIDTH                   1
#define DxF0xE4_xB1_LcDeassertRxEnInL0s_MASK                    0x80000
#define DxF0xE4_xB1_LcBlockElIdleinL0_OFFSET                    20
#define DxF0xE4_xB1_LcBlockElIdleinL0_WIDTH                     1
#define DxF0xE4_xB1_LcBlockElIdleinL0_MASK                      0x100000
#define DxF0xE4_xB1_Reserved_31_21_OFFSET                       21
#define DxF0xE4_xB1_Reserved_31_21_WIDTH                        11
#define DxF0xE4_xB1_Reserved_31_21_MASK                         0xffe00000

/// DxF0xE4_xB1
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_18_0:19; ///<
    UINT32                                      LcDeassertRxEnInL0s:1 ; ///<
    UINT32                                        LcBlockElIdleinL0:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xB1_STRUCT;

// **** DxF0xE4_xC0 Register Definition ****
// Address
#define DxF0xE4_xC0_ADDRESS                                     0xc0

// Type
#define DxF0xE4_xC0_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xC0_Reserved_12_0_OFFSET                        0
#define DxF0xE4_xC0_Reserved_12_0_WIDTH                         13
#define DxF0xE4_xC0_Reserved_12_0_MASK                          0x1fff
#define DxF0xE4_xC0_StrapForceCompliance_OFFSET                 13
#define DxF0xE4_xC0_StrapForceCompliance_WIDTH                  1
#define DxF0xE4_xC0_StrapForceCompliance_MASK                   0x2000
#define DxF0xE4_xC0_Reserved_14_14_OFFSET                       14
#define DxF0xE4_xC0_Reserved_14_14_WIDTH                        1
#define DxF0xE4_xC0_Reserved_14_14_MASK                         0x4000
#define DxF0xE4_xC0_StrapAutoRcSpeedNegotiationDis_OFFSET       15
#define DxF0xE4_xC0_StrapAutoRcSpeedNegotiationDis_WIDTH        1
#define DxF0xE4_xC0_StrapAutoRcSpeedNegotiationDis_MASK         0x8000
#define DxF0xE4_xC0_Reserved_31_16_OFFSET                       16
#define DxF0xE4_xC0_Reserved_31_16_WIDTH                        16
#define DxF0xE4_xC0_Reserved_31_16_MASK                         0xffff0000

/// DxF0xE4_xC0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_12_0:13; ///<
    UINT32                                     StrapForceCompliance:1 ; ///<
    UINT32                                           Reserved_14_14:1 ; ///<
    UINT32                           StrapAutoRcSpeedNegotiationDis:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xC0_STRUCT;



// **** GMMx4D0 Register Definition ****
// Address
#define GMMx4D0_ADDRESS                                         0x4d0

// Type
#define GMMx4D0_TYPE                                            TYPE_GMM
// Field Data
#define GMMx4D0_DispclkDccgGateDisable_OFFSET                   0
#define GMMx4D0_DispclkDccgGateDisable_WIDTH                    1
#define GMMx4D0_DispclkDccgGateDisable_MASK                     0x1
#define GMMx4D0_DispclkRDccgGateDisable_OFFSET                  1
#define GMMx4D0_DispclkRDccgGateDisable_WIDTH                   1
#define GMMx4D0_DispclkRDccgGateDisable_MASK                    0x2
#define GMMx4D0_SclkGateDisable_OFFSET                          2
#define GMMx4D0_SclkGateDisable_WIDTH                           1
#define GMMx4D0_SclkGateDisable_MASK                            0x4
#define GMMx4D0_Reserved_7_3_OFFSET                             3
#define GMMx4D0_Reserved_7_3_WIDTH                              5
#define GMMx4D0_Reserved_7_3_MASK                               0xf8
#define GMMx4D0_SymclkaGateDisable_OFFSET                       8
#define GMMx4D0_SymclkaGateDisable_WIDTH                        1
#define GMMx4D0_SymclkaGateDisable_MASK                         0x100
#define GMMx4D0_SymclkbGateDisable_OFFSET                       9
#define GMMx4D0_SymclkbGateDisable_WIDTH                        1
#define GMMx4D0_SymclkbGateDisable_MASK                         0x200
#define GMMx4D0_Reserved_31_10_OFFSET                           10
#define GMMx4D0_Reserved_31_10_WIDTH                            22
#define GMMx4D0_Reserved_31_10_MASK                             0xfffffc00

/// GMMx4D0
typedef union {
  struct {                                                              ///<
    UINT32                                   DispclkDccgGateDisable:1 ; ///<
    UINT32                                  DispclkRDccgGateDisable:1 ; ///<
    UINT32                                          SclkGateDisable:1 ; ///<
    UINT32                                             Reserved_7_3:5 ; ///<
    UINT32                                       SymclkaGateDisable:1 ; ///<
    UINT32                                       SymclkbGateDisable:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx4D0_STRUCT;

// **** GMMx770 Register Definition ****
// Address
#define GMMx770_ADDRESS                                         0x770

// Type
#define GMMx770_TYPE                                            TYPE_GMM
// Field Data
#define GMMx770_VoltageChangeReq_OFFSET                         0
#define GMMx770_VoltageChangeReq_WIDTH                          1
#define GMMx770_VoltageChangeReq_MASK                           0x1
#define GMMx770_VoltageLevel_OFFSET                             1
#define GMMx770_VoltageLevel_WIDTH                              2
#define GMMx770_VoltageLevel_MASK                               0x6
#define GMMx770_VoltageChangeEn_OFFSET                          3
#define GMMx770_VoltageChangeEn_WIDTH                           1
#define GMMx770_VoltageChangeEn_MASK                            0x8
#define GMMx770_VoltageForceEn_OFFSET                           4
#define GMMx770_VoltageForceEn_WIDTH                            1
#define GMMx770_VoltageForceEn_MASK                             0x10
#define GMMx770_Reserved_31_5_OFFSET                            5
#define GMMx770_Reserved_31_5_WIDTH                             27
#define GMMx770_Reserved_31_5_MASK                              0xffffffe0

/// GMMx770
typedef union {
  struct {                                                              ///<
    UINT32                                         VoltageChangeReq:1 ; ///<
    UINT32                                             VoltageLevel:2 ; ///<
    UINT32                                          VoltageChangeEn:1 ; ///<
    UINT32                                           VoltageForceEn:1 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx770_STRUCT;

// **** GMMx774 Register Definition ****
// Address
#define GMMx774_ADDRESS                                         0x774

// Type
#define GMMx774_TYPE                                            TYPE_GMM
// Field Data
#define GMMx774_VoltageChangeAck_OFFSET                         0
#define GMMx774_VoltageChangeAck_WIDTH                          1
#define GMMx774_VoltageChangeAck_MASK                           0x1
#define GMMx774_CurrentVoltageLevel_OFFSET                      1
#define GMMx774_CurrentVoltageLevel_WIDTH                       2
#define GMMx774_CurrentVoltageLevel_MASK                        0x6
#define GMMx774_Reserved_31_3_OFFSET                            3
#define GMMx774_Reserved_31_3_WIDTH                             29
#define GMMx774_Reserved_31_3_MASK                              0xfffffff8

/// GMMx774
typedef union {
  struct {                                                              ///<
    UINT32                                         VoltageChangeAck:1 ; ///<
    UINT32                                      CurrentVoltageLevel:2 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx774_STRUCT;

// **** GMMx15C0 Register Definition ****
// Address
#define GMMx15C0_ADDRESS                                        0x15c0

// Type
#define GMMx15C0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx15C0_OnDly_OFFSET                                   0
#define GMMx15C0_OnDly_WIDTH                                    6
#define GMMx15C0_OnDly_MASK                                     0x3f
#define GMMx15C0_OffDly_OFFSET                                  6
#define GMMx15C0_OffDly_WIDTH                                   6
#define GMMx15C0_OffDly_MASK                                    0xfc0
#define GMMx15C0_RdyDly_OFFSET                                  12
#define GMMx15C0_RdyDly_WIDTH                                   6
#define GMMx15C0_RdyDly_MASK                                    0x3f000
#define GMMx15C0_Enable_OFFSET                                  18
#define GMMx15C0_Enable_WIDTH                                   1
#define GMMx15C0_Enable_MASK                                    0x40000
#define GMMx15C0_Reserved_31_19_OFFSET                          19
#define GMMx15C0_Reserved_31_19_WIDTH                           13
#define GMMx15C0_Reserved_31_19_MASK                            0xfff80000

/// GMMx15C0
typedef union {
  struct {                                                              ///<
    UINT32                                                    OnDly:6 ; ///<
    UINT32                                                   OffDly:6 ; ///<
    UINT32                                                   RdyDly:6 ; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx15C0_STRUCT;





// **** GMMx2024 Register Definition ****
// Address
#define GMMx2024_ADDRESS                                        0x2024

// Type
#define GMMx2024_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2024_Base_OFFSET                                    0
#define GMMx2024_Base_WIDTH                                     16
#define GMMx2024_Base_MASK                                      0xffff
#define GMMx2024_Top_OFFSET                                     16
#define GMMx2024_Top_WIDTH                                      16
#define GMMx2024_Top_MASK                                       0xffff0000

/// GMMx2024
typedef union {
  struct {                                                              ///<
    UINT32                                                     Base:16; ///<
    UINT32                                                      Top:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2024_STRUCT;








































// **** GMMx2814 Register Definition ****
// Address
#define GMMx2814_ADDRESS                                        0x2814

// Type
#define GMMx2814_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2814_WriteClks_OFFSET                               0
#define GMMx2814_WriteClks_WIDTH                                9
#define GMMx2814_WriteClks_MASK                                 0x1ff
#define GMMx2814_UvdHarshPriority_OFFSET                        9
#define GMMx2814_UvdHarshPriority_WIDTH                         1
#define GMMx2814_UvdHarshPriority_MASK                          0x200
#define GMMx2814_Reserved_31_10_OFFSET                          10
#define GMMx2814_Reserved_31_10_WIDTH                           22
#define GMMx2814_Reserved_31_10_MASK                            0xfffffc00

/// GMMx2814
typedef union {
  struct {                                                              ///<
    UINT32                                                WriteClks:9 ; ///<
    UINT32                                         UvdHarshPriority:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2814_STRUCT;

// **** GMMx281C Register Definition ****
// Address
#define GMMx281C_ADDRESS                                        0x281c

// Type
#define GMMx281C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx281C_CSEnable_OFFSET                                0
#define GMMx281C_CSEnable_WIDTH                                 1
#define GMMx281C_CSEnable_MASK                                  0x1
#define GMMx281C_Reserved_4_1_OFFSET                            1
#define GMMx281C_Reserved_4_1_WIDTH                             4
#define GMMx281C_Reserved_4_1_MASK                              0x1e
#define GMMx281C_BaseAddr_21_13__OFFSET                         5
#define GMMx281C_BaseAddr_21_13__WIDTH                          9
#define GMMx281C_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx281C_Reserved_18_14_OFFSET                          14
#define GMMx281C_Reserved_18_14_WIDTH                           5
#define GMMx281C_Reserved_18_14_MASK                            0x7c000
#define GMMx281C_BaseAddr_36_27__OFFSET                         19
#define GMMx281C_BaseAddr_36_27__WIDTH                          10
#define GMMx281C_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx281C_Reserved_31_29_OFFSET                          29
#define GMMx281C_Reserved_31_29_WIDTH                           3
#define GMMx281C_Reserved_31_29_MASK                            0xe0000000

/// GMMx281C
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx281C_STRUCT;

// **** GMMx2820 Register Definition ****
// Address
#define GMMx2820_ADDRESS                                        0x2820

// Type
#define GMMx2820_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2820_CSEnable_OFFSET                                0
#define GMMx2820_CSEnable_WIDTH                                 1
#define GMMx2820_CSEnable_MASK                                  0x1
#define GMMx2820_Reserved_4_1_OFFSET                            1
#define GMMx2820_Reserved_4_1_WIDTH                             4
#define GMMx2820_Reserved_4_1_MASK                              0x1e
#define GMMx2820_BaseAddr_21_13__OFFSET                         5
#define GMMx2820_BaseAddr_21_13__WIDTH                          9
#define GMMx2820_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx2820_Reserved_18_14_OFFSET                          14
#define GMMx2820_Reserved_18_14_WIDTH                           5
#define GMMx2820_Reserved_18_14_MASK                            0x7c000
#define GMMx2820_BaseAddr_36_27__OFFSET                         19
#define GMMx2820_BaseAddr_36_27__WIDTH                          10
#define GMMx2820_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx2820_Reserved_31_29_OFFSET                          29
#define GMMx2820_Reserved_31_29_WIDTH                           3
#define GMMx2820_Reserved_31_29_MASK                            0xe0000000

/// GMMx2820
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2820_STRUCT;

// **** GMMx2824 Register Definition ****
// Address
#define GMMx2824_ADDRESS                                        0x2824

// Type
#define GMMx2824_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2824_CSEnable_OFFSET                                0
#define GMMx2824_CSEnable_WIDTH                                 1
#define GMMx2824_CSEnable_MASK                                  0x1
#define GMMx2824_Reserved_4_1_OFFSET                            1
#define GMMx2824_Reserved_4_1_WIDTH                             4
#define GMMx2824_Reserved_4_1_MASK                              0x1e
#define GMMx2824_BaseAddr_21_13__OFFSET                         5
#define GMMx2824_BaseAddr_21_13__WIDTH                          9
#define GMMx2824_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx2824_Reserved_18_14_OFFSET                          14
#define GMMx2824_Reserved_18_14_WIDTH                           5
#define GMMx2824_Reserved_18_14_MASK                            0x7c000
#define GMMx2824_BaseAddr_36_27__OFFSET                         19
#define GMMx2824_BaseAddr_36_27__WIDTH                          10
#define GMMx2824_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx2824_Reserved_31_29_OFFSET                          29
#define GMMx2824_Reserved_31_29_WIDTH                           3
#define GMMx2824_Reserved_31_29_MASK                            0xe0000000

/// GMMx2824
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2824_STRUCT;

// **** GMMx2828 Register Definition ****
// Address
#define GMMx2828_ADDRESS                                        0x2828

// Type
#define GMMx2828_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2828_CSEnable_OFFSET                                0
#define GMMx2828_CSEnable_WIDTH                                 1
#define GMMx2828_CSEnable_MASK                                  0x1
#define GMMx2828_Reserved_4_1_OFFSET                            1
#define GMMx2828_Reserved_4_1_WIDTH                             4
#define GMMx2828_Reserved_4_1_MASK                              0x1e
#define GMMx2828_BaseAddr_21_13__OFFSET                         5
#define GMMx2828_BaseAddr_21_13__WIDTH                          9
#define GMMx2828_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx2828_Reserved_18_14_OFFSET                          14
#define GMMx2828_Reserved_18_14_WIDTH                           5
#define GMMx2828_Reserved_18_14_MASK                            0x7c000
#define GMMx2828_BaseAddr_36_27__OFFSET                         19
#define GMMx2828_BaseAddr_36_27__WIDTH                          10
#define GMMx2828_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx2828_Reserved_31_29_OFFSET                          29
#define GMMx2828_Reserved_31_29_WIDTH                           3
#define GMMx2828_Reserved_31_29_MASK                            0xe0000000

/// GMMx2828
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2828_STRUCT;

// **** GMMx282C Register Definition ****
// Address
#define GMMx282C_ADDRESS                                        0x282c

// Type
#define GMMx282C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx282C_CSEnable_OFFSET                                0
#define GMMx282C_CSEnable_WIDTH                                 1
#define GMMx282C_CSEnable_MASK                                  0x1
#define GMMx282C_Reserved_4_1_OFFSET                            1
#define GMMx282C_Reserved_4_1_WIDTH                             4
#define GMMx282C_Reserved_4_1_MASK                              0x1e
#define GMMx282C_BaseAddr_21_13__OFFSET                         5
#define GMMx282C_BaseAddr_21_13__WIDTH                          9
#define GMMx282C_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx282C_Reserved_18_14_OFFSET                          14
#define GMMx282C_Reserved_18_14_WIDTH                           5
#define GMMx282C_Reserved_18_14_MASK                            0x7c000
#define GMMx282C_BaseAddr_36_27__OFFSET                         19
#define GMMx282C_BaseAddr_36_27__WIDTH                          10
#define GMMx282C_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx282C_Reserved_31_29_OFFSET                          29
#define GMMx282C_Reserved_31_29_WIDTH                           3
#define GMMx282C_Reserved_31_29_MASK                            0xe0000000

/// GMMx282C
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx282C_STRUCT;

// **** GMMx2830 Register Definition ****
// Address
#define GMMx2830_ADDRESS                                        0x2830

// Type
#define GMMx2830_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2830_CSEnable_OFFSET                                0
#define GMMx2830_CSEnable_WIDTH                                 1
#define GMMx2830_CSEnable_MASK                                  0x1
#define GMMx2830_Reserved_4_1_OFFSET                            1
#define GMMx2830_Reserved_4_1_WIDTH                             4
#define GMMx2830_Reserved_4_1_MASK                              0x1e
#define GMMx2830_BaseAddr_21_13__OFFSET                         5
#define GMMx2830_BaseAddr_21_13__WIDTH                          9
#define GMMx2830_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx2830_Reserved_18_14_OFFSET                          14
#define GMMx2830_Reserved_18_14_WIDTH                           5
#define GMMx2830_Reserved_18_14_MASK                            0x7c000
#define GMMx2830_BaseAddr_36_27__OFFSET                         19
#define GMMx2830_BaseAddr_36_27__WIDTH                          10
#define GMMx2830_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx2830_Reserved_31_29_OFFSET                          29
#define GMMx2830_Reserved_31_29_WIDTH                           3
#define GMMx2830_Reserved_31_29_MASK                            0xe0000000

/// GMMx2830
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2830_STRUCT;

// **** GMMx2834 Register Definition ****
// Address
#define GMMx2834_ADDRESS                                        0x2834

// Type
#define GMMx2834_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2834_CSEnable_OFFSET                                0
#define GMMx2834_CSEnable_WIDTH                                 1
#define GMMx2834_CSEnable_MASK                                  0x1
#define GMMx2834_Reserved_4_1_OFFSET                            1
#define GMMx2834_Reserved_4_1_WIDTH                             4
#define GMMx2834_Reserved_4_1_MASK                              0x1e
#define GMMx2834_BaseAddr_21_13__OFFSET                         5
#define GMMx2834_BaseAddr_21_13__WIDTH                          9
#define GMMx2834_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx2834_Reserved_18_14_OFFSET                          14
#define GMMx2834_Reserved_18_14_WIDTH                           5
#define GMMx2834_Reserved_18_14_MASK                            0x7c000
#define GMMx2834_BaseAddr_36_27__OFFSET                         19
#define GMMx2834_BaseAddr_36_27__WIDTH                          10
#define GMMx2834_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx2834_Reserved_31_29_OFFSET                          29
#define GMMx2834_Reserved_31_29_WIDTH                           3
#define GMMx2834_Reserved_31_29_MASK                            0xe0000000

/// GMMx2834
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2834_STRUCT;

// **** GMMx2838 Register Definition ****
// Address
#define GMMx2838_ADDRESS                                        0x2838

// Type
#define GMMx2838_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2838_CSEnable_OFFSET                                0
#define GMMx2838_CSEnable_WIDTH                                 1
#define GMMx2838_CSEnable_MASK                                  0x1
#define GMMx2838_Reserved_4_1_OFFSET                            1
#define GMMx2838_Reserved_4_1_WIDTH                             4
#define GMMx2838_Reserved_4_1_MASK                              0x1e
#define GMMx2838_BaseAddr_21_13__OFFSET                         5
#define GMMx2838_BaseAddr_21_13__WIDTH                          9
#define GMMx2838_BaseAddr_21_13__MASK                           0x3fe0
#define GMMx2838_Reserved_18_14_OFFSET                          14
#define GMMx2838_Reserved_18_14_WIDTH                           5
#define GMMx2838_Reserved_18_14_MASK                            0x7c000
#define GMMx2838_BaseAddr_36_27__OFFSET                         19
#define GMMx2838_BaseAddr_36_27__WIDTH                          10
#define GMMx2838_BaseAddr_36_27__MASK                           0x1ff80000
#define GMMx2838_Reserved_31_29_OFFSET                          29
#define GMMx2838_Reserved_31_29_WIDTH                           3
#define GMMx2838_Reserved_31_29_MASK                            0xe0000000

/// GMMx2838
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2838_STRUCT;

// **** GMMx283C Register Definition ****
// Address
#define GMMx283C_ADDRESS                                        0x283c

// Type
#define GMMx283C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx283C_Reserved_4_0_OFFSET                            0
#define GMMx283C_Reserved_4_0_WIDTH                             5
#define GMMx283C_Reserved_4_0_MASK                              0x1f
#define GMMx283C_AddrMask_21_13__OFFSET                         5
#define GMMx283C_AddrMask_21_13__WIDTH                          9
#define GMMx283C_AddrMask_21_13__MASK                           0x3fe0
#define GMMx283C_Reserved_18_14_OFFSET                          14
#define GMMx283C_Reserved_18_14_WIDTH                           5
#define GMMx283C_Reserved_18_14_MASK                            0x7c000
#define GMMx283C_AddrMask_36_27__OFFSET                         19
#define GMMx283C_AddrMask_36_27__WIDTH                          10
#define GMMx283C_AddrMask_36_27__MASK                           0x1ff80000
#define GMMx283C_Reserved_31_29_OFFSET                          29
#define GMMx283C_Reserved_31_29_WIDTH                           3
#define GMMx283C_Reserved_31_29_MASK                            0xe0000000

/// GMMx283C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx283C_STRUCT;

// **** GMMx2840 Register Definition ****
// Address
#define GMMx2840_ADDRESS                                        0x2840

// Type
#define GMMx2840_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2840_Reserved_4_0_OFFSET                            0
#define GMMx2840_Reserved_4_0_WIDTH                             5
#define GMMx2840_Reserved_4_0_MASK                              0x1f
#define GMMx2840_AddrMask_21_13__OFFSET                         5
#define GMMx2840_AddrMask_21_13__WIDTH                          9
#define GMMx2840_AddrMask_21_13__MASK                           0x3fe0
#define GMMx2840_Reserved_18_14_OFFSET                          14
#define GMMx2840_Reserved_18_14_WIDTH                           5
#define GMMx2840_Reserved_18_14_MASK                            0x7c000
#define GMMx2840_AddrMask_36_27__OFFSET                         19
#define GMMx2840_AddrMask_36_27__WIDTH                          10
#define GMMx2840_AddrMask_36_27__MASK                           0x1ff80000
#define GMMx2840_Reserved_31_29_OFFSET                          29
#define GMMx2840_Reserved_31_29_WIDTH                           3
#define GMMx2840_Reserved_31_29_MASK                            0xe0000000

/// GMMx2840
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2840_STRUCT;

// **** GMMx2844 Register Definition ****
// Address
#define GMMx2844_ADDRESS                                        0x2844

// Type
#define GMMx2844_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2844_Reserved_4_0_OFFSET                            0
#define GMMx2844_Reserved_4_0_WIDTH                             5
#define GMMx2844_Reserved_4_0_MASK                              0x1f
#define GMMx2844_AddrMask_21_13__OFFSET                         5
#define GMMx2844_AddrMask_21_13__WIDTH                          9
#define GMMx2844_AddrMask_21_13__MASK                           0x3fe0
#define GMMx2844_Reserved_18_14_OFFSET                          14
#define GMMx2844_Reserved_18_14_WIDTH                           5
#define GMMx2844_Reserved_18_14_MASK                            0x7c000
#define GMMx2844_AddrMask_36_27__OFFSET                         19
#define GMMx2844_AddrMask_36_27__WIDTH                          10
#define GMMx2844_AddrMask_36_27__MASK                           0x1ff80000
#define GMMx2844_Reserved_31_29_OFFSET                          29
#define GMMx2844_Reserved_31_29_WIDTH                           3
#define GMMx2844_Reserved_31_29_MASK                            0xe0000000

/// GMMx2844
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2844_STRUCT;

// **** GMMx2848 Register Definition ****
// Address
#define GMMx2848_ADDRESS                                        0x2848

// Type
#define GMMx2848_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2848_Reserved_4_0_OFFSET                            0
#define GMMx2848_Reserved_4_0_WIDTH                             5
#define GMMx2848_Reserved_4_0_MASK                              0x1f
#define GMMx2848_AddrMask_21_13__OFFSET                         5
#define GMMx2848_AddrMask_21_13__WIDTH                          9
#define GMMx2848_AddrMask_21_13__MASK                           0x3fe0
#define GMMx2848_Reserved_18_14_OFFSET                          14
#define GMMx2848_Reserved_18_14_WIDTH                           5
#define GMMx2848_Reserved_18_14_MASK                            0x7c000
#define GMMx2848_AddrMask_36_27__OFFSET                         19
#define GMMx2848_AddrMask_36_27__WIDTH                          10
#define GMMx2848_AddrMask_36_27__MASK                           0x1ff80000
#define GMMx2848_Reserved_31_29_OFFSET                          29
#define GMMx2848_Reserved_31_29_WIDTH                           3
#define GMMx2848_Reserved_31_29_MASK                            0xe0000000

/// GMMx2848
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_36_27_:10; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2848_STRUCT;

// **** GMMx284C Register Definition ****
// Address
#define GMMx284C_ADDRESS                                        0x284c

// Type
#define GMMx284C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx284C_Dimm0AddrMap_OFFSET                            0
#define GMMx284C_Dimm0AddrMap_WIDTH                             4
#define GMMx284C_Dimm0AddrMap_MASK                              0xf
#define GMMx284C_Dimm1AddrMap_OFFSET                            4
#define GMMx284C_Dimm1AddrMap_WIDTH                             4
#define GMMx284C_Dimm1AddrMap_MASK                              0xf0
#define GMMx284C_Reserved_15_8_OFFSET                           8
#define GMMx284C_Reserved_15_8_WIDTH                            8
#define GMMx284C_Reserved_15_8_MASK                             0xff00
#define GMMx284C_BankSwizzleMode_OFFSET                         16
#define GMMx284C_BankSwizzleMode_WIDTH                          1
#define GMMx284C_BankSwizzleMode_MASK                           0x10000
#define GMMx284C_Ddr3Mode_OFFSET                                17
#define GMMx284C_Ddr3Mode_WIDTH                                 1
#define GMMx284C_Ddr3Mode_MASK                                  0x20000
#define GMMx284C_BurstLength32_OFFSET                           18
#define GMMx284C_BurstLength32_WIDTH                            1
#define GMMx284C_BurstLength32_MASK                             0x40000
#define GMMx284C_BankSwap_OFFSET                                19
#define GMMx284C_BankSwap_WIDTH                                 1
#define GMMx284C_BankSwap_MASK                                  0x80000
#define GMMx284C_Reserved_31_20_OFFSET                          20
#define GMMx284C_Reserved_31_20_WIDTH                           12
#define GMMx284C_Reserved_31_20_MASK                            0xfff00000

/// GMMx284C
typedef union {
  struct {                                                              ///<
    UINT32                                             Dimm0AddrMap:4 ; ///<
    UINT32                                             Dimm1AddrMap:4 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                          BankSwizzleMode:1 ; ///<
    UINT32                                                 Ddr3Mode:1 ; ///<
    UINT32                                            BurstLength32:1 ; ///<
    UINT32                                                 BankSwap:1 ; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx284C_STRUCT;

// **** GMMx2850 Register Definition ****
// Address
#define GMMx2850_ADDRESS                                        0x2850

// Type
#define GMMx2850_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2850_Dimm0AddrMap_OFFSET                            0
#define GMMx2850_Dimm0AddrMap_WIDTH                             4
#define GMMx2850_Dimm0AddrMap_MASK                              0xf
#define GMMx2850_Dimm1AddrMap_OFFSET                            4
#define GMMx2850_Dimm1AddrMap_WIDTH                             4
#define GMMx2850_Dimm1AddrMap_MASK                              0xf0
#define GMMx2850_Reserved_15_8_OFFSET                           8
#define GMMx2850_Reserved_15_8_WIDTH                            8
#define GMMx2850_Reserved_15_8_MASK                             0xff00
#define GMMx2850_BankSwizzleMode_OFFSET                         16
#define GMMx2850_BankSwizzleMode_WIDTH                          1
#define GMMx2850_BankSwizzleMode_MASK                           0x10000
#define GMMx2850_Ddr3Mode_OFFSET                                17
#define GMMx2850_Ddr3Mode_WIDTH                                 1
#define GMMx2850_Ddr3Mode_MASK                                  0x20000
#define GMMx2850_BurstLength32_OFFSET                           18
#define GMMx2850_BurstLength32_WIDTH                            1
#define GMMx2850_BurstLength32_MASK                             0x40000
#define GMMx2850_BankSwap_OFFSET                                19
#define GMMx2850_BankSwap_WIDTH                                 1
#define GMMx2850_BankSwap_MASK                                  0x80000
#define GMMx2850_Reserved_31_20_OFFSET                          20
#define GMMx2850_Reserved_31_20_WIDTH                           12
#define GMMx2850_Reserved_31_20_MASK                            0xfff00000

/// GMMx2850
typedef union {
  struct {                                                              ///<
    UINT32                                             Dimm0AddrMap:4 ; ///<
    UINT32                                             Dimm1AddrMap:4 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                          BankSwizzleMode:1 ; ///<
    UINT32                                                 Ddr3Mode:1 ; ///<
    UINT32                                            BurstLength32:1 ; ///<
    UINT32                                                 BankSwap:1 ; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2850_STRUCT;

// **** GMMx2854 Register Definition ****
// Address
#define GMMx2854_ADDRESS                                        0x2854

// Type
#define GMMx2854_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2854_DctSelHiRngEn_OFFSET                           0
#define GMMx2854_DctSelHiRngEn_WIDTH                            1
#define GMMx2854_DctSelHiRngEn_MASK                             0x1
#define GMMx2854_DctSelHi_OFFSET                                1
#define GMMx2854_DctSelHi_WIDTH                                 1
#define GMMx2854_DctSelHi_MASK                                  0x2
#define GMMx2854_DctSelIntLvEn_OFFSET                           2
#define GMMx2854_DctSelIntLvEn_WIDTH                            1
#define GMMx2854_DctSelIntLvEn_MASK                             0x4
#define GMMx2854_Reserved_5_3_OFFSET                            3
#define GMMx2854_Reserved_5_3_WIDTH                             3
#define GMMx2854_Reserved_5_3_MASK                              0x38
#define GMMx2854_DctSelIntLvAddr_1_0__OFFSET                    6
#define GMMx2854_DctSelIntLvAddr_1_0__WIDTH                     2
#define GMMx2854_DctSelIntLvAddr_1_0__MASK                      0xc0
#define GMMx2854_Reserved_10_8_OFFSET                           8
#define GMMx2854_Reserved_10_8_WIDTH                            3
#define GMMx2854_Reserved_10_8_MASK                             0x700
#define GMMx2854_DctSelBaseAddr_39_27__OFFSET                   11
#define GMMx2854_DctSelBaseAddr_39_27__WIDTH                    13
#define GMMx2854_DctSelBaseAddr_39_27__MASK                     0xfff800
#define GMMx2854_Reserved_31_24_OFFSET                          24
#define GMMx2854_Reserved_31_24_WIDTH                           8
#define GMMx2854_Reserved_31_24_MASK                            0xff000000

/// GMMx2854
typedef union {
  struct {                                                              ///<
    UINT32                                            DctSelHiRngEn:1 ; ///<
    UINT32                                                 DctSelHi:1 ; ///<
    UINT32                                            DctSelIntLvEn:1 ; ///<
    UINT32                                             Reserved_5_3:3 ; ///<
    UINT32                                     DctSelIntLvAddr_1_0_:2 ; ///<
    UINT32                                            Reserved_10_8:3 ; ///<
    UINT32                                    DctSelBaseAddr_39_27_:13; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2854_STRUCT;

// **** GMMx2858 Register Definition ****
// Address
#define GMMx2858_ADDRESS                                        0x2858

// Type
#define GMMx2858_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2858_Reserved_8_0_OFFSET                            0
#define GMMx2858_Reserved_8_0_WIDTH                             9
#define GMMx2858_Reserved_8_0_MASK                              0x1ff
#define GMMx2858_DctSelIntLvAddr_2__OFFSET                      9
#define GMMx2858_DctSelIntLvAddr_2__WIDTH                       1
#define GMMx2858_DctSelIntLvAddr_2__MASK                        0x200
#define GMMx2858_DctSelBaseOffset_39_26__OFFSET                 10
#define GMMx2858_DctSelBaseOffset_39_26__WIDTH                  14
#define GMMx2858_DctSelBaseOffset_39_26__MASK                   0xfffc00
#define GMMx2858_Reserved_31_24_OFFSET                          24
#define GMMx2858_Reserved_31_24_WIDTH                           8
#define GMMx2858_Reserved_31_24_MASK                            0xff000000

/// GMMx2858
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                       DctSelIntLvAddr_2_:1 ; ///<
    UINT32                                  DctSelBaseOffset_39_26_:14; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2858_STRUCT;

// **** GMMx285C Register Definition ****
// Address
#define GMMx285C_ADDRESS                                        0x285c

// Type
#define GMMx285C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx285C_DramHoleValid_OFFSET                           0
#define GMMx285C_DramHoleValid_WIDTH                            1
#define GMMx285C_DramHoleValid_MASK                             0x1
#define GMMx285C_Reserved_6_1_OFFSET                            1
#define GMMx285C_Reserved_6_1_WIDTH                             6
#define GMMx285C_Reserved_6_1_MASK                              0x7e
#define GMMx285C_DramHoleOffset_31_23__OFFSET                   7
#define GMMx285C_DramHoleOffset_31_23__WIDTH                    9
#define GMMx285C_DramHoleOffset_31_23__MASK                     0xff80
#define GMMx285C_Reserved_23_16_OFFSET                          16
#define GMMx285C_Reserved_23_16_WIDTH                           8
#define GMMx285C_Reserved_23_16_MASK                            0xff0000
#define GMMx285C_DramHoleBase_31_24__OFFSET                     24
#define GMMx285C_DramHoleBase_31_24__WIDTH                      8
#define GMMx285C_DramHoleBase_31_24__MASK                       0xff000000

/// GMMx285C
typedef union {
  struct {                                                              ///<
    UINT32                                            DramHoleValid:1 ; ///<
    UINT32                                             Reserved_6_1:6 ; ///<
    UINT32                                    DramHoleOffset_31_23_:9 ; ///<
    UINT32                                           Reserved_23_16:8 ; ///<
    UINT32                                      DramHoleBase_31_24_:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx285C_STRUCT;





// **** GMMx2870 Register Definition ****
// Address
#define GMMx2870_ADDRESS                                        0x2870

// Type
#define GMMx2870_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2870_Base_OFFSET                                    0
#define GMMx2870_Base_WIDTH                                     20
#define GMMx2870_Base_MASK                                      0xfffff
#define GMMx2870_Reserved_31_20_OFFSET                          20
#define GMMx2870_Reserved_31_20_WIDTH                           12
#define GMMx2870_Reserved_31_20_MASK                            0xfff00000

/// GMMx2870
typedef union {
  struct {                                                              ///<
    UINT32                                                     Base:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2870_STRUCT;

// **** GMMx2874 Register Definition ****
// Address
#define GMMx2874_ADDRESS                                        0x2874

// Type
#define GMMx2874_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2874_Base_OFFSET                                    0
#define GMMx2874_Base_WIDTH                                     20
#define GMMx2874_Base_MASK                                      0xfffff
#define GMMx2874_Reserved_31_20_OFFSET                          20
#define GMMx2874_Reserved_31_20_WIDTH                           12
#define GMMx2874_Reserved_31_20_MASK                            0xfff00000

/// GMMx2874
typedef union {
  struct {                                                              ///<
    UINT32                                                     Base:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2874_STRUCT;


// **** GMMx287C Register Definition ****
// Address
#define GMMx287C_ADDRESS                                        0x287c

// Type
#define GMMx287C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx287C_Top_OFFSET                                     0
#define GMMx287C_Top_WIDTH                                      20
#define GMMx287C_Top_MASK                                       0xfffff
#define GMMx287C_Reserved_31_20_OFFSET                          20
#define GMMx287C_Reserved_31_20_WIDTH                           12
#define GMMx287C_Reserved_31_20_MASK                            0xfff00000

/// GMMx287C
typedef union {
  struct {                                                              ///<
    UINT32                                                      Top:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx287C_STRUCT;



// **** GMMx2888 Register Definition ****
// Address
#define GMMx2888_ADDRESS                                        0x2888

// Type
#define GMMx2888_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2888_Top_OFFSET                                     0
#define GMMx2888_Top_WIDTH                                      20
#define GMMx2888_Top_MASK                                       0xfffff
#define GMMx2888_Reserved_31_20_OFFSET                          20
#define GMMx2888_Reserved_31_20_WIDTH                           12
#define GMMx2888_Reserved_31_20_MASK                            0xfff00000

/// GMMx2888
typedef union {
  struct {                                                              ///<
    UINT32                                                      Top:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2888_STRUCT;






// **** GMMx28D8 Register Definition ****
// Address
#define GMMx28D8_ADDRESS                                        0x28d8

// Type
#define GMMx28D8_TYPE                                           TYPE_GMM
// Field Data
#define GMMx28D8_ActRd_OFFSET                                   0
#define GMMx28D8_ActRd_WIDTH                                    8
#define GMMx28D8_ActRd_MASK                                     0xff
#define GMMx28D8_ActWr_OFFSET                                   8
#define GMMx28D8_ActWr_WIDTH                                    8
#define GMMx28D8_ActWr_MASK                                     0xff00
#define GMMx28D8_RasMActRd_OFFSET                               16
#define GMMx28D8_RasMActRd_WIDTH                                8
#define GMMx28D8_RasMActRd_MASK                                 0xff0000
#define GMMx28D8_RasMActWr_OFFSET                               24
#define GMMx28D8_RasMActWr_WIDTH                                8
#define GMMx28D8_RasMActWr_MASK                                 0xff000000

/// GMMx28D8
typedef union {
  struct {                                                              ///<
    UINT32                                                    ActRd:8 ; ///<
    UINT32                                                    ActWr:8 ; ///<
    UINT32                                                RasMActRd:8 ; ///<
    UINT32                                                RasMActWr:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx28D8_STRUCT;







// **** GMMx2C04 Register Definition ****
// Address
#define GMMx2C04_ADDRESS                                        0x2c04

// Type
#define GMMx2C04_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2C04_NonsurfBase_OFFSET                             0
#define GMMx2C04_NonsurfBase_WIDTH                              28
#define GMMx2C04_NonsurfBase_MASK                               0xfffffff
#define GMMx2C04_Reserved_31_28_OFFSET                          28
#define GMMx2C04_Reserved_31_28_WIDTH                           4
#define GMMx2C04_Reserved_31_28_MASK                            0xf0000000

/// GMMx2C04
typedef union {
  struct {                                                              ///<
    UINT32                                              NonsurfBase:28; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2C04_STRUCT;

// **** GMMx5428 Register Definition ****
// Address
#define GMMx5428_ADDRESS                                        0x5428

// Type
#define GMMx5428_TYPE                                           TYPE_GMM
// Field Data
#define GMMx5428_ConfigMemsize_OFFSET                           0
#define GMMx5428_ConfigMemsize_WIDTH                            32
#define GMMx5428_ConfigMemsize_MASK                             0xffffffff

/// GMMx5428
typedef union {
  struct {                                                              ///<
    UINT32                                            ConfigMemsize:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx5428_STRUCT;

// **** GMMx5490 Register Definition ****
// Address
#define GMMx5490_ADDRESS                                        0x5490

// Type
#define GMMx5490_TYPE                                           TYPE_GMM
// Field Data
#define GMMx5490_FbReadEn_OFFSET                                0
#define GMMx5490_FbReadEn_WIDTH                                 1
#define GMMx5490_FbReadEn_MASK                                  0x1
#define GMMx5490_FbWriteEn_OFFSET                               1
#define GMMx5490_FbWriteEn_WIDTH                                1
#define GMMx5490_FbWriteEn_MASK                                 0x2
#define GMMx5490_Reserved_31_2_OFFSET                           2
#define GMMx5490_Reserved_31_2_WIDTH                            30
#define GMMx5490_Reserved_31_2_MASK                             0xfffffffc

/// GMMx5490
typedef union {
  struct {                                                              ///<
    UINT32                                                 FbReadEn:1 ; ///<
    UINT32                                                FbWriteEn:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx5490_STRUCT;


/// SMUx73
typedef union {
  struct {                                                              ///<
    UINT32                                            DisLclkGating:1 ; ///<
    UINT32                                            DisSclkGating:1 ; ///<
    UINT32                                            Reserved_15_2:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx73_STRUCT;

// **** MSRC001_001A Register Definition ****
// Address
#define MSRC001_001A_ADDRESS                                    0xc001001a

// Type
#define MSRC001_001A_TYPE                                       TYPE_MSR
// Field Data
#define MSRC001_001A_RAZ_22_0_OFFSET                            0
#define MSRC001_001A_RAZ_22_0_WIDTH                             23
#define MSRC001_001A_RAZ_22_0_MASK                              0x7fffff
#define MSRC001_001A_TOM_39_23__OFFSET                          23
#define MSRC001_001A_TOM_39_23__WIDTH                           17
#define MSRC001_001A_TOM_39_23__MASK                            0xffff800000
#define MSRC001_001A_MBZ_47_40_OFFSET                           40
#define MSRC001_001A_MBZ_47_40_WIDTH                            8
#define MSRC001_001A_MBZ_47_40_MASK                             0xff0000000000
#define MSRC001_001A_RAZ_63_48_OFFSET                           48
#define MSRC001_001A_RAZ_63_48_WIDTH                            16
#define MSRC001_001A_RAZ_63_48_MASK                             0xffff000000000000

/// MSRC001_001A
typedef union {
  struct {                                                              ///<
    UINT64                                                 RAZ_22_0:23; ///<
    UINT64                                               TOM_39_23_:17; ///<
    UINT64                                                MBZ_47_40:8 ; ///<
    UINT64                                                RAZ_63_48:16; ///<
  } Field;                                                              ///<
  UINT64 Value;                                                         ///<
} MSRC001_001A_STRUCT;







// **** D0F0xE4_WRAP_8013 Register Definition ****
// Address
#define D0F0xE4_WRAP_8013_ADDRESS                               0x8013

// Field Data
#define D0F0xE4_WRAP_8013_MasterPciePllA_OFFSET                 0
#define D0F0xE4_WRAP_8013_MasterPciePllA_WIDTH                  1
#define D0F0xE4_WRAP_8013_MasterPciePllA_MASK                   0x1
#define D0F0xE4_WRAP_8013_Reserved_1_1_OFFSET                   1
#define D0F0xE4_WRAP_8013_Reserved_1_1_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_1_1_MASK                     0x2
#define D0F0xE4_WRAP_8013_Reserved_2_2_OFFSET                   2
#define D0F0xE4_WRAP_8013_Reserved_2_2_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_2_2_MASK                     0x4
#define D0F0xE4_WRAP_8013_Reserved_3_3_OFFSET                   3
#define D0F0xE4_WRAP_8013_Reserved_3_3_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_3_3_MASK                     0x8
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideA_OFFSET       4
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideA_WIDTH        1
#define D0F0xE4_WRAP_8013_ClkDividerResetOverrideA_MASK         0x10
#define D0F0xE4_WRAP_8013_Reserved_5_5_OFFSET                   5
#define D0F0xE4_WRAP_8013_Reserved_5_5_WIDTH                    1
#define D0F0xE4_WRAP_8013_Reserved_5_5_MASK                     0x20
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
#define D0F0xE4_WRAP_8013_Reserved_10_10_OFFSET                 10
#define D0F0xE4_WRAP_8013_Reserved_10_10_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_10_10_MASK                   0x400
#define D0F0xE4_WRAP_8013_Reserved_11_11_OFFSET                 11
#define D0F0xE4_WRAP_8013_Reserved_11_11_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_11_11_MASK                   0x800
#define D0F0xE4_WRAP_8013_Reserved_12_12_OFFSET                 12
#define D0F0xE4_WRAP_8013_Reserved_12_12_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_12_12_MASK                   0x1000
#define D0F0xE4_WRAP_8013_Reserved_15_13_OFFSET                 13
#define D0F0xE4_WRAP_8013_Reserved_15_13_WIDTH                  3
#define D0F0xE4_WRAP_8013_Reserved_15_13_MASK                   0xe000
#define D0F0xE4_WRAP_8013_Reserved_16_16_OFFSET                 16
#define D0F0xE4_WRAP_8013_Reserved_16_16_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_16_16_MASK                   0x10000
#define D0F0xE4_WRAP_8013_Reserved_19_17_OFFSET                 17
#define D0F0xE4_WRAP_8013_Reserved_19_17_WIDTH                  3
#define D0F0xE4_WRAP_8013_Reserved_19_17_MASK                   0xe0000
#define D0F0xE4_WRAP_8013_Reserved_20_20_OFFSET                 20
#define D0F0xE4_WRAP_8013_Reserved_20_20_WIDTH                  1
#define D0F0xE4_WRAP_8013_Reserved_20_20_MASK                   0x100000
#define D0F0xE4_WRAP_8013_Reserved_31_21_OFFSET                 21
#define D0F0xE4_WRAP_8013_Reserved_31_21_WIDTH                  11
#define D0F0xE4_WRAP_8013_Reserved_31_21_MASK                   0xffe00000

/// D0F0xE4_WRAP_8013
typedef union {
  struct {                                                              ///<
    UINT32                                           MasterPciePllA:1 ; ///<
    UINT32                                           MasterPciePllB:1 ; ///<
    UINT32                                           MasterPciePllC:1 ; ///<
    UINT32                                           MasterPciePllD:1 ; ///<
    UINT32                                 ClkDividerResetOverrideA:1 ; ///<
    UINT32                                             Reserved_5_5:1 ; ///<
    UINT32                                             Reserved_6_6:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                     TxclkSelCoreOverride:1 ; ///<
    UINT32                                     TxclkSelPifAOverride:1 ; ///<
    UINT32                                           Reserved_10_10:1 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                           Reserved_12_12:1 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                           Reserved_16_16:1 ; ///<
    UINT32                                           Reserved_19_17:3 ; ///<
    UINT32                                           Reserved_20_20:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8013_STRUCT;

// **** D0F0xE4_WRAP_8014 Register Definition ****
// Address
#define D0F0xE4_WRAP_8014_ADDRESS                               0x8014

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
#define D0F0xE4_WRAP_8014_Reserved_6_6_OFFSET                   6
#define D0F0xE4_WRAP_8014_Reserved_6_6_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_6_6_MASK                     0x40
#define D0F0xE4_WRAP_8014_Reserved_7_7_OFFSET                   7
#define D0F0xE4_WRAP_8014_Reserved_7_7_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_7_7_MASK                     0x80
#define D0F0xE4_WRAP_8014_Reserved_8_8_OFFSET                   8
#define D0F0xE4_WRAP_8014_Reserved_8_8_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_8_8_MASK                     0x100
#define D0F0xE4_WRAP_8014_Reserved_9_9_OFFSET                   9
#define D0F0xE4_WRAP_8014_Reserved_9_9_WIDTH                    1
#define D0F0xE4_WRAP_8014_Reserved_9_9_MASK                     0x200
#define D0F0xE4_WRAP_8014_Reserved_10_10_OFFSET                 10
#define D0F0xE4_WRAP_8014_Reserved_10_10_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_10_10_MASK                   0x400
#define D0F0xE4_WRAP_8014_Reserved_11_11_OFFSET                 11
#define D0F0xE4_WRAP_8014_Reserved_11_11_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_11_11_MASK                   0x800
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_OFFSET           12
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_WIDTH            1
#define D0F0xE4_WRAP_8014_PcieGatePifA1xEnable_MASK             0x1000
#define D0F0xE4_WRAP_8014_Reserved_13_13_OFFSET                 13
#define D0F0xE4_WRAP_8014_Reserved_13_13_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_13_13_MASK                   0x2000
#define D0F0xE4_WRAP_8014_Reserved_14_14_OFFSET                 14
#define D0F0xE4_WRAP_8014_Reserved_14_14_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_14_14_MASK                   0x4000
#define D0F0xE4_WRAP_8014_Reserved_15_15_OFFSET                 15
#define D0F0xE4_WRAP_8014_Reserved_15_15_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_15_15_MASK                   0x8000
#define D0F0xE4_WRAP_8014_PcieGatePifA2p5xEnable_OFFSET         16
#define D0F0xE4_WRAP_8014_PcieGatePifA2p5xEnable_WIDTH          1
#define D0F0xE4_WRAP_8014_PcieGatePifA2p5xEnable_MASK           0x10000
#define D0F0xE4_WRAP_8014_Reserved_17_17_OFFSET                 17
#define D0F0xE4_WRAP_8014_Reserved_17_17_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_17_17_MASK                   0x20000
#define D0F0xE4_WRAP_8014_Reserved_18_18_OFFSET                 18
#define D0F0xE4_WRAP_8014_Reserved_18_18_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_18_18_MASK                   0x40000
#define D0F0xE4_WRAP_8014_Reserved_19_19_OFFSET                 19
#define D0F0xE4_WRAP_8014_Reserved_19_19_WIDTH                  1
#define D0F0xE4_WRAP_8014_Reserved_19_19_MASK                   0x80000
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_OFFSET  20
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_WIDTH   1
#define D0F0xE4_WRAP_8014_TxclkPermGateOnlyWhenPllPwrDn_MASK    0x100000
#define D0F0xE4_WRAP_8014_Reserved_31_21_OFFSET                 21
#define D0F0xE4_WRAP_8014_Reserved_31_21_WIDTH                  11
#define D0F0xE4_WRAP_8014_Reserved_31_21_MASK                   0xffe00000

/// D0F0xE4_WRAP_8014
typedef union {
  struct {
    UINT32                                      TxclkPermGateEnable:1 ; ///<
    UINT32                                      TxclkPrbsGateEnable:1 ; ///<
    UINT32                                      DdiGatePifA1xEnable:1 ; ///<
    UINT32                                      DdiGatePifB1xEnable:1 ; ///<
    UINT32                                      DdiGatePifC1xEnable:1 ; ///<
    UINT32                                      DdiGatePifD1xEnable:1 ; ///<
    UINT32                                        DdiGateDigAEnable:1 ; ///<
    UINT32                                        DdiGateDigBEnable:1 ; ///<
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
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8014_STRUCT;





// **** GMMx6124 Register Definition ****
// Address
#define GMMx6124_ADDRESS                                        0x6124

// **** GMMx6124 Register Definition ****
// Address
#define GMMx6124_ADDRESS                                        0x6124

// Type
#define GMMx6124_TYPE                                           TYPE_GMM
// Field Data
#define GMMx6124_DoutScratch_OFFSET                             0
#define GMMx6124_DoutScratch_WIDTH                              32
#define GMMx6124_DoutScratch_MASK                               0xffffffff








// **** D0F0xE4_CORE_0020 Register Definition ****
// Address
#define D0F0xE4_CORE_0020_ADDRESS                               0x20

// Type
#define D0F0xE4_CORE_0020_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_0020_Reserved_1_0_OFFSET                   0
#define D0F0xE4_CORE_0020_Reserved_1_0_WIDTH                    2
#define D0F0xE4_CORE_0020_Reserved_1_0_MASK                     0x3
#define D0F0xE4_CORE_0020_Reserved_31_12_OFFSET                 12
#define D0F0xE4_CORE_0020_Reserved_31_12_WIDTH                  20
#define D0F0xE4_CORE_0020_Reserved_31_12_MASK                   0xfffff000

/// D0F0xE4_CORE_0020
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                       :1 ; ///<
    UINT32                                             :1 ; ///<
    UINT32                                         :2 ; ///<
    UINT32                                         :1 ; ///<
    UINT32                                          :1 ; ///<
    UINT32                                         :1 ; ///<
    UINT32                                        :1 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0020_STRUCT;

// **** D0F0xE4_CORE_0010 Register Definition ****
// Address
#define D0F0xE4_CORE_0010_ADDRESS                               0x10

// Type
#define D0F0xE4_CORE_0010_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_0010_HwInitWrLock_OFFSET                   0
#define D0F0xE4_CORE_0010_HwInitWrLock_WIDTH                    1
#define D0F0xE4_CORE_0010_HwInitWrLock_MASK                     0x1
#define D0F0xE4_CORE_0010_LcHotPlugDelSel_OFFSET                1
#define D0F0xE4_CORE_0010_LcHotPlugDelSel_WIDTH                 3
#define D0F0xE4_CORE_0010_LcHotPlugDelSel_MASK                  0xe
#define D0F0xE4_CORE_0010_Reserved_6_4_OFFSET                   4
#define D0F0xE4_CORE_0010_Reserved_6_4_WIDTH                    3
#define D0F0xE4_CORE_0010_Reserved_6_4_MASK                     0x70

/// D0F0xE4_CORE_0010
typedef union {
  struct {                                                              ///<
    UINT32                                             HwInitWrLock:1 ; ///<
    UINT32                                          LcHotPlugDelSel:3 ; ///<
    UINT32                                             Reserved_6_4:3 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                             :1 ; ///<
    UINT32                                            :1 ; ///<
    UINT32                                      :3 ; ///<
    UINT32                                           :3 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                      :1 ; ///<
    UINT32                                         :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                     :1 ; ///<
    UINT32                                        :1 ; ///<
    UINT32                                     :1 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                               :6 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                      :1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0010_STRUCT;


// **** D0F0x98_x0C Register Definition ****
// Address
#define D0F0x98_x0C_ADDRESS                                     0xc

// Type
#define D0F0x98_x0C_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x0C_GcmWrrLenA_OFFSET                           0
#define D0F0x98_x0C_GcmWrrLenA_WIDTH                            8
#define D0F0x98_x0C_GcmWrrLenA_MASK                             0xff
#define D0F0x98_x0C_GcmWrrLenB_OFFSET                           8
#define D0F0x98_x0C_GcmWrrLenB_WIDTH                            8
#define D0F0x98_x0C_GcmWrrLenB_MASK                             0xff00
#define D0F0x98_x0C_Reserved_29_16_OFFSET                       16
#define D0F0x98_x0C_Reserved_29_16_WIDTH                        14
#define D0F0x98_x0C_Reserved_29_16_MASK                         0x3fff0000
#define D0F0x98_x0C_StrictSelWinnerEn_OFFSET                    30
#define D0F0x98_x0C_StrictSelWinnerEn_WIDTH                     1
#define D0F0x98_x0C_StrictSelWinnerEn_MASK                      0x40000000

/// D0F0x98_x0C
typedef union {
  struct {                                                              ///<
    UINT32                                               GcmWrrLenA:8 ; ///<
    UINT32                                               GcmWrrLenB:8 ; ///<
    UINT32                                           Reserved_29_16:14; ///<
    UINT32                                        StrictSelWinnerEn:1 ; ///<
    UINT32                                             :1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x0C_STRUCT;





















// **** D0F0xE4_WRAP_8063 Register Definition ****
// Address
#define D0F0xE4_WRAP_8063_ADDRESS                               0x8063

// Type
#define D0F0xE4_WRAP_8063_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8063_Reserved_0_0_OFFSET                   0
#define D0F0xE4_WRAP_8063_Reserved_0_0_WIDTH                    1
#define D0F0xE4_WRAP_8063_Reserved_0_0_MASK                     0x1
#define D0F0xE4_WRAP_8063_Reserved_25_25_OFFSET                 25
#define D0F0xE4_WRAP_8063_Reserved_25_25_WIDTH                  1
#define D0F0xE4_WRAP_8063_Reserved_25_25_MASK                   0x2000000
#define D0F0xE4_WRAP_8063_Reserved_27_26_OFFSET                 26
#define D0F0xE4_WRAP_8063_Reserved_27_26_WIDTH                  2
#define D0F0xE4_WRAP_8063_Reserved_27_26_MASK                   0xc000000
#define D0F0xE4_WRAP_8063_Reserved_31_28_OFFSET                 28
#define D0F0xE4_WRAP_8063_Reserved_31_28_WIDTH                  4
#define D0F0xE4_WRAP_8063_Reserved_31_28_MASK                   0xf0000000

/// D0F0xE4_WRAP_8063
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    line331:1 ; ///<
    UINT32    line332:1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :2 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    line338:1 ; ///<
    UINT32    line339:1 ; ///<
    UINT32    line340:1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :2 ; ///<
    UINT32    :1 ; ///<
    UINT32    :1 ; ///<
    UINT32    :2 ; ///<
    UINT32    :1 ; ///<
    UINT32                                           Reserved_25_25:1 ; ///<
    UINT32                                           Reserved_27_26:2 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8063_STRUCT;

// **** D0F0xE4_WRAP_8015 Register Definition ****
// Address
#define D0F0xE4_WRAP_8015_ADDRESS                               0x8015

// Type
#define D0F0xE4_WRAP_8015_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8015_EnableD0StateReport_OFFSET            0
#define D0F0xE4_WRAP_8015_EnableD0StateReport_WIDTH             1
#define D0F0xE4_WRAP_8015_EnableD0StateReport_MASK              0x1
#define D0F0xE4_WRAP_8015_Reserved_1_1_OFFSET                   1
#define D0F0xE4_WRAP_8015_Reserved_1_1_WIDTH                    1
#define D0F0xE4_WRAP_8015_Reserved_1_1_MASK                     0x2
#define D0F0xE4_WRAP_8015_SlowRefclkThroughTxclk2p5x_OFFSET     2
#define D0F0xE4_WRAP_8015_SlowRefclkThroughTxclk2p5x_WIDTH      1
#define D0F0xE4_WRAP_8015_SlowRefclkThroughTxclk2p5x_MASK       0x4
#define D0F0xE4_WRAP_8015_SlowRefclkEnableTxclk2p5x_OFFSET      3
#define D0F0xE4_WRAP_8015_SlowRefclkEnableTxclk2p5x_WIDTH       1
#define D0F0xE4_WRAP_8015_SlowRefclkEnableTxclk2p5x_MASK        0x8
#define D0F0xE4_WRAP_8015_SlowRefclkDivideTxclk2p5x_OFFSET      4
#define D0F0xE4_WRAP_8015_SlowRefclkDivideTxclk2p5x_WIDTH       2
#define D0F0xE4_WRAP_8015_SlowRefclkDivideTxclk2p5x_MASK        0x30
#define D0F0xE4_WRAP_8015_SlowRefclkBurstTxclk2p5x_OFFSET       6
#define D0F0xE4_WRAP_8015_SlowRefclkBurstTxclk2p5x_WIDTH        2
#define D0F0xE4_WRAP_8015_SlowRefclkBurstTxclk2p5x_MASK         0xc0
#define D0F0xE4_WRAP_8015_Reserved_8_8_OFFSET                   8
#define D0F0xE4_WRAP_8015_Reserved_8_8_WIDTH                    1
#define D0F0xE4_WRAP_8015_Reserved_8_8_MASK                     0x100
#define D0F0xE4_WRAP_8015_SlowRefclkLcntGateForce_OFFSET        9
#define D0F0xE4_WRAP_8015_SlowRefclkLcntGateForce_WIDTH         1
#define D0F0xE4_WRAP_8015_SlowRefclkLcntGateForce_MASK          0x200
#define D0F0xE4_WRAP_8015_SlowRefclkThroughTxclk1x_OFFSET       10
#define D0F0xE4_WRAP_8015_SlowRefclkThroughTxclk1x_WIDTH        1
#define D0F0xE4_WRAP_8015_SlowRefclkThroughTxclk1x_MASK         0x400
#define D0F0xE4_WRAP_8015_SlowRefclkEnableTxclk1x_OFFSET        11
#define D0F0xE4_WRAP_8015_SlowRefclkEnableTxclk1x_WIDTH         1
#define D0F0xE4_WRAP_8015_SlowRefclkEnableTxclk1x_MASK          0x800
#define D0F0xE4_WRAP_8015_SlowRefclkDivideTxclk1x_OFFSET        12
#define D0F0xE4_WRAP_8015_SlowRefclkDivideTxclk1x_WIDTH         2
#define D0F0xE4_WRAP_8015_SlowRefclkDivideTxclk1x_MASK          0x3000
#define D0F0xE4_WRAP_8015_SlowRefclkBurstTxclk1x_OFFSET         14
#define D0F0xE4_WRAP_8015_SlowRefclkBurstTxclk1x_WIDTH          2
#define D0F0xE4_WRAP_8015_SlowRefclkBurstTxclk1x_MASK           0xc000
#define D0F0xE4_WRAP_8015_RefclkRegsGateLatency_OFFSET          16
#define D0F0xE4_WRAP_8015_RefclkRegsGateLatency_WIDTH           6
#define D0F0xE4_WRAP_8015_RefclkRegsGateLatency_MASK            0x3f0000
#define D0F0xE4_WRAP_8015_Reserved_22_22_OFFSET                 22
#define D0F0xE4_WRAP_8015_Reserved_22_22_WIDTH                  1
#define D0F0xE4_WRAP_8015_Reserved_22_22_MASK                   0x400000
#define D0F0xE4_WRAP_8015_RefclkRegsGateEnable_OFFSET           23
#define D0F0xE4_WRAP_8015_RefclkRegsGateEnable_WIDTH            1
#define D0F0xE4_WRAP_8015_RefclkRegsGateEnable_MASK             0x800000
#define D0F0xE4_WRAP_8015_RefclkBphyGateLatency_OFFSET          24
#define D0F0xE4_WRAP_8015_RefclkBphyGateLatency_WIDTH           6
#define D0F0xE4_WRAP_8015_RefclkBphyGateLatency_MASK            0x3f000000
#define D0F0xE4_WRAP_8015_Reserved_30_30_OFFSET                 30
#define D0F0xE4_WRAP_8015_Reserved_30_30_WIDTH                  1
#define D0F0xE4_WRAP_8015_Reserved_30_30_MASK                   0x40000000
#define D0F0xE4_WRAP_8015_RefclkBphyGateEnable_OFFSET           31
#define D0F0xE4_WRAP_8015_RefclkBphyGateEnable_WIDTH            1
#define D0F0xE4_WRAP_8015_RefclkBphyGateEnable_MASK             0x80000000

/// D0F0xE4_WRAP_8015
typedef union {
  struct {                                                              ///<
    UINT32                                      EnableD0StateReport:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                               SlowRefclkThroughTxclk2p5x:1 ; ///<
    UINT32                                SlowRefclkEnableTxclk2p5x:1 ; ///<
    UINT32                                SlowRefclkDivideTxclk2p5x:2 ; ///<
    UINT32                                 SlowRefclkBurstTxclk2p5x:2 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                  SlowRefclkLcntGateForce:1 ; ///<
    UINT32                                 SlowRefclkThroughTxclk1x:1 ; ///<
    UINT32                                  SlowRefclkEnableTxclk1x:1 ; ///<
    UINT32                                  SlowRefclkDivideTxclk1x:2 ; ///<
    UINT32                                   SlowRefclkBurstTxclk1x:2 ; ///<
    UINT32                                    RefclkRegsGateLatency:6 ; ///<
    UINT32                                           Reserved_22_22:1 ; ///<
    UINT32                                     RefclkRegsGateEnable:1 ; ///<
    UINT32                                    RefclkBphyGateLatency:6 ; ///<
    UINT32                                           Reserved_30_30:1 ; ///<
    UINT32                                     RefclkBphyGateEnable:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8015_STRUCT;

// **** DxF0xE4_xB5 Register Definition ****
// Address
#define DxF0xE4_xB5_ADDRESS                                     0xb5

// Type
#define DxF0xE4_xB5_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xB5_LcSelectDeemphasis_OFFSET                   0
#define DxF0xE4_xB5_LcSelectDeemphasis_WIDTH                    1
#define DxF0xE4_xB5_LcSelectDeemphasis_MASK                     0x1
#define DxF0xE4_xB5_LcSelectDeemphasisCntl_OFFSET               1
#define DxF0xE4_xB5_LcSelectDeemphasisCntl_WIDTH                2
#define DxF0xE4_xB5_LcSelectDeemphasisCntl_MASK                 0x6
#define DxF0xE4_xB5_LcRcvdDeemphasis_OFFSET                     3
#define DxF0xE4_xB5_LcRcvdDeemphasis_WIDTH                      1
#define DxF0xE4_xB5_LcRcvdDeemphasis_MASK                       0x8
#define DxF0xE4_xB5_Reserved_31_23_OFFSET                       23
#define DxF0xE4_xB5_Reserved_31_23_WIDTH                        9
#define DxF0xE4_xB5_Reserved_31_23_MASK                         0xff800000

/// DxF0xE4_xB5
typedef union {
  struct {                                                              ///<
    UINT32                                       LcSelectDeemphasis:1 ; ///<
    UINT32                                   LcSelectDeemphasisCntl:2 ; ///<
    UINT32                                         LcRcvdDeemphasis:1 ; ///<
    UINT32  :1 ; ///<
    UINT32  :1 ; ///<
    UINT32  :2 ; ///<
    UINT32  :1 ; ///<
    UINT32  :1 ; ///<
    UINT32  line519:1 ; ///<
    UINT32  :1 ; ///<
    UINT32  line521:2 ; ///<
    UINT32  line522:2 ; ///<
    UINT32  :1 ; ///<
    UINT32  :1 ; ///<
    UINT32  :1 ; ///<
    UINT32  :2 ; ///<
    UINT32  :1 ; ///<
    UINT32  :1 ; ///<
    UINT32                                           Reserved_31_23:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xB5_STRUCT;


// **** D0F0xE4_PHY_6006 Register Definition ****
// Address
#define D0F0xE4_PHY_6006_ADDRESS                                0x6006

// Type
#define D0F0xE4_PHY_6006_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_6006_TxMarginNom_OFFSET                     0
#define D0F0xE4_PHY_6006_TxMarginNom_WIDTH                      8
#define D0F0xE4_PHY_6006_TxMarginNom_MASK                       0xff
#define D0F0xE4_PHY_6006_DeemphGen1Nom_OFFSET                   8
#define D0F0xE4_PHY_6006_DeemphGen1Nom_WIDTH                    8
#define D0F0xE4_PHY_6006_DeemphGen1Nom_MASK                     0xff00
#define D0F0xE4_PHY_6006_Deemph35Gen2Nom_OFFSET                 16
#define D0F0xE4_PHY_6006_Deemph35Gen2Nom_WIDTH                  8
#define D0F0xE4_PHY_6006_Deemph35Gen2Nom_MASK                   0xff0000
#define D0F0xE4_PHY_6006_Deemph60Gen2Nom_OFFSET                 24
#define D0F0xE4_PHY_6006_Deemph60Gen2Nom_WIDTH                  8
#define D0F0xE4_PHY_6006_Deemph60Gen2Nom_MASK                   0xff000000

/// D0F0xE4_PHY_6006
typedef union {
  struct {                                                              ///<
    UINT32                                              TxMarginNom:8 ; ///<
    UINT32                                            DeemphGen1Nom:8 ; ///<
    UINT32                                          Deemph35Gen2Nom:8 ; ///<
    UINT32                                          Deemph60Gen2Nom:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_6006_STRUCT;



// **** D0F0x64_x1C Register Definition ****
// Address
#define D0F0x64_x1C_ADDRESS                                     0x1c

// Type
#define D0F0x64_x1C_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x1C_WriteDis_OFFSET                             0
#define D0F0x64_x1C_WriteDis_WIDTH                              1
#define D0F0x64_x1C_WriteDis_MASK                               0x1
#define D0F0x64_x1C_F0NonlegacyDeviceTypeEn_OFFSET              1
#define D0F0x64_x1C_F0NonlegacyDeviceTypeEn_WIDTH               1
#define D0F0x64_x1C_F0NonlegacyDeviceTypeEn_MASK                0x2
#define D0F0x64_x1C_F064BarEn_OFFSET                            2
#define D0F0x64_x1C_F064BarEn_WIDTH                             1
#define D0F0x64_x1C_F064BarEn_MASK                              0x4
#define D0F0x64_x1C_MemApSize_OFFSET                            3
#define D0F0x64_x1C_MemApSize_WIDTH                             3
#define D0F0x64_x1C_MemApSize_MASK                              0x38
#define D0F0x64_x1C_RegApSize_OFFSET                            6
#define D0F0x64_x1C_RegApSize_WIDTH                             1
#define D0F0x64_x1C_RegApSize_MASK                              0x40

/// D0F0x64_x1C
typedef union {
  struct {                                                              ///<
    UINT32                                                 WriteDis:1 ; ///<
    UINT32                                  F0NonlegacyDeviceTypeEn:1 ; ///<
    UINT32                                                F064BarEn:1 ; ///<
    UINT32                                                MemApSize:3 ; ///<
    UINT32                                                RegApSize:1 ; ///<
    UINT32  /*                                        DualfuncDisplayEn*/:1 ; ///<
    UINT32  /*                                                  AudioEn*/:1 ; ///<
    UINT32  /*                                                   MsiDis*/:1 ; ///<
    UINT32  /*                               AudioNonlegacyDeviceTypeEn*/:1 ; ///<
    UINT32  /*                                             Audio64BarEn*/:1 ; ///<
    UINT32  /*                                                   VgaDis*/:1 ; ///<
    UINT32  /*                                               FbAlwaysOn*/:1 ; ///<
    UINT32  /*                                             FbCplTypeSel*/:2 ; ///<
    UINT32  /*                                                 IoBarDis*/:1 ; ///<
    UINT32  /*                                                     F0En*/:1 ; ///<
    UINT32  /*                                                  F0BarEn*/:1 ; ///<
    UINT32  /*                                                  F1BarEn*/:1 ; ///<
    UINT32  /*                                                  F2BarEn*/:1 ; ///<
    UINT32  /*                                                  PcieDis*/:1 ; ///<
    UINT32  /*                                          BifBxcntlSpare0*/:1 ; ///<
    UINT32  /*                                                   RcieEn*/:1 ; ///<
    UINT32  /*                                           BifBxcntlSpare*/:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x1C_STRUCT;

// **** GMMx00 Register Definition ****
// Address
#define GMMx00_ADDRESS                                          0x0

// Type
#define GMMx00_TYPE                                             TYPE_GMM
// Field Data
#define GMMx00_Offset_OFFSET                                    0
#define GMMx00_Offset_WIDTH                                     31
#define GMMx00_Offset_MASK                                      0x7fffffff
#define GMMx00_Aper_OFFSET                                      31
#define GMMx00_Aper_WIDTH                                       1
#define GMMx00_Aper_MASK                                        0x80000000

/// GMMx00
typedef union {
  struct {                                                              ///<
    UINT32                                                   Offset:31; ///<
    UINT32                                                     Aper:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx00_STRUCT;

// **** GMMx04 Register Definition ****
// Address
#define GMMx04_ADDRESS                                          0x4

// Type
#define GMMx04_TYPE                                             TYPE_GMM
// Field Data
#define GMMx04_Data_OFFSET                                      0
#define GMMx04_Data_WIDTH                                       32
#define GMMx04_Data_MASK                                        0xffffffff

/// GMMx04
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx04_STRUCT;



// **** D18F2x09C_x0D0FE00A Register Definition ****
// Address
#define D18F2x09C_x0D0FE00A_ADDRESS                             0x0D0FE00A

// Type
#define D18F2x09C_x0D0FE00A_TYPE                                TYPE_D18F2x9C
// Field Data
#define D18F2x09C_x0D0FE00A_Reserved_3_0_OFFSET                 0
#define D18F2x09C_x0D0FE00A_Reserved_3_0_WIDTH                  4
#define D18F2x09C_x0D0FE00A_Reserved_3_0_MASK                   0xF
#define D18F2x09C_x0D0FE00A_SkewMemClk_OFFSET                   4
#define D18F2x09C_x0D0FE00A_SkewMemClk_WIDTH                    1
#define D18F2x09C_x0D0FE00A_SkewMemClk_MASK                     0x10
#define D18F2x09C_x0D0FE00A_Reserved_11_5_OFFSET                5
#define D18F2x09C_x0D0FE00A_Reserved_11_5_WIDTH                 7
#define D18F2x09C_x0D0FE00A_Reserved_11_5_MASK                  0xFE0
#define D18F2x09C_x0D0FE00A_Reserved_31_15_OFFSET               15
#define D18F2x09C_x0D0FE00A_Reserved_31_15_WIDTH                17
#define D18F2x09C_x0D0FE00A_Reserved_31_15_MASK                 0xFFFF8000

/// D18F2x09C_x0D0FE00A
typedef union {
  struct {                                                              ///<
    UINT32                                              Reserved_3_0:4; ///<
    UINT32                                                SkewMemClk:1; ///<
    UINT32                                             Reserved_11_5:7; ///<
    UINT32                                         :2; ///<
    UINT32                                           :1; ///<
    UINT32                                            Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0FE00A_STRUCT;

/// D0F0xE4_WRAP_8016
typedef union {
  struct {                                                              ///<
    UINT32                                          CalibAckLatency:6 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                         CalibDoneSelPifA:1 ; ///<
    UINT32                                             Reserved_9_9:1 ; ///<
    UINT32                                           Reserved_10_10:1 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                           Gen1OnlyEngage:1 ; ///<
    UINT32                                          Gen1OnlyEngaged:1 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                       LclkDynGateLatency:6 ; ///<
    UINT32                                             LclkGateFree:1 ; ///<
    UINT32                                        LclkDynGateEnable:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} ex688_STRUCT;



#endif
