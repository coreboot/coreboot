/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB UNB library
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
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "cpuServices.h"
#include  "Gnb.h"
#include  "GnbCommonLib.h"
#include  "GnbFamServices.h"
#include  "GnbPcieConfig.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBMSOCKETLIB_GNBMSOCKETLIB_FILECODE
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
 * Get Host bridge PCI Address
 *
 *
 *
 * @param[in]   GnbHandle      Socket ID
 * @param[in]   StdHeader      Standard configuration header
 * @retval                     PCI address of GNB for a given socket/silicon.
 */

PCI_ADDR
GnbFmGetPciAddress (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  GnbPciAddress;
  UINT8     NodeId;
  UINT8     Register;
  UINT32    Value;
  GnbPciAddress.AddressValue = ILLEGAL_SBDFO;
  NodeId = GnbGetNodeId (GnbHandle);
  for (Register = 0xE0; Register <= 0xEC; Register = Register + 4) {
    GnbLibPciRead (MAKE_SBDFO (0, 0, 24 + NodeId, 1, Register), AccessWidth32, &Value, StdHeader);
    if (((Value >> 4) & 0x7) == NodeId) {
      GnbPciAddress.AddressValue = MAKE_SBDFO (0, (Value >> 16) & 0xff, 0, 0, 0);
      break;
    }
  }
  ASSERT (GnbPciAddress.AddressValue != ILLEGAL_SBDFO);
  return GnbPciAddress;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get bus range decoded by GNB
 *
 * Final bus allocation can not be assumed until AmdInitMid
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  StartBusNumber  Beggining of the Bus Range
 * @param[out]  EndBusNumber    End of the Bus Range
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Satus
 */
AGESA_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *StartBusNumber,
     OUT   UINT8                      *EndBusNumber,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT8         NodeId;
  UINT8         Register;
  UINT32        Value;
  AGESA_STATUS  Status;
  Status = AGESA_ERROR;
  NodeId = GnbGetNodeId (GnbHandle);
  for (Register = 0xE0; Register <= 0xEC; Register = Register + 4) {
    GnbLibPciRead (MAKE_SBDFO (0, 0, 24 + NodeId, 1, Register), AccessWidth32, &Value, StdHeader);
    if (((Value >> 4) & 0x7) == NodeId) {
      *StartBusNumber = (UINT8) ((Value >> 16) & 0xff);
      *EndBusNumber = (UINT8) ((Value >> 24) & 0xff);
      Status = AGESA_SUCCESS;
      break;
    }
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get link to which GNB connected to
 *
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  LinkId          Link to which GNB connected to
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Satus
 */

AGESA_STATUS
GnbFmGetLinkId (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *LinkId,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32  Value;
  ASSERT (GnbHandle->NodeId != 0xFF);
  GnbLibPciRead (MAKE_SBDFO (0, 0, 0x18 + GnbHandle->NodeId, 0, 0x1A0), AccessWidth32, &Value, StdHeader);
  *LinkId = LibAmdBitScanForward (Value & 0xAAAA) / 2;
  ASSERT (*LinkId < 8);
  return AGESA_SUCCESS;
}
