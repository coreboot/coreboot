/* $NoKeywords:$ */
/**
 * @file
 *
 * KB service installation file
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 86079 $   @e \$Date: 2013-01-16 00:59:04 -0600 (Wed, 16 Jan 2013) $
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

#ifndef _GNB_INIT_KB_INSTALL_H_
#define _GNB_INIT_KB_INSTALL_H_

//-----------------------------------------------------------------------
// Specify definition used by module services
//-----------------------------------------------------------------------

#include  "GnbPcie.h"
#include  "GnbUraServices.h"
#include  "GnbPcieFamServices.h"
#include  "GnbFamServices.h"
#include  "GnbGfxFamServices.h"

//-----------------------------------------------------------------------
// Export services
//-----------------------------------------------------------------------

#if  (AGESA_ENTRY_INIT_EARLY == TRUE)

  extern F_PCIEFMGETSBCONFIGINFO                  PcieGetSbConfigInfoKB;
  extern F_PCIEFMGETCOMPLEXDATALENGTH             PcieGetComplexDataLengthKB;
  extern F_PCIEFMBUILDCOMPLEXCONFIGURATION        PcieBuildComplexConfigurationKB;
  extern F_PCIEFMCONFIGUREENGINESLANEALLOCATION   PcieConfigureEnginesLaneAllocationKB;
  extern F_PCIEFMCHECKPORTPCIDEVICEMAPPING        PcieCheckPortPciDeviceMappingKB;
  extern F_PCIEFMMAPPORTPCIADDRESS                PcieMapPortPciAddressKB;
  extern F_PCIEFMCHECKPORTPCIELANECANBEMUXED      PcieCheckPortPcieLaneCanBeMuxedKB;


  CONST PCIe_FAM_CONFIG_SERVICES GnbPcieConfigProtocolKB = {
    PcieGetComplexDataLengthKB,
    PcieBuildComplexConfigurationKB,
    PcieConfigureEnginesLaneAllocationKB,
    PcieCheckPortPciDeviceMappingKB,
    PcieMapPortPciAddressKB,
    PcieCheckPortPcieLaneCanBeMuxedKB,
    PcieGetSbConfigInfoKB
  };

  CONST GNB_SERVICE GnbPcieCongigServicesKB = {
    GnbPcieFamConfigService,
    AMD_FAMILY_KB,
    &GnbPcieConfigProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbPcieCongigServicesKB
#endif

#if  (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE)
  extern F_PCIEFMGETCORECONFIGURATIONVALUE        PcieGetCoreConfigurationValueKB;
  extern F_PCIEFMGETLINKSPEEDCAP                  PcieGetLinkSpeedCapKB;
  extern F_PCIEFMGETNATIVEPHYLANEBITMAP           PcieGetNativePhyLaneBitmapKB;
  extern F_PCIEFMSETLINKSPEEDCAP                  PcieSetLinkSpeedCapV4;

  CONST PCIe_FAM_INIT_SERVICES GnbPcieInitProtocolKB = {
    PcieGetCoreConfigurationValueKB,
    PcieGetLinkSpeedCapKB,
    PcieGetNativePhyLaneBitmapKB,
    PcieSetLinkSpeedCapV4
  };

  CONST GNB_SERVICE GnbPcieInitServicesKB = {
    GnbPcieFamInitService,
    AMD_FAMILY_KB,
    &GnbPcieInitProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbPcieInitServicesKB
#endif

#if  (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE)
  #if IDSOPT_IDS_ENABLED == TRUE
    #if IDSOPT_TRACING_ENABLED == TRUE
      extern F_PCIEFMDEBUGGETHOSTREGADDRESSSPACESTRING  PcieDebugGetHostRegAddressSpaceStringKB;
      extern F_PCIEFMDEBUGGETWRAPPERNAMESTRING          PcieDebugGetWrapperNameStringKB;
      extern F_PCIEFMDEBUGGETCORECONFIGURATIONSTRING    PcieDebugGetCoreConfigurationStringKB;

      CONST PCIe_FAM_DEBUG_SERVICES GnbPcieDebugProtocolKB = {
        PcieDebugGetHostRegAddressSpaceStringKB,
        PcieDebugGetWrapperNameStringKB,
        PcieDebugGetCoreConfigurationStringKB
      };

      CONST GNB_SERVICE GnbPcieDebugServicesKB = {
        GnbPcieFamDebugService,
        AMD_FAMILY_KB,
        &GnbPcieDebugProtocolKB,
        SERVICES_POINTER
      };
      #undef  SERVICES_POINTER
      #define SERVICES_POINTER  &GnbPcieDebugServicesKB
    #endif
  #endif
#endif

#if  (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  extern F_GNB_REGISTER_ACCESS  GnbRegisterReadKB;
  extern F_GNB_REGISTER_ACCESS  GnbRegisterWriteKB;

  CONST GNB_REGISTER_SERVICE GnbRegisterAccessProtocolKB = {
    GnbRegisterReadKB,
    GnbRegisterWriteKB
  };

  CONST GNB_SERVICE GnbRegisterAccessServicesKB = {
    GnbRegisterAccessService,
    AMD_FAMILY_KB,
    &GnbRegisterAccessProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbRegisterAccessServicesKB

  extern F_GNBURASERVICELOCATEREGTBL GnbUraLocateRegTblKB;
  extern F_GNBURASERVICEGET GnbUraGetKB;
  extern F_GNBURASERVICESET GnbUraSetKB;
  extern F_GNBURASERVICESTREAMSET GnbUraStreamSetKB;

  CONST GNB_URA_SERVICE GnbUraProtocolKB = {
    GnbUraLocateRegTblKB,
    GnbUraGetKB,
    GnbUraSetKB,
    GnbUraStreamSetKB
  };

  CONST GNB_SERVICE GnbUraServicesKB = {
    GnbUraService,
    AMD_FAMILY_KB,
    &GnbUraProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbUraServicesKB

  extern F_GFXMAPENGINETODISPLAYPATH GfxMapEngineToDisplayPathKB;
  extern F_GFXDISABLECONTROLLER      GfxDisableControllerKB;
  extern F_GFXCALCULATECLOCK         GfxCalculateClockKB;
  extern F_GFXISVBIOSPOSTED          GfxIsVbiosPostedKB;

  CONST GFX_FAM_SERVICES GfxFamilyServiceProtocolKB = {
    GfxMapEngineToDisplayPathKB,
    GfxDisableControllerKB,
    GfxCalculateClockKB,
    GfxIsVbiosPostedKB
  };

  CONST GNB_SERVICE GfxFamilyServicesKB = {
    GfxFamService,
    AMD_FAMILY_KB,
    &GfxFamilyServiceProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GfxFamilyServicesKB

  extern F_GNBTIMESTAMP GnbTimeStampKB;

  CONST GNB_FAM_TS_SERVICES GnbFamTsProtocolKB = {
    GnbTimeStampKB,
  };

  CONST GNB_SERVICE GnbFamTsServicesKB = {
    GnbFamTsService,
    AMD_FAMILY_KB,
    &GnbFamTsProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbFamTsServicesKB


  extern F_PCIE_MAXPAYLOAD_SETTING  PcieMaxPayloadKB;

  CONST PCIE_MAXPAYLOAD_SERVICE PcieMaxPayloadProtocolKB = {
    PcieMaxPayloadKB
  };

  CONST GNB_SERVICE PcieMaxPayloadServicesKB = {
    GnbPcieMaxPayloadService,
    AMD_FAMILY_KB,
    &PcieMaxPayloadProtocolKB,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &PcieMaxPayloadServicesKB

#endif
#endif  // _GNB_INIT_KB_INSTALL_H_
