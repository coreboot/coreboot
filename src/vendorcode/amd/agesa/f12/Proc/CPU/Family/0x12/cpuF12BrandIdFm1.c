/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions and structures.
 *
 * Contains code that provides CPU BrandId information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x12
 * @e \$Revision: 52412 $   @e \$Date: 2011-05-06 08:13:56 +0800 (Fri, 06 May 2011) $
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
#include "F12PackageType.h"

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

// String1
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A4_3[] = "AMD A4-3";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A4_33[] = "AMD A4-33";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A4_34[] = "AMD A4-34";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A6_3[] = "AMD A6-3";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A6_34[] = "AMD A6-34";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A6_35[] = "AMD A6-35";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A6_36[] = "AMD A6-36";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A8_3[] = "AMD A8-3";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A8_35[] = "AMD A8-35";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_A8_38[] = "AMD A8-38";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_E2_1[] = "AMD E2-1";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_E2_12[] = "AMD E2-12";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_E2_3[] = "AMD E2-3";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_E2_32[] = "AMD E2-32";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II[] = "AMD Athlon(tm) II ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_1[] = "AMD Athlon(tm) II 1";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X2[] = "AMD Athlon(tm) II X2 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X2_2[] = "AMD Athlon(tm) II X2 2";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X3[] = "AMD Athlon(tm) II X3 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X3_3[] = "AMD Athlon(tm) II X3 3";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X3_4[] = "AMD Athlon(tm) II X3 4";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X4_4[] = "AMD Athlon(tm) II X4 4";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X4_6[] = "AMD Athlon(tm) II X4 6";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_II_X4[] = "AMD Athlon(tm) II X4 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_FM1[] = "AMD Athlon(tm) FM1 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_FM1_X2[] = "AMD Athlon(tm) FM1 X2 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_FM1_X3[] = "AMD Athlon(tm) FM1 X3 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Athlon_FM1_X4[] = "AMD Athlon(tm) FM1 X4 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_1[] = "AMD Sempron(tm) 1";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_X2_1[] = "AMD Sempron(tm) X2 1";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_X2_2[] = "AMD Sempron(tm) X2 2";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II[] = "AMD Sempron(tm) II ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_1[] = "AMD Sempron(tm) II 1";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_X2[] = "AMD Sempron(tm) II X2 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_X2_2[] = "AMD Sempron(tm) II X2 2";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_X3[] = "AMD Sempron(tm) II X3 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_X3_3[] = "AMD Sempron(tm) II X3 3";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_X4_4[] = "AMD Sempron(tm) II X4 4";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_II_X4[] = "AMD Sempron(tm) II X4 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_FM1[] = "AMD Sempron(tm) FM1 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_FM1_X2[] = "AMD Sempron(tm) FM1 X2 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_FM1_X3[] = "AMD Sempron(tm) FM1 X3 ";
CONST CHAR8 ROMDATA str_F12_Fm1_AMD_Sempron_FM1_X4[] = "AMD Sempron(tm) FM1 X4 ";

