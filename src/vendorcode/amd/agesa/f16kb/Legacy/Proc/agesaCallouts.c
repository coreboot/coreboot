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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*****************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * Description: agesaCallouts.c - AGESA Call out functions
 *
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

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "cpuRegisters.h"
#include "Dispatcher.h"
#include "cpuServices.h"
#include "Ids.h"
#include "FchPlatform.h"
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
  AGESA_STATUS Status;
  Status = AmdAgesaCallout(AGESA_FCH_OEM_CALLOUT, (UINT32)FchData, ((FCH_DATA_BLOCK *)FchData)->StdHeader);
  return  Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Optional call to the host environment interface to change the external Vref for 2D Training.
 *
 * @param[in]      SocketIdModuleId - (SocketID << 8) | ModuleId
 * @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaExternal2dTrainVrefChange (
  IN       UINTN           SocketIdModuleId,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status = AGESA_UNSUPPORTED;


  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Call to the host environment interface to change an external Voltage
 *
 * @param[in]      SocketIdModuleId - (SocketID << 8) | ModuleId
 * @param[in,out]  *AdjustValue - Pointer to VOLTAGE_ADJUST structure
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaExternalVoltageAdjust (
  IN       UINTN           SocketIdModuleId,
  IN OUT   VOLTAGE_ADJUST *AdjustValue
  )
{
  AGESA_STATUS Status;

  Status =  AmdAgesaCallout (AGESA_EXTERNAL_VOLTAGE_ADJUST, (UINT32)SocketIdModuleId, AdjustValue);

  return Status;
}
