/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 ROM Execution Cache Defaults
 *
 * Contains default values for ROM execution cache setup
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
#include "cpuCacheInit.h"
#include "cpuFamilyTranslation.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15CACHEDEFAULTS_FILECODE


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
VOID
GetF15CacheInfo (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **CacheInfoPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
#define MEM_TRAINING_BUFFER_SIZE  16384
#define VAR_MTRR_MASK             0x0000FFFFFFFFFFFFul
#define VAR_MTRR_MASK_CP          0x0000FFFFFFFEFFFFul

#define HEAP_BASE_MASK_CP         0x0000FFFFFFFEFF00ul
#define HEAP_BASE_MASK            0x0000FFFFFFFFFFFFul

#define SHARED_MEM_SIZE       0

CONST CACHE_INFO ROMDATA CpuF15CacheInfo =
{
  BSP_STACK_SIZE_64K,
  CORE0_STACK_SIZE,
  CORE1_STACK_SIZE,
  MEM_TRAINING_BUFFER_SIZE,
  SHARED_MEM_SIZE,
  VAR_MTRR_MASK,
  VAR_MTRR_MASK,
  HEAP_BASE_MASK,
  InfiniteExe
};

CONST CACHE_INFO ROMDATA CpuF15CacheInfoCP =
{
  BSP_STACK_SIZE_64K,
  CORE0_STACK_SIZE,
  CORE1_STACK_SIZE,
  MEM_TRAINING_BUFFER_SIZE,
  SHARED_MEM_SIZE,
  VAR_MTRR_MASK,
  VAR_MTRR_MASK_CP,
  HEAP_BASE_MASK_CP,
  InfiniteExe
};


/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the family specific properties of the cache, and its usage.
 *
 *  @CpuServiceMethod{::F_CPU_GET_FAMILY_SPECIFIC_ARRAY}.
 *
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[out]  CacheInfoPtr             Points to the cache info properties on exit.
 *  @param[out]  NumberOfElements         Will be one to indicate one entry.
 *  @param[in]   StdHeader                Header for library and services.
 *
 */
VOID
GetF15CacheInfo (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
     OUT   CONST VOID **CacheInfoPtr,
     OUT   UINT8 *NumberOfElements,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Enabled;
  UINT32 DualCore;
  UINT32 Node;
  PCI_ADDR  PciAddress;
  CPU_SPECIFIC_SERVICES *FamilyServices;
  AP_MAILBOXES     ApMailboxes;
  CORE_PAIR_MAP *CorePairMap;
  AGESA_STATUS IgnoredStatus;

  if (!IsBsp (StdHeader, &IgnoredStatus)) {
    GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);

    FamilyServices->GetApMailboxFromHardware (FamilyServices, &ApMailboxes, StdHeader);
    Node = ApMailboxes.ApMailInfo.Fields.Node;

    // Since pre-heap, get compute unit status from hardware, using mailbox info.
    PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
    PciAddress.Address.Device = PciAddress.Address.Device + Node;
    PciAddress.Address.Function = FUNC_5;
    PciAddress.Address.Register = COMPUTE_UNIT_STATUS;
    LibAmdPciReadBits (PciAddress, 3, 0, &Enabled, StdHeader);
    LibAmdPciReadBits (PciAddress, 19, 16, &DualCore, StdHeader);

    // Find the core to compute unit mapping for this node.
    CorePairMap = FamilyServices->CorePairMap;
    if ((Enabled != 0) && (CorePairMap != NULL)) {
      while (CorePairMap->Enabled != 0xFF) {
        if ((Enabled == CorePairMap->Enabled) && (DualCore == CorePairMap->DualCore)) {
          break;
        }
        CorePairMap++;
      }
      // The assert is for finding a processor configured in a way the core pair map doesn't support.
      ASSERT (CorePairMap->Enabled != 0xFF);
      switch (CorePairMap->Mapping) {
      case AllCoresMapping:
        // No cores are sharing a compute unit
        *CacheInfoPtr = &CpuF15CacheInfo;
        break;
      case EvenCoresMapping:
        // Cores are paired into compute units
        *CacheInfoPtr = &CpuF15CacheInfoCP;
        break;
      default:
        ASSERT (FALSE);
      }
    }
  } else {
    // the BSC is always just the first slice, we could return either one. Return the non for safest.
    *CacheInfoPtr = &CpuF15CacheInfo;
  }
  *NumberOfElements = 1;
}

