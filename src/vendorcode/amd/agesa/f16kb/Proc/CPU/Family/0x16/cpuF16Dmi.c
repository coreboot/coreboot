/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD DMI Record Creation API, and related functions for Family 16h.
 *
 * Contains code that produce the DMI related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x16
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*****************************************************************************
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
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuLateInit.h"
#include "cpuF16PowerMgmt.h"
#include "cpuServices.h"
#include "cpuF16Dmi.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X16_CPUF16DMI_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  DmiF16GetMaxSpeed
 *
 *    Get the Max Speed
 *
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval  MaxSpeed   - CPU Max Speed.
 *
 */
UINT16
DmiF16GetMaxSpeed (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8 PP_FUSE_ARRAY_V2_fld36;
  UINT32 P0Frequency;
  UINT32 PciData;
  PCI_ADDR PciAddress;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_4, 0x15C);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);
  PP_FUSE_ARRAY_V2_fld36 = (UINT8) ((PciData >> 2) & 7);

  FamilyServices->GetPstateFrequency (FamilyServices, PP_FUSE_ARRAY_V2_fld36, &P0Frequency, StdHeader);
  return ((UINT16) P0Frequency);
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
