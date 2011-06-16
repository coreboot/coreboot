/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Register Table Related Functions
 *
 * Set registers according to a set of register tables
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 50057 $   @e \$Date: 2011-04-01 13:30:57 +0800 (Fri, 01 Apr 2011) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "OptionMultiSocket.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "Table.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuFeatures.h"
#include "CommonReturns.h"
#include "cpuL3Features.h"
#include "cpuEarlyInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_TABLE_FILECODE

extern OPTION_MULTISOCKET_CONFIGURATION OptionMultiSocketConfiguration;

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
VOID
SetRegistersFromTablesAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

extern BUILD_OPT_CFG UserOptions;
extern CPU_FAMILY_SUPPORT_TABLE L3FeatureFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 * An iterator for all the Family and Model Register Tables.
 *
 * RegisterTableHandle should be set to NULL to begin iteration, the first time the method is
 * invoked.  Register tables can be processed, until this method returns NULL.  RegisterTableHandle
 * should simply be passed back to the method without modification or use by the caller.
 * The table selector allows the relevant tables for different cores to be iterated, if the family separates
 * tables.  For example, MSRs can be in a table processed by all cores and PCI registers in a table processed by
 * primary cores.
 *
 * @param[in]     FamilySpecificServices  The current Family Specific Services.
 * @param[in]     Selector                Select whether to iterate over tables for either all cores, primary cores, bsp, ....
 * @param[in,out] RegisterTableHandle     IN: The handle of the current register table, or NULL if Begin.
 *                                        OUT: The handle of the next register table, if not End.
 * @param[out]    NumberOfEntries         The number of entries in the table returned, if not End.
 * @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 * @return        The pointer to the next Register Table, or NULL if End.
 */
