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
 * @e \$Revision: 46474 $   @e \$Date: 2011-02-03 05:46:17 +0800 (Thu, 03 Feb 2011) $
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
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A4_35[] = "AMD A4-35";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A4_34[] = "AMD A4-34";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A4_33[] = "AMD A4-33";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A4_32[] = "AMD A4-32";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_E2_30[] = "AMD E2-30";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_E2_20[] = "AMD E2-20";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_E2_10[] = "AMD E2-10";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A8_35[] = "AMD A8-35";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A8_34[] = "AMD A8-34";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A6_34[] = "AMD A6-34";
CONST CHAR8 ROMDATA str_F12_Fs1_AMD_A6_33[] = "AMD A6-33";

// String2
CONST CHAR8 ROMDATA str_F12_Fs1_M_APU[] = "M APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fs1_MX_APU[] = "MX APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fs1_ML_APU[] = "ML APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fs1_MZ_APU[] = "MZ APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fs1_MC_APU[] = "MC APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_F12_Fs1_MF_Processor[] = "MF Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MG_Processor[] = "MG Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MF_DC_Processor[] = "MF Dual-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MG_DC_Processor[] = "MG Dual-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MF_TC_Processor[] = "MF Triple-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MG_TC_Processor[] = "MG Triple-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MF_QC_Processor[] = "MF Quad-Core Processor";
CONST CHAR8 ROMDATA str_F12_Fs1_MG_QC_Processor[] = "MG Quad-Core Processor";


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF12LnBrandIdString1ArrayFs1[] =
{
  // FS1
  {1, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_35, sizeof (str_F12_Fs1_AMD_A4_35)},
  {2, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_35, sizeof (str_F12_Fs1_AMD_A4_35)},
  {1, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_34, sizeof (str_F12_Fs1_AMD_A4_34)},
  {2, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_34, sizeof (str_F12_Fs1_AMD_A4_34)},
  {1, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_33, sizeof (str_F12_Fs1_AMD_A4_33)},
  {2, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_33, sizeof (str_F12_Fs1_AMD_A4_33)},
  {1, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_32, sizeof (str_F12_Fs1_AMD_A4_32)},
  {2, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_AMD_A4_32, sizeof (str_F12_Fs1_AMD_A4_32)},
  {1, 0, 5, LN_SOCKET_FS1, str_F12_Fs1_AMD_E2_30, sizeof (str_F12_Fs1_AMD_E2_30)},
  {2, 0, 5, LN_SOCKET_FS1, str_F12_Fs1_AMD_E2_30, sizeof (str_F12_Fs1_AMD_E2_30)},
  {1, 0, 6, LN_SOCKET_FS1, str_F12_Fs1_AMD_E2_20, sizeof (str_F12_Fs1_AMD_E2_20)},
  {2, 0, 6, LN_SOCKET_FS1, str_F12_Fs1_AMD_E2_20, sizeof (str_F12_Fs1_AMD_E2_20)},
  {1, 0, 7, LN_SOCKET_FS1, str_F12_Fs1_AMD_E2_10, sizeof (str_F12_Fs1_AMD_E2_10)},
  {2, 0, 7, LN_SOCKET_FS1, str_F12_Fs1_AMD_E2_10, sizeof (str_F12_Fs1_AMD_E2_10)},
  {3, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_AMD_A8_35, sizeof (str_F12_Fs1_AMD_A8_35)},
  {4, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_AMD_A8_35, sizeof (str_F12_Fs1_AMD_A8_35)},
  {3, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_AMD_A8_34, sizeof (str_F12_Fs1_AMD_A8_34)},
  {4, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_AMD_A8_34, sizeof (str_F12_Fs1_AMD_A8_34)},
  {3, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_AMD_A6_34, sizeof (str_F12_Fs1_AMD_A6_34)},
  {4, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_AMD_A6_34, sizeof (str_F12_Fs1_AMD_A6_34)},
  {3, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_AMD_A6_33, sizeof (str_F12_Fs1_AMD_A6_33)},
  {4, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_AMD_A6_33, sizeof (str_F12_Fs1_AMD_A6_33)},
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF12LnBrandIdString2ArrayFs1[] =
{
  // FS1
  {1, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_M_APU, sizeof (str_F12_Fs1_M_APU)},
  {2, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_M_APU, sizeof (str_F12_Fs1_M_APU)},
  {3, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_M_APU, sizeof (str_F12_Fs1_M_APU)},
  {4, 0, 1, LN_SOCKET_FS1, str_F12_Fs1_M_APU, sizeof (str_F12_Fs1_M_APU)},
  {1, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_MX_APU, sizeof (str_F12_Fs1_MX_APU)},
  {2, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_MX_APU, sizeof (str_F12_Fs1_MX_APU)},
  {3, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_MX_APU, sizeof (str_F12_Fs1_MX_APU)},
  {4, 0, 2, LN_SOCKET_FS1, str_F12_Fs1_MX_APU, sizeof (str_F12_Fs1_MX_APU)},
  {1, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_ML_APU, sizeof (str_F12_Fs1_ML_APU)},
  {2, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_ML_APU, sizeof (str_F12_Fs1_ML_APU)},
  {3, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_ML_APU, sizeof (str_F12_Fs1_ML_APU)},
  {4, 0, 3, LN_SOCKET_FS1, str_F12_Fs1_ML_APU, sizeof (str_F12_Fs1_ML_APU)},
  {1, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_MZ_APU, sizeof (str_F12_Fs1_MZ_APU)},
  {2, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_MZ_APU, sizeof (str_F12_Fs1_MZ_APU)},
  {3, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_MZ_APU, sizeof (str_F12_Fs1_MZ_APU)},
  {4, 0, 4, LN_SOCKET_FS1, str_F12_Fs1_MZ_APU, sizeof (str_F12_Fs1_MZ_APU)},
  {1, 0, 5, LN_SOCKET_FS1, str_F12_Fs1_MC_APU, sizeof (str_F12_Fs1_MC_APU)},
  {2, 0, 5, LN_SOCKET_FS1, str_F12_Fs1_MC_APU, sizeof (str_F12_Fs1_MC_APU)},
  {3, 0, 5, LN_SOCKET_FS1, str_F12_Fs1_MC_APU, sizeof (str_F12_Fs1_MC_APU)},
  {4, 0, 5, LN_SOCKET_FS1, str_F12_Fs1_MC_APU, sizeof (str_F12_Fs1_MC_APU)},
  {1, 0, 6, LN_SOCKET_FS1, str_F12_Fs1_MF_Processor, sizeof (str_F12_Fs1_MF_Processor)},
  {1, 0, 7, LN_SOCKET_FS1, str_F12_Fs1_MG_Processor, sizeof (str_F12_Fs1_MG_Processor)},
  {2, 0, 6, LN_SOCKET_FS1, str_F12_Fs1_MF_DC_Processor, sizeof (str_F12_Fs1_MF_DC_Processor)},
  {2, 0, 7, LN_SOCKET_FS1, str_F12_Fs1_MG_DC_Processor, sizeof (str_F12_Fs1_MG_DC_Processor)},
  {3, 0, 6, LN_SOCKET_FS1, str_F12_Fs1_MF_TC_Processor, sizeof (str_F12_Fs1_MF_TC_Processor)},
  {3, 0, 7, LN_SOCKET_FS1, str_F12_Fs1_MG_TC_Processor, sizeof (str_F12_Fs1_MG_TC_Processor)},
  {4, 0, 6, LN_SOCKET_FS1, str_F12_Fs1_MF_QC_Processor, sizeof (str_F12_Fs1_MF_QC_Processor)},
  {4, 0, 7, LN_SOCKET_FS1, str_F12_Fs1_MG_QC_Processor, sizeof (str_F12_Fs1_MG_QC_Processor)},
 };    //Cores, page, index, socket, stringstart, stringlength


CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString1ArrayFs1 = {
  (sizeof (CpuF12LnBrandIdString1ArrayFs1) / sizeof (AMD_CPU_BRAND)),
  CpuF12LnBrandIdString1ArrayFs1
};


CONST CPU_BRAND_TABLE ROMDATA F12LnBrandIdString2ArrayFs1 = {
  (sizeof (CpuF12LnBrandIdString2ArrayFs1) / sizeof (AMD_CPU_BRAND)),
  CpuF12LnBrandIdString2ArrayFs1
};


