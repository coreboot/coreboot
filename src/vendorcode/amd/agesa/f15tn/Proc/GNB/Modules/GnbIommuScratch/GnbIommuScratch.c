/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "S3SaveState.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbFamServices.h"
#include  "GnbRegistersTN.h"
#include  "heapManager.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBIOMMUSCRATCH_GNBIOMMUSCRATCH_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Set Iommu Scratch Memory Range
 * 1) code needs to be executed at Late Init
 * 2) Allocate heap using  heap type HEAP_RUNTIME_SYSTEM_MEM
 * 3) Allocate enough memory to be able to get address aligned required by register
 * 4) Assign same address to all Gnb in system
 *
 *
 * @param[in]  StdHeader       Standard configuration header
 */

AGESA_STATUS
GnbIommuScratchMemoryRangeInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  AGESA_STATUS          Status;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;
  UINT32                AddressLow;
  UINT32                AddressHigh;
  GNB_HANDLE            *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuScratchMemoryRangeInterface Enter\n");

  AllocHeapParams.RequestedBufferSize = 128;
  AllocHeapParams.BufferHandle = AMD_GNB_IOMMU_SCRATCH_MEM_HANDLE;
  AllocHeapParams.Persist = HEAP_RUNTIME_SYSTEM_MEM;
  Status = HeapAllocateBuffer (&AllocHeapParams, StdHeader);
  if (Status != AGESA_SUCCESS) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Iommu Scratch Memory not allocated.\n");
    ASSERT (FALSE);
    return AGESA_FATAL;
  }

  AddressLow = (((UINT32) ((UINT64) AllocHeapParams.BufferPtr)) + 0x3F) & D0F0x98_x27_IOMMUUrAddr_31_6__MASK;
  AddressHigh = ((UINT32) (((UINT64) AllocHeapParams.BufferPtr) >> 32)) & D0F0x98_x26_IOMMUUrAddr_39_32__MASK;

  GnbHandle = GnbGetHandle (StdHeader);
  while (GnbHandle != NULL) {
    if (GnbFmCheckIommuPresent (GnbHandle, StdHeader)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Set Iommu Scratch Memory for Socket %d Silicon %d\n", GnbGetSocketId (GnbHandle) , GnbGetSiliconId (GnbHandle));
      GnbLibPciIndirectWrite (
        GnbHandle->Address.AddressValue | D0F0x94_ADDRESS,
        D0F0x98_x27_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
        AccessS3SaveWidth32,
        &AddressLow,
        StdHeader);

      GnbLibPciIndirectWrite (
        GnbHandle->Address.AddressValue | D0F0x94_ADDRESS,
        D0F0x98_x26_ADDRESS | (1 << D0F0x94_OrbIndWrEn_OFFSET),
        AccessS3SaveWidth32,
        &AddressHigh,
        StdHeader);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuScratchMemoryRangeInterface Exit\n");

  return AGESA_SUCCESS;
}
