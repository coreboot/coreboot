/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD ACPI PState option API.
 *
 * Contains structures and values used to control the PStates option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*****************************************************************************
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

#ifndef _OPTION_PSTATE_H_
#define _OPTION_PSTATE_H_

#include "cpuPstateTables.h"

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */

typedef AGESA_STATUS OPTION_SSDT_FEATURE (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **AcpiPstatePtr
  );

typedef UINT32 OPTION_ACPI_FEATURE (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       PSTATE_LEVELING        *PStateLevelingBuffer,
  IN OUT   VOID                   **AcpiPStatePtr,
  IN       UINT8                  LocalApicId,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

typedef AGESA_STATUS OPTION_PSTATE_GATHER (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr
  );

typedef AGESA_STATUS OPTION_PSTATE_LEVELING (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

#define PSTATE_STRUCT_VERSION  0x01

/// Indirection vectors for POST/PEI PState code
typedef struct  {
  UINT16                  OptPstateVersion;     ///< revision of this structure
  OPTION_PSTATE_GATHER    *PstateGather;        ///< vector for data gathering routine
  OPTION_PSTATE_LEVELING  *PstateLeveling;      ///< vector for leveling routine
} OPTION_PSTATE_POST_CONFIGURATION;

/// Indirection vectors for LATE/DXE PState code
typedef struct  {
  UINT16                  OptPstateVersion;     ///< revision of this structure
  OPTION_SSDT_FEATURE     *SsdtFeature;         ///< vector for routine to generate SSDT
  OPTION_ACPI_FEATURE     *PstateFeature;       ///< vector for routine to generate ACPI PState Objects
  OPTION_ACPI_FEATURE     *CstateFeature;       ///< vector for routine to generate ACPI CState Objects
  BOOLEAN                 CfgPstatePpc;         ///< boolean for creating _PPC method
  BOOLEAN                 CfgPstatePct;         ///< boolean for creating _PCT method
  BOOLEAN                 CfgPstatePsd;         ///< boolean for creating _PSD method
  BOOLEAN                 CfgPstatePss;         ///< boolean for creating _PSS method
  BOOLEAN                 CfgPstateXpss;        ///< boolean for creating _XPSS method
} OPTION_PSTATE_LATE_CONFIGURATION;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

#endif  // _OPTION_PSTATE_H_
