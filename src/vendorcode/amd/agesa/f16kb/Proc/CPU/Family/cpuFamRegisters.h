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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

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
#define MAX_CPU_FAMILIES    64
#define MAX_CPU_REVISIONS   63    // Max Cpu Revisions Per Family

// CPU_LOGICAL_ID.Family equates
// Family 15h equates
#define AMD_FAMILY_15_TN  0x0000000000000200ull
#define AMD_FAMILY_TN     (AMD_FAMILY_15_TN)
#define AMD_FAMILY_15     (AMD_FAMILY_15_TN)

// Family 16h equates
#define AMD_FAMILY_16_KB  0x0000000000010000ull
#define AMD_FAMILY_KB     AMD_FAMILY_16_KB
#define AMD_FAMILY_16     (AMD_FAMILY_KB)

// Family Unknown
#define AMD_FAMILY_UNKNOWN 0x8000000000000000ull


// Family 15h CPU_LOGICAL_ID.Revision equates
// -------------------------------------

  // Family 15h TN steppings
#define AMD_F15_TN_A0    0x0000000000000100ull
#define AMD_F15_TN_A1    0x0000000000000200ull
  // Family 15h Unknown stepping
  // * This equate is used to ensure that unknown CPU revisions are       *
  // * identified as the last known revision of the silicon family:       *
  // * - Update AMD_F15_UNKNOWN whenever newer F15h steppings are added   *
#define AMD_F15_UNKNOWN  (AMD_FAMILY_UNKNOWN | AMD_F15_TN_A1)


#define AMD_F15_TN_Ax    (AMD_F15_TN_A0 | AMD_F15_TN_A1)
#define AMD_F15_TN_GT_A0 (AMD_F15_TN_ALL & ~AMD_F15_TN_A0)
#define AMD_F15_TN_ONLY  (AMD_F15_TN_Ax)
#define AMD_F15_TN_ALL   (AMD_F15_TN_Ax | AMD_F15_RL_ALL)


#define AMD_F15_ALL      (AMD_F15_TN_ALL)

// Family 16h CPU_LOGICAL_ID.Revision equates
// -------------------------------------

  // Family 16h KB steppings
#define AMD_F16_KB_A0    0x0000000000000001ull
#define AMD_F16_KB_A1    0x0000000000000002ull
  // Family 16h ML steppings
  // Family 16h Unknown stepping
  // * This equate is used to ensure that unknown CPU revisions are       *
  // * identified as the last known revision of the silicon family:       *
  // * - Update AMD_F16_UNKNOWN whenever newer F16h steppings are added   *
#define AMD_F16_UNKNOWN  (AMD_FAMILY_UNKNOWN | AMD_F16_KB_A1)

#define AMD_F16_KB_Ax    (AMD_F16_KB_A0 | AMD_F16_KB_A1)
#define AMD_F16_KB_ALL   (AMD_F16_KB_Ax)


#define AMD_F16_ALL      (AMD_F16_KB_ALL)

#endif      // _CPU_FAM_REGISTERS_H_