TABLE_ENTRY_FIELDS
STATIC
*GetNextRegisterTable (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       TABLE_CORE_SELECTOR     Selector,
  IN OUT   REGISTER_TABLE       ***RegisterTableHandle,
     OUT   UINTN                  *NumberOfEntries,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  REGISTER_TABLE **NextTable;
  TABLE_ENTRY_FIELDS *Entries;

  ASSERT ((FamilySpecificServices != NULL) && (StdHeader != NULL));
  ASSERT (Selector < TableCoreSelectorMax);

  NextTable = *RegisterTableHandle;
  if (NextTable == NULL) {
    // Begin
    NextTable = FamilySpecificServices->RegisterTableList;
    IDS_OPTION_HOOK (IDS_REG_TABLE, &NextTable, StdHeader);
  } else {
    NextTable++;
  }
  // skip if not selected
  while ((*NextTable != NULL) && (*NextTable)->Selector != Selector) {
    NextTable++;
  }
  if (*NextTable == NULL) {
    // End
    *RegisterTableHandle = NULL;
    Entries = NULL;
  } else {
    // Iterate next table
    *RegisterTableHandle = NextTable;
    *NumberOfEntries = (*NextTable)->NumberOfEntries;
    Entries = (TABLE_ENTRY_FIELDS *) (*NextTable)->Table;
  }
  return Entries;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Compare counts to a pair of ranges.
 *
 * @param[in]       FirstCount       The actual count to be compared to the first range.
 * @param[in]       SecondCount      The actual count to be compared to the second range.
 * @param[in]       Ranges           The ranges which the counts are compared to.
 *
 * @retval          TRUE             Either one, or both, of the counts is in the range given.
 * @retval          FALSE            Neither count is in the range given.
 */
BOOLEAN
IsEitherCountInRange (
  IN       UINTN                FirstCount,
  IN       UINTN                SecondCount,
  IN       COUNT_RANGE_FEATURE  Ranges
  )
{
  // Errors: Entire Range value is zero, Min and Max reversed or not <=, ranges overlap (OK if first range is all),
  // the real counts are too big.
  ASSERT ((Ranges.Range0Min <= Ranges.Range0Max) &&
          (Ranges.Range1Min <= Ranges.Range1Max) &&
          (Ranges.Range0Max != 0) &&
          (Ranges.Range1Max != 0) &&
          ((Ranges.Range0Max == COUNT_RANGE_HIGH) || (Ranges.Range0Max < Ranges.Range1Min)) &&
          ((FirstCount < COUNT_RANGE_HIGH) && (SecondCount < COUNT_RANGE_HIGH)));

  return (BOOLEAN) (((FirstCount <= Ranges.Range0Max) && (FirstCount >= Ranges.Range0Min)) ||
                   ((SecondCount <= Ranges.Range1Max) && (SecondCount >= Ranges.Range1Min)));
}

/*-------------------------------------------------------------------------------------*/
/**
 * Returns the performance profile features list of the currently running processor core.
 *
 * @param[out]      Features          The performance profile features supported by this platform
 * @param[in]       PlatformConfig    Config handle for platform specific information
 * @param[in]       StdHeader         Header for library and services
 *
 */
VOID
GetPerformanceFeatures (
     OUT   PERFORMANCE_PROFILE_FEATS    *Features,
  IN       PLATFORM_CONFIGURATION       *PlatformConfig,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  )
{
  CPUID_DATA  CpuidDataStruct;
  CPU_SPECIFIC_SERVICES  *FamilySpecificServices;
  L3_FEATURE_FAMILY_SERVICES *FeatureFamilyServices;

  Features->PerformanceProfileValue = 0;
  // Reflect Probe Filter Configuration.
  Features->PerformanceProfileFeatures.ProbeFilter = 0;
  if (IsFeatureEnabled (L3Features, PlatformConfig, StdHeader)) {
    GetFeatureServicesOfCurrentCore (&L3FeatureFamilyServiceTable, (const VOID **) &FeatureFamilyServices, StdHeader);
    if ((FeatureFamilyServices != NULL) &&
        (FeatureFamilyServices->IsHtAssistSupported (FeatureFamilyServices, PlatformConfig, StdHeader))) {
      Features->PerformanceProfileFeatures.ProbeFilter = 1;
    }
  }

  // Reflect Display Refresh Requests use 32 bytes Configuration.
  Features->PerformanceProfileFeatures.RefreshRequest32Byte = 0;
  if (PlatformConfig->PlatformProfile.Use32ByteRefresh) {
    Features->PerformanceProfileFeatures.RefreshRequest32Byte = 1;
  }
  // Reflect Mct Isoc Read Priority set to variable Configuration.
  Features->PerformanceProfileFeatures.MctIsocVariable = 0;
  if (PlatformConfig->PlatformProfile.UseVariableMctIsocPriority) {
    Features->PerformanceProfileFeatures.MctIsocVariable = 1;
  }
  // Indicate if this boot is a warm reset.
  Features->PerformanceProfileFeatures.IsWarmReset = 0;
  if (IsWarmReset (StdHeader)) {
    Features->PerformanceProfileFeatures.IsWarmReset = 1;
  }

  // Get L3 Cache present as indicated by CPUID
  Features->PerformanceProfileFeatures.L3Cache = 0;
  Features->PerformanceProfileFeatures.NoL3Cache = 1;
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &CpuidDataStruct, StdHeader);
  if (((CpuidDataStruct.EDX_Reg & 0xFFFC0000) >> 18) != 0) {
    Features->PerformanceProfileFeatures.L3Cache = 1;
    Features->PerformanceProfileFeatures.NoL3Cache = 0;
  }

  // Get VRM select high speed from build option.
  Features->PerformanceProfileFeatures.VrmHighSpeed = 0;
  if (PlatformConfig->VrmProperties[CoreVrm].HiSpeedEnable) {
    Features->PerformanceProfileFeatures.VrmHighSpeed = 1;
  }

  // Get some family, model specific performance type info.
  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  ASSERT (FamilySpecificServices != NULL);

  // Is the Northbridge P-State feature enabled
  Features->PerformanceProfileFeatures.NbPstates = 0;
  if (FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, PlatformConfig, StdHeader)) {
    Features->PerformanceProfileFeatures.NbPstates = 1;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the MSR Register Entry.
 *
 * @TableEntryTypeMethod{::MsrRegister}.
 *
 * Read - Modify - Write the MSR, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The MSR register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForMsrEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64          MsrData;

  // Even for only single bit fields, use those in the mask.  "Mask nothing" is a bug, even if just by policy.
  ASSERT (Entry->MsrEntry.Mask != 0);

  LibAmdMsrRead (Entry->MsrEntry.Address, &MsrData, StdHeader);
  MsrData = MsrData & (~(Entry->MsrEntry.Mask));
  MsrData = MsrData | Entry->MsrEntry.Data;
  LibAmdMsrWrite (Entry->MsrEntry.Address, &MsrData, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the PCI Register Entry.
 *
 * @TableEntryTypeMethod{::PciRegister}.
 *
 * Make the current core's PCI address with the function and register for the entry.
 * Read - Modify - Write the PCI register, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForPciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32          TempVar32_a;
  UINT32          MySocket;
  UINT32          MyModule;
  UINT32          Ignored;
  PCI_ADDR        MyPciAddress;
  AGESA_STATUS    IgnoredSts;
  TABLE_ENTRY_DATA  PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  // Even for only single bit fields, use those in the mask.  "Mask nothing" is a bug, even if just by policy.
  ASSERT ((Entry->InitialValues[4] == 0) &&
          (Entry->InitialValues[3] == 0) &&
          (Entry->PciEntry.Mask != 0));

  LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
  PciEntry.PciEntry = Entry->PciEntry;

  IDS_OPTION_HOOK (IDS_SET_PCI_REGISTER_ENTRY, &PciEntry, StdHeader);

  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredSts);
  GetPciAddress (StdHeader, MySocket, MyModule, &MyPciAddress, &IgnoredSts);
  MyPciAddress.Address.Function = PciEntry.PciEntry.Address.Address.Function;
  MyPciAddress.Address.Register = PciEntry.PciEntry.Address.Address.Register;
  LibAmdPciRead (AccessWidth32, MyPciAddress, &TempVar32_a, StdHeader);
  TempVar32_a = TempVar32_a & (~(PciEntry.PciEntry.Mask));
  TempVar32_a = TempVar32_a | PciEntry.PciEntry.Data;
  LibAmdPciWrite (AccessWidth32, MyPciAddress, &TempVar32_a, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Family Specific Workaround Register Entry.
 *
 * @TableEntryTypeMethod{::FamSpecificWorkaround}.
 *
 * Call the function, passing the data.
 *
 * See if you can use the other entries or make an entry that covers the fix.
 * After all, the purpose of having a table entry is to @b NOT have code which
 * isn't generic feature code, but is family/model code specific to one case.
 *
 * @param[in]     Entry             The Family Specific Workaround register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForFamSpecificWorkaroundEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  ASSERT (Entry->FamSpecificEntry.DoAction != NULL);

  Entry->FamSpecificEntry.DoAction (Entry->FamSpecificEntry.Data, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Program HT Phy PCI registers using BKDG values.
 *
 * @TableEntryTypeMethod{::HtPhyRegister}.
 *
 *
 * @param[in]       Entry               The type specific entry data to be implemented (that is written).
 * @param[in]       PlatformConfig      Config handle for platform specific information
 * @param[in]       StdHeader           Config params for library, services.
 *
 */
VOID
SetRegisterForHtPhyEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  PCI_ADDR              CapabilitySet;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  BOOLEAN               MatchedSublink1;
  HT_FREQUENCIES        Freq0;
  HT_FREQUENCIES        Freq1;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT ((Entry->InitialValues[4] == 0) &&
          ((Entry->HtPhyEntry.TypeFeats.HtPhyLinkValue & ~(HTPHY_LINKTYPE_ALL | HTPHY_LINKTYPE_SL0_AND | HTPHY_LINKTYPE_SL1_AND)) == 0) &&
          (Entry->HtPhyEntry.Address < HTPHY_REGISTER_MAX));

  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  Link = 0;
  while (FamilySpecificServices->NextLinkHasHtPhyFeats (
           FamilySpecificServices,
           &CapabilitySet,
           &Link,
           &Entry->HtPhyEntry.TypeFeats,
           &MatchedSublink1,
           &Freq0,
           &Freq1,
           StdHeader)) {
    FamilySpecificServices->SetHtPhyRegister (FamilySpecificServices, &Entry->HtPhyEntry, CapabilitySet, Link, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Program a range of HT Phy PCI registers using BKDG values.
 *
 * @TableEntryTypeMethod{::HtPhyRangeRegister}.
 *
 *
 * @param[in]       Entry               The type specific entry data to be implemented (that is written).
 * @param[in]       PlatformConfig      Config handle for platform specific information
 * @param[in]       StdHeader           Config params for library, services.
 *
 */
VOID
SetRegisterForHtPhyRangeEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  PCI_ADDR              CapabilitySet;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  HT_PHY_TYPE_ENTRY_DATA CurrentHtPhyRegister;
  BOOLEAN                MatchedSublink1;
  HT_FREQUENCIES        Freq0;
  HT_FREQUENCIES        Freq1;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->HtPhyRangeEntry.TypeFeats.HtPhyLinkValue & ~(HTPHY_LINKTYPE_ALL)) == 0) &&
          (Entry->HtPhyRangeEntry.LowAddress <= Entry->HtPhyRangeEntry.HighAddress) &&
          (Entry->HtPhyRangeEntry.HighAddress < HTPHY_REGISTER_MAX) &&
          (Entry->HtPhyRangeEntry.HighAddress != 0));

  CurrentHtPhyRegister.Mask = Entry->HtPhyRangeEntry.Mask;
  CurrentHtPhyRegister.Data = Entry->HtPhyRangeEntry.Data;
  CurrentHtPhyRegister.TypeFeats = Entry->HtPhyRangeEntry.TypeFeats;

  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  Link = 0;
  while (FamilySpecificServices->NextLinkHasHtPhyFeats (
           FamilySpecificServices,
           &CapabilitySet,
           &Link,
           &Entry->HtPhyRangeEntry.TypeFeats,
           &MatchedSublink1,
           &Freq0,
           &Freq1,
           StdHeader)) {
    for (CurrentHtPhyRegister.Address = Entry->HtPhyRangeEntry.LowAddress;
         CurrentHtPhyRegister.Address <= Entry->HtPhyRangeEntry.HighAddress;
         CurrentHtPhyRegister.Address++) {
      FamilySpecificServices->SetHtPhyRegister (FamilySpecificServices, &CurrentHtPhyRegister, CapabilitySet, Link, StdHeader);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Is PackageLink an Internal Link?
 *
 * This is a test for the logical link match codes in the user interface, not a test for
 * the actual northbridge links.
 *
 * @param[in]    PackageLink   The link
 *
 * @retval       TRUE          This is an internal link
 * @retval       FALSE         This is not an internal link
 */
BOOLEAN
STATIC
IsDeemphasisLinkInternal (
  IN       UINT32  PackageLink
  )
{
  return (BOOLEAN) ((PackageLink <= HT_LIST_MATCH_INTERNAL_LINK_2) && (PackageLink >= HT_LIST_MATCH_INTERNAL_LINK_0));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get the Package Link number, for the current node and real link number.
 *
 * Based on the link to package link mapping from BKDG, look up package link for
 * the input link on the internal node number corresponding to the current core's node.
 * For single module processors, the northbridge link and package link are the same.
 *
 * @param[in]   Link                      the link on the current node.
 * @param[in]   FamilySpecificServices    CPU specific support interface.
 * @param[in]   StdHeader                 Config params for library, services.
 *
 * @return      the Package Link, HT_LIST_TERMINAL Not connected in package, HT_LIST_MATCH_INTERNAL_LINK package internal link.
 *
 */
UINT32
STATIC
LookupPackageLink (
  IN       UINT32                 Link,
  IN       CPU_SPECIFIC_SERVICES *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32 PackageLinkMapItem;
  UINT32 PackageLink;
  AP_MAIL_INFO ApMailbox;

  PackageLink = HT_LIST_TERMINAL;

  GetApMailbox (&ApMailbox.Info, StdHeader);

  if (ApMailbox.Fields.ModuleType != 0) {
    ASSERT (FamilySpecificServices->PackageLinkMap != NULL);
    // Use table to find this module's package link
    PackageLinkMapItem = 0;
    while ((*FamilySpecificServices->PackageLinkMap)[PackageLinkMapItem].Link != HT_LIST_TERMINAL) {
      if (((*FamilySpecificServices->PackageLinkMap)[PackageLinkMapItem].Module == ApMailbox.Fields.Module) &&
          ((*FamilySpecificServices->PackageLinkMap)[PackageLinkMapItem].Link == Link)) {
        PackageLink = (*FamilySpecificServices->PackageLinkMap)[PackageLinkMapItem].PackageLink;
        break;
      }
      PackageLinkMapItem++;
    }
  } else {
    PackageLink = Link;
  }
  return PackageLink;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get the platform's specified deemphasis levels for the current link.
 *
 * Search the platform's list for a match to the current link and also matching frequency.
 * If a match is found, use the specified deemphasis levels.
 *
 * @param[in]       Socket                    The current Socket.
 * @param[in]       Link                      The link on that socket.
 * @param[in]       Frequency                 The frequency the link is set to.
 * @param[in]       PlatformConfig            Config handle for platform specific information
 * @param[in]       FamilySpecificServices    CPU specific support interface.
 * @param[in]       StdHeader                 Config params for library, services.
 *
 * @return          The Deemphasis values for the link.
 */
UINT32
STATIC
GetLinkDeemphasis (
  IN       UINT32                  Socket,
  IN       UINT32                  Link,
  IN       HT_FREQUENCIES          Frequency,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 Result;
  CPU_HT_DEEMPHASIS_LEVEL *Match;
  UINT32 PackageLink;

  PackageLink = LookupPackageLink (Link, FamilySpecificServices, StdHeader);
  // All External and Internal links have deemphasis level none as the default.
  // However, it is expected that the platform BIOS will provide deemphasis levels for the external links.
  Result = ((DCV_LEVEL_NONE) | (DEEMPHASIS_LEVEL_NONE));

  if (PlatformConfig->PlatformDeemphasisList != NULL) {
    Match = PlatformConfig->PlatformDeemphasisList;
    while (Match->Socket != HT_LIST_TERMINAL) {
      if (((Match->Socket == Socket) || (Match->Socket == HT_LIST_MATCH_ANY)) &&
          ((Match->Link == PackageLink) ||
           ((Match->Link == HT_LIST_MATCH_ANY) && (!IsDeemphasisLinkInternal (PackageLink))) ||
           ((Match->Link == HT_LIST_MATCH_INTERNAL_LINK) && (IsDeemphasisLinkInternal (PackageLink)))) &&
          ((Match->LoFreq <= Frequency) && (Match->HighFreq >= Frequency))) {
        // Found a match, get the deemphasis value.
        ASSERT ((MaxPlatformDeemphasisLevel > Match->DcvDeemphasis) | (MaxPlatformDeemphasisLevel > Match->ReceiverDeemphasis));
        Result = ((1 << Match->DcvDeemphasis) | (1 << Match->ReceiverDeemphasis));
        break;
      } else {
        Match++;
      }
    }
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Program Deemphasis registers using BKDG values, for the platform specified levels.
 *
 * @TableEntryTypeMethod{::DeemphasisRegister}.
 *
 *
 * @param[in]    Entry            The type specific entry data to be implemented (that is written).
 * @param[in]    PlatformConfig   Config handle for platform specific information
 * @param[in]    StdHeader        Config params for library, services.
 *
 */
VOID
SetRegisterForDeemphasisEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  PCI_ADDR              CapabilitySet;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  BOOLEAN               MatchedSublink1;
  HT_FREQUENCIES        Freq0;
  HT_FREQUENCIES        Freq1;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->DeemphasisEntry.Levels.DeemphasisValues & ~(VALID_DEEMPHASIS_LEVELS)) == 0) &&
          ((Entry->DeemphasisEntry.HtPhyEntry.TypeFeats.HtPhyLinkValue & ~(HTPHY_LINKTYPE_ALL)) == 0) &&
          (Entry->DeemphasisEntry.HtPhyEntry.Address < HTPHY_REGISTER_MAX));

  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  Link = 0;
  while (FamilySpecificServices->NextLinkHasHtPhyFeats (
           FamilySpecificServices,
           &CapabilitySet,
           &Link,
           &Entry->DeemphasisEntry.HtPhyEntry.TypeFeats,
           &MatchedSublink1,
           &Freq0,
           &Freq1,
           StdHeader)) {
    if (DoesEntryTypeSpecificInfoMatch (
          GetLinkDeemphasis (
            MySocket,
            (MatchedSublink1 ? (Link + 4) : Link),
            (MatchedSublink1 ? Freq1 : Freq0),
            PlatformConfig,
            FamilySpecificServices,
            StdHeader),
          Entry->DeemphasisEntry.Levels.DeemphasisValues)) {
      FamilySpecificServices->SetHtPhyRegister (
        FamilySpecificServices,
        &Entry->DeemphasisEntry.HtPhyEntry,
        CapabilitySet,
        Link,
        StdHeader
        );
      IDS_HDT_CONSOLE (HT_TRACE, "Socket %d Module %d Sub-link %1d :\n  ----> running on HT3, %s Level is %s\n",
        MySocket, MyModule,
        ((Entry->DeemphasisEntry.HtPhyEntry.TypeFeats.HtPhyLinkValue & HTPHY_LINKTYPE_SL0_ALL) != 0) ? Link : (Link + 4),
        ((Entry->DeemphasisEntry.Levels.DeemphasisValues & DCV_LEVELS_ALL) != 0) ? "DCV" : "Deemphasis",
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL_NONE) ? " 0 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL__3) ? " - 3 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL__6) ? " - 6 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL__6) ? " - 6 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL__8) ? " - 8 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL__11) ? " - 11 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DEEMPHASIS_LEVEL__11_8) ? " - 11 dB postcursor with - 8 dB precursor" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL_NONE) ? " 0 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__2) ? " - 2 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__3) ? " - 3 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__5) ? " - 5 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__6) ? " - 6 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__7) ? " - 7 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__8) ? " - 8 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__9) ? " - 9 dB" :
        (Entry->DeemphasisEntry.Levels.DeemphasisValues == DCV_LEVEL__11) ? " - 11 dB" : "Undefined");
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Program HT Phy PCI registers which have complex frequency dependencies.
 *
 * @TableEntryTypeMethod{::HtPhyFreqRegister}.
 *
 * After matching a link for HT Features, check if the HT frequency matches the given range.
 * If it does, get the northbridge frequency limits for implemented NB P-states and check if
 * each matches the given range - range 0 and range 1 for each NB frequency, respectively.
 * If all matches, apply the entry.
 *
 * @param[in]       Entry               The type specific entry data to be implemented (that is written).
 * @param[in]       PlatformConfig      Config handle for platform specific information
 * @param[in]       StdHeader           Config params for library, services.
 *
 */
