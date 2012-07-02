/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU AGESA Callout Functions
 *
 * Contains code to set / get useful platform information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * Description: agesaCallouts.c - AGESA Call out functions
 *
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
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Dispatcher.h"
#include "cpuServices.h"
#include "Ids.h"
#include "Filecode.h"
#include "FchPlatform.h"
#define FILECODE LEGACY_PROC_AGESACALLOUTS_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S - (AGESA ONLY)
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  Call the host environment interface to do the warm or cold reset.
 *
 *  @param[in]      ResetType       Warm or Cold Reset is requested
 *  @param[in,out]  StdHeader       Config header
 *
 */
VOID
AgesaDoReset (
  IN       UINTN             ResetType,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  AGESA_STATUS Status;
  WARM_RESET_REQUEST Request;

  // Clear warm request bit and set state bits to the current post stage
  GetWarmResetFlag (StdHeader, &Request);
  Request.RequestBit = FALSE;
  Request.StateBits = Request.PostStage;
  SetWarmResetFlag (StdHeader, &Request);

  Status =  AmdAgesaCallout (AGESA_DO_RESET, (UINT32)ResetType, (VOID *) StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Call the host environment interface to allocate buffer in main system memory.
 *
 * @param[in]      FcnData
 * @param[in,out]  AllocParams  Heap manager parameters
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaAllocateBuffer (
  IN       UINTN               FcnData,
  IN OUT   AGESA_BUFFER_PARAMS *AllocParams
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_ALLOCATE_BUFFER, (UINT32)FcnData, (VOID *) AllocParams);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to deallocate buffer in main system memory.
 *
 * @param[in]      FcnData
 * @param[in,out]  DeallocParams   Heap Manager parameters
 *
 * @return   The AGESA Status returned from the callout.
 */
AGESA_STATUS
AgesaDeallocateBuffer (
  IN       UINTN                 FcnData,
  IN OUT   AGESA_BUFFER_PARAMS  *DeallocParams
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_DEALLOCATE_BUFFER, (UINT32)FcnData, (VOID *) DeallocParams);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 * Call the host environment interface to Locate buffer Pointer in main system memory
 *
 * @param[in]      FcnData
 * @param[in,out]  LocateParams   Heap manager parameters
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaLocateBuffer (
  IN       UINTN                 FcnData,
  IN OUT   AGESA_BUFFER_PARAMS  *LocateParams
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_LOCATE_BUFFER, (UINT32)FcnData, (VOID *) LocateParams);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to launch APs
 *
 * @param[in]      ApicIdOfCore
 * @param[in,out]  LaunchApParams
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaRunFcnOnAp (
  IN        UINTN         ApicIdOfCore,
  IN        AP_EXE_PARAMS *LaunchApParams
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_RUNFUNC_ONAP, (UINT32)ApicIdOfCore, (VOID *) LaunchApParams);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to read an SPD's content.
 *
 * @param[in]      FcnData
 * @param[in,out]  ReadSpd
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaReadSpd (
  IN       UINTN                 FcnData,
  IN OUT   AGESA_READ_SPD_PARAMS *ReadSpd
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_READ_SPD, (UINT32)FcnData, ReadSpd);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to read an SPD's content.
 *
 * @param[in]      FcnData
 * @param[in,out]  ReadSpd
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaReadSpdRecovery (
  IN       UINTN                 FcnData,
  IN OUT   AGESA_READ_SPD_PARAMS *ReadSpd
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_READ_SPD_RECOVERY, (UINT32)FcnData, ReadSpd);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to provide a user hook opportunity.
 *
 * @param[in]      FcnData
 * @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaHookBeforeDramInitRecovery (
  IN       UINTN           FcnData,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY, (UINT32)FcnData, MemData);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to provide a user hook opportunity.
 *
 * @param[in]      SocketIdModuleId - (SocketID << 8) | ModuleId
 * @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaHookBeforeDramInit (
  IN       UINTN           SocketIdModuleId,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_HOOKBEFORE_DRAM_INIT, (UINT32)SocketIdModuleId, MemData);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Call the host environment interface to provide a user hook opportunity.
 *
 *  @param[in]      SocketIdModuleId - (SocketID << 8) | ModuleId
 *  @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaHookBeforeDQSTraining (
  IN       UINTN           SocketIdModuleId,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_HOOKBEFORE_DQS_TRAINING, (UINT32)SocketIdModuleId, MemData);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Call the host environment interface to provide a user hook opportunity.
 *
 *  @param[in]      FcnData
 *  @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaHookBeforeExitSelfRefresh (
  IN       UINTN           FcnData,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_HOOKBEFORE_EXIT_SELF_REF, (UINT32)FcnData, MemData);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Call the host environment interface to provide a user hook opportunity.
 *
 * @param[in]      Data
 * @param[in,out]  IdsCalloutData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */


AGESA_STATUS
AgesaGetIdsData  (
  IN       UINTN              Data,
  IN OUT   IDS_CALLOUT_STRUCT *IdsCalloutData
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_GET_IDS_INIT_DATA, (UINT32)Data, IdsCalloutData);

  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIE slot reset control
 *
 *
 *
 * @param[in]  FcnData              Function data
 * @param[in]  ResetInfo            Reset information
 * @retval     Status               Agesa status
 */

AGESA_STATUS
AgesaPcieSlotResetControl (
  IN      UINTN                 FcnData,
  IN      PCIe_SLOT_RESET_INFO  *ResetInfo
  )
{
  AGESA_STATUS Status;
  Status = AmdAgesaCallout (AGESA_GNB_PCIE_SLOT_RESET, (UINT32) FcnData, ResetInfo);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get VBIOS image
 *
 *
 *
 * @param[in]  FcnData              Function data
 * @param[in]  VbiosImageInfo       VBIOS image info
 * @retval     Status               Agesa status
 */

AGESA_STATUS
AgesaGetVbiosImage (
  IN       UINTN                 FcnData,
  IN OUT   GFX_VBIOS_IMAGE_INFO *VbiosImageInfo
  )
{
  AGESA_STATUS Status;
  Status = AmdAgesaCallout (AGESA_GNB_GFX_GET_VBIOS_IMAGE, (UINT32) FcnData, VbiosImageInfo);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * OEM callout function for FCH data override
 *
 *
 * @param[in]  FchData              FCH data pointer
 * @retval     Status               This feature is not supported
 */

AGESA_STATUS
AgesaFchOemCallout (
  IN      VOID              *FchData
  )
{
  AGESA_STATUS Status;  Status = AmdAgesaCallout(AGESA_FCH_OEM_CALLOUT, (UINT32)FchData, ((FCH_DATA_BLOCK *)FchData)->StdHeader);  return  Status;	//return AGESA_UNSUPPORTED;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * @param[in]      SocketIdModuleId - (SocketID << 8) | ModuleId
 * @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
excel331  (
  IN       UINTN           SocketIdModuleId,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (0x00028146ul , (UINT32)SocketIdModuleId, MemData);

  return Status;
}
