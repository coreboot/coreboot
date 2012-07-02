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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
