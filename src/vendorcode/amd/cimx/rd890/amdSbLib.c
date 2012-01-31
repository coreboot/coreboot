/**
 * @file
 *
 * SB common library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Common Library
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
#include "AGESA.h"
#include "amdlib.h"
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
/**
 * Stall
 *
 *
 *
 * @param[in] uSec
 *
 */

VOID
LibAmdSbStall (
  IN      UINT32      uSec,
  IN      VOID        *ConfigPtr
  )
{
  UINT16  AcpiTimerBaseAddress;
  UINT32  StartTime;
  UINT32  ElapsedTime;
  SB_INFO SbInfo;
  SbInfo = LibAmdSbGetRevisionInfo (ConfigPtr);
  if (SbInfo.Type == SB_UNKNOWN) {
    AcpiTimerBaseAddress = 0;
  } else {
    LibAmdSbPmioRead ((SbInfo.Type == SB_SB700) ? 0x24 : 0x64, AccessWidth16, &AcpiTimerBaseAddress, ConfigPtr);
  }
  if (AcpiTimerBaseAddress == 0) {
    uSec = uSec / 2;
    while (uSec != 0) {
      LibAmdIoRead (AccessWidth8, 0x80, &StartTime, (AMD_CONFIG_PARAMS *)ConfigPtr);
      uSec--;
    }
  } else {
    LibAmdIoRead (AccessWidth32, AcpiTimerBaseAddress, &StartTime, (AMD_CONFIG_PARAMS *)ConfigPtr);
    do {
      LibAmdIoRead (AccessWidth32, AcpiTimerBaseAddress, &ElapsedTime, (AMD_CONFIG_PARAMS *)ConfigPtr);
      if (ElapsedTime < StartTime) {
        ElapsedTime = ElapsedTime + (0xFFFFFFFF - StartTime);
      } else {
        ElapsedTime = ElapsedTime - StartTime;
      }
    } while ((ElapsedTime*28/100)<uSec); //28/100
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read PMIO
 *
 *
 *
 * @param[in] uSec
 *
 */

VOID
LibAmdSbPmioRead (
  IN       UINT8         Address,
  IN       ACCESS_WIDTH  Width,
     OUT   VOID          *Value,
  IN       VOID          *ConfigPtr
  )
{
  UINT8 i;
  if (Width > 0x80) {
    Width -= 0x80;
  }
  for (i = 0; i <= Width; i++) {
    LibAmdIoWrite (AccessWidth8, 0xCD6, &Address, (AMD_CONFIG_PARAMS *)ConfigPtr);   // SB_IOMAP_REGCD6
    Address++;
    LibAmdIoRead (AccessWidth8,0xCD7, (UINT8 *)Value + i, (AMD_CONFIG_PARAMS *)ConfigPtr);     // SB_IOMAP_REGCD7
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read PMIO
 *
 *
 *
 * @param[in] uSec
 *
 */

VOID
LibAmdSbPmioWrite (
  IN       UINT8         Address,
  IN       ACCESS_WIDTH  Width,
  IN       VOID          *Value,
  IN       VOID          *ConfigPtr
  )
{
  UINT8 i;
  if (Width > 0x80) {
    Width -= 0x80;
  }
  for (i = 0; i <= Width; i++) {
    LibAmdIoWrite (AccessWidth8, 0xCD6, &Address, (AMD_CONFIG_PARAMS *)ConfigPtr);    // SB_IOMAP_REGCD6
    Address++;
    LibAmdIoWrite (AccessWidth8,0xCD7, (UINT8 *)Value + i, (AMD_CONFIG_PARAMS *)ConfigPtr);     // SB_IOMAP_REGCD7
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get SB Type
 *
 *
 *
 *
 * @param[in] pConfig  Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
SB_INFO
LibAmdSbGetRevisionInfo (
  IN      VOID  *ConfigPtr
  )
{
  UINT32    DeviceId;
  UINT8     RevisionId;
  SB_INFO   SbInfo;
  PCI_ADDR  SbSmbusAddress;
  SbSmbusAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0, 0x00);
  LibAmdPciRead (AccessWidth32, SbSmbusAddress, &DeviceId, (AMD_CONFIG_PARAMS *)ConfigPtr);
  SbInfo.Revision = SB_REV_UNKNOWN;
  switch (DeviceId) {
  case 0x43851002:
    SbSmbusAddress.AddressValue = MAKE_SBDFO (0, 0, 0x14, 0, 0x08);
    LibAmdPciRead (AccessWidth8, SbSmbusAddress, &RevisionId, (AMD_CONFIG_PARAMS *)ConfigPtr);
    if (RevisionId >= 0x40) {
      SbInfo.Type = SB_SB800;
    } else {
      SbInfo.Type = SB_SB700;
    }
    break;
  default:
    SbInfo.Type = SB_UNKNOWN;
  }
  return SbInfo;
}
