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
 * @e \$Revision: 39433 $   @e \$Date: 2010-10-11 14:15:36 -0700 (Mon, 11 Oct 2010) $
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
#include  "amdlib.h"
#include  "heapManager.h"
#include  "cpuLateInit.h"
#include  "Gnb.h"
#include  "GnbFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbIommuIvrs.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBIVRSLIB_GNBIVRSLIB_FILECODE
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
 * Create IVHDR entry for device range
 *
 *
 * @param[in]  StartRange      Address of start range
 * @param[in]  EndRange        Address of end range
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddDeviceRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  IVHD_GENERIC_ENTRY  *Entry;
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryStartRange;
  Entry->DeviceId = DEVICE_ID (StartRange);
  Entry->DataSetting = DataSetting;
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryEndRange;
  Entry->DeviceId = DEVICE_ID (EndRange);
  Ivhd->Length += sizeof (IVHD_GENERIC_ENTRY);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHDR entry for aliased range
 *
 *
 * @param[in]  StartRange      Address of start range
 * @param[in]  EndRange        Address of end range
 * @param[in]  Alias           Address of alias requestor ID for range
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddDeviceAliasRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       PCI_ADDR             Alias,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  IVHD_ALIAS_ENTRY    *RangeEntry;
  IVHD_GENERIC_ENTRY  *Entry;
  UINT8               Padding;
  Padding = (UINT8) ((Ivhd->Length + 0x7) & (~ 0x7));
  RangeEntry = (IVHD_ALIAS_ENTRY *) ((UINT8 *) Ivhd + Padding);
  RangeEntry->Type = IvhdEntryAliasStartRange;
  RangeEntry->DeviceId = DEVICE_ID (StartRange);
  RangeEntry->AliasDeviceId = DEVICE_ID (Alias);
  RangeEntry->DataSetting = DataSetting;
  Ivhd->Length += sizeof (IVHD_ALIAS_ENTRY);
  Entry = (IVHD_GENERIC_ENTRY *) ((UINT8 *) Ivhd + Ivhd->Length);
  Entry->Type = IvhdEntryEndRange;
  Entry->DeviceId = DEVICE_ID (EndRange);
  Ivhd->Length += (sizeof (IVHD_GENERIC_ENTRY) + Padding);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVHDR entry for special device
 *
 *
 * @param[in]  SpecialDevice   Special device Type
 * @param[in]  Device          Address of requestor ID for special device
 * @param[in]  Id              Apic ID/ Hpet ID
 * @param[in]  DataSetting     Data setting
 * @param[in]  Ivhd            Pointer to IVHD entry
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddSpecialDeviceEntry (
  IN       IVHD_SPECIAL_DEVICE  SpecialDevice,
  IN       PCI_ADDR             Device,
  IN       UINT8                Id,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  IVHD_SPECIAL_ENTRY  *SpecialEntry;
  UINT8               Padding;
  Padding = (UINT8) ((Ivhd->Length + 0x7) & (~ 0x7));
  SpecialEntry = (IVHD_SPECIAL_ENTRY *) ((UINT8 *) Ivhd + Padding);
  SpecialEntry->Type = IvhdEntrySpecialDevice;
  SpecialEntry->AliasDeviceId = DEVICE_ID (Device);
  SpecialEntry->Variety = (UINT8) SpecialDevice;
  Ivhd->Length += (sizeof (IVHD_SPECIAL_ENTRY) + Padding);
}
