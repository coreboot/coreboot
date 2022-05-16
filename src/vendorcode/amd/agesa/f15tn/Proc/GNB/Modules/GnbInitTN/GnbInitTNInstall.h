/* $NoKeywords:$ */
/**
 * @file
 *
 * Tn service installation file
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

#ifndef _GNB_INIT_TN_INSTALL_H_
#define _GNB_INIT_TN_INSTALL_H_

//-----------------------------------------------------------------------
// Specify definition used by module services
//-----------------------------------------------------------------------

#include  <Proc/GNB/Common/GnbPcie.h>
#include  <Proc/GNB/Common/GnbPcieFamServices.h>
#include  <Proc/GNB/Common/GnbFamServices.h>

//-----------------------------------------------------------------------
// Export services
//-----------------------------------------------------------------------

#if  (AGESA_ENTRY_INIT_EARLY == TRUE)
  extern F_PCIEFMGETCOMPLEXDATALENGTH             PcieGetComplexDataLengthTN;
  extern F_PCIEFMBUILDCOMPLEXCONFIGURATION        PcieBuildComplexConfigurationTN;
  extern F_PCIEFMCONFIGUREENGINESLANEALLOCATION   PcieConfigureEnginesLaneAllocationTN;
  extern F_PCIEFMCHECKPORTPCIDEVICEMAPPING        PcieCheckPortPciDeviceMappingTN;
  extern F_PCIEFMMAPPORTPCIADDRESS                PcieMapPortPciAddressTN;
  extern F_PCIEFMCHECKPORTPCIELANECANBEMUXED      PcieCheckPortPcieLaneCanBeMuxedTN;
  extern F_PCIEFMGETSBCONFIGINFO                  PcieGetSbConfigInfoTN;
CONST PCIe_FAM_CONFIG_SERVICES GnbPcieConfigProtocolTN = {
    PcieGetComplexDataLengthTN,
    PcieBuildComplexConfigurationTN,
    PcieConfigureEnginesLaneAllocationTN,
    PcieCheckPortPciDeviceMappingTN,
    PcieMapPortPciAddressTN,
    PcieCheckPortPcieLaneCanBeMuxedTN,
    PcieGetSbConfigInfoTN
  };

  CONST GNB_SERVICE GnbPcieCongigServicesTN = {
    GnbPcieFamConfigService,
    AMD_FAMILY_TN,
    &GnbPcieConfigProtocolTN,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbPcieCongigServicesTN
#endif

#if  (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE)
  extern F_PCIEFMGETCORECONFIGURATIONVALUE        PcieGetCoreConfigurationValueTN;
  extern F_PCIEFMGETLINKSPEEDCAP                  PcieGetLinkSpeedCapTN;
  extern F_PCIEFMGETNATIVEPHYLANEBITMAP           PcieGetNativePhyLaneBitmapTN;
  extern F_PCIEFMSETLINKSPEEDCAP                  PcieSetLinkSpeedCapV4;

  CONST PCIe_FAM_INIT_SERVICES GnbPcieInitProtocolTN = {
    PcieGetCoreConfigurationValueTN,
    PcieGetLinkSpeedCapTN,
    PcieGetNativePhyLaneBitmapTN,
    PcieSetLinkSpeedCapV4
  };

  CONST GNB_SERVICE GnbPcieInitServicesTN = {
    GnbPcieFamInitService,
    AMD_FAMILY_TN,
    &GnbPcieInitProtocolTN,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbPcieInitServicesTN
#endif

#if  (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE)
  #if IDSOPT_IDS_ENABLED == TRUE
    #if IDSOPT_TRACING_ENABLED == TRUE
      extern F_PCIEFMDEBUGGETHOSTREGADDRESSSPACESTRING  PcieDebugGetHostRegAddressSpaceStringTN;
      extern F_PCIEFMDEBUGGETWRAPPERNAMESTRING          PcieDebugGetWrapperNameStringTN;
      extern F_PCIEFMDEBUGGETCORECONFIGURATIONSTRING    PcieDebugGetCoreConfigurationStringTN;

      CONST PCIe_FAM_DEBUG_SERVICES GnbPcieDebugProtocolTN = {
        PcieDebugGetHostRegAddressSpaceStringTN,
        PcieDebugGetWrapperNameStringTN,
        PcieDebugGetCoreConfigurationStringTN
      };

      CONST GNB_SERVICE GnbPcieDebugServicesTN = {
        GnbPcieFamDebugService,
        AMD_FAMILY_TN,
        &GnbPcieDebugProtocolTN,
        SERVICES_POINTER
      };
      #undef  SERVICES_POINTER
      #define SERVICES_POINTER  &GnbPcieDebugServicesTN
    #endif
  #endif
#endif

#if  (AGESA_ENTRY_INIT_EARLY == TRUE) || (AGESA_ENTRY_INIT_POST == TRUE) || (AGESA_ENTRY_INIT_MID == TRUE) || (AGESA_ENTRY_INIT_LATE == TRUE)
  extern F_GNB_REGISTER_ACCESS  GnbRegisterReadServiceTN;
  extern F_GNB_REGISTER_ACCESS  GnbRegisterWriteServiceTN;

  CONST GNB_REGISTER_SERVICE GnbRegiterAccessProtocol = {
    GnbRegisterReadServiceTN,
    GnbRegisterWriteServiceTN
  };

  CONST GNB_SERVICE GnbRegisterAccessServicesTN = {
    GnbRegisterAccessService,
    AMD_FAMILY_TN,
    &GnbRegiterAccessProtocol,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbRegisterAccessServicesTN

  extern F_GNBFMCREATEIVRSENTRY    GnbCreateIvrsEntryTN;
  extern F_GNBFMCHECKIOMMUPRESENT  GnbCheckIommuPresentTN;

  CONST GNB_FAM_IOMMU_SERVICES GnbIommuConfigProtocolTN = {
    GnbCheckIommuPresentTN,
    GnbCreateIvrsEntryTN
  };

  CONST GNB_SERVICE GnbIommuConfigServicesTN = {
    GnbIommuService,
    AMD_FAMILY_TN,
    &GnbIommuConfigProtocolTN,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbIommuConfigServicesTN

#endif
#endif  // _GNB_INIT_TN_INSTALL_H_
