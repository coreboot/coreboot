/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15  PCI tables with values as defined in BKDG
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "Table.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15PCITABLES_FILECODE

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

//  P C I    T a b l e s
// ----------------------

STATIC CONST TABLE_ENTRY_FIELDS ROMDATA F15PciRegisters[] =
{
// F2x1B0 - Extended Memory Controller Configuration Low
// bits[10:8], CohPrefPrbLmt = 1
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_2, 0x1B0),  // Address
      0x00000100,                           // regData
      0x00000700,                           // regMask
    }}
  },

// Function 3 - Misc. Control

// F3x6C - Data Buffer Count
// bits[30:28] IsocRspDBC = 1
// bits[18:16] UpRspDBC = 1
// bits[7:6]   DnRspDBC = 1
// bits[5:4]   DnReqDBC = 1
// bits[2:0]   UpReqDBC = 2
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x6C),  // Address
      0x10010052,                           // regData
      0x700700F7,                           // regMask
    }}
  },
// F3xA0 - Power Control Miscellaneous
// bits[13:11] PllLockTime = 1
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA0),  // Address
      0x00000800,                           // regData
      0x00003800,                           // regMask
    }}
  },
// F3xA4 - Reported Temperature Control
// bits[12:8] PerStepTimeDn = 0x0F
// bits[7] TmpSlewDnEn = 1
// bits[6:5] TmpMaxDiffUp = 3
// bits[4:0] PerStepTimeUp = 0x0F
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0xA4),  // Address
      0x00000FEF,                           // regData
      0x00001FFF,                           // regMask
    }}
  },
// F3x1CC - IBS Control
// bits[8] LvtOffsetVal = 1
// bits[3:0] LvtOffset = 0
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_3, 0x1CC), // Address
      0x00000100,                           // regData
      0x0000010F,                           // regMask
    }}
  },
// F4x15C - Core Performance Boost Control
// bits[1:0] BoostSrc = 0
  {
    PciRegister,
    {
      (AMD_FAMILY_15_OR | AMD_FAMILY_15_TN | 0x0000000000000800ull) ,                      // CpuFamily
      (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)                           // CpuRevision
    },
    {AMD_PF_ALL},                           // platformFeatures
    {{
      MAKE_SBDFO (0, 0, 24, FUNC_4, 0x15C), // Address
      0x00000000,                           // regData
      0x00000003,                           // regMask
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F15PciRegisterTable = {
  PrimaryCores,
  (sizeof (F15PciRegisters) / sizeof (TABLE_ENTRY_FIELDS)),
  F15PciRegisters,
};
