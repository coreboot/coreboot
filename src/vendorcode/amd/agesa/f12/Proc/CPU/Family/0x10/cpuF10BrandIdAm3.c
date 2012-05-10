/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions and structures for socket Am3.
 *
 * Contains code that provides CPU BrandId information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "cpuEarlyInit.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


// PRIVATE FORMAT FOR BRAND TABLE ... FOR AMD USE ONLY

// String1
/*CHAR8 strEngSample[] = "AMD Engineering Sample";
CHAR8 strTtkSample[] = "AMD Thermal Test Kit";
CHAR8 strUnknown[] = "AMD Processor model unknown";
*/
//AM3 NC 0
CONST CHAR8 ROMDATA str_F10_Am3_SC_AthlonLE[] =  "AMD Athlon(tm) Processor LE-";
CONST CHAR8 ROMDATA str_F10_Am3_SC_SempronLE[] = "AMD Sempron(tm) Processor LE-";
CONST CHAR8 ROMDATA str_F10_Am3_SC_Sempron_1[] = "AMD Sempron(tm) 1";
CONST CHAR8 ROMDATA str_F10_Am3_SC_Athlon_1[] = "AMD Athlon(tm) II 1";

//AM3 NC 1
CONST CHAR8 ROMDATA str_F10_Am3_Athlon[] = "AMD Athlon(tm) ";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_XL_V[] = "AMD Athlon(tm) II XL V";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_XLT_V[] = "AMD Athlon(tm) II XLT V";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X2_4[] = "AMD Athlon(tm) II X2 4";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X2_2[] = "AMD Athlon(tm) II X2 2";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X2_B[] = "AMD Athlon(tm) II X2 B";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X2[] = "AMD Athlon(tm) II X2 ";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_Neo_X2[] = "AMD Athlon(tm) II Neo X2 ";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X2_5[] = "AMD Phenom(tm) II X2 5";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X2_5[] = "AMD Athlon(tm) II X2 5";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X2_3[] = "AMD Athlon(tm) II X2 3";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X2[] = "AMD Phenom(tm) II X2 ";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X2_B[] = "AMD Phenom(tm) II X2 B";
CONST CHAR8 ROMDATA str_F10_Am3_DC_Opteron13[] = "Dual-Core AMD Opteron(tm) Processor 13";
CONST CHAR8 ROMDATA str_F10_Am3_Sempron_X2_1[] = "AMD Sempron(tm) X2 1";

//AM3 NC2
CONST CHAR8 ROMDATA str_F10_Am3_Phenom[] = "AMD Phenom(tm) ";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X3_5[] = "AMD Phenom(tm) II X3 5";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X3_4[] = "AMD Phenom(tm) II X3 4";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X3_B[] = "AMD Phenom(tm) II X3 B";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X3[] = "AMD Phenom(tm) II X3 ";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X3_3[] = "AMD Athlon(tm) II X3 3";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_Neo_X3[] = "AMD Athlon(tm) II Neo X3 ";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X3_4[] = "AMD Athlon(tm) II X3 4";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X3_7[] = "AMD Phenom(tm) II X3 7";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X3_B[] = "AMD Athlon(tm) II X3 B";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X3[] = "AMD Athlon(tm) II X3 ";

//AM3 NC 3
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_FX[] =  "AMD Phenom(tm) FX-";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X4_9[] = "AMD Phenom(tm) II X4 9";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X4_8[] = "AMD Phenom(tm) II X4 8";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X4_7[] = "AMD Phenom(tm) II X4 7";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X4_6[] = "AMD Phenom(tm) II X4 6";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X4_B[] = "AMD Phenom(tm) II X4 B";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X4[] = "AMD Phenom(tm) II X4 ";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_Neo_X4[] = "AMD Phenom(tm) II Neo X4 ";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X4_6[] = "AMD Athlon(tm) II X4 6";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X4_5[] = "AMD Athlon(tm) II X4 5";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_Neo_X4[] = "AMD Athlon(tm) II Neo X4 ";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X4_B[] = "AMD Athlon(tm) II X4 B";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II__FX[] = "AMD Phenom(tm) II FX-";
CONST CHAR8 ROMDATA str_F10_Am3_Athlon_II_X4[] = "AMD Athlon(tm) II X4 ";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II[] = "AMD Phenom(tm) II ";
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_XLT_Q[] = "AMD Phenom(tm) II XLT Q";
CONST CHAR8 ROMDATA str_F10_Am3_QC_Opteron13[] = "Quad-Core AMD Opteron(tm) Processor 13";