// String2
CONST CHAR8 ROMDATA str_F12_Fm1_APU[] = " APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fm1_0_APU[] = "0 APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fm1_P_APU[] = "P APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fm1_0P_APU[] = "0P APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fm1_Processor[] = " Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_0_Processor[] = "0 Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_DC_Processor[] = " Dual-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_0_DC_Processor[] = "0 Dual-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_TC_Processor[] = " Triple-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_0_TC_Processor[] = "0 Triple-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_QC_Processor[] = " Quad-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fm1_0_QC_Processor[] = "0 Quad-Core Processor";


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF12LnBrandIdString1ArrayFm1[] =
{
  // FM1
  {1, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_AMD_E2_12, sizeof (str_F12_Fm1_AMD_E2_12)},
  {1, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_1, sizeof (str_F12_Fm1_AMD_Sempron_II_1)},
  {1, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_1, sizeof (str_F12_Fm1_AMD_Athlon_II_1)},
  {1, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_AMD_E2_1, sizeof (str_F12_Fm1_AMD_E2_1)},
  {1, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II, sizeof (str_F12_Fm1_AMD_Sempron_II)},
  {1, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II, sizeof (str_F12_Fm1_AMD_Athlon_II)},
  {1, 0, 7, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_FM1, sizeof (str_F12_Fm1_AMD_Sempron_FM1)},
  {1, 0, 8, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_FM1, sizeof (str_F12_Fm1_AMD_Athlon_FM1)},
  {1, 0, 9, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_1, sizeof (str_F12_Fm1_AMD_Sempron_1)},
  {2, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_AMD_A4_33, sizeof (str_F12_Fm1_AMD_A4_33)},
  {2, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_AMD_E2_32, sizeof (str_F12_Fm1_AMD_E2_32)},
  {2, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_X2_2, sizeof (str_F12_Fm1_AMD_Sempron_II_X2_2)},
  {2, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X2_2, sizeof (str_F12_Fm1_AMD_Athlon_II_X2_2)},
  {2, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_AMD_A4_34, sizeof (str_F12_Fm1_AMD_A4_34)},
  {2, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_AMD_A4_3, sizeof (str_F12_Fm1_AMD_A4_3)},
  {2, 0, 7, LN_SOCKET_FM1, str_F12_Fm1_AMD_E2_3, sizeof (str_F12_Fm1_AMD_E2_3)},
  {2, 0, 8, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_X2, sizeof (str_F12_Fm1_AMD_Sempron_II_X2)},
  {2, 0, 9, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X2, sizeof (str_F12_Fm1_AMD_Athlon_II_X2)},
  {2, 0, 0xA, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_FM1_X2, sizeof (str_F12_Fm1_AMD_Sempron_FM1_X2)},
  {2, 0, 0xB, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_FM1_X2, sizeof (str_F12_Fm1_AMD_Athlon_FM1_X2)},
  {2, 0, 0xC, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_X2_1, sizeof (str_F12_Fm1_AMD_Sempron_X2_1)},
  {2, 0, 0xD, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_X2_2, sizeof (str_F12_Fm1_AMD_Sempron_X2_2)},
  {3, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_34, sizeof (str_F12_Fm1_AMD_A6_34)},
  {3, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_X3_3, sizeof (str_F12_Fm1_AMD_Sempron_II_X3_3)},
  {3, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X3_3, sizeof (str_F12_Fm1_AMD_Athlon_II_X3_3)},
  {3, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_36, sizeof (str_F12_Fm1_AMD_A6_36)},
  {3, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_35, sizeof (str_F12_Fm1_AMD_A6_35)},
  {3, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_3, sizeof (str_F12_Fm1_AMD_A6_3)},
  {3, 0, 7, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_X3, sizeof (str_F12_Fm1_AMD_Sempron_II_X3)},
  {3, 0, 8, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X3, sizeof (str_F12_Fm1_AMD_Athlon_II_X3)},
  {3, 0, 9, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_FM1_X3, sizeof (str_F12_Fm1_AMD_Sempron_FM1_X3)},
  {3, 0, 0xA, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_FM1_X3, sizeof (str_F12_Fm1_AMD_Athlon_FM1_X3)},
  {3, 0, 0xB, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X3_4, sizeof (str_F12_Fm1_AMD_Athlon_II_X3_4)},
  {4, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_AMD_A8_35, sizeof (str_F12_Fm1_AMD_A8_35)},
  {4, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_34, sizeof (str_F12_Fm1_AMD_A6_34)},
  {4, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_X4_4, sizeof (str_F12_Fm1_AMD_Sempron_II_X4_4)},
  {4, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X4_4, sizeof (str_F12_Fm1_AMD_Athlon_II_X4_4)},
  {4, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_AMD_A8_38, sizeof (str_F12_Fm1_AMD_A8_38)},
  {4, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_36, sizeof (str_F12_Fm1_AMD_A6_36)},
  {4, 0, 7, LN_SOCKET_FM1, str_F12_Fm1_AMD_A8_3, sizeof (str_F12_Fm1_AMD_A8_3)},
  {4, 0, 8, LN_SOCKET_FM1, str_F12_Fm1_AMD_A6_3, sizeof (str_F12_Fm1_AMD_A6_3)},
  {4, 0, 9, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_II_X4, sizeof (str_F12_Fm1_AMD_Sempron_II_X4)},
  {4, 0, 0xA, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X4, sizeof (str_F12_Fm1_AMD_Athlon_II_X4)},
  {4, 0, 0xB, LN_SOCKET_FM1, str_F12_Fm1_AMD_Sempron_FM1_X4, sizeof (str_F12_Fm1_AMD_Sempron_FM1_X4)},
  {4, 0, 0xC, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_FM1_X4, sizeof (str_F12_Fm1_AMD_Athlon_FM1_X4)},
  {4, 0, 0xD, LN_SOCKET_FM1, str_F12_Fm1_AMD_Athlon_II_X4_6, sizeof (str_F12_Fm1_AMD_Athlon_II_X4_6)},
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF12LnBrandIdString2ArrayFm1[] =
{
  // FM1
  {1, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_APU, sizeof (str_F12_Fm1_APU)},
  {1, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_Processor, sizeof (str_F12_Fm1_Processor)},
  {1, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_P_APU, sizeof (str_F12_Fm1_P_APU)},
  {1, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_0_APU, sizeof (str_F12_Fm1_0_APU)},
  {1, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_0P_APU, sizeof (str_F12_Fm1_0P_APU)},
  {1, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_0_Processor, sizeof (str_F12_Fm1_0_Processor)},
  {2, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_APU, sizeof (str_F12_Fm1_APU)},
  {2, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_DC_Processor, sizeof (str_F12_Fm1_DC_Processor)},
  {2, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_P_APU, sizeof (str_F12_Fm1_P_APU)},
  {2, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_0_APU, sizeof (str_F12_Fm1_0_APU)},
  {2, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_0P_APU, sizeof (str_F12_Fm1_0P_APU)},
  {2, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_0_DC_Processor, sizeof (str_F12_Fm1_0_DC_Processor)},
  {3, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_APU, sizeof (str_F12_Fm1_APU)},
  {3, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_P_APU, sizeof (str_F12_Fm1_P_APU)},
  {3, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_TC_Processor, sizeof (str_F12_Fm1_TC_Processor)},
  {3, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_0_APU, sizeof (str_F12_Fm1_0_APU)},
  {3, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_0P_APU, sizeof (str_F12_Fm1_0P_APU)},
  {3, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_0_TC_Processor, sizeof (str_F12_Fm1_0_TC_Processor)},
  {4, 0, 1, LN_SOCKET_FM1, str_F12_Fm1_APU, sizeof (str_F12_Fm1_APU)},
  {4, 0, 2, LN_SOCKET_FM1, str_F12_Fm1_P_APU, sizeof (str_F12_Fm1_P_APU)},
  {4, 0, 3, LN_SOCKET_FM1, str_F12_Fm1_QC_Processor, sizeof (str_F12_Fm1_QC_Processor)},
  {4, 0, 4, LN_SOCKET_FM1, str_F12_Fm1_0_APU, sizeof (str_F12_Fm1_0_APU)},
  {4, 0, 5, LN_SOCKET_FM1, str_F12_Fm1_0P_APU, sizeof (str_F12_Fm1_0P_APU)},
  {4, 0, 6, LN_SOCKET_FM1, str_F12_Fm1_0_QC_Processor, sizeof (str_F12_Fm1_0_QC_Processor)},
 };    //Cores, page, index, socket, stringstart, stringlength


CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString1ArrayFm1 = {
  (sizeof (CpuF12LnBrandIdString1ArrayFm1) / sizeof (AMD_CPU_BRAND)),
  CpuF12LnBrandIdString1ArrayFm1
};


CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString2ArrayFm1 = {
  (sizeof (CpuF12LnBrandIdString2ArrayFm1) / sizeof (AMD_CPU_BRAND)),
  CpuF12LnBrandIdString2ArrayFm1
};


