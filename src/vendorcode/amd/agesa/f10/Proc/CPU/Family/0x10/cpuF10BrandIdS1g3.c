/**
 * @file
 *
 * AMD CPU BrandId related functions and structures for socket S1g3.
 *
 * Contains code that provides CPU BrandId information
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 6396 $   @e \$Date: 2008-06-16 05:52:20 -0700 (Mon, 16 Jun 2008) $
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
 * 
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "cpuEarlyInit.h"

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
// S1g3 NC 0
CONST CHAR8 ROMDATA str_Sempron_M1[] = "AMD Sempron(tm) M1";

// S1g3 NC 1
CONST CHAR8 ROMDATA str_Turion_II_U_DC_M_M6[] = "AMD Turion(tm) II Ultra Dual-Core Mobile M6";
CONST CHAR8 ROMDATA str_Turion_II_DC_M_M5[] = "AMD Turion(tm) II Dual-Core Mobile M5";
CONST CHAR8 ROMDATA str_Athlon_II_DC_M3[] = "AMD Athlon(tm) II Dual-Core M3";

// String2


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString1ArrayS1g3[] =
{
  // S1g3
  {1, 0, 0, DR_SOCKET_S1G3, str_Sempron_M1, sizeof (str_Sempron_M1)},
  {2, 0, 0, DR_SOCKET_S1G3, str_Turion_II_U_DC_M_M6, sizeof (str_Turion_II_U_DC_M_M6)},
  {2, 0, 1, DR_SOCKET_S1G3, str_Turion_II_DC_M_M5, sizeof (str_Turion_II_DC_M_M5)},
  {2, 0, 2, DR_SOCKET_S1G3, str_Athlon_II_DC_M3, sizeof (str_Athlon_II_DC_M3)}
};    //Cores, page, index, socket, stringstart, stringlength


CONST AMD_CPU_BRAND ROMDATA CpuF10BrandIdString2ArrayS1g3[] =
{
  // S1g3
  {1, 0, 0x0F, DR_SOCKET_S1G3, 0, 0},    //Size 0 for no suffix
  {2, 0, 0x0F, DR_SOCKET_S1G3, 0, 0}     //Size 0 for no suffix
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString1ArrayS1g3 = {
  (sizeof (CpuF10BrandIdString1ArrayS1g3) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString1ArrayS1g3
};


CONST CPU_BRAND_TABLE ROMDATA F10BrandIdString2ArrayS1g3 = {
  (sizeof (CpuF10BrandIdString2ArrayS1g3) / sizeof (AMD_CPU_BRAND)),
  CpuF10BrandIdString2ArrayS1g3
};

