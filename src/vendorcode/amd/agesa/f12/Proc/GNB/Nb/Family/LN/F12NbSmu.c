/* $NoKeywords:$ */
/**
 * @file
 *
 * SMU initialization
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 47632 $   @e \$Date: 2011-02-24 13:42:20 +0800 (Thu, 24 Feb 2011) $
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
#include  "amdlib.h"
#include  "Ids.h"
//#include  "heapManager.h"
#include  "Gnb.h"
#include  "NbSmuLib.h"
#include  "F12NbSmuFirmware.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_NB_FAMILY_LN_F12NBSMU_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
F12NbSmuInitFeature (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );


/*----------------------------------------------------------------------------------------*/
/**
 * SMU Initialize
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
F12NbSmuInitFeature (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  SMU_FIRMWARE_REV  Revision;
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuInitFeature Enter\n");
  Revision = NbSmuFirmwareRevision (StdHeader);
  IDS_HDT_CONSOLE (NB_MISC, "  Current SMU firmware rev %d.%x\n", Revision.MajorRev, Revision.MinorRev);
  IDS_HDT_CONSOLE (NB_MISC, "  New SMU firmware rev %d.%x\n", Fm.Revision.MajorRev, Fm.Revision.MinorRev);
  if ((Revision.MajorRev < Fm.Revision.MajorRev) || (Revision.MajorRev == Fm.Revision.MajorRev && Revision.MinorRev < Fm.Revision.MinorRev)) {
    IDS_HDT_CONSOLE (NB_MISC, "  Updating SMU firmware\n");
    NbSmuFirmwareDownload (&Fm, StdHeader);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "NbSmuInitFeature Exit\n");
  return AGESA_SUCCESS;
}
