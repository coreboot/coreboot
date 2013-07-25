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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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

#ifndef _GNBREGISTERSCOMMON_H_
#define _GNBREGISTERSCOMMON_H_


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
#define  D18F2x9C  0xe
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
#define D0F0x04_Reserved_7_7_OFFSET                             7
#define D0F0x04_Reserved_7_7_WIDTH                              1
#define D0F0x04_Reserved_7_7_MASK                               0x80
#define D0F0x04_Reserved_19_10_OFFSET                           10
#define D0F0x04_Reserved_19_10_WIDTH                            10
#define D0F0x04_Reserved_19_10_MASK                             0xffc00
#define D0F0x04_CapList_OFFSET                                  20
#define D0F0x04_CapList_WIDTH                                   1
#define D0F0x04_CapList_MASK                                    0x100000

/// D0F0x04
typedef union {
  struct {                                                              ///<
    UINT32                                               IoAccessEn:1 ; ///<
    UINT32                                              MemAccessEn:1 ; ///<
    UINT32                                              BusMasterEn:1 ; ///<
    UINT32                                             Reserved_3_3:1 ; ///<
    UINT32                                             Reserved_4_4:1 ; ///<
    UINT32                                             Reserved_5_5:1 ; ///<
    UINT32                                             Reserved_6_6:1 ; ///<
    UINT32                                             Reserved_7_7:1 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                             Reserved_9_9:1 ; ///<
    UINT32                                           Reserved_19_10:10; ///<
    UINT32                                                  CapList:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x04_STRUCT;

// **** D0F0x60 Register Definition ****
// Address
#define D0F0x60_ADDRESS                                         0x60


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



// **** DxF0x18 Register Definition ****
// Address
#define DxF0x18_ADDRESS                                         0x18

// Type
#define DxF0x18_TYPE                                            TYPE_D4F0
// **** DxF0x58 Register Definition ****
// Address




// **** D0F0x64_x0C Register Definition ****
// Address
#define D0F0x64_x0C_ADDRESS                                     0xc

// Type
#define D0F0x64_x0C_TYPE                                        TYPE_D0F0x64

/// D0F0x64_x0C
typedef union {
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
    UINT32                                             Reserved_1_1:1 ; ///<
    UINT32                                                    VgaEn:1 ; ///<
    UINT32                                             Reserved_2_2:1 ; ///<
    UINT32                                                  Vga16En:1 ; ///<
    UINT32                                            Reserved_31_4:28; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0x64_x1D_STRUCT;


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

// **** D0F0x98_x3A Register Definition ****
// Address
#define D0F0x98_x3A_ADDRESS                                     0x3a

// Type


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
#define D0F0xE4_WRAP_8011_TxclkPermStop_OFFSET                  8
#define D0F0xE4_WRAP_8011_TxclkPermStop_WIDTH                   1
#define D0F0xE4_WRAP_8011_TxclkPermStop_MASK                    0x100
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
#define D0F0xE4_WRAP_8011_Reserved_30_25_OFFSET                 25
#define D0F0xE4_WRAP_8011_Reserved_30_25_WIDTH                  6
#define D0F0xE4_WRAP_8011_Reserved_30_25_MASK                   0x7e000000

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
    UINT32                                           Reserved_31_31:1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8011_STRUCT;

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

/// D0F0xE4_WRAP_8012
typedef union {
  struct {                                                              ///<
    UINT32                                     Pif1xIdleGateLatency:6 ; ///<
    UINT32                                             Reserved_6_6:1 ; ///<
    UINT32                                      Pif1xIdleGateEnable:1 ; ///<
    UINT32                                   Pif1xIdleResumeLatency:6 ; ///<
    UINT32                                           Reserved_15_14:2 ; ///<
    UINT32                                           Reserved_21_16:6 ; ///<
    UINT32                                           Reserved_22_22:1 ; ///<
    UINT32                                           Reserved_23_23:1 ; ///<
    UINT32                                           Reserved_24_29:6 ; ///<
    UINT32                                           Reserved_31_30:2 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8012_STRUCT;


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
// Field Data

// **** D0F0xE4_WRAP_8023 Register Definition ****
// Address
#define D0F0xE4_WRAP_8023_ADDRESS                               0x8023

// Type
#define D0F0xE4_WRAP_8023_TYPE                                  TYPE_D0F0xE4
/// D0F0xE4_WRAP_8023
typedef union {
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8023_STRUCT;

// **** D0F0xE4_WRAP_8025 Register Definition ****
// Address
#define D0F0xE4_WRAP_8025_ADDRESS                               0x8025

// Type
#define D0F0xE4_WRAP_8025_TYPE                                  TYPE_D0F0xE4
/// D0F0xE4_WRAP_8025
typedef union {
  UINT32 Value;                                                         ///<
} D0F0xE4_WRAP_8025_STRUCT;


// **** D0F0xE4_WRAP_8040 Register Definition ****
// Address
#define D0F0xE4_WRAP_8040_ADDRESS                               0x8040

// Type
#define D0F0xE4_WRAP_8040_TYPE                                  TYPE_D0F0xE4


// **** D0F0xE4_WRAP_8060 Register Definition ****
// Address
#define D0F0xE4_WRAP_8060_ADDRESS                               0x8060

// Type
#define D0F0xE4_WRAP_8060_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8060_BifGlobalReset_OFFSET                 16
#define D0F0xE4_WRAP_8060_BifGlobalReset_WIDTH                  1
#define D0F0xE4_WRAP_8060_BifGlobalReset_MASK                   0x10000
#define D0F0xE4_WRAP_8060_BifCalibrationReset_OFFSET            17
#define D0F0xE4_WRAP_8060_BifCalibrationReset_WIDTH             1
#define D0F0xE4_WRAP_8060_BifCalibrationReset_MASK              0x20000
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
    UINT32                                           BifGlobalReset:1 ; ///<
    UINT32                                      BifCalibrationReset:1 ; ///<
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
    UINT32                                                 b12:1 ; ///<
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








// **** DxF0xE4_x70 Register Definition ****
// Address


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

// **** D0F0x64_x51 Register Definition ****
// Address
#define D0F0x64_x51_ADDRESS                                     0x51

// Type
#define D0F0x64_x51_TYPE                                        TYPE_D0F0x64


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
#define D0F0xE4_CORE_0010_Reserved_8_8_OFFSET                   8
#define D0F0xE4_CORE_0010_Reserved_8_8_WIDTH                    1
#define D0F0xE4_CORE_0010_Reserved_8_8_MASK                     0x100
#define D0F0xE4_CORE_0010_UmiNpMemWrite_OFFSET                  9
#define D0F0xE4_CORE_0010_UmiNpMemWrite_WIDTH                   1
#define D0F0xE4_CORE_0010_UmiNpMemWrite_MASK                    0x200
#define D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_OFFSET            10
#define D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_WIDTH             3
#define D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_MASK              0x1c00
#define D0F0xE4_CORE_0010_Reserved_15_13_OFFSET                 13
#define D0F0xE4_CORE_0010_Reserved_15_13_WIDTH                  3
#define D0F0xE4_CORE_0010_Reserved_15_13_MASK                   0xe000

/// D0F0xE4_CORE_0010
typedef union {
  struct {                                                              ///<
    UINT32                                             HwInitWrLock:1 ; ///<
    UINT32                                          LcHotPlugDelSel:3 ; ///<
    UINT32                                             Reserved_6_4:3 ; ///<
    UINT32                                           :1 ; ///<
    UINT32                                             Reserved_8_8:1 ; ///<
    UINT32                                            UmiNpMemWrite:1 ; ///<
    UINT32                                      RxUmiAdjPayloadSize:3 ; ///<
    UINT32                                           Reserved_15_13:3 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :6 ; ///<
    UINT32                                  :1 ; ///<
    UINT32                                  :1 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xE4_CORE_0010_STRUCT;


typedef union {
  struct {                                                              ///<
    UINT32                                           bit_0_0:1 ; ///<
    UINT32                                           bit1:1 ; ///<
    UINT32                                           bit2:1 ; ///<
    UINT32                                           bit3:1 ; ///<
    UINT32                                           bit4:1 ; ///<
    UINT32                                           bit5:1 ; ///<
    UINT32                                           bit6:1 ; ///<
    UINT32                                           bit_7_7:1 ; ///<
    UINT32                                           bit_9_8:2 ; ///<
    UINT32                                           bit_10_10:1 ; ///<
    UINT32                                           bit11:1 ; ///<
    UINT32                                           bit12:1 ; ///<
    UINT32                                           bit13:1 ; ///<
    UINT32                                           bit14:1 ; ///<
    UINT32                                           bit15:1 ; ///<
    UINT32                                           bit16:1 ; ///<
    UINT32                                           bit_17_17:1 ; ///<
    UINT32                                           bit_19_18:2 ; ///<
    UINT32                                           bit20:1 ; ///<
    UINT32                                           bit_21_21:1 ; ///<
    UINT32                                           bit_23_22:2 ; ///<
    UINT32                                           bit24:1 ; ///<
    UINT32                                           bit_25_25:1 ; ///<
    UINT32                                           bit_27_26:2 ; ///<
    UINT32                                           bit_31_28:4 ; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} GNBREGCOMMON_STR1_STRUCT;




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


// **** D0F0xE4_WRAP_8016 Register Definition ****
// Address
#define D0F0xE4_WRAP_8016_ADDRESS                               0x8016

// Type
#define D0F0xE4_WRAP_8016_TYPE                                  TYPE_D0F0xE4
// Field Data
#define D0F0xE4_WRAP_8016_CalibAckLatency_OFFSET                0
#define D0F0xE4_WRAP_8016_CalibAckLatency_WIDTH                 6
#define D0F0xE4_WRAP_8016_CalibAckLatency_MASK                  0x3f
#define D0F0xE4_WRAP_8016_Reserved_7_6_OFFSET                   6
#define D0F0xE4_WRAP_8016_Reserved_7_6_WIDTH                    2
#define D0F0xE4_WRAP_8016_Reserved_7_6_MASK                     0xc0
#define D0F0xE4_WRAP_8016_CalibDoneSelPifA_OFFSET               8
#define D0F0xE4_WRAP_8016_CalibDoneSelPifA_WIDTH                1
#define D0F0xE4_WRAP_8016_CalibDoneSelPifA_MASK                 0x100
#define D0F0xE4_WRAP_8016_Reserved_9_9_OFFSET                   9
#define D0F0xE4_WRAP_8016_Reserved_9_9_WIDTH                    1
#define D0F0xE4_WRAP_8016_Reserved_9_9_MASK                     0x200
#define D0F0xE4_WRAP_8016_Reserved_10_10_OFFSET                 10
#define D0F0xE4_WRAP_8016_Reserved_10_10_WIDTH                  1
#define D0F0xE4_WRAP_8016_Reserved_10_10_MASK                   0x400
#define D0F0xE4_WRAP_8016_Reserved_11_11_OFFSET                 11
#define D0F0xE4_WRAP_8016_Reserved_11_11_WIDTH                  1
#define D0F0xE4_WRAP_8016_Reserved_11_11_MASK                   0x800
#define D0F0xE4_WRAP_8016_Gen1OnlyEngage_OFFSET                 12
#define D0F0xE4_WRAP_8016_Gen1OnlyEngage_WIDTH                  1
#define D0F0xE4_WRAP_8016_Gen1OnlyEngage_MASK                   0x1000
#define D0F0xE4_WRAP_8016_Gen1OnlyEngaged_OFFSET                13
#define D0F0xE4_WRAP_8016_Gen1OnlyEngaged_WIDTH                 1
#define D0F0xE4_WRAP_8016_Gen1OnlyEngaged_MASK                  0x2000
#define D0F0xE4_WRAP_8016_Reserved_15_14_OFFSET                 14
#define D0F0xE4_WRAP_8016_Reserved_15_14_WIDTH                  2
#define D0F0xE4_WRAP_8016_Reserved_15_14_MASK                   0xc000
#define D0F0xE4_WRAP_8016_LclkDynGateLatency_OFFSET             16
#define D0F0xE4_WRAP_8016_LclkDynGateLatency_WIDTH              6
#define D0F0xE4_WRAP_8016_LclkDynGateLatency_MASK               0x3f0000
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
} D0F0xE4_WRAP_8016_STRUCT;

#endif
