/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB Single Socket Library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 85947 $   @e \$Date: 2013-01-14 17:25:21 -0600 (Mon, 14 Jan 2013) $
 *
 */
/*
*****************************************************************************
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
#include  "cpuFamilyTranslation.h"
#include  "cpuServices.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbLib.h"
#include  "GnbLibPciAcc.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBSSOCKETLIB_GNBSSOCKETLIB_FILECODE
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
PCI_ADDR
GnbFmGetPciAddress (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

AGESA_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *StartBusNumber,
     OUT   UINT8                      *EndBusNumber,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

AGESA_STATUS
GnbFmGetLinkId (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *LinkId,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Get Host bridge PCI Address
 *
 *
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      PCI address of GNB for a given socket/silicon.
 */

PCI_ADDR
GnbFmGetPciAddress (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  Gnb;
  Gnb.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  return Gnb;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get bus range decoded by GNB
 *
 * Final bus allocation can not be assumed until AmdInitMid
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  StartBusNumber  Beginning of the Bus Range
 * @param[out]  EndBusNumber    End of the Bus Range
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Status
 */

AGESA_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *StartBusNumber,
     OUT   UINT8                      *EndBusNumber,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  *StartBusNumber = 0x0;
  *EndBusNumber = 0xff;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get link to which GNB connected to
 *
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  LinkId          Link to which GNB connected to
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Status
 */

AGESA_STATUS
GnbFmGetLinkId (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   UINT8                      *LinkId,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{

  *LinkId = 0x00;
  return AGESA_SUCCESS;
}