VOID
SetRegisterForHtPhyFreqEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32                Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  PCI_ADDR              CapabilitySet;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  BOOLEAN               MatchedSublink1;
  HT_FREQUENCIES        Freq0;
  HT_FREQUENCIES        Freq1;
  BOOLEAN               Temp1;
  BOOLEAN               Temp2;
  UINT32                NbFreq0;
  UINT32                NbFreq1;
  UINT32                NbDivisor0;
  UINT32                NbDivisor1;

  // Errors:  extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->HtPhyFreqEntry.HtPhyEntry.TypeFeats.HtPhyLinkValue & ~(HTPHY_LINKTYPE_ALL)) == 0) &&
          (Entry->HtPhyFreqEntry.HtPhyEntry.Address < HTPHY_REGISTER_MAX));

  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  Link = 0;
  while (FamilySpecificServices->NextLinkHasHtPhyFeats (
           FamilySpecificServices,
           &CapabilitySet,
           &Link,
           &Entry->HtPhyFreqEntry.HtPhyEntry.TypeFeats,
           &MatchedSublink1,
           &Freq0,
           &Freq1,
           StdHeader)) {
    // Check the HT Frequency for match to the range.
    if (IsEitherCountInRange (
          (MatchedSublink1 ? Freq1 : Freq0),
          (MatchedSublink1 ? Freq1 : Freq0),
          Entry->HtPhyFreqEntry.HtFreqCounts.HtFreqCountRanges)) {
      // Get the NB Frequency, convert to 100's of MHz, then convert to equivalent HT encoding.  This supports
      // NB frequencies from 800 MHz to 2600 MHz, which is currently greater than any processor supports.
      OptionMultiSocketConfiguration.GetSystemNbPstateSettings (
        (UINT32) 0,
        PlatformConfig,
        &NbFreq0,
        &NbDivisor0,
        &Temp1,
        &Temp2,
        StdHeader);

      if (OptionMultiSocketConfiguration.GetSystemNbPstateSettings (
            (UINT32) 1,
            PlatformConfig,
            &NbFreq1,
            &NbDivisor1,
            &Temp1,
            &Temp2,
            StdHeader)) {
        ASSERT (NbDivisor1 != 0);
        NbFreq1 = (NbFreq1 / NbDivisor1);
        NbFreq1 = (NbFreq1 / 100);
        NbFreq1 = (NbFreq1 / 2) + 1;
      } else {
        NbFreq1 = 0;
      }

      ASSERT (NbDivisor0 != 0);
      NbFreq0 = (NbFreq0 / NbDivisor0);
      NbFreq0 = (NbFreq0 / 100);
      NbFreq0 = (NbFreq0 / 2) + 1;
      if (IsEitherCountInRange (NbFreq0, NbFreq1, Entry->HtPhyFreqEntry.NbFreqCounts.HtFreqCountRanges)) {
        FamilySpecificServices->SetHtPhyRegister (
          FamilySpecificServices,
          &Entry->HtPhyFreqEntry.HtPhyEntry,
          CapabilitySet,
          Link,
          StdHeader);
      }
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Performance Profile PCI Register Entry.
 *
 * @TableEntryTypeMethod{::ProfileFixup}.
 *
 * Check the entry's performance profile features to the platform's and do the
 * PCI register entry if they match.
 *
 * @param[in]     Entry             The Performance Profile register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForPerformanceProfileEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  TABLE_ENTRY_DATA          PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->TokenPciEntry.TypeFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0) &&
          (Entry->InitialValues[4] == 0));

  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue,
                                      Entry->FixupEntry.TypeFeats.PerformanceProfileValue)) {
    LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
    PciEntry.PciEntry = Entry->FixupEntry.PciEntry;
    SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the HT Phy Performance Profile Register Entry.
 *
 * @TableEntryTypeMethod{::HtPhyProfileRegister}.
 *
 * @param[in]     Entry             The HT Phy register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForHtPhyProfileEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  TABLE_ENTRY_DATA          HtPhyEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->HtPhyProfileEntry.TypeFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0) &&
          (Entry->InitialValues[5] == 0));

  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (
        PlatformProfile.PerformanceProfileValue,
        Entry->HtPhyProfileEntry.TypeFeats.PerformanceProfileValue)) {
    LibAmdMemFill (&HtPhyEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
    HtPhyEntry.HtPhyEntry = Entry->HtPhyProfileEntry.HtPhyEntry;
    SetRegisterForHtPhyEntry (&HtPhyEntry, PlatformConfig, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the HT Host PCI Register Entry.
 *
 * @TableEntryTypeMethod{::HtHostPciRegister}.
 *
 * Make the current core's PCI address with the function and register for the entry.
 * For all HT links, check the link's feature set for a match to the entry.
 * Read - Modify - Write the PCI register, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForHtHostEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN                 Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  PCI_ADDR              CapabilitySet;
  PCI_ADDR              PciAddress;
  HT_HOST_FEATS         HtHostFeats;
  UINT32                RegisterData;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT ((Entry->InitialValues[4] == 0) &&
          ((Entry->HtHostEntry.TypeFeats.HtHostValue & ~((HT_HOST_FEATURES_ALL) | (HT_HOST_AND))) == 0) &&
          (Entry->HtHostEntry.Address.Address.Register < HT_LINK_HOST_CAP_MAX));

  HtHostFeats.HtHostValue = 0;
  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  Link = 0;
  while (FamilySpecificServices->GetNextHtLinkFeatures (FamilySpecificServices, &Link, &CapabilitySet, &HtHostFeats, StdHeader)) {
    if (DoesEntryTypeSpecificInfoMatch (HtHostFeats.HtHostValue, Entry->HtHostEntry.TypeFeats.HtHostValue)) {
      // Do the HT Host PCI register update.
      PciAddress = CapabilitySet;
      PciAddress.Address.Register += Entry->HtHostEntry.Address.Address.Register;
      LibAmdPciRead (AccessWidth32, PciAddress, &RegisterData, StdHeader);
      RegisterData = RegisterData & (~(Entry->HtHostEntry.Mask));
      RegisterData = RegisterData | Entry->HtHostEntry.Data;
      LibAmdPciWrite (AccessWidth32, PciAddress, &RegisterData, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the HT Host Performance PCI Register Entry.
 *
 * @TableEntryTypeMethod{::HtHostPerfPciRegister}.
 *
 * Make the current core's PCI address with the function and register for the entry.
 * For all HT links, check the link's feature set for a match to the entry.
 * Read - Modify - Write the PCI register, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForHtHostPerfEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  TABLE_ENTRY_DATA HtHostPciTypeEntryData;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT ((Entry->InitialValues[5] == 0) &&
          ((Entry->HtHostEntry.TypeFeats.HtHostValue & ~((HT_HOST_FEATURES_ALL) | (HT_HOST_AND))) == 0) &&
          (Entry->HtHostEntry.Address.Address.Register < HT_LINK_HOST_CAP_MAX));

  // Check for any performance profile features.
  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue,
                                      Entry->HtHostPerfEntry.PerformanceFeats.PerformanceProfileValue)) {
    // Perform HT Host entry process.
    LibAmdMemFill (&HtHostPciTypeEntryData, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
    HtHostPciTypeEntryData.HtHostEntry = Entry->HtHostPerfEntry.HtHostEntry;
    SetRegisterForHtHostEntry (&HtHostPciTypeEntryData, PlatformConfig, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Set the HT Link Token Count registers.
 *
 * @TableEntryTypeMethod{::HtTokenPciRegister}.
 *
 * Make the current core's PCI address with the function and register for the entry.
 * Check the performance profile features.
 * For all HT links, check the link's feature set for a match to the entry.
 * Read - Modify - Write the PCI register, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The Link Token register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForHtLinkTokenEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN                 Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  PCI_ADDR              CapabilitySet;
  HT_HOST_FEATS         HtHostFeats;
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  UINTN                 ProcessorCount;
  UINTN                     SystemDegree;
  UINT32                RegisterData;
  PCI_ADDR              PciAddress;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->HtTokenEntry.LinkFeats.HtHostValue & ~((HT_HOST_FEATURES_ALL) | (HT_HOST_AND))) == 0) &&
          ((Entry->HtTokenEntry.PerformanceFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0) &&
          (Entry->HtTokenEntry.Mask != 0));

  HtHostFeats.HtHostValue = 0;
  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  // Check if the actual processor count and SystemDegree are in either range.
  ProcessorCount = GetNumberOfProcessors (StdHeader);
  SystemDegree = GetSystemDegree (StdHeader);
  if (IsEitherCountInRange (ProcessorCount, SystemDegree, Entry->HtTokenEntry.ConnectivityCount.ConnectivityCountRanges)) {
    // Check for any performance profile features.
    GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
    if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue,
                                        Entry->HtTokenEntry.PerformanceFeats.PerformanceProfileValue)) {
      // Check the link features.
      Link = 0;
      while (FamilySpecificServices->GetNextHtLinkFeatures (FamilySpecificServices, &Link, &CapabilitySet, &HtHostFeats, StdHeader)) {
        if (DoesEntryTypeSpecificInfoMatch (HtHostFeats.HtHostValue, Entry->HtTokenEntry.LinkFeats.HtHostValue)) {
          // Do the HT Host PCI register update. Token register are four registers, sublink 0 and 1 share fields.
          // If sublink 0 is unconnected, we should let sublink 1 match.  If the links are ganged, of course only sublink 0 matches.
          // If the links are unganged and both connected, the BKDG settings are for both coherent.
          PciAddress = CapabilitySet;
          PciAddress.Address.Register = Entry->HtTokenEntry.Address.Address.Register +
            ((Link > 3) ? (((UINT32)Link - 4) * 4) : ((UINT32)Link * 4));
          PciAddress.Address.Function = Entry->HtTokenEntry.Address.Address.Function;
          LibAmdPciRead (AccessWidth32, PciAddress, &RegisterData, StdHeader);
          RegisterData = RegisterData & (~(Entry->HtTokenEntry.Mask));
          RegisterData = RegisterData | Entry->HtTokenEntry.Data;
          LibAmdPciWrite (AccessWidth32, PciAddress, &RegisterData, StdHeader);
        }
      }
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Core Counts Performance PCI Register Entry.
 *
 * @TableEntryTypeMethod{::CoreCountsPciRegister}.
 *
 * Check the performance profile.
 * Check the actual core count to the range pair given, and apply if matched.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForCoreCountsPerformanceEntry (
  IN       TABLE_ENTRY_DATA      *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  UINTN ActualCoreCount;
  TABLE_ENTRY_DATA          PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->CoreCountEntry.TypeFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0));

  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue, Entry->CoreCountEntry.TypeFeats.PerformanceProfileValue)) {
    ActualCoreCount = GetActiveCoresInCurrentModule (StdHeader);
    // Check if the actual core count is in either range.
    if (IsEitherCountInRange (ActualCoreCount, ActualCoreCount, Entry->CoreCountEntry.CoreCounts.CoreRanges)) {
      LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
      PciEntry.PciEntry = Entry->CoreCountEntry.PciEntry;
      SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Processor Counts PCI Register Entry.
 *
 * @TableEntryTypeMethod{::ProcCountsPciRegister}.
 *
 * Check the performance profile.
 * Check the actual processor count (not node count!) to the range pair given, and apply if matched.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForProcessorCountsEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  UINTN ProcessorCount;
  TABLE_ENTRY_DATA          PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->ProcCountEntry.TypeFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0));

  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue, Entry->ProcCountEntry.TypeFeats.PerformanceProfileValue)) {
    ProcessorCount = GetNumberOfProcessors (StdHeader);
    // Check if the actual processor count is in either range.
    if (IsEitherCountInRange (ProcessorCount, ProcessorCount, Entry->ProcCountEntry.ProcessorCounts.ProcessorCountRanges)) {
      LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
      PciEntry.PciEntry = Entry->ProcCountEntry.PciEntry;
      SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Compute Unit Counts PCI Register Entry.
 *
 * @TableEntryTypeMethod{::CompUnitCountsPciRegister}.
 *
 * Check the entry's performance profile features and the compute unit count
 * to the platform's and do the PCI register entry if they match.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForComputeUnitCountsEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  UINTN ComputeUnitCount;
  TABLE_ENTRY_DATA          PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->CompUnitCountEntry.TypeFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0));

  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue, Entry->CompUnitCountEntry.TypeFeats.PerformanceProfileValue)) {
    ComputeUnitCount = GetNumberOfCompUnitsInCurrentModule (StdHeader);
    // Check if the actual compute unit count is in either range.
    if (IsEitherCountInRange (ComputeUnitCount, ComputeUnitCount, Entry->CompUnitCountEntry.ComputeUnitCounts.ComputeUnitRanges)) {
      LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
      PciEntry.PciEntry = Entry->CompUnitCountEntry.PciEntry;
      SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Compute Unit Counts MSR Register Entry.
 *
 * @TableEntryTypeMethod{::CompUnitCountsMsr}.
 *
 * Check the entry's compute unit count to the platform's and do the
 * MSR entry if they match.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetMsrForComputeUnitCountsEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN ComputeUnitCount;
  TABLE_ENTRY_DATA          MsrEntry;

  ComputeUnitCount = GetNumberOfCompUnitsInCurrentModule (StdHeader);
  // Check if the actual compute unit count is in either range.
  if (IsEitherCountInRange (ComputeUnitCount, ComputeUnitCount, Entry->CompUnitCountMsrEntry.ComputeUnitCounts.ComputeUnitRanges)) {
    LibAmdMemFill (&MsrEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
    MsrEntry.MsrEntry = Entry->CompUnitCountMsrEntry.MsrEntry;
    SetRegisterForMsrEntry (&MsrEntry, PlatformConfig, StdHeader);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the Processor Token Counts PCI Register Entry.
 *
 * @TableEntryTypeMethod{::TokenPciRegister}.
 *
 * The table criteria then translate as:
 * - 2 Socket, half populated  ==   Degree 1
 * - 4 Socket, half populated   ==  Degree 2
 * - 2 Socket, fully populated  ==  Degree 3
 * - 4 Socket, fully populated  ==  Degree > 3.  (4 or 5 if 3P, 6 if 4P)
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForTokenPciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PERFORMANCE_PROFILE_FEATS PlatformProfile;
  UINTN                     SystemDegree;
  TABLE_ENTRY_DATA          PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT (((Entry->TokenPciEntry.TypeFeats.PerformanceProfileValue & ~((PERFORMANCE_PROFILE_ALL) | (PERFORMANCE_AND))) == 0));

  GetPerformanceFeatures (&PlatformProfile, PlatformConfig, StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (PlatformProfile.PerformanceProfileValue, Entry->TokenPciEntry.TypeFeats.PerformanceProfileValue)) {
    SystemDegree = GetSystemDegree (StdHeader);
    // Check if the system degree is in the range.
    if (IsEitherCountInRange (SystemDegree, SystemDegree, Entry->TokenPciEntry.ConnectivityCount.ConnectivityCountRanges)) {
      LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
      PciEntry.PciEntry = Entry->TokenPciEntry.PciEntry;
      SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the HT Link Feature PCI Register Entry.
 *
 * @TableEntryTypeMethod{::HtFeatPciRegister}.
 *
 * Set a single field (that is, the register field is not in HT Host capability or a
 * set of per link registers) in PCI config, based on HT link features and package type.
 * This code is used for two cases: single link processors and multilink processors.
 * For single link cases, the link will be tested for a match to the HT Features for the link.
 * For multilink processors, the entry will match if @b any link is found which matches.
 * For example, a setting can be applied based on coherent HT3 by matching coherent AND HT3.
 *
 * Make the core's PCI address.  Check the package type (currently more important to the single link case),
 * and if matching, iterate through all links checking for an HT feature match until found or exhausted.
 * If a match was found, pass the PCI entry data to the implementer for writing for the current core.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForHtFeaturePciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN                 Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  PCI_ADDR              CapabilitySet;
  HT_HOST_FEATS         HtHostFeats;
  UINT32                ProcessorPackageType;
  BOOLEAN               IsMatch;
  TABLE_ENTRY_DATA      PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT ((Entry->HtFeatPciEntry.PciEntry.Mask != 0) &&
          ((Entry->HtFeatPciEntry.LinkFeats.HtHostValue & ~((HT_HOST_FEATURES_ALL) | (HT_HOST_AND))) == 0));

  HtHostFeats.HtHostValue = 0;
  LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
  PciEntry.PciEntry = Entry->HtFeatPciEntry.PciEntry;
  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  ASSERT ((Entry->HtFeatPciEntry.PackageType.PackageTypeValue & ~(PACKAGE_TYPE_ALL)) == 0);

  ProcessorPackageType = LibAmdGetPackageType (StdHeader);
  if (DoesEntryTypeSpecificInfoMatch (ProcessorPackageType, Entry->HtFeatPciEntry.PackageType.PackageTypeValue)) {
    IsMatch = FALSE;
    while (FamilySpecificServices->GetNextHtLinkFeatures (FamilySpecificServices, &Link, &CapabilitySet, &HtHostFeats, StdHeader)) {
      if (DoesEntryTypeSpecificInfoMatch (HtHostFeats.HtHostValue, Entry->HtFeatPciEntry.LinkFeats.HtHostValue)) {
        IsMatch = TRUE;
        break;
      }
    }
    if (IsMatch) {
      // Do the PCI register update.
      SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Perform the HT Link PCI Register Entry.
 *
 * @TableEntryTypeMethod{::HtLinkPciRegister}.
 *
 * Make the current core's PCI address with the function and register for the entry.
 * Registers are processed for match per link, assuming sequential PCI address per link.
 * Read - Modify - Write each matching link's PCI register, clearing masked bits, and setting the data bits.
 *
 * @param[in]     Entry             The PCI register entry to perform
 * @param[in]     PlatformConfig    Config handle for platform specific information
 * @param[in]     StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegisterForHtLinkPciEntry (
  IN       TABLE_ENTRY_DATA       *Entry,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINTN                 Link;
  UINT32                MySocket;
  UINT32                MyModule;
  AGESA_STATUS          IgnoredStatus;
  UINT32                Ignored;
  CPU_LOGICAL_ID        CpuFamilyRevision;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  PCI_ADDR              CapabilitySet;
  HT_HOST_FEATS         HtHostFeats;
  TABLE_ENTRY_DATA      PciEntry;

  // Errors:  Possible values in unused entry space, extra type features, value range checks.
  // Check that the entry type is correct and the actual supplied entry data is appropriate for that entry.
  ASSERT ((Entry->HtLinkPciEntry.PciEntry.Mask != 0) &&
          ((Entry->HtLinkPciEntry.LinkFeats.HtHostValue & ~((HT_HOST_FEATURES_ALL) | (HT_HOST_AND))) == 0));

  HtHostFeats.HtHostValue = 0;
  LibAmdMemFill (&PciEntry, 0, sizeof (TABLE_ENTRY_DATA), StdHeader);
  PciEntry.PciEntry = Entry->HtLinkPciEntry.PciEntry;
  IdentifyCore (StdHeader, &MySocket, &MyModule, &Ignored, &IgnoredStatus);
  GetPciAddress (StdHeader, MySocket, MyModule, &CapabilitySet, &IgnoredStatus);
  GetLogicalIdOfCurrentCore (&CpuFamilyRevision, StdHeader);
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  Link = 0;
  while (FamilySpecificServices->GetNextHtLinkFeatures (FamilySpecificServices, &Link, &CapabilitySet, &HtHostFeats, StdHeader)) {
    if (DoesEntryTypeSpecificInfoMatch (HtHostFeats.HtHostValue, Entry->HtLinkPciEntry.LinkFeats.HtHostValue)) {
      // Do the update to the link's non-Host PCI register, based on the entry address.
      PciEntry.PciEntry.Address = Entry->HtLinkPciEntry.PciEntry.Address;
      PciEntry.PciEntry.Address.Address.Register = PciEntry.PciEntry.Address.Address.Register + ((UINT32)Link * 4);
      SetRegisterForPciEntry (&PciEntry, PlatformConfig, StdHeader);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 * Returns the platform features list of the currently running processor core.
 *
 * @param[out]      Features          The Features supported by this platform
 * @param[in]       PlatformConfig    Config handle for platform specific information
 * @param[in]       StdHeader         Header for library and services
 *
 */
VOID
GetPlatformFeatures (
     OUT   PLATFORM_FEATS         *Features,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR  PciAddress;
  UINT32    CapabilityReg;
  UINT32     Link;
  CPU_SPECIFIC_SERVICES  *FamilySpecificServices;
  UINT32     CoreCount;

  // Start with none.
  Features->PlatformValue = 0;

  switch (PlatformConfig->PlatformProfile.PlatformControlFlowMode) {
  case Nfcm:
    Features->PlatformFeatures.PlatformNfcm = 1;
    break;
  case UmaDr:
    Features->PlatformFeatures.PlatformUma = 1;
    break;
  case UmaIfcm:
    Features->PlatformFeatures.PlatformUmaIfcm = 1;
    break;
  case Ifcm:
    Features->PlatformFeatures.PlatformIfcm = 1;
    break;
  case Iommu:
    Features->PlatformFeatures.PlatformIommu = 1;
    break;
  default:
    ASSERT (FALSE);
  }
  // Check - Single Link?
  // This is based on the implemented links on the package regardless of their
  // connection status.  All processors must match the BSP, so we only check it and
  // not the current node.  We don't care exactly how many links there are, as soon
  // as we find more than one we are done.
  Link = 0;
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, PCI_DEV_BASE, FUNC_0, 0);
  // Until either all capabilities are done or until the desired link is found,
  // keep looking for HT Host Capabilities.
  while (Link < 2) {
    LibAmdPciFindNextCap (&PciAddress, StdHeader);
    if (PciAddress.AddressValue != ILLEGAL_SBDFO) {
      LibAmdPciRead (AccessWidth32, PciAddress, &CapabilityReg, StdHeader);
      if ((CapabilityReg & 0xE00000FF) == 0x20000008) {
        Link++;
      }
      // A capability other than an HT capability, keep looking.
    } else {
      // end of capabilities
      break;
    }
  }
  if (Link < 2) {
    Features->PlatformFeatures.PlatformSingleLink = 1;
  } else {
    Features->PlatformFeatures.PlatformMultiLink = 1;
  }

  // Set the legacy core count bits.
  GetActiveCoresInCurrentSocket (&CoreCount, StdHeader);
  switch (CoreCount) {
  case 1:
    Features->PlatformFeatures.PlatformSingleCore = 1;
    break;
  case 2:
    Features->PlatformFeatures.PlatformDualCore = 1;
    break;
  default:
    Features->PlatformFeatures.PlatformMultiCore = 1;
  }

  //
  // Get some specific platform type info, VC...etc.
  //
  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  ASSERT (FamilySpecificServices != NULL);
  FamilySpecificServices->GetPlatformTypeSpecificInfo (FamilySpecificServices, Features, StdHeader);

}

/*---------------------------------------------------------------------------------------*/
/**
 * Checks if a register table entry applies to the executing core.
 *
 * This function uses a combination of logical ID and platform features to
 * determine whether or not a register table entry applies to the executing core.
 *
 * @param[in]     CoreCpuRevision         The current core's logical ID
 * @param[in]     EntryCpuRevision        The entry's desired logical IDs
 * @param[in]     PlatformFeatures        The platform features
 * @param[in]     EntryFeatures           The entry's desired platform features
 *
 * @retval        TRUE           This entry should be applied
 * @retval        FALSE          This entry does not apply
 *
 */
BOOLEAN
STATIC
DoesEntryMatchPlatform (
  IN       CPU_LOGICAL_ID   CoreCpuRevision,
  IN       CPU_LOGICAL_ID   EntryCpuRevision,
  IN       PLATFORM_FEATS   PlatformFeatures,
  IN       PLATFORM_FEATS   EntryFeatures
  )
{
  BOOLEAN Result;

  Result = FALSE;

  if (((CoreCpuRevision.Family & EntryCpuRevision.Family) != 0) &&
      ((CoreCpuRevision.Revision & EntryCpuRevision.Revision) != 0)) {
    if (EntryFeatures.PlatformFeatures.AndPlatformFeats == 0) {
      // Match if ANY entry feats match a platform feat (an OR test)
      if ((EntryFeatures.PlatformValue & PlatformFeatures.PlatformValue) != 0) {
        Result = TRUE;
      }
    } else {
      // Match if ALL entry feats match a platform feat (an AND test)
      if ((EntryFeatures.PlatformValue & ~(AMD_PF_AND)) ==
          (EntryFeatures.PlatformValue & PlatformFeatures.PlatformValue)) {
        Result = TRUE;
      }
    }
  }

  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Checks register table entry type specific criteria to the platform.
 *
 * Entry Data Type implementer methods can use this generically to check their own
 * specific criteria.  The method collects the actual platform characteristics and
 * provides them along with the table entry's criteria to this service.
 *
 * There are a couple considerations for any implementer method using this service.
 * The criteria value has to be representable as a UINT32.  The MSB, Bit 31, has to
 * be used as a AND test request if set in the entry.  (The platform value should never
 * have that bit set.)
 *
 * @param[in]     PlatformTypeSpecificFeatures        The platform features
 * @param[in]     EntryTypeFeatures                   The entry's desired platform features
 *
 * @retval        TRUE                                This entry should be applied
 * @retval        FALSE                               This entry does not apply
 *
 */
BOOLEAN
DoesEntryTypeSpecificInfoMatch (
  IN       UINT32   PlatformTypeSpecificFeatures,
  IN       UINT32   EntryTypeFeatures
  )
{
  BOOLEAN Result;

  Result = FALSE;

  if ((EntryTypeFeatures & BIT31) == 0) {
    // Match if ANY entry feats match a platform feat (an OR test)
    if ((EntryTypeFeatures & PlatformTypeSpecificFeatures) != 0) {
      Result = TRUE;
    }
  } else {
    // Match if ALL entry feats match a platform feat (an AND test)
    if ((EntryTypeFeatures & ~(BIT31)) == (EntryTypeFeatures & PlatformTypeSpecificFeatures)) {
      Result = TRUE;
    }
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Determine this core's Selector matches.
 *
 * @param[in]  Selector    Is the current core this selector type?
 * @param[in]  StdHeader   Config handle for library and services.
 *
 * @retval  TRUE           Yes, it is.
 * @retval  FALSE          No, it is not.
 */
BOOLEAN
STATIC
IsCoreSelector (
  IN       TABLE_CORE_SELECTOR       Selector,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  BOOLEAN Result;
  AGESA_STATUS  CalledStatus;

  Result = TRUE;
  ASSERT (Selector < TableCoreSelectorMax);

  if ((Selector == PrimaryCores) && !IsCurrentCorePrimary (StdHeader)) {
    Result = FALSE;
  }
  if ((Selector == CorePairPrimary) && !IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    Result = FALSE;
  }
  if ((Selector == BscCore) && (!IsBsp (StdHeader, &CalledStatus))) {
    Result = FALSE;
  }
  return Result;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Set the registers for this core based on entries in a list of Register Tables.
 *
 * Determine the platform features and this core's logical id.  Get the specific table
 * entry type implementations for the logical model, which may be either generic (the ones
 * in this file) or specific.
 *
 * Scan the tables starting the with ones for all cores and progressively narrowing the selection
 * based on this core's role (ex. primary core).  For a selected table, check for each entry
 * matching the current core and platform, and call the implementer method to perform the
 * register set operation if it matches.
 *
 * @param[in]  PlatformConfig    Config handle for platform specific information
 * @param[in]  StdHeader         Config handle for library and services.
 *
 */
VOID
SetRegistersFromTables (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CPU_LOGICAL_ID         CpuLogicalId;
  PLATFORM_FEATS         PlatformFeatures;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  TABLE_ENTRY_FIELDS    *Entries;
  TABLE_CORE_SELECTOR    Selector;
  TABLE_ENTRY_TYPE       EntryType;
  REGISTER_TABLE       **TableHandle;
  UINTN                  NumberOfEntries;
  UINTN                  CurrentEntryCount;
  TABLE_ENTRY_TYPE_DESCRIPTOR *TypeImplementer;
  PF_DO_TABLE_ENTRY       DoTableEntry[TableEntryTypeMax];

  // Did you really mean to increase the size of ALL table entries??!!
  // While it is not necessarily a bug to increase the size of table entries:
  //   - Is this warning a surprise?  Please fix it.
  //   - If expected, is this really a feature which is worth the increase?  Then let other entries also use the space.
  ASSERT (sizeof (TABLE_ENTRY_DATA) == (MAX_ENTRY_TYPE_ITEMS32 * sizeof (UINT32)));

  PlatformFeatures.PlatformValue = 0;
  GetLogicalIdOfCurrentCore (&CpuLogicalId, StdHeader);
  GetPlatformFeatures (&PlatformFeatures, PlatformConfig, StdHeader);
  GetCpuServicesFromLogicalId (&CpuLogicalId, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  // Build a non-sparse table of implementer methods, so we don't have to keep searching.
  // It is a bug to not include a descriptor for a type that is in the table (but the
  // descriptor can point to a non-assert stub).
  // Also, it is not a bug to have no register table implementations, but it is a bug to have none and call this routine.
  for (EntryType = MsrRegister; EntryType < TableEntryTypeMax; EntryType++) {
    DoTableEntry[EntryType] = (PF_DO_TABLE_ENTRY)CommonAssert;
  }
  TypeImplementer = FamilySpecificServices->TableEntryTypeDescriptors;
  ASSERT (TypeImplementer != NULL);
  while (TypeImplementer->EntryType < TableEntryTypeMax) {
    DoTableEntry[TypeImplementer->EntryType] = TypeImplementer->DoTableEntry;
    TypeImplementer++;
  }

  for (Selector = AllCores; Selector < TableCoreSelectorMax; Selector++) {
    if (IsCoreSelector (Selector, StdHeader)) {
      // If the current core is the selected type of core, work the table list for tables for that type of core.
      TableHandle = NULL;
      Entries = GetNextRegisterTable (FamilySpecificServices, Selector, &TableHandle, &NumberOfEntries, StdHeader);
      while (Entries != NULL) {
        for (CurrentEntryCount = 0; CurrentEntryCount < NumberOfEntries; CurrentEntryCount++, Entries++) {
          if (DoesEntryMatchPlatform (CpuLogicalId, Entries->CpuRevision, PlatformFeatures, Entries->Features)) {
            // The entry matches this config, Do It!
            // Find the implementer for this entry type and pass the entry data to it.
            ASSERT (Entries->EntryType < TableEntryTypeMax);
            DoTableEntry[Entries->EntryType] (&Entries->Entry, PlatformConfig, StdHeader);
          }
        }
        Entries = GetNextRegisterTable (FamilySpecificServices, Selector, &TableHandle, &NumberOfEntries, StdHeader);
      }
    } else {
      // Once a selector does not match the current core, quit looking.
      break;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Set the registers for this core based on entries in a list of Register Tables.
 *
 * This function acts as a wrapper for calling the SetRegistersFromTables
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
SetRegistersFromTablesAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_TESTPOINT (TpProcCpuProcessRegisterTables, StdHeader);
  SetRegistersFromTables (&EarlyParams->PlatformConfig, StdHeader);
}
