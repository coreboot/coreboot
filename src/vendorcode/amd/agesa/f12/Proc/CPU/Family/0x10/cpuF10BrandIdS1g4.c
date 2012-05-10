/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU BrandId related functions and structures for package S1g4.
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

// String1
CONST CHAR8 ROMDATA str_F10_S1g4_AMD_V[] = "AMD V";
CONST CHAR8 ROMDATA str_F10_S1g4_Turion_II_P[] = "AMD Turion(tm) II P";
CONST CHAR8 ROMDATA str_F10_S1g4_Athlon_II_P[] = "AMD Athlon(tm) II P";
CONST CHAR8 ROMDATA str_F10_S1g4_Phenom_II_X[] = "AMD Phenom(tm) II X";
CONST CHAR8 ROMDATA str_F10_S1g4_Turion_II_N[] = "AMD Turion(tm) II N";
CONST CHAR8 ROMDATA str_F10_S1g4_Athlon_II_N[] = "AMD Athlon(tm) II N";
CONST CHAR8 ROMDATA str_F10_S1g4_Phenom_II_P[] = "AMD Phenom(tm) II P";
CONST CHAR8 ROMDATA str_F10_S1g4_Phenom_II_N[] = "AMD Phenom(tm) II N";

// String2
CONST CHAR8 ROMDATA str_F10_S1g4_0_Processor[] = "0 Processor";
CONST CHAR8 ROMDATA str_F10_S1g4_0_Dual_Core_Processor[] = "0 Dual-Core Processor";
CONST CHAR8 ROMDATA str_F10_S1g4_0_Triple_Core_Processor[] = "0 Triple-Core Processor";
CONST CHAR8 ROMDATA str_F10_S1g4_0_Quad_Core_Processor[] = "0 Quad-Core Processor";

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString1ArrayS1g4[] =
{
  // S1g4
  {1, 0, 1, DR_SOCKET_S1G4, str_F10_S1g4_AMD_V, sizeof (str_F10_S1g4_AMD_V)},
  {2, 0, 3, DR_SOCKET_S1G4, str_F10_S1g4_Turion_II_P, sizeof (str_F10_S1g4_Turion_II_P)},
  {2, 0, 4, DR_SOCKET_S1G4, str_F10_S1g4_Athlon_II_P, sizeof (str_F10_S1g4_Athlon_II_P)},
  {2, 0, 5, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_X, sizeof (str_F10_S1g4_Phenom_II_X)},
  {2, 0, 6, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_N, sizeof (str_F10_S1g4_Phenom_II_N)},
  {2, 0, 7, DR_SOCKET_S1G4, str_F10_S1g4_Turion_II_N, sizeof (str_F10_S1g4_Turion_II_N)},
  {2, 0, 8, DR_SOCKET_S1G4, str_F10_S1g4_Athlon_II_N, sizeof (str_F10_S1g4_Athlon_II_N)},
  {2, 0, 9, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_P, sizeof (str_F10_S1g4_Phenom_II_P)},
  {3, 0, 2, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_P, sizeof (str_F10_S1g4_Phenom_II_P)},
  {3, 0, 3, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_N, sizeof (str_F10_S1g4_Phenom_II_N)},
  {3, 0, 4, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_X, sizeof (str_F10_S1g4_Phenom_II_X)},
  {4, 0, 1, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_P, sizeof (str_F10_S1g4_Phenom_II_P)},
  {4, 0, 2, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_X, sizeof (str_F10_S1g4_Phenom_II_X)},
  {4, 0, 3, DR_SOCKET_S1G4, str_F10_S1g4_Phenom_II_N, sizeof (str_F10_S1g4_Phenom_II_N)}
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString2ArrayS1g4[] =
{
  // S1g4
  {1, 0, 0x01, DR_SOCKET_S1G4, str_F10_S1g4_0_Processor, sizeof (str_F10_S1g4_0_Processor)},
  {2, 0, 0x02, DR_SOCKET_S1G4, str_F10_S1g4_0_Dual_Core_Processor, sizeof (str_F10_S1g4_0_Dual_Core_Processor)},
  {3, 0, 0x02, DR_SOCKET_S1G4, str_F10_S1g4_0_Triple_Core_Processor, sizeof (str_F10_S1g4_0_Triple_Core_Processor)},
  {4, 0, 0x01, DR_SOCKET_S1G4, str_F10_S1g4_0_Quad_Core_Processor, sizeof (str_F10_S1g4_0_Quad_Core_Processor)},
  {1, 0, 0x0F, DR_SOCKET_S1G4, 0, 0},    //Size 0 for no suffix
  {2, 0, 0x0F, DR_SOCKET_S1G4, 0, 0},    //Size 0 for no suffix
  {3, 0, 0x0F, DR_SOCKET_S1G4, 0, 0},    //Size 0 for no suffix
  {4, 0, 0x0F, DR_SOCKET_S1G4, 0, 0}     //Size 0 for no suffix
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayS1g4 = {
  (sizeof (CpuF10BrandIdString1ArrayS1g4) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString1ArrayS1g4
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayS1g4 = {
  (sizeof (CpuF10BrandIdString2ArrayS1g4) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString2ArrayS1g4
};


