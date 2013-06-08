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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_TABLEHT_FILECODE

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
extern BUILD_OPT_CFG UserOptions;


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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);
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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);
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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);
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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);
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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);
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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);

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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);

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
  GetCpuServicesFromLogicalId (&CpuFamilyRevision, &FamilySpecificServices, StdHeader);

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

