/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 specific utility functions.
 *
 * Provides numerous utility functions specific to family 15h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 44737 $   @e \$Date: 2011-01-05 00:59:55 -0700 (Wed, 05 Jan 2011) $
 *
 */
/*
 *****************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuCommonF15Utilities.h"
#include "cpuF15PowerMgmt.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUCOMMONF15UTILITIES_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

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

/*---------------------------------------------------------------------------------------*/
/**
 *  Set warm reset status and count
 *
 *  @CpuServiceMethod{::F_CPU_SET_WARM_RESET_FLAG}.
 *
 *  This function will use bit9, and bit 10 of register F0x6C as a warm reset status and count.
 *
 *  @param[in]  FamilySpecificServices   The current Family Specific Services.
 *  @param[in]  StdHeader                Handle of Header for calling lib functions and services.
 *  @param[in]  Request                  Indicate warm reset status
 *
 */
VOID
F15SetAgesaWarmResetFlag (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       WARM_RESET_REQUEST *Request
  )
{
  PCI_ADDR  PciAddress;
  UINT32    PciData;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_0, HT_INIT_CTRL);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

  // bit[5] - indicate a warm reset is or is not required
  PciData &= ~(HT_INIT_BIOS_RST_DET_0);
  PciData = PciData | (Request->RequestBit << 5);

  // bit[10,9] - indicate warm reset status and count
  PciData &= ~(HT_INIT_BIOS_RST_DET_1 | HT_INIT_BIOS_RST_DET_2);
  PciData |= Request->StateBits << 9;

  LibAmdPciWrite (AccessWidth32, PciAddress, &PciData, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Get warm reset status and count
 *
 *  @CpuServiceMethod{::F_CPU_GET_WARM_RESET_FLAG}.
 *
 *  This function will bit9, and bit 10 of register F0x6C as a warm reset status and count.
 *
 *  @param[in]  FamilySpecificServices   The current Family Specific Services.
 *  @param[in]  StdHeader                Config handle for library and services
 *  @param[out] Request                  Indicate warm reset status
 *
 */
VOID
F15GetAgesaWarmResetFlag (
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS *StdHeader,
     OUT   WARM_RESET_REQUEST *Request
  )
{
  PCI_ADDR  PciAddress;
  UINT32    PciData;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0 , PCI_DEV_BASE, FUNC_0, HT_INIT_CTRL);
  LibAmdPciRead (AccessWidth32, PciAddress, &PciData, StdHeader);

  // bit[5] - indicate a warm reset is or is not required
  Request->RequestBit = (UINT8) ((PciData & HT_INIT_BIOS_RST_DET_0) >> 5);
  // bit[10,9] - indicate warm reset status and count
  Request->StateBits = (UINT8) ((PciData & (HT_INIT_BIOS_RST_DET_1 | HT_INIT_BIOS_RST_DET_2)) >> 9);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Return a number zero or one, based on the Core ID position in the initial APIC Id.
 *
 * @CpuServiceMethod{::F_CORE_ID_POSITION_IN_INITIAL_APIC_ID}.
 *
 * @param[in]     FamilySpecificServices  The current Family Specific Services.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @retval        CoreIdPositionZero      Core Id is not low
 * @retval        CoreIdPositionOne       Core Id is low
 */
CORE_ID_POSITION
F15CpuAmdCoreIdPositionInInitialApicId (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64 InitApicIdCpuIdLo;

  //  Check bit_54 [InitApicIdCpuIdLo] to find core id position.
  LibAmdMsrRead (MSR_NB_CFG, &InitApicIdCpuIdLo, StdHeader);
  InitApicIdCpuIdLo = ((InitApicIdCpuIdLo & BIT54) >> 54);
  return ((InitApicIdCpuIdLo == 0) ? CoreIdPositionZero : CoreIdPositionOne);
}

