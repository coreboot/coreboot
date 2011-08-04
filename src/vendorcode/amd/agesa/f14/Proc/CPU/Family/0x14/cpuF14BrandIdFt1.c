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
 * @e sub-project:  CPU
 * @e \$Revision: 45203 $   @e \$Date: 2011-01-13 12:36:39 -0700 (Thu, 13 Jan 2011) $
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
#include "F14PackageType.h"

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
CONST CHAR8 ROMDATA str_AMD_C[] = "AMD C-";
CONST CHAR8 ROMDATA str_AMD_E[] = "AMD E-";
CONST CHAR8 ROMDATA str_AMD_G_T[] = "AMD G-T";
CONST CHAR8 ROMDATA str_AMD_Z[] = "AMD Z-";

// String2
CONST CHAR8 ROMDATA str___Processor[] = " Processor";
CONST CHAR8 ROMDATA str___0_Processor[] = "0 Processor";
CONST CHAR8 ROMDATA str_5_Processor[] = "5 Processor";
CONST CHAR8 ROMDATA str_0x_Processor[] = "0x Processor";
CONST CHAR8 ROMDATA str_5x_Processor[] = "5x Processor";
CONST CHAR8 ROMDATA str_x_Processor[] = "x Processor";
CONST CHAR8 ROMDATA str_L_Processor[] = "L Processor";
CONST CHAR8 ROMDATA str_N_Processor[] = "N Processor";
CONST CHAR8 ROMDATA str_R_Processor[] = "R Processor";
CONST CHAR8 ROMDATA str_E_Processor[] = "E Processor";
CONST CHAR8 ROMDATA str_0D_APU[] = "0D APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_0_APU[] = "0 APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_5_APU[] = "5 APU with Radeon(tm) HD Graphics";
CONST CHAR8 ROMDATA str_APU[] = " APU with Radeon(tm) HD Graphics";

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF14OnBrandIdString1ArrayFt1[] =
{
  // FT1
  {1, 0, 1, ON_SOCKET_FT1, str_AMD_C, sizeof (str_AMD_C)},
  {2, 0, 1, ON_SOCKET_FT1, str_AMD_C, sizeof (str_AMD_C)},
  {1, 0, 2, ON_SOCKET_FT1, str_AMD_E, sizeof (str_AMD_E)},
  {2, 0, 2, ON_SOCKET_FT1, str_AMD_E, sizeof (str_AMD_E)},
  {2, 0, 3, ON_SOCKET_FT1, str_AMD_Z, sizeof (str_AMD_Z)},
  {1, 0, 4, ON_SOCKET_FT1, str_AMD_G_T, sizeof (str_AMD_G_T)},
  {2, 0, 4, ON_SOCKET_FT1, str_AMD_G_T, sizeof (str_AMD_G_T)}
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF14OnBrandIdString2ArrayFt1[] =
{
  // FT1
  {1, 0, 0x01, ON_SOCKET_FT1, str___Processor, sizeof (str___Processor)},
  {2, 0, 0x01, ON_SOCKET_FT1, str___Processor, sizeof (str___Processor)},
  {1, 0, 0x02, ON_SOCKET_FT1, str___0_Processor, sizeof (str___0_Processor)},
  {2, 0, 0x02, ON_SOCKET_FT1, str___0_Processor, sizeof (str___0_Processor)},
  {1, 0, 0x03, ON_SOCKET_FT1, str_5_Processor, sizeof (str_5_Processor)},
  {2, 0, 0x03, ON_SOCKET_FT1, str_5_Processor, sizeof (str_5_Processor)},
  {1, 0, 0x04, ON_SOCKET_FT1, str_0x_Processor, sizeof (str_0x_Processor)},
  {2, 0, 0x04, ON_SOCKET_FT1, str_0x_Processor, sizeof (str_0x_Processor)},
  {1, 0, 0x05, ON_SOCKET_FT1, str_5x_Processor, sizeof (str_5x_Processor)},
  {2, 0, 0x05, ON_SOCKET_FT1, str_5x_Processor, sizeof (str_5x_Processor)},
  {1, 0, 0x06, ON_SOCKET_FT1, str_x_Processor, sizeof (str_x_Processor)},
  {2, 0, 0x06, ON_SOCKET_FT1, str_x_Processor, sizeof (str_x_Processor)},
  {1, 0, 0x07, ON_SOCKET_FT1, str_L_Processor, sizeof (str_L_Processor)},
  {2, 0, 0x07, ON_SOCKET_FT1, str_L_Processor, sizeof (str_L_Processor)},
  {1, 0, 0x08, ON_SOCKET_FT1, str_N_Processor, sizeof (str_N_Processor)},
  {2, 0, 0x08, ON_SOCKET_FT1, str_N_Processor, sizeof (str_N_Processor)},
  {1, 0, 0x09, ON_SOCKET_FT1, str_R_Processor, sizeof (str_R_Processor)},
  {2, 0, 0x09, ON_SOCKET_FT1, str_0_APU, sizeof (str_0_APU)},
  {1, 0, 0x0A, ON_SOCKET_FT1, str_0_APU, sizeof (str_0_APU)},
  {2, 0, 0x0A, ON_SOCKET_FT1, str_5_APU, sizeof (str_5_APU)},
  {1, 0, 0x0B, ON_SOCKET_FT1, str_5_APU, sizeof (str_5_APU)},
  {2, 0, 0x0B, ON_SOCKET_FT1, str_APU, sizeof (str_APU)},
  {1, 0, 0x0C, ON_SOCKET_FT1, str_APU, sizeof (str_APU)},
  {2, 0, 0x0C, ON_SOCKET_FT1, str_E_Processor, sizeof (str_E_Processor)},
  {1, 0, 0x0D, ON_SOCKET_FT1, str_0D_APU, sizeof (str_0D_APU)},
  {2, 0, 0x0D, ON_SOCKET_FT1, str_0D_APU, sizeof (str_0D_APU)},
  {1, 0, 0x0F, ON_SOCKET_FT1, 0, 0},    //Size 0 for no suffix
  {2, 0, 0x0F, ON_SOCKET_FT1, 0, 0},    //Size 0 for no suffix
 };    //Cores, page, index, socket, stringstart, stringlength


CONST CPU_BRAND_TABLE ROMDATA F14OnBrandIdString1ArrayFt1 = {
  (sizeof (CpuF14OnBrandIdString1ArrayFt1) / sizeof (AMD_CPU_BRAND)),
  CpuF14OnBrandIdString1ArrayFt1
};


CONST CPU_BRAND_TABLE ROMDATA F14OnBrandIdString2ArrayFt1 = {
  (sizeof (CpuF14OnBrandIdString2ArrayFt1) / sizeof (AMD_CPU_BRAND)),
  CpuF14OnBrandIdString2ArrayFt1
};


