/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions and structures for socket Fr1207.
 *
 * Contains code that provides CPU BrandId information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
 * ***************************************************************************
 *
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
CONST CHAR8 ROMDATA str_DC_Opteron83[] = "Dual-Core AMD Opteron(tm) Processor 83";
CONST CHAR8 ROMDATA str_DC_Opteron23[] = "Dual-Core AMD Opteron(tm) Processor 23";
CONST CHAR8 ROMDATA str_QC_Opteron83[] = "Quad-Core AMD Opteron(tm) Processor 83";
CONST CHAR8 ROMDATA str_QC_Opteron23[] = "Quad-Core AMD Opteron(tm) Processor 23";
CONST CHAR8 ROMDATA str_eQC_Opteron83[] = "Embedded AMD Opteron(tm) Processor 83";
CONST CHAR8 ROMDATA str_eQC_Opteron23[] = "Embedded AMD Opteron(tm) Processor 23";
CONST CHAR8 ROMDATA str_eQC_Opteron13[] = "Embedded AMD Opteron(tm) Processor 13";
CONST CHAR8 ROMDATA str_Embedded_Opteron[] = "Embedded AMD Opteron(tm) Processor ";
CONST CHAR8 ROMDATA str_SC_Opteron84[] = "Six-Core AMD Opteron(tm) Processor 84";
CONST CHAR8 ROMDATA str_SC_Opteron24[] = "Six-Core AMD Opteron(tm) Processor 24";

CONST CHAR8 ROMDATA str_PhenomFX[] =  "AMD Phenom(tm) FX-";


// String2
CONST CHAR8 ROMDATA str2_SE[] = " SE";
CONST CHAR8 ROMDATA str2_HE[] = " HE";
CONST CHAR8 ROMDATA str2_EE[] = " EE";
CONST CHAR8 ROMDATA str2_VS[] = "VS";

CONST CHAR8 ROMDATA str2_NP_HE[] = "NP HE";
CONST CHAR8 ROMDATA str2_GF_HE[] = "GF HE";
CONST CHAR8 ROMDATA str2_HF_HE[] = "HF HE";
CONST CHAR8 ROMDATA str2_QS_HE[] = "QS HE";
CONST CHAR8 ROMDATA str2_KH_HE[] = "KH HE";
CONST CHAR8 ROMDATA str2_KS_EE[] = "KS HE";

CONST CHAR8 ROMDATA str2_QCP[] = " Quad-Core Processor";

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString1ArrayFr1207[] =
{
  // FR2/FR4 1207
  {2, 0, 0, DR_SOCKET_1207, str_DC_Opteron83, sizeof (str_DC_Opteron83)},
  {2, 0, 1, DR_SOCKET_1207, str_DC_Opteron23, sizeof (str_DC_Opteron23)},
  {3, 0, 0, DR_SOCKET_1207, str_Embedded_Opteron, sizeof (str_Embedded_Opteron)},
  {4, 0, 0, DR_SOCKET_1207, str_QC_Opteron83, sizeof (str_QC_Opteron83)},
  {4, 0, 1, DR_SOCKET_1207, str_QC_Opteron23, sizeof (str_QC_Opteron23)},
  {4, 0, 2, DR_SOCKET_1207, str_eQC_Opteron83, sizeof (str_eQC_Opteron83)},
  {4, 0, 3, DR_SOCKET_1207, str_eQC_Opteron23, sizeof (str_eQC_Opteron23)},
  {4, 0, 4, DR_SOCKET_1207, str_eQC_Opteron13, sizeof (str_eQC_Opteron13)},
  {4, 0, 5, DR_SOCKET_1207, str_PhenomFX, sizeof (str_PhenomFX)},
  {4, 1, 1, DR_SOCKET_1207, str_Embedded_Opteron, sizeof (str_Embedded_Opteron)},
  {6, 0, 0, DR_SOCKET_1207, str_SC_Opteron84, sizeof (str_SC_Opteron84)},
  {6, 0, 1, DR_SOCKET_1207, str_SC_Opteron24, sizeof (str_SC_Opteron24)}
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString2ArrayFr1207[] =
{
  // FR2/FR4 1207
  {2, 0, 0x0A, DR_SOCKET_1207, str2_SE, sizeof (str2_SE)},
  {2, 0, 0x0B, DR_SOCKET_1207, str2_HE, sizeof (str2_HE)},
  {2, 0, 0x0C, DR_SOCKET_1207, str2_EE, sizeof (str2_EE)},
  {2, 0, 0x0F, DR_SOCKET_1207, 0, 0},    //Size 0 for no suffix
  {3, 0, 0x00, DR_SOCKET_1207, str2_NP_HE, sizeof (str2_NP_HE)},
  {3, 0, 0x0F, DR_SOCKET_1207, 0, 0},    //Size 0 for no suffix
  {4, 0, 0x0A, DR_SOCKET_1207, str2_SE, sizeof (str2_SE)},
  {4, 0, 0x0B, DR_SOCKET_1207, str2_HE, sizeof (str2_HE)},
  {4, 0, 0x0C, DR_SOCKET_1207, str2_EE, sizeof (str2_EE)},
  {4, 0, 0x0D, DR_SOCKET_1207, str2_QCP, sizeof (str2_QCP)},
  {4, 0, 0x0F, DR_SOCKET_1207, 0, 0},    //Size 0 for no suffix
  {4, 1, 0x01, DR_SOCKET_1207, str2_GF_HE, sizeof (str2_GF_HE)},
  {4, 1, 0x02, DR_SOCKET_1207, str2_HF_HE, sizeof (str2_HF_HE)},
  {4, 1, 0x03, DR_SOCKET_1207, str2_VS, sizeof (str2_VS)},
  {4, 1, 0x04, DR_SOCKET_1207, str2_QS_HE, sizeof (str2_QS_HE)},
  {4, 1, 0x05, DR_SOCKET_1207, str2_NP_HE, sizeof (str2_NP_HE)},
  {4, 1, 0x06, DR_SOCKET_1207, str2_KH_HE, sizeof (str2_KH_HE)},
  {4, 1, 0x07, DR_SOCKET_1207, str2_KS_EE, sizeof (str2_KS_EE)},
  {6, 0, 0x00, DR_SOCKET_1207, str2_SE, sizeof (str2_SE)},
  {6, 0, 0x01, DR_SOCKET_1207, str2_HE, sizeof (str2_HE)},
  {6, 0, 0x02, DR_SOCKET_1207, str2_EE, sizeof (str2_EE)},
  {6, 0, 0x0F, DR_SOCKET_1207, 0, 0}    //Size 0 for no suffix
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayFr1207 = {
  (sizeof (CpuF10BrandIdString1ArrayFr1207) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString1ArrayFr1207
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayFr1207 = {
  (sizeof (CpuF10BrandIdString2ArrayFr1207) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString2ArrayFr1207
};

