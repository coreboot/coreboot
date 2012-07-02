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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
* ***************************************************************************
*
*/

#ifndef _GNB_INIT_TN_INSTALL_H_
#define _GNB_INIT_TN_INSTALL_H_

//-----------------------------------------------------------------------
// Specify definition used by module services
//-----------------------------------------------------------------------

#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbFamServices.h"

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
  PCIe_FAM_CONFIG_SERVICES GnbPcieConfigProtocolTN = {
    PcieGetComplexDataLengthTN,
    PcieBuildComplexConfigurationTN,
    PcieConfigureEnginesLaneAllocationTN,
    PcieCheckPortPciDeviceMappingTN,
    PcieMapPortPciAddressTN,
    PcieCheckPortPcieLaneCanBeMuxedTN,
    PcieGetSbConfigInfoTN
  };

  GNB_SERVICE GnbPcieCongigServicesTN = {
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

  PCIe_FAM_INIT_SERVICES GnbPcieInitProtocolTN = {
    PcieGetCoreConfigurationValueTN,
    PcieGetLinkSpeedCapTN,
    PcieGetNativePhyLaneBitmapTN,
    PcieSetLinkSpeedCapV4
  };

  GNB_SERVICE GnbPcieInitServicesTN = {
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

      PCIe_FAM_DEBUG_SERVICES GnbPcieDebugProtocolTN = {
        PcieDebugGetHostRegAddressSpaceStringTN,
        PcieDebugGetWrapperNameStringTN,
        PcieDebugGetCoreConfigurationStringTN
      };

      GNB_SERVICE GnbPcieDebugServicesTN = {
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

  GNB_REGISTER_SERVICE GnbRegiterAccessProtocol = {
    GnbRegisterReadServiceTN,
    GnbRegisterWriteServiceTN
  };

  GNB_SERVICE GnbRegisterAccessServicesTN = {
    GnbRegisterAccessService,
    AMD_FAMILY_TN,
    &GnbRegiterAccessProtocol,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbRegisterAccessServicesTN

  extern F_GNBFMCREATEIVRSENTRY    GnbCreateIvrsEntryTN;
  extern F_GNBFMCHECKIOMMUPRESENT  GnbCheckIommuPresentTN;

  GNB_FAM_IOMMU_SERVICES GnbIommuConfigProtocolTN = {
    GnbCheckIommuPresentTN,
    GnbCreateIvrsEntryTN
  };

  GNB_SERVICE GnbIommuConfigServicesTN = {
    GnbIommuService,
    AMD_FAMILY_TN,
    &GnbIommuConfigProtocolTN,
    SERVICES_POINTER
  };
  #undef  SERVICES_POINTER
  #define SERVICES_POINTER  &GnbIommuConfigServicesTN

#endif
#endif  // _GNB_INIT_TN_INSTALL_H_
