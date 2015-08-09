/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Register Table Related Functions
 *
 * Contains the definition of the CPU CPUID MSRs and PCI registers with BKDG recommended values
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 309899 $   @e \$Date: 2014-12-23 02:21:13 -0600 (Tue, 23 Dec 2014) $
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2015, Advanced Micro Devices, Inc.
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
 ***************************************************************************/

#ifndef _CPU_FAM_REGISTERS_H_
#define _CPU_FAM_REGISTERS_H_

/*
 *--------------------------------------------------------------
 *
 *                      M O D U L E S    U S E D
 *
 *---------------------------------------------------------------
 */

/*
 *--------------------------------------------------------------
 *
 *                      D E F I N I T I O N S  /  M A C R O S
 *
 *---------------------------------------------------------------
 */

// This define should be equal to the total number of families
// in the cpuFamily enum.
#define MAX_CPU_FAMILIES    15
#define MAX_CPU_REVISIONS   15    // Max Cpu Revisions Per Family

// CPU_LOGICAL_ID.Family equates
// Family 15h equates
#define AMD_FAMILY_15_CZ  0x0004u
#define AMD_FAMILY_CZ     (AMD_FAMILY_15_CZ)

#define AMD_FAMILY_15_ST  0x0008u
#define AMD_FAMILY_ST     (AMD_FAMILY_15_ST)

#define AMD_FAMILY_15     (AMD_FAMILY_15_CZ | AMD_FAMILY_ST)

// Family Unknown
#define AMD_FAMILY_UNKNOWN 0x8000u


// Family 15h CPU_LOGICAL_ID.Revision equates
// -------------------------------------

  // Family 15h CZ steppings
#define AMD_F15_CZ_A0    0x0001u
#define AMD_F15_CZ_A1    0x0002u
#define AMD_F15_BR_A1    0x0004u
  // Family 15h ST steppings
#define AMD_F15_ST_A0    0x0010u
  // Family 15h Unknown stepping
#define AMD_F15_UNKNOWN  0x8000u
  // CZ and BR
#define AMD_F15_BR_Ax    (AMD_F15_BR_A1)
#define AMD_F15_BR_ALL   (AMD_F15_BR_Ax)

#define AMD_F15_CZ_Ax    (AMD_F15_CZ_A0 | AMD_F15_CZ_A1 | AMD_F15_BR_Ax)
#define AMD_F15_CZ_ALL   (AMD_F15_CZ_Ax | AMD_F15_BR_ALL)

  // ST
#define AMD_F15_ST_Ax    (AMD_F15_ST_A0)
#define AMD_F15_ST_ALL   (AMD_F15_ST_Ax)

#define AMD_F15_ALL      (AMD_F15_CZ_ALL | AMD_F15_ST_ALL)

#endif      // _CPU_FAM_REGISTERS_H_