//AM3 NC 5
CONST CHAR8 ROMDATA str_F10_Am3_Phenom_II_X6_1[] =  "AMD Phenom(tm) II X6 1";

// String2
CONST CHAR8 ROMDATA str2_F10_Am3_SE[] = " SE";
CONST CHAR8 ROMDATA str2_F10_Am3_HE[] = " HE";
CONST CHAR8 ROMDATA str2_F10_Am3_EE[] = " EE";

CONST CHAR8 ROMDATA str2_F10_Am3_QCP[] = " Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_00[] = "00";
CONST CHAR8 ROMDATA str2_F10_Am3_10[] = "10";
CONST CHAR8 ROMDATA str2_F10_Am3_20[] = "20";
CONST CHAR8 ROMDATA str2_F10_Am3_30[] = "30";
CONST CHAR8 ROMDATA str2_F10_Am3_40[] = "40";
CONST CHAR8 ROMDATA str2_F10_Am3_50[] = "50";
CONST CHAR8 ROMDATA str2_F10_Am3_60[] = "60";
CONST CHAR8 ROMDATA str2_F10_Am3_70[] = "70";
CONST CHAR8 ROMDATA str2_F10_Am3_80[] = "80";
CONST CHAR8 ROMDATA str2_F10_Am3_90[] = "90";
CONST CHAR8 ROMDATA str2_F10_Am3_DC_00[] = "00 Dual-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_DC_00e[] = "00e Dual-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_DC_00B[] = "00B Dual-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_DC_50[] = "50 Dual-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_DC_50e[] = "50e Dual-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_DC_50B[] = "50B Dual-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_Processor[] = " Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_e_Processor[] = "e Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_B_Processor[] = "B Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_0e_Processor[] = "0e Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_u_Processor[] = "u Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_0_Processor[] = "0 Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_L_Processor[] = "L Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_C_Processor[] = "C Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_TWKR_Black_Edition[] = " TWKR Black Edition";

