/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 49994 $   @e \$Date: 2011-03-31 15:44:04 +0800 (Thu, 31 Mar 2011) $
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include  "AGESA.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "OptionGnb.h"
#include  "GnbPcie.h"
#include  "GnbGfx.h"
#include  "cpuLateInit.h"
#include  "GnbCommonLib.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxInitLibV1.h"
#include  "F12PcieAlibSsdt.h"
#include  "GnbPcieFamServices.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_FAMILY_LN_F12PCIEALIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS GnbBuildOptions;

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
 * Build ALIB ACPI table
 *
 *
 *
 * @param[in,out] AlibSsdtPtr     Pointer to ALIB SSDT table
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_FATAL
 */

AGESA_STATUS
PcieFmAlibBuildAcpiTable (
  IN       VOID                          *AlibSsdtPtr,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  UINT32                AmlObjName;
  GFX_PLATFORM_CONFIG   *Gfx;
  VOID                  *AmlObjPtr;
  BOOLEAN               AltVddNbSupport;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmAlibBuildAcpiTable Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  AltVddNbSupport = TRUE;
//  AmlObjName = 'A0DA';
  AmlObjName = Int32FromChar ('A', '0', 'D', 'A');
  AmlObjPtr = GnbLibFind (AlibSsdtPtr, ((ACPI_TABLE_HEADER*) &AlibSsdt[0])->TableLength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
  ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr != NULL) {
    Status =  GfxLocateConfigData (StdHeader, &Gfx);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    ASSERT (Status == AGESA_SUCCESS);
    if ((Status != AGESA_SUCCESS) || (GnbBuildOptions.CfgAltVddNb == FALSE) || (Gfx->UmaInfo.MemClock > DDR1333_FREQUENCY) ||
        ((Gfx->GfxDiscreteCardInfo.AmdPcieGfxCardBitmap != 0) && GfxLibIsControllerPresent (StdHeader))) {
      AltVddNbSupport = FALSE;
    }
    // CBS/IDS can change AltVddNbSupport
    IDS_OPTION_HOOK (IDS_GNB_ALTVDDNB, &AltVddNbSupport, StdHeader);
    if (!AltVddNbSupport) {
      IDS_HDT_CONSOLE (GNB_TRACE, " AltVddNb - Disabled\n");
      *(UINT8*)((UINT8*) AmlObjPtr + 5) = 0;
    }
  } else {
    AgesaStatus = AGESA_ERROR;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmAlibBuildAcpiTable Exit[0x%x]\n", AgesaStatus);
  return AgesaStatus;
}
