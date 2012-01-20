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
 * @e \$Revision: 57647 $   @e \$Date: 2011-08-08 14:56:33 -0600 (Mon, 08 Aug 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
// Family 10h equates
#define AMD_FAMILY_10_RB  0x0000000000000001
#define AMD_FAMILY_10_BL  0x0000000000000002
#define AMD_FAMILY_10_DA  0x0000000000000004
#define AMD_FAMILY_10_HY  0x0000000000000008
#define AMD_FAMILY_10_PH  0x0000000000000010
#define AMD_FAMILY_10_C32 AMD_FAMILY_10_HY

#define AMD_FAMILY_10 (AMD_FAMILY_10_RB | AMD_FAMILY_10_BL | AMD_FAMILY_10_DA | AMD_FAMILY_10_HY | AMD_FAMILY_10_PH)
#define AMD_FAMILY_GH    (AMD_FAMILY_10)

// Family 12h equates
#define AMD_FAMILY_12_LN  0x0000000000000020
#define AMD_FAMILY_12     (AMD_FAMILY_12_LN)
#define AMD_FAMILY_LN     (AMD_FAMILY_12_LN)

// Family 14h equates
#define AMD_FAMILY_14_ON  0x0000000000000040
#define AMD_FAMILY_ON     (AMD_FAMILY_14_ON)
#define AMD_FAMILY_14_KR  0x0000000000000080
#define AMD_FAMILY_KR     (AMD_FAMILY_14_KR)
#define AMD_FAMILY_14     (AMD_FAMILY_14_ON | AMD_FAMILY_14_KR)

// Family 15h equates
#define AMD_FAMILY_15_OR  0x0000000000000100
#define AMD_FAMILY_OR     (AMD_FAMILY_15_OR)
#define AMD_FAMILY_15_TN  0x0000000000000200
#define AMD_FAMILY_TN     (AMD_FAMILY_15_TN)
#define AMD_FAMILY_15_KM  0x0000000000000400
#define AMD_FAMILY_KM     (AMD_FAMILY_15_KM)
#define AMD_FAMILY_15     (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | AMD_FAMILY_15_KM)

// Family 16h equates
#define AMD_FAMILY_16     0x0000000000000800
#define AMD_FAMILY_WF     (AMD_FAMILY_16)

// Family Unknown
#define AMD_FAMILY_UNKNOWN 0x8000000000000000

// Family Group equates
#define AMD_FAMILY_GE_12   (AMD_FAMILY_12 | AMD_FAMILY_14 | AMD_FAMILY_15 | AMD_FAMILY_16)

// Family 10h CPU_LOGICAL_ID.Revision equates
// -------------------------------------
  // Family 10h RB steppings
#define AMD_F10_RB_C0      0x0000000000000001
#define AMD_F10_RB_C1      0x0000000000000002
#define AMD_F10_RB_C2      0x0000000000000004
#define AMD_F10_RB_C3      0x0000000000000008
  // Family 10h BL steppings
#define AMD_F10_BL_C2      0x0000000000000010
#define AMD_F10_BL_C3      0x0000000000000020
  // Family 10h DA steppings
#define AMD_F10_DA_C2      0x0000000000000040
#define AMD_F10_DA_C3      0x0000000000000080
  // Family 10h HY SCM steppings
#define AMD_F10_HY_SCM_D0  0x0000000000000100
#define AMD_F10_HY_SCM_D1  0x0000000000000400
  // Family 10h HY MCM steppings
#define AMD_F10_HY_MCM_D0  0x0000000000000200
#define AMD_F10_HY_MCM_D1  0x0000000000000800
  // Family 10h PH steppings
#define AMD_F10_PH_E0      0x0000000000001000
  // Family 10h Unknown stepping
  // * This equate is used to ensure that unknown CPU revisions are       *
  // * identified as the last known revision of the silicon family:       *
  // * - Update AMD_F10_UNKNOWN whenever newer F10h steppings are added   *
#define AMD_F10_UNKNOWN    (AMD_FAMILY_UNKNOWN | AMD_F10_C3 | AMD_F10_D1 | AMD_F10_PH_E0)

  // Family 10h Miscellaneous equates
#define AMD_F10_C0 (AMD_F10_RB_C0)
#define AMD_F10_C1 (AMD_F10_RB_C1)
#define AMD_F10_C2 (AMD_F10_RB_C2 | AMD_F10_DA_C2 | AMD_F10_BL_C2)
#define AMD_F10_C3 (AMD_F10_RB_C3 | AMD_F10_DA_C3 | AMD_F10_BL_C3)
#define AMD_F10_Cx (AMD_F10_C0 | AMD_F10_C1 | AMD_F10_C2 | AMD_F10_C3)

#define AMD_F10_RB_ALL (AMD_F10_RB_C0 | AMD_F10_RB_C1 | AMD_F10_RB_C2 | AMD_F10_RB_C3)

#define AMD_F10_BL_ALL (AMD_F10_BL_C2 | AMD_F10_BL_C3)
#define AMD_F10_BL_Cx  (AMD_F10_BL_C2 | AMD_F10_BL_C3)

#define AMD_F10_DA_ALL (AMD_F10_DA_C2 | AMD_F10_DA_C3)
#define AMD_F10_DA_Cx  (AMD_F10_DA_C2 | AMD_F10_DA_C3)

#define AMD_F10_D0 (AMD_F10_HY_SCM_D0 | AMD_F10_HY_MCM_D0)
#define AMD_F10_D1 (AMD_F10_HY_SCM_D1 | AMD_F10_HY_MCM_D1)
#define AMD_F10_Dx (AMD_F10_D0 | AMD_F10_D1)

#define AMD_F10_PH_ALL (AMD_F10_PH_E0)
#define AMD_F10_Ex (AMD_F10_PH_E0)

#define AMD_F10_HY_ALL (AMD_F10_Dx)
#define AMD_F10_C32_ALL (AMD_F10_HY_SCM_D0 | AMD_F10_HY_SCM_D1)

#define AMD_F10_GT_B0 (AMD_F10_Cx | AMD_F10_Dx | AMD_F10_Ex)
#define AMD_F10_GT_Bx (AMD_F10_Cx | AMD_F10_Dx | AMD_F10_Ex)
#define AMD_F10_GT_A2 (AMD_F10_Cx | AMD_F10_Dx | AMD_F10_Ex)
#define AMD_F10_GT_Ax (AMD_F10_Cx | AMD_F10_Dx | AMD_F10_Ex)
#define AMD_F10_GT_C0 ((AMD_F10_Cx & ~AMD_F10_C0) | AMD_F10_Dx | AMD_F10_Ex)
#define AMD_F10_GT_D0 ((AMD_F10_Dx & ~AMD_F10_D0) | AMD_F10_Ex)

#define AMD_F10_ALL (AMD_F10_Cx | AMD_F10_Dx  | AMD_F10_Ex)

// Family 12h CPU_LOGICAL_ID.Revision equates
// -------------------------------------

  // Family 12h LN steppings
#define AMD_F12_LN_A0    0x0000000000000001
#define AMD_F12_LN_A1    0x0000000000000002
#define AMD_F12_LN_B0    0x0000000000000004
  // Family 12h Unknown stepping
  // * This equate is used to ensure that unknown CPU revisions are       *
  // * identified as the last known revision of the silicon family:       *
  // * - Update AMD_F12_UNKNOWN whenever newer F12h steppings are added   *
#define AMD_F12_UNKNOWN  (AMD_FAMILY_UNKNOWN | AMD_F12_LN_B0)

#define AMD_F12_LN_Ax    (AMD_F12_LN_A0 | AMD_F12_LN_A1)
#define AMD_F12_LN_Bx    (AMD_F12_LN_B0)

#define AMD_F12_ALL      (AMD_F12_LN_Ax | AMD_F12_LN_Bx)

// Family 14h CPU_LOGICAL_ID.Revision equates
// -------------------------------------

  // Family 14h ON steppings
#define AMD_F14_ON_A0    0x0000000000000001
#define AMD_F14_ON_A1    0x0000000000000002
#define AMD_F14_ON_B0    0x0000000000000004
#define AMD_F14_ON_C0    0x0000000000000008
  // Family 14h KR steppings
#define AMD_F14_KR_A0    0x0000000000000100
#define AMD_F14_KR_A1    0x0000000000000200
#define AMD_F14_KR_B0    0x0000000000000400
  // Family 14h Unknown stepping
  // * This equate is used to ensure that unknown CPU revisions are       *
  // * identified as the last known revision of the silicon family:       *
  // * - Update AMD_F14_UNKNOWN whenever newer F14h steppings are added   *
#define AMD_F14_UNKNOWN  (AMD_FAMILY_UNKNOWN | AMD_F14_KR_B0 | AMD_F14_ON_C0)

#define AMD_F14_ON_Ax    (AMD_F14_ON_A0 | AMD_F14_ON_A1)
#define AMD_F14_ON_Bx    (AMD_F14_ON_B0)
#define AMD_F14_ON_Cx    (AMD_F14_ON_C0)
#define AMD_F14_ON_ALL   (AMD_F14_ON_Ax | AMD_F14_ON_Bx | AMD_F14_ON_Cx)

#define AMD_F14_KR_Ax    (AMD_F14_KR_A0 | AMD_F14_KR_A1)
#define AMD_F14_KR_Bx    AMD_F14_KR_B0
#define AMD_F14_KR_ALL   (AMD_F14_KR_Ax | AMD_F14_KR_Bx)

#define AMD_F14_ALL      (AMD_F14_ON_ALL | AMD_F14_KR_ALL)

// Family 15h CPU_LOGICAL_ID.Revision equates
// -------------------------------------

  // Family 15h OROCHI steppings
#define AMD_F15_OR_A0    0x0000000000000001
#define AMD_F15_OR_A1    0x0000000000000002
#define AMD_F15_OR_B0    0x0000000000000004
#define AMD_F15_OR_B1    0x0000000000000008
#define AMD_F15_OR_B2    0x0000000000000010
  // Family 15h TN steppings
#define AMD_F15_TN_A0    0x0000000000000100
  // Family 15h KM steppings
#define AMD_F15_KM_A0    0x0000000000010000
#define AMD_F15_KM_A1    0x0000000000020000
  // Family 15h Unknown stepping
  // * This equate is used to ensure that unknown CPU revisions are       *
  // * identified as the last known revision of the silicon family:       *
  // * - Update AMD_F15_UNKNOWN whenever newer F15h steppings are added   *
#define AMD_F15_UNKNOWN  (AMD_FAMILY_UNKNOWN | AMD_F15_OR_B2 | AMD_F15_TN_A0 | AMD_F15_KM_A1)

#define AMD_F15_OR_Ax    (AMD_F15_OR_A0 | AMD_F15_OR_A1)
#define AMD_F15_OR_Bx    (AMD_F15_OR_B0 | AMD_F15_OR_B1 | AMD_F15_OR_B2)
#define AMD_F15_OR_GT_Ax (AMD_F15_OR_Bx)
#define AMD_F15_OR_LT_B1 (AMD_F15_OR_Ax | AMD_F15_OR_B0)
#define AMD_F15_OR_ALL   (AMD_F15_OR_Ax | AMD_F15_OR_Bx)

#define AMD_F15_TN_Ax    (AMD_F15_TN_A0)
#define AMD_F15_TN_ALL   (AMD_F15_TN_Ax)

#define AMD_F15_KM_Ax    (AMD_F15_KM_A0 | AMD_F15_KM_A1)
#define AMD_F15_KM_ALL   (AMD_F15_KM_Ax)

#define AMD_F15_ALL      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | AMD_F15_KM_ALL)

// Family 16h CPU_LOGICAL_ID.Revision equates
// TBD

#endif      // _CPU_FAM_REGISTERS_H_