CONST CHAR8 ROMDATA str2_F10_Am3_TC_00[] = "00 Triple-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_TC_00e[] = "00e Triple-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_TC_00B[] = "00B Triple-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_TC_50[] = "50 Triple-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_TC_50e[] = "50e Triple-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_TC_50B[] = "50B Triple-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_00[] = "00 Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_00e[] = "00e Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_00B[] = "00B Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_50[] = "50 Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_50e[] = "50e Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_50B[] = "50B Quad-Core Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_QC_T[] = "T Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_SC_0T[] = "0T Processor";
CONST CHAR8 ROMDATA str2_F10_Am3_SC_5T[] = "5T Processor";

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString1ArrayAm3[] =
{
  // AM3
  {1, 0, 0, DR_SOCKET_AM3, str_F10_Am3_SC_AthlonLE, sizeof (str_F10_Am3_SC_AthlonLE)},
  {1, 0, 1, DR_SOCKET_AM3, str_F10_Am3_SC_SempronLE, sizeof (str_F10_Am3_SC_SempronLE)},
  {1, 0, 2, DR_SOCKET_AM3, str_F10_Am3_SC_Sempron_1, sizeof (str_F10_Am3_SC_Sempron_1)},
  {1, 0, 3, DR_SOCKET_AM3, str_F10_Am3_SC_Athlon_1, sizeof (str_F10_Am3_SC_Athlon_1)},

  {2, 0, 0, DR_SOCKET_AM3, str_F10_Am3_DC_Opteron13, sizeof (str_F10_Am3_DC_Opteron13)},
  {2, 0, 1, DR_SOCKET_AM3, str_F10_Am3_Athlon, sizeof (str_F10_Am3_Athlon)},
  {2, 0, 2, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X2_4, sizeof (str_F10_Am3_Athlon_II_X2_4)},
  {2, 0, 3, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X2_2, sizeof (str_F10_Am3_Athlon_II_X2_2)},
  {2, 0, 4, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X2_B, sizeof (str_F10_Am3_Athlon_II_X2_B)},
  {2, 0, 5, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X2, sizeof (str_F10_Am3_Athlon_II_X2)},
  {2, 0, 6, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_Neo_X2, sizeof (str_F10_Am3_Athlon_II_Neo_X2)},
  {2, 0, 7, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X2_5, sizeof (str_F10_Am3_Phenom_II_X2_5)},
  {2, 0, 8, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X2_5, sizeof (str_F10_Am3_Athlon_II_X2_5)},
  {2, 0, 9, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X2_3, sizeof (str_F10_Am3_Athlon_II_X2_3)},
  {2, 0, 10, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X2, sizeof (str_F10_Am3_Phenom_II_X2)},
  {2, 0, 11, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X2_B, sizeof (str_F10_Am3_Phenom_II_X2_B)},
  {2, 0, 12, DR_SOCKET_AM3, str_F10_Am3_Sempron_X2_1, sizeof (str_F10_Am3_Sempron_X2_1)},
  {2, 1, 1, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_XLT_V, sizeof (str_F10_Am3_Athlon_II_XLT_V)},
  {2, 1, 2, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_XL_V, sizeof (str_F10_Am3_Athlon_II_XL_V)},

  {3, 0, 0, DR_SOCKET_AM3, str_F10_Am3_Phenom, sizeof (str_F10_Am3_Phenom)},
  {3, 0, 1, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X3_5, sizeof (str_F10_Am3_Phenom_II_X3_5)},
  {3, 0, 2, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X3_4, sizeof (str_F10_Am3_Phenom_II_X3_4)},
  {3, 0, 3, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X3_B, sizeof (str_F10_Am3_Phenom_II_X3_B)},
  {3, 0, 4, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X3, sizeof (str_F10_Am3_Phenom_II_X3)},
  {3, 0, 5, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X3_3, sizeof (str_F10_Am3_Athlon_II_X3_3)},
  {3, 0, 6, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_Neo_X3, sizeof (str_F10_Am3_Athlon_II_Neo_X3)},
  {3, 0, 7, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X3_4, sizeof (str_F10_Am3_Athlon_II_X3_4)},
  {3, 0, 8, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X3_7, sizeof (str_F10_Am3_Phenom_II_X3_7)},
  {3, 0, 9, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X3_B, sizeof (str_F10_Am3_Athlon_II_X3_B)},
  {3, 0, 10, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X3, sizeof (str_F10_Am3_Athlon_II_X3)},

  {4, 0, 0, DR_SOCKET_AM3, str_F10_Am3_QC_Opteron13, sizeof (str_F10_Am3_QC_Opteron13)},
  {4, 0, 1, DR_SOCKET_AM3, str_F10_Am3_Phenom_FX, sizeof (str_F10_Am3_Phenom_FX)},
  {4, 0, 2, DR_SOCKET_AM3, str_F10_Am3_Phenom, sizeof (str_F10_Am3_Phenom)},
  {4, 0, 3, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_9, sizeof (str_F10_Am3_Phenom_II_X4_9)},
  {4, 0, 4, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_8, sizeof (str_F10_Am3_Phenom_II_X4_8)},
  {4, 0, 5, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_7, sizeof (str_F10_Am3_Phenom_II_X4_7)},
  {4, 0, 6, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_6, sizeof (str_F10_Am3_Phenom_II_X4_6)},
  {4, 0, 7, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_B, sizeof (str_F10_Am3_Phenom_II_X4_B)},
  {4, 0, 8, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4, sizeof (str_F10_Am3_Phenom_II_X4)},
  {4, 0, 9, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_Neo_X4, sizeof (str_F10_Am3_Phenom_II_Neo_X4)},
  {4, 0, 10, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X4_6, sizeof (str_F10_Am3_Athlon_II_X4_6)},
  {4, 0, 11, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X4_5, sizeof (str_F10_Am3_Athlon_II_X4_5)},
  {4, 0, 12, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_Neo_X4, sizeof (str_F10_Am3_Athlon_II_Neo_X4)},
  {4, 0, 13, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X4_B, sizeof (str_F10_Am3_Athlon_II_X4_B)},
  {4, 0, 14, DR_SOCKET_AM3, str_F10_Am3_Phenom_II__FX, sizeof (str_F10_Am3_Phenom_II__FX)},
  {4, 0, 15, DR_SOCKET_AM3, str_F10_Am3_Athlon_II_X4, sizeof (str_F10_Am3_Athlon_II_X4)},
  {4, 1, 0, DR_SOCKET_AM3, str_F10_Am3_Phenom_II, sizeof (str_F10_Am3_Phenom_II)},
  {4, 1, 1, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_XLT_Q, sizeof (str_F10_Am3_Phenom_II_XLT_Q)},
  {4, 1, 2, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_9, sizeof (str_F10_Am3_Phenom_II_X4_9)},
  {4, 1, 3, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_8, sizeof (str_F10_Am3_Phenom_II_X4_8)},
  {4, 1, 4, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X4_6, sizeof (str_F10_Am3_Phenom_II_X4_6)},

  {6, 0, 0, DR_SOCKET_AM3, str_F10_Am3_Phenom_II_X6_1, sizeof (str_F10_Am3_Phenom_II_X6_1)}
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString2ArrayAm3[] =
{
  // AM3
  {1, 0, 0x00, DR_SOCKET_AM3, str2_F10_Am3_00, sizeof (str2_F10_Am3_00)},
  {1, 0, 0x01, DR_SOCKET_AM3, str2_F10_Am3_10, sizeof (str2_F10_Am3_10)},
  {1, 0, 0x02, DR_SOCKET_AM3, str2_F10_Am3_20, sizeof (str2_F10_Am3_20)},
  {1, 0, 0x03, DR_SOCKET_AM3, str2_F10_Am3_30, sizeof (str2_F10_Am3_30)},
  {1, 0, 0x04, DR_SOCKET_AM3, str2_F10_Am3_40, sizeof (str2_F10_Am3_40)},
  {1, 0, 0x05, DR_SOCKET_AM3, str2_F10_Am3_50, sizeof (str2_F10_Am3_50)},
  {1, 0, 0x06, DR_SOCKET_AM3, str2_F10_Am3_60, sizeof (str2_F10_Am3_60)},
  {1, 0, 0x07, DR_SOCKET_AM3, str2_F10_Am3_70, sizeof (str2_F10_Am3_70)},
  {1, 0, 0x08, DR_SOCKET_AM3, str2_F10_Am3_80, sizeof (str2_F10_Am3_80)},
  {1, 0, 0x09, DR_SOCKET_AM3, str2_F10_Am3_90, sizeof (str2_F10_Am3_90)},
  {1, 0, 0x0A, DR_SOCKET_AM3, str2_F10_Am3_Processor, sizeof (str2_F10_Am3_Processor)},
  {1, 0, 0x0B, DR_SOCKET_AM3, str2_F10_Am3_u_Processor, sizeof (str2_F10_Am3_u_Processor)},
  {1, 0, 0x0F, DR_SOCKET_AM3, 0, 0},    //Size 0 for no suffix
  {2, 0, 0x00, DR_SOCKET_AM3, str2_F10_Am3_DC_00, sizeof (str2_F10_Am3_DC_00)},
  {2, 0, 0x01, DR_SOCKET_AM3, str2_F10_Am3_DC_00e, sizeof (str2_F10_Am3_DC_00e)},
  {2, 0, 0x02, DR_SOCKET_AM3, str2_F10_Am3_DC_00B, sizeof (str2_F10_Am3_DC_00B)},
  {2, 0, 0x03, DR_SOCKET_AM3, str2_F10_Am3_DC_50, sizeof (str2_F10_Am3_DC_50)},
  {2, 0, 0x04, DR_SOCKET_AM3, str2_F10_Am3_DC_50e, sizeof (str2_F10_Am3_DC_50e)},
  {2, 0, 0x05, DR_SOCKET_AM3, str2_F10_Am3_DC_50B, sizeof (str2_F10_Am3_DC_50B)},
  {2, 0, 0x06, DR_SOCKET_AM3, str2_F10_Am3_Processor, sizeof (str2_F10_Am3_Processor)},
  {2, 0, 0x07, DR_SOCKET_AM3, str2_F10_Am3_e_Processor, sizeof (str2_F10_Am3_e_Processor)},
  {2, 0, 0x08, DR_SOCKET_AM3, str2_F10_Am3_B_Processor, sizeof (str2_F10_Am3_B_Processor)},
  {2, 0, 0x09, DR_SOCKET_AM3, str2_F10_Am3_0_Processor, sizeof (str2_F10_Am3_0_Processor)},
  {2, 0, 0x0A, DR_SOCKET_AM3, str2_F10_Am3_0e_Processor, sizeof (str2_F10_Am3_0e_Processor)},
  {2, 0, 0x0B, DR_SOCKET_AM3, str2_F10_Am3_u_Processor, sizeof (str2_F10_Am3_u_Processor)},
  {2, 0, 0x0F, DR_SOCKET_AM3, 0, 0},    // Size 0 for no suffix
  {2, 1, 0x01, DR_SOCKET_AM3, str2_F10_Am3_L_Processor, sizeof (str2_F10_Am3_L_Processor)},
  {2, 1, 0x02, DR_SOCKET_AM3, str2_F10_Am3_C_Processor, sizeof (str2_F10_Am3_C_Processor)},
  {3, 0, 0x00, DR_SOCKET_AM3, str2_F10_Am3_TC_00, sizeof (str2_F10_Am3_TC_00)},
  {3, 0, 0x01, DR_SOCKET_AM3, str2_F10_Am3_TC_00e, sizeof (str2_F10_Am3_TC_00e)},
  {3, 0, 0x02, DR_SOCKET_AM3, str2_F10_Am3_TC_00B, sizeof (str2_F10_Am3_TC_00B)},
  {3, 0, 0x03, DR_SOCKET_AM3, str2_F10_Am3_TC_50, sizeof (str2_F10_Am3_TC_50)},
  {3, 0, 0x04, DR_SOCKET_AM3, str2_F10_Am3_TC_50e, sizeof (str2_F10_Am3_TC_50e)},
  {3, 0, 0x05, DR_SOCKET_AM3, str2_F10_Am3_TC_50B, sizeof (str2_F10_Am3_TC_50B)},
  {3, 0, 0x06, DR_SOCKET_AM3, str2_F10_Am3_Processor, sizeof (str2_F10_Am3_Processor)},
  {3, 0, 0x07, DR_SOCKET_AM3, str2_F10_Am3_e_Processor, sizeof (str2_F10_Am3_e_Processor)},
  {3, 0, 0x08, DR_SOCKET_AM3, str2_F10_Am3_B_Processor, sizeof (str2_F10_Am3_B_Processor)},
  {3, 0, 0x09, DR_SOCKET_AM3, str2_F10_Am3_0e_Processor, sizeof (str2_F10_Am3_0e_Processor)},
  {3, 0, 0x0A, DR_SOCKET_AM3, str2_F10_Am3_0_Processor, sizeof (str2_F10_Am3_0_Processor)},
  {3, 0, 0x0F, DR_SOCKET_AM3, 0, 0},    //Size 0 for no suffix
  {4, 0, 0x00, DR_SOCKET_AM3, str2_F10_Am3_QC_00, sizeof (str2_F10_Am3_QC_00)},
  {4, 0, 0x01, DR_SOCKET_AM3, str2_F10_Am3_QC_00e, sizeof (str2_F10_Am3_QC_00e)},
  {4, 0, 0x02, DR_SOCKET_AM3, str2_F10_Am3_QC_00B, sizeof (str2_F10_Am3_QC_00B)},
  {4, 0, 0x03, DR_SOCKET_AM3, str2_F10_Am3_QC_50, sizeof (str2_F10_Am3_QC_50)},
  {4, 0, 0x04, DR_SOCKET_AM3, str2_F10_Am3_QC_50e, sizeof (str2_F10_Am3_QC_50e)},
  {4, 0, 0x05, DR_SOCKET_AM3, str2_F10_Am3_QC_50B, sizeof (str2_F10_Am3_QC_50B)},
  {4, 0, 0x06, DR_SOCKET_AM3, str2_F10_Am3_Processor, sizeof (str2_F10_Am3_Processor)},
  {4, 0, 0x07, DR_SOCKET_AM3, str2_F10_Am3_e_Processor, sizeof (str2_F10_Am3_e_Processor)},
  {4, 0, 0x08, DR_SOCKET_AM3, str2_F10_Am3_B_Processor, sizeof (str2_F10_Am3_B_Processor)},
  {4, 0, 0x09, DR_SOCKET_AM3, str2_F10_Am3_0e_Processor, sizeof (str2_F10_Am3_0e_Processor)},
  {4, 0, 0x0A, DR_SOCKET_AM3, str2_F10_Am3_SE, sizeof (str2_F10_Am3_SE)},
  {4, 0, 0x0B, DR_SOCKET_AM3, str2_F10_Am3_HE, sizeof (str2_F10_Am3_HE)},
  {4, 0, 0x0C, DR_SOCKET_AM3, str2_F10_Am3_EE, sizeof (str2_F10_Am3_EE)},
  {4, 0, 0x0D, DR_SOCKET_AM3, str2_F10_Am3_QCP, sizeof (str2_F10_Am3_QCP)},
  {4, 0, 0x0E, DR_SOCKET_AM3, str2_F10_Am3_0_Processor, sizeof (str2_F10_Am3_0_Processor)},
  {4, 0, 0x0F, DR_SOCKET_AM3, 0, 0},    //Size 0 for no suffix
  {4, 1, 0x00, DR_SOCKET_AM3, str2_F10_Am3_TWKR_Black_Edition, sizeof (str2_F10_Am3_TWKR_Black_Edition)},
  {4, 1, 0x01, DR_SOCKET_AM3, str2_F10_Am3_L_Processor, sizeof (str2_F10_Am3_L_Processor)},
  {4, 1, 0x04, DR_SOCKET_AM3, str2_F10_Am3_QC_T, sizeof (str2_F10_Am3_QC_T)},
  {6, 0, 0x00, DR_SOCKET_AM3, str2_F10_Am3_SC_5T, sizeof (str2_F10_Am3_SC_5T)},
  {6, 0, 0x01, DR_SOCKET_AM3, str2_F10_Am3_SC_0T, sizeof (str2_F10_Am3_SC_0T)},
  {6, 0, 0x0F, DR_SOCKET_AM3, 0, 0}    //Size 0 for no suffix
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayAm3 = {
  (sizeof (CpuF10BrandIdString1ArrayAm3) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString1ArrayAm3
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayAm3 = {
  (sizeof (CpuF10BrandIdString2ArrayAm3) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString2ArrayAm3
};

