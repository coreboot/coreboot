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
 * @e \$Revision:$   @e \$Date:$
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
 ***************************************************************************
 *
 */

#ifndef _GNBREGISTERSON_H_
#define _GNBREGISTERSON_H_
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

// **** D0F0x78 Register Definition ****
// Address
#define D0F0x78_ADDRESS                                         0x78

// Type
#define D0F0x78_TYPE                                            TYPE_D0F0
// Field Data
#define D0F0x78_Scratch_OFFSET                                  0
#define D0F0x78_Scratch_WIDTH                                   32
#define D0F0x78_Scratch_MASK                                    0xffffffff

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

// **** D18F2x04 Register Definition ****
// Address
#define D18F2x04_ADDRESS                                        0x4

// Type
#define D18F2x04_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x04_Command_OFFSET                                 0
#define D18F2x04_Command_WIDTH                                  16
#define D18F2x04_Command_MASK                                   0xffff
#define D18F2x04_Status_OFFSET                                  16
#define D18F2x04_Status_WIDTH                                   16
#define D18F2x04_Status_MASK                                    0xffff0000

/// D18F2x04
typedef union {
  struct {                                                              ///<
    UINT32                                                  Command:16; ///<
    UINT32                                                   Status:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x04_STRUCT;

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

// **** D18F2x34 Register Definition ****
// Address
#define D18F2x34_ADDRESS                                        0x34

// Type
#define D18F2x34_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x34_CapPtr_OFFSET                                  0
#define D18F2x34_CapPtr_WIDTH                                   8
#define D18F2x34_CapPtr_MASK                                    0xff
#define D18F2x34_Reserved_31_8_OFFSET                           8
#define D18F2x34_Reserved_31_8_WIDTH                            24
#define D18F2x34_Reserved_31_8_MASK                             0xffffff00

/// D18F2x34
typedef union {
  struct {                                                              ///<
    UINT32                                                   CapPtr:8 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x34_STRUCT;

// **** D18F2x40 Register Definition ****
// Address
#define D18F2x40_ADDRESS                                        0x40

// Type
#define D18F2x40_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x40_CSEnable_OFFSET                                0
#define D18F2x40_CSEnable_WIDTH                                 1
#define D18F2x40_CSEnable_MASK                                  0x1
#define D18F2x40_Reserved_1_1_OFFSET                            1
#define D18F2x40_Reserved_1_1_WIDTH                             1
#define D18F2x40_Reserved_1_1_MASK                              0x2
#define D18F2x40_TestFail_OFFSET                                2
#define D18F2x40_TestFail_WIDTH                                 1
#define D18F2x40_TestFail_MASK                                  0x4
#define D18F2x40_OnDimmMirror_OFFSET                            3
#define D18F2x40_OnDimmMirror_WIDTH                             1
#define D18F2x40_OnDimmMirror_MASK                              0x8
#define D18F2x40_Reserved_4_4_OFFSET                            4
#define D18F2x40_Reserved_4_4_WIDTH                             1
#define D18F2x40_Reserved_4_4_MASK                              0x10
#define D18F2x40_BaseAddr_21_13__OFFSET                         5
#define D18F2x40_BaseAddr_21_13__WIDTH                          9
#define D18F2x40_BaseAddr_21_13__MASK                           0x3fe0
#define D18F2x40_Reserved_18_14_OFFSET                          14
#define D18F2x40_Reserved_18_14_WIDTH                           5
#define D18F2x40_Reserved_18_14_MASK                            0x7c000
#define D18F2x40_BaseAddr_35_27__OFFSET                         19
#define D18F2x40_BaseAddr_35_27__WIDTH                          9
#define D18F2x40_BaseAddr_35_27__MASK                           0xff80000
#define D18F2x40_Reserved_28_28_OFFSET                          28
#define D18F2x40_Reserved_28_28_WIDTH                           1
#define D18F2x40_Reserved_28_28_MASK                            0x10000000
#define D18F2x40_Reserved_31_29_OFFSET                          29
#define D18F2x40_Reserved_31_29_WIDTH                           3
#define D18F2x40_Reserved_31_29_MASK                            0xe0000000

/// D18F2x40
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x40_STRUCT;

// **** D18F2x44 Register Definition ****
// Address
#define D18F2x44_ADDRESS                                        0x44

// Type
#define D18F2x44_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x44_CSEnable_OFFSET                                0
#define D18F2x44_CSEnable_WIDTH                                 1
#define D18F2x44_CSEnable_MASK                                  0x1
#define D18F2x44_Reserved_1_1_OFFSET                            1
#define D18F2x44_Reserved_1_1_WIDTH                             1
#define D18F2x44_Reserved_1_1_MASK                              0x2
#define D18F2x44_TestFail_OFFSET                                2
#define D18F2x44_TestFail_WIDTH                                 1
#define D18F2x44_TestFail_MASK                                  0x4
#define D18F2x44_OnDimmMirror_OFFSET                            3
#define D18F2x44_OnDimmMirror_WIDTH                             1
#define D18F2x44_OnDimmMirror_MASK                              0x8
#define D18F2x44_Reserved_4_4_OFFSET                            4
#define D18F2x44_Reserved_4_4_WIDTH                             1
#define D18F2x44_Reserved_4_4_MASK                              0x10
#define D18F2x44_BaseAddr_21_13__OFFSET                         5
#define D18F2x44_BaseAddr_21_13__WIDTH                          9
#define D18F2x44_BaseAddr_21_13__MASK                           0x3fe0
#define D18F2x44_Reserved_18_14_OFFSET                          14
#define D18F2x44_Reserved_18_14_WIDTH                           5
#define D18F2x44_Reserved_18_14_MASK                            0x7c000
#define D18F2x44_BaseAddr_35_27__OFFSET                         19
#define D18F2x44_BaseAddr_35_27__WIDTH                          9
#define D18F2x44_BaseAddr_35_27__MASK                           0xff80000
#define D18F2x44_Reserved_28_28_OFFSET                          28
#define D18F2x44_Reserved_28_28_WIDTH                           1
#define D18F2x44_Reserved_28_28_MASK                            0x10000000
#define D18F2x44_Reserved_31_29_OFFSET                          29
#define D18F2x44_Reserved_31_29_WIDTH                           3
#define D18F2x44_Reserved_31_29_MASK                            0xe0000000

/// D18F2x44
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x44_STRUCT;

// **** D18F2x48 Register Definition ****
// Address
#define D18F2x48_ADDRESS                                        0x48

// Type
#define D18F2x48_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x48_CSEnable_OFFSET                                0
#define D18F2x48_CSEnable_WIDTH                                 1
#define D18F2x48_CSEnable_MASK                                  0x1
#define D18F2x48_Reserved_1_1_OFFSET                            1
#define D18F2x48_Reserved_1_1_WIDTH                             1
#define D18F2x48_Reserved_1_1_MASK                              0x2
#define D18F2x48_TestFail_OFFSET                                2
#define D18F2x48_TestFail_WIDTH                                 1
#define D18F2x48_TestFail_MASK                                  0x4
#define D18F2x48_OnDimmMirror_OFFSET                            3
#define D18F2x48_OnDimmMirror_WIDTH                             1
#define D18F2x48_OnDimmMirror_MASK                              0x8
#define D18F2x48_Reserved_4_4_OFFSET                            4
#define D18F2x48_Reserved_4_4_WIDTH                             1
#define D18F2x48_Reserved_4_4_MASK                              0x10
#define D18F2x48_BaseAddr_21_13__OFFSET                         5
#define D18F2x48_BaseAddr_21_13__WIDTH                          9
#define D18F2x48_BaseAddr_21_13__MASK                           0x3fe0
#define D18F2x48_Reserved_18_14_OFFSET                          14
#define D18F2x48_Reserved_18_14_WIDTH                           5
#define D18F2x48_Reserved_18_14_MASK                            0x7c000
#define D18F2x48_BaseAddr_35_27__OFFSET                         19
#define D18F2x48_BaseAddr_35_27__WIDTH                          9
#define D18F2x48_BaseAddr_35_27__MASK                           0xff80000
#define D18F2x48_Reserved_28_28_OFFSET                          28
#define D18F2x48_Reserved_28_28_WIDTH                           1
#define D18F2x48_Reserved_28_28_MASK                            0x10000000
#define D18F2x48_Reserved_31_29_OFFSET                          29
#define D18F2x48_Reserved_31_29_WIDTH                           3
#define D18F2x48_Reserved_31_29_MASK                            0xe0000000

/// D18F2x48
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x48_STRUCT;

// **** D18F2x4C Register Definition ****
// Address
#define D18F2x4C_ADDRESS                                        0x4c

// Type
#define D18F2x4C_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x4C_CSEnable_OFFSET                                0
#define D18F2x4C_CSEnable_WIDTH                                 1
#define D18F2x4C_CSEnable_MASK                                  0x1
#define D18F2x4C_Reserved_1_1_OFFSET                            1
#define D18F2x4C_Reserved_1_1_WIDTH                             1
#define D18F2x4C_Reserved_1_1_MASK                              0x2
#define D18F2x4C_TestFail_OFFSET                                2
#define D18F2x4C_TestFail_WIDTH                                 1
#define D18F2x4C_TestFail_MASK                                  0x4
#define D18F2x4C_OnDimmMirror_OFFSET                            3
#define D18F2x4C_OnDimmMirror_WIDTH                             1
#define D18F2x4C_OnDimmMirror_MASK                              0x8
#define D18F2x4C_Reserved_4_4_OFFSET                            4
#define D18F2x4C_Reserved_4_4_WIDTH                             1
#define D18F2x4C_Reserved_4_4_MASK                              0x10
#define D18F2x4C_BaseAddr_21_13__OFFSET                         5
#define D18F2x4C_BaseAddr_21_13__WIDTH                          9
#define D18F2x4C_BaseAddr_21_13__MASK                           0x3fe0
#define D18F2x4C_Reserved_18_14_OFFSET                          14
#define D18F2x4C_Reserved_18_14_WIDTH                           5
#define D18F2x4C_Reserved_18_14_MASK                            0x7c000
#define D18F2x4C_BaseAddr_35_27__OFFSET                         19
#define D18F2x4C_BaseAddr_35_27__WIDTH                          9
#define D18F2x4C_BaseAddr_35_27__MASK                           0xff80000
#define D18F2x4C_Reserved_28_28_OFFSET                          28
#define D18F2x4C_Reserved_28_28_WIDTH                           1
#define D18F2x4C_Reserved_28_28_MASK                            0x10000000
#define D18F2x4C_Reserved_31_29_OFFSET                          29
#define D18F2x4C_Reserved_31_29_WIDTH                           3
#define D18F2x4C_Reserved_31_29_MASK                            0xe0000000

/// D18F2x4C
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                 TestFail:1 ; ///<
    UINT32                                             OnDimmMirror:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x4C_STRUCT;

// **** D18F2x60 Register Definition ****
// Address
#define D18F2x60_ADDRESS                                        0x60

// Type
#define D18F2x60_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x60_Reserved_4_0_OFFSET                            0
#define D18F2x60_Reserved_4_0_WIDTH                             5
#define D18F2x60_Reserved_4_0_MASK                              0x1f
#define D18F2x60_AddrMask_21_13__OFFSET                         5
#define D18F2x60_AddrMask_21_13__WIDTH                          9
#define D18F2x60_AddrMask_21_13__MASK                           0x3fe0
#define D18F2x60_Reserved_18_14_OFFSET                          14
#define D18F2x60_Reserved_18_14_WIDTH                           5
#define D18F2x60_Reserved_18_14_MASK                            0x7c000
#define D18F2x60_AddrMask_35_27__OFFSET                         19
#define D18F2x60_AddrMask_35_27__WIDTH                          9
#define D18F2x60_AddrMask_35_27__MASK                           0xff80000
#define D18F2x60_Reserved_28_28_OFFSET                          28
#define D18F2x60_Reserved_28_28_WIDTH                           1
#define D18F2x60_Reserved_28_28_MASK                            0x10000000
#define D18F2x60_Reserved_31_29_OFFSET                          29
#define D18F2x60_Reserved_31_29_WIDTH                           3
#define D18F2x60_Reserved_31_29_MASK                            0xe0000000

/// D18F2x60
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x60_STRUCT;

// **** D18F2x64 Register Definition ****
// Address
#define D18F2x64_ADDRESS                                        0x64

// Type
#define D18F2x64_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x64_Reserved_4_0_OFFSET                            0
#define D18F2x64_Reserved_4_0_WIDTH                             5
#define D18F2x64_Reserved_4_0_MASK                              0x1f
#define D18F2x64_AddrMask_21_13__OFFSET                         5
#define D18F2x64_AddrMask_21_13__WIDTH                          9
#define D18F2x64_AddrMask_21_13__MASK                           0x3fe0
#define D18F2x64_Reserved_18_14_OFFSET                          14
#define D18F2x64_Reserved_18_14_WIDTH                           5
#define D18F2x64_Reserved_18_14_MASK                            0x7c000
#define D18F2x64_AddrMask_35_27__OFFSET                         19
#define D18F2x64_AddrMask_35_27__WIDTH                          9
#define D18F2x64_AddrMask_35_27__MASK                           0xff80000
#define D18F2x64_Reserved_28_28_OFFSET                          28
#define D18F2x64_Reserved_28_28_WIDTH                           1
#define D18F2x64_Reserved_28_28_MASK                            0x10000000
#define D18F2x64_Reserved_31_29_OFFSET                          29
#define D18F2x64_Reserved_31_29_WIDTH                           3
#define D18F2x64_Reserved_31_29_MASK                            0xe0000000

/// D18F2x64
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x64_STRUCT;

// **** D18F2x78 Register Definition ****
// Address
#define D18F2x78_ADDRESS                                        0x78

// Type
#define D18F2x78_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x78_RdPtrInit_OFFSET                               0
#define D18F2x78_RdPtrInit_WIDTH                                4
#define D18F2x78_RdPtrInit_MASK                                 0xf
#define D18F2x78_Reserved_5_4_OFFSET                            4
#define D18F2x78_Reserved_5_4_WIDTH                             2
#define D18F2x78_Reserved_5_4_MASK                              0x30
#define D18F2x78_RxPtrInitReq_OFFSET                            6
#define D18F2x78_RxPtrInitReq_WIDTH                             1
#define D18F2x78_RxPtrInitReq_MASK                              0x40
#define D18F2x78_Reserved_7_7_OFFSET                            7
#define D18F2x78_Reserved_7_7_WIDTH                             1
#define D18F2x78_Reserved_7_7_MASK                              0x80
#define D18F2x78_Twrrd_3_2__OFFSET                              8
#define D18F2x78_Twrrd_3_2__WIDTH                               2
#define D18F2x78_Twrrd_3_2__MASK                                0x300
#define D18F2x78_Twrwr_3_2__OFFSET                              10
#define D18F2x78_Twrwr_3_2__WIDTH                               2
#define D18F2x78_Twrwr_3_2__MASK                                0xc00
#define D18F2x78_Trdrd_3_2__OFFSET                              12
#define D18F2x78_Trdrd_3_2__WIDTH                               2
#define D18F2x78_Trdrd_3_2__MASK                                0x3000
#define D18F2x78_Reserved_16_14_OFFSET                          14
#define D18F2x78_Reserved_16_14_WIDTH                           3
#define D18F2x78_Reserved_16_14_MASK                            0x1c000
#define D18F2x78_AddrCmdTriEn_OFFSET                            17
#define D18F2x78_AddrCmdTriEn_WIDTH                             1
#define D18F2x78_AddrCmdTriEn_MASK                              0x20000
#define D18F2x78_Reserved_19_18_OFFSET                          18
#define D18F2x78_Reserved_19_18_WIDTH                           2
#define D18F2x78_Reserved_19_18_MASK                            0xc0000
#define D18F2x78_ForceCasToSlot0_OFFSET                         20
#define D18F2x78_ForceCasToSlot0_WIDTH                          1
#define D18F2x78_ForceCasToSlot0_MASK                           0x100000
#define D18F2x78_DisCutThroughMode_OFFSET                       21
#define D18F2x78_DisCutThroughMode_WIDTH                        1
#define D18F2x78_DisCutThroughMode_MASK                         0x200000
#define D18F2x78_MaxRdLatency_OFFSET                            22
#define D18F2x78_MaxRdLatency_WIDTH                             10
#define D18F2x78_MaxRdLatency_MASK                              0xffc00000

/// D18F2x78
typedef union {
  struct {                                                              ///<
    UINT32                                                RdPtrInit:4 ; ///<
    UINT32                                             Reserved_5_4:2 ; ///<
    UINT32                                             RxPtrInitReq:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                               Twrrd_3_2_:2 ; ///<
    UINT32                                               Twrwr_3_2_:2 ; ///<
    UINT32                                               Trdrd_3_2_:2 ; ///<
    UINT32                                           Reserved_16_14:3 ; ///<
    UINT32                                             AddrCmdTriEn:1 ; ///<
    UINT32                                           Reserved_19_18:2 ; ///<
    UINT32                                          ForceCasToSlot0:1 ; ///<
    UINT32                                        DisCutThroughMode:1 ; ///<
    UINT32                                             MaxRdLatency:10; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x78_STRUCT;

// **** D18F2x7C Register Definition ****
// Address
#define D18F2x7C_ADDRESS                                        0x7c

// Type
#define D18F2x7C_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x7C_MrsAddress_OFFSET                              0
#define D18F2x7C_MrsAddress_WIDTH                               16
#define D18F2x7C_MrsAddress_MASK                                0xffff
#define D18F2x7C_MrsBank_OFFSET                                 16
#define D18F2x7C_MrsBank_WIDTH                                  3
#define D18F2x7C_MrsBank_MASK                                   0x70000
#define D18F2x7C_Reserved_19_19_OFFSET                          19
#define D18F2x7C_Reserved_19_19_WIDTH                           1
#define D18F2x7C_Reserved_19_19_MASK                            0x80000
#define D18F2x7C_MrsChipSel_OFFSET                              20
#define D18F2x7C_MrsChipSel_WIDTH                               3
#define D18F2x7C_MrsChipSel_MASK                                0x700000
#define D18F2x7C_Reserved_23_23_OFFSET                          23
#define D18F2x7C_Reserved_23_23_WIDTH                           1
#define D18F2x7C_Reserved_23_23_MASK                            0x800000
#define D18F2x7C_SendPchgAll_OFFSET                             24
#define D18F2x7C_SendPchgAll_WIDTH                              1
#define D18F2x7C_SendPchgAll_MASK                               0x1000000
#define D18F2x7C_SendAutoRefresh_OFFSET                         25
#define D18F2x7C_SendAutoRefresh_WIDTH                          1
#define D18F2x7C_SendAutoRefresh_MASK                           0x2000000
#define D18F2x7C_SendMrsCmd_OFFSET                              26
#define D18F2x7C_SendMrsCmd_WIDTH                               1
#define D18F2x7C_SendMrsCmd_MASK                                0x4000000
#define D18F2x7C_DeassertMemRstX_OFFSET                         27
#define D18F2x7C_DeassertMemRstX_WIDTH                          1
#define D18F2x7C_DeassertMemRstX_MASK                           0x8000000
#define D18F2x7C_AssertCke_OFFSET                               28
#define D18F2x7C_AssertCke_WIDTH                                1
#define D18F2x7C_AssertCke_MASK                                 0x10000000
#define D18F2x7C_SendZQCmd_OFFSET                               29
#define D18F2x7C_SendZQCmd_WIDTH                                1
#define D18F2x7C_SendZQCmd_MASK                                 0x20000000
#define D18F2x7C_Reserved_30_30_OFFSET                          30
#define D18F2x7C_Reserved_30_30_WIDTH                           1
#define D18F2x7C_Reserved_30_30_MASK                            0x40000000
#define D18F2x7C_EnDramInit_OFFSET                              31
#define D18F2x7C_EnDramInit_WIDTH                               1
#define D18F2x7C_EnDramInit_MASK                                0x80000000

/// D18F2x7C
typedef union {
  struct {                                                              ///<
    UINT32                                               MrsAddress:16; ///<
    UINT32                                                  MrsBank:3 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                               MrsChipSel:3 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                              SendPchgAll:1 ; ///<
    UINT32                                          SendAutoRefresh:1 ; ///<
    UINT32                                               SendMrsCmd:1 ; ///<
    UINT32                                          DeassertMemRstX:1 ; ///<
    UINT32                                                AssertCke:1 ; ///<
    UINT32                                                SendZQCmd:1 ; ///<
    UINT32                                           Reserved_30_30:1 ; ///<
    UINT32                                               EnDramInit:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x7C_STRUCT;

// **** D18F2x80 Register Definition ****
// Address
#define D18F2x80_ADDRESS                                        0x80

// Type
#define D18F2x80_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x80_Dimm0AddrMap_OFFSET                            0
#define D18F2x80_Dimm0AddrMap_WIDTH                             4
#define D18F2x80_Dimm0AddrMap_MASK                              0xf
#define D18F2x80_Dimm1AddrMap_OFFSET                            4
#define D18F2x80_Dimm1AddrMap_WIDTH                             4
#define D18F2x80_Dimm1AddrMap_MASK                              0xf0
#define D18F2x80_Reserved_31_8_OFFSET                           8
#define D18F2x80_Reserved_31_8_WIDTH                            24
#define D18F2x80_Reserved_31_8_MASK                             0xffffff00

/// D18F2x80
typedef union {
  struct {                                                              ///<
    UINT32                                             Dimm0AddrMap:4 ; ///<
    UINT32                                             Dimm1AddrMap:4 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x80_STRUCT;

// **** D18F2x084 Register Definition ****
// Address
#define D18F2x084_ADDRESS                                        0x84

// Type
#define D18F2x084_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x084_BurstCtrl_OFFSET                               0
#define D18F2x084_BurstCtrl_WIDTH                                2
#define D18F2x084_BurstCtrl_MASK                                 0x3
#define D18F2x084_Reserved_3_2_OFFSET                            2
#define D18F2x084_Reserved_3_2_WIDTH                             2
#define D18F2x084_Reserved_3_2_MASK                              0xc
#define D18F2x084_Twr_OFFSET                                     4
#define D18F2x084_Twr_WIDTH                                      3
#define D18F2x084_Twr_MASK                                       0x70
#define D18F2x084_Reserved_19_7_OFFSET                           7
#define D18F2x084_Reserved_19_7_WIDTH                            13
#define D18F2x084_Reserved_19_7_MASK                             0xfff80
#define D18F2x084_Tcwl_OFFSET                                    20
#define D18F2x084_Tcwl_WIDTH                                     3
#define D18F2x084_Tcwl_MASK                                      0x700000
#define D18F2x084_PchgPDModeSel_OFFSET                           23
#define D18F2x084_PchgPDModeSel_WIDTH                            1
#define D18F2x084_PchgPDModeSel_MASK                             0x800000
#define D18F2x084_Reserved_31_24_OFFSET                          24
#define D18F2x084_Reserved_31_24_WIDTH                           8
#define D18F2x084_Reserved_31_24_MASK                            0xff000000

/// D18F2x084
typedef union {
  struct {                                                              ///<
    UINT32                                                BurstCtrl:2 ; ///<
    UINT32                                             Reserved_3_2:2 ; ///<
    UINT32                                                      Twr:3 ; ///<
    UINT32                                            Reserved_19_7:13; ///<
    UINT32                                                     Tcwl:3 ; ///<
    UINT32                                            PchgPDModeSel:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x084_STRUCT;

// **** D18F2x08C Register Definition ****
// Address
#define D18F2x08C_ADDRESS                                        0x8c

// Type
#define D18F2x08C_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x08C_TrwtWB_OFFSET                                  0
#define D18F2x08C_TrwtWB_WIDTH                                   4
#define D18F2x08C_TrwtWB_MASK                                    0xf
#define D18F2x08C_TrwtTO_OFFSET                                  4
#define D18F2x08C_TrwtTO_WIDTH                                   4
#define D18F2x08C_TrwtTO_MASK                                    0xf0
#define D18F2x08C_Reserved_9_8_OFFSET                            8
#define D18F2x08C_Reserved_9_8_WIDTH                             2
#define D18F2x08C_Reserved_9_8_MASK                              0x300
#define D18F2x08C_Twrrd_1_0__OFFSET                              10
#define D18F2x08C_Twrrd_1_0__WIDTH                               2
#define D18F2x08C_Twrrd_1_0__MASK                                0xc00
#define D18F2x08C_Twrwr_1_0__OFFSET                              12
#define D18F2x08C_Twrwr_1_0__WIDTH                               2
#define D18F2x08C_Twrwr_1_0__MASK                                0x3000
#define D18F2x08C_Trdrd_1_0__OFFSET                              14
#define D18F2x08C_Trdrd_1_0__WIDTH                               2
#define D18F2x08C_Trdrd_1_0__MASK                                0xc000
#define D18F2x08C_Tref_OFFSET                                    16
#define D18F2x08C_Tref_WIDTH                                     2
#define D18F2x08C_Tref_MASK                                      0x30000
#define D18F2x08C_DisAutoRefresh_OFFSET                          18
#define D18F2x08C_DisAutoRefresh_WIDTH                           1
#define D18F2x08C_DisAutoRefresh_MASK                            0x40000
#define D18F2x08C_Reserved_19_19_OFFSET                          19
#define D18F2x08C_Reserved_19_19_WIDTH                           1
#define D18F2x08C_Reserved_19_19_MASK                            0x80000
#define D18F2x08C_Trfc0_OFFSET                                   20
#define D18F2x08C_Trfc0_WIDTH                                    3
#define D18F2x08C_Trfc0_MASK                                     0x700000
#define D18F2x08C_Trfc1_OFFSET                                   23
#define D18F2x08C_Trfc1_WIDTH                                    3
#define D18F2x08C_Trfc1_MASK                                     0x3800000
#define D18F2x08C_Reserved_31_26_OFFSET                          26
#define D18F2x08C_Reserved_31_26_WIDTH                           6
#define D18F2x08C_Reserved_31_26_MASK                            0xfc000000

/// D18F2x08C
typedef union {
  struct {                                                              ///<
    UINT32                                                   TrwtWB:4 ; ///<
    UINT32                                                   TrwtTO:4 ; ///<
    UINT32                                             Reserved_9_8:2 ; ///<
    UINT32                                               Twrrd_1_0_:2 ; ///<
    UINT32                                               Twrwr_1_0_:2 ; ///<
    UINT32                                               Trdrd_1_0_:2 ; ///<
    UINT32                                                     Tref:2 ; ///<
    UINT32                                           DisAutoRefresh:1 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                                    Trfc0:3 ; ///<
    UINT32                                                    Trfc1:3 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x08C_STRUCT;

// **** D18F2x090 Register Definition ****
// Address
#define D18F2x090_ADDRESS                                        0x90

// Type
#define D18F2x090_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2x090_Reserved_0_0_OFFSET                            0
#define D18F2x090_Reserved_0_0_WIDTH                             1
#define D18F2x090_Reserved_0_0_MASK                              0x1
#define D18F2x090_ExitSelfRef_OFFSET                             1
#define D18F2x090_ExitSelfRef_WIDTH                              1
#define D18F2x090_ExitSelfRef_MASK                               0x2
#define D18F2x090_Reserved_16_2_OFFSET                           2
#define D18F2x090_Reserved_16_2_WIDTH                            15
#define D18F2x090_Reserved_16_2_MASK                             0x1fffc
#define D18F2x090_EnterSelfRef_OFFSET                            17
#define D18F2x090_EnterSelfRef_WIDTH                             1
#define D18F2x090_EnterSelfRef_MASK                              0x20000
#define D18F2x090_Reserved_19_18_OFFSET                          18
#define D18F2x090_Reserved_19_18_WIDTH                           2
#define D18F2x090_Reserved_19_18_MASK                            0xc0000
#define D18F2x090_DynPageCloseEn_OFFSET                          20
#define D18F2x090_DynPageCloseEn_WIDTH                           1
#define D18F2x090_DynPageCloseEn_MASK                            0x100000
#define D18F2x090_IdleCycInit_OFFSET                             21
#define D18F2x090_IdleCycInit_WIDTH                              2
#define D18F2x090_IdleCycInit_MASK                               0x600000
#define D18F2x090_ForceAutoPchg_OFFSET                           23
#define D18F2x090_ForceAutoPchg_WIDTH                            1
#define D18F2x090_ForceAutoPchg_MASK                             0x800000
#define D18F2x090_Reserved_24_24_OFFSET                          24
#define D18F2x090_Reserved_24_24_WIDTH                           1
#define D18F2x090_Reserved_24_24_MASK                            0x1000000
#define D18F2x090_EnDispAutoPrecharge_OFFSET                     25
#define D18F2x090_EnDispAutoPrecharge_WIDTH                      1
#define D18F2x090_EnDispAutoPrecharge_MASK                       0x2000000
#define D18F2x090_DbeSkidBufDis_OFFSET                           26
#define D18F2x090_DbeSkidBufDis_WIDTH                            1
#define D18F2x090_DbeSkidBufDis_MASK                             0x4000000
#define D18F2x090_DisDllShutdownSR_OFFSET                        27
#define D18F2x090_DisDllShutdownSR_WIDTH                         1
#define D18F2x090_DisDllShutdownSR_MASK                          0x8000000
#define D18F2x090_Reserved_31_28_OFFSET                          28
#define D18F2x090_Reserved_31_28_WIDTH                           4
#define D18F2x090_Reserved_31_28_MASK                            0xf0000000

/// D18F2x090
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                              ExitSelfRef:1 ; ///<
    UINT32                                            Reserved_16_2:15; ///<
    UINT32                                             EnterSelfRef:1 ; ///<
    UINT32                                           Reserved_19_18:2 ; ///<
    UINT32                                           DynPageCloseEn:1 ; ///<
    UINT32                                              IdleCycInit:2 ; ///<
    UINT32                                            ForceAutoPchg:1 ; ///<
    UINT32                                           Reserved_24_24:1 ; ///<
    UINT32                                      EnDispAutoPrecharge:1 ; ///<
    UINT32                                            DbeSkidBufDis:1 ; ///<
    UINT32                                         DisDllShutdownSR:1 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x090_STRUCT;

// **** D18F2x9C Register Definition ****
// Address
#define D18F2x9C_ADDRESS                                        0x9c

// **** D18F2xA0 Register Definition ****
// Address
#define D18F2xA0_ADDRESS                                        0xa0

// Type
#define D18F2xA0_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xA0_Reserved_31_0_OFFSET                           0
#define D18F2xA0_Reserved_31_0_WIDTH                            32
#define D18F2xA0_Reserved_31_0_MASK                             0xffffffff

/// D18F2xA0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_31_0:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2xA0_STRUCT;

// **** D18F2xA4 Register Definition ****
// Address
#define D18F2xA4_ADDRESS                                        0xa4

// Type
#define D18F2xA4_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xA4_DoubleTrefRateEn_OFFSET                        0
#define D18F2xA4_DoubleTrefRateEn_WIDTH                         1
#define D18F2xA4_DoubleTrefRateEn_MASK                          0x1
#define D18F2xA4_ThrottleEn_OFFSET                              1
#define D18F2xA4_ThrottleEn_WIDTH                               2
#define D18F2xA4_ThrottleEn_MASK                                0x6
#define D18F2xA4_Reserved_31_3_OFFSET                           3
#define D18F2xA4_Reserved_31_3_WIDTH                            29
#define D18F2xA4_Reserved_31_3_MASK                             0xfffffff8

/// D18F2xA4
typedef union {
  struct {                                                              ///<
    UINT32                                         DoubleTrefRateEn:1 ; ///<
    UINT32                                               ThrottleEn:2 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2xA4_STRUCT;

// **** D18F2xA8 Register Definition ****
// Address
#define D18F2xA8_ADDRESS                                        0xa8

// Type
#define D18F2xA8_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xA8_Reserved_19_0_OFFSET                           0
#define D18F2xA8_Reserved_19_0_WIDTH                            20
#define D18F2xA8_Reserved_19_0_MASK                             0xfffff
#define D18F2xA8_BankSwap_OFFSET                                20
#define D18F2xA8_BankSwap_WIDTH                                 1
#define D18F2xA8_BankSwap_MASK                                  0x100000
#define D18F2xA8_DbeGskMemClkAlignMode_OFFSET                   21
#define D18F2xA8_DbeGskMemClkAlignMode_WIDTH                    2
#define D18F2xA8_DbeGskMemClkAlignMode_MASK                     0x600000
#define D18F2xA8_Reserved_31_23_OFFSET                          23
#define D18F2xA8_Reserved_31_23_WIDTH                           9
#define D18F2xA8_Reserved_31_23_MASK                            0xff800000

/// D18F2xA8
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_19_0:20; ///<
    UINT32                                                 BankSwap:1 ; ///<
    UINT32                                    DbeGskMemClkAlignMode:2 ; ///<
    UINT32                                           Reserved_31_23:9 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2xA8_STRUCT;

// **** D18F2xAC Register Definition ****
// Address
#define D18F2xAC_ADDRESS                                        0xac

// Type
#define D18F2xAC_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xAC_MemTempHot_OFFSET                              0
#define D18F2xAC_MemTempHot_WIDTH                               1
#define D18F2xAC_MemTempHot_MASK                                0x1
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

// **** D18F2xF0 Register Definition ****
// Address
#define D18F2xF0_ADDRESS                                        0xf0

// Type
#define D18F2xF0_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xF0_DctOffset_OFFSET                               0
#define D18F2xF0_DctOffset_WIDTH                                28
#define D18F2xF0_DctOffset_MASK                                 0xfffffff
#define D18F2xF0_Reserved_29_28_OFFSET                          28
#define D18F2xF0_Reserved_29_28_WIDTH                           2
#define D18F2xF0_Reserved_29_28_MASK                            0x30000000
#define D18F2xF0_DctAccessWrite_OFFSET                          30
#define D18F2xF0_DctAccessWrite_WIDTH                           1
#define D18F2xF0_DctAccessWrite_MASK                            0x40000000
#define D18F2xF0_DctAccessDone_OFFSET                           31
#define D18F2xF0_DctAccessDone_WIDTH                            1
#define D18F2xF0_DctAccessDone_MASK                             0x80000000

/// D18F2xF0
typedef union {
  struct {                                                              ///<
    UINT32                                                DctOffset:28; ///<
    UINT32                                           Reserved_29_28:2 ; ///<
    UINT32                                           DctAccessWrite:1 ; ///<
    UINT32                                            DctAccessDone:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2xF0_STRUCT;

// **** D18F2xF4 Register Definition ****
// Address
#define D18F2xF4_ADDRESS                                        0xf4

// Type
#define D18F2xF4_TYPE                                           TYPE_D18F2
// Field Data
#define D18F2xF4_DctExtDataPort_OFFSET                          0
#define D18F2xF4_DctExtDataPort_WIDTH                           32
#define D18F2xF4_DctExtDataPort_MASK                            0xffffffff

/// D18F2xF4
typedef union {
  struct {                                                              ///<
    UINT32                                           DctExtDataPort:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2xF4_STRUCT;

// **** D18F2x110 Register Definition ****
// Address
#define D18F2x110_ADDRESS                                       0x110

// Type
#define D18F2x110_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x110_Reserved_2_0_OFFSET                           0
#define D18F2x110_Reserved_2_0_WIDTH                            3
#define D18F2x110_Reserved_2_0_MASK                             0x7
#define D18F2x110_MemClrInit_OFFSET                             3
#define D18F2x110_MemClrInit_WIDTH                              1
#define D18F2x110_MemClrInit_MASK                               0x8
#define D18F2x110_Reserved_7_4_OFFSET                           4
#define D18F2x110_Reserved_7_4_WIDTH                            4
#define D18F2x110_Reserved_7_4_MASK                             0xf0
#define D18F2x110_DramEnable_OFFSET                             8
#define D18F2x110_DramEnable_WIDTH                              1
#define D18F2x110_DramEnable_MASK                               0x100
#define D18F2x110_MemClrBusy_OFFSET                             9
#define D18F2x110_MemClrBusy_WIDTH                              1
#define D18F2x110_MemClrBusy_MASK                               0x200
#define D18F2x110_MemCleared_OFFSET                             10
#define D18F2x110_MemCleared_WIDTH                              1
#define D18F2x110_MemCleared_MASK                               0x400
#define D18F2x110_Reserved_31_11_OFFSET                         11
#define D18F2x110_Reserved_31_11_WIDTH                          21
#define D18F2x110_Reserved_31_11_MASK                           0xfffff800

/// D18F2x110
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                               MemClrInit:1 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                               DramEnable:1 ; ///<
    UINT32                                               MemClrBusy:1 ; ///<
    UINT32                                               MemCleared:1 ; ///<
    UINT32                                           Reserved_31_11:21; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x110_STRUCT;

// **** D18F2x114 Register Definition ****
// Address
#define D18F2x114_ADDRESS                                       0x114

// Type
#define D18F2x114_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x114_Reserved_8_0_OFFSET                           0
#define D18F2x114_Reserved_8_0_WIDTH                            9
#define D18F2x114_Reserved_8_0_MASK                             0x1ff
#define D18F2x114_DctSelBankSwap_OFFSET                         9
#define D18F2x114_DctSelBankSwap_WIDTH                          1
#define D18F2x114_DctSelBankSwap_MASK                           0x200
#define D18F2x114_Reserved_31_10_OFFSET                         10
#define D18F2x114_Reserved_31_10_WIDTH                          22
#define D18F2x114_Reserved_31_10_MASK                           0xfffffc00

/// D18F2x114
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                           DctSelBankSwap:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x114_STRUCT;

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
#define D18F3x64_HtcLock_OFFSET                                 31
#define D18F3x64_HtcLock_WIDTH                                  1
#define D18F3x64_HtcLock_MASK                                   0x80000000

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
    UINT32                                                  HtcLock:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x64_STRUCT;

// **** D18F3x6C Register Definition ****
// Address
#define D18F3x6C_ADDRESS                                        0x6c

// Type
#define D18F3x6C_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x6C_UpLoPreqDBC_OFFSET                             0
#define D18F3x6C_UpLoPreqDBC_WIDTH                              4
#define D18F3x6C_UpLoPreqDBC_MASK                               0xf
#define D18F3x6C_UpLoNpreqDBC_OFFSET                            4
#define D18F3x6C_UpLoNpreqDBC_WIDTH                             4
#define D18F3x6C_UpLoNpreqDBC_MASK                              0xf0
#define D18F3x6C_UpLoRespDBC_OFFSET                             8
#define D18F3x6C_UpLoRespDBC_WIDTH                              4
#define D18F3x6C_UpLoRespDBC_MASK                               0xf00
#define D18F3x6C_Reserved_15_12_OFFSET                          12
#define D18F3x6C_Reserved_15_12_WIDTH                           4
#define D18F3x6C_Reserved_15_12_MASK                            0xf000
#define D18F3x6C_UpHiPreqDBC_OFFSET                             16
#define D18F3x6C_UpHiPreqDBC_WIDTH                              4
#define D18F3x6C_UpHiPreqDBC_MASK                               0xf0000
#define D18F3x6C_UpHiNpreqDBC_OFFSET                            20
#define D18F3x6C_UpHiNpreqDBC_WIDTH                             4
#define D18F3x6C_UpHiNpreqDBC_MASK                              0xf00000
#define D18F3x6C_Reserved_31_24_OFFSET                          24
#define D18F3x6C_Reserved_31_24_WIDTH                           8
#define D18F3x6C_Reserved_31_24_MASK                            0xff000000

/// D18F3x6C
typedef union {
  struct {                                                              ///<
    UINT32                                              UpLoPreqDBC:4 ; ///<
    UINT32                                             UpLoNpreqDBC:4 ; ///<
    UINT32                                              UpLoRespDBC:4 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                              UpHiPreqDBC:4 ; ///<
    UINT32                                             UpHiNpreqDBC:4 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x6C_STRUCT;

// **** D18F3x74 Register Definition ****
// Address
#define D18F3x74_ADDRESS                                        0x74

// Type
#define D18F3x74_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x74_UpLoPreqCBC_OFFSET                             0
#define D18F3x74_UpLoPreqCBC_WIDTH                              4
#define D18F3x74_UpLoPreqCBC_MASK                               0xf
#define D18F3x74_UpLoNpreqCBC_OFFSET                            4
#define D18F3x74_UpLoNpreqCBC_WIDTH                             4
#define D18F3x74_UpLoNpreqCBC_MASK                              0xf0
#define D18F3x74_UpLoRespCBC_OFFSET                             8
#define D18F3x74_UpLoRespCBC_WIDTH                              4
#define D18F3x74_UpLoRespCBC_MASK                               0xf00
#define D18F3x74_Reserved_15_12_OFFSET                          12
#define D18F3x74_Reserved_15_12_WIDTH                           4
#define D18F3x74_Reserved_15_12_MASK                            0xf000
#define D18F3x74_UpHiPreqCBC_OFFSET                             16
#define D18F3x74_UpHiPreqCBC_WIDTH                              4
#define D18F3x74_UpHiPreqCBC_MASK                               0xf0000
#define D18F3x74_UpHiNpreqCBC_OFFSET                            20
#define D18F3x74_UpHiNpreqCBC_WIDTH                             4
#define D18F3x74_UpHiNpreqCBC_MASK                              0xf00000
#define D18F3x74_Reserved_31_24_OFFSET                          24
#define D18F3x74_Reserved_31_24_WIDTH                           8
#define D18F3x74_Reserved_31_24_MASK                            0xff000000

/// D18F3x74
typedef union {
  struct {                                                              ///<
    UINT32                                              UpLoPreqCBC:4 ; ///<
    UINT32                                             UpLoNpreqCBC:4 ; ///<
    UINT32                                              UpLoRespCBC:4 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                              UpHiPreqCBC:4 ; ///<
    UINT32                                             UpHiNpreqCBC:4 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x74_STRUCT;

// **** D18F3x7C Register Definition ****
// Address
#define D18F3x7C_ADDRESS                                        0x7c

// Type
#define D18F3x7C_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3x7C_CpuBC_OFFSET                                   0
#define D18F3x7C_CpuBC_WIDTH                                    6
#define D18F3x7C_CpuBC_MASK                                     0x3f
#define D18F3x7C_Reserved_7_6_OFFSET                            6
#define D18F3x7C_Reserved_7_6_WIDTH                             2
#define D18F3x7C_Reserved_7_6_MASK                              0xc0
#define D18F3x7C_LoPriPBC_OFFSET                                8
#define D18F3x7C_LoPriPBC_WIDTH                                 6
#define D18F3x7C_LoPriPBC_MASK                                  0x3f00
#define D18F3x7C_Reserved_15_14_OFFSET                          14
#define D18F3x7C_Reserved_15_14_WIDTH                           2
#define D18F3x7C_Reserved_15_14_MASK                            0xc000
#define D18F3x7C_LoPriNPBC_OFFSET                               16
#define D18F3x7C_LoPriNPBC_WIDTH                                6
#define D18F3x7C_LoPriNPBC_MASK                                 0x3f0000
#define D18F3x7C_Reserved_23_22_OFFSET                          22
#define D18F3x7C_Reserved_23_22_WIDTH                           2
#define D18F3x7C_Reserved_23_22_MASK                            0xc00000
#define D18F3x7C_FreePoolBC_OFFSET                              24
#define D18F3x7C_FreePoolBC_WIDTH                               6
#define D18F3x7C_FreePoolBC_MASK                                0x3f000000
#define D18F3x7C_Reserved_31_30_OFFSET                          30
#define D18F3x7C_Reserved_31_30_WIDTH                           2
#define D18F3x7C_Reserved_31_30_MASK                            0xc0000000

/// D18F3x7C
typedef union {
  struct {                                                              ///<
    UINT32                                                    CpuBC:6 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                                 LoPriPBC:6 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                                LoPriNPBC:6 ; ///<
    UINT32                                           Reserved_23_22:2 ; ///<
    UINT32                                               FreePoolBC:6 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x7C_STRUCT;

// **** D18F3xD8 Register Definition ****
// Address
#define D18F3xD8_ADDRESS                                        0xd8

// Type
#define D18F3xD8_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xD8_Reserved_3_0_OFFSET                            0
#define D18F3xD8_Reserved_3_0_WIDTH                             4
#define D18F3xD8_Reserved_3_0_MASK                              0xf
#define D18F3xD8_VSRampSlamTime_OFFSET                          4
#define D18F3xD8_VSRampSlamTime_WIDTH                           3
#define D18F3xD8_VSRampSlamTime_MASK                            0x70
#define D18F3xD8_ExtndTriDly_OFFSET                             7
#define D18F3xD8_ExtndTriDly_WIDTH                              5
#define D18F3xD8_ExtndTriDly_MASK                               0xf80
#define D18F3xD8_Reserved_31_12_OFFSET                          12
#define D18F3xD8_Reserved_31_12_WIDTH                           20
#define D18F3xD8_Reserved_31_12_MASK                            0xfffff000

/// D18F3xD8
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                           VSRampSlamTime:3 ; ///<
    UINT32                                              ExtndTriDly:5 ; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xD8_STRUCT;

// **** D18F3xDC Register Definition ****
// Address
#define D18F3xDC_ADDRESS                                        0xdc

// Type
#define D18F3xDC_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xDC_Reserved_7_0_OFFSET                            0
#define D18F3xDC_Reserved_7_0_WIDTH                             8
#define D18F3xDC_Reserved_7_0_MASK                              0xff
#define D18F3xDC_PstateMaxVal_OFFSET                            8
#define D18F3xDC_PstateMaxVal_WIDTH                             3
#define D18F3xDC_PstateMaxVal_MASK                              0x700
#define D18F3xDC_Reserved_11_11_OFFSET                          11
#define D18F3xDC_Reserved_11_11_WIDTH                           1
#define D18F3xDC_Reserved_11_11_MASK                            0x800
#define D18F3xDC_NbPs0Vid_OFFSET                                12
#define D18F3xDC_NbPs0Vid_WIDTH                                 7
#define D18F3xDC_NbPs0Vid_MASK                                  0x7f000
#define D18F3xDC_NclkFreqDone_OFFSET                            19
#define D18F3xDC_NclkFreqDone_WIDTH                             1
#define D18F3xDC_NclkFreqDone_MASK                              0x80000
#define D18F3xDC_NbPs0NclkDiv_OFFSET                            20
#define D18F3xDC_NbPs0NclkDiv_WIDTH                             7
#define D18F3xDC_NbPs0NclkDiv_MASK                              0x7f00000
#define D18F3xDC_NbClockGateHyst_OFFSET                         27
#define D18F3xDC_NbClockGateHyst_WIDTH                          3
#define D18F3xDC_NbClockGateHyst_MASK                           0x38000000
#define D18F3xDC_NbClockGateEn_OFFSET                           30
#define D18F3xDC_NbClockGateEn_WIDTH                            1
#define D18F3xDC_NbClockGateEn_MASK                             0x40000000
#define D18F3xDC_CnbCifClockGateEn_OFFSET                       31
#define D18F3xDC_CnbCifClockGateEn_WIDTH                        1
#define D18F3xDC_CnbCifClockGateEn_MASK                         0x80000000

/// D18F3xDC
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_7_0:8 ; ///<
    UINT32                                             PstateMaxVal:3 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                                 NbPs0Vid:7 ; ///<
    UINT32                                             NclkFreqDone:1 ; ///<
    UINT32                                             NbPs0NclkDiv:7 ; ///<
    UINT32                                          NbClockGateHyst:3 ; ///<
    UINT32                                            NbClockGateEn:1 ; ///<
    UINT32                                        CnbCifClockGateEn:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xDC_STRUCT;

// **** D18F3x15C Register Definition ****
// Address
#define D18F3x15C_ADDRESS                                       0x15c

// Type
#define D18F3x15C_TYPE                                          TYPE_D18F3
// Field Data
#define D18F3x15C_SclkVidLevel0_OFFSET                          0
#define D18F3x15C_SclkVidLevel0_WIDTH                           7
#define D18F3x15C_SclkVidLevel0_MASK                            0x7f
#define D18F3x15C_Reserved_7_7_OFFSET                           7
#define D18F3x15C_Reserved_7_7_WIDTH                            1
#define D18F3x15C_Reserved_7_7_MASK                             0x80
#define D18F3x15C_SclkVidLevel1_OFFSET                          8
#define D18F3x15C_SclkVidLevel1_WIDTH                           7
#define D18F3x15C_SclkVidLevel1_MASK                            0x7f00
#define D18F3x15C_Reserved_15_15_OFFSET                         15
#define D18F3x15C_Reserved_15_15_WIDTH                          1
#define D18F3x15C_Reserved_15_15_MASK                           0x8000
#define D18F3x15C_SclkVidLevel2_OFFSET                          16
#define D18F3x15C_SclkVidLevel2_WIDTH                           7
#define D18F3x15C_SclkVidLevel2_MASK                            0x7f0000
#define D18F3x15C_Reserved_23_23_OFFSET                         23
#define D18F3x15C_Reserved_23_23_WIDTH                          1
#define D18F3x15C_Reserved_23_23_MASK                           0x800000
#define D18F3x15C_SclkVidLevel3_OFFSET                          24
#define D18F3x15C_SclkVidLevel3_WIDTH                           7
#define D18F3x15C_SclkVidLevel3_MASK                            0x7f000000
#define D18F3x15C_Reserved_31_31_OFFSET                         31
#define D18F3x15C_Reserved_31_31_WIDTH                          1
#define D18F3x15C_Reserved_31_31_MASK                           0x80000000

/// D18F3x15C
typedef union {
  struct {                                                              ///<
    UINT32                                            SclkVidLevel0:7 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                            SclkVidLevel1:7 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                            SclkVidLevel2:7 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                            SclkVidLevel3:7 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x15C_STRUCT;

// **** D18F3x17C Register Definition ****
// Address
#define D18F3x17C_ADDRESS                                       0x17c

// Type
#define D18F3x17C_TYPE                                          TYPE_D18F3
// Field Data
#define D18F3x17C_HiPriPBC_OFFSET                               0
#define D18F3x17C_HiPriPBC_WIDTH                                6
#define D18F3x17C_HiPriPBC_MASK                                 0x3f
#define D18F3x17C_Reserved_7_6_OFFSET                           6
#define D18F3x17C_Reserved_7_6_WIDTH                            2
#define D18F3x17C_Reserved_7_6_MASK                             0xc0
#define D18F3x17C_HiPriNPBC_OFFSET                              8
#define D18F3x17C_HiPriNPBC_WIDTH                               6
#define D18F3x17C_HiPriNPBC_MASK                                0x3f00
#define D18F3x17C_Reserved_31_14_OFFSET                         14
#define D18F3x17C_Reserved_31_14_WIDTH                          18
#define D18F3x17C_Reserved_31_14_MASK                           0xffffc000

/// D18F3x17C
typedef union {
  struct {                                                              ///<
    UINT32                                                 HiPriPBC:6 ; ///<
    UINT32                                             Reserved_7_6:2 ; ///<
    UINT32                                                HiPriNPBC:6 ; ///<
    UINT32                                           Reserved_31_14:18; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3x17C_STRUCT;

// **** D18F4x12C Register Definition ****
// Address
#define D18F4x12C_ADDRESS                                       0x12c

// Type
#define D18F4x12C_TYPE                                          TYPE_D18F4
// Field Data
#define D18F4x12C_C6Base_35_24__OFFSET                          0
#define D18F4x12C_C6Base_35_24__WIDTH                           12
#define D18F4x12C_C6Base_35_24__MASK                            0xfff
#define D18F4x12C_Reserved_31_12_OFFSET                         12
#define D18F4x12C_Reserved_31_12_WIDTH                          20
#define D18F4x12C_Reserved_31_12_MASK                           0xfffff000

/// D18F4x12C
typedef union {
  struct {                                                              ///<
    UINT32                                            C6Base_35_24_:12; ///<
    UINT32                                           Reserved_31_12:20; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F4x12C_STRUCT;

// **** D18F4x164 Register Definition ****
// Address
#define D18F4x164_ADDRESS                                       0x164

// Type
#define D18F4x164_TYPE                                          TYPE_D18F4
// Field Data
#define D18F4x164_FixedErrata_OFFSET                            0
#define D18F4x164_FixedErrata_WIDTH                             32
#define D18F4x164_FixedErrata_MASK                              0xffffffff

/// D18F4x164
typedef union {
  struct {                                                              ///<
    UINT32                                              FixedErrata:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F4x164_STRUCT;

// **** D18F6x90 Register Definition ****
// Address
#define D18F6x90_ADDRESS                                        0x90

// Type
#define D18F6x90_TYPE                                           TYPE_D18F6
// Field Data
#define D18F6x90_NbPs1NclkDiv_OFFSET                            0
#define D18F6x90_NbPs1NclkDiv_WIDTH                             7
#define D18F6x90_NbPs1NclkDiv_MASK                              0x7f
#define D18F6x90_Reserved_7_7_OFFSET                            7
#define D18F6x90_Reserved_7_7_WIDTH                             1
#define D18F6x90_Reserved_7_7_MASK                              0x80
#define D18F6x90_NbPs1Vid_OFFSET                                8
#define D18F6x90_NbPs1Vid_WIDTH                                 7
#define D18F6x90_NbPs1Vid_MASK                                  0x7f00
#define D18F6x90_Reserved_15_15_OFFSET                          15
#define D18F6x90_Reserved_15_15_WIDTH                           1
#define D18F6x90_Reserved_15_15_MASK                            0x8000
#define D18F6x90_NbPs1GnbSlowIgn_OFFSET                         16
#define D18F6x90_NbPs1GnbSlowIgn_WIDTH                          1
#define D18F6x90_NbPs1GnbSlowIgn_MASK                           0x10000
#define D18F6x90_Reserved_19_17_OFFSET                          17
#define D18F6x90_Reserved_19_17_WIDTH                           3
#define D18F6x90_Reserved_19_17_MASK                            0xe0000
#define D18F6x90_NbPsLock_OFFSET                                20
#define D18F6x90_NbPsLock_WIDTH                                 1
#define D18F6x90_NbPsLock_MASK                                  0x100000
#define D18F6x90_Reserved_27_21_OFFSET                          21
#define D18F6x90_Reserved_27_21_WIDTH                           7
#define D18F6x90_Reserved_27_21_MASK                            0xfe00000
#define D18F6x90_NbPsForceReq_OFFSET                            28
#define D18F6x90_NbPsForceReq_WIDTH                             1
#define D18F6x90_NbPsForceReq_MASK                              0x10000000
#define D18F6x90_NbPsForceSel_OFFSET                            29
#define D18F6x90_NbPsForceSel_WIDTH                             1
#define D18F6x90_NbPsForceSel_MASK                              0x20000000
#define D18F6x90_NbPsCtrlDis_OFFSET                             30
#define D18F6x90_NbPsCtrlDis_WIDTH                              1
#define D18F6x90_NbPsCtrlDis_MASK                               0x40000000
#define D18F6x90_NbPsCap_OFFSET                                 31
#define D18F6x90_NbPsCap_WIDTH                                  1
#define D18F6x90_NbPsCap_MASK                                   0x80000000

/// D18F6x90
typedef union {
  struct {                                                              ///<
    UINT32                                             NbPs1NclkDiv:7 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                                 NbPs1Vid:7 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                          NbPs1GnbSlowIgn:1 ; ///<
    UINT32                                           Reserved_19_17:3 ; ///<
    UINT32                                                 NbPsLock:1 ; ///<
    UINT32                                           Reserved_27_21:7 ; ///<
    UINT32                                             NbPsForceReq:1 ; ///<
    UINT32                                             NbPsForceSel:1 ; ///<
    UINT32                                              NbPsCtrlDis:1 ; ///<
    UINT32                                                  NbPsCap:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F6x90_STRUCT;

// **** D18F6x94 Register Definition ****
// Address
#define D18F6x94_ADDRESS                                        0x94

// Type
#define D18F6x94_TYPE                                           TYPE_D18F6
// Field Data
#define D18F6x94_CpuPstateThr_OFFSET                            0
#define D18F6x94_CpuPstateThr_WIDTH                             3
#define D18F6x94_CpuPstateThr_MASK                              0x7
#define D18F6x94_CpuPstateThrEn_OFFSET                          3
#define D18F6x94_CpuPstateThrEn_WIDTH                           1
#define D18F6x94_CpuPstateThrEn_MASK                            0x8
#define D18F6x94_NbPsNoTransOnDma_OFFSET                        4
#define D18F6x94_NbPsNoTransOnDma_WIDTH                         1
#define D18F6x94_NbPsNoTransOnDma_MASK                          0x10
#define D18F6x94_Reserved_19_5_OFFSET                           5
#define D18F6x94_Reserved_19_5_WIDTH                            15
#define D18F6x94_Reserved_19_5_MASK                             0xfffe0
#define D18F6x94_NbPsNonC0Timer_OFFSET                          20
#define D18F6x94_NbPsNonC0Timer_WIDTH                           3
#define D18F6x94_NbPsNonC0Timer_MASK                            0x700000
#define D18F6x94_NbPsC0Timer_OFFSET                             23
#define D18F6x94_NbPsC0Timer_WIDTH                              3
#define D18F6x94_NbPsC0Timer_MASK                               0x3800000
#define D18F6x94_NbPs1ResTmrMin_OFFSET                          26
#define D18F6x94_NbPs1ResTmrMin_WIDTH                           3
#define D18F6x94_NbPs1ResTmrMin_MASK                            0x1c000000
#define D18F6x94_NbPs0ResTmrMin_OFFSET                          29
#define D18F6x94_NbPs0ResTmrMin_WIDTH                           3
#define D18F6x94_NbPs0ResTmrMin_MASK                            0xe0000000

/// D18F6x94
typedef union {
  struct {                                                              ///<
    UINT32                                             CpuPstateThr:3 ; ///<
    UINT32                                           CpuPstateThrEn:1 ; ///<
    UINT32                                         NbPsNoTransOnDma:1 ; ///<
    UINT32                                            Reserved_19_5:15; ///<
    UINT32                                           NbPsNonC0Timer:3 ; ///<
    UINT32                                              NbPsC0Timer:3 ; ///<
    UINT32                                           NbPs1ResTmrMin:3 ; ///<
    UINT32                                           NbPs0ResTmrMin:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F6x94_STRUCT;

// **** D18F6x98 Register Definition ****
// Address
#define D18F6x98_ADDRESS                                        0x98

// Type
#define D18F6x98_TYPE                                           TYPE_D18F6
// Field Data
#define D18F6x98_NbPsTransInFlight_OFFSET                       0
#define D18F6x98_NbPsTransInFlight_WIDTH                        1
#define D18F6x98_NbPsTransInFlight_MASK                         0x1
#define D18F6x98_NbPs1ActSts_OFFSET                             1
#define D18F6x98_NbPs1ActSts_WIDTH                              1
#define D18F6x98_NbPs1ActSts_MASK                               0x2
#define D18F6x98_NbPs1Act_OFFSET                                2
#define D18F6x98_NbPs1Act_WIDTH                                 1
#define D18F6x98_NbPs1Act_MASK                                  0x4
#define D18F6x98_Reserved_29_3_OFFSET                           3
#define D18F6x98_Reserved_29_3_WIDTH                            27
#define D18F6x98_Reserved_29_3_MASK                             0x3ffffff8
#define D18F6x98_NbPsCsrAccSel_OFFSET                           30
#define D18F6x98_NbPsCsrAccSel_WIDTH                            1
#define D18F6x98_NbPsCsrAccSel_MASK                             0x40000000
#define D18F6x98_NbPsDbgEn_OFFSET                               31
#define D18F6x98_NbPsDbgEn_WIDTH                                1
#define D18F6x98_NbPsDbgEn_MASK                                 0x80000000

/// D18F6x98
typedef union {
  struct {                                                              ///<
    UINT32                                        NbPsTransInFlight:1 ; ///<
    UINT32                                              NbPs1ActSts:1 ; ///<
    UINT32                                                 NbPs1Act:1 ; ///<
    UINT32                                            Reserved_29_3:27; ///<
    UINT32                                            NbPsCsrAccSel:1 ; ///<
    UINT32                                                NbPsDbgEn:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F6x98_STRUCT;

// **** D18F6x9C Register Definition ****
// Address
#define D18F6x9C_ADDRESS                                        0x9c

// Type
#define D18F6x9C_TYPE                                           TYPE_D18F6
// Field Data
#define D18F6x9C_NclkRedDiv_OFFSET                              0
#define D18F6x9C_NclkRedDiv_WIDTH                               7
#define D18F6x9C_NclkRedDiv_MASK                                0x7f
#define D18F6x9C_NclkRedSelfRefrAlways_OFFSET                   7
#define D18F6x9C_NclkRedSelfRefrAlways_WIDTH                    1
#define D18F6x9C_NclkRedSelfRefrAlways_MASK                     0x80
#define D18F6x9C_NclkRampWithDllRelock_OFFSET                   8
#define D18F6x9C_NclkRampWithDllRelock_WIDTH                    1
#define D18F6x9C_NclkRampWithDllRelock_MASK                     0x100
#define D18F6x9C_Reserved_31_9_OFFSET                           9
#define D18F6x9C_Reserved_31_9_WIDTH                            23
#define D18F6x9C_Reserved_31_9_MASK                             0xfffffe00

/// D18F6x9C
typedef union {
  struct {                                                              ///<
    UINT32                                               NclkRedDiv:7 ; ///<
    UINT32                                    NclkRedSelfRefrAlways:1 ; ///<
    UINT32                                    NclkRampWithDllRelock:1 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F6x9C_STRUCT;

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

// **** DxF0x04 Register Definition ****
// Address
#define DxF0x04_ADDRESS                                         0x4

// Type
#define DxF0x04_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x04_IoAccessEn_OFFSET                               0
#define DxF0x04_IoAccessEn_WIDTH                                1
#define DxF0x04_IoAccessEn_MASK                                 0x1
#define DxF0x04_MemAccessEn_OFFSET                              1
#define DxF0x04_MemAccessEn_WIDTH                               1
#define DxF0x04_MemAccessEn_MASK                                0x2
#define DxF0x04_BusMasterEn_OFFSET                              2
#define DxF0x04_BusMasterEn_WIDTH                               1
#define DxF0x04_BusMasterEn_MASK                                0x4
#define DxF0x04_SpecialCycleEn_OFFSET                           3
#define DxF0x04_SpecialCycleEn_WIDTH                            1
#define DxF0x04_SpecialCycleEn_MASK                             0x8
#define DxF0x04_MemWriteInvalidateEn_OFFSET                     4
#define DxF0x04_MemWriteInvalidateEn_WIDTH                      1
#define DxF0x04_MemWriteInvalidateEn_MASK                       0x10
#define DxF0x04_PalSnoopEn_OFFSET                               5
#define DxF0x04_PalSnoopEn_WIDTH                                1
#define DxF0x04_PalSnoopEn_MASK                                 0x20
#define DxF0x04_ParityErrorEn_OFFSET                            6
#define DxF0x04_ParityErrorEn_WIDTH                             1
#define DxF0x04_ParityErrorEn_MASK                              0x40
#define DxF0x04_IdselStepping_OFFSET                            7
#define DxF0x04_IdselStepping_WIDTH                             1
#define DxF0x04_IdselStepping_MASK                              0x80
#define DxF0x04_SerrEn_OFFSET                                   8
#define DxF0x04_SerrEn_WIDTH                                    1
#define DxF0x04_SerrEn_MASK                                     0x100
#define DxF0x04_FastB2BEn_OFFSET                                9
#define DxF0x04_FastB2BEn_WIDTH                                 1
#define DxF0x04_FastB2BEn_MASK                                  0x200
#define DxF0x04_IntDis_OFFSET                                   10
#define DxF0x04_IntDis_WIDTH                                    1
#define DxF0x04_IntDis_MASK                                     0x400
#define DxF0x04_Reserved_18_11_OFFSET                           11
#define DxF0x04_Reserved_18_11_WIDTH                            8
#define DxF0x04_Reserved_18_11_MASK                             0x7f800
#define DxF0x04_IntStatus_OFFSET                                19
#define DxF0x04_IntStatus_WIDTH                                 1
#define DxF0x04_IntStatus_MASK                                  0x80000
#define DxF0x04_CapList_OFFSET                                  20
#define DxF0x04_CapList_WIDTH                                   1
#define DxF0x04_CapList_MASK                                    0x100000
#define DxF0x04_PCI66En_OFFSET                                  21
#define DxF0x04_PCI66En_WIDTH                                   1
#define DxF0x04_PCI66En_MASK                                    0x200000
#define DxF0x04_UDFEn_OFFSET                                    22
#define DxF0x04_UDFEn_WIDTH                                     1
#define DxF0x04_UDFEn_MASK                                      0x400000
#define DxF0x04_FastBackCapable_OFFSET                          23
#define DxF0x04_FastBackCapable_WIDTH                           1
#define DxF0x04_FastBackCapable_MASK                            0x800000
#define DxF0x04_MasterDataPerr_OFFSET                           24
#define DxF0x04_MasterDataPerr_WIDTH                            1
#define DxF0x04_MasterDataPerr_MASK                             0x1000000
#define DxF0x04_DevselTiming_OFFSET                             25
#define DxF0x04_DevselTiming_WIDTH                              2
#define DxF0x04_DevselTiming_MASK                               0x6000000
#define DxF0x04_SignaledTargetAbort_OFFSET                      27
#define DxF0x04_SignaledTargetAbort_WIDTH                       1
#define DxF0x04_SignaledTargetAbort_MASK                        0x8000000
#define DxF0x04_ReceivedTargetAbort_OFFSET                      28
#define DxF0x04_ReceivedTargetAbort_WIDTH                       1
#define DxF0x04_ReceivedTargetAbort_MASK                        0x10000000
#define DxF0x04_ReceivedMasterAbort_OFFSET                      29
#define DxF0x04_ReceivedMasterAbort_WIDTH                       1
#define DxF0x04_ReceivedMasterAbort_MASK                        0x20000000
#define DxF0x04_SignaledSystemError_OFFSET                      30
#define DxF0x04_SignaledSystemError_WIDTH                       1
#define DxF0x04_SignaledSystemError_MASK                        0x40000000
#define DxF0x04_ParityErrorDetected_OFFSET                      31
#define DxF0x04_ParityErrorDetected_WIDTH                       1
#define DxF0x04_ParityErrorDetected_MASK                        0x80000000

/// DxF0x04
typedef union {
  struct {                                                              ///<
    UINT32                                               IoAccessEn:1 ; ///<
    UINT32                                              MemAccessEn:1 ; ///<
    UINT32                                              BusMasterEn:1 ; ///<
    UINT32                                           SpecialCycleEn:1 ; ///<
    UINT32                                     MemWriteInvalidateEn:1 ; ///<
    UINT32                                               PalSnoopEn:1 ; ///<
    UINT32                                            ParityErrorEn:1 ; ///<
    UINT32                                            IdselStepping:1 ; ///<
    UINT32                                                   SerrEn:1 ; ///<
    UINT32                                                FastB2BEn:1 ; ///<
    UINT32                                                   IntDis:1 ; ///<
    UINT32                                           Reserved_18_11:8 ; ///<
    UINT32                                                IntStatus:1 ; ///<
    UINT32                                                  CapList:1 ; ///<
    UINT32                                                  PCI66En:1 ; ///<
    UINT32                                                    UDFEn:1 ; ///<
    UINT32                                          FastBackCapable:1 ; ///<
    UINT32                                           MasterDataPerr:1 ; ///<
    UINT32                                             DevselTiming:2 ; ///<
    UINT32                                      SignaledTargetAbort:1 ; ///<
    UINT32                                      ReceivedTargetAbort:1 ; ///<
    UINT32                                      ReceivedMasterAbort:1 ; ///<
    UINT32                                      SignaledSystemError:1 ; ///<
    UINT32                                      ParityErrorDetected:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x04_STRUCT;

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

// **** DxF0x1C Register Definition ****
// Address
#define DxF0x1C_ADDRESS                                         0x1c

// Type
#define DxF0x1C_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x1C_Reserved_3_0_OFFSET                             0
#define DxF0x1C_Reserved_3_0_WIDTH                              4
#define DxF0x1C_Reserved_3_0_MASK                               0xf
#define DxF0x1C_IOBase_15_12__OFFSET                            4
#define DxF0x1C_IOBase_15_12__WIDTH                             4
#define DxF0x1C_IOBase_15_12__MASK                              0xf0
#define DxF0x1C_Reserved_11_8_OFFSET                            8
#define DxF0x1C_Reserved_11_8_WIDTH                             4
#define DxF0x1C_Reserved_11_8_MASK                              0xf00
#define DxF0x1C_IOLimit_15_12__OFFSET                           12
#define DxF0x1C_IOLimit_15_12__WIDTH                            4
#define DxF0x1C_IOLimit_15_12__MASK                             0xf000
#define DxF0x1C_Reserved_19_16_OFFSET                           16
#define DxF0x1C_Reserved_19_16_WIDTH                            4
#define DxF0x1C_Reserved_19_16_MASK                             0xf0000
#define DxF0x1C_CapList_OFFSET                                  20
#define DxF0x1C_CapList_WIDTH                                   1
#define DxF0x1C_CapList_MASK                                    0x100000
#define DxF0x1C_PCI66En_OFFSET                                  21
#define DxF0x1C_PCI66En_WIDTH                                   1
#define DxF0x1C_PCI66En_MASK                                    0x200000
#define DxF0x1C_UDFEn_OFFSET                                    22
#define DxF0x1C_UDFEn_WIDTH                                     1
#define DxF0x1C_UDFEn_MASK                                      0x400000
#define DxF0x1C_FastBackCapable_OFFSET                          23
#define DxF0x1C_FastBackCapable_WIDTH                           1
#define DxF0x1C_FastBackCapable_MASK                            0x800000
#define DxF0x1C_MasterDataPerr_OFFSET                           24
#define DxF0x1C_MasterDataPerr_WIDTH                            1
#define DxF0x1C_MasterDataPerr_MASK                             0x1000000
#define DxF0x1C_DevselTiming_OFFSET                             25
#define DxF0x1C_DevselTiming_WIDTH                              2
#define DxF0x1C_DevselTiming_MASK                               0x6000000
#define DxF0x1C_SignalTargetAbort_OFFSET                        27
#define DxF0x1C_SignalTargetAbort_WIDTH                         1
#define DxF0x1C_SignalTargetAbort_MASK                          0x8000000
#define DxF0x1C_ReceivedTargetAbort_OFFSET                      28
#define DxF0x1C_ReceivedTargetAbort_WIDTH                       1
#define DxF0x1C_ReceivedTargetAbort_MASK                        0x10000000
#define DxF0x1C_ReceivedMasterAbort_OFFSET                      29
#define DxF0x1C_ReceivedMasterAbort_WIDTH                       1
#define DxF0x1C_ReceivedMasterAbort_MASK                        0x20000000
#define DxF0x1C_ReceivedSystemError_OFFSET                      30
#define DxF0x1C_ReceivedSystemError_WIDTH                       1
#define DxF0x1C_ReceivedSystemError_MASK                        0x40000000
#define DxF0x1C_ParityErrorDetected_OFFSET                      31
#define DxF0x1C_ParityErrorDetected_WIDTH                       1
#define DxF0x1C_ParityErrorDetected_MASK                        0x80000000

/// DxF0x1C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                            IOBase_15_12_:4 ; ///<
    UINT32                                            Reserved_11_8:4 ; ///<
    UINT32                                           IOLimit_15_12_:4 ; ///<
    UINT32                                           Reserved_19_16:4 ; ///<
    UINT32                                                  CapList:1 ; ///<
    UINT32                                                  PCI66En:1 ; ///<
    UINT32                                                    UDFEn:1 ; ///<
    UINT32                                          FastBackCapable:1 ; ///<
    UINT32                                           MasterDataPerr:1 ; ///<
    UINT32                                             DevselTiming:2 ; ///<
    UINT32                                        SignalTargetAbort:1 ; ///<
    UINT32                                      ReceivedTargetAbort:1 ; ///<
    UINT32                                      ReceivedMasterAbort:1 ; ///<
    UINT32                                      ReceivedSystemError:1 ; ///<
    UINT32                                      ParityErrorDetected:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x1C_STRUCT;

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
#define DxF0x3C_Reserved_15_11_OFFSET                           11
#define DxF0x3C_Reserved_15_11_WIDTH                            5
#define DxF0x3C_Reserved_15_11_MASK                             0xf800
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
    UINT32                                           Reserved_15_11:5 ; ///<
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

// **** DxF0x64 Register Definition ****
// Address
#define DxF0x64_ADDRESS                                         0x64

// Type
#define DxF0x64_TYPE                                            TYPE_D4F0
// Field Data
#define DxF0x64_LinkSpeed_OFFSET                                0
#define DxF0x64_LinkSpeed_WIDTH                                 4
#define DxF0x64_LinkSpeed_MASK                                  0xf
#define DxF0x64_LinkWidth_OFFSET                                4
#define DxF0x64_LinkWidth_WIDTH                                 6
#define DxF0x64_LinkWidth_MASK                                  0x3f0
#define DxF0x64_PMSupport_OFFSET                                10
#define DxF0x64_PMSupport_WIDTH                                 2
#define DxF0x64_PMSupport_MASK                                  0xc00
#define DxF0x64_L0sExitLatency_OFFSET                           12
#define DxF0x64_L0sExitLatency_WIDTH                            3
#define DxF0x64_L0sExitLatency_MASK                             0x7000
#define DxF0x64_L1ExitLatency_OFFSET                            15
#define DxF0x64_L1ExitLatency_WIDTH                             3
#define DxF0x64_L1ExitLatency_MASK                              0x38000
#define DxF0x64_ClockPowerManagement_OFFSET                     18
#define DxF0x64_ClockPowerManagement_WIDTH                      1
#define DxF0x64_ClockPowerManagement_MASK                       0x40000
#define DxF0x64_Reserved_19_19_OFFSET                           19
#define DxF0x64_Reserved_19_19_WIDTH                            1
#define DxF0x64_Reserved_19_19_MASK                             0x80000
#define DxF0x64_DlActiveReportingCapable_OFFSET                 20
#define DxF0x64_DlActiveReportingCapable_WIDTH                  1
#define DxF0x64_DlActiveReportingCapable_MASK                   0x100000
#define DxF0x64_LinkBWNotificationCap_OFFSET                    21
#define DxF0x64_LinkBWNotificationCap_WIDTH                     1
#define DxF0x64_LinkBWNotificationCap_MASK                      0x200000
#define DxF0x64_Reserved_23_22_OFFSET                           22
#define DxF0x64_Reserved_23_22_WIDTH                            2
#define DxF0x64_Reserved_23_22_MASK                             0xc00000
#define DxF0x64_PortNumber_OFFSET                               24
#define DxF0x64_PortNumber_WIDTH                                8
#define DxF0x64_PortNumber_MASK                                 0xff000000

/// DxF0x64
typedef union {
  struct {                                                              ///<
    UINT32                                                LinkSpeed:4 ; ///<
    UINT32                                                LinkWidth:6 ; ///<
    UINT32                                                PMSupport:2 ; ///<
    UINT32                                           L0sExitLatency:3 ; ///<
    UINT32                                            L1ExitLatency:3 ; ///<
    UINT32                                     ClockPowerManagement:1 ; ///<
    UINT32                                           Reserved_19_19:1 ; ///<
    UINT32                                 DlActiveReportingCapable:1 ; ///<
    UINT32                                    LinkBWNotificationCap:1 ; ///<
    UINT32                                           Reserved_23_22:2 ; ///<
    UINT32                                               PortNumber:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0x64_STRUCT;

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

// **** FCRxFE00_6000 Register Definition ****
// Address
#define FCRxFE00_6000_ADDRESS                                   0xfe006000

// Type
#define FCRxFE00_6000_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_6000_Reserved_6_0_OFFSET                       0
#define FCRxFE00_6000_Reserved_6_0_WIDTH                        7
#define FCRxFE00_6000_Reserved_6_0_MASK                         0x7f
#define FCRxFE00_6000_NbPs0Vid_OFFSET                           7
#define FCRxFE00_6000_NbPs0Vid_WIDTH                            7
#define FCRxFE00_6000_NbPs0Vid_MASK                             0x3f80
#define FCRxFE00_6000_NbPs1Vid_OFFSET                           14
#define FCRxFE00_6000_NbPs1Vid_WIDTH                            7
#define FCRxFE00_6000_NbPs1Vid_MASK                             0x1fc000
#define FCRxFE00_6000_Reserved_31_21_OFFSET                     21
#define FCRxFE00_6000_Reserved_31_21_WIDTH                      11
#define FCRxFE00_6000_Reserved_31_21_MASK                       0xffe00000

/// FCRxFE00_6000
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_6_0:7 ; ///<
    UINT32                                                 NbPs0Vid:7 ; ///<
    UINT32                                                 NbPs1Vid:7 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_6000_STRUCT;

// **** FCRxFE00_6002 Register Definition ****
// Address
#define FCRxFE00_6002_ADDRESS                                   0xfe006002

// Type
#define FCRxFE00_6002_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_6002_Reserved_4_0_OFFSET                       0
#define FCRxFE00_6002_Reserved_4_0_WIDTH                        5
#define FCRxFE00_6002_Reserved_4_0_MASK                         0x1f
#define FCRxFE00_6002_NbPs1VidAddl_OFFSET                       5
#define FCRxFE00_6002_NbPs1VidAddl_WIDTH                        7
#define FCRxFE00_6002_NbPs1VidAddl_MASK                         0xfe0
#define FCRxFE00_6002_NbPs1VidHigh_OFFSET                       12
#define FCRxFE00_6002_NbPs1VidHigh_WIDTH                        7
#define FCRxFE00_6002_NbPs1VidHigh_MASK                         0x7f000
#define FCRxFE00_6002_Reserved_31_19_OFFSET                     19
#define FCRxFE00_6002_Reserved_31_19_WIDTH                      13
#define FCRxFE00_6002_Reserved_31_19_MASK                       0xfff80000

/// FCRxFE00_6002
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                             NbPs1VidAddl:7 ; ///<
    UINT32                                             NbPs1VidHigh:7 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_6002_STRUCT;

// **** FCRxFE00_7006 Register Definition ****
// Address
#define FCRxFE00_7006_ADDRESS                                   0xfe007006

// Type
#define FCRxFE00_7006_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_7006_Reserved_13_0_OFFSET                      0
#define FCRxFE00_7006_Reserved_13_0_WIDTH                       14
#define FCRxFE00_7006_Reserved_13_0_MASK                        0x3fff
#define FCRxFE00_7006_NbPs1NclkDiv_OFFSET                       14
#define FCRxFE00_7006_NbPs1NclkDiv_WIDTH                        7
#define FCRxFE00_7006_NbPs1NclkDiv_MASK                         0x1fc000
#define FCRxFE00_7006_MaxNbFreqAtMinVid_OFFSET                  21
#define FCRxFE00_7006_MaxNbFreqAtMinVid_WIDTH                   5
#define FCRxFE00_7006_MaxNbFreqAtMinVid_MASK                    0x3e00000
#define FCRxFE00_7006_Reserved_31_26_OFFSET                     26
#define FCRxFE00_7006_Reserved_31_26_WIDTH                      6
#define FCRxFE00_7006_Reserved_31_26_MASK                       0xfc000000

/// FCRxFE00_7006
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_13_0:14; ///<
    UINT32                                             NbPs1NclkDiv:7 ; ///<
    UINT32                                        MaxNbFreqAtMinVid:5 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_7006_STRUCT;

// **** FCRxFE00_7009 Register Definition ****
// Address
#define FCRxFE00_7009_ADDRESS                                   0xfe007009

// Type
#define FCRxFE00_7009_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_7009_Reserved_1_0_OFFSET                       0
#define FCRxFE00_7009_Reserved_1_0_WIDTH                        2
#define FCRxFE00_7009_Reserved_1_0_MASK                         0x3
#define FCRxFE00_7009_NbPs0NclkDiv_OFFSET                       2
#define FCRxFE00_7009_NbPs0NclkDiv_WIDTH                        7
#define FCRxFE00_7009_NbPs0NclkDiv_MASK                         0x1fc
#define FCRxFE00_7009_Reserved_31_9_OFFSET                      9
#define FCRxFE00_7009_Reserved_31_9_WIDTH                       23
#define FCRxFE00_7009_Reserved_31_9_MASK                        0xfffffe00

/// FCRxFE00_7009
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                             NbPs0NclkDiv:7 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_7009_STRUCT;


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
#define D0F0x64_x0C_Reserved_3_0_OFFSET                         0
#define D0F0x64_x0C_Reserved_3_0_WIDTH                          4
#define D0F0x64_x0C_Reserved_3_0_MASK                           0xf
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
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                            Dev4BridgeDis:1 ; ///<
    UINT32                                            Dev5BridgeDis:1 ; ///<
    UINT32                                            Dev6BridgeDis:1 ; ///<
    UINT32                                            Dev7BridgeDis:1 ; ///<
    UINT32                                            Reserved_31_8:24; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x0C_STRUCT;

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
#define D0F0x64_x1A_Tom2_35_32__OFFSET                          0
#define D0F0x64_x1A_Tom2_35_32__WIDTH                           4
#define D0F0x64_x1A_Tom2_35_32__MASK                            0xf
#define D0F0x64_x1A_Reserved_31_4_OFFSET                        4
#define D0F0x64_x1A_Reserved_31_4_WIDTH                         28
#define D0F0x64_x1A_Reserved_31_4_MASK                          0xfffffff0

/// D0F0x64_x1A
typedef union {
  struct {                                                              ///<
    UINT32                                              Tom2_35_32_:4 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
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

// **** D0F0x64_x20 Register Definition ****
// Address
#define D0F0x64_x20_ADDRESS                                     0x20

// Type
#define D0F0x64_x20_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x20_Reserved_0_0_OFFSET                         0
#define D0F0x64_x20_Reserved_0_0_WIDTH                          1
#define D0F0x64_x20_Reserved_0_0_MASK                           0x1
#define D0F0x64_x20_PcieDevRemapDis_OFFSET                      1
#define D0F0x64_x20_PcieDevRemapDis_WIDTH                       1
#define D0F0x64_x20_PcieDevRemapDis_MASK                        0x2
#define D0F0x64_x20_Reserved_31_2_OFFSET                        2
#define D0F0x64_x20_Reserved_31_2_WIDTH                         30
#define D0F0x64_x20_Reserved_31_2_MASK                          0xfffffffc

/// D0F0x64_x20
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                          PcieDevRemapDis:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x20_STRUCT;

// **** D0F0x64_x46 Register Definition ****
// Address
#define D0F0x64_x46_ADDRESS                                     0x46

// Type
#define D0F0x64_x46_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x46_Reserved_0_0_OFFSET                         0
#define D0F0x64_x46_Reserved_0_0_WIDTH                          1
#define D0F0x64_x46_Reserved_0_0_MASK                           0x1
#define D0F0x64_x46_P2PMode_OFFSET                              1
#define D0F0x64_x46_P2PMode_WIDTH                               2
#define D0F0x64_x46_P2PMode_MASK                                0x6
#define D0F0x64_x46_Reserved_15_3_OFFSET                        3
#define D0F0x64_x46_Reserved_15_3_WIDTH                         13
#define D0F0x64_x46_Reserved_15_3_MASK                          0xfff8
#define D0F0x64_x46_Msi64bitEn_OFFSET                           16
#define D0F0x64_x46_Msi64bitEn_WIDTH                            1
#define D0F0x64_x46_Msi64bitEn_MASK                             0x10000
#define D0F0x64_x46_Reserved_31_17_OFFSET                       17
#define D0F0x64_x46_Reserved_31_17_WIDTH                        15
#define D0F0x64_x46_Reserved_31_17_MASK                         0xfffe0000

/// D0F0x64_x46
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                                  P2PMode:2 ; ///<
    UINT32                                            Reserved_15_3:13; ///<
    UINT32                                               Msi64bitEn:1 ; ///<
    UINT32                                           Reserved_31_17:15; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x46_STRUCT;

// **** D0F0x64_x4D Register Definition ****
// Address
#define D0F0x64_x4D_ADDRESS                                     0x4d

// Type
#define D0F0x64_x4D_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x4D_WriteData_OFFSET                            0
#define D0F0x64_x4D_WriteData_WIDTH                             16
#define D0F0x64_x4D_WriteData_MASK                              0xffff
#define D0F0x64_x4D_SmuAddr_OFFSET                              16
#define D0F0x64_x4D_SmuAddr_WIDTH                               8
#define D0F0x64_x4D_SmuAddr_MASK                                0xff0000
#define D0F0x64_x4D_ReqToggle_OFFSET                            24
#define D0F0x64_x4D_ReqToggle_WIDTH                             1
#define D0F0x64_x4D_ReqToggle_MASK                              0x1000000
#define D0F0x64_x4D_ReqType_OFFSET                              25
#define D0F0x64_x4D_ReqType_WIDTH                               1
#define D0F0x64_x4D_ReqType_MASK                                0x2000000
#define D0F0x64_x4D_Reserved_31_26_OFFSET                       26
#define D0F0x64_x4D_Reserved_31_26_WIDTH                        6
#define D0F0x64_x4D_Reserved_31_26_MASK                         0xfc000000

/// D0F0x64_x4D
typedef union {
  struct {                                                              ///<
    UINT32                                                WriteData:16; ///<
    UINT32                                                  SmuAddr:8 ; ///<
    UINT32                                                ReqToggle:1 ; ///<
    UINT32                                                  ReqType:1 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x4D_STRUCT;

// **** D0F0x64_x4E Register Definition ****
// Address
#define D0F0x64_x4E_ADDRESS                                     0x4e

// Type
#define D0F0x64_x4E_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x4E_SmuReadData_OFFSET                          0
#define D0F0x64_x4E_SmuReadData_WIDTH                           32
#define D0F0x64_x4E_SmuReadData_MASK                            0xffffffff

/// D0F0x64_x4E
typedef union {
  struct {                                                              ///<
    UINT32                                              SmuReadData:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x4E_STRUCT;

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

// **** D0F0x64_x6A Register Definition ****
// Address
#define D0F0x64_x6A_ADDRESS                                     0x6a

// Type
#define D0F0x64_x6A_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x6A_VoltageForceEn_OFFSET                       0
#define D0F0x64_x6A_VoltageForceEn_WIDTH                        1
#define D0F0x64_x6A_VoltageForceEn_MASK                         0x1
#define D0F0x64_x6A_VoltageChangeEn_OFFSET                      1
#define D0F0x64_x6A_VoltageChangeEn_WIDTH                       1
#define D0F0x64_x6A_VoltageChangeEn_MASK                        0x2
#define D0F0x64_x6A_VoltageChangeReq_OFFSET                     2
#define D0F0x64_x6A_VoltageChangeReq_WIDTH                      1
#define D0F0x64_x6A_VoltageChangeReq_MASK                       0x4
#define D0F0x64_x6A_VoltageLevel_OFFSET                         3
#define D0F0x64_x6A_VoltageLevel_WIDTH                          2
#define D0F0x64_x6A_VoltageLevel_MASK                           0x18
#define D0F0x64_x6A_Reserved_31_5_OFFSET                        5
#define D0F0x64_x6A_Reserved_31_5_WIDTH                         27
#define D0F0x64_x6A_Reserved_31_5_MASK                          0xffffffe0

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
} D0F0x64_x6A_STRUCT;

// **** D0F0x64_x6B Register Definition ****
// Address
#define D0F0x64_x6B_ADDRESS                                     0x6b

// Type
#define D0F0x64_x6B_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x6B_VoltageChangeAck_OFFSET                     0
#define D0F0x64_x6B_VoltageChangeAck_WIDTH                      1
#define D0F0x64_x6B_VoltageChangeAck_MASK                       0x1
#define D0F0x64_x6B_CurrentVoltageLevel_OFFSET                  1
#define D0F0x64_x6B_CurrentVoltageLevel_WIDTH                   2
#define D0F0x64_x6B_CurrentVoltageLevel_MASK                    0x6
#define D0F0x64_x6B_Reserved_31_3_OFFSET                        3
#define D0F0x64_x6B_Reserved_31_3_WIDTH                         29
#define D0F0x64_x6B_Reserved_31_3_MASK                          0xfffffff8

/// D0F0x64_x6B
typedef union {
  struct {                                                              ///<
    UINT32                                         VoltageChangeAck:1 ; ///<
    UINT32                                      CurrentVoltageLevel:2 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x6B_STRUCT;

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

// **** D0F0x98_x07 Register Definition ****
// Address
#define D0F0x98_x07_ADDRESS                                     0x7

// Type
#define D0F0x98_x07_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x07_IocBwOptEn_OFFSET                           0
#define D0F0x98_x07_IocBwOptEn_WIDTH                            1
#define D0F0x98_x07_IocBwOptEn_MASK                             0x1
#define D0F0x98_x07_Reserved_13_1_OFFSET                        1
#define D0F0x98_x07_Reserved_13_1_WIDTH                         13
#define D0F0x98_x07_Reserved_13_1_MASK                          0x3ffe
#define D0F0x98_x07_MSIHTIntConversionEn_OFFSET                 14
#define D0F0x98_x07_MSIHTIntConversionEn_WIDTH                  1
#define D0F0x98_x07_MSIHTIntConversionEn_MASK                   0x4000
#define D0F0x98_x07_DropZeroMaskWrEn_OFFSET                     15
#define D0F0x98_x07_DropZeroMaskWrEn_WIDTH                      1
#define D0F0x98_x07_DropZeroMaskWrEn_MASK                       0x8000
#define D0F0x98_x07_Reserved_31_16_OFFSET                       16
#define D0F0x98_x07_Reserved_31_16_WIDTH                        16
#define D0F0x98_x07_Reserved_31_16_MASK                         0xffff0000

/// D0F0x98_x07
typedef union {
  struct {                                                              ///<
    UINT32                                               IocBwOptEn:1 ; ///<
    UINT32                                            Reserved_13_1:13; ///<
    UINT32                                     MSIHTIntConversionEn:1 ; ///<
    UINT32                                         DropZeroMaskWrEn:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x07_STRUCT;

// **** D0F0x98_x08 Register Definition ****
// Address
#define D0F0x98_x08_ADDRESS                                     0x8

// Type
#define D0F0x98_x08_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x08_NpWrrLenA_OFFSET                            0
#define D0F0x98_x08_NpWrrLenA_WIDTH                             8
#define D0F0x98_x08_NpWrrLenA_MASK                              0xff
#define D0F0x98_x08_Reserved_15_8_OFFSET                        8
#define D0F0x98_x08_Reserved_15_8_WIDTH                         8
#define D0F0x98_x08_Reserved_15_8_MASK                          0xff00
#define D0F0x98_x08_NpWrrLenC_OFFSET                            16
#define D0F0x98_x08_NpWrrLenC_WIDTH                             8
#define D0F0x98_x08_NpWrrLenC_MASK                              0xff0000
#define D0F0x98_x08_Reserved_31_24_OFFSET                       24
#define D0F0x98_x08_Reserved_31_24_WIDTH                        8
#define D0F0x98_x08_Reserved_31_24_MASK                         0xff000000

/// D0F0x98_x08
typedef union {
  struct {                                                              ///<
    UINT32                                                NpWrrLenA:8 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                                NpWrrLenC:8 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x08_STRUCT;

// **** D0F0x98_x09 Register Definition ****
// Address
#define D0F0x98_x09_ADDRESS                                     0x9

// Type
#define D0F0x98_x09_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x09_PWrrLenA_OFFSET                             0
#define D0F0x98_x09_PWrrLenA_WIDTH                              8
#define D0F0x98_x09_PWrrLenA_MASK                               0xff
#define D0F0x98_x09_Reserved_23_8_OFFSET                        8
#define D0F0x98_x09_Reserved_23_8_WIDTH                         16
#define D0F0x98_x09_Reserved_23_8_MASK                          0xffff00
#define D0F0x98_x09_PWrrLenD_OFFSET                             24
#define D0F0x98_x09_PWrrLenD_WIDTH                              8
#define D0F0x98_x09_PWrrLenD_MASK                               0xff000000

/// D0F0x98_x09
typedef union {
  struct {                                                              ///<
    UINT32                                                 PWrrLenA:8 ; ///<
    UINT32                                            Reserved_23_8:16; ///<
    UINT32                                                 PWrrLenD:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x09_STRUCT;

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
#define D0F0x98_x0C_Reserved_31_31_OFFSET                       31
#define D0F0x98_x0C_Reserved_31_31_WIDTH                        1
#define D0F0x98_x0C_Reserved_31_31_MASK                         0x80000000

/// D0F0x98_x0C
typedef union {
  struct {                                                              ///<
    UINT32                                               GcmWrrLenA:8 ; ///<
    UINT32                                               GcmWrrLenB:8 ; ///<
    UINT32                                           Reserved_29_16:14; ///<
    UINT32                                        StrictSelWinnerEn:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x0C_STRUCT;

// **** D0F0x98_x0E Register Definition ****
// Address
#define D0F0x98_x0E_ADDRESS                                     0xe

// Type
#define D0F0x98_x0E_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x0E_MsiHtRsvIntRemapEn_OFFSET                   0
#define D0F0x98_x0E_MsiHtRsvIntRemapEn_WIDTH                    1
#define D0F0x98_x0E_MsiHtRsvIntRemapEn_MASK                     0x1
#define D0F0x98_x0E_Reserved_1_1_OFFSET                         1
#define D0F0x98_x0E_Reserved_1_1_WIDTH                          1
#define D0F0x98_x0E_Reserved_1_1_MASK                           0x2
#define D0F0x98_x0E_MsiHtRsvIntMt_OFFSET                        2
#define D0F0x98_x0E_MsiHtRsvIntMt_WIDTH                         3
#define D0F0x98_x0E_MsiHtRsvIntMt_MASK                          0x1c
#define D0F0x98_x0E_MsiHtRsvIntRqEoi_OFFSET                     5
#define D0F0x98_x0E_MsiHtRsvIntRqEoi_WIDTH                      1
#define D0F0x98_x0E_MsiHtRsvIntRqEoi_MASK                       0x20
#define D0F0x98_x0E_MsiHtRsvIntDM_OFFSET                        6
#define D0F0x98_x0E_MsiHtRsvIntDM_WIDTH                         1
#define D0F0x98_x0E_MsiHtRsvIntDM_MASK                          0x40
#define D0F0x98_x0E_Reserved_7_7_OFFSET                         7
#define D0F0x98_x0E_Reserved_7_7_WIDTH                          1
#define D0F0x98_x0E_Reserved_7_7_MASK                           0x80
#define D0F0x98_x0E_MsiHtRsvIntDestination_OFFSET               8
#define D0F0x98_x0E_MsiHtRsvIntDestination_WIDTH                8
#define D0F0x98_x0E_MsiHtRsvIntDestination_MASK                 0xff00
#define D0F0x98_x0E_MsiHtRsvIntVector_OFFSET                    16
#define D0F0x98_x0E_MsiHtRsvIntVector_WIDTH                     8
#define D0F0x98_x0E_MsiHtRsvIntVector_MASK                      0xff0000
#define D0F0x98_x0E_Reserved_31_24_OFFSET                       24
#define D0F0x98_x0E_Reserved_31_24_WIDTH                        8
#define D0F0x98_x0E_Reserved_31_24_MASK                         0xff000000

/// D0F0x98_x0E
typedef union {
  struct {                                                              ///<
    UINT32                                       MsiHtRsvIntRemapEn:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                            MsiHtRsvIntMt:3 ; ///<
    UINT32                                         MsiHtRsvIntRqEoi:1 ; ///<
    UINT32                                            MsiHtRsvIntDM:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                   MsiHtRsvIntDestination:8 ; ///<
    UINT32                                        MsiHtRsvIntVector:8 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x0E_STRUCT;

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

// **** D0F0x98_x28 Register Definition ****
// Address
#define D0F0x98_x28_ADDRESS                                     0x28

// Type
#define D0F0x98_x28_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x28_SmuPmInterfaceEn_OFFSET                     0
#define D0F0x98_x28_SmuPmInterfaceEn_WIDTH                      1
#define D0F0x98_x28_SmuPmInterfaceEn_MASK                       0x1
#define D0F0x98_x28_ForceCoherentIntr_OFFSET                    1
#define D0F0x98_x28_ForceCoherentIntr_WIDTH                     1
#define D0F0x98_x28_ForceCoherentIntr_MASK                      0x2
#define D0F0x98_x28_Reserved_31_2_OFFSET                        2
#define D0F0x98_x28_Reserved_31_2_WIDTH                         30
#define D0F0x98_x28_Reserved_31_2_MASK                          0xfffffffc

/// D0F0x98_x28
typedef union {
  struct {                                                              ///<
    UINT32                                         SmuPmInterfaceEn:1 ; ///<
    UINT32                                        ForceCoherentIntr:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x28_STRUCT;

// **** D0F0x98_x2C Register Definition ****
// Address
#define D0F0x98_x2C_ADDRESS                                     0x2c

// Type
#define D0F0x98_x2C_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x2C_Reserved_0_0_OFFSET                         0
#define D0F0x98_x2C_Reserved_0_0_WIDTH                          1
#define D0F0x98_x2C_Reserved_0_0_MASK                           0x1
#define D0F0x98_x2C_DynWakeEn_OFFSET                            1
#define D0F0x98_x2C_DynWakeEn_WIDTH                             1
#define D0F0x98_x2C_DynWakeEn_MASK                              0x2
#define D0F0x98_x2C_Reserved_15_2_OFFSET                        2
#define D0F0x98_x2C_Reserved_15_2_WIDTH                         14
#define D0F0x98_x2C_Reserved_15_2_MASK                          0xfffc
#define D0F0x98_x2C_WakeHysteresis_OFFSET                       16
#define D0F0x98_x2C_WakeHysteresis_WIDTH                        16
#define D0F0x98_x2C_WakeHysteresis_MASK                         0xffff0000

/// D0F0x98_x2C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                                DynWakeEn:1 ; ///<
    UINT32                                            Reserved_15_2:14; ///<
    UINT32                                           WakeHysteresis:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x2C_STRUCT;

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

// **** D0F0xE4_WRAP_8002 Register Definition ****
// Address
#define D0F0xE4_WRAP_8002_ADDRESS                               0x8002

// Type
#define D0F0xE4_WRAP_8002_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8002_SubsystemVendorID_OFFSET              0
#define D0F0xE4_WRAP_8002_SubsystemVendorID_WIDTH               16
#define D0F0xE4_WRAP_8002_SubsystemVendorID_MASK                0xffff
#define D0F0xE4_WRAP_8002_SubsystemID_OFFSET                    16
#define D0F0xE4_WRAP_8002_SubsystemID_WIDTH                     16
#define D0F0xE4_WRAP_8002_SubsystemID_MASK                      0xffff0000

/// D0F0xE4_WRAP_8002
typedef union {
  struct {                                                              ///<
    UINT32                                        SubsystemVendorID:16; ///<
    UINT32                                              SubsystemID:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8002_STRUCT;

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
#define D0F0xE4_WRAP_8060_Reserved_31_3_OFFSET                  3
#define D0F0xE4_WRAP_8060_Reserved_31_3_WIDTH                   29
#define D0F0xE4_WRAP_8060_Reserved_31_3_MASK                    0xfffffff8

/// D0F0xE4_WRAP_8060
typedef union {
  struct {                                                              ///<
    UINT32                                              Reconfigure:1 ; ///<
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                            ResetComplete:1 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8060_STRUCT;

// **** D0F0xE4_WRAP_8061 Register Definition ****
// Address
#define D0F0xE4_WRAP_8061_ADDRESS                               0x8061

// Type
#define D0F0xE4_WRAP_8061_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8061_Reserved_14_0_OFFSET                  0
#define D0F0xE4_WRAP_8061_Reserved_14_0_WIDTH                   15
#define D0F0xE4_WRAP_8061_Reserved_14_0_MASK                    0x7fff
#define D0F0xE4_WRAP_8061_ResetCpm_OFFSET                       15
#define D0F0xE4_WRAP_8061_ResetCpm_WIDTH                        1
#define D0F0xE4_WRAP_8061_ResetCpm_MASK                         0x8000
#define D0F0xE4_WRAP_8061_ResetPif0_OFFSET                      16
#define D0F0xE4_WRAP_8061_ResetPif0_WIDTH                       1
#define D0F0xE4_WRAP_8061_ResetPif0_MASK                        0x10000
#define D0F0xE4_WRAP_8061_Reserved_23_17_OFFSET                 17
#define D0F0xE4_WRAP_8061_Reserved_23_17_WIDTH                  7
#define D0F0xE4_WRAP_8061_Reserved_23_17_MASK                   0xfe0000
#define D0F0xE4_WRAP_8061_ResetPhy0_OFFSET                      24
#define D0F0xE4_WRAP_8061_ResetPhy0_WIDTH                       1
#define D0F0xE4_WRAP_8061_ResetPhy0_MASK                        0x1000000
#define D0F0xE4_WRAP_8061_Reserved_31_25_OFFSET                 25
#define D0F0xE4_WRAP_8061_Reserved_31_25_WIDTH                  7
#define D0F0xE4_WRAP_8061_Reserved_31_25_MASK                   0xfe000000

/// D0F0xE4_WRAP_8061
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_14_0:15; ///<
    UINT32                                                 ResetCpm:1 ; ///<
    UINT32                                                ResetPif0:1 ; ///<
    UINT32                                           Reserved_23_17:7 ; ///<
    UINT32                                                ResetPhy0:1 ; ///<
    UINT32                                           Reserved_31_25:7 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8061_STRUCT;

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

// **** D0F0xE4_x0108_8071 Register Definition ****
// Address
#define D0F0xE4_x0108_8071_ADDRESS                              0x1088071

// Type
#define D0F0xE4_x0108_8071_TYPE                                 TYPE_D0F0xE4
// Field Data
#define D0F0xE4_x0108_8071_RxAdjust_OFFSET                      0
#define D0F0xE4_x0108_8071_RxAdjust_WIDTH                       3
#define D0F0xE4_x0108_8071_RxAdjust_MASK                        0x7
#define D0F0xE4_x0108_8071_Reserved_31_3_OFFSET                 3
#define D0F0xE4_x0108_8071_Reserved_31_3_WIDTH                  29
#define D0F0xE4_x0108_8071_Reserved_31_3_MASK                   0xfffffff8

/// D0F0xE4_x0108_8071
typedef union {
  struct {                                                              ///<
    UINT32                                                 RxAdjust:3 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_x0108_8071_STRUCT;

// **** D0F0xE4_x0108_8072 Register Definition ****
// Address
#define D0F0xE4_x0108_8072_ADDRESS                              0x1088072

// Type
#define D0F0xE4_x0108_8072_TYPE                                 TYPE_D0F0xE4
// Field Data
#define D0F0xE4_x0108_8072_TxAdjust_OFFSET                      0
#define D0F0xE4_x0108_8072_TxAdjust_WIDTH                       3
#define D0F0xE4_x0108_8072_TxAdjust_MASK                        0x7
#define D0F0xE4_x0108_8072_Reserved_31_3_OFFSET                 3
#define D0F0xE4_x0108_8072_Reserved_31_3_WIDTH                  29
#define D0F0xE4_x0108_8072_Reserved_31_3_MASK                   0xfffffff8

/// D0F0xE4_x0108_8072
typedef union {
  struct {                                                              ///<
    UINT32                                                 TxAdjust:3 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_x0108_8072_STRUCT;

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
#define D0F0xE4_PIF_0010_EiCycleOffTime_OFFSET                  20
#define D0F0xE4_PIF_0010_EiCycleOffTime_WIDTH                   3
#define D0F0xE4_PIF_0010_EiCycleOffTime_MASK                    0x700000
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
    UINT32                                           EiCycleOffTime:3 ; ///<
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

// **** D0F0xE4_CORE_0011 Register Definition ****
// Address
#define D0F0xE4_CORE_0011_ADDRESS                               0x11

// Type
#define D0F0xE4_CORE_0011_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_0011_DynClkLatency_OFFSET                  0
#define D0F0xE4_CORE_0011_DynClkLatency_WIDTH                   4
#define D0F0xE4_CORE_0011_DynClkLatency_MASK                    0xf
#define D0F0xE4_CORE_0011_Reserved_31_4_OFFSET                  4
#define D0F0xE4_CORE_0011_Reserved_31_4_WIDTH                   28
#define D0F0xE4_CORE_0011_Reserved_31_4_MASK                    0xfffffff0

/// D0F0xE4_CORE_0011
typedef union {
  struct {                                                              ///<
    UINT32                                            DynClkLatency:4 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0011_STRUCT;

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

// **** D0F0xE4_CORE_00C0 Register Definition ****
// Address
#define D0F0xE4_CORE_00C0_ADDRESS                               0xc0

// Type
#define D0F0xE4_CORE_00C0_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_00C0_Reserved_27_0_OFFSET                  0
#define D0F0xE4_CORE_00C0_Reserved_27_0_WIDTH                   28
#define D0F0xE4_CORE_00C0_Reserved_27_0_MASK                    0xfffffff
#define D0F0xE4_CORE_00C0_StrapReverseAll_OFFSET                28
#define D0F0xE4_CORE_00C0_StrapReverseAll_WIDTH                 1
#define D0F0xE4_CORE_00C0_StrapReverseAll_MASK                  0x10000000
#define D0F0xE4_CORE_00C0_StrapMstAdr64En_OFFSET                29
#define D0F0xE4_CORE_00C0_StrapMstAdr64En_WIDTH                 1
#define D0F0xE4_CORE_00C0_StrapMstAdr64En_MASK                  0x20000000
#define D0F0xE4_CORE_00C0_Reserved_31_30_OFFSET                 30
#define D0F0xE4_CORE_00C0_Reserved_31_30_WIDTH                  2
#define D0F0xE4_CORE_00C0_Reserved_31_30_MASK                   0xc0000000

/// D0F0xE4_CORE_00C0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_27_0:28; ///<
    UINT32                                          StrapReverseAll:1 ; ///<
    UINT32                                          StrapMstAdr64En:1 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_00C0_STRUCT;

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

// **** D0F0xE4_PHY_4004 Register Definition ****
// Address
#define D0F0xE4_PHY_4004_ADDRESS                                0x4004

// Type
#define D0F0xE4_PHY_4004_TYPE                                   TYPE_D0F0xE4
// Field Data
#define D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdEn_OFFSET            0
#define D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdEn_WIDTH             1
#define D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdEn_MASK              0x1
#define D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdVal_OFFSET           1
#define D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdVal_WIDTH            1
#define D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdVal_MASK             0x2
#define D0F0xE4_PHY_4004_Reserved_31_2_OFFSET                   2
#define D0F0xE4_PHY_4004_Reserved_31_2_WIDTH                    30
#define D0F0xE4_PHY_4004_Reserved_31_2_MASK                     0xfffffffc

/// D0F0xE4_PHY_4004
typedef union {
  struct {                                                              ///<
    UINT32                                     PllBiasGenPdnbOvrdEn:1 ; ///<
    UINT32                                    PllBiasGenPdnbOvrdVal:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_PHY_4004_STRUCT;

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

// **** DxF0xE4_xA4 Register Definition ****
// Address
#define DxF0xE4_xA4_ADDRESS                                     0xa4

// Type
#define DxF0xE4_xA4_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xA4_LcGen2EnStrap_OFFSET                        0
#define DxF0xE4_xA4_LcGen2EnStrap_WIDTH                         1
#define DxF0xE4_xA4_LcGen2EnStrap_MASK                          0x1
#define DxF0xE4_xA4_Reserved_3_1_OFFSET                         1
#define DxF0xE4_xA4_Reserved_3_1_WIDTH                          3
#define DxF0xE4_xA4_Reserved_3_1_MASK                           0xe
#define DxF0xE4_xA4_LcForceDisSwSpeedChange_OFFSET              4
#define DxF0xE4_xA4_LcForceDisSwSpeedChange_WIDTH               1
#define DxF0xE4_xA4_LcForceDisSwSpeedChange_MASK                0x10
#define DxF0xE4_xA4_Reserved_6_5_OFFSET                         5
#define DxF0xE4_xA4_Reserved_6_5_WIDTH                          2
#define DxF0xE4_xA4_Reserved_6_5_MASK                           0x60
#define DxF0xE4_xA4_LcInitiateLinkSpeedChange_OFFSET            7
#define DxF0xE4_xA4_LcInitiateLinkSpeedChange_WIDTH             1
#define DxF0xE4_xA4_LcInitiateLinkSpeedChange_MASK              0x80
#define DxF0xE4_xA4_LcSpeedChangeAttemptsAllowed_OFFSET         8
#define DxF0xE4_xA4_LcSpeedChangeAttemptsAllowed_WIDTH          2
#define DxF0xE4_xA4_LcSpeedChangeAttemptsAllowed_MASK           0x300
#define DxF0xE4_xA4_LcSpeedChangeAttemptFailed_OFFSET           10
#define DxF0xE4_xA4_LcSpeedChangeAttemptFailed_WIDTH            1
#define DxF0xE4_xA4_LcSpeedChangeAttemptFailed_MASK             0x400
#define DxF0xE4_xA4_Reserved_17_11_OFFSET                       11
#define DxF0xE4_xA4_Reserved_17_11_WIDTH                        7
#define DxF0xE4_xA4_Reserved_17_11_MASK                         0x3f800
#define DxF0xE4_xA4_LcGoToRecovery_OFFSET                       18
#define DxF0xE4_xA4_LcGoToRecovery_WIDTH                        1
#define DxF0xE4_xA4_LcGoToRecovery_MASK                         0x40000
#define DxF0xE4_xA4_Reserved_23_19_OFFSET                       19
#define DxF0xE4_xA4_Reserved_23_19_WIDTH                        5
#define DxF0xE4_xA4_Reserved_23_19_MASK                         0xf80000
#define DxF0xE4_xA4_LcOtherSideSupportsGen2_OFFSET              24
#define DxF0xE4_xA4_LcOtherSideSupportsGen2_WIDTH               1
#define DxF0xE4_xA4_LcOtherSideSupportsGen2_MASK                0x1000000
#define DxF0xE4_xA4_Reserved_28_25_OFFSET                       25
#define DxF0xE4_xA4_Reserved_28_25_WIDTH                        4
#define DxF0xE4_xA4_Reserved_28_25_MASK                         0x1e000000
#define DxF0xE4_xA4_LcMultUpstreamAutoSpdChngEn_OFFSET          29
#define DxF0xE4_xA4_LcMultUpstreamAutoSpdChngEn_WIDTH           1
#define DxF0xE4_xA4_LcMultUpstreamAutoSpdChngEn_MASK            0x20000000
#define DxF0xE4_xA4_Reserved_31_30_OFFSET                       30
#define DxF0xE4_xA4_Reserved_31_30_WIDTH                        2
#define DxF0xE4_xA4_Reserved_31_30_MASK                         0xc0000000

/// DxF0xE4_xA4
typedef union {
  struct {                                                              ///<
    UINT32                                            LcGen2EnStrap:1 ; ///<
    UINT32                                             Reserved_3_1:3 ; ///<
    UINT32                                  LcForceDisSwSpeedChange:1 ; ///<
    UINT32                                             Reserved_6_5:2 ; ///<
    UINT32                                LcInitiateLinkSpeedChange:1 ; ///<
    UINT32                             LcSpeedChangeAttemptsAllowed:2 ; ///<
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
} DxF0xE4_xA4_STRUCT;

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

// **** DxF0xE4_xC1 Register Definition ****
// Address
#define DxF0xE4_xC1_ADDRESS                                     0xc1

// Type
#define DxF0xE4_xC1_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xC1_Reserved_3_0_OFFSET                         0
#define DxF0xE4_xC1_Reserved_3_0_WIDTH                          4
#define DxF0xE4_xC1_Reserved_3_0_MASK                           0xf
#define DxF0xE4_xC1_StrapReverseLanes_OFFSET                    4
#define DxF0xE4_xC1_StrapReverseLanes_WIDTH                     1
#define DxF0xE4_xC1_StrapReverseLanes_MASK                      0x10
#define DxF0xE4_xC1_Reserved_31_5_OFFSET                        5
#define DxF0xE4_xC1_Reserved_31_5_WIDTH                         27
#define DxF0xE4_xC1_Reserved_31_5_MASK                          0xffffffe0

/// DxF0xE4_xC1
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                        StrapReverseLanes:1 ; ///<
    UINT32                                            Reserved_31_5:27; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} DxF0xE4_xC1_STRUCT;

// **** SMUx0B_x8600 Register Definition ****
// Address
#define SMUx0B_x8600_ADDRESS                                    0x8600

// Type
#define SMUx0B_x8600_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8600_Txn1MBusAddr_7_0__OFFSET                   0
#define SMUx0B_x8600_Txn1MBusAddr_7_0__WIDTH                    8
#define SMUx0B_x8600_Txn1MBusAddr_7_0__MASK                     0xff
#define SMUx0B_x8600_MemAddr_7_0__OFFSET                        8
#define SMUx0B_x8600_MemAddr_7_0__WIDTH                         8
#define SMUx0B_x8600_MemAddr_7_0__MASK                          0xff00
#define SMUx0B_x8600_MemAddr_15_8__OFFSET                       16
#define SMUx0B_x8600_MemAddr_15_8__WIDTH                        8
#define SMUx0B_x8600_MemAddr_15_8__MASK                         0xff0000
#define SMUx0B_x8600_TransactionCount_OFFSET                    24
#define SMUx0B_x8600_TransactionCount_WIDTH                     8
#define SMUx0B_x8600_TransactionCount_MASK                      0xff000000

/// SMUx0B_x8600
typedef union {
  struct {                                                              ///<
    UINT32                                        Txn1MBusAddr_7_0_:8 ; ///<
    UINT32                                             MemAddr_7_0_:8 ; ///<
    UINT32                                            MemAddr_15_8_:8 ; ///<
    UINT32                                         TransactionCount:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8600_STRUCT;

// **** SMUx0B_x8604 Register Definition ****
// Address
#define SMUx0B_x8604_ADDRESS                                    0x8604

// Type
#define SMUx0B_x8604_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8604_Txn1TransferLength_7_0__OFFSET             0
#define SMUx0B_x8604_Txn1TransferLength_7_0__WIDTH              8
#define SMUx0B_x8604_Txn1TransferLength_7_0__MASK               0xff
#define SMUx0B_x8604_Txn1MBusAddr_31_24__OFFSET                 8
#define SMUx0B_x8604_Txn1MBusAddr_31_24__WIDTH                  8
#define SMUx0B_x8604_Txn1MBusAddr_31_24__MASK                   0xff00
#define SMUx0B_x8604_Txn1MBusAddr_23_16__OFFSET                 16
#define SMUx0B_x8604_Txn1MBusAddr_23_16__WIDTH                  8
#define SMUx0B_x8604_Txn1MBusAddr_23_16__MASK                   0xff0000
#define SMUx0B_x8604_Txn1MBusAddr_15_8__OFFSET                  24
#define SMUx0B_x8604_Txn1MBusAddr_15_8__WIDTH                   8
#define SMUx0B_x8604_Txn1MBusAddr_15_8__MASK                    0xff000000

/// SMUx0B_x8604
typedef union {
  struct {                                                              ///<
    UINT32                                  Txn1TransferLength_7_0_:8 ; ///<
    UINT32                                      Txn1MBusAddr_31_24_:8 ; ///<
    UINT32                                      Txn1MBusAddr_23_16_:8 ; ///<
    UINT32                                       Txn1MBusAddr_15_8_:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8604_STRUCT;

// **** SMUx0B_x8608 Register Definition ****
// Address
#define SMUx0B_x8608_ADDRESS                                    0x8608

// Type
#define SMUx0B_x8608_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8608_Txn2Mbusaddr158_OFFSET                     0
#define SMUx0B_x8608_Txn2Mbusaddr158_WIDTH                      8
#define SMUx0B_x8608_Txn2Mbusaddr158_MASK                       0xff
#define SMUx0B_x8608_Txn2Mbusaddr70_OFFSET                      8
#define SMUx0B_x8608_Txn2Mbusaddr70_WIDTH                       8
#define SMUx0B_x8608_Txn2Mbusaddr70_MASK                        0xff00
#define SMUx0B_x8608_Txn1Mode_OFFSET                            16
#define SMUx0B_x8608_Txn1Mode_WIDTH                             2
#define SMUx0B_x8608_Txn1Mode_MASK                              0x30000
#define SMUx0B_x8608_Txn1Static_OFFSET                          18
#define SMUx0B_x8608_Txn1Static_WIDTH                           1
#define SMUx0B_x8608_Txn1Static_MASK                            0x40000
#define SMUx0B_x8608_Txn1Overlap_OFFSET                         19
#define SMUx0B_x8608_Txn1Overlap_WIDTH                          1
#define SMUx0B_x8608_Txn1Overlap_MASK                           0x80000
#define SMUx0B_x8608_Txn1Spare_OFFSET                           20
#define SMUx0B_x8608_Txn1Spare_WIDTH                            4
#define SMUx0B_x8608_Txn1Spare_MASK                             0xf00000
#define SMUx0B_x8608_Txn1TransferLength_13_8__OFFSET            24
#define SMUx0B_x8608_Txn1TransferLength_13_8__WIDTH             6
#define SMUx0B_x8608_Txn1TransferLength_13_8__MASK              0x3f000000
#define SMUx0B_x8608_Txn1Tsize_OFFSET                           30
#define SMUx0B_x8608_Txn1Tsize_WIDTH                            2
#define SMUx0B_x8608_Txn1Tsize_MASK                             0xc0000000

/// SMUx0B_x8608
typedef union {
  struct {                                                              ///<
    UINT32                                          Txn2Mbusaddr158:8 ; ///<
    UINT32                                           Txn2Mbusaddr70:8 ; ///<
    UINT32                                                 Txn1Mode:2 ; ///<
    UINT32                                               Txn1Static:1 ; ///<
    UINT32                                              Txn1Overlap:1 ; ///<
    UINT32                                                Txn1Spare:4 ; ///<
    UINT32                                 Txn1TransferLength_13_8_:6 ; ///<
    UINT32                                                Txn1Tsize:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8608_STRUCT;

// **** SMUx0B_x860C Register Definition ****
// Address
#define SMUx0B_x860C_ADDRESS                                    0x860c

// Type
#define SMUx0B_x860C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x860C_Txn2TransferLength138_OFFSET               0
#define SMUx0B_x860C_Txn2TransferLength138_WIDTH                6
#define SMUx0B_x860C_Txn2TransferLength138_MASK                 0x3f
#define SMUx0B_x860C_Txn2Tsize_OFFSET                           6
#define SMUx0B_x860C_Txn2Tsize_WIDTH                            2
#define SMUx0B_x860C_Txn2Tsize_MASK                             0xc0
#define SMUx0B_x860C_Txn2TransferLength70_OFFSET                8
#define SMUx0B_x860C_Txn2TransferLength70_WIDTH                 8
#define SMUx0B_x860C_Txn2TransferLength70_MASK                  0xff00
#define SMUx0B_x860C_Txn2MBusAddr3124_OFFSET                    16
#define SMUx0B_x860C_Txn2MBusAddr3124_WIDTH                     8
#define SMUx0B_x860C_Txn2MBusAddr3124_MASK                      0xff0000
#define SMUx0B_x860C_Txn2MBusAddr2316_OFFSET                    24
#define SMUx0B_x860C_Txn2MBusAddr2316_WIDTH                     8
#define SMUx0B_x860C_Txn2MBusAddr2316_MASK                      0xff000000

/// SMUx0B_x860C
typedef union {
  struct {                                                              ///<
    UINT32                                    Txn2TransferLength138:6 ; ///<
    UINT32                                                Txn2Tsize:2 ; ///<
    UINT32                                     Txn2TransferLength70:8 ; ///<
    UINT32                                         Txn2MBusAddr3124:8 ; ///<
    UINT32                                         Txn2MBusAddr2316:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x860C_STRUCT;

// **** SMUx0B_x8610 Register Definition ****
// Address
#define SMUx0B_x8610_ADDRESS                                    0x8610

// Type
#define SMUx0B_x8610_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8610_Txn3MBusAddr2316_OFFSET                    0
#define SMUx0B_x8610_Txn3MBusAddr2316_WIDTH                     8
#define SMUx0B_x8610_Txn3MBusAddr2316_MASK                      0xff
#define SMUx0B_x8610_Txn3MBusAddr158_OFFSET                     8
#define SMUx0B_x8610_Txn3MBusAddr158_WIDTH                      8
#define SMUx0B_x8610_Txn3MBusAddr158_MASK                       0xff00
#define SMUx0B_x8610_Txn3MBusAddr70_OFFSET                      16
#define SMUx0B_x8610_Txn3MBusAddr70_WIDTH                       8
#define SMUx0B_x8610_Txn3MBusAddr70_MASK                        0xff0000
#define SMUx0B_x8610_Txn2Mode_OFFSET                            24
#define SMUx0B_x8610_Txn2Mode_WIDTH                             2
#define SMUx0B_x8610_Txn2Mode_MASK                              0x3000000
#define SMUx0B_x8610_Txn2Static_OFFSET                          26
#define SMUx0B_x8610_Txn2Static_WIDTH                           1
#define SMUx0B_x8610_Txn2Static_MASK                            0x4000000
#define SMUx0B_x8610_Txn2Overlap_OFFSET                         27
#define SMUx0B_x8610_Txn2Overlap_WIDTH                          1
#define SMUx0B_x8610_Txn2Overlap_MASK                           0x8000000
#define SMUx0B_x8610_Txn2Spare_OFFSET                           28
#define SMUx0B_x8610_Txn2Spare_WIDTH                            4
#define SMUx0B_x8610_Txn2Spare_MASK                             0xf0000000

/// SMUx0B_x8610
typedef union {
  struct {                                                              ///<
    UINT32                                         Txn3MBusAddr2316:8 ; ///<
    UINT32                                          Txn3MBusAddr158:8 ; ///<
    UINT32                                           Txn3MBusAddr70:8 ; ///<
    UINT32                                                 Txn2Mode:2 ; ///<
    UINT32                                               Txn2Static:1 ; ///<
    UINT32                                              Txn2Overlap:1 ; ///<
    UINT32                                                Txn2Spare:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8610_STRUCT;

// **** SMUx0B_x8614 Register Definition ****
// Address
#define SMUx0B_x8614_ADDRESS                                    0x8614

// Type
#define SMUx0B_x8614_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8614_Txn3Mode_OFFSET                            0
#define SMUx0B_x8614_Txn3Mode_WIDTH                             2
#define SMUx0B_x8614_Txn3Mode_MASK                              0x3
#define SMUx0B_x8614_Txn3Static_OFFSET                          2
#define SMUx0B_x8614_Txn3Static_WIDTH                           1
#define SMUx0B_x8614_Txn3Static_MASK                            0x4
#define SMUx0B_x8614_Txn3Overlap_OFFSET                         3
#define SMUx0B_x8614_Txn3Overlap_WIDTH                          1
#define SMUx0B_x8614_Txn3Overlap_MASK                           0x8
#define SMUx0B_x8614_Txn3Spare_OFFSET                           4
#define SMUx0B_x8614_Txn3Spare_WIDTH                            4
#define SMUx0B_x8614_Txn3Spare_MASK                             0xf0
#define SMUx0B_x8614_Txn3TransferLength138_OFFSET               8
#define SMUx0B_x8614_Txn3TransferLength138_WIDTH                6
#define SMUx0B_x8614_Txn3TransferLength138_MASK                 0x3f00
#define SMUx0B_x8614_Txn3Tsize_OFFSET                           14
#define SMUx0B_x8614_Txn3Tsize_WIDTH                            2
#define SMUx0B_x8614_Txn3Tsize_MASK                             0xc000
#define SMUx0B_x8614_Txn3TransferLength70_OFFSET                16
#define SMUx0B_x8614_Txn3TransferLength70_WIDTH                 8
#define SMUx0B_x8614_Txn3TransferLength70_MASK                  0xff0000
#define SMUx0B_x8614_Txn3MBusAddr3124_OFFSET                    24
#define SMUx0B_x8614_Txn3MBusAddr3124_WIDTH                     8
#define SMUx0B_x8614_Txn3MBusAddr3124_MASK                      0xff000000

/// SMUx0B_x8614
typedef union {
  struct {                                                              ///<
    UINT32                                                 Txn3Mode:2 ; ///<
    UINT32                                               Txn3Static:1 ; ///<
    UINT32                                              Txn3Overlap:1 ; ///<
    UINT32                                                Txn3Spare:4 ; ///<
    UINT32                                    Txn3TransferLength138:6 ; ///<
    UINT32                                                Txn3Tsize:2 ; ///<
    UINT32                                     Txn3TransferLength70:8 ; ///<
    UINT32                                         Txn3MBusAddr3124:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8614_STRUCT;

// **** SMUx0B_x8618 Register Definition ****
// Address
#define SMUx0B_x8618_ADDRESS                                    0x8618

// Type
#define SMUx0B_x8618_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8618_Txn4MBusAddr3124_OFFSET                    0
#define SMUx0B_x8618_Txn4MBusAddr3124_WIDTH                     8
#define SMUx0B_x8618_Txn4MBusAddr3124_MASK                      0xff
#define SMUx0B_x8618_Txn4MBusAddr2316_OFFSET                    8
#define SMUx0B_x8618_Txn4MBusAddr2316_WIDTH                     8
#define SMUx0B_x8618_Txn4MBusAddr2316_MASK                      0xff00
#define SMUx0B_x8618_Txn4MBusAddr158_OFFSET                     16
#define SMUx0B_x8618_Txn4MBusAddr158_WIDTH                      8
#define SMUx0B_x8618_Txn4MBusAddr158_MASK                       0xff0000
#define SMUx0B_x8618_Txn4MBusAddr70_OFFSET                      24
#define SMUx0B_x8618_Txn4MBusAddr70_WIDTH                       8
#define SMUx0B_x8618_Txn4MBusAddr70_MASK                        0xff000000

/// SMUx0B_x8618
typedef union {
  struct {                                                              ///<
    UINT32                                         Txn4MBusAddr3124:8 ; ///<
    UINT32                                         Txn4MBusAddr2316:8 ; ///<
    UINT32                                          Txn4MBusAddr158:8 ; ///<
    UINT32                                           Txn4MBusAddr70:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8618_STRUCT;

// **** SMUx0B_x861C Register Definition ****
// Address
#define SMUx0B_x861C_ADDRESS                                    0x861c

// Type
#define SMUx0B_x861C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x861C_Txn5Mbusaddr70_OFFSET                      0
#define SMUx0B_x861C_Txn5Mbusaddr70_WIDTH                       8
#define SMUx0B_x861C_Txn5Mbusaddr70_MASK                        0xff
#define SMUx0B_x861C_Txn4Mode_OFFSET                            8
#define SMUx0B_x861C_Txn4Mode_WIDTH                             2
#define SMUx0B_x861C_Txn4Mode_MASK                              0x300
#define SMUx0B_x861C_Txn4Static_OFFSET                          10
#define SMUx0B_x861C_Txn4Static_WIDTH                           1
#define SMUx0B_x861C_Txn4Static_MASK                            0x400
#define SMUx0B_x861C_Txn4Overlap_OFFSET                         11
#define SMUx0B_x861C_Txn4Overlap_WIDTH                          1
#define SMUx0B_x861C_Txn4Overlap_MASK                           0x800
#define SMUx0B_x861C_Txn4Spare_OFFSET                           12
#define SMUx0B_x861C_Txn4Spare_WIDTH                            4
#define SMUx0B_x861C_Txn4Spare_MASK                             0xf000
#define SMUx0B_x861C_Txn4TransferLength138_OFFSET               16
#define SMUx0B_x861C_Txn4TransferLength138_WIDTH                6
#define SMUx0B_x861C_Txn4TransferLength138_MASK                 0x3f0000
#define SMUx0B_x861C_Txn4Tsize_OFFSET                           22
#define SMUx0B_x861C_Txn4Tsize_WIDTH                            2
#define SMUx0B_x861C_Txn4Tsize_MASK                             0xc00000
#define SMUx0B_x861C_Txn4TransferLength70_OFFSET                24
#define SMUx0B_x861C_Txn4TransferLength70_WIDTH                 8
#define SMUx0B_x861C_Txn4TransferLength70_MASK                  0xff000000

/// SMUx0B_x861C
typedef union {
  struct {                                                              ///<
    UINT32                                           Txn5Mbusaddr70:8 ; ///<
    UINT32                                                 Txn4Mode:2 ; ///<
    UINT32                                               Txn4Static:1 ; ///<
    UINT32                                              Txn4Overlap:1 ; ///<
    UINT32                                                Txn4Spare:4 ; ///<
    UINT32                                    Txn4TransferLength138:6 ; ///<
    UINT32                                                Txn4Tsize:2 ; ///<
    UINT32                                     Txn4TransferLength70:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x861C_STRUCT;

// **** SMUx0B_x8620 Register Definition ****
// Address
#define SMUx0B_x8620_ADDRESS                                    0x8620

// Type
#define SMUx0B_x8620_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8620_Txn5TransferLength70_OFFSET                0
#define SMUx0B_x8620_Txn5TransferLength70_WIDTH                 8
#define SMUx0B_x8620_Txn5TransferLength70_MASK                  0xff
#define SMUx0B_x8620_Txn5MBusAddr3124_OFFSET                    8
#define SMUx0B_x8620_Txn5MBusAddr3124_WIDTH                     8
#define SMUx0B_x8620_Txn5MBusAddr3124_MASK                      0xff00
#define SMUx0B_x8620_Txn5MBusAddr2316_OFFSET                    16
#define SMUx0B_x8620_Txn5MBusAddr2316_WIDTH                     8
#define SMUx0B_x8620_Txn5MBusAddr2316_MASK                      0xff0000
#define SMUx0B_x8620_Txn5MBusAddr158_OFFSET                     24
#define SMUx0B_x8620_Txn5MBusAddr158_WIDTH                      8
#define SMUx0B_x8620_Txn5MBusAddr158_MASK                       0xff000000

/// SMUx0B_x8620
typedef union {
  struct {                                                              ///<
    UINT32                                     Txn5TransferLength70:8 ; ///<
    UINT32                                         Txn5MBusAddr3124:8 ; ///<
    UINT32                                         Txn5MBusAddr2316:8 ; ///<
    UINT32                                          Txn5MBusAddr158:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8620_STRUCT;

// **** SMUx0B_x8624 Register Definition ****
// Address
#define SMUx0B_x8624_ADDRESS                                    0x8624

// Type
#define SMUx0B_x8624_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8624_Txn6MBusAddr158_OFFSET                     0
#define SMUx0B_x8624_Txn6MBusAddr158_WIDTH                      8
#define SMUx0B_x8624_Txn6MBusAddr158_MASK                       0xff
#define SMUx0B_x8624_Txn6MBusAddr70_OFFSET                      8
#define SMUx0B_x8624_Txn6MBusAddr70_WIDTH                       8
#define SMUx0B_x8624_Txn6MBusAddr70_MASK                        0xff00
#define SMUx0B_x8624_Txn5Mode_OFFSET                            16
#define SMUx0B_x8624_Txn5Mode_WIDTH                             2
#define SMUx0B_x8624_Txn5Mode_MASK                              0x30000
#define SMUx0B_x8624_Txn5Static_OFFSET                          18
#define SMUx0B_x8624_Txn5Static_WIDTH                           1
#define SMUx0B_x8624_Txn5Static_MASK                            0x40000
#define SMUx0B_x8624_Txn5Overlap_OFFSET                         19
#define SMUx0B_x8624_Txn5Overlap_WIDTH                          1
#define SMUx0B_x8624_Txn5Overlap_MASK                           0x80000
#define SMUx0B_x8624_Txn5Spare_OFFSET                           20
#define SMUx0B_x8624_Txn5Spare_WIDTH                            4
#define SMUx0B_x8624_Txn5Spare_MASK                             0xf00000
#define SMUx0B_x8624_Txn5TransferLength138_OFFSET               24
#define SMUx0B_x8624_Txn5TransferLength138_WIDTH                6
#define SMUx0B_x8624_Txn5TransferLength138_MASK                 0x3f000000
#define SMUx0B_x8624_Txn5Tsize_OFFSET                           30
#define SMUx0B_x8624_Txn5Tsize_WIDTH                            2
#define SMUx0B_x8624_Txn5Tsize_MASK                             0xc0000000

/// SMUx0B_x8624
typedef union {
  struct {                                                              ///<
    UINT32                                          Txn6MBusAddr158:8 ; ///<
    UINT32                                           Txn6MBusAddr70:8 ; ///<
    UINT32                                                 Txn5Mode:2 ; ///<
    UINT32                                               Txn5Static:1 ; ///<
    UINT32                                              Txn5Overlap:1 ; ///<
    UINT32                                                Txn5Spare:4 ; ///<
    UINT32                                    Txn5TransferLength138:6 ; ///<
    UINT32                                                Txn5Tsize:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8624_STRUCT;

// **** SMUx0B_x8628 Register Definition ****
// Address
#define SMUx0B_x8628_ADDRESS                                    0x8628

// Type
#define SMUx0B_x8628_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8628_Txn6TransferLength138_OFFSET               0
#define SMUx0B_x8628_Txn6TransferLength138_WIDTH                6
#define SMUx0B_x8628_Txn6TransferLength138_MASK                 0x3f
#define SMUx0B_x8628_Txn6Tsize_OFFSET                           6
#define SMUx0B_x8628_Txn6Tsize_WIDTH                            2
#define SMUx0B_x8628_Txn6Tsize_MASK                             0xc0
#define SMUx0B_x8628_Txn6TransferLength70_OFFSET                8
#define SMUx0B_x8628_Txn6TransferLength70_WIDTH                 8
#define SMUx0B_x8628_Txn6TransferLength70_MASK                  0xff00
#define SMUx0B_x8628_Txn6MBusAddr3124_OFFSET                    16
#define SMUx0B_x8628_Txn6MBusAddr3124_WIDTH                     8
#define SMUx0B_x8628_Txn6MBusAddr3124_MASK                      0xff0000
#define SMUx0B_x8628_Txn6MBusAddr2316_OFFSET                    24
#define SMUx0B_x8628_Txn6MBusAddr2316_WIDTH                     8
#define SMUx0B_x8628_Txn6MBusAddr2316_MASK                      0xff000000

/// SMUx0B_x8628
typedef union {
  struct {                                                              ///<
    UINT32                                    Txn6TransferLength138:6 ; ///<
    UINT32                                                Txn6Tsize:2 ; ///<
    UINT32                                     Txn6TransferLength70:8 ; ///<
    UINT32                                         Txn6MBusAddr3124:8 ; ///<
    UINT32                                         Txn6MBusAddr2316:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8628_STRUCT;

// **** SMUx0B_x862C Register Definition ****
// Address
#define SMUx0B_x862C_ADDRESS                                    0x862c

// Type
#define SMUx0B_x862C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x862C_Txn7MBusAddr2316_OFFSET                    0
#define SMUx0B_x862C_Txn7MBusAddr2316_WIDTH                     8
#define SMUx0B_x862C_Txn7MBusAddr2316_MASK                      0xff
#define SMUx0B_x862C_Txn7MBusAddr158_OFFSET                     8
#define SMUx0B_x862C_Txn7MBusAddr158_WIDTH                      8
#define SMUx0B_x862C_Txn7MBusAddr158_MASK                       0xff00
#define SMUx0B_x862C_Txn7MBusAddr70_OFFSET                      16
#define SMUx0B_x862C_Txn7MBusAddr70_WIDTH                       8
#define SMUx0B_x862C_Txn7MBusAddr70_MASK                        0xff0000
#define SMUx0B_x862C_Txn6Mode_OFFSET                            24
#define SMUx0B_x862C_Txn6Mode_WIDTH                             2
#define SMUx0B_x862C_Txn6Mode_MASK                              0x3000000
#define SMUx0B_x862C_Txn6Static_OFFSET                          26
#define SMUx0B_x862C_Txn6Static_WIDTH                           1
#define SMUx0B_x862C_Txn6Static_MASK                            0x4000000
#define SMUx0B_x862C_Txn6Overlap_OFFSET                         27
#define SMUx0B_x862C_Txn6Overlap_WIDTH                          1
#define SMUx0B_x862C_Txn6Overlap_MASK                           0x8000000
#define SMUx0B_x862C_Txn6Spare_OFFSET                           28
#define SMUx0B_x862C_Txn6Spare_WIDTH                            4
#define SMUx0B_x862C_Txn6Spare_MASK                             0xf0000000

/// SMUx0B_x862C
typedef union {
  struct {                                                              ///<
    UINT32                                         Txn7MBusAddr2316:8 ; ///<
    UINT32                                          Txn7MBusAddr158:8 ; ///<
    UINT32                                           Txn7MBusAddr70:8 ; ///<
    UINT32                                                 Txn6Mode:2 ; ///<
    UINT32                                               Txn6Static:1 ; ///<
    UINT32                                              Txn6Overlap:1 ; ///<
    UINT32                                                Txn6Spare:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x862C_STRUCT;

// **** SMUx0B_x8630 Register Definition ****
// Address
#define SMUx0B_x8630_ADDRESS                                    0x8630

// Type
#define SMUx0B_x8630_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8630_Txn7Mode_OFFSET                            0
#define SMUx0B_x8630_Txn7Mode_WIDTH                             2
#define SMUx0B_x8630_Txn7Mode_MASK                              0x3
#define SMUx0B_x8630_Txn7Static_OFFSET                          2
#define SMUx0B_x8630_Txn7Static_WIDTH                           1
#define SMUx0B_x8630_Txn7Static_MASK                            0x4
#define SMUx0B_x8630_Txn7Overlap_OFFSET                         3
#define SMUx0B_x8630_Txn7Overlap_WIDTH                          1
#define SMUx0B_x8630_Txn7Overlap_MASK                           0x8
#define SMUx0B_x8630_Txn7Spare_OFFSET                           4
#define SMUx0B_x8630_Txn7Spare_WIDTH                            4
#define SMUx0B_x8630_Txn7Spare_MASK                             0xf0
#define SMUx0B_x8630_Txn7TransferLength138_OFFSET               8
#define SMUx0B_x8630_Txn7TransferLength138_WIDTH                6
#define SMUx0B_x8630_Txn7TransferLength138_MASK                 0x3f00
#define SMUx0B_x8630_Txn7Tsize_OFFSET                           14
#define SMUx0B_x8630_Txn7Tsize_WIDTH                            2
#define SMUx0B_x8630_Txn7Tsize_MASK                             0xc000
#define SMUx0B_x8630_Txn7TransferLength70_OFFSET                16
#define SMUx0B_x8630_Txn7TransferLength70_WIDTH                 8
#define SMUx0B_x8630_Txn7TransferLength70_MASK                  0xff0000
#define SMUx0B_x8630_Txn7MBusAddr3124_OFFSET                    24
#define SMUx0B_x8630_Txn7MBusAddr3124_WIDTH                     8
#define SMUx0B_x8630_Txn7MBusAddr3124_MASK                      0xff000000

/// SMUx0B_x8630
typedef union {
  struct {                                                              ///<
    UINT32                                                 Txn7Mode:2 ; ///<
    UINT32                                               Txn7Static:1 ; ///<
    UINT32                                              Txn7Overlap:1 ; ///<
    UINT32                                                Txn7Spare:4 ; ///<
    UINT32                                    Txn7TransferLength138:6 ; ///<
    UINT32                                                Txn7Tsize:2 ; ///<
    UINT32                                     Txn7TransferLength70:8 ; ///<
    UINT32                                         Txn7MBusAddr3124:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8630_STRUCT;

// **** SMUx0B_x8634 Register Definition ****
// Address
#define SMUx0B_x8634_ADDRESS                                    0x8634

// Type
#define SMUx0B_x8634_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8634_Txn8MBusAddr3124_OFFSET                    0
#define SMUx0B_x8634_Txn8MBusAddr3124_WIDTH                     8
#define SMUx0B_x8634_Txn8MBusAddr3124_MASK                      0xff
#define SMUx0B_x8634_Txn8MBusAddr2316_OFFSET                    8
#define SMUx0B_x8634_Txn8MBusAddr2316_WIDTH                     8
#define SMUx0B_x8634_Txn8MBusAddr2316_MASK                      0xff00
#define SMUx0B_x8634_Txn8MBusAddr158_OFFSET                     16
#define SMUx0B_x8634_Txn8MBusAddr158_WIDTH                      8
#define SMUx0B_x8634_Txn8MBusAddr158_MASK                       0xff0000
#define SMUx0B_x8634_Txn8MBusAddr70_OFFSET                      24
#define SMUx0B_x8634_Txn8MBusAddr70_WIDTH                       8
#define SMUx0B_x8634_Txn8MBusAddr70_MASK                        0xff000000

/// SMUx0B_x8634
typedef union {
  struct {                                                              ///<
    UINT32                                         Txn8MBusAddr3124:8 ; ///<
    UINT32                                         Txn8MBusAddr2316:8 ; ///<
    UINT32                                          Txn8MBusAddr158:8 ; ///<
    UINT32                                           Txn8MBusAddr70:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8634_STRUCT;

// **** SMUx0B_x8638 Register Definition ****
// Address
#define SMUx0B_x8638_ADDRESS                                    0x8638

// Type
#define SMUx0B_x8638_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8638_Txn9MBusAddr70_OFFSET                      0
#define SMUx0B_x8638_Txn9MBusAddr70_WIDTH                       8
#define SMUx0B_x8638_Txn9MBusAddr70_MASK                        0xff
#define SMUx0B_x8638_Txn8Mode_OFFSET                            8
#define SMUx0B_x8638_Txn8Mode_WIDTH                             2
#define SMUx0B_x8638_Txn8Mode_MASK                              0x300
#define SMUx0B_x8638_Txn8Static_OFFSET                          10
#define SMUx0B_x8638_Txn8Static_WIDTH                           1
#define SMUx0B_x8638_Txn8Static_MASK                            0x400
#define SMUx0B_x8638_Txn8Overlap_OFFSET                         11
#define SMUx0B_x8638_Txn8Overlap_WIDTH                          1
#define SMUx0B_x8638_Txn8Overlap_MASK                           0x800
#define SMUx0B_x8638_Txn8Spare_OFFSET                           12
#define SMUx0B_x8638_Txn8Spare_WIDTH                            4
#define SMUx0B_x8638_Txn8Spare_MASK                             0xf000
#define SMUx0B_x8638_Txn8TransferLength138_OFFSET               16
#define SMUx0B_x8638_Txn8TransferLength138_WIDTH                6
#define SMUx0B_x8638_Txn8TransferLength138_MASK                 0x3f0000
#define SMUx0B_x8638_Txn8Tsize_OFFSET                           22
#define SMUx0B_x8638_Txn8Tsize_WIDTH                            2
#define SMUx0B_x8638_Txn8Tsize_MASK                             0xc00000
#define SMUx0B_x8638_Txn8TransferLength70_OFFSET                24
#define SMUx0B_x8638_Txn8TransferLength70_WIDTH                 8
#define SMUx0B_x8638_Txn8TransferLength70_MASK                  0xff000000

/// SMUx0B_x8638
typedef union {
  struct {                                                              ///<
    UINT32                                           Txn9MBusAddr70:8 ; ///<
    UINT32                                                 Txn8Mode:2 ; ///<
    UINT32                                               Txn8Static:1 ; ///<
    UINT32                                              Txn8Overlap:1 ; ///<
    UINT32                                                Txn8Spare:4 ; ///<
    UINT32                                    Txn8TransferLength138:6 ; ///<
    UINT32                                                Txn8Tsize:2 ; ///<
    UINT32                                     Txn8TransferLength70:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8638_STRUCT;

// **** SMUx0B_x863C Register Definition ****
// Address
#define SMUx0B_x863C_ADDRESS                                    0x863c

// Type
#define SMUx0B_x863C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x863C_Txn9TransferLength70_OFFSET                0
#define SMUx0B_x863C_Txn9TransferLength70_WIDTH                 8
#define SMUx0B_x863C_Txn9TransferLength70_MASK                  0xff
#define SMUx0B_x863C_Txn9MBusAddr3124_OFFSET                    8
#define SMUx0B_x863C_Txn9MBusAddr3124_WIDTH                     8
#define SMUx0B_x863C_Txn9MBusAddr3124_MASK                      0xff00
#define SMUx0B_x863C_Txn9MBuAaddr2316_OFFSET                    16
#define SMUx0B_x863C_Txn9MBuAaddr2316_WIDTH                     8
#define SMUx0B_x863C_Txn9MBuAaddr2316_MASK                      0xff0000
#define SMUx0B_x863C_Txn9MBusAddr158_OFFSET                     24
#define SMUx0B_x863C_Txn9MBusAddr158_WIDTH                      8
#define SMUx0B_x863C_Txn9MBusAddr158_MASK                       0xff000000

/// SMUx0B_x863C
typedef union {
  struct {                                                              ///<
    UINT32                                     Txn9TransferLength70:8 ; ///<
    UINT32                                         Txn9MBusAddr3124:8 ; ///<
    UINT32                                         Txn9MBuAaddr2316:8 ; ///<
    UINT32                                          Txn9MBusAddr158:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x863C_STRUCT;

// **** SMUx0B_x8640 Register Definition ****
// Address
#define SMUx0B_x8640_ADDRESS                                    0x8640

// Type
#define SMUx0B_x8640_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8640_Txn10MBusAddr158_OFFSET                    0
#define SMUx0B_x8640_Txn10MBusAddr158_WIDTH                     8
#define SMUx0B_x8640_Txn10MBusAddr158_MASK                      0xff
#define SMUx0B_x8640_Txn10MBusAddr70_OFFSET                     8
#define SMUx0B_x8640_Txn10MBusAddr70_WIDTH                      8
#define SMUx0B_x8640_Txn10MBusAddr70_MASK                       0xff00
#define SMUx0B_x8640_Txn9Mode_OFFSET                            16
#define SMUx0B_x8640_Txn9Mode_WIDTH                             2
#define SMUx0B_x8640_Txn9Mode_MASK                              0x30000
#define SMUx0B_x8640_Txn9Static_OFFSET                          18
#define SMUx0B_x8640_Txn9Static_WIDTH                           1
#define SMUx0B_x8640_Txn9Static_MASK                            0x40000
#define SMUx0B_x8640_Txn9Overlap_OFFSET                         19
#define SMUx0B_x8640_Txn9Overlap_WIDTH                          1
#define SMUx0B_x8640_Txn9Overlap_MASK                           0x80000
#define SMUx0B_x8640_Txn9Spare_OFFSET                           20
#define SMUx0B_x8640_Txn9Spare_WIDTH                            4
#define SMUx0B_x8640_Txn9Spare_MASK                             0xf00000
#define SMUx0B_x8640_Txn9TransferLength138_OFFSET               24
#define SMUx0B_x8640_Txn9TransferLength138_WIDTH                6
#define SMUx0B_x8640_Txn9TransferLength138_MASK                 0x3f000000
#define SMUx0B_x8640_Txn9Tsize_OFFSET                           30
#define SMUx0B_x8640_Txn9Tsize_WIDTH                            2
#define SMUx0B_x8640_Txn9Tsize_MASK                             0xc0000000

/// SMUx0B_x8640
typedef union {
  struct {                                                              ///<
    UINT32                                         Txn10MBusAddr158:8 ; ///<
    UINT32                                          Txn10MBusAddr70:8 ; ///<
    UINT32                                                 Txn9Mode:2 ; ///<
    UINT32                                               Txn9Static:1 ; ///<
    UINT32                                              Txn9Overlap:1 ; ///<
    UINT32                                                Txn9Spare:4 ; ///<
    UINT32                                    Txn9TransferLength138:6 ; ///<
    UINT32                                                Txn9Tsize:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8640_STRUCT;

// **** SMUx0B_x8650 Register Definition ****
// Address
#define SMUx0B_x8650_ADDRESS                                    0x8650

// Type
#define SMUx0B_x8650_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8650_Data_OFFSET                                0
#define SMUx0B_x8650_Data_WIDTH                                 32
#define SMUx0B_x8650_Data_MASK                                  0xffffffff

/// SMUx0B_x8650
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8650_STRUCT;

// **** SMUx0B_x8654 Register Definition ****
// Address
#define SMUx0B_x8654_ADDRESS                                    0x8654

// Type
#define SMUx0B_x8654_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8654_Data_OFFSET                                0
#define SMUx0B_x8654_Data_WIDTH                                 32
#define SMUx0B_x8654_Data_MASK                                  0xffffffff

/// SMUx0B_x8654
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8654_STRUCT;

// **** SMUx0B_x8658 Register Definition ****
// Address
#define SMUx0B_x8658_ADDRESS                                    0x8658

// Type
#define SMUx0B_x8658_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8658_Data_OFFSET                                0
#define SMUx0B_x8658_Data_WIDTH                                 32
#define SMUx0B_x8658_Data_MASK                                  0xffffffff

/// SMUx0B_x8658
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8658_STRUCT;

// **** SMUx0B_x865C Register Definition ****
// Address
#define SMUx0B_x865C_ADDRESS                                    0x865c

// Type
#define SMUx0B_x865C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x865C_Data_OFFSET                                0
#define SMUx0B_x865C_Data_WIDTH                                 32
#define SMUx0B_x865C_Data_MASK                                  0xffffffff

/// SMUx0B_x865C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x865C_STRUCT;

// **** SMUx0B_x8660 Register Definition ****
// Address
#define SMUx0B_x8660_ADDRESS                                    0x8660

// Type
#define SMUx0B_x8660_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8660_Data_OFFSET                                0
#define SMUx0B_x8660_Data_WIDTH                                 32
#define SMUx0B_x8660_Data_MASK                                  0xffffffff

/// SMUx0B_x8660
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8660_STRUCT;

// **** SMUx0B_x8664 Register Definition ****
// Address
#define SMUx0B_x8664_ADDRESS                                    0x8664

// Type
#define SMUx0B_x8664_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8664_Data_OFFSET                                0
#define SMUx0B_x8664_Data_WIDTH                                 32
#define SMUx0B_x8664_Data_MASK                                  0xffffffff

/// SMUx0B_x8664
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8664_STRUCT;

// **** SMUx0B_x8668 Register Definition ****
// Address
#define SMUx0B_x8668_ADDRESS                                    0x8668

// Type
#define SMUx0B_x8668_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8668_Data_OFFSET                                0
#define SMUx0B_x8668_Data_WIDTH                                 32
#define SMUx0B_x8668_Data_MASK                                  0xffffffff

/// SMUx0B_x8668
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8668_STRUCT;

// **** SMUx0B_x866C Register Definition ****
// Address
#define SMUx0B_x866C_ADDRESS                                    0x866c

// Type
#define SMUx0B_x866C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x866C_Data_OFFSET                                0
#define SMUx0B_x866C_Data_WIDTH                                 32
#define SMUx0B_x866C_Data_MASK                                  0xffffffff

/// SMUx0B_x866C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x866C_STRUCT;

// **** SMUx0B_x8670 Register Definition ****
// Address
#define SMUx0B_x8670_ADDRESS                                    0x8670

// Type
#define SMUx0B_x8670_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8670_Data_OFFSET                                0
#define SMUx0B_x8670_Data_WIDTH                                 32
#define SMUx0B_x8670_Data_MASK                                  0xffffffff

/// SMUx0B_x8670
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8670_STRUCT;

// **** SMUx0B_x8674 Register Definition ****
// Address
#define SMUx0B_x8674_ADDRESS                                    0x8674

// Type
#define SMUx0B_x8674_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8674_Data_OFFSET                                0
#define SMUx0B_x8674_Data_WIDTH                                 32
#define SMUx0B_x8674_Data_MASK                                  0xffffffff

/// SMUx0B_x8674
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8674_STRUCT;

// **** SMUx0B_x8678 Register Definition ****
// Address
#define SMUx0B_x8678_ADDRESS                                    0x8678

// Type
#define SMUx0B_x8678_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8678_Data_OFFSET                                0
#define SMUx0B_x8678_Data_WIDTH                                 32
#define SMUx0B_x8678_Data_MASK                                  0xffffffff

/// SMUx0B_x8678
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8678_STRUCT;

// **** SMUx0B_x867C Register Definition ****
// Address
#define SMUx0B_x867C_ADDRESS                                    0x867c

// Type
#define SMUx0B_x867C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x867C_Data_OFFSET                                0
#define SMUx0B_x867C_Data_WIDTH                                 32
#define SMUx0B_x867C_Data_MASK                                  0xffffffff

/// SMUx0B_x867C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x867C_STRUCT;

// **** SMUx0B_x8680 Register Definition ****
// Address
#define SMUx0B_x8680_ADDRESS                                    0x8680

// Type
#define SMUx0B_x8680_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8680_Data_OFFSET                                0
#define SMUx0B_x8680_Data_WIDTH                                 32
#define SMUx0B_x8680_Data_MASK                                  0xffffffff

/// SMUx0B_x8680
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8680_STRUCT;

// **** SMUx0B_x8684 Register Definition ****
// Address
#define SMUx0B_x8684_ADDRESS                                    0x8684

// Type
#define SMUx0B_x8684_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8684_Data_OFFSET                                0
#define SMUx0B_x8684_Data_WIDTH                                 32
#define SMUx0B_x8684_Data_MASK                                  0xffffffff

/// SMUx0B_x8684
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8684_STRUCT;

// **** SMUx0B_x8688 Register Definition ****
// Address
#define SMUx0B_x8688_ADDRESS                                    0x8688

// Type
#define SMUx0B_x8688_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8688_Data_OFFSET                                0
#define SMUx0B_x8688_Data_WIDTH                                 32
#define SMUx0B_x8688_Data_MASK                                  0xffffffff

/// SMUx0B_x8688
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8688_STRUCT;

// **** SMUx0B_x868C Register Definition ****
// Address
#define SMUx0B_x868C_ADDRESS                                    0x868c

// Type
#define SMUx0B_x868C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x868C_Data_OFFSET                                0
#define SMUx0B_x868C_Data_WIDTH                                 32
#define SMUx0B_x868C_Data_MASK                                  0xffffffff

/// SMUx0B_x868C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x868C_STRUCT;

// **** SMUx0B_x8690 Register Definition ****
// Address
#define SMUx0B_x8690_ADDRESS                                    0x8690

// Type
#define SMUx0B_x8690_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8690_Data_OFFSET                                0
#define SMUx0B_x8690_Data_WIDTH                                 32
#define SMUx0B_x8690_Data_MASK                                  0xffffffff

/// SMUx0B_x8690
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8690_STRUCT;

// **** SMUx0B_x8694 Register Definition ****
// Address
#define SMUx0B_x8694_ADDRESS                                    0x8694

// Type
#define SMUx0B_x8694_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8694_Data_OFFSET                                0
#define SMUx0B_x8694_Data_WIDTH                                 32
#define SMUx0B_x8694_Data_MASK                                  0xffffffff

/// SMUx0B_x8694
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8694_STRUCT;

// **** SMUx0B_x8698 Register Definition ****
// Address
#define SMUx0B_x8698_ADDRESS                                    0x8698

// Type
#define SMUx0B_x8698_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8698_Data_OFFSET                                0
#define SMUx0B_x8698_Data_WIDTH                                 32
#define SMUx0B_x8698_Data_MASK                                  0xffffffff

/// SMUx0B_x8698
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8698_STRUCT;

// **** SMUx0B_x869C Register Definition ****
// Address
#define SMUx0B_x869C_ADDRESS                                    0x869c

// Type
#define SMUx0B_x869C_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x869C_Data_OFFSET                                0
#define SMUx0B_x869C_Data_WIDTH                                 32
#define SMUx0B_x869C_Data_MASK                                  0xffffffff

/// SMUx0B_x869C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x869C_STRUCT;

// **** SMUx0B_x86A0 Register Definition ****
// Address
#define SMUx0B_x86A0_ADDRESS                                    0x86a0

// Type
#define SMUx0B_x86A0_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x86A0_Data_OFFSET                                0
#define SMUx0B_x86A0_Data_WIDTH                                 32
#define SMUx0B_x86A0_Data_MASK                                  0xffffffff

/// SMUx0B_x86A0
typedef union {
  struct {                                                              ///<
    UINT32                                                     Data:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x86A0_STRUCT;

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
#define GMMx15C0_Reserved_17_0_OFFSET                           0
#define GMMx15C0_Reserved_17_0_WIDTH                            18
#define GMMx15C0_Reserved_17_0_MASK                             0x3ffff
#define GMMx15C0_Enable_OFFSET                                  18
#define GMMx15C0_Enable_WIDTH                                   1
#define GMMx15C0_Enable_MASK                                    0x40000
#define GMMx15C0_Reserved_31_19_OFFSET                          19
#define GMMx15C0_Reserved_31_19_WIDTH                           13
#define GMMx15C0_Reserved_31_19_MASK                            0xfff80000

/// GMMx15C0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx15C0_STRUCT;

// **** GMMx2014 Register Definition ****
// Address
#define GMMx2014_ADDRESS                                        0x2014

// Type
#define GMMx2014_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2014_Rlc_OFFSET                                     0
#define GMMx2014_Rlc_WIDTH                                      4
#define GMMx2014_Rlc_MASK                                       0xf
#define GMMx2014_Vmc_OFFSET                                     4
#define GMMx2014_Vmc_WIDTH                                      4
#define GMMx2014_Vmc_MASK                                       0xf0
#define GMMx2014_Dmif_OFFSET                                    8
#define GMMx2014_Dmif_WIDTH                                     4
#define GMMx2014_Dmif_MASK                                      0xf00
#define GMMx2014_Mcif_OFFSET                                    12
#define GMMx2014_Mcif_WIDTH                                     4
#define GMMx2014_Mcif_MASK                                      0xf000
#define GMMx2014_Reserved_31_16_OFFSET                          16
#define GMMx2014_Reserved_31_16_WIDTH                           16
#define GMMx2014_Reserved_31_16_MASK                            0xffff0000

/// GMMx2014
typedef union {
  struct {                                                              ///<
    UINT32                                                      Rlc:4 ; ///<
    UINT32                                                      Vmc:4 ; ///<
    UINT32                                                     Dmif:4 ; ///<
    UINT32                                                     Mcif:4 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2014_STRUCT;

// **** GMMx2018 Register Definition ****
// Address
#define GMMx2018_ADDRESS                                        0x2018

// Type
#define GMMx2018_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2018_Ih_OFFSET                                      0
#define GMMx2018_Ih_WIDTH                                       4
#define GMMx2018_Ih_MASK                                        0xf
#define GMMx2018_Mcif_OFFSET                                    4
#define GMMx2018_Mcif_WIDTH                                     4
#define GMMx2018_Mcif_MASK                                      0xf0
#define GMMx2018_Rlc_OFFSET                                     8
#define GMMx2018_Rlc_WIDTH                                      4
#define GMMx2018_Rlc_MASK                                       0xf00
#define GMMx2018_Vip_OFFSET                                     12
#define GMMx2018_Vip_WIDTH                                      4
#define GMMx2018_Vip_MASK                                       0xf000
#define GMMx2018_Reserved_31_16_OFFSET                          16
#define GMMx2018_Reserved_31_16_WIDTH                           16
#define GMMx2018_Reserved_31_16_MASK                            0xffff0000

/// GMMx2018
typedef union {
  struct {                                                              ///<
    UINT32                                                       Ih:4 ; ///<
    UINT32                                                     Mcif:4 ; ///<
    UINT32                                                      Rlc:4 ; ///<
    UINT32                                                      Vip:4 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2018_STRUCT;

// **** GMMx2020 Register Definition ****
// Address
#define GMMx2020_ADDRESS                                        0x2020

// Type
#define GMMx2020_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2020_UvdExt0_OFFSET                                 0
#define GMMx2020_UvdExt0_WIDTH                                  4
#define GMMx2020_UvdExt0_MASK                                   0xf
#define GMMx2020_DrmDma_OFFSET                                  4
#define GMMx2020_DrmDma_WIDTH                                   4
#define GMMx2020_DrmDma_MASK                                    0xf0
#define GMMx2020_Hdp_OFFSET                                     8
#define GMMx2020_Hdp_WIDTH                                      4
#define GMMx2020_Hdp_MASK                                       0xf00
#define GMMx2020_Sem_OFFSET                                     12
#define GMMx2020_Sem_WIDTH                                      4
#define GMMx2020_Sem_MASK                                       0xf000
#define GMMx2020_Umc_OFFSET                                     16
#define GMMx2020_Umc_WIDTH                                      4
#define GMMx2020_Umc_MASK                                       0xf0000
#define GMMx2020_Uvd_OFFSET                                     20
#define GMMx2020_Uvd_WIDTH                                      4
#define GMMx2020_Uvd_MASK                                       0xf00000
#define GMMx2020_Xdp_OFFSET                                     24
#define GMMx2020_Xdp_WIDTH                                      4
#define GMMx2020_Xdp_MASK                                       0xf000000
#define GMMx2020_UvdExt1_OFFSET                                 28
#define GMMx2020_UvdExt1_WIDTH                                  4
#define GMMx2020_UvdExt1_MASK                                   0xf0000000

/// GMMx2020
typedef union {
  struct {                                                              ///<
    UINT32                                                  UvdExt0:4 ; ///<
    UINT32                                                   DrmDma:4 ; ///<
    UINT32                                                      Hdp:4 ; ///<
    UINT32                                                      Sem:4 ; ///<
    UINT32                                                      Umc:4 ; ///<
    UINT32                                                      Uvd:4 ; ///<
    UINT32                                                      Xdp:4 ; ///<
    UINT32                                                  UvdExt1:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2020_STRUCT;

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

// **** GMMx2028 Register Definition ****
// Address
#define GMMx2028_ADDRESS                                        0x2028

// Type
#define GMMx2028_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2028_SysTop_39_22__OFFSET                           0
#define GMMx2028_SysTop_39_22__WIDTH                            18
#define GMMx2028_SysTop_39_22__MASK                             0x3ffff
#define GMMx2028_Reserved_31_18_OFFSET                          18
#define GMMx2028_Reserved_31_18_WIDTH                           14
#define GMMx2028_Reserved_31_18_MASK                            0xfffc0000

/// GMMx2028
typedef union {
  struct {                                                              ///<
    UINT32                                            SysTop_39_22_:18; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2028_STRUCT;

// **** GMMx202C Register Definition ****
// Address
#define GMMx202C_ADDRESS                                        0x202c

// Type
#define GMMx202C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx202C_SysBot_39_22__OFFSET                           0
#define GMMx202C_SysBot_39_22__WIDTH                            18
#define GMMx202C_SysBot_39_22__MASK                             0x3ffff
#define GMMx202C_Reserved_31_18_OFFSET                          18
#define GMMx202C_Reserved_31_18_WIDTH                           14
#define GMMx202C_Reserved_31_18_MASK                            0xfffc0000

/// GMMx202C
typedef union {
  struct {                                                              ///<
    UINT32                                            SysBot_39_22_:18; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx202C_STRUCT;

// **** GMMx20B4 Register Definition ****
// Address
#define GMMx20B4_ADDRESS                                        0x20b4

// Type
#define GMMx20B4_TYPE                                           TYPE_GMM
// Field Data
#define GMMx20B4_StutterMode_OFFSET                             0
#define GMMx20B4_StutterMode_WIDTH                              2
#define GMMx20B4_StutterMode_MASK                               0x3
#define GMMx20B4_GateOverride_OFFSET                            2
#define GMMx20B4_GateOverride_WIDTH                             1
#define GMMx20B4_GateOverride_MASK                              0x4
#define GMMx20B4_Reserved_31_3_OFFSET                           3
#define GMMx20B4_Reserved_31_3_WIDTH                            29
#define GMMx20B4_Reserved_31_3_MASK                             0xfffffff8

/// GMMx20B4
typedef union {
  struct {                                                              ///<
    UINT32                                              StutterMode:2 ; ///<
    UINT32                                             GateOverride:1 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx20B4_STRUCT;

// **** GMMx20B8 Register Definition ****
// Address
#define GMMx20B8_ADDRESS                                        0x20b8

// Type
#define GMMx20B8_TYPE                                           TYPE_GMM
// Field Data
#define GMMx20B8_Reserved_17_0_OFFSET                           0
#define GMMx20B8_Reserved_17_0_WIDTH                            18
#define GMMx20B8_Reserved_17_0_MASK                             0x3ffff
#define GMMx20B8_Enable_OFFSET                                  18
#define GMMx20B8_Enable_WIDTH                                   1
#define GMMx20B8_Enable_MASK                                    0x40000
#define GMMx20B8_Reserved_31_19_OFFSET                          19
#define GMMx20B8_Reserved_31_19_WIDTH                           13
#define GMMx20B8_Reserved_31_19_MASK                            0xfff80000

/// GMMx20B8
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx20B8_STRUCT;

// **** GMMx20BC Register Definition ****
// Address
#define GMMx20BC_ADDRESS                                        0x20bc

// Type
#define GMMx20BC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx20BC_Reserved_17_0_OFFSET                           0
#define GMMx20BC_Reserved_17_0_WIDTH                            18
#define GMMx20BC_Reserved_17_0_MASK                             0x3ffff
#define GMMx20BC_Enable_OFFSET                                  18
#define GMMx20BC_Enable_WIDTH                                   1
#define GMMx20BC_Enable_MASK                                    0x40000
#define GMMx20BC_Reserved_31_19_OFFSET                          19
#define GMMx20BC_Reserved_31_19_WIDTH                           13
#define GMMx20BC_Reserved_31_19_MASK                            0xfff80000

/// GMMx20BC
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx20BC_STRUCT;

// **** GMMx20C0 Register Definition ****
// Address
#define GMMx20C0_ADDRESS                                        0x20c0

// Type
#define GMMx20C0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx20C0_Reserved_17_0_OFFSET                           0
#define GMMx20C0_Reserved_17_0_WIDTH                            18
#define GMMx20C0_Reserved_17_0_MASK                             0x3ffff
#define GMMx20C0_Enable_OFFSET                                  18
#define GMMx20C0_Enable_WIDTH                                   1
#define GMMx20C0_Enable_MASK                                    0x40000
#define GMMx20C0_Reserved_31_19_OFFSET                          19
#define GMMx20C0_Reserved_31_19_WIDTH                           13
#define GMMx20C0_Reserved_31_19_MASK                            0xfff80000

/// GMMx20C0
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx20C0_STRUCT;

// **** GMMx20D4 Register Definition ****
// Address
#define GMMx20D4_ADDRESS                                        0x20d4

// Type
#define GMMx20D4_TYPE                                           TYPE_GMM
// Field Data
#define GMMx20D4_LocalBlackout_OFFSET                           0
#define GMMx20D4_LocalBlackout_WIDTH                            1
#define GMMx20D4_LocalBlackout_MASK                             0x1
#define GMMx20D4_Reserved_31_1_OFFSET                           1
#define GMMx20D4_Reserved_31_1_WIDTH                            31
#define GMMx20D4_Reserved_31_1_MASK                             0xfffffffe

/// GMMx20D4
typedef union {
  struct {                                                              ///<
    UINT32                                            LocalBlackout:1 ; ///<
    UINT32                                            Reserved_31_1:31; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx20D4_STRUCT;

// **** GMMx20EC Register Definition ****
// Address
#define GMMx20EC_ADDRESS                                        0x20ec

// Type
#define GMMx20EC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx20EC_RemoteBlackout_OFFSET                          0
#define GMMx20EC_RemoteBlackout_WIDTH                           1
#define GMMx20EC_RemoteBlackout_MASK                            0x1
#define GMMx20EC_LocalBlackout_OFFSET                           1
#define GMMx20EC_LocalBlackout_WIDTH                            1
#define GMMx20EC_LocalBlackout_MASK                             0x2
#define GMMx20EC_Reserved_31_2_OFFSET                           2
#define GMMx20EC_Reserved_31_2_WIDTH                            30
#define GMMx20EC_Reserved_31_2_MASK                             0xfffffffc

/// GMMx20EC
typedef union {
  struct {                                                              ///<
    UINT32                                           RemoteBlackout:1 ; ///<
    UINT32                                            LocalBlackout:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx20EC_STRUCT;

// **** GMMx21A4 Register Definition ****
// Address
#define GMMx21A4_ADDRESS                                        0x21a4

// Type
#define GMMx21A4_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21A4_Enable_OFFSET                                  0
#define GMMx21A4_Enable_WIDTH                                   1
#define GMMx21A4_Enable_MASK                                    0x1
#define GMMx21A4_Prescale_OFFSET                                1
#define GMMx21A4_Prescale_WIDTH                                 2
#define GMMx21A4_Prescale_MASK                                  0x6
#define GMMx21A4_BlackoutExempt_OFFSET                          3
#define GMMx21A4_BlackoutExempt_WIDTH                           1
#define GMMx21A4_BlackoutExempt_MASK                            0x8
#define GMMx21A4_StallMode_OFFSET                               4
#define GMMx21A4_StallMode_WIDTH                                2
#define GMMx21A4_StallMode_MASK                                 0x30
#define GMMx21A4_StallOverride_OFFSET                           6
#define GMMx21A4_StallOverride_WIDTH                            1
#define GMMx21A4_StallOverride_MASK                             0x40
#define GMMx21A4_MaxBurst_OFFSET                                7
#define GMMx21A4_MaxBurst_WIDTH                                 4
#define GMMx21A4_MaxBurst_MASK                                  0x780
#define GMMx21A4_LazyTimer_OFFSET                               11
#define GMMx21A4_LazyTimer_WIDTH                                4
#define GMMx21A4_LazyTimer_MASK                                 0x7800
#define GMMx21A4_StallOverrideWtm_OFFSET                        15
#define GMMx21A4_StallOverrideWtm_WIDTH                         1
#define GMMx21A4_StallOverrideWtm_MASK                          0x8000
#define GMMx21A4_Reserved_31_16_OFFSET                          16
#define GMMx21A4_Reserved_31_16_WIDTH                           16
#define GMMx21A4_Reserved_31_16_MASK                            0xffff0000

/// GMMx21A4
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21A4_STRUCT;

// **** GMMx21A8 Register Definition ****
// Address
#define GMMx21A8_ADDRESS                                        0x21a8

// Type
#define GMMx21A8_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21A8_Enable_OFFSET                                  0
#define GMMx21A8_Enable_WIDTH                                   1
#define GMMx21A8_Enable_MASK                                    0x1
#define GMMx21A8_Prescale_OFFSET                                1
#define GMMx21A8_Prescale_WIDTH                                 2
#define GMMx21A8_Prescale_MASK                                  0x6
#define GMMx21A8_BlackoutExempt_OFFSET                          3
#define GMMx21A8_BlackoutExempt_WIDTH                           1
#define GMMx21A8_BlackoutExempt_MASK                            0x8
#define GMMx21A8_StallMode_OFFSET                               4
#define GMMx21A8_StallMode_WIDTH                                2
#define GMMx21A8_StallMode_MASK                                 0x30
#define GMMx21A8_StallOverride_OFFSET                           6
#define GMMx21A8_StallOverride_WIDTH                            1
#define GMMx21A8_StallOverride_MASK                             0x40
#define GMMx21A8_MaxBurst_OFFSET                                7
#define GMMx21A8_MaxBurst_WIDTH                                 4
#define GMMx21A8_MaxBurst_MASK                                  0x780
#define GMMx21A8_LazyTimer_OFFSET                               11
#define GMMx21A8_LazyTimer_WIDTH                                4
#define GMMx21A8_LazyTimer_MASK                                 0x7800
#define GMMx21A8_StallOverrideWtm_OFFSET                        15
#define GMMx21A8_StallOverrideWtm_WIDTH                         1
#define GMMx21A8_StallOverrideWtm_MASK                          0x8000
#define GMMx21A8_Reserved_31_16_OFFSET                          16
#define GMMx21A8_Reserved_31_16_WIDTH                           16
#define GMMx21A8_Reserved_31_16_MASK                            0xffff0000

/// GMMx21A8
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21A8_STRUCT;

// **** GMMx21AC Register Definition ****
// Address
#define GMMx21AC_ADDRESS                                        0x21ac

// Type
#define GMMx21AC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21AC_Enable_OFFSET                                  0
#define GMMx21AC_Enable_WIDTH                                   1
#define GMMx21AC_Enable_MASK                                    0x1
#define GMMx21AC_Prescale_OFFSET                                1
#define GMMx21AC_Prescale_WIDTH                                 2
#define GMMx21AC_Prescale_MASK                                  0x6
#define GMMx21AC_BlackoutExempt_OFFSET                          3
#define GMMx21AC_BlackoutExempt_WIDTH                           1
#define GMMx21AC_BlackoutExempt_MASK                            0x8
#define GMMx21AC_StallMode_OFFSET                               4
#define GMMx21AC_StallMode_WIDTH                                2
#define GMMx21AC_StallMode_MASK                                 0x30
#define GMMx21AC_StallOverride_OFFSET                           6
#define GMMx21AC_StallOverride_WIDTH                            1
#define GMMx21AC_StallOverride_MASK                             0x40
#define GMMx21AC_MaxBurst_OFFSET                                7
#define GMMx21AC_MaxBurst_WIDTH                                 4
#define GMMx21AC_MaxBurst_MASK                                  0x780
#define GMMx21AC_LazyTimer_OFFSET                               11
#define GMMx21AC_LazyTimer_WIDTH                                4
#define GMMx21AC_LazyTimer_MASK                                 0x7800
#define GMMx21AC_StallOverrideWtm_OFFSET                        15
#define GMMx21AC_StallOverrideWtm_WIDTH                         1
#define GMMx21AC_StallOverrideWtm_MASK                          0x8000
#define GMMx21AC_Reserved_31_16_OFFSET                          16
#define GMMx21AC_Reserved_31_16_WIDTH                           16
#define GMMx21AC_Reserved_31_16_MASK                            0xffff0000

/// GMMx21AC
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21AC_STRUCT;

// **** GMMx21B0 Register Definition ****
// Address
#define GMMx21B0_ADDRESS                                        0x21b0

// Type
#define GMMx21B0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21B0_Enable_OFFSET                                  0
#define GMMx21B0_Enable_WIDTH                                   1
#define GMMx21B0_Enable_MASK                                    0x1
#define GMMx21B0_Prescale_OFFSET                                1
#define GMMx21B0_Prescale_WIDTH                                 2
#define GMMx21B0_Prescale_MASK                                  0x6
#define GMMx21B0_BlackoutExempt_OFFSET                          3
#define GMMx21B0_BlackoutExempt_WIDTH                           1
#define GMMx21B0_BlackoutExempt_MASK                            0x8
#define GMMx21B0_StallMode_OFFSET                               4
#define GMMx21B0_StallMode_WIDTH                                2
#define GMMx21B0_StallMode_MASK                                 0x30
#define GMMx21B0_StallOverride_OFFSET                           6
#define GMMx21B0_StallOverride_WIDTH                            1
#define GMMx21B0_StallOverride_MASK                             0x40
#define GMMx21B0_MaxBurst_OFFSET                                7
#define GMMx21B0_MaxBurst_WIDTH                                 4
#define GMMx21B0_MaxBurst_MASK                                  0x780
#define GMMx21B0_LazyTimer_OFFSET                               11
#define GMMx21B0_LazyTimer_WIDTH                                4
#define GMMx21B0_LazyTimer_MASK                                 0x7800
#define GMMx21B0_StallOverrideWtm_OFFSET                        15
#define GMMx21B0_StallOverrideWtm_WIDTH                         1
#define GMMx21B0_StallOverrideWtm_MASK                          0x8000
#define GMMx21B0_Reserved_31_16_OFFSET                          16
#define GMMx21B0_Reserved_31_16_WIDTH                           16
#define GMMx21B0_Reserved_31_16_MASK                            0xffff0000

/// GMMx21B0
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21B0_STRUCT;

// **** GMMx21B4 Register Definition ****
// Address
#define GMMx21B4_ADDRESS                                        0x21b4

// Type
#define GMMx21B4_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21B4_Enable_OFFSET                                  0
#define GMMx21B4_Enable_WIDTH                                   1
#define GMMx21B4_Enable_MASK                                    0x1
#define GMMx21B4_Prescale_OFFSET                                1
#define GMMx21B4_Prescale_WIDTH                                 2
#define GMMx21B4_Prescale_MASK                                  0x6
#define GMMx21B4_BlackoutExempt_OFFSET                          3
#define GMMx21B4_BlackoutExempt_WIDTH                           1
#define GMMx21B4_BlackoutExempt_MASK                            0x8
#define GMMx21B4_StallMode_OFFSET                               4
#define GMMx21B4_StallMode_WIDTH                                2
#define GMMx21B4_StallMode_MASK                                 0x30
#define GMMx21B4_StallOverride_OFFSET                           6
#define GMMx21B4_StallOverride_WIDTH                            1
#define GMMx21B4_StallOverride_MASK                             0x40
#define GMMx21B4_MaxBurst_OFFSET                                7
#define GMMx21B4_MaxBurst_WIDTH                                 4
#define GMMx21B4_MaxBurst_MASK                                  0x780
#define GMMx21B4_LazyTimer_OFFSET                               11
#define GMMx21B4_LazyTimer_WIDTH                                4
#define GMMx21B4_LazyTimer_MASK                                 0x7800
#define GMMx21B4_StallOverrideWtm_OFFSET                        15
#define GMMx21B4_StallOverrideWtm_WIDTH                         1
#define GMMx21B4_StallOverrideWtm_MASK                          0x8000
#define GMMx21B4_Reserved_31_16_OFFSET                          16
#define GMMx21B4_Reserved_31_16_WIDTH                           16
#define GMMx21B4_Reserved_31_16_MASK                            0xffff0000

/// GMMx21B4
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21B4_STRUCT;

// **** GMMx21B8 Register Definition ****
// Address
#define GMMx21B8_ADDRESS                                        0x21b8

// Type
#define GMMx21B8_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21B8_Enable_OFFSET                                  0
#define GMMx21B8_Enable_WIDTH                                   1
#define GMMx21B8_Enable_MASK                                    0x1
#define GMMx21B8_Prescale_OFFSET                                1
#define GMMx21B8_Prescale_WIDTH                                 2
#define GMMx21B8_Prescale_MASK                                  0x6
#define GMMx21B8_BlackoutExempt_OFFSET                          3
#define GMMx21B8_BlackoutExempt_WIDTH                           1
#define GMMx21B8_BlackoutExempt_MASK                            0x8
#define GMMx21B8_StallMode_OFFSET                               4
#define GMMx21B8_StallMode_WIDTH                                2
#define GMMx21B8_StallMode_MASK                                 0x30
#define GMMx21B8_StallOverride_OFFSET                           6
#define GMMx21B8_StallOverride_WIDTH                            1
#define GMMx21B8_StallOverride_MASK                             0x40
#define GMMx21B8_MaxBurst_OFFSET                                7
#define GMMx21B8_MaxBurst_WIDTH                                 4
#define GMMx21B8_MaxBurst_MASK                                  0x780
#define GMMx21B8_LazyTimer_OFFSET                               11
#define GMMx21B8_LazyTimer_WIDTH                                4
#define GMMx21B8_LazyTimer_MASK                                 0x7800
#define GMMx21B8_StallOverrideWtm_OFFSET                        15
#define GMMx21B8_StallOverrideWtm_WIDTH                         1
#define GMMx21B8_StallOverrideWtm_MASK                          0x8000
#define GMMx21B8_Reserved_31_16_OFFSET                          16
#define GMMx21B8_Reserved_31_16_WIDTH                           16
#define GMMx21B8_Reserved_31_16_MASK                            0xffff0000

/// GMMx21B8
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21B8_STRUCT;

// **** GMMx21BC Register Definition ****
// Address
#define GMMx21BC_ADDRESS                                        0x21bc

// Type
#define GMMx21BC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21BC_Enable_OFFSET                                  0
#define GMMx21BC_Enable_WIDTH                                   1
#define GMMx21BC_Enable_MASK                                    0x1
#define GMMx21BC_Prescale_OFFSET                                1
#define GMMx21BC_Prescale_WIDTH                                 2
#define GMMx21BC_Prescale_MASK                                  0x6
#define GMMx21BC_BlackoutExempt_OFFSET                          3
#define GMMx21BC_BlackoutExempt_WIDTH                           1
#define GMMx21BC_BlackoutExempt_MASK                            0x8
#define GMMx21BC_StallMode_OFFSET                               4
#define GMMx21BC_StallMode_WIDTH                                2
#define GMMx21BC_StallMode_MASK                                 0x30
#define GMMx21BC_StallOverride_OFFSET                           6
#define GMMx21BC_StallOverride_WIDTH                            1
#define GMMx21BC_StallOverride_MASK                             0x40
#define GMMx21BC_MaxBurst_OFFSET                                7
#define GMMx21BC_MaxBurst_WIDTH                                 4
#define GMMx21BC_MaxBurst_MASK                                  0x780
#define GMMx21BC_LazyTimer_OFFSET                               11
#define GMMx21BC_LazyTimer_WIDTH                                4
#define GMMx21BC_LazyTimer_MASK                                 0x7800
#define GMMx21BC_StallOverrideWtm_OFFSET                        15
#define GMMx21BC_StallOverrideWtm_WIDTH                         1
#define GMMx21BC_StallOverrideWtm_MASK                          0x8000
#define GMMx21BC_Reserved_31_16_OFFSET                          16
#define GMMx21BC_Reserved_31_16_WIDTH                           16
#define GMMx21BC_Reserved_31_16_MASK                            0xffff0000

/// GMMx21BC
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21BC_STRUCT;

// **** GMMx21C0 Register Definition ****
// Address
#define GMMx21C0_ADDRESS                                        0x21c0

// Type
#define GMMx21C0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21C0_Enable_OFFSET                                  0
#define GMMx21C0_Enable_WIDTH                                   1
#define GMMx21C0_Enable_MASK                                    0x1
#define GMMx21C0_Prescale_OFFSET                                1
#define GMMx21C0_Prescale_WIDTH                                 2
#define GMMx21C0_Prescale_MASK                                  0x6
#define GMMx21C0_BlackoutExempt_OFFSET                          3
#define GMMx21C0_BlackoutExempt_WIDTH                           1
#define GMMx21C0_BlackoutExempt_MASK                            0x8
#define GMMx21C0_StallMode_OFFSET                               4
#define GMMx21C0_StallMode_WIDTH                                2
#define GMMx21C0_StallMode_MASK                                 0x30
#define GMMx21C0_StallOverride_OFFSET                           6
#define GMMx21C0_StallOverride_WIDTH                            1
#define GMMx21C0_StallOverride_MASK                             0x40
#define GMMx21C0_MaxBurst_OFFSET                                7
#define GMMx21C0_MaxBurst_WIDTH                                 4
#define GMMx21C0_MaxBurst_MASK                                  0x780
#define GMMx21C0_LazyTimer_OFFSET                               11
#define GMMx21C0_LazyTimer_WIDTH                                4
#define GMMx21C0_LazyTimer_MASK                                 0x7800
#define GMMx21C0_StallOverrideWtm_OFFSET                        15
#define GMMx21C0_StallOverrideWtm_WIDTH                         1
#define GMMx21C0_StallOverrideWtm_MASK                          0x8000
#define GMMx21C0_Reserved_31_16_OFFSET                          16
#define GMMx21C0_Reserved_31_16_WIDTH                           16
#define GMMx21C0_Reserved_31_16_MASK                            0xffff0000

/// GMMx21C0
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21C0_STRUCT;

// **** GMMx21C4 Register Definition ****
// Address
#define GMMx21C4_ADDRESS                                        0x21c4

// Type
#define GMMx21C4_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21C4_Enable_OFFSET                                  0
#define GMMx21C4_Enable_WIDTH                                   1
#define GMMx21C4_Enable_MASK                                    0x1
#define GMMx21C4_Prescale_OFFSET                                1
#define GMMx21C4_Prescale_WIDTH                                 2
#define GMMx21C4_Prescale_MASK                                  0x6
#define GMMx21C4_BlackoutExempt_OFFSET                          3
#define GMMx21C4_BlackoutExempt_WIDTH                           1
#define GMMx21C4_BlackoutExempt_MASK                            0x8
#define GMMx21C4_StallMode_OFFSET                               4
#define GMMx21C4_StallMode_WIDTH                                2
#define GMMx21C4_StallMode_MASK                                 0x30
#define GMMx21C4_StallOverride_OFFSET                           6
#define GMMx21C4_StallOverride_WIDTH                            1
#define GMMx21C4_StallOverride_MASK                             0x40
#define GMMx21C4_MaxBurst_OFFSET                                7
#define GMMx21C4_MaxBurst_WIDTH                                 4
#define GMMx21C4_MaxBurst_MASK                                  0x780
#define GMMx21C4_LazyTimer_OFFSET                               11
#define GMMx21C4_LazyTimer_WIDTH                                4
#define GMMx21C4_LazyTimer_MASK                                 0x7800
#define GMMx21C4_StallOverrideWtm_OFFSET                        15
#define GMMx21C4_StallOverrideWtm_WIDTH                         1
#define GMMx21C4_StallOverrideWtm_MASK                          0x8000
#define GMMx21C4_Reserved_31_16_OFFSET                          16
#define GMMx21C4_Reserved_31_16_WIDTH                           16
#define GMMx21C4_Reserved_31_16_MASK                            0xffff0000

/// GMMx21C4
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21C4_STRUCT;

// **** GMMx21C8 Register Definition ****
// Address
#define GMMx21C8_ADDRESS                                        0x21c8

// Type
#define GMMx21C8_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21C8_Enable_OFFSET                                  0
#define GMMx21C8_Enable_WIDTH                                   1
#define GMMx21C8_Enable_MASK                                    0x1
#define GMMx21C8_Prescale_OFFSET                                1
#define GMMx21C8_Prescale_WIDTH                                 2
#define GMMx21C8_Prescale_MASK                                  0x6
#define GMMx21C8_BlackoutExempt_OFFSET                          3
#define GMMx21C8_BlackoutExempt_WIDTH                           1
#define GMMx21C8_BlackoutExempt_MASK                            0x8
#define GMMx21C8_StallMode_OFFSET                               4
#define GMMx21C8_StallMode_WIDTH                                2
#define GMMx21C8_StallMode_MASK                                 0x30
#define GMMx21C8_StallOverride_OFFSET                           6
#define GMMx21C8_StallOverride_WIDTH                            1
#define GMMx21C8_StallOverride_MASK                             0x40
#define GMMx21C8_MaxBurst_OFFSET                                7
#define GMMx21C8_MaxBurst_WIDTH                                 4
#define GMMx21C8_MaxBurst_MASK                                  0x780
#define GMMx21C8_LazyTimer_OFFSET                               11
#define GMMx21C8_LazyTimer_WIDTH                                4
#define GMMx21C8_LazyTimer_MASK                                 0x7800
#define GMMx21C8_StallOverrideWtm_OFFSET                        15
#define GMMx21C8_StallOverrideWtm_WIDTH                         1
#define GMMx21C8_StallOverrideWtm_MASK                          0x8000
#define GMMx21C8_Reserved_31_16_OFFSET                          16
#define GMMx21C8_Reserved_31_16_WIDTH                           16
#define GMMx21C8_Reserved_31_16_MASK                            0xffff0000

/// GMMx21C8
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21C8_STRUCT;

// **** GMMx21CC Register Definition ****
// Address
#define GMMx21CC_ADDRESS                                        0x21cc

// Type
#define GMMx21CC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21CC_Enable_OFFSET                                  0
#define GMMx21CC_Enable_WIDTH                                   1
#define GMMx21CC_Enable_MASK                                    0x1
#define GMMx21CC_Prescale_OFFSET                                1
#define GMMx21CC_Prescale_WIDTH                                 2
#define GMMx21CC_Prescale_MASK                                  0x6
#define GMMx21CC_BlackoutExempt_OFFSET                          3
#define GMMx21CC_BlackoutExempt_WIDTH                           1
#define GMMx21CC_BlackoutExempt_MASK                            0x8
#define GMMx21CC_StallMode_OFFSET                               4
#define GMMx21CC_StallMode_WIDTH                                2
#define GMMx21CC_StallMode_MASK                                 0x30
#define GMMx21CC_StallOverride_OFFSET                           6
#define GMMx21CC_StallOverride_WIDTH                            1
#define GMMx21CC_StallOverride_MASK                             0x40
#define GMMx21CC_MaxBurst_OFFSET                                7
#define GMMx21CC_MaxBurst_WIDTH                                 4
#define GMMx21CC_MaxBurst_MASK                                  0x780
#define GMMx21CC_LazyTimer_OFFSET                               11
#define GMMx21CC_LazyTimer_WIDTH                                4
#define GMMx21CC_LazyTimer_MASK                                 0x7800
#define GMMx21CC_StallOverrideWtm_OFFSET                        15
#define GMMx21CC_StallOverrideWtm_WIDTH                         1
#define GMMx21CC_StallOverrideWtm_MASK                          0x8000
#define GMMx21CC_Reserved_31_16_OFFSET                          16
#define GMMx21CC_Reserved_31_16_WIDTH                           16
#define GMMx21CC_Reserved_31_16_MASK                            0xffff0000

/// GMMx21CC
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21CC_STRUCT;

// **** GMMx21D0 Register Definition ****
// Address
#define GMMx21D0_ADDRESS                                        0x21d0

// Type
#define GMMx21D0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx21D0_Enable_OFFSET                                  0
#define GMMx21D0_Enable_WIDTH                                   1
#define GMMx21D0_Enable_MASK                                    0x1
#define GMMx21D0_Prescale_OFFSET                                1
#define GMMx21D0_Prescale_WIDTH                                 2
#define GMMx21D0_Prescale_MASK                                  0x6
#define GMMx21D0_BlackoutExempt_OFFSET                          3
#define GMMx21D0_BlackoutExempt_WIDTH                           1
#define GMMx21D0_BlackoutExempt_MASK                            0x8
#define GMMx21D0_StallMode_OFFSET                               4
#define GMMx21D0_StallMode_WIDTH                                2
#define GMMx21D0_StallMode_MASK                                 0x30
#define GMMx21D0_StallOverride_OFFSET                           6
#define GMMx21D0_StallOverride_WIDTH                            1
#define GMMx21D0_StallOverride_MASK                             0x40
#define GMMx21D0_MaxBurst_OFFSET                                7
#define GMMx21D0_MaxBurst_WIDTH                                 4
#define GMMx21D0_MaxBurst_MASK                                  0x780
#define GMMx21D0_LazyTimer_OFFSET                               11
#define GMMx21D0_LazyTimer_WIDTH                                4
#define GMMx21D0_LazyTimer_MASK                                 0x7800
#define GMMx21D0_StallOverrideWtm_OFFSET                        15
#define GMMx21D0_StallOverrideWtm_WIDTH                         1
#define GMMx21D0_StallOverrideWtm_MASK                          0x8000
#define GMMx21D0_Reserved_31_16_OFFSET                          16
#define GMMx21D0_Reserved_31_16_WIDTH                           16
#define GMMx21D0_Reserved_31_16_MASK                            0xffff0000

/// GMMx21D0
typedef union {
  struct {                                                              ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 Prescale:2 ; ///<
    UINT32                                           BlackoutExempt:1 ; ///<
    UINT32                                                StallMode:2 ; ///<
    UINT32                                            StallOverride:1 ; ///<
    UINT32                                                 MaxBurst:4 ; ///<
    UINT32                                                LazyTimer:4 ; ///<
    UINT32                                         StallOverrideWtm:1 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx21D0_STRUCT;

// **** GMMx25C0 Register Definition ****
// Address
#define GMMx25C0_ADDRESS                                        0x25c0

// Type
#define GMMx25C0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx25C0_BlackoutRd_OFFSET                              0
#define GMMx25C0_BlackoutRd_WIDTH                               1
#define GMMx25C0_BlackoutRd_MASK                                0x1
#define GMMx25C0_BlackoutWr_OFFSET                              1
#define GMMx25C0_BlackoutWr_WIDTH                               1
#define GMMx25C0_BlackoutWr_MASK                                0x2
#define GMMx25C0_Reserved_31_2_OFFSET                           2
#define GMMx25C0_Reserved_31_2_WIDTH                            30
#define GMMx25C0_Reserved_31_2_MASK                             0xfffffffc

/// GMMx25C0
typedef union {
  struct {                                                              ///<
    UINT32                                               BlackoutRd:1 ; ///<
    UINT32                                               BlackoutWr:1 ; ///<
    UINT32                                            Reserved_31_2:30; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx25C0_STRUCT;

// **** GMMx25C8 Register Definition ****
// Address
#define GMMx25C8_ADDRESS                                        0x25c8

// Type
#define GMMx25C8_TYPE                                           TYPE_GMM
// Field Data
#define GMMx25C8_ReadLcl_OFFSET                                 0
#define GMMx25C8_ReadLcl_WIDTH                                  8
#define GMMx25C8_ReadLcl_MASK                                   0xff
#define GMMx25C8_ReadHub_OFFSET                                 8
#define GMMx25C8_ReadHub_WIDTH                                  8
#define GMMx25C8_ReadHub_MASK                                   0xff00
#define GMMx25C8_ReadPri_OFFSET                                 16
#define GMMx25C8_ReadPri_WIDTH                                  8
#define GMMx25C8_ReadPri_MASK                                   0xff0000
#define GMMx25C8_LclPri_OFFSET                                  24
#define GMMx25C8_LclPri_WIDTH                                   1
#define GMMx25C8_LclPri_MASK                                    0x1000000
#define GMMx25C8_HubPri_OFFSET                                  25
#define GMMx25C8_HubPri_WIDTH                                   1
#define GMMx25C8_HubPri_MASK                                    0x2000000
#define GMMx25C8_Reserved_31_26_OFFSET                          26
#define GMMx25C8_Reserved_31_26_WIDTH                           6
#define GMMx25C8_Reserved_31_26_MASK                            0xfc000000

/// GMMx25C8
typedef union {
  struct {                                                              ///<
    UINT32                                                  ReadLcl:8 ; ///<
    UINT32                                                  ReadHub:8 ; ///<
    UINT32                                                  ReadPri:8 ; ///<
    UINT32                                                   LclPri:1 ; ///<
    UINT32                                                   HubPri:1 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx25C8_STRUCT;

// **** GMMx25CC Register Definition ****
// Address
#define GMMx25CC_ADDRESS                                        0x25cc

// Type
#define GMMx25CC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx25CC_WriteLcl_OFFSET                                0
#define GMMx25CC_WriteLcl_WIDTH                                 8
#define GMMx25CC_WriteLcl_MASK                                  0xff
#define GMMx25CC_WriteHub_OFFSET                                8
#define GMMx25CC_WriteHub_WIDTH                                 8
#define GMMx25CC_WriteHub_MASK                                  0xff00
#define GMMx25CC_HubPri_OFFSET                                  16
#define GMMx25CC_HubPri_WIDTH                                   1
#define GMMx25CC_HubPri_MASK                                    0x10000
#define GMMx25CC_Reserved_31_17_OFFSET                          17
#define GMMx25CC_Reserved_31_17_WIDTH                           15
#define GMMx25CC_Reserved_31_17_MASK                            0xfffe0000

/// GMMx25CC
typedef union {
  struct {                                                              ///<
    UINT32                                                 WriteLcl:8 ; ///<
    UINT32                                                 WriteHub:8 ; ///<
    UINT32                                                   HubPri:1 ; ///<
    UINT32                                           Reserved_31_17:15; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx25CC_STRUCT;

// **** GMMx2610 Register Definition ****
// Address
#define GMMx2610_ADDRESS                                        0x2610

// Type
#define GMMx2610_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2610_TctFetch0_OFFSET                               0
#define GMMx2610_TctFetch0_WIDTH                                4
#define GMMx2610_TctFetch0_MASK                                 0xf
#define GMMx2610_TcvFetch0_OFFSET                               4
#define GMMx2610_TcvFetch0_WIDTH                                4
#define GMMx2610_TcvFetch0_MASK                                 0xf0
#define GMMx2610_Vc0_OFFSET                                     8
#define GMMx2610_Vc0_WIDTH                                      4
#define GMMx2610_Vc0_MASK                                       0xf00
#define GMMx2610_Cb0_OFFSET                                     12
#define GMMx2610_Cb0_WIDTH                                      4
#define GMMx2610_Cb0_MASK                                       0xf000
#define GMMx2610_CbcMask0_OFFSET                                16
#define GMMx2610_CbcMask0_WIDTH                                 4
#define GMMx2610_CbcMask0_MASK                                  0xf0000
#define GMMx2610_CbfMask0_OFFSET                                20
#define GMMx2610_CbfMask0_WIDTH                                 4
#define GMMx2610_CbfMask0_MASK                                  0xf00000
#define GMMx2610_Db0_OFFSET                                     24
#define GMMx2610_Db0_WIDTH                                      4
#define GMMx2610_Db0_MASK                                       0xf000000
#define GMMx2610_DbhTile0_OFFSET                                28
#define GMMx2610_DbhTile0_WIDTH                                 4
#define GMMx2610_DbhTile0_MASK                                  0xf0000000

/// GMMx2610
typedef union {
  struct {                                                              ///<
    UINT32                                                TctFetch0:4 ; ///<
    UINT32                                                TcvFetch0:4 ; ///<
    UINT32                                                      Vc0:4 ; ///<
    UINT32                                                      Cb0:4 ; ///<
    UINT32                                                 CbcMask0:4 ; ///<
    UINT32                                                 CbfMask0:4 ; ///<
    UINT32                                                      Db0:4 ; ///<
    UINT32                                                 DbhTile0:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2610_STRUCT;

// **** GMMx2614 Register Definition ****
// Address
#define GMMx2614_ADDRESS                                        0x2614

// Type
#define GMMx2614_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2614_Cb0_OFFSET                                     0
#define GMMx2614_Cb0_WIDTH                                      4
#define GMMx2614_Cb0_MASK                                       0xf
#define GMMx2614_CbcMask0_OFFSET                                4
#define GMMx2614_CbcMask0_WIDTH                                 4
#define GMMx2614_CbcMask0_MASK                                  0xf0
#define GMMx2614_CbfMask0_OFFSET                                8
#define GMMx2614_CbfMask0_WIDTH                                 4
#define GMMx2614_CbfMask0_MASK                                  0xf00
#define GMMx2614_Db0_OFFSET                                     12
#define GMMx2614_Db0_WIDTH                                      4
#define GMMx2614_Db0_MASK                                       0xf000
#define GMMx2614_DbhTile0_OFFSET                                16
#define GMMx2614_DbhTile0_WIDTH                                 4
#define GMMx2614_DbhTile0_MASK                                  0xf0000
#define GMMx2614_Sx0_OFFSET                                     20
#define GMMx2614_Sx0_WIDTH                                      4
#define GMMx2614_Sx0_MASK                                       0xf00000
#define GMMx2614_Bcast0_OFFSET                                  24
#define GMMx2614_Bcast0_WIDTH                                   4
#define GMMx2614_Bcast0_MASK                                    0xf000000
#define GMMx2614_Cbimmed0_OFFSET                                28
#define GMMx2614_Cbimmed0_WIDTH                                 4
#define GMMx2614_Cbimmed0_MASK                                  0xf0000000

/// GMMx2614
typedef union {
  struct {                                                              ///<
    UINT32                                                      Cb0:4 ; ///<
    UINT32                                                 CbcMask0:4 ; ///<
    UINT32                                                 CbfMask0:4 ; ///<
    UINT32                                                      Db0:4 ; ///<
    UINT32                                                 DbhTile0:4 ; ///<
    UINT32                                                      Sx0:4 ; ///<
    UINT32                                                   Bcast0:4 ; ///<
    UINT32                                                 Cbimmed0:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2614_STRUCT;

// **** GMMx2618 Register Definition ****
// Address
#define GMMx2618_ADDRESS                                        0x2618

// Type
#define GMMx2618_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2618_DbstEn0_OFFSET                                 0
#define GMMx2618_DbstEn0_WIDTH                                  4
#define GMMx2618_DbstEn0_MASK                                   0xf
#define GMMx2618_TcvFetch1_OFFSET                               4
#define GMMx2618_TcvFetch1_WIDTH                                4
#define GMMx2618_TcvFetch1_MASK                                 0xf0
#define GMMx2618_TctFetch1_OFFSET                               8
#define GMMx2618_TctFetch1_WIDTH                                4
#define GMMx2618_TctFetch1_MASK                                 0xf00
#define GMMx2618_Vc1_OFFSET                                     12
#define GMMx2618_Vc1_WIDTH                                      4
#define GMMx2618_Vc1_MASK                                       0xf000
#define GMMx2618_Reserved_31_16_OFFSET                          16
#define GMMx2618_Reserved_31_16_WIDTH                           16
#define GMMx2618_Reserved_31_16_MASK                            0xffff0000

/// GMMx2618
typedef union {
  struct {                                                              ///<
    UINT32                                                  DbstEn0:4 ; ///<
    UINT32                                                TcvFetch1:4 ; ///<
    UINT32                                                TctFetch1:4 ; ///<
    UINT32                                                      Vc1:4 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2618_STRUCT;

// **** GMMx261C Register Definition ****
// Address
#define GMMx261C_ADDRESS                                        0x261c

// Type
#define GMMx261C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx261C_DbstEn0_OFFSET                                 0
#define GMMx261C_DbstEn0_WIDTH                                  4
#define GMMx261C_DbstEn0_MASK                                   0xf
#define GMMx261C_Reserved_31_4_OFFSET                           4
#define GMMx261C_Reserved_31_4_WIDTH                            28
#define GMMx261C_Reserved_31_4_MASK                             0xfffffff0

/// GMMx261C
typedef union {
  struct {                                                              ///<
    UINT32                                                  DbstEn0:4 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx261C_STRUCT;

// **** GMMx2638 Register Definition ****
// Address
#define GMMx2638_ADDRESS                                        0x2638

// Type
#define GMMx2638_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2638_Reserved_17_0_OFFSET                           0
#define GMMx2638_Reserved_17_0_WIDTH                            18
#define GMMx2638_Reserved_17_0_MASK                             0x3ffff
#define GMMx2638_Enable_OFFSET                                  18
#define GMMx2638_Enable_WIDTH                                   1
#define GMMx2638_Enable_MASK                                    0x40000
#define GMMx2638_Reserved_31_19_OFFSET                          19
#define GMMx2638_Reserved_31_19_WIDTH                           13
#define GMMx2638_Reserved_31_19_MASK                            0xfff80000

/// GMMx2638
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2638_STRUCT;

// **** GMMx263C Register Definition ****
// Address
#define GMMx263C_ADDRESS                                        0x263c

// Type
#define GMMx263C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx263C_Reserved_17_0_OFFSET                           0
#define GMMx263C_Reserved_17_0_WIDTH                            18
#define GMMx263C_Reserved_17_0_MASK                             0x3ffff
#define GMMx263C_Enable_OFFSET                                  18
#define GMMx263C_Enable_WIDTH                                   1
#define GMMx263C_Enable_MASK                                    0x40000
#define GMMx263C_Reserved_31_19_OFFSET                          19
#define GMMx263C_Reserved_31_19_WIDTH                           13
#define GMMx263C_Reserved_31_19_MASK                            0xfff80000

/// GMMx263C
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx263C_STRUCT;

// **** GMMx2640 Register Definition ****
// Address
#define GMMx2640_ADDRESS                                        0x2640

// Type
#define GMMx2640_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2640_Reserved_17_0_OFFSET                           0
#define GMMx2640_Reserved_17_0_WIDTH                            18
#define GMMx2640_Reserved_17_0_MASK                             0x3ffff
#define GMMx2640_Enable_OFFSET                                  18
#define GMMx2640_Enable_WIDTH                                   1
#define GMMx2640_Enable_MASK                                    0x40000
#define GMMx2640_Reserved_31_19_OFFSET                          19
#define GMMx2640_Reserved_31_19_WIDTH                           13
#define GMMx2640_Reserved_31_19_MASK                            0xfff80000

/// GMMx2640
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_17_0:18; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2640_STRUCT;

// **** GMMx277C Register Definition ****
// Address
#define GMMx277C_ADDRESS                                        0x277c

// Type
#define GMMx277C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx277C_ActRd_OFFSET                                   0
#define GMMx277C_ActRd_WIDTH                                    8
#define GMMx277C_ActRd_MASK                                     0xff
#define GMMx277C_ActWr_OFFSET                                   8
#define GMMx277C_ActWr_WIDTH                                    8
#define GMMx277C_ActWr_MASK                                     0xff00
#define GMMx277C_RasMActRd_OFFSET                               16
#define GMMx277C_RasMActRd_WIDTH                                8
#define GMMx277C_RasMActRd_MASK                                 0xff0000
#define GMMx277C_RasMActWr_OFFSET                               24
#define GMMx277C_RasMActWr_WIDTH                                8
#define GMMx277C_RasMActWr_MASK                                 0xff000000

/// GMMx277C
typedef union {
  struct {                                                              ///<
    UINT32                                                    ActRd:8 ; ///<
    UINT32                                                    ActWr:8 ; ///<
    UINT32                                                RasMActRd:8 ; ///<
    UINT32                                                RasMActWr:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx277C_STRUCT;

// **** GMMx2780 Register Definition ****
// Address
#define GMMx2780_ADDRESS                                        0x2780

// Type
#define GMMx2780_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2780_Ras2Ras_OFFSET                                 0
#define GMMx2780_Ras2Ras_WIDTH                                  8
#define GMMx2780_Ras2Ras_MASK                                   0xff
#define GMMx2780_Rp_OFFSET                                      8
#define GMMx2780_Rp_WIDTH                                       8
#define GMMx2780_Rp_MASK                                        0xff00
#define GMMx2780_WrPlusRp_OFFSET                                16
#define GMMx2780_WrPlusRp_WIDTH                                 8
#define GMMx2780_WrPlusRp_MASK                                  0xff0000
#define GMMx2780_BusTurn_OFFSET                                 24
#define GMMx2780_BusTurn_WIDTH                                  8
#define GMMx2780_BusTurn_MASK                                   0xff000000

/// GMMx2780
typedef union {
  struct {                                                              ///<
    UINT32                                                  Ras2Ras:8 ; ///<
    UINT32                                                       Rp:8 ; ///<
    UINT32                                                 WrPlusRp:8 ; ///<
    UINT32                                                  BusTurn:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2780_STRUCT;

// **** GMMx2784 Register Definition ****
// Address
#define GMMx2784_ADDRESS                                        0x2784

// Type
#define GMMx2784_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2784_WtMode_OFFSET                                  0
#define GMMx2784_WtMode_WIDTH                                   2
#define GMMx2784_WtMode_MASK                                    0x3
#define GMMx2784_HarshPri_OFFSET                                2
#define GMMx2784_HarshPri_WIDTH                                 1
#define GMMx2784_HarshPri_MASK                                  0x4
#define GMMx2784_Reserved_31_3_OFFSET                           3
#define GMMx2784_Reserved_31_3_WIDTH                            29
#define GMMx2784_Reserved_31_3_MASK                             0xfffffff8

/// GMMx2784
typedef union {
  struct {                                                              ///<
    UINT32                                                   WtMode:2 ; ///<
    UINT32                                                 HarshPri:1 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2784_STRUCT;

// **** GMMx2788 Register Definition ****
// Address
#define GMMx2788_ADDRESS                                        0x2788

// Type
#define GMMx2788_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2788_WtMode_OFFSET                                  0
#define GMMx2788_WtMode_WIDTH                                   2
#define GMMx2788_WtMode_MASK                                    0x3
#define GMMx2788_HarshPri_OFFSET                                2
#define GMMx2788_HarshPri_WIDTH                                 1
#define GMMx2788_HarshPri_MASK                                  0x4
#define GMMx2788_Reserved_31_3_OFFSET                           3
#define GMMx2788_Reserved_31_3_WIDTH                            29
#define GMMx2788_Reserved_31_3_MASK                             0xfffffff8

/// GMMx2788
typedef union {
  struct {                                                              ///<
    UINT32                                                   WtMode:2 ; ///<
    UINT32                                                 HarshPri:1 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2788_STRUCT;

// **** GMMx279C Register Definition ****
// Address
#define GMMx279C_ADDRESS                                        0x279c

// Type
#define GMMx279C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx279C_Group0_OFFSET                                  0
#define GMMx279C_Group0_WIDTH                                   8
#define GMMx279C_Group0_MASK                                    0xff
#define GMMx279C_Group1_OFFSET                                  8
#define GMMx279C_Group1_WIDTH                                   8
#define GMMx279C_Group1_MASK                                    0xff00
#define GMMx279C_Group2_OFFSET                                  16
#define GMMx279C_Group2_WIDTH                                   8
#define GMMx279C_Group2_MASK                                    0xff0000
#define GMMx279C_Group3_OFFSET                                  24
#define GMMx279C_Group3_WIDTH                                   8
#define GMMx279C_Group3_MASK                                    0xff000000

/// GMMx279C
typedef union {
  struct {                                                              ///<
    UINT32                                                   Group0:8 ; ///<
    UINT32                                                   Group1:8 ; ///<
    UINT32                                                   Group2:8 ; ///<
    UINT32                                                   Group3:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx279C_STRUCT;

// **** GMMx27A0 Register Definition ****
// Address
#define GMMx27A0_ADDRESS                                        0x27a0

// Type
#define GMMx27A0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx27A0_Group0_OFFSET                                  0
#define GMMx27A0_Group0_WIDTH                                   8
#define GMMx27A0_Group0_MASK                                    0xff
#define GMMx27A0_Group1_OFFSET                                  8
#define GMMx27A0_Group1_WIDTH                                   8
#define GMMx27A0_Group1_MASK                                    0xff00
#define GMMx27A0_Group2_OFFSET                                  16
#define GMMx27A0_Group2_WIDTH                                   8
#define GMMx27A0_Group2_MASK                                    0xff0000
#define GMMx27A0_Group3_OFFSET                                  24
#define GMMx27A0_Group3_WIDTH                                   8
#define GMMx27A0_Group3_MASK                                    0xff000000

/// GMMx27A0
typedef union {
  struct {                                                              ///<
    UINT32                                                   Group0:8 ; ///<
    UINT32                                                   Group1:8 ; ///<
    UINT32                                                   Group2:8 ; ///<
    UINT32                                                   Group3:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx27A0_STRUCT;

// **** GMMx27CC Register Definition ****
// Address
#define GMMx27CC_ADDRESS                                        0x27cc

// Type
#define GMMx27CC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx27CC_StreakLimit_OFFSET                             0
#define GMMx27CC_StreakLimit_WIDTH                              8
#define GMMx27CC_StreakLimit_MASK                               0xff
#define GMMx27CC_StreakLimitUber_OFFSET                         8
#define GMMx27CC_StreakLimitUber_WIDTH                          8
#define GMMx27CC_StreakLimitUber_MASK                           0xff00
#define GMMx27CC_StreakBreak_OFFSET                             16
#define GMMx27CC_StreakBreak_WIDTH                              1
#define GMMx27CC_StreakBreak_MASK                               0x10000
#define GMMx27CC_StreakUber_OFFSET                              17
#define GMMx27CC_StreakUber_WIDTH                               1
#define GMMx27CC_StreakUber_MASK                                0x20000
#define GMMx27CC_Reserved_31_18_OFFSET                          18
#define GMMx27CC_Reserved_31_18_WIDTH                           14
#define GMMx27CC_Reserved_31_18_MASK                            0xfffc0000

/// GMMx27CC
typedef union {
  struct {                                                              ///<
    UINT32                                              StreakLimit:8 ; ///<
    UINT32                                          StreakLimitUber:8 ; ///<
    UINT32                                              StreakBreak:1 ; ///<
    UINT32                                               StreakUber:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx27CC_STRUCT;

// **** GMMx27D0 Register Definition ****
// Address
#define GMMx27D0_ADDRESS                                        0x27d0

// Type
#define GMMx27D0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx27D0_StreakLimit_OFFSET                             0
#define GMMx27D0_StreakLimit_WIDTH                              8
#define GMMx27D0_StreakLimit_MASK                               0xff
#define GMMx27D0_StreakLimitUber_OFFSET                         8
#define GMMx27D0_StreakLimitUber_WIDTH                          8
#define GMMx27D0_StreakLimitUber_MASK                           0xff00
#define GMMx27D0_StreakBreak_OFFSET                             16
#define GMMx27D0_StreakBreak_WIDTH                              1
#define GMMx27D0_StreakBreak_MASK                               0x10000
#define GMMx27D0_StreakUber_OFFSET                              17
#define GMMx27D0_StreakUber_WIDTH                               1
#define GMMx27D0_StreakUber_MASK                                0x20000
#define GMMx27D0_Reserved_31_18_OFFSET                          18
#define GMMx27D0_Reserved_31_18_WIDTH                           14
#define GMMx27D0_Reserved_31_18_MASK                            0xfffc0000

/// GMMx27D0
typedef union {
  struct {                                                              ///<
    UINT32                                              StreakLimit:8 ; ///<
    UINT32                                          StreakLimitUber:8 ; ///<
    UINT32                                              StreakBreak:1 ; ///<
    UINT32                                               StreakUber:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx27D0_STRUCT;

// **** GMMx27DC Register Definition ****
// Address
#define GMMx27DC_ADDRESS                                        0x27dc

// Type
#define GMMx27DC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx27DC_Lcl_OFFSET                                     0
#define GMMx27DC_Lcl_WIDTH                                      8
#define GMMx27DC_Lcl_MASK                                       0xff
#define GMMx27DC_Hub_OFFSET                                     8
#define GMMx27DC_Hub_WIDTH                                      8
#define GMMx27DC_Hub_MASK                                       0xff00
#define GMMx27DC_Disp_OFFSET                                    16
#define GMMx27DC_Disp_WIDTH                                     8
#define GMMx27DC_Disp_MASK                                      0xff0000
#define GMMx27DC_Reserved_31_24_OFFSET                          24
#define GMMx27DC_Reserved_31_24_WIDTH                           8
#define GMMx27DC_Reserved_31_24_MASK                            0xff000000

/// GMMx27DC
typedef union {
  struct {                                                              ///<
    UINT32                                                      Lcl:8 ; ///<
    UINT32                                                      Hub:8 ; ///<
    UINT32                                                     Disp:8 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx27DC_STRUCT;

// **** GMMx27E0 Register Definition ****
// Address
#define GMMx27E0_ADDRESS                                        0x27e0

// Type
#define GMMx27E0_TYPE                                           TYPE_GMM
// Field Data
#define GMMx27E0_Lcl_OFFSET                                     0
#define GMMx27E0_Lcl_WIDTH                                      8
#define GMMx27E0_Lcl_MASK                                       0xff
#define GMMx27E0_Hub_OFFSET                                     8
#define GMMx27E0_Hub_WIDTH                                      8
#define GMMx27E0_Hub_MASK                                       0xff00
#define GMMx27E0_Reserved_31_16_OFFSET                          16
#define GMMx27E0_Reserved_31_16_WIDTH                           16
#define GMMx27E0_Reserved_31_16_MASK                            0xffff0000

/// GMMx27E0
typedef union {
  struct {                                                              ///<
    UINT32                                                      Lcl:8 ; ///<
    UINT32                                                      Hub:8 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx27E0_STRUCT;

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
#define GMMx281C_BaseAddr_35_27__OFFSET                         19
#define GMMx281C_BaseAddr_35_27__WIDTH                          9
#define GMMx281C_BaseAddr_35_27__MASK                           0xff80000
#define GMMx281C_Reserved_31_28_OFFSET                          28
#define GMMx281C_Reserved_31_28_WIDTH                           4
#define GMMx281C_Reserved_31_28_MASK                            0xf0000000

/// GMMx281C
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx281C_STRUCT;

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
#define GMMx2824_BaseAddr_35_27__OFFSET                         19
#define GMMx2824_BaseAddr_35_27__WIDTH                          9
#define GMMx2824_BaseAddr_35_27__MASK                           0xff80000
#define GMMx2824_Reserved_31_28_OFFSET                          28
#define GMMx2824_Reserved_31_28_WIDTH                           4
#define GMMx2824_Reserved_31_28_MASK                            0xf0000000

/// GMMx2824
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2824_STRUCT;

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
#define GMMx282C_BaseAddr_35_27__OFFSET                         19
#define GMMx282C_BaseAddr_35_27__WIDTH                          9
#define GMMx282C_BaseAddr_35_27__MASK                           0xff80000
#define GMMx282C_Reserved_31_28_OFFSET                          28
#define GMMx282C_Reserved_31_28_WIDTH                           4
#define GMMx282C_Reserved_31_28_MASK                            0xf0000000

/// GMMx282C
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx282C_STRUCT;

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
#define GMMx2834_BaseAddr_35_27__OFFSET                         19
#define GMMx2834_BaseAddr_35_27__WIDTH                          9
#define GMMx2834_BaseAddr_35_27__MASK                           0xff80000
#define GMMx2834_Reserved_31_28_OFFSET                          28
#define GMMx2834_Reserved_31_28_WIDTH                           4
#define GMMx2834_Reserved_31_28_MASK                            0xf0000000

/// GMMx2834
typedef union {
  struct {                                                              ///<
    UINT32                                                 CSEnable:1 ; ///<
    UINT32                                             Reserved_4_1:4 ; ///<
    UINT32                                          BaseAddr_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          BaseAddr_35_27_:9 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2834_STRUCT;

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
#define GMMx283C_AddrMask_35_27__OFFSET                         19
#define GMMx283C_AddrMask_35_27__WIDTH                          9
#define GMMx283C_AddrMask_35_27__MASK                           0xff80000
#define GMMx283C_Reserved_28_28_OFFSET                          28
#define GMMx283C_Reserved_28_28_WIDTH                           1
#define GMMx283C_Reserved_28_28_MASK                            0x10000000
#define GMMx283C_Reserved_31_29_OFFSET                          29
#define GMMx283C_Reserved_31_29_WIDTH                           3
#define GMMx283C_Reserved_31_29_MASK                            0xe0000000

/// GMMx283C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
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
#define GMMx2840_AddrMask_35_27__OFFSET                         19
#define GMMx2840_AddrMask_35_27__WIDTH                          9
#define GMMx2840_AddrMask_35_27__MASK                           0xff80000
#define GMMx2840_Reserved_28_28_OFFSET                          28
#define GMMx2840_Reserved_28_28_WIDTH                           1
#define GMMx2840_Reserved_28_28_MASK                            0x10000000
#define GMMx2840_Reserved_31_29_OFFSET                          29
#define GMMx2840_Reserved_31_29_WIDTH                           3
#define GMMx2840_Reserved_31_29_MASK                            0xe0000000

/// GMMx2840
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                          AddrMask_21_13_:9 ; ///<
    UINT32                                           Reserved_18_14:5 ; ///<
    UINT32                                          AddrMask_35_27_:9 ; ///<
    UINT32                                           Reserved_28_28:1 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2840_STRUCT;

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
#define GMMx284C_Reserved_18_17_OFFSET                          17
#define GMMx284C_Reserved_18_17_WIDTH                           2
#define GMMx284C_Reserved_18_17_MASK                            0x60000
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
    UINT32                                           Reserved_18_17:2 ; ///<
    UINT32                                                 BankSwap:1 ; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx284C_STRUCT;

// **** GMMx2858 Register Definition ****
// Address
#define GMMx2858_ADDRESS                                        0x2858

// Type
#define GMMx2858_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2858_Reserved_8_0_OFFSET                            0
#define GMMx2858_Reserved_8_0_WIDTH                             9
#define GMMx2858_Reserved_8_0_MASK                              0x1ff
#define GMMx2858_DctSelBankSwap_OFFSET                          9
#define GMMx2858_DctSelBankSwap_WIDTH                           1
#define GMMx2858_DctSelBankSwap_MASK                            0x200
#define GMMx2858_Reserved_31_10_OFFSET                          10
#define GMMx2858_Reserved_31_10_WIDTH                           22
#define GMMx2858_Reserved_31_10_MASK                            0xfffffc00

/// GMMx2858
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                           DctSelBankSwap:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
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

// **** GMMx2864 Register Definition ****
// Address
#define GMMx2864_ADDRESS                                        0x2864

// Type
#define GMMx2864_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2864_A8Map_OFFSET                                   0
#define GMMx2864_A8Map_WIDTH                                    4
#define GMMx2864_A8Map_MASK                                     0xf
#define GMMx2864_A9Map_OFFSET                                   4
#define GMMx2864_A9Map_WIDTH                                    4
#define GMMx2864_A9Map_MASK                                     0xf0
#define GMMx2864_A10Map_OFFSET                                  8
#define GMMx2864_A10Map_WIDTH                                   4
#define GMMx2864_A10Map_MASK                                    0xf00
#define GMMx2864_A11Map_OFFSET                                  12
#define GMMx2864_A11Map_WIDTH                                   4
#define GMMx2864_A11Map_MASK                                    0xf000
#define GMMx2864_A12Map_OFFSET                                  16
#define GMMx2864_A12Map_WIDTH                                   4
#define GMMx2864_A12Map_MASK                                    0xf0000
#define GMMx2864_A13Map_OFFSET                                  20
#define GMMx2864_A13Map_WIDTH                                   4
#define GMMx2864_A13Map_MASK                                    0xf00000
#define GMMx2864_A14Map_OFFSET                                  24
#define GMMx2864_A14Map_WIDTH                                   4
#define GMMx2864_A14Map_MASK                                    0xf000000
#define GMMx2864_A15Map_OFFSET                                  28
#define GMMx2864_A15Map_WIDTH                                   4
#define GMMx2864_A15Map_MASK                                    0xf0000000

/// GMMx2864
typedef union {
  struct {                                                              ///<
    UINT32                                                    A8Map:4 ; ///<
    UINT32                                                    A9Map:4 ; ///<
    UINT32                                                   A10Map:4 ; ///<
    UINT32                                                   A11Map:4 ; ///<
    UINT32                                                   A12Map:4 ; ///<
    UINT32                                                   A13Map:4 ; ///<
    UINT32                                                   A14Map:4 ; ///<
    UINT32                                                   A15Map:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2864_STRUCT;

// **** GMMx286C Register Definition ****
// Address
#define GMMx286C_ADDRESS                                        0x286c

// Type
#define GMMx286C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx286C_Base_OFFSET                                    0
#define GMMx286C_Base_WIDTH                                     20
#define GMMx286C_Base_MASK                                      0xfffff
#define GMMx286C_Reserved_31_20_OFFSET                          20
#define GMMx286C_Reserved_31_20_WIDTH                           12
#define GMMx286C_Reserved_31_20_MASK                            0xfff00000

/// GMMx286C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Base:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx286C_STRUCT;

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

// **** GMMx2878 Register Definition ****
// Address
#define GMMx2878_ADDRESS                                        0x2878

// Type
#define GMMx2878_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2878_Base_OFFSET                                    0
#define GMMx2878_Base_WIDTH                                     20
#define GMMx2878_Base_MASK                                      0xfffff
#define GMMx2878_Reserved_31_20_OFFSET                          20
#define GMMx2878_Reserved_31_20_WIDTH                           12
#define GMMx2878_Reserved_31_20_MASK                            0xfff00000

/// GMMx2878
typedef union {
  struct {                                                              ///<
    UINT32                                                     Base:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2878_STRUCT;

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

// **** GMMx2880 Register Definition ****
// Address
#define GMMx2880_ADDRESS                                        0x2880

// Type
#define GMMx2880_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2880_Top_OFFSET                                     0
#define GMMx2880_Top_WIDTH                                      20
#define GMMx2880_Top_MASK                                       0xfffff
#define GMMx2880_Reserved_31_20_OFFSET                          20
#define GMMx2880_Reserved_31_20_WIDTH                           12
#define GMMx2880_Reserved_31_20_MASK                            0xfff00000

/// GMMx2880
typedef union {
  struct {                                                              ///<
    UINT32                                                      Top:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2880_STRUCT;

// **** GMMx2884 Register Definition ****
// Address
#define GMMx2884_ADDRESS                                        0x2884

// Type
#define GMMx2884_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2884_Top_OFFSET                                     0
#define GMMx2884_Top_WIDTH                                      20
#define GMMx2884_Top_MASK                                       0xfffff
#define GMMx2884_Reserved_31_20_OFFSET                          20
#define GMMx2884_Reserved_31_20_WIDTH                           12
#define GMMx2884_Reserved_31_20_MASK                            0xfff00000

/// GMMx2884
typedef union {
  struct {                                                              ///<
    UINT32                                                      Top:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2884_STRUCT;

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

// **** GMMx288C Register Definition ****
// Address
#define GMMx288C_ADDRESS                                        0x288c

// Type
#define GMMx288C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx288C_Base_OFFSET                                    0
#define GMMx288C_Base_WIDTH                                     20
#define GMMx288C_Base_MASK                                      0xfffff
#define GMMx288C_Reserved_31_20_OFFSET                          20
#define GMMx288C_Reserved_31_20_WIDTH                           12
#define GMMx288C_Reserved_31_20_MASK                            0xfff00000

/// GMMx288C
typedef union {
  struct {                                                              ///<
    UINT32                                                     Base:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx288C_STRUCT;

// **** GMMx2890 Register Definition ****
// Address
#define GMMx2890_ADDRESS                                        0x2890

// Type
#define GMMx2890_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2890_Top_OFFSET                                     0
#define GMMx2890_Top_WIDTH                                      20
#define GMMx2890_Top_MASK                                       0xfffff
#define GMMx2890_Reserved_31_20_OFFSET                          20
#define GMMx2890_Reserved_31_20_WIDTH                           12
#define GMMx2890_Reserved_31_20_MASK                            0xfff00000

/// GMMx2890
typedef union {
  struct {                                                              ///<
    UINT32                                                      Top:20; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2890_STRUCT;

// **** GMMx2894 Register Definition ****
// Address
#define GMMx2894_ADDRESS                                        0x2894

// Type
#define GMMx2894_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2894_Def_OFFSET                                     0
#define GMMx2894_Def_WIDTH                                      28
#define GMMx2894_Def_MASK                                       0xfffffff
#define GMMx2894_Reserved_31_28_OFFSET                          28
#define GMMx2894_Reserved_31_28_WIDTH                           4
#define GMMx2894_Reserved_31_28_MASK                            0xf0000000

/// GMMx2894
typedef union {
  struct {                                                              ///<
    UINT32                                                      Def:28; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2894_STRUCT;

// **** GMMx2898 Register Definition ****
// Address
#define GMMx2898_ADDRESS                                        0x2898

// Type
#define GMMx2898_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2898_Offset_OFFSET                                  0
#define GMMx2898_Offset_WIDTH                                   20
#define GMMx2898_Offset_MASK                                    0xfffff
#define GMMx2898_Base_OFFSET                                    20
#define GMMx2898_Base_WIDTH                                     4
#define GMMx2898_Base_MASK                                      0xf00000
#define GMMx2898_Top_OFFSET                                     24
#define GMMx2898_Top_WIDTH                                      4
#define GMMx2898_Top_MASK                                       0xf000000
#define GMMx2898_Reserved_31_28_OFFSET                          28
#define GMMx2898_Reserved_31_28_WIDTH                           4
#define GMMx2898_Reserved_31_28_MASK                            0xf0000000

/// GMMx2898
typedef union {
  struct {                                                              ///<
    UINT32                                                   Offset:20; ///<
    UINT32                                                     Base:4 ; ///<
    UINT32                                                      Top:4 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2898_STRUCT;

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

// **** GMMx28DC Register Definition ****
// Address
#define GMMx28DC_ADDRESS                                        0x28dc

// Type
#define GMMx28DC_TYPE                                           TYPE_GMM
// Field Data
#define GMMx28DC_Ras2Ras_OFFSET                                 0
#define GMMx28DC_Ras2Ras_WIDTH                                  8
#define GMMx28DC_Ras2Ras_MASK                                   0xff
#define GMMx28DC_Rp_OFFSET                                      8
#define GMMx28DC_Rp_WIDTH                                       8
#define GMMx28DC_Rp_MASK                                        0xff00
#define GMMx28DC_WrPlusRp_OFFSET                                16
#define GMMx28DC_WrPlusRp_WIDTH                                 8
#define GMMx28DC_WrPlusRp_MASK                                  0xff0000
#define GMMx28DC_BusTurn_OFFSET                                 24
#define GMMx28DC_BusTurn_WIDTH                                  8
#define GMMx28DC_BusTurn_MASK                                   0xff000000

/// GMMx28DC
typedef union {
  struct {                                                              ///<
    UINT32                                                  Ras2Ras:8 ; ///<
    UINT32                                                       Rp:8 ; ///<
    UINT32                                                 WrPlusRp:8 ; ///<
    UINT32                                                  BusTurn:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx28DC_STRUCT;

// **** GMMx2B8C Register Definition ****
// Address
#define GMMx2B8C_ADDRESS                                        0x2b8c

// Type
#define GMMx2B8C_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2B8C_RengRamIndex_OFFSET                            0
#define GMMx2B8C_RengRamIndex_WIDTH                             10
#define GMMx2B8C_RengRamIndex_MASK                              0x3ff
#define GMMx2B8C_Reserved_31_10_OFFSET                          10
#define GMMx2B8C_Reserved_31_10_WIDTH                           22
#define GMMx2B8C_Reserved_31_10_MASK                            0xfffffc00

/// GMMx2B8C
typedef union {
  struct {                                                              ///<
    UINT32                                             RengRamIndex:10; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2B8C_STRUCT;

// **** GMMx2B90 Register Definition ****
// Address
#define GMMx2B90_ADDRESS                                        0x2b90

// Type
#define GMMx2B90_TYPE                                           TYPE_GMM
// Field Data
#define GMMx2B90_RengRamData_OFFSET                             0
#define GMMx2B90_RengRamData_WIDTH                              32
#define GMMx2B90_RengRamData_MASK                               0xffffffff

/// GMMx2B90
typedef union {
  struct {                                                              ///<
    UINT32                                              RengRamData:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GMMx2B90_STRUCT;

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

// **** SMUx03 Register Definition ****
// Address
#define SMUx03_ADDRESS                                          0x3

// Type
#define SMUx03_TYPE                                             TYPE_SMU
// Field Data
#define SMUx03_IntReq_OFFSET                                    0
#define SMUx03_IntReq_WIDTH                                     1
#define SMUx03_IntReq_MASK                                      0x1
#define SMUx03_IntAck_OFFSET                                    1
#define SMUx03_IntAck_WIDTH                                     1
#define SMUx03_IntAck_MASK                                      0x2
#define SMUx03_IntDone_OFFSET                                   2
#define SMUx03_IntDone_WIDTH                                    1
#define SMUx03_IntDone_MASK                                     0x4
#define SMUx03_ServiceIndex_OFFSET                              3
#define SMUx03_ServiceIndex_WIDTH                               8
#define SMUx03_ServiceIndex_MASK                                0x7f8
#define SMUx03_Reserved_31_11_OFFSET                            11
#define SMUx03_Reserved_31_11_WIDTH                             21
#define SMUx03_Reserved_31_11_MASK                              0xfffff800

/// SMUx03
typedef union {
  struct {                                                              ///<
    UINT32                                                   IntReq:1 ; ///<
    UINT32                                                   IntAck:1 ; ///<
    UINT32                                                  IntDone:1 ; ///<
    UINT32                                             ServiceIndex:8 ; ///<
    UINT32                                           Reserved_31_11:21; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx03_STRUCT;

// **** SMUx05 Register Definition ****
// Address
#define SMUx05_ADDRESS                                          0x5

// Type
#define SMUx05_TYPE                                             TYPE_SMU
// Field Data
#define SMUx05_McuRam_OFFSET                                    0
#define SMUx05_McuRam_WIDTH                                     32
#define SMUx05_McuRam_MASK                                      0xffffffff

/// SMUx05
typedef union {
  struct {                                                              ///<
    UINT32                                                   McuRam:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx05_STRUCT;

// **** SMUx0B Register Definition ****
// Address
#define SMUx0B_ADDRESS                                          0xb

// Type
#define SMUx0B_TYPE                                             TYPE_SMU
// Field Data
#define SMUx0B_MemAddr_OFFSET                                   0
#define SMUx0B_MemAddr_WIDTH                                    16
#define SMUx0B_MemAddr_MASK                                     0xffff

/// SMUx0B
typedef union {
  struct {                                                              ///<
    UINT32                                                  MemAddr:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_STRUCT;

// **** MSRC001_001A Register Definition ****
// Address
#define MSRC001_001A_ADDRESS                                    0xc001001a

// Type
#define MSRC001_001A_TYPE                                       TYPE_MSR
// Field Data
#define MSRC001_001A_RAZ_22_0_OFFSET                            0
#define MSRC001_001A_RAZ_22_0_WIDTH                             23
#define MSRC001_001A_RAZ_22_0_MASK                              0x7fffff
#define MSRC001_001A_TOM_35_23__OFFSET                          23
#define MSRC001_001A_TOM_35_23__WIDTH                           13
#define MSRC001_001A_TOM_35_23__MASK                            0xfff800000
#define MSRC001_001A_RAZ_63_36_OFFSET                           36
#define MSRC001_001A_RAZ_63_36_WIDTH                            28
#define MSRC001_001A_RAZ_63_36_MASK                             0xfffffff000000000

/// MSRC001_001A
typedef union {
  struct {                                                              ///<
    UINT64                                                 RAZ_22_0:23; ///<
    UINT64                                               TOM_35_23_:13; ///<
    UINT64                                                RAZ_63_36:28; ///<
  } Field;                                                              ///<
  UINT64 Value;                                                         ///<
} MSRC001_001A_STRUCT;


// **** FCRxFF30_0AE6(GMMx2B98) Register Definition ****
// Address
#define FCRxFF30_0AE6_ADDRESS                                   0xff300AE6

// Field Data
#define FCRxFF30_0AE6_RengExecuteNonsecureStartPtr_OFFSET            0
#define FCRxFF30_0AE6_RengExecuteNonsecureStartPtr_WIDTH             10
#define FCRxFF30_0AE6_RengExecuteNowMode_OFFSET                      10
#define FCRxFF30_0AE6_RengExecuteNowMode_WIDTH                       1
#define FCRxFF30_0AE6_RengExecuteOnRegUpdate_OFFSET                  11
#define FCRxFF30_0AE6_RengExecuteOnRegUpdate_WIDTH                   1
#define FCRxFF30_0AE6_RengSrbmCreditsMcd_OFFSET                      12
#define FCRxFF30_0AE6_RengSrbmCreditsMcd_WIDTH                       4
#define FCRxFF30_0AE6_StctrlStutterEn_OFFSET                         16
#define FCRxFF30_0AE6_StctrlStutterEn_WIDTH                          1
#define FCRxFF30_0AE6_StctrlGmcIdleThreshold_OFFSET                  17
#define FCRxFF30_0AE6_StctrlGmcIdleThreshold_WIDTH                   2
#define FCRxFF30_0AE6_StctrlSrbmIdleThreshold_OFFSET                 19
#define FCRxFF30_0AE6_StctrlSrbmIdleThreshold_WIDTH                  2
#define FCRxFF30_0AE6_StctrlIgnorePreSr_OFFSET                       21
#define FCRxFF30_0AE6_StctrlIgnorePreSr_WIDTH                        1
#define FCRxFF30_0AE6_StctrlIgnoreAllowStop_OFFSET                   22
#define FCRxFF30_0AE6_StctrlIgnoreAllowStop_WIDTH                    1
#define FCRxFF30_0AE6_StctrlIgnoreDramOffline_OFFSET                 23
#define FCRxFF30_0AE6_StctrlIgnoreDramOffline_WIDTH                  1
#define FCRxFF30_0AE6_StctrlIgnoreProtectionFault_OFFSET             24
#define FCRxFF30_0AE6_StctrlIgnoreProtectionFault_WIDTH              1
#define FCRxFF30_0AE6_StctrlDisableAllowSr_OFFSET                    25
#define FCRxFF30_0AE6_StctrlDisableAllowSr_WIDTH                     1
#define FCRxFF30_0AE6_StctrlDisableGmcOffline_OFFSET                 26
#define FCRxFF30_0AE6_StctrlDisableGmcOffline_WIDTH                  1
#define FCRxFF30_0AE6_CriticalRegsLock_OFFSET                        27
#define FCRxFF30_0AE6_CriticalRegsLock_WIDTH                         1
#define FCRxFF30_0AE6_SmuExecuteOnRegUpdate_OFFSET                   28
#define FCRxFF30_0AE6_SmuExecuteOnRegUpdate_WIDTH                    1
#define FCRxFF30_0AE6_AllowDeepSleepMode_OFFSET                      29
#define FCRxFF30_0AE6_AllowDeepSleepMode_WIDTH                       2
#define FCRxFF30_0AE6_Reserved_31_31_OFFSET                          31
#define FCRxFF30_0AE6_Reserved_31_31_WIDTH                           1

/// FCRxFF30_0AE6
typedef union {
  struct {                                                              ///<
    UINT32                             RengExecuteNonsecureStartPtr:10; ///<
    UINT32                                       RengExecuteNowMode:1 ; ///<
    UINT32                                   RengExecuteOnRegUpdate:1 ; ///<
    UINT32                                       RengSrbmCreditsMcd:4 ; ///<
    UINT32                                          StctrlStutterEn:1 ; ///<
    UINT32                                   StctrlGmcIdleThreshold:2 ; ///<
    UINT32                                  StctrlSrbmIdleThreshold:2 ; ///<
    UINT32                                        StctrlIgnorePreSr:1 ; ///<
    UINT32                                    StctrlIgnoreAllowStop:1 ; ///<
    UINT32                                  StctrlIgnoreDramOffline:1 ; ///<
    UINT32                              StctrlIgnoreProtectionFault:1 ; ///<
    UINT32                                     StctrlDisableAllowSr:1 ; ///<
    UINT32                                  StctrlDisableGmcOffline:1 ; ///<
    UINT32                                         CriticalRegsLock:1 ; ///<
    UINT32                                    SmuExecuteOnRegUpdate:1 ; ///<
    UINT32                                       AllowDeepSleepMode:2 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;
  UINT32 Value;
} FCRxFF30_0AE6_STRUCT;

// **** FCRxFF30_0134(GMMx4D0) Register Definition ****
// Address
#define FCRxFF30_0134_ADDRESS                                   0xff300134

// Field Data
#define FCRxFF30_0134_DispclkDccgGateDisable_OFFSET                   0
#define FCRxFF30_0134_DispclkDccgGateDisable_WIDTH                    1
#define FCRxFF30_0134_DispclkDccgGateDisable_MASK                     0x1
#define FCRxFF30_0134_DispclkRDccgGateDisable_OFFSET                  1
#define FCRxFF30_0134_DispclkRDccgGateDisable_WIDTH                   1
#define FCRxFF30_0134_DispclkRDccgGateDisable_MASK                    0x2
#define FCRxFF30_0134_SclkGateDisable_OFFSET                          2
#define FCRxFF30_0134_SclkGateDisable_WIDTH                           1
#define FCRxFF30_0134_SclkGateDisable_MASK                            0x4
#define FCRxFF30_0134_Reserved_7_3_OFFSET                             3
#define FCRxFF30_0134_Reserved_7_3_WIDTH                              5
#define FCRxFF30_0134_Reserved_7_3_MASK                               0xf8
#define FCRxFF30_0134_SymclkaGateDisable_OFFSET                       8
#define FCRxFF30_0134_SymclkaGateDisable_WIDTH                        1
#define FCRxFF30_0134_SymclkaGateDisable_MASK                         0x100
#define FCRxFF30_0134_SymclkbGateDisable_OFFSET                       9
#define FCRxFF30_0134_SymclkbGateDisable_WIDTH                        1
#define FCRxFF30_0134_SymclkbGateDisable_MASK                         0x200
#define FCRxFF30_0134_Reserved_31_10_OFFSET                           10
#define FCRxFF30_0134_Reserved_31_10_WIDTH                            22
#define FCRxFF30_0134_Reserved_31_10_MASK                             0xfffffc00

/// FCRxFF30_0134
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
} FCRxFF30_0134_STRUCT;

// **** FCRxFF30_1B7C(GMMx6DF0) Register Definition ****
// Address
#define FCRxFF30_1B7C_ADDRESS                                   0xff301B7C

// Field Data
#define FCRxFF30_1B7C_Reserved_3_0_OFFSET                            0
#define FCRxFF30_1B7C_Reserved_3_0_WIDTH                             4
#define FCRxFF30_1B7C_Reserved_3_0_MASK                              0xf
#define FCRxFF30_1B7C_CrtcDispclkRDcfeGateDisable_OFFSET             4
#define FCRxFF30_1B7C_CrtcDispclkRDcfeGateDisable_WIDTH              1
#define FCRxFF30_1B7C_CrtcDispclkRDcfeGateDisable_MASK               0x10
#define FCRxFF30_1B7C_Reserved_7_5_OFFSET                            5
#define FCRxFF30_1B7C_Reserved_7_5_WIDTH                             3
#define FCRxFF30_1B7C_Reserved_7_5_MASK                              0xe0
#define FCRxFF30_1B7C_CrtcDispclkGDcpGateDisable_OFFSET              8
#define FCRxFF30_1B7C_CrtcDispclkGDcpGateDisable_WIDTH               1
#define FCRxFF30_1B7C_CrtcDispclkGDcpGateDisable_MASK                0x100
#define FCRxFF30_1B7C_Reserved_11_9_OFFSET                           9
#define FCRxFF30_1B7C_Reserved_11_9_WIDTH                            3
#define FCRxFF30_1B7C_Reserved_11_9_MASK                             0xe00
#define FCRxFF30_1B7C_CrtcDispclkGSclGateDisable_OFFSET              12
#define FCRxFF30_1B7C_CrtcDispclkGSclGateDisable_WIDTH               1
#define FCRxFF30_1B7C_CrtcDispclkGSclGateDisable_MASK                0x1000
#define FCRxFF30_1B7C_Reserved_31_13_OFFSET                          13
#define FCRxFF30_1B7C_Reserved_31_13_WIDTH                           19
#define FCRxFF30_1B7C_Reserved_31_13_MASK                            0xffffe000

/// FCRxFF30_1B7C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                              CrtcDispclkRDcfeGateDisable:1 ; ///<
    UINT32                                             Reserved_7_5:3 ; ///<
    UINT32                               CrtcDispclkGDcpGateDisable:1 ; ///<
    UINT32                                            Reserved_11_9:3 ; ///<
    UINT32                               CrtcDispclkGSclGateDisable:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_1B7C_STRUCT;

// **** FCRxFF30_1E7C(GMMx79F0) Register Definition ****
// Address
#define FCRxFF30_1E7C_ADDRESS                                   0xff301E7C

// Field Data
#define FCRxFF30_1E7C_Reserved_3_0_OFFSET                            0
#define FCRxFF30_1E7C_Reserved_3_0_WIDTH                             4
#define FCRxFF30_1E7C_Reserved_3_0_MASK                              0xf
#define FCRxFF30_1E7C_CrtcDispclkRDcfeGateDisable_OFFSET             4
#define FCRxFF30_1E7C_CrtcDispclkRDcfeGateDisable_WIDTH              1
#define FCRxFF30_1E7C_CrtcDispclkRDcfeGateDisable_MASK               0x10
#define FCRxFF30_1E7C_Reserved_7_5_OFFSET                            5
#define FCRxFF30_1E7C_Reserved_7_5_WIDTH                             3
#define FCRxFF30_1E7C_Reserved_7_5_MASK                              0xe0
#define FCRxFF30_1E7C_CrtcDispclkGDcpGateDisable_OFFSET              8
#define FCRxFF30_1E7C_CrtcDispclkGDcpGateDisable_WIDTH               1
#define FCRxFF30_1E7C_CrtcDispclkGDcpGateDisable_MASK                0x100
#define FCRxFF30_1E7C_Reserved_11_9_OFFSET                           9
#define FCRxFF30_1E7C_Reserved_11_9_WIDTH                            3
#define FCRxFF30_1E7C_Reserved_11_9_MASK                             0xe00
#define FCRxFF30_1E7C_CrtcDispclkGSclGateDisable_OFFSET              12
#define FCRxFF30_1E7C_CrtcDispclkGSclGateDisable_WIDTH               1
#define FCRxFF30_1E7C_CrtcDispclkGSclGateDisable_MASK                0x1000
#define FCRxFF30_1E7C_Reserved_31_13_OFFSET                          13
#define FCRxFF30_1E7C_Reserved_31_13_WIDTH                           19
#define FCRxFF30_1E7C_Reserved_31_13_MASK                            0xffffe000

/// FCRxFF30_1E7C
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                              CrtcDispclkRDcfeGateDisable:1 ; ///<
    UINT32                                             Reserved_7_5:3 ; ///<
    UINT32                               CrtcDispclkGDcpGateDisable:1 ; ///<
    UINT32                                            Reserved_11_9:3 ; ///<
    UINT32                               CrtcDispclkGSclGateDisable:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_1E7C_STRUCT;

// **** FCRxFE00_600E Register Definition ****
// Address
#define FCRxFE00_600E_ADDRESS                                   0xfe00600e

// Field Data
#define FCRxFE00_600E_MainPllOpFreqIdStartup_OFFSET             0
#define FCRxFE00_600E_MainPllOpFreqIdStartup_WIDTH              6
#define FCRxFE00_600E_WrCkDid_OFFSET                            10
#define FCRxFE00_600E_WrCkDid_WIDTH                             5

/// FCRxFE00_600E
typedef union {
  struct {
    UINT32                                   MainPllOpFreqIdStartup:6 ; ///<
    UINT32                                                 Reserved:5 ; ///<
    UINT32                                                  WrCkDid:5 ; ///<
  } Field;
  UINT32 Value;
} FCRxFE00_600E_STRUCT;

// **** SMUx0B_x8498 Register Definition ****
// Address
#define SMUx0B_x8498_ADDRESS                                    0x8498

// Field Data
#define SMUx0B_x8498_ConditionalBF_1_0_OFFSET                   0
#define SMUx0B_x8498_ConditionalBF_1_0_WIDTH                    2
#define SMUx0B_x8498_ConditionalBF_1_0_MASK                     0x3
#define SMUx0B_x8498_ConditionalBF_3_2_OFFSET                   2
#define SMUx0B_x8498_ConditionalBF_3_2_WIDTH                    2
#define SMUx0B_x8498_ConditionalBF_3_2_MASK                     0xc
#define SMUx0B_x8498_Reserved_7_4_OFFSET                        4
#define SMUx0B_x8498_Reserved_7_4_WIDTH                         4
#define SMUx0B_x8498_Reserved_7_4_MASK                          0xf0
#define SMUx0B_x8498_ConditionalBF_9_8_OFFSET                   8
#define SMUx0B_x8498_ConditionalBF_9_8_WIDTH                    2
#define SMUx0B_x8498_ConditionalBF_9_8_MASK                     0x300
#define SMUx0B_x8498_ConditionalBF_11_10_OFFSET                 10
#define SMUx0B_x8498_ConditionalBF_11_10_WIDTH                  2
#define SMUx0B_x8498_ConditionalBF_11_10_MASK                   0xc00
#define SMUx0B_x8498_Reserved_15_12_OFFSET                      12
#define SMUx0B_x8498_Reserved_15_12_WIDTH                       4
#define SMUx0B_x8498_Reserved_15_12_MASK                        0xf000
#define SMUx0B_x8498_BaseVid_5_OFFSET                           16
#define SMUx0B_x8498_BaseVid_5_WIDTH                            2
#define SMUx0B_x8498_BaseVid_5_MASK                             0x30000
#define SMUx0B_x8498_TolExcdVid_5_OFFSET                        18
#define SMUx0B_x8498_TolExcdVid_5_WIDTH                         2
#define SMUx0B_x8498_TolExcdVid_5_MASK                          0xc0000
#define SMUx0B_x8498_Reserved_23_20_OFFSET                      20
#define SMUx0B_x8498_Reserved_23_20_WIDTH                       4
#define SMUx0B_x8498_Reserved_23_20_MASK                        0xf00000
#define SMUx0B_x8498_BaseVid_4_OFFSET                           24
#define SMUx0B_x8498_BaseVid_4_WIDTH                            2
#define SMUx0B_x8498_BaseVid_4_MASK                             0x3000000
#define SMUx0B_x8498_TolExcdVid_4_OFFSET                        26
#define SMUx0B_x8498_TolExcdVid_4_WIDTH                         2
#define SMUx0B_x8498_TolExcdVid_4_MASK                          0xc000000
#define SMUx0B_x8498_Reserved_31_28_OFFSET                      28
#define SMUx0B_x8498_Reserved_31_28_WIDTH                       4
#define SMUx0B_x8498_Reserved_31_28_MASK                        0xf0000000

/// SMUx0B_x8498
typedef union {
  struct {                                                              ///<
    UINT32                                        ConditionalBF_1_0:2 ; ///<
    UINT32                                        ConditionalBF_3_2:2 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                        ConditionalBF_9_8:2 ; ///<
    UINT32                                      ConditionalBF_11_10:2 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                                BaseVid_5:2 ; ///<
    UINT32                                             TolExcdVid_5:2 ; ///<
    UINT32                                           Reserved_23_20:4 ; ///<
    UINT32                                                BaseVid_4:2 ; ///<
    UINT32                                             TolExcdVid_4:2 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8498_STRUCT;

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

// **** SMUx0B_x85B0 Register Definition ****
// Address
#define SMUx0B_x85B0_ADDRESS                                    0x85B0


// **** SMUx0B_x85D0 Register Definition ****
// Address
#define SMUx0B_x85D0_ADDRESS                                    0x85D0

// **** D0F0x64_x51 Register Definition ****
// Address
#define D0F0x64_x51_ADDRESS                                     0x51

// Type
#define D0F0x64_x51_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x51_Reserved_2_0_OFFSET                         0
#define D0F0x64_x51_Reserved_2_0_WIDTH                          3
#define D0F0x64_x51_Reserved_2_0_MASK                           0x7
#define D0F0x64_x51_P2pDis_OFFSET                               3
#define D0F0x64_x51_P2pDis_WIDTH                                1
#define D0F0x64_x51_P2pDis_MASK                                 0x8
#define D0F0x64_x51_Reserved_15_4_OFFSET                        4
#define D0F0x64_x51_Reserved_15_4_WIDTH                         12
#define D0F0x64_x51_Reserved_15_4_MASK                          0xfff0
#define D0F0x64_x51_ExtDevPlug_OFFSET                           16
#define D0F0x64_x51_ExtDevPlug_WIDTH                            1
#define D0F0x64_x51_ExtDevPlug_MASK                             0x10000
#define D0F0x64_x51_ExtDevCrsEn_OFFSET                          17
#define D0F0x64_x51_ExtDevCrsEn_WIDTH                           1
#define D0F0x64_x51_ExtDevCrsEn_MASK                            0x20000
#define D0F0x64_x51_CrsEn_OFFSET                                18
#define D0F0x64_x51_CrsEn_WIDTH                                 1
#define D0F0x64_x51_CrsEn_MASK                                  0x40000
#define D0F0x64_x51_IntSelMode_OFFSET                           19
#define D0F0x64_x51_IntSelMode_WIDTH                            1
#define D0F0x64_x51_IntSelMode_MASK                             0x80000
#define D0F0x64_x51_SetPowEn_OFFSET                             20
#define D0F0x64_x51_SetPowEn_WIDTH                              1
#define D0F0x64_x51_SetPowEn_MASK                               0x100000
#define D0F0x64_x51_Reserved_31_21_OFFSET                       21
#define D0F0x64_x51_Reserved_31_21_WIDTH                        11
#define D0F0x64_x51_Reserved_31_21_MASK                         0xffe00000

/// D0F0x64_x51
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                                   P2pDis:1 ; ///<
    UINT32                                            Reserved_15_4:12; ///<
    UINT32                                               ExtDevPlug:1 ; ///<
    UINT32                                              ExtDevCrsEn:1 ; ///<
    UINT32                                                    CrsEn:1 ; ///<
    UINT32                                               IntSelMode:1 ; ///<
    UINT32                                                 SetPowEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x51_STRUCT;

// **** SMUx33 Register Definition ****
// Address
#define SMUx33_ADDRESS                                          0x33

// Type
#define SMUx33_TYPE                                             TYPE_SMU
// Field Data
#define SMUx33_LclkActMonPrd_OFFSET                             0
#define SMUx33_LclkActMonPrd_WIDTH                              16
#define SMUx33_LclkActMonPrd_MASK                               0xffff
#define SMUx33_LclkActMonUnt_OFFSET                             16
#define SMUx33_LclkActMonUnt_WIDTH                              4
#define SMUx33_LclkActMonUnt_MASK                               0xf0000
#define SMUx33_Reserved_22_20_OFFSET                            20
#define SMUx33_Reserved_22_20_WIDTH                             3
#define SMUx33_Reserved_22_20_MASK                              0x700000
#define SMUx33_BusyCntSel_OFFSET                                23
#define SMUx33_BusyCntSel_WIDTH                                 2
#define SMUx33_BusyCntSel_MASK                                  0x1800000
#define SMUx33_Reserved_31_25_OFFSET                            25
#define SMUx33_Reserved_31_25_WIDTH                             7
#define SMUx33_Reserved_31_25_MASK                              0xfe000000

/// SMUx33
typedef union {
  struct {                                                              ///<
    UINT32                                            LclkActMonPrd:16; ///<
    UINT32                                            LclkActMonUnt:4 ; ///<
    UINT32                                           Reserved_22_20:3 ; ///<
    UINT32                                               BusyCntSel:2 ; ///<
    UINT32                                           Reserved_31_25:7 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx33_STRUCT;

// **** SMUx0B_x8434 Register Definition ****
// Address
#define SMUx0B_x8434_ADDRESS                                    0x8434

// Type
#define SMUx0B_x8434_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8434_LclkDpmEn_OFFSET                           0
#define SMUx0B_x8434_LclkDpmEn_WIDTH                            1
#define SMUx0B_x8434_LclkDpmEn_MASK                             0x1
#define SMUx0B_x8434_LclkDpmType_OFFSET                         1
#define SMUx0B_x8434_LclkDpmType_WIDTH                          1
#define SMUx0B_x8434_LclkDpmType_MASK                           0x2
#define SMUx0B_x8434_Reserved_3_2_OFFSET                        2
#define SMUx0B_x8434_Reserved_3_2_WIDTH                         2
#define SMUx0B_x8434_Reserved_3_2_MASK                          0xc
#define SMUx0B_x8434_LclkTimerPrescalar_OFFSET                  4
#define SMUx0B_x8434_LclkTimerPrescalar_WIDTH                   4
#define SMUx0B_x8434_LclkTimerPrescalar_MASK                    0xf0
#define SMUx0B_x8434_Reserved_15_8_OFFSET                       8
#define SMUx0B_x8434_Reserved_15_8_WIDTH                        8
#define SMUx0B_x8434_Reserved_15_8_MASK                         0xff00
#define SMUx0B_x8434_LclkTimerPeriod_OFFSET                     16
#define SMUx0B_x8434_LclkTimerPeriod_WIDTH                      16
#define SMUx0B_x8434_LclkTimerPeriod_MASK                       0xffff0000

/// SMUx0B_x8434
typedef union {
  struct {                                                              ///<
    UINT32                                                LclkDpmEn:1 ; ///<
    UINT32                                              LclkDpmType:1 ; ///<
    UINT32                                             Reserved_3_2:2 ; ///<
    UINT32                                       LclkTimerPrescalar:4 ; ///<
    UINT32                                            Reserved_15_8:8 ; ///<
    UINT32                                          LclkTimerPeriod:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8434_STRUCT;

// **** FCRxFF30_01E4 Register Definition ****
// Address
#define FCRxFF30_01E4_ADDRESS                                   0xff3001e4

// Type
#define FCRxFF30_01E4_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFF30_01E4_Reserved_19_0_OFFSET                      0
#define FCRxFF30_01E4_Reserved_19_0_WIDTH                       20
#define FCRxFF30_01E4_Reserved_19_0_MASK                        0xfffff
#define FCRxFF30_01E4_VoltageChangeEn_OFFSET                    20
#define FCRxFF30_01E4_VoltageChangeEn_WIDTH                     1
#define FCRxFF30_01E4_VoltageChangeEn_MASK                      0x100000
#define FCRxFF30_01E4_Reserved_31_21_OFFSET                     21
#define FCRxFF30_01E4_Reserved_31_21_WIDTH                      11
#define FCRxFF30_01E4_Reserved_31_21_MASK                       0xffe00000

/// FCRxFF30_01E4
typedef union {
  struct {                                                              ///<
    UINT32                                           Reserved_19_0:20; ///<
    UINT32                                          VoltageChangeEn:1 ; ///<
    UINT32                                           Reserved_31_21:11; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_01E4_STRUCT;

// **** SMUx0B_x8470 Register Definition ****
// Address
#define SMUx0B_x8470_ADDRESS                                    0x8470


// **** SMUx0B_x8440 Register Definition ****
// Address
#define SMUx0B_x8440_ADDRESS                                    0x8440


// **** SMUx0B_x848C Register Definition ****
// Address
#define SMUx0B_x848C_ADDRESS                                    0x848c


// **** SMUx35 Register Definition ****
// Address
#define SMUx35_ADDRESS                                          0x35

// Type
#define SMUx35_TYPE                                             TYPE_SMU
// Field Data
#define SMUx35_DownTrendCoef_OFFSET                             0
#define SMUx35_DownTrendCoef_WIDTH                              10
#define SMUx35_DownTrendCoef_MASK                               0x3ff
#define SMUx35_UpTrendCoef_OFFSET                               10
#define SMUx35_UpTrendCoef_WIDTH                                10
#define SMUx35_UpTrendCoef_MASK                                 0xffc00
#define SMUx35_Reserved_31_20_OFFSET                            20
#define SMUx35_Reserved_31_20_WIDTH                             12
#define SMUx35_Reserved_31_20_MASK                              0xfff00000

/// SMUx35
typedef union {
  struct {                                                              ///<
    UINT32                                            DownTrendCoef:10; ///<
    UINT32                                              UpTrendCoef:10; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx35_STRUCT;

// **** SMUx37 Register Definition ****
// Address
#define SMUx37_ADDRESS                                          0x37


// **** SMUx51 Register Definition ****
// Address
#define SMUx51_ADDRESS                                          0x51


// **** SMUx0B_x8490 Register Definition ****
// Address
#define SMUx0B_x8490_ADDRESS                                    0x8490


// **** DxF0xE4_xB5 Register Definition ****
// Address
#define DxF0xE4_xB5_ADDRESS                                     0xb5

// Type
#define DxF0xE4_xB5_TYPE                                        TYPE_D4F0xE4
// Field Data
#define DxF0xE4_xB5_Reserved_9_0_OFFSET                         0
#define DxF0xE4_xB5_Reserved_9_0_WIDTH                          10
#define DxF0xE4_xB5_Reserved_9_0_MASK                           0x3ff
#define DxF0xE4_xB5_LcEnhancedHotPlugEn_OFFSET                  10
#define DxF0xE4_xB5_LcEnhancedHotPlugEn_WIDTH                   1
#define DxF0xE4_xB5_LcEnhancedHotPlugEn_MASK                    0x400
#define DxF0xE4_xB5_Reserved_11_11_OFFSET                       11
#define DxF0xE4_xB5_Reserved_11_11_WIDTH                        1
#define DxF0xE4_xB5_Reserved_11_11_MASK                         0x800
#define DxF0xE4_xB5_LcEhpRxPhyCmd_OFFSET                        12
#define DxF0xE4_xB5_LcEhpRxPhyCmd_WIDTH                         2
#define DxF0xE4_xB5_LcEhpRxPhyCmd_MASK                          0x3000
#define DxF0xE4_xB5_LcEhpTxPhyCmd_OFFSET                        14
#define DxF0xE4_xB5_LcEhpTxPhyCmd_WIDTH                         2
#define DxF0xE4_xB5_LcEhpTxPhyCmd_MASK                          0xc000
#define DxF0xE4_xB5_Reserved_31_16_OFFSET                       16
#define DxF0xE4_xB5_Reserved_31_16_WIDTH                        16
#define DxF0xE4_xB5_Reserved_31_16_MASK                         0xffff0000

/// DxF0xE4_xB5
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_9_0:10; ///<
    UINT32                                      LcEnhancedHotPlugEn:1 ; ///<
    UINT32                                           Reserved_11_11:1 ; ///<
    UINT32                                            LcEhpRxPhyCmd:2 ; ///<
    UINT32                                            LcEhpTxPhyCmd:2 ; ///<
    UINT32                                           Reserved_31_16:16 ; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} DxF0xE4_xB5_STRUCT;

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

// **** DxF0xE4_xA5 Register Definition ****
// Address
#define DxF0xE4_xA5_ADDRESS                                     0xa5


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

// **** D0F0xE4_WRAP_8011 Register Definition ****
// Address
#define D0F0xE4_WRAP_8011_ADDRESS                               0x8011

// Type
#define D0F0xE4_WRAP_8011_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8011_TxclkDynGateLatency_OFFSET            0
#define D0F0xE4_WRAP_8011_TxclkDynGateLatency_WIDTH             6
#define D0F0xE4_WRAP_8011_TxclkDynGateLatency_MASK              0x3f
#define D0F0xE4_WRAP_8011_TxclkPermGateEven_OFFSET              6
#define D0F0xE4_WRAP_8011_TxclkPermGateEven_WIDTH               1
#define D0F0xE4_WRAP_8011_TxclkPermGateEven_MASK                0x40
#define D0F0xE4_WRAP_8011_TxclkDynGateEnable_OFFSET             7
#define D0F0xE4_WRAP_8011_TxclkDynGateEnable_WIDTH              1
#define D0F0xE4_WRAP_8011_TxclkDynGateEnable_MASK               0x80
#define D0F0xE4_WRAP_8011_Reserved_8_8_OFFSET                   8
#define D0F0xE4_WRAP_8011_Reserved_8_8_WIDTH                    1
#define D0F0xE4_WRAP_8011_Reserved_8_8_MASK                     0x100
#define D0F0xE4_WRAP_8011_TxclkRegsGateEnable_OFFSET            9
#define D0F0xE4_WRAP_8011_TxclkRegsGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8011_TxclkRegsGateEnable_MASK              0x200
#define D0F0xE4_WRAP_8011_TxclkRegsGateLatency_OFFSET           10
#define D0F0xE4_WRAP_8011_TxclkRegsGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8011_TxclkRegsGateLatency_MASK             0xfc00
#define D0F0xE4_WRAP_8011_RcvrDetClkEnable_OFFSET               16
#define D0F0xE4_WRAP_8011_RcvrDetClkEnable_WIDTH                1
#define D0F0xE4_WRAP_8011_RcvrDetClkEnable_MASK                 0x10000
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_OFFSET           17
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_WIDTH            6
#define D0F0xE4_WRAP_8011_TxclkPermGateLatency_MASK             0x7e0000
#define D0F0xE4_WRAP_8011_Reserved_23_23_OFFSET                 23
#define D0F0xE4_WRAP_8011_Reserved_23_23_WIDTH                  1
#define D0F0xE4_WRAP_8011_Reserved_23_23_MASK                   0x800000
#define D0F0xE4_WRAP_8011_TxclkLcntGateEnable_OFFSET            24
#define D0F0xE4_WRAP_8011_TxclkLcntGateEnable_WIDTH             1
#define D0F0xE4_WRAP_8011_TxclkLcntGateEnable_MASK              0x1000000
#define D0F0xE4_WRAP_8011_Reserved_31_25_OFFSET                 25
#define D0F0xE4_WRAP_8011_Reserved_31_25_WIDTH                  7
#define D0F0xE4_WRAP_8011_Reserved_31_25_MASK                   0xfe000000

/// D0F0xE4_WRAP_8011
typedef union {
  struct {                                                              ///<
    UINT32                                      TxclkDynGateLatency:6 ; ///<
    UINT32                                        TxclkPermGateEven:1 ; ///<
    UINT32                                       TxclkDynGateEnable:1 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                      TxclkRegsGateEnable:1 ; ///<
    UINT32                                     TxclkRegsGateLatency:6 ; ///<
    UINT32                                         RcvrDetClkEnable:1 ; ///<
    UINT32                                     TxclkPermGateLatency:6 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                      TxclkLcntGateEnable:1 ; ///< 
    UINT32                                           Reserved_31_25:7 ; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8011_STRUCT;

// **** D0F0xE4_WRAP_8016 Register Definition ****
// Address
#define D0F0xE4_WRAP_8016_ADDRESS                               0x8016

// Type
#define D0F0xE4_WRAP_8016_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8016_CalibAckLatency_OFFSET                0
#define D0F0xE4_WRAP_8016_CalibAckLatency_WIDTH                 6
#define D0F0xE4_WRAP_8016_CalibAckLatency_MASK                  0x3f
#define D0F0xE4_WRAP_8016_Reserved_21_6_OFFSET                  6
#define D0F0xE4_WRAP_8016_Reserved_21_6_WIDTH                   16
#define D0F0xE4_WRAP_8016_Reserved_21_6_MASK                    0x3fffc0
#define D0F0xE4_WRAP_8016_LclkGateFree_OFFSET                   22
#define D0F0xE4_WRAP_8016_LclkGateFree_WIDTH                    1
#define D0F0xE4_WRAP_8016_LclkGateFree_MASK                     0x400000
#define D0F0xE4_WRAP_8016_LclkDynGateEnable_OFFSET              23
#define D0F0xE4_WRAP_8016_LclkDynGateEnable_WIDTH               1
#define D0F0xE4_WRAP_8016_LclkDynGateEnable_MASK                0x800000
#define D0F0xE4_WRAP_8016_Reserved_31_24_OFFSET                 24
#define D0F0xE4_WRAP_8016_Reserved_31_24_WIDTH                  8
#define D0F0xE4_WRAP_8016_Reserved_31_24_MASK                   0xff000000

/// D0F0xE4_WRAP_8016
typedef union {
  struct {                                                              ///<
    UINT32                                          CalibAckLatency:6 ; ///<
    UINT32                                            Reserved_21_6:16; ///<
    UINT32                                             LclkGateFree:1 ; ///<
    UINT32                                        LclkDynGateEnable:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8016_STRUCT;

// **** D18F6x110 Register Definition ****
// Address
#define D18F6x110_ADDRESS                                       0x110

// Type
#define D18F6x110_TYPE                                          TYPE_D18F6
// Field Data
#define D18F6x110_NclkFifoOff_OFFSET                            0
#define D18F6x110_NclkFifoOff_WIDTH                             3
#define D18F6x110_NclkFifoOff_MASK                              0x7
#define D18F6x110_Reserved_3_3_OFFSET                           3
#define D18F6x110_Reserved_3_3_WIDTH                            1
#define D18F6x110_Reserved_3_3_MASK                             0x8
#define D18F6x110_LclkFifoOff_OFFSET                            4
#define D18F6x110_LclkFifoOff_WIDTH                             3
#define D18F6x110_LclkFifoOff_MASK                              0x70
#define D18F6x110_Reserved_7_7_OFFSET                           7
#define D18F6x110_Reserved_7_7_WIDTH                            1
#define D18F6x110_Reserved_7_7_MASK                             0x80
#define D18F6x110_PllMult_OFFSET                                8
#define D18F6x110_PllMult_WIDTH                                 6
#define D18F6x110_PllMult_MASK                                  0x3f00
#define D18F6x110_Reserved_14_14_OFFSET                         14
#define D18F6x110_Reserved_14_14_WIDTH                          1
#define D18F6x110_Reserved_14_14_MASK                           0x4000
#define D18F6x110_Enable_OFFSET                                 15
#define D18F6x110_Enable_WIDTH                                  1
#define D18F6x110_Enable_MASK                                   0x8000
#define D18F6x110_LclkFreq_OFFSET                               16
#define D18F6x110_LclkFreq_WIDTH                                7
#define D18F6x110_LclkFreq_MASK                                 0x7f0000
#define D18F6x110_LclkFreqType_OFFSET                           23
#define D18F6x110_LclkFreqType_WIDTH                            1
#define D18F6x110_LclkFreqType_MASK                             0x800000
#define D18F6x110_NclkFreq_OFFSET                               24
#define D18F6x110_NclkFreq_WIDTH                                7
#define D18F6x110_NclkFreq_MASK                                 0x7f000000
#define D18F6x110_NclkFreqType_OFFSET                           31
#define D18F6x110_NclkFreqType_WIDTH                            1
#define D18F6x110_NclkFreqType_MASK                             0x80000000

/// D18F6x110
typedef union {
  struct {                                                              ///<
    UINT32                                              NclkFifoOff:3 ; ///<
    UINT32                                             Reserved_3_3:1 ; ///<
    UINT32                                              LclkFifoOff:3 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                                  PllMult:6 ; ///<
    UINT32                                           Reserved_14_14:1 ; ///<
    UINT32                                                   Enable:1 ; ///<
    UINT32                                                 LclkFreq:7 ; ///<
    UINT32                                             LclkFreqType:1 ; ///<
    UINT32                                                 NclkFreq:7 ; ///<
    UINT32                                             NclkFreqType:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F6x110_STRUCT;

// **** D18F3xA0 Register Definition ****
// Address
#define D18F3xA0_ADDRESS                                        0xa0

// Type
#define D18F3xA0_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xA0_PsiVid_OFFSET                                  0
#define D18F3xA0_PsiVid_WIDTH                                   7
#define D18F3xA0_PsiVid_MASK                                    0x7f
#define D18F3xA0_PsiVidEn_OFFSET                                7
#define D18F3xA0_PsiVidEn_WIDTH                                 1
#define D18F3xA0_PsiVidEn_MASK                                  0x80
#define D18F3xA0_Reserved_8_8_OFFSET                            8
#define D18F3xA0_Reserved_8_8_WIDTH                             1
#define D18F3xA0_Reserved_8_8_MASK                              0x100
#define D18F3xA0_SviHighFreqSel_OFFSET                          9
#define D18F3xA0_SviHighFreqSel_WIDTH                           1
#define D18F3xA0_SviHighFreqSel_MASK                            0x200
#define D18F3xA0_Reserved_15_10_OFFSET                          10
#define D18F3xA0_Reserved_15_10_WIDTH                           6
#define D18F3xA0_Reserved_15_10_MASK                            0xfc00
#define D18F3xA0_ConfigId_OFFSET                                16
#define D18F3xA0_ConfigId_WIDTH                                 12
#define D18F3xA0_ConfigId_MASK                                  0xfff0000
#define D18F3xA0_Reserved_30_28_OFFSET                          28
#define D18F3xA0_Reserved_30_28_WIDTH                           3
#define D18F3xA0_Reserved_30_28_MASK                            0x70000000
#define D18F3xA0_CofVidProg_OFFSET                              31
#define D18F3xA0_CofVidProg_WIDTH                               1
#define D18F3xA0_CofVidProg_MASK                                0x80000000

/// D18F3xA0
typedef union {
  struct {                                                              ///<
    UINT32                                                   PsiVid:7 ; ///<
    UINT32                                                 PsiVidEn:1 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                           SviHighFreqSel:1 ; ///<
    UINT32                                           Reserved_15_10:6 ; ///<
    UINT32                                                 ConfigId:12; ///<
    UINT32                                           Reserved_30_28:3 ; ///<
    UINT32                                               CofVidProg:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xA0_STRUCT;

// **** FCRxFF30_0398 Register Definition ****
// Address
#define FCRxFF30_0398_ADDRESS                                   0xff300398

// Type
#define FCRxFF30_0398_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFF30_0398_Reserved_4_0_OFFSET                       0
#define FCRxFF30_0398_Reserved_4_0_WIDTH                        5
#define FCRxFF30_0398_Reserved_4_0_MASK                         0x1f
#define FCRxFF30_0398_SoftResetDc_OFFSET                        5
#define FCRxFF30_0398_SoftResetDc_WIDTH                         1
#define FCRxFF30_0398_SoftResetDc_MASK                          0x20
#define FCRxFF30_0398_Reserved_6_6_OFFSET                       6
#define FCRxFF30_0398_Reserved_6_6_WIDTH                        1
#define FCRxFF30_0398_Reserved_6_6_MASK                         0x40
#define FCRxFF30_0398_SoftResetGrbm_OFFSET                      8
#define FCRxFF30_0398_SoftResetGrbm_WIDTH                       1
#define FCRxFF30_0398_SoftResetGrbm_MASK                        0x100
#define FCRxFF30_0398_SoftResetMc_OFFSET                        11
#define FCRxFF30_0398_SoftResetMc_WIDTH                         1
#define FCRxFF30_0398_SoftResetMc_MASK                          0x800
#define FCRxFF30_0398_Reserved_12_12_OFFSET                     12
#define FCRxFF30_0398_Reserved_12_12_WIDTH                      1
#define FCRxFF30_0398_Reserved_12_12_MASK                       0x1000
#define FCRxFF30_0398_SoftResetRlc_OFFSET                       13
#define FCRxFF30_0398_SoftResetRlc_WIDTH                        1
#define FCRxFF30_0398_SoftResetRlc_MASK                         0x2000
#define FCRxFF30_0398_Reserved_16_16_OFFSET                     16
#define FCRxFF30_0398_Reserved_16_16_WIDTH                      1
#define FCRxFF30_0398_Reserved_16_16_MASK                       0x10000
#define FCRxFF30_0398_SoftResetUvd_OFFSET                       18
#define FCRxFF30_0398_SoftResetUvd_WIDTH                        1
#define FCRxFF30_0398_SoftResetUvd_MASK                         0x40000
#define FCRxFF30_0398_Reserved_31_19_OFFSET                     19
#define FCRxFF30_0398_Reserved_31_19_WIDTH                      13
#define FCRxFF30_0398_Reserved_31_19_MASK                       0xfff80000

/// FCRxFF30_0398
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                              SoftResetDc:1 ; ///<
    UINT32                                             Reserved_6_6:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                            SoftResetGrbm:1 ; ///<
    UINT32                                             Reserved_9_9:1 ; ///<
    UINT32                                           Reserved_10_10:1 ; ///<
    UINT32                                              SoftResetMc:1 ; ///<
    UINT32                                           Reserved_12_12:1 ; ///<
    UINT32                                             SoftResetRlc:1 ; ///<
    UINT32                                           Reserved_14_14:1 ; ///<
    UINT32                                           Reserved_15_15:1 ; ///<
    UINT32                                           Reserved_16_16:1 ; ///<
    UINT32                                           Reserved_17_17:1 ; ///<
    UINT32                                             SoftResetUvd:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_0398_STRUCT;

// **** SMUx0B_x8504 Register Definition ****
// Address
#define SMUx0B_x8504_ADDRESS                                    0x8504

// Type
#define SMUx0B_x8504_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8504_SaveRestoreWidth_OFFSET                    0
#define SMUx0B_x8504_SaveRestoreWidth_WIDTH                     8
#define SMUx0B_x8504_SaveRestoreWidth_MASK                      0xff
#define SMUx0B_x8504_PsoRestoreTimer_OFFSET                     8
#define SMUx0B_x8504_PsoRestoreTimer_WIDTH                      8
#define SMUx0B_x8504_PsoRestoreTimer_MASK                       0xff00
#define SMUx0B_x8504_Reserved_31_16_OFFSET                      16
#define SMUx0B_x8504_Reserved_31_16_WIDTH                       16
#define SMUx0B_x8504_Reserved_31_16_MASK                        0xffff0000

/// SMUx0B_x8504
typedef union {
  struct {                                                              ///<
    UINT32                                         SaveRestoreWidth:8 ; ///<
    UINT32                                          PsoRestoreTimer:8 ; ///<
    UINT32                                           Reserved_31_16:16; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8504_STRUCT;

// **** SMUx0B_x8408 Register Definition ****
// Address
#define SMUx0B_x8408_ADDRESS                                    0x8408


// **** SMUx0B_x8410 Register Definition ****
// Address
#define SMUx0B_x8410_ADDRESS                                    0x8410

// Type
#define SMUx0B_x8410_TYPE                                       TYPE_SMUx0B
// Field Data
#define SMUx0B_x8410_PwrGatingEn_OFFSET                         0
#define SMUx0B_x8410_PwrGatingEn_WIDTH                          1
#define SMUx0B_x8410_PwrGatingEn_MASK                           0x1
#define SMUx0B_x8410_Reserved_2_1_OFFSET                        1
#define SMUx0B_x8410_Reserved_2_1_WIDTH                         2
#define SMUx0B_x8410_Reserved_2_1_MASK                          0x6
#define SMUx0B_x8410_PsoControlValidNum_OFFSET                  3
#define SMUx0B_x8410_PsoControlValidNum_WIDTH                   5
#define SMUx0B_x8410_PsoControlValidNum_MASK                    0xf8
#define SMUx0B_x8410_SavePsoDelay_OFFSET                        8
#define SMUx0B_x8410_SavePsoDelay_WIDTH                         4
#define SMUx0B_x8410_SavePsoDelay_MASK                          0xf00
#define SMUx0B_x8410_Reserved_27_12_OFFSET                      12
#define SMUx0B_x8410_Reserved_27_12_WIDTH                       16
#define SMUx0B_x8410_Reserved_27_12_MASK                        0xffff000
#define SMUx0B_x8410_PwrGaterSel_OFFSET                         28
#define SMUx0B_x8410_PwrGaterSel_WIDTH                          4
#define SMUx0B_x8410_PwrGaterSel_MASK                           0xf0000000

/// SMUx0B_x8410
typedef union {
  struct {                                                              ///<
    UINT32                                              PwrGatingEn:1 ; ///<
    UINT32                                             Reserved_2_1:2 ; ///<
    UINT32                                       PsoControlValidNum:5 ; ///<
    UINT32                                             SavePsoDelay:4 ; ///<
    UINT32                                           Reserved_27_12:16; ///<
    UINT32                                              PwrGaterSel:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx0B_x8410_STRUCT;

// **** SMUx0B_x84A0 Register Definition ****
// Address
#define SMUx0B_x84A0_ADDRESS                                    0x84a0


// **** D0F0xE4_CORE_0020 Register Definition ****
// Address
#define D0F0xE4_CORE_0020_ADDRESS                               0x20

// Type
#define D0F0xE4_CORE_0020_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_CORE_0020_Reserved_8_0_OFFSET                   0
#define D0F0xE4_CORE_0020_Reserved_8_0_WIDTH                    9
#define D0F0xE4_CORE_0020_Reserved_8_0_MASK                     0x1ff
#define D0F0xE4_CORE_0020_CiSlvOrderingDis_OFFSET               8
#define D0F0xE4_CORE_0020_CiSlvOrderingDis_WIDTH                1
#define D0F0xE4_CORE_0020_CiSlvOrderingDis_MASK                 0x100
#define D0F0xE4_CORE_0020_CiRcOrderingDis_OFFSET                9
#define D0F0xE4_CORE_0020_CiRcOrderingDis_WIDTH                 1
#define D0F0xE4_CORE_0020_CiRcOrderingDis_MASK                  0x200
#define D0F0xE4_CORE_0020_Reserved_31_10_OFFSET                 10
#define D0F0xE4_CORE_0020_Reserved_31_10_WIDTH                  22
#define D0F0xE4_CORE_0020_Reserved_31_10_MASK                   0xfffffc00

/// D0F0xE4_CORE_0020
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_8_0:9 ; ///<
    UINT32                                          CiRcOrderingDis:1 ; ///<
    UINT32                                           Reserved_31_10:22; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0020_STRUCT;

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
#define D0F0xE4_CORE_00B0_Reserved_31_3_OFFSET                  3
#define D0F0xE4_CORE_00B0_Reserved_31_3_WIDTH                   29
#define D0F0xE4_CORE_00B0_Reserved_31_3_MASK                    0xfffffff8

/// D0F0xE4_CORE_00B0
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                             StrapF0MsiEn:1 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_00B0_STRUCT;

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
#define D0F0x64_x1C_Reserved_2_2_OFFSET                         2
#define D0F0x64_x1C_Reserved_2_2_WIDTH                          1
#define D0F0x64_x1C_Reserved_2_2_MASK                           0x4
#define D0F0x64_x1C_MemApSize_OFFSET                            3
#define D0F0x64_x1C_MemApSize_WIDTH                             3
#define D0F0x64_x1C_MemApSize_MASK                              0x38
#define D0F0x64_x1C_RegApSize_OFFSET                            6
#define D0F0x64_x1C_RegApSize_WIDTH                             1
#define D0F0x64_x1C_RegApSize_MASK                              0x40
#define D0F0x64_x1C_Reserved_7_7_OFFSET                         7
#define D0F0x64_x1C_Reserved_7_7_WIDTH                          1
#define D0F0x64_x1C_Reserved_7_7_MASK                           0x80
#define D0F0x64_x1C_AudioEn_OFFSET                              8
#define D0F0x64_x1C_AudioEn_WIDTH                               1
#define D0F0x64_x1C_AudioEn_MASK                                0x100
#define D0F0x64_x1C_Reserved_9_9_OFFSET                         9
#define D0F0x64_x1C_Reserved_9_9_WIDTH                          1
#define D0F0x64_x1C_Reserved_9_9_MASK                           0x200
#define D0F0x64_x1C_AudioNonlegacyDeviceTypeEn_OFFSET           10
#define D0F0x64_x1C_AudioNonlegacyDeviceTypeEn_WIDTH            1
#define D0F0x64_x1C_AudioNonlegacyDeviceTypeEn_MASK             0x400
#define D0F0x64_x1C_Reserved_16_11_OFFSET                       11
#define D0F0x64_x1C_Reserved_16_11_WIDTH                        6
#define D0F0x64_x1C_Reserved_16_11_MASK                         0x1f800
#define D0F0x64_x1C_F0En_OFFSET                                 17
#define D0F0x64_x1C_F0En_WIDTH                                  1
#define D0F0x64_x1C_F0En_MASK                                   0x20000
#define D0F0x64_x1C_Reserved_22_18_OFFSET                       18
#define D0F0x64_x1C_Reserved_22_18_WIDTH                        5
#define D0F0x64_x1C_Reserved_22_18_MASK                         0x7c0000
#define D0F0x64_x1C_RcieEn_OFFSET                               23
#define D0F0x64_x1C_RcieEn_WIDTH                                1
#define D0F0x64_x1C_RcieEn_MASK                                 0x800000
#define D0F0x64_x1C_Reserved_31_24_OFFSET                       24
#define D0F0x64_x1C_Reserved_31_24_WIDTH                        8
#define D0F0x64_x1C_Reserved_31_24_MASK                         0xff000000

/// D0F0x64_x1C
typedef union {
  struct {                                                              ///<
    UINT32                                                 WriteDis:1 ; ///<
    UINT32                                  F0NonlegacyDeviceTypeEn:1 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                                MemApSize:3 ; ///< 
    UINT32                                                RegApSize:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                                  AudioEn:1 ; ///<
    UINT32                                             Reserved_9_9:1 ; ///<
    UINT32                               AudioNonlegacyDeviceTypeEn:1 ; ///< 
    UINT32                                           Reserved_16_11:6 ; ///<
    UINT32                                                     F0En:1 ; ///<
    UINT32                                           Reserved_22_18:5 ; ///<
    UINT32                                                   RcieEn:1 ; ///<
    UINT32                                           Reserved_31_24:8 ; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} D0F0x64_x1C_STRUCT;

// **** D18F2x0F4_x40 Register Definition ****
// Address
#define D18F2x0F4_x40_ADDRESS                                   0x40

// Type
#define D18F2x0F4_x40_TYPE                                      TYPE_D18F2x0F4
// Field Data
#define D18F2x0F4_x40_Trcd_OFFSET                               0
#define D18F2x0F4_x40_Trcd_WIDTH                                4
#define D18F2x0F4_x40_Trcd_MASK                                 0xf
#define D18F2x0F4_x40_Reserved_7_4_OFFSET                       4
#define D18F2x0F4_x40_Reserved_7_4_WIDTH                        4
#define D18F2x0F4_x40_Reserved_7_4_MASK                         0xf0
#define D18F2x0F4_x40_Trp_OFFSET                                8
#define D18F2x0F4_x40_Trp_WIDTH                                 4
#define D18F2x0F4_x40_Trp_MASK                                  0xf00
#define D18F2x0F4_x40_Reserved_15_12_OFFSET                     12
#define D18F2x0F4_x40_Reserved_15_12_WIDTH                      4
#define D18F2x0F4_x40_Reserved_15_12_MASK                       0xf000
#define D18F2x0F4_x40_Tras_OFFSET                               16
#define D18F2x0F4_x40_Tras_WIDTH                                5
#define D18F2x0F4_x40_Tras_MASK                                 0x1f0000
#define D18F2x0F4_x40_Reserved_23_21_OFFSET                     21
#define D18F2x0F4_x40_Reserved_23_21_WIDTH                      3
#define D18F2x0F4_x40_Reserved_23_21_MASK                       0xe00000
#define D18F2x0F4_x40_Trc_OFFSET                                24
#define D18F2x0F4_x40_Trc_WIDTH                                 6
#define D18F2x0F4_x40_Trc_MASK                                  0x3f000000
#define D18F2x0F4_x40_Reserved_31_30_OFFSET                     30
#define D18F2x0F4_x40_Reserved_31_30_WIDTH                      2
#define D18F2x0F4_x40_Reserved_31_30_MASK                       0xc0000000

/// D18F2x0F4_x40
typedef union {
  struct {                                                              ///<
    UINT32                                                     Trcd:4 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                                      Trp:4 ; ///<
    UINT32                                           Reserved_15_12:4 ; ///<
    UINT32                                                     Tras:5 ; ///<
    UINT32                                           Reserved_23_21:3 ; ///<
    UINT32                                                      Trc:6 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///< 
  UINT32 Value;                                                         ///<
} D18F2x0F4_x40_STRUCT;

// **** D18F2x0F4_x41 Register Definition ****
// Address
#define D18F2x0F4_x41_ADDRESS                                   0x41

// Type
#define D18F2x0F4_x41_TYPE                                      TYPE_D18F2x0F4
// Field Data
#define D18F2x0F4_x41_Trtp_OFFSET                               0
#define D18F2x0F4_x41_Trtp_WIDTH                                3
#define D18F2x0F4_x41_Trtp_MASK                                 0x7
#define D18F2x0F4_x41_Reserved_7_3_OFFSET                       3
#define D18F2x0F4_x41_Reserved_7_3_WIDTH                        5
#define D18F2x0F4_x41_Reserved_7_3_MASK                         0xf8
#define D18F2x0F4_x41_Trrd_OFFSET                               8
#define D18F2x0F4_x41_Trrd_WIDTH                                3
#define D18F2x0F4_x41_Trrd_MASK                                 0x700
#define D18F2x0F4_x41_Reserved_15_11_OFFSET                     11
#define D18F2x0F4_x41_Reserved_15_11_WIDTH                      5
#define D18F2x0F4_x41_Reserved_15_11_MASK                       0xf800
#define D18F2x0F4_x41_Twtr_OFFSET                               16
#define D18F2x0F4_x41_Twtr_WIDTH                                3
#define D18F2x0F4_x41_Twtr_MASK                                 0x70000
#define D18F2x0F4_x41_Reserved_31_19_OFFSET                     19
#define D18F2x0F4_x41_Reserved_31_19_WIDTH                      13
#define D18F2x0F4_x41_Reserved_31_19_MASK                       0xfff80000

/// D18F2x0F4_x41
typedef union {
  struct {                                                              ///<
    UINT32                                                     Trtp:3 ; ///<
    UINT32                                             Reserved_7_3:5 ; ///<
    UINT32                                                     Trrd:3 ; ///<
    UINT32                                           Reserved_15_11:5 ; ///<
    UINT32                                                     Twtr:3 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x0F4_x41_STRUCT;

// **** D18F2x0F0 Register Definition ****
// Address
#define D18F2x0F0_ADDRESS                                       0xf0


// **** D18F2x1F0 Register Definition ****
// Address
#define D18F2x1F0_ADDRESS                                       0x1f0


// **** D18F2x184 Register Definition ****
// Address
#define D18F2x184_ADDRESS                                       0x184


// **** D18F2x094 Register Definition ****
// Address
#define D18F2x094_ADDRESS                                       0x94

// Type
#define D18F2x094_TYPE                                          TYPE_D18F2
// Field Data
#define D18F2x094_MemClkFreq_OFFSET                             0
#define D18F2x094_MemClkFreq_WIDTH                              5
#define D18F2x094_MemClkFreq_MASK                               0x1f
#define D18F2x094_Reserved_6_5_OFFSET                           5
#define D18F2x094_Reserved_6_5_WIDTH                            2
#define D18F2x094_Reserved_6_5_MASK                             0x60
#define D18F2x094_MemClkFreqVal_OFFSET                          7
#define D18F2x094_MemClkFreqVal_WIDTH                           1
#define D18F2x094_MemClkFreqVal_MASK                            0x80
#define D18F2x094_Reserved_9_8_OFFSET                           8
#define D18F2x094_Reserved_9_8_WIDTH                            2
#define D18F2x094_Reserved_9_8_MASK                             0x300
#define D18F2x094_ZqcsInterval_OFFSET                           10
#define D18F2x094_ZqcsInterval_WIDTH                            2
#define D18F2x094_ZqcsInterval_MASK                             0xc00
#define D18F2x094_Reserved_13_12_OFFSET                         12
#define D18F2x094_Reserved_13_12_WIDTH                          2
#define D18F2x094_Reserved_13_12_MASK                           0x3000
#define D18F2x094_DisDramInterface_OFFSET                       14
#define D18F2x094_DisDramInterface_WIDTH                        1
#define D18F2x094_DisDramInterface_MASK                         0x4000
#define D18F2x094_PowerDownEn_OFFSET                            15
#define D18F2x094_PowerDownEn_WIDTH                             1
#define D18F2x094_PowerDownEn_MASK                              0x8000
#define D18F2x094_PowerDownMode_OFFSET                          16
#define D18F2x094_PowerDownMode_WIDTH                           1
#define D18F2x094_PowerDownMode_MASK                            0x10000
#define D18F2x094_Reserved_19_17_OFFSET                         17
#define D18F2x094_Reserved_19_17_WIDTH                          3
#define D18F2x094_Reserved_19_17_MASK                           0xe0000
#define D18F2x094_SlowAccessMode_OFFSET                         20
#define D18F2x094_SlowAccessMode_WIDTH                          1
#define D18F2x094_SlowAccessMode_MASK                           0x100000
#define D18F2x094_Reserved_21_21_OFFSET                         21
#define D18F2x094_Reserved_21_21_WIDTH                          1
#define D18F2x094_Reserved_21_21_MASK                           0x200000
#define D18F2x094_BankSwizzleMode_OFFSET                        22
#define D18F2x094_BankSwizzleMode_WIDTH                         1
#define D18F2x094_BankSwizzleMode_MASK                          0x400000
#define D18F2x094_ProcOdtDis_OFFSET                             23
#define D18F2x094_ProcOdtDis_WIDTH                              1
#define D18F2x094_ProcOdtDis_MASK                               0x800000
#define D18F2x094_DcqBypassMax_OFFSET                           24
#define D18F2x094_DcqBypassMax_WIDTH                            4
#define D18F2x094_DcqBypassMax_MASK                             0xf000000
#define D18F2x094_FourActWindow_OFFSET                          28
#define D18F2x094_FourActWindow_WIDTH                           4
#define D18F2x094_FourActWindow_MASK                            0xf0000000

/// D18F2x094
typedef union {
  struct {                                                              ///<
    UINT32                                               MemClkFreq:5 ; ///<
    UINT32                                             Reserved_6_5:2 ; ///<
    UINT32                                            MemClkFreqVal:1 ; ///<
    UINT32                                             Reserved_9_8:2 ; ///<
    UINT32                                             ZqcsInterval:2 ; ///<
    UINT32                                           Reserved_13_12:2 ; ///<
    UINT32                                         DisDramInterface:1 ; ///<
    UINT32                                              PowerDownEn:1 ; ///<
    UINT32                                            PowerDownMode:1 ; ///<
    UINT32                                           Reserved_19_17:3 ; ///<
    UINT32                                           SlowAccessMode:1 ; ///<
    UINT32                                           Reserved_21_21:1 ; ///<
    UINT32                                          BankSwizzleMode:1 ; ///<
    UINT32                                               ProcOdtDis:1 ; ///<
    UINT32                                             DcqBypassMax:4 ; ///<
    UINT32                                            FourActWindow:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x094_STRUCT;

// **** D18F2x194 Register Definition ****
// Address
#define D18F2x194_ADDRESS                                       0x194


// **** D18F2x18C Register Definition ****
// Address
#define D18F2x18C_ADDRESS                                       0x18c


// **** D18F2x190 Register Definition ****
// Address
#define D18F2x190_ADDRESS                                       0x190


// **** D18F2x098 Register Definition ****
// Address
#define D18F2x098_ADDRESS                                       0x98


// **** D18F2x198 Register Definition ****
// Address
#define D18F2x198_ADDRESS                                       0x198


// **** D18F2x09C_x0D0FE00A Register Definition ****
// Address
#define D18F2x09C_x0D0FE00A_ADDRESS                             0x0D0FE00A

// Type
#define D18F2x09C_x0D0FE00A_TYPE                                TYPE_D18F2x9C
// Field Data
#define D18F2x09C_x0D0FE00A_Reserved_11_0_OFFSET                0
#define D18F2x09C_x0D0FE00A_Reserved_11_0_WIDTH                 12
#define D18F2x09C_x0D0FE00A_Reserved_11_0_MASK                  0xfff
#define D18F2x09C_x0D0FE00A_CsrPhySrPllPdMode_OFFSET            12
#define D18F2x09C_x0D0FE00A_CsrPhySrPllPdMode_WIDTH             2
#define D18F2x09C_x0D0FE00A_CsrPhySrPllPdMode_MASK              0x3000
#define D18F2x09C_x0D0FE00A_SelCsrPllPdMode_OFFSET              14
#define D18F2x09C_x0D0FE00A_SelCsrPllPdMode_WIDTH               1
#define D18F2x09C_x0D0FE00A_SelCsrPllPdMode_MASK                0x4000
#define D18F2x09C_x0D0FE00A_Reserved_31_15_OFFSET               15
#define D18F2x09C_x0D0FE00A_Reserved_31_15_WIDTH                17
#define D18F2x09C_x0D0FE00A_Reserved_31_15_MASK                 0xFFFF8000

/// D18F2x09C_x0D0FE00A
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_11_0:12; ///<
    UINT32                                         CsrPhySrPllPdMode:2; ///<
    UINT32                                           SelCsrPllPdMode:1; ///<
    UINT32                                            Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F2x09C_x0D0FE00A_STRUCT;

// **** GMMx201C Register Definition ****
// Address
#define GMMx201C_ADDRESS                                        0x201c


// **** GMMx217C Register Definition ****
// Address
#define GMMx217C_ADDRESS                                        0x217c


// **** GMMx2188 Register Definition ****
// Address
#define GMMx2188_ADDRESS                                        0x2188


// **** GMMx28C8 Register Definition ****
// Address
#define GMMx28C8_ADDRESS                                        0x28c8


// **** SMUx01 Register Definition ****
// Address
#define SMUx01_ADDRESS                                          0x1

// Type
#define SMUx01_TYPE                                             TYPE_SMU
// Field Data
#define SMUx01_RamSwitch_OFFSET                                 0
#define SMUx01_RamSwitch_WIDTH                                  1
#define SMUx01_RamSwitch_MASK                                   0x1
#define SMUx01_Reset_OFFSET                                     1
#define SMUx01_Reset_WIDTH                                      1
#define SMUx01_Reset_MASK                                       0x2
#define SMUx01_Reserved_17_2_OFFSET                             2
#define SMUx01_Reserved_17_2_WIDTH                              16
#define SMUx01_Reserved_17_2_MASK                               0x3fffc
#define SMUx01_VectorOverride_OFFSET                            18
#define SMUx01_VectorOverride_WIDTH                             1
#define SMUx01_VectorOverride_MASK                              0x40000
#define SMUx01_Reserved_31_19_OFFSET                            19
#define SMUx01_Reserved_31_19_WIDTH                             13
#define SMUx01_Reserved_31_19_MASK                              0xfff80000
//
/// SMUx01
typedef union {
  struct {                                                              ///<
    UINT32                                                RamSwitch:1 ; ///<
    UINT32                                                    Reset:1 ; ///<
    UINT32                                            Reserved_17_2:16; ///<
    UINT32                                           VectorOverride:1 ; ///<
    UINT32                                           Reserved_31_19:13; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx01_STRUCT;

// **** FCRxFE00_70A4 Register Definition ****
// Address
#define FCRxFE00_70A4_ADDRESS                                   0xfe0070a4

// Type
#define FCRxFE00_70A4_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70A4_Reserved_3_0_OFFSET                       0
#define FCRxFE00_70A4_Reserved_3_0_WIDTH                        4
#define FCRxFE00_70A4_Reserved_3_0_MASK                         0xf
#define FCRxFE00_70A4_SclkDpmVid0_OFFSET                        4
#define FCRxFE00_70A4_SclkDpmVid0_WIDTH                         2
#define FCRxFE00_70A4_SclkDpmVid0_MASK                          0x30
#define FCRxFE00_70A4_SclkDpmVid1_OFFSET                        6
#define FCRxFE00_70A4_SclkDpmVid1_WIDTH                         2
#define FCRxFE00_70A4_SclkDpmVid1_MASK                          0xc0
#define FCRxFE00_70A4_SclkDpmVid2_OFFSET                        8
#define FCRxFE00_70A4_SclkDpmVid2_WIDTH                         2
#define FCRxFE00_70A4_SclkDpmVid2_MASK                          0x300
#define FCRxFE00_70A4_SclkDpmVid3_OFFSET                        10
#define FCRxFE00_70A4_SclkDpmVid3_WIDTH                         2
#define FCRxFE00_70A4_SclkDpmVid3_MASK                          0xc00
#define FCRxFE00_70A4_SclkDpmVid4_OFFSET                        12
#define FCRxFE00_70A4_SclkDpmVid4_WIDTH                         2
#define FCRxFE00_70A4_SclkDpmVid4_MASK                          0x3000
#define FCRxFE00_70A4_Reserved_31_14_OFFSET                     14
#define FCRxFE00_70A4_Reserved_31_14_WIDTH                      18
#define FCRxFE00_70A4_Reserved_31_14_MASK                       0xffffc000

/// FCRxFE00_70A4
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                              SclkDpmVid0:2 ; ///<
    UINT32                                              SclkDpmVid1:2 ; ///<
    UINT32                                              SclkDpmVid2:2 ; ///<
    UINT32                                              SclkDpmVid3:2 ; ///<
    UINT32                                              SclkDpmVid4:2 ; ///<
    UINT32                                           Reserved_31_14:18; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70A4_STRUCT;

// **** FCRxFE00_70A5 Register Definition ****
// Address
#define FCRxFE00_70A5_ADDRESS                                   0xfe0070a5

// Type
#define FCRxFE00_70A5_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70A5_Reserved_5_0_OFFSET                       0
#define FCRxFE00_70A5_Reserved_5_0_WIDTH                        6
#define FCRxFE00_70A5_Reserved_5_0_MASK                         0x3f
#define FCRxFE00_70A5_SclkDpmDid0_OFFSET                        6
#define FCRxFE00_70A5_SclkDpmDid0_WIDTH                         7
#define FCRxFE00_70A5_SclkDpmDid0_MASK                          0x1fc0
#define FCRxFE00_70A5_SclkDpmDid1_OFFSET                        13
#define FCRxFE00_70A5_SclkDpmDid1_WIDTH                         7
#define FCRxFE00_70A5_SclkDpmDid1_MASK                          0xfe000
#define FCRxFE00_70A5_SclkDpmDid2_OFFSET                        20
#define FCRxFE00_70A5_SclkDpmDid2_WIDTH                         7
#define FCRxFE00_70A5_SclkDpmDid2_MASK                          0x7f00000
#define FCRxFE00_70A5_Reserved_31_27_OFFSET                     27
#define FCRxFE00_70A5_Reserved_31_27_WIDTH                      5
#define FCRxFE00_70A5_Reserved_31_27_MASK                       0xf8000000

/// FCRxFE00_70A5
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_5_0:6 ; ///<
    UINT32                                              SclkDpmDid0:7 ; ///<
    UINT32                                              SclkDpmDid1:7 ; ///<
    UINT32                                              SclkDpmDid2:7 ; ///<
    UINT32                                           Reserved_31_27:5 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70A5_STRUCT;

// **** FCRxFE00_70A8 Register Definition ****
// Address
#define FCRxFE00_70A8_ADDRESS                                   0xfe0070a8

// Type
#define FCRxFE00_70A8_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70A8_Reserved_2_0_OFFSET                       0
#define FCRxFE00_70A8_Reserved_2_0_WIDTH                        3
#define FCRxFE00_70A8_Reserved_2_0_MASK                         0x7
#define FCRxFE00_70A8_SclkDpmDid3_OFFSET                        3
#define FCRxFE00_70A8_SclkDpmDid3_WIDTH                         7
#define FCRxFE00_70A8_SclkDpmDid3_MASK                          0x3f8
#define FCRxFE00_70A8_SclkDpmDid4_OFFSET                        10
#define FCRxFE00_70A8_SclkDpmDid4_WIDTH                         7
#define FCRxFE00_70A8_SclkDpmDid4_MASK                          0x1fc00
#define FCRxFE00_70A8_Reserved_31_17_OFFSET                     17
#define FCRxFE00_70A8_Reserved_31_17_WIDTH                      15
#define FCRxFE00_70A8_Reserved_31_17_MASK                       0xfffe0000

/// FCRxFE00_70A8
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                              SclkDpmDid3:7 ; ///<
    UINT32                                              SclkDpmDid4:7 ; ///<
    UINT32                                           Reserved_31_17:15; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70A8_STRUCT;

// **** FCRxFE00_70AE Register Definition ****
// Address
#define FCRxFE00_70AE_ADDRESS                                   0xfe0070ae

// Type
#define FCRxFE00_70AE_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70AE_Reserved_0_0_OFFSET                       0
#define FCRxFE00_70AE_Reserved_0_0_WIDTH                        1
#define FCRxFE00_70AE_Reserved_0_0_MASK                         0x1
#define FCRxFE00_70AE_DispClkDid0_OFFSET                        1
#define FCRxFE00_70AE_DispClkDid0_WIDTH                         7
#define FCRxFE00_70AE_DispClkDid0_MASK                          0xfe
#define FCRxFE00_70AE_DispClkDid1_OFFSET                        8
#define FCRxFE00_70AE_DispClkDid1_WIDTH                         7
#define FCRxFE00_70AE_DispClkDid1_MASK                          0x7f00
#define FCRxFE00_70AE_DispClkDid2_OFFSET                        15
#define FCRxFE00_70AE_DispClkDid2_WIDTH                         7
#define FCRxFE00_70AE_DispClkDid2_MASK                          0x3f8000
#define FCRxFE00_70AE_DispClkDid3_OFFSET                        22
#define FCRxFE00_70AE_DispClkDid3_WIDTH                         7
#define FCRxFE00_70AE_DispClkDid3_MASK                          0x1fc00000
#define FCRxFE00_70AE_Reserved_31_29_OFFSET                     29
#define FCRxFE00_70AE_Reserved_31_29_WIDTH                      3
#define FCRxFE00_70AE_Reserved_31_29_MASK                       0xe0000000

/// FCRxFE00_70AE
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                              DispClkDid0:7 ; ///<
    UINT32                                              DispClkDid1:7 ; ///<
    UINT32                                              DispClkDid2:7 ; ///<
    UINT32                                              DispClkDid3:7 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70AE_STRUCT;

// **** FCRxFE00_70B1 Register Definition ****
// Address
#define FCRxFE00_70B1_ADDRESS                                   0xfe0070b1

// Type
#define FCRxFE00_70B1_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70B1_Reserved_4_0_OFFSET                       0
#define FCRxFE00_70B1_Reserved_4_0_WIDTH                        5
#define FCRxFE00_70B1_Reserved_4_0_MASK                         0x1f
#define FCRxFE00_70B1_LclkDpmDid0_OFFSET                        5
#define FCRxFE00_70B1_LclkDpmDid0_WIDTH                         7
#define FCRxFE00_70B1_LclkDpmDid0_MASK                          0xfe0
#define FCRxFE00_70B1_LclkDpmDid1_OFFSET                        12
#define FCRxFE00_70B1_LclkDpmDid1_WIDTH                         7
#define FCRxFE00_70B1_LclkDpmDid1_MASK                          0x7f000
#define FCRxFE00_70B1_LclkDpmDid2_OFFSET                        19
#define FCRxFE00_70B1_LclkDpmDid2_WIDTH                         7
#define FCRxFE00_70B1_LclkDpmDid2_MASK                          0x3f80000
#define FCRxFE00_70B1_Reserved_31_26_OFFSET                     26
#define FCRxFE00_70B1_Reserved_31_26_WIDTH                      6
#define FCRxFE00_70B1_Reserved_31_26_MASK                       0xfc000000

/// FCRxFE00_70B1
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                              LclkDpmDid0:7 ; ///<
    UINT32                                              LclkDpmDid1:7 ; ///<
    UINT32                                              LclkDpmDid2:7 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70B1_STRUCT;

// **** FCRxFE00_70B4 Register Definition ****
// Address
#define FCRxFE00_70B4_ADDRESS                                   0xfe0070b4

// Type
#define FCRxFE00_70B4_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70B4_Reserved_1_0_OFFSET                       0
#define FCRxFE00_70B4_Reserved_1_0_WIDTH                        2
#define FCRxFE00_70B4_Reserved_1_0_MASK                         0x3
#define FCRxFE00_70B4_LclkDpmDid3_OFFSET                        2
#define FCRxFE00_70B4_LclkDpmDid3_WIDTH                         7
#define FCRxFE00_70B4_LclkDpmDid3_MASK                          0x1fc
#define FCRxFE00_70B4_LclkDpmValid0_OFFSET                      9
#define FCRxFE00_70B4_LclkDpmValid0_WIDTH                       1
#define FCRxFE00_70B4_LclkDpmValid0_MASK                        0x200
#define FCRxFE00_70B4_LclkDpmValid1_OFFSET                      10
#define FCRxFE00_70B4_LclkDpmValid1_WIDTH                       1
#define FCRxFE00_70B4_LclkDpmValid1_MASK                        0x400
#define FCRxFE00_70B4_LclkDpmValid2_OFFSET                      11
#define FCRxFE00_70B4_LclkDpmValid2_WIDTH                       1
#define FCRxFE00_70B4_LclkDpmValid2_MASK                        0x800
#define FCRxFE00_70B4_LclkDpmValid3_OFFSET                      12
#define FCRxFE00_70B4_LclkDpmValid3_WIDTH                       1
#define FCRxFE00_70B4_LclkDpmValid3_MASK                        0x1000
#define FCRxFE00_70B4_Reserved_31_13_OFFSET                     13
#define FCRxFE00_70B4_Reserved_31_13_WIDTH                      19
#define FCRxFE00_70B4_Reserved_31_13_MASK                       0xffffe000

/// FCRxFE00_70B4
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                              LclkDpmDid3:7 ; ///<
    UINT32                                            LclkDpmValid0:1 ; ///<
    UINT32                                            LclkDpmValid1:1 ; ///<
    UINT32                                            LclkDpmValid2:1 ; ///<
    UINT32                                            LclkDpmValid3:1 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70B4_STRUCT;

// **** FCRxFE00_70B5 Register Definition ****
// Address
#define FCRxFE00_70B5_ADDRESS                                   0xfe0070b5

// Type
#define FCRxFE00_70B5_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70B5_Reserved_4_0_OFFSET                       0
#define FCRxFE00_70B5_Reserved_4_0_WIDTH                        5
#define FCRxFE00_70B5_Reserved_4_0_MASK                         0x1f
#define FCRxFE00_70B5_DclkDid0_OFFSET                           5
#define FCRxFE00_70B5_DclkDid0_WIDTH                            7
#define FCRxFE00_70B5_DclkDid0_MASK                             0xfe0
#define FCRxFE00_70B5_DclkDid1_OFFSET                           12
#define FCRxFE00_70B5_DclkDid1_WIDTH                            7
#define FCRxFE00_70B5_DclkDid1_MASK                             0x7f000
#define FCRxFE00_70B5_DclkDid2_OFFSET                           19
#define FCRxFE00_70B5_DclkDid2_WIDTH                            7
#define FCRxFE00_70B5_DclkDid2_MASK                             0x3f80000
#define FCRxFE00_70B5_Reserved_31_26_OFFSET                     26
#define FCRxFE00_70B5_Reserved_31_26_WIDTH                      6
#define FCRxFE00_70B5_Reserved_31_26_MASK                       0xfc000000

/// FCRxFE00_70B5
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                                 DclkDid0:7 ; ///<
    UINT32                                                 DclkDid1:7 ; ///<
    UINT32                                                 DclkDid2:7 ; ///<
    UINT32                                           Reserved_31_26:6 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70B5_STRUCT;

// **** FCRxFE00_70B8 Register Definition ****
// Address
#define FCRxFE00_70B8_ADDRESS                                   0xfe0070b8

// Type
#define FCRxFE00_70B8_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70B8_Reserved_1_0_OFFSET                       0
#define FCRxFE00_70B8_Reserved_1_0_WIDTH                        2
#define FCRxFE00_70B8_Reserved_1_0_MASK                         0x3
#define FCRxFE00_70B8_DclkDid3_OFFSET                           2
#define FCRxFE00_70B8_DclkDid3_WIDTH                            7
#define FCRxFE00_70B8_DclkDid3_MASK                             0x1fc
#define FCRxFE00_70B8_Reserved_31_9_OFFSET                      9
#define FCRxFE00_70B8_Reserved_31_9_WIDTH                       23
#define FCRxFE00_70B8_Reserved_31_9_MASK                        0xfffffe00

/// FCRxFE00_70B8
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_1_0:2 ; ///<
    UINT32                                                 DclkDid3:7 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70B8_STRUCT;

// **** FCRxFE00_70B9 Register Definition ****
// Address
#define FCRxFE00_70B9_ADDRESS                                   0xfe0070b9

// Type
#define FCRxFE00_70B9_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70B9_Reserved_0_0_OFFSET                       0
#define FCRxFE00_70B9_Reserved_0_0_WIDTH                        1
#define FCRxFE00_70B9_Reserved_0_0_MASK                         0x1
#define FCRxFE00_70B9_VclkDid0_OFFSET                           1
#define FCRxFE00_70B9_VclkDid0_WIDTH                            7
#define FCRxFE00_70B9_VclkDid0_MASK                             0xfe
#define FCRxFE00_70B9_VclkDid1_OFFSET                           8
#define FCRxFE00_70B9_VclkDid1_WIDTH                            7
#define FCRxFE00_70B9_VclkDid1_MASK                             0x7f00
#define FCRxFE00_70B9_VclkDid2_OFFSET                           15
#define FCRxFE00_70B9_VclkDid2_WIDTH                            7
#define FCRxFE00_70B9_VclkDid2_MASK                             0x3f8000
#define FCRxFE00_70B9_VclkDid3_OFFSET                           22
#define FCRxFE00_70B9_VclkDid3_WIDTH                            7
#define FCRxFE00_70B9_VclkDid3_MASK                             0x1fc00000
#define FCRxFE00_70B9_Reserved_31_29_OFFSET                     29
#define FCRxFE00_70B9_Reserved_31_29_WIDTH                      3
#define FCRxFE00_70B9_Reserved_31_29_MASK                       0xe0000000

/// FCRxFE00_70B9
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                                 VclkDid0:7 ; ///<
    UINT32                                                 VclkDid1:7 ; ///<
    UINT32                                                 VclkDid2:7 ; ///<
    UINT32                                                 VclkDid3:7 ; ///<
    UINT32                                           Reserved_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70B9_STRUCT;

// **** FCRxFE00_70BC Register Definition ****
// Address
#define FCRxFE00_70BC_ADDRESS                                   0xfe0070bc

// Type
#define FCRxFE00_70BC_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70BC_Reserved_4_0_OFFSET                       0
#define FCRxFE00_70BC_Reserved_4_0_WIDTH                        5
#define FCRxFE00_70BC_Reserved_4_0_MASK                         0x1f
#define FCRxFE00_70BC_SclkDpmValid0_OFFSET                      5
#define FCRxFE00_70BC_SclkDpmValid0_WIDTH                       5
#define FCRxFE00_70BC_SclkDpmValid0_MASK                        0x3e0
#define FCRxFE00_70BC_SclkDpmValid1_OFFSET                      10
#define FCRxFE00_70BC_SclkDpmValid1_WIDTH                       5
#define FCRxFE00_70BC_SclkDpmValid1_MASK                        0x7c00
#define FCRxFE00_70BC_SclkDpmValid2_OFFSET                      15
#define FCRxFE00_70BC_SclkDpmValid2_WIDTH                       5
#define FCRxFE00_70BC_SclkDpmValid2_MASK                        0xf8000
#define FCRxFE00_70BC_SclkDpmValid3_OFFSET                      20
#define FCRxFE00_70BC_SclkDpmValid3_WIDTH                       5
#define FCRxFE00_70BC_SclkDpmValid3_MASK                        0x1f00000
#define FCRxFE00_70BC_SclkDpmValid4_OFFSET                      25
#define FCRxFE00_70BC_SclkDpmValid4_WIDTH                       5
#define FCRxFE00_70BC_SclkDpmValid4_MASK                        0x3e000000
#define FCRxFE00_70BC_Reserved_31_30_OFFSET                     30
#define FCRxFE00_70BC_Reserved_31_30_WIDTH                      2
#define FCRxFE00_70BC_Reserved_31_30_MASK                       0xc0000000

/// FCRxFE00_70BC
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_4_0:5 ; ///<
    UINT32                                            SclkDpmValid0:5 ; ///<
    UINT32                                            SclkDpmValid1:5 ; ///<
    UINT32                                            SclkDpmValid2:5 ; ///<
    UINT32                                            SclkDpmValid3:5 ; ///<
    UINT32                                            SclkDpmValid4:5 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70BC_STRUCT;

// **** FCRxFE00_70BF Register Definition ****
// Address
#define FCRxFE00_70BF_ADDRESS                                   0xfe0070bf

// Type
#define FCRxFE00_70BF_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70BF_Reserved_5_0_OFFSET                       0
#define FCRxFE00_70BF_Reserved_5_0_WIDTH                        6
#define FCRxFE00_70BF_Reserved_5_0_MASK                         0x3f
#define FCRxFE00_70BF_SclkDpmValid5_OFFSET                      6
#define FCRxFE00_70BF_SclkDpmValid5_WIDTH                       5
#define FCRxFE00_70BF_SclkDpmValid5_MASK                        0x7c0
#define FCRxFE00_70BF_Reserved_31_11_OFFSET                     11
#define FCRxFE00_70BF_Reserved_31_11_WIDTH                      21
#define FCRxFE00_70BF_Reserved_31_11_MASK                       0xfffff800

/// FCRxFE00_70BF
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_5_0:6 ; ///<
    UINT32                                            SclkDpmValid5:5 ; ///<
    UINT32                                           Reserved_31_11:21; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70BF_STRUCT;

// **** FCRxFE00_70C0 Register Definition ****
// Address
#define FCRxFE00_70C0_ADDRESS                                   0xfe0070c0

// Type
#define FCRxFE00_70C0_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70C0_Reserved_2_0_OFFSET                       0
#define FCRxFE00_70C0_Reserved_2_0_WIDTH                        3
#define FCRxFE00_70C0_Reserved_2_0_MASK                         0x7
#define FCRxFE00_70C0_PolicyLabel0_OFFSET                       3
#define FCRxFE00_70C0_PolicyLabel0_WIDTH                        2
#define FCRxFE00_70C0_PolicyLabel0_MASK                         0x18
#define FCRxFE00_70C0_PolicyLabel1_OFFSET                       5
#define FCRxFE00_70C0_PolicyLabel1_WIDTH                        2
#define FCRxFE00_70C0_PolicyLabel1_MASK                         0x60
#define FCRxFE00_70C0_PolicyLabel2_OFFSET                       7
#define FCRxFE00_70C0_PolicyLabel2_WIDTH                        2
#define FCRxFE00_70C0_PolicyLabel2_MASK                         0x180
#define FCRxFE00_70C0_PolicyLabel3_OFFSET                       9
#define FCRxFE00_70C0_PolicyLabel3_WIDTH                        2
#define FCRxFE00_70C0_PolicyLabel3_MASK                         0x600
#define FCRxFE00_70C0_PolicyLabel4_OFFSET                       11
#define FCRxFE00_70C0_PolicyLabel4_WIDTH                        2
#define FCRxFE00_70C0_PolicyLabel4_MASK                         0x1800
#define FCRxFE00_70C0_PolicyLabel5_OFFSET                       13
#define FCRxFE00_70C0_PolicyLabel5_WIDTH                        2
#define FCRxFE00_70C0_PolicyLabel5_MASK                         0x6000
#define FCRxFE00_70C0_Reserved_31_15_OFFSET                     15
#define FCRxFE00_70C0_Reserved_31_15_WIDTH                      17
#define FCRxFE00_70C0_Reserved_31_15_MASK                       0xffff8000

/// FCRxFE00_70C0
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_2_0:3 ; ///<
    UINT32                                             PolicyLabel0:2 ; ///<
    UINT32                                             PolicyLabel1:2 ; ///<
    UINT32                                             PolicyLabel2:2 ; ///<
    UINT32                                             PolicyLabel3:2 ; ///<
    UINT32                                             PolicyLabel4:2 ; ///<
    UINT32                                             PolicyLabel5:2 ; ///<
    UINT32                                           Reserved_31_15:17; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70C0_STRUCT;

// **** FCRxFE00_70C1 Register Definition ****
// Address
#define FCRxFE00_70C1_ADDRESS                                   0xfe0070c1

// Type
#define FCRxFE00_70C1_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70C1_Reserved_6_0_OFFSET                       0
#define FCRxFE00_70C1_Reserved_6_0_WIDTH                        7
#define FCRxFE00_70C1_Reserved_6_0_MASK                         0x7f
#define FCRxFE00_70C1_PolicyFlags0_OFFSET                       7
#define FCRxFE00_70C1_PolicyFlags0_WIDTH                        7
#define FCRxFE00_70C1_PolicyFlags0_MASK                         0x3f80
#define FCRxFE00_70C1_PolicyFlags1_OFFSET                       14
#define FCRxFE00_70C1_PolicyFlags1_WIDTH                        7
#define FCRxFE00_70C1_PolicyFlags1_MASK                         0x1fc000
#define FCRxFE00_70C1_PolicyFlags2_OFFSET                       21
#define FCRxFE00_70C1_PolicyFlags2_WIDTH                        7
#define FCRxFE00_70C1_PolicyFlags2_MASK                         0xfe00000
#define FCRxFE00_70C1_Reserved_31_28_OFFSET                     28
#define FCRxFE00_70C1_Reserved_31_28_WIDTH                      4
#define FCRxFE00_70C1_Reserved_31_28_MASK                       0xf0000000

/// FCRxFE00_70C1
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_6_0:7 ; ///<
    UINT32                                             PolicyFlags0:7 ; ///<
    UINT32                                             PolicyFlags1:7 ; ///<
    UINT32                                             PolicyFlags2:7 ; ///<
    UINT32                                           Reserved_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70C1_STRUCT;

// **** FCRxFE00_70C4 Register Definition ****
// Address
#define FCRxFE00_70C4_ADDRESS                                   0xfe0070c4

// Type
#define FCRxFE00_70C4_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70C4_Reserved_3_0_OFFSET                       0
#define FCRxFE00_70C4_Reserved_3_0_WIDTH                        4
#define FCRxFE00_70C4_Reserved_3_0_MASK                         0xf
#define FCRxFE00_70C4_PolicyFlags3_OFFSET                       4
#define FCRxFE00_70C4_PolicyFlags3_WIDTH                        7
#define FCRxFE00_70C4_PolicyFlags3_MASK                         0x7f0
#define FCRxFE00_70C4_PolicyFlags4_OFFSET                       11
#define FCRxFE00_70C4_PolicyFlags4_WIDTH                        7
#define FCRxFE00_70C4_PolicyFlags4_MASK                         0x3f800
#define FCRxFE00_70C4_PolicyFlags5_OFFSET                       18
#define FCRxFE00_70C4_PolicyFlags5_WIDTH                        7
#define FCRxFE00_70C4_PolicyFlags5_MASK                         0x1fc0000
#define FCRxFE00_70C4_Reserved_31_25_OFFSET                     25
#define FCRxFE00_70C4_Reserved_31_25_WIDTH                      7
#define FCRxFE00_70C4_Reserved_31_25_MASK                       0xfe000000

/// FCRxFE00_70C4
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_3_0:4 ; ///<
    UINT32                                             PolicyFlags3:7 ; ///<
    UINT32                                             PolicyFlags4:7 ; ///<
    UINT32                                             PolicyFlags5:7 ; ///<
    UINT32                                           Reserved_31_25:7 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70C4_STRUCT;

// **** FCRxFE00_70C7 Register Definition ****
// Address
#define FCRxFE00_70C7_ADDRESS                                   0xfe0070c7

// Type
#define FCRxFE00_70C7_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70C7_Reserved_0_0_OFFSET                       0
#define FCRxFE00_70C7_Reserved_0_0_WIDTH                        1
#define FCRxFE00_70C7_Reserved_0_0_MASK                         0x1
#define FCRxFE00_70C7_DclkVclkSel0_OFFSET                       1
#define FCRxFE00_70C7_DclkVclkSel0_WIDTH                        2
#define FCRxFE00_70C7_DclkVclkSel0_MASK                         0x6
#define FCRxFE00_70C7_DclkVclkSel1_OFFSET                       3
#define FCRxFE00_70C7_DclkVclkSel1_WIDTH                        2
#define FCRxFE00_70C7_DclkVclkSel1_MASK                         0x18
#define FCRxFE00_70C7_DclkVclkSel2_OFFSET                       5
#define FCRxFE00_70C7_DclkVclkSel2_WIDTH                        2
#define FCRxFE00_70C7_DclkVclkSel2_MASK                         0x60
#define FCRxFE00_70C7_DclkVclkSel3_OFFSET                       7
#define FCRxFE00_70C7_DclkVclkSel3_WIDTH                        2
#define FCRxFE00_70C7_DclkVclkSel3_MASK                         0x180
#define FCRxFE00_70C7_DclkVclkSel4_OFFSET                       9
#define FCRxFE00_70C7_DclkVclkSel4_WIDTH                        2
#define FCRxFE00_70C7_DclkVclkSel4_MASK                         0x600
#define FCRxFE00_70C7_DclkVclkSel5_OFFSET                       11
#define FCRxFE00_70C7_DclkVclkSel5_WIDTH                        2
#define FCRxFE00_70C7_DclkVclkSel5_MASK                         0x1800
#define FCRxFE00_70C7_Reserved_31_13_OFFSET                     13
#define FCRxFE00_70C7_Reserved_31_13_WIDTH                      19
#define FCRxFE00_70C7_Reserved_31_13_MASK                       0xffffe000

/// FCRxFE00_70C7
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                             DclkVclkSel0:2 ; ///<
    UINT32                                             DclkVclkSel1:2 ; ///<
    UINT32                                             DclkVclkSel2:2 ; ///<
    UINT32                                             DclkVclkSel3:2 ; ///<
    UINT32                                             DclkVclkSel4:2 ; ///<
    UINT32                                             DclkVclkSel5:2 ; ///<
    UINT32                                           Reserved_31_13:19; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70C7_STRUCT;

// **** FCRxFE00_70A2 Register Definition ****
// Address
#define FCRxFE00_70A2_ADDRESS                                   0xfe0070a2

// Type
#define FCRxFE00_70A2_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70A2_Reserved_6_0_OFFSET                       0
#define FCRxFE00_70A2_Reserved_6_0_WIDTH                        7
#define FCRxFE00_70A2_Reserved_6_0_MASK                         0x7f
#define FCRxFE00_70A2_PPlayTableRev_OFFSET                      7
#define FCRxFE00_70A2_PPlayTableRev_WIDTH                       4
#define FCRxFE00_70A2_PPlayTableRev_MASK                        0x780
#define FCRxFE00_70A2_SclkThermDid_OFFSET                       11
#define FCRxFE00_70A2_SclkThermDid_WIDTH                        7
#define FCRxFE00_70A2_SclkThermDid_MASK                         0x3f800
#define FCRxFE00_70A2_PcieGen2Vid_OFFSET                        18
#define FCRxFE00_70A2_PcieGen2Vid_WIDTH                         2
#define FCRxFE00_70A2_PcieGen2Vid_MASK                          0xc0000
#define FCRxFE00_70A2_Reserved_31_20_OFFSET                     20
#define FCRxFE00_70A2_Reserved_31_20_WIDTH                      12
#define FCRxFE00_70A2_Reserved_31_20_MASK                       0xfff00000

/// FCRxFE00_70A2
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_6_0:7 ; ///<
    UINT32                                            PPlayTableRev:4 ; ///<
    UINT32                                             SclkThermDid:7 ; ///<
    UINT32                                              PcieGen2Vid:2 ; ///<
    UINT32                                           Reserved_31_20:12; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70A2_STRUCT;

// **** FCRxFE00_70AA Register Definition ****
// Address
#define FCRxFE00_70AA_ADDRESS                                   0xfe0070aa

// Type
#define FCRxFE00_70AA_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFE00_70AA_Reserved_0_0_OFFSET                       0
#define FCRxFE00_70AA_Reserved_0_0_WIDTH                        1
#define FCRxFE00_70AA_Reserved_0_0_MASK                         0x1
#define FCRxFE00_70AA_SclkDpmCacBase_OFFSET                     1
#define FCRxFE00_70AA_SclkDpmCacBase_WIDTH                      8
#define FCRxFE00_70AA_SclkDpmCacBase_MASK                       0x1fe
#define FCRxFE00_70AA_Reserved_31_9_OFFSET                      9
#define FCRxFE00_70AA_Reserved_31_9_WIDTH                       23
#define FCRxFE00_70AA_Reserved_31_9_MASK                        0xfffffe00

/// FCRxFE00_70AA
typedef union {
  struct {                                                              ///<
    UINT32                                             Reserved_0_0:1 ; ///<
    UINT32                                           SclkDpmCacBase:8 ; ///<
    UINT32                                            Reserved_31_9:23; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFE00_70AA_STRUCT;

// **** D18F3xD4 Register Definition ****
// Address
#define D18F3xD4_ADDRESS                                        0xd4

// Type
#define D18F3xD4_TYPE                                           TYPE_D18F3
// Field Data
#define D18F3xD4_MainPllOpFreqId_OFFSET                         0
#define D18F3xD4_MainPllOpFreqId_WIDTH                          6
#define D18F3xD4_MainPllOpFreqId_MASK                           0x3f
#define D18F3xD4_MainPllOpFreqIdEn_OFFSET                       6
#define D18F3xD4_MainPllOpFreqIdEn_WIDTH                        1
#define D18F3xD4_MainPllOpFreqIdEn_MASK                         0x40
#define D18F3xD4_Reserved_7_7_OFFSET                            7
#define D18F3xD4_Reserved_7_7_WIDTH                             1
#define D18F3xD4_Reserved_7_7_MASK                              0x80
#define D18F3xD4_ClkRampHystSel_OFFSET                          8
#define D18F3xD4_ClkRampHystSel_WIDTH                           4
#define D18F3xD4_ClkRampHystSel_MASK                            0xf00
#define D18F3xD4_OnionOutHyst_OFFSET                            12
#define D18F3xD4_OnionOutHyst_WIDTH                             4
#define D18F3xD4_OnionOutHyst_MASK                              0xf000
#define D18F3xD4_DisNclkGatingIdle_OFFSET                       16
#define D18F3xD4_DisNclkGatingIdle_WIDTH                        1
#define D18F3xD4_DisNclkGatingIdle_MASK                         0x10000
#define D18F3xD4_ClockGatingEnDram_OFFSET                       17
#define D18F3xD4_ClockGatingEnDram_WIDTH                        1
#define D18F3xD4_ClockGatingEnDram_MASK                         0x20000
#define D18F3xD4_Reserved_31_18_OFFSET                          18
#define D18F3xD4_Reserved_31_18_WIDTH                           14
#define D18F3xD4_Reserved_31_18_MASK                            0xfffc0000

/// D18F3xD4
typedef union {
  struct {                                                              ///<
    UINT32                                          MainPllOpFreqId:6 ; ///<
    UINT32                                        MainPllOpFreqIdEn:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                           ClkRampHystSel:4 ; ///<
    UINT32                                             OnionOutHyst:4 ; ///<
    UINT32                                        DisNclkGatingIdle:1 ; ///<
    UINT32                                        ClockGatingEnDram:1 ; ///<
    UINT32                                           Reserved_31_18:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D18F3xD4_STRUCT;

// **** FCRxFF30_01F4 Register Definition ****
// Address
#define FCRxFF30_01F4_ADDRESS                                   0xff3001f4

// Type
#define FCRxFF30_01F4_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFF30_01F4_ReservedCgttSclk_21_0_OFFSET              0
#define FCRxFF30_01F4_ReservedCgttSclk_21_0_WIDTH               21
#define FCRxFF30_01F4_ReservedCgttSclk_21_0_MASK                0x3fffff
#define FCRxFF30_01F4_CgBifCgttSclkOverride_OFFSET              22
#define FCRxFF30_01F4_CgBifCgttSclkOverride_WIDTH               1
#define FCRxFF30_01F4_CgBifCgttSclkOverride_MASK                0x400000
#define FCRxFF30_01F4_ReservedCgttSclk_24_23_OFFSET             23
#define FCRxFF30_01F4_ReservedCgttSclk_24_23_WIDTH              2
#define FCRxFF30_01F4_ReservedCgttSclk_24_23_MASK               0x1800000
#define FCRxFF30_01F4_CgDcCgttSclkOverride_OFFSET               25
#define FCRxFF30_01F4_CgDcCgttSclkOverride_WIDTH                1
#define FCRxFF30_01F4_CgDcCgttSclkOverride_MASK                 0x2000000
#define FCRxFF30_01F4_ReservedCgttSclk_26_26_OFFSET             26
#define FCRxFF30_01F4_ReservedCgttSclk_26_26_WIDTH              1
#define FCRxFF30_01F4_ReservedCgttSclk_26_26_MASK               0x4000000
#define FCRxFF30_01F4_CgMcbCgttSclkOverride_OFFSET              27
#define FCRxFF30_01F4_CgMcbCgttSclkOverride_WIDTH               1
#define FCRxFF30_01F4_CgMcbCgttSclkOverride_MASK                0x8000000
#define FCRxFF30_01F4_CgMcdwCgttSclkOverride_OFFSET             28
#define FCRxFF30_01F4_CgMcdwCgttSclkOverride_WIDTH              1
#define FCRxFF30_01F4_CgMcdwCgttSclkOverride_MASK               0x10000000
#define FCRxFF30_01F4_ReservedCgttSclk_31_29_OFFSET             29
#define FCRxFF30_01F4_ReservedCgttSclk_31_29_WIDTH              3
#define FCRxFF30_01F4_ReservedCgttSclk_31_29_MASK               0xe0000000

/// FCRxFF30_01F4
typedef union {
  struct {                                                              ///<
    UINT32                                    ReservedCgttSclk_21_0:22; ///<
    UINT32                                    CgBifCgttSclkOverride:1 ; ///<
    UINT32                                   ReservedCgttSclk_24_23:2 ; ///<
    UINT32                                     CgDcCgttSclkOverride:1 ; ///<
    UINT32                                   ReservedCgttSclk_26_26:1 ; ///<
    UINT32                                    CgMcbCgttSclkOverride:1 ; ///<
    UINT32                                   CgMcdwCgttSclkOverride:1 ; ///<
    UINT32                                   ReservedCgttSclk_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_01F4_STRUCT;

// **** FCRxFF30_01F5 Register Definition ****
// Address
#define FCRxFF30_01F5_ADDRESS                                   0xff3001f5

// Type
#define FCRxFF30_01F5_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFF30_01F5_ReservedCgttSclk_10_0_OFFSET              0
#define FCRxFF30_01F5_ReservedCgttSclk_10_0_WIDTH               11
#define FCRxFF30_01F5_ReservedCgttSclk_10_0_MASK                0x7ff
#define FCRxFF30_01F5_CgVmcCgttSclkOverride_OFFSET              11
#define FCRxFF30_01F5_CgVmcCgttSclkOverride_WIDTH               1
#define FCRxFF30_01F5_CgVmcCgttSclkOverride_MASK                0x800
#define FCRxFF30_01F5_CgOrbCgttSclkOverride_OFFSET              12
#define FCRxFF30_01F5_CgOrbCgttSclkOverride_WIDTH               1
#define FCRxFF30_01F5_CgOrbCgttSclkOverride_MASK                0x1000
#define FCRxFF30_01F5_CgOrbCgttLclkOverride_OFFSET              13
#define FCRxFF30_01F5_CgOrbCgttLclkOverride_WIDTH               1
#define FCRxFF30_01F5_CgOrbCgttLclkOverride_MASK                0x2000
#define FCRxFF30_01F5_CgIocCgttSclkOverride_OFFSET              14
#define FCRxFF30_01F5_CgIocCgttSclkOverride_WIDTH               1
#define FCRxFF30_01F5_CgIocCgttSclkOverride_MASK                0x4000
#define FCRxFF30_01F5_CgIocCgttLclkOverride_OFFSET              15
#define FCRxFF30_01F5_CgIocCgttLclkOverride_WIDTH               1
#define FCRxFF30_01F5_CgIocCgttLclkOverride_MASK                0x8000
#define FCRxFF30_01F5_ReservedCgttSclk_27_16_OFFSET             16
#define FCRxFF30_01F5_ReservedCgttSclk_27_16_WIDTH              12
#define FCRxFF30_01F5_ReservedCgttSclk_27_16_MASK               0xfff0000
#define FCRxFF30_01F5_CgDcCgttDispClkOverride_OFFSET            28
#define FCRxFF30_01F5_CgDcCgttDispClkOverride_WIDTH             1
#define FCRxFF30_01F5_CgDcCgttDispClkOverride_MASK              0x10000000
#define FCRxFF30_01F5_ReservedCgttSclk_31_29_OFFSET             29
#define FCRxFF30_01F5_ReservedCgttSclk_31_29_WIDTH              3
#define FCRxFF30_01F5_ReservedCgttSclk_31_29_MASK               0xe0000000

/// FCRxFF30_01F5
typedef union {
  struct {                                                              ///<
    UINT32                                    ReservedCgttSclk_10_0:11; ///<
    UINT32                                    CgVmcCgttSclkOverride:1 ; ///<
    UINT32                                    CgOrbCgttSclkOverride:1 ; ///<
    UINT32                                    CgOrbCgttLclkOverride:1 ; ///<
    UINT32                                    CgIocCgttSclkOverride:1 ; ///<
    UINT32                                    CgIocCgttLclkOverride:1 ; ///<
    UINT32                                   ReservedCgttSclk_27_16:12; ///<
    UINT32                                  CgDcCgttDispClkOverride:1 ; ///<
    UINT32                                   ReservedCgttSclk_31_29:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_01F5_STRUCT;

// **** FCRxFF30_1512 Register Definition ****
// Address
#define FCRxFF30_1512_ADDRESS                                   0xff301512

// Type
#define FCRxFF30_1512_TYPE                                      TYPE_FCR
// Field Data
#define FCRxFF30_1512_Reserved_30_0_OFFSET                      0
#define FCRxFF30_1512_Reserved_30_0_WIDTH                       31
#define FCRxFF30_1512_Reserved_30_0_MASK                        0x7fffffff
#define FCRxFF30_1512_SoftOverride0_OFFSET                      31
#define FCRxFF30_1512_SoftOverride0_WIDTH                       1
#define FCRxFF30_1512_SoftOverride0_MASK                        0x80000000

/// FCRxFF30_1512
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_30_0:31; ///<
    UINT32                                            SoftOverride0:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} FCRxFF30_1512_STRUCT;

// **** SMUx1B Register Definition ****
// Address
#define SMUx1B_ADDRESS                                          0x1b

// Type
#define SMUx1B_TYPE                                             TYPE_SMU
// Field Data
#define SMUx1B_LclkDpSlpDiv_OFFSET                              0
#define SMUx1B_LclkDpSlpDiv_WIDTH                               3
#define SMUx1B_LclkDpSlpDiv_MASK                                0x7
#define SMUx1B_RampDis_OFFSET                                   3
#define SMUx1B_RampDis_WIDTH                                    1
#define SMUx1B_RampDis_MASK                                     0x8
#define SMUx1B_Reserved_7_4_OFFSET                              4
#define SMUx1B_Reserved_7_4_WIDTH                               4
#define SMUx1B_Reserved_7_4_MASK                                0xf0
#define SMUx1B_LclkDpSlpMask_OFFSET                             8
#define SMUx1B_LclkDpSlpMask_WIDTH                              8
#define SMUx1B_LclkDpSlpMask_MASK                               0xff00

/// SMUx1B
typedef union {
  struct {                                                              ///<
    UINT32                                             LclkDpSlpDiv:3 ; ///<
    UINT32                                                  RampDis:1 ; ///<
    UINT32                                             Reserved_7_4:4 ; ///<
    UINT32                                            LclkDpSlpMask:8 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx1B_STRUCT;

// **** SMUx1D Register Definition ****
// Address
#define SMUx1D_ADDRESS                                          0x1d

// Type
#define SMUx1D_TYPE                                             TYPE_SMU
// Field Data
#define SMUx1D_LclkDpSlpHyst_OFFSET                             0
#define SMUx1D_LclkDpSlpHyst_WIDTH                              12
#define SMUx1D_LclkDpSlpHyst_MASK                               0xfff
#define SMUx1D_LclkDpSlpEn_OFFSET                               12
#define SMUx1D_LclkDpSlpEn_WIDTH                                1
#define SMUx1D_LclkDpSlpEn_MASK                                 0x1000
#define SMUx1D_Reserved_15_13_OFFSET                            13
#define SMUx1D_Reserved_15_13_WIDTH                             3
#define SMUx1D_Reserved_15_13_MASK                              0xe000

/// SMUx1D
typedef union {
  struct {                                                              ///<
    UINT32                                            LclkDpSlpHyst:12; ///<
    UINT32                                              LclkDpSlpEn:1 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx1D_STRUCT;

// **** SMUx6F Register Definition ****
// Address
#define SMUx6F_ADDRESS                                          0x6f


// **** SMUx71 Register Definition ****
// Address
#define SMUx71_ADDRESS                                          0x71


// **** SMUx73 Register Definition ****
// Address
#define SMUx73_ADDRESS                                          0x73

// Type
#define SMUx73_TYPE                                             TYPE_SMU
// Field Data
#define SMUx73_DisLclkGating_OFFSET                             0
#define SMUx73_DisLclkGating_WIDTH                              1
#define SMUx73_DisLclkGating_MASK                               0x1
#define SMUx73_DisSclkGating_OFFSET                             1
#define SMUx73_DisSclkGating_WIDTH                              1
#define SMUx73_DisSclkGating_MASK                               0x2
#define SMUx73_Reserved_15_2_OFFSET                             2
#define SMUx73_Reserved_15_2_WIDTH                              14
#define SMUx73_Reserved_15_2_MASK                               0xfffc

/// SMUx73
typedef union {
  struct {                                                              ///<
    UINT32                                            DisLclkGating:1 ; ///<
    UINT32                                            DisSclkGating:1 ; ///<
    UINT32                                            Reserved_15_2:14; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} SMUx73_STRUCT;

// **** D0F0x98_x49 Register Definition ****
// Address
#define D0F0x98_x49_ADDRESS                                     0x49

// Type
#define D0F0x98_x49_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x49_Reserved_23_0_OFFSET                        0
#define D0F0x98_x49_Reserved_23_0_WIDTH                         24
#define D0F0x98_x49_Reserved_23_0_MASK                          0xffffff
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
    UINT32                                            Reserved_23_0:24; ///<
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
#define D0F0x98_x4A_Reserved_23_0_OFFSET                        0
#define D0F0x98_x4A_Reserved_23_0_WIDTH                         24
#define D0F0x98_x4A_Reserved_23_0_MASK                          0xffffff
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
    UINT32                                            Reserved_23_0:24; ///<
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

// **** D0F0x98_x4B Register Definition ****
// Address
#define D0F0x98_x4B_ADDRESS                                     0x4b

// Type
#define D0F0x98_x4B_TYPE                                        TYPE_D0F0x98
// Field Data
#define D0F0x98_x4B_Reserved_29_0_OFFSET                        0
#define D0F0x98_x4B_Reserved_29_0_WIDTH                         30
#define D0F0x98_x4B_Reserved_29_0_MASK                          0x3fffffff
#define D0F0x98_x4B_SoftOverrideClk_OFFSET                      30
#define D0F0x98_x4B_SoftOverrideClk_WIDTH                       1
#define D0F0x98_x4B_SoftOverrideClk_MASK                        0x40000000
#define D0F0x98_x4B_Reserved_31_31_OFFSET                       31
#define D0F0x98_x4B_Reserved_31_31_WIDTH                        1
#define D0F0x98_x4B_Reserved_31_31_MASK                         0x80000000

/// D0F0x98_x4B
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_29_0:30; ///<
    UINT32                                          SoftOverrideClk:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x98_x4B_STRUCT;

// **** D0F0x64_x22 Register Definition ****
// Address
#define D0F0x64_x22_ADDRESS                                     0x22

// Type
#define D0F0x64_x22_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x22_Reserved_25_0_OFFSET                        0
#define D0F0x64_x22_Reserved_25_0_WIDTH                         26
#define D0F0x64_x22_Reserved_25_0_MASK                          0x3ffffff
#define D0F0x64_x22_SoftOverrideClk4_OFFSET                     26
#define D0F0x64_x22_SoftOverrideClk4_WIDTH                      1
#define D0F0x64_x22_SoftOverrideClk4_MASK                       0x4000000
#define D0F0x64_x22_SoftOverrideClk3_OFFSET                     27
#define D0F0x64_x22_SoftOverrideClk3_WIDTH                      1
#define D0F0x64_x22_SoftOverrideClk3_MASK                       0x8000000
#define D0F0x64_x22_SoftOverrideClk2_OFFSET                     28
#define D0F0x64_x22_SoftOverrideClk2_WIDTH                      1
#define D0F0x64_x22_SoftOverrideClk2_MASK                       0x10000000
#define D0F0x64_x22_SoftOverrideClk1_OFFSET                     29
#define D0F0x64_x22_SoftOverrideClk1_WIDTH                      1
#define D0F0x64_x22_SoftOverrideClk1_MASK                       0x20000000
#define D0F0x64_x22_SoftOverrideClk0_OFFSET                     30
#define D0F0x64_x22_SoftOverrideClk0_WIDTH                      1
#define D0F0x64_x22_SoftOverrideClk0_MASK                       0x40000000
#define D0F0x64_x22_Reserved_31_31_OFFSET                       31
#define D0F0x64_x22_Reserved_31_31_WIDTH                        1
#define D0F0x64_x22_Reserved_31_31_MASK                         0x80000000

/// D0F0x64_x22
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_25_0:26; ///<
    UINT32                                         SoftOverrideClk4:1 ; ///<
    UINT32                                         SoftOverrideClk3:1 ; ///<
    UINT32                                         SoftOverrideClk2:1 ; ///<
    UINT32                                         SoftOverrideClk1:1 ; ///<
    UINT32                                         SoftOverrideClk0:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x22_STRUCT;

// **** D0F0x64_x23 Register Definition ****
// Address
#define D0F0x64_x23_ADDRESS                                     0x23

// Type
#define D0F0x64_x23_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x23_Reserved_26_0_OFFSET                        0
#define D0F0x64_x23_Reserved_26_0_WIDTH                         27
#define D0F0x64_x23_Reserved_26_0_MASK                          0x7ffffff
#define D0F0x64_x23_SoftOverrideClk3_OFFSET                     27
#define D0F0x64_x23_SoftOverrideClk3_WIDTH                      1
#define D0F0x64_x23_SoftOverrideClk3_MASK                       0x8000000
#define D0F0x64_x23_SoftOverrideClk2_OFFSET                     28
#define D0F0x64_x23_SoftOverrideClk2_WIDTH                      1
#define D0F0x64_x23_SoftOverrideClk2_MASK                       0x10000000
#define D0F0x64_x23_SoftOverrideClk1_OFFSET                     29
#define D0F0x64_x23_SoftOverrideClk1_WIDTH                      1
#define D0F0x64_x23_SoftOverrideClk1_MASK                       0x20000000
#define D0F0x64_x23_SoftOverrideClk0_OFFSET                     30
#define D0F0x64_x23_SoftOverrideClk0_WIDTH                      1
#define D0F0x64_x23_SoftOverrideClk0_MASK                       0x40000000
#define D0F0x64_x23_Reserved_31_31_OFFSET                       31
#define D0F0x64_x23_Reserved_31_31_WIDTH                        1
#define D0F0x64_x23_Reserved_31_31_MASK                         0x80000000

/// D0F0x64_x23
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_26_0:27; ///<
    UINT32                                         SoftOverrideClk3:1 ; ///<
    UINT32                                         SoftOverrideClk2:1 ; ///<
    UINT32                                         SoftOverrideClk1:1 ; ///<
    UINT32                                         SoftOverrideClk0:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x23_STRUCT;

// **** D0F0x64_x24 Register Definition ****
// Address
#define D0F0x64_x24_ADDRESS                                     0x24

// Type
#define D0F0x64_x24_TYPE                                        TYPE_D0F0x64
// Field Data
#define D0F0x64_x24_Reserved_28_0_OFFSET                       0
#define D0F0x64_x24_Reserved_28_0_WIDTH                        29
#define D0F0x64_x24_Reserved_28_0_MASK                         0x1fffffff
#define D0F0x64_x24_SoftOverrideClk1_OFFSET                     29
#define D0F0x64_x24_SoftOverrideClk1_WIDTH                      1
#define D0F0x64_x24_SoftOverrideClk1_MASK                       0x20000000
#define D0F0x64_x24_SoftOverrideClk0_OFFSET                     30
#define D0F0x64_x24_SoftOverrideClk0_WIDTH                      1
#define D0F0x64_x24_SoftOverrideClk0_MASK                       0x40000000
#define D0F0x64_x24_Reserved_31_31_OFFSET                       31
#define D0F0x64_x24_Reserved_31_31_WIDTH                        1
#define D0F0x64_x24_Reserved_31_31_MASK                         0x80000000

/// D0F0x64_x24
typedef union {
  struct {                                                              ///<
    UINT32                                            Reserved_28_0:29; ///<
    UINT32                                         SoftOverrideClk1:1 ; ///<
    UINT32                                         SoftOverrideClk0:1 ; ///<
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x24_STRUCT;

#endif
