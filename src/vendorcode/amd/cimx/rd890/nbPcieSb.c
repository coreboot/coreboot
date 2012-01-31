/**
 * @file
 *
 * PCIe support for misc Southbridges.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
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
 *
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"
#include "amdSbLib.h"

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

/*----------------------------------------------------------------------------------------*/
/*
 * Set up NB-SB virtual channel for audio traffic
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */

/*----------------------------------------------------------------------------------------*/
/*
 * Set up NB-SB virtual channel for audio traffic
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieSbSetupVc (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  UINT16        AlinkPort;

  Status = PcieSbAgetAlinkIoAddress (&AlinkPort, pConfig);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }

  LibNbIoRMW (AlinkPort, AccessS3SaveWidth32, 0x0,  0x80000124, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xffffff01, 0, pConfig);
  LibNbIoRMW (AlinkPort, AccessS3SaveWidth32 , 0x0, 0x80000130, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, (UINT32)~(BIT24 + BIT25 + BIT26), 0xFE + BIT24, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xffffffff, BIT31, pConfig);
  return AGESA_SUCCESS;
}
/*----------------------------------------------------------------------------------------*/
/*
 * Set up NB-SB virtual channel for audio traffic
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
VOID
PcieSbEnableVc (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  UINT16        AlinkPort;
  Status = PcieSbAgetAlinkIoAddress (&AlinkPort, pConfig);
  if (Status != AGESA_SUCCESS) {
    return;
  }
  LibNbIoRMW (AlinkPort, AccessS3SaveWidth32, 0x0, 0xC0000050, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xffffffff, BIT3, pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init SB ASPM.
 * Enable ASPM states on SB
 *
 *
 * @param[in] Lx       Lx ASPM bitmap. Lx[0] - L0s enable. Lx[1] - L1 enable.
 * @param[in] pConfig  Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieSbInitAspm (
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  UINT16        AlinkPort;

  Status = PcieSbAgetAlinkIoAddress (&AlinkPort, pConfig);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  LibNbIoRMW (AlinkPort, AccessS3SaveWidth32, 0x0, 0x40000038, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0x0, 0xA0, pConfig);
  LibNbIoRMW (AlinkPort  , AccessS3SaveWidth32, 0x0, 0x4000003c, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xffff00ff, 0x6900, pConfig );
  LibNbIoRMW (AlinkPort  , AccessS3SaveWidth32, 0x0, 0x80000068, pConfig);
  LibNbIoRMW (AlinkPort + 4, AccessS3SaveWidth32, 0xffffffff, Lx, pConfig);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Alink config address
 *
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieSbAgetAlinkIoAddress (
     OUT   UINT16         *AlinkPort,
  IN       AMD_NB_CONFIG  *pConfig
  )
{
  SB_INFO SbInfo;
  SbInfo = LibAmdSbGetRevisionInfo ((pConfig == NULL)?NULL:GET_BLOCK_CONFIG_PTR (pConfig));
  if (SbInfo.Type == SB_UNKNOWN) {
    return AGESA_UNSUPPORTED;
  }
  if (SbInfo.Type == SB_SB700) {
    LibNbPciRead (MAKE_SBDFO (0, 0, 0x14, 0, 0xf0), AccessWidth16, AlinkPort, pConfig);
  } else {
    LibAmdSbPmioRead (0xE0, AccessWidth16, AlinkPort, NULL);
  }
  if (*AlinkPort == 0) {
    return AGESA_UNSUPPORTED;
  }
  return  AGESA_SUCCESS;
}