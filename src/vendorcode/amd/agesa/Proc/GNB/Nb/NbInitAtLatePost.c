/* $NoKeywords:$ */
/**
 * @file
 *
 * NB late POST init interface
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:
 * @e sub-project:
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * ***************************************************************************
 *
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbNbInitLibV1)
#include  "NbConfigData.h"
#include  "NbPowerMgmt.h"
#include  "NbInitAtLatePost.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_NB_NBINITATLATEPOST_FILECODE
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
 * Init GNB at Late Post
 *
 *
 *
 * @param[in]   StdHeader       Standard configuration header
 * @retval      AGESA_STATUS
 */

AGESA_STATUS
NbInitAtLatePost (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GNB_PLATFORM_CONFIG   Gnb;
  UINT32                NumberOfSockets;
  UINT32                SocketId;
  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "NbInitAtLatePost Enter\n");
  Status = NbAllocateConfigData (StdHeader, &Gnb);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  NumberOfSockets = GnbGetNumberOfSockets (StdHeader);
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    UINT32  NumberOfSilicons;
    UINT32  SiliconId;
    if (!GnbIsDevicePresentInSocket (SocketId, StdHeader)) {
      continue;
    }
    NumberOfSilicons = GnbGetNumberOfSiliconsOnSocket (SocketId, StdHeader);
    for (SiliconId = 0; SiliconId < NumberOfSilicons; SiliconId++) {
      Gnb.GnbPciAddress = GnbGetPciAddress (SocketId, SiliconId, StdHeader);
      Status = NbInitPowerManagement (&Gnb);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
      GnbLock (Gnb.GnbPciAddress, StdHeader);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "NbInitAtLatePost Exit[0x%x]\n", Status);
  return Status;
}
