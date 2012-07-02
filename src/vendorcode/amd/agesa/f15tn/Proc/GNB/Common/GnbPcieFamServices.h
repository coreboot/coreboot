/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe family specific services.
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
#ifndef _GNBPCIEFAMSERVICES_H_
#define _GNBPCIEFAMSERVICES_H_

#include  "Gnb.h"
#include  "GnbPcie.h"

AGESA_STATUS
PcieFmGetComplexDataLength (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

typedef AGESA_STATUS F_PCIEFMGETCOMPLEXDATALENGTH (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

AGESA_STATUS
PcieFmBuildComplexConfiguration (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

typedef AGESA_STATUS F_PCIEFMBUILDCOMPLEXCONFIGURATION (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

AGESA_STATUS
PcieFmConfigureEnginesLaneAllocation (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIE_ENGINE_TYPE              EngineType,
  IN       UINT8                         ConfigurationId
  );

typedef AGESA_STATUS F_PCIEFMCONFIGUREENGINESLANEALLOCATION (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIE_ENGINE_TYPE              EngineType,
  IN       UINT8                         ConfigurationId
  );

AGESA_STATUS
PcieFmGetCoreConfigurationValue (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       UINT8                         CoreId,
  IN       UINT64                        ConfigurationSignature,
  IN       UINT8                         *ConfigurationValue
  );

typedef AGESA_STATUS F_PCIEFMGETCORECONFIGURATIONVALUE (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       UINT8                         CoreId,
  IN       UINT64                        ConfigurationSignature,
  IN       UINT8                         *ConfigurationValue
  );

BOOLEAN
PcieFmCheckPortPciDeviceMapping (
  IN      PCIe_PORT_DESCRIPTOR           *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

typedef BOOLEAN F_PCIEFMCHECKPORTPCIDEVICEMAPPING (
  IN      PCIe_PORT_DESCRIPTOR           *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

AGESA_STATUS
PcieFmMapPortPciAddress (
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

typedef AGESA_STATUS F_PCIEFMMAPPORTPCIADDRESS (
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

BOOLEAN
PcieFmCheckPortPcieLaneCanBeMuxed (
  IN      PCIe_PORT_DESCRIPTOR           *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

typedef BOOLEAN F_PCIEFMCHECKPORTPCIELANECANBEMUXED (
  IN      PCIe_PORT_DESCRIPTOR           *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

CONST CHAR8*
PcieFmDebugGetCoreConfigurationString (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       UINT8                         ConfigurationValue
 );

typedef CONST CHAR8* F_PCIEFMDEBUGGETCORECONFIGURATIONSTRING (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       UINT8                         ConfigurationValue
 );

CONST CHAR8*
PcieFmDebugGetWrapperNameString (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper
 );

typedef CONST CHAR8* F_PCIEFMDEBUGGETWRAPPERNAMESTRING (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper
 );

CONST CHAR8*
PcieFmDebugGetHostRegAddressSpaceString (
  IN      PCIe_SILICON_CONFIG            *Silicon,
  IN      UINT16                         AddressFrame
  );

typedef CONST CHAR8* F_PCIEFMDEBUGGETHOSTREGADDRESSSPACESTRING (
  IN      PCIe_SILICON_CONFIG            *Silicon,
  IN      UINT16                         AddressFrame
  );

PCIE_LINK_SPEED_CAP
PcieFmGetLinkSpeedCap (
  IN       UINT32                        Flags,
  IN       PCIe_ENGINE_CONFIG            *Engine
  );

typedef PCIE_LINK_SPEED_CAP F_PCIEFMGETLINKSPEEDCAP (
  IN       UINT32                        Flags,
  IN       PCIe_ENGINE_CONFIG            *Engine
  );

VOID
PcieFmSetLinkSpeedCap (
  IN      PCIE_LINK_SPEED_CAP            LinkSpeedCapability,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

typedef VOID F_PCIEFMSETLINKSPEEDCAP (
  IN      PCIE_LINK_SPEED_CAP            LinkSpeedCapability,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

UINT32
PcieFmGetNativePhyLaneBitmap (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  );

typedef UINT32 F_PCIEFMGETNATIVEPHYLANEBITMAP (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  );

AGESA_STATUS
PcieFmAlibBuildAcpiTable (
  IN       VOID                          *AlibSsdtPtr,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

AGESA_STATUS
PcieFmGetSbConfigInfo (
  IN       UINT8                        SocketId,
     OUT   PCIe_PORT_DESCRIPTOR         *SbPort,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  );

typedef AGESA_STATUS F_PCIEFMGETSBCONFIGINFO (
  IN       UINT8                        SocketId,
     OUT   PCIe_PORT_DESCRIPTOR         *SbPort,
  IN       AMD_CONFIG_PARAMS            *StdHeader
  );


/// PCIe config services
typedef struct {
  F_PCIEFMGETCOMPLEXDATALENGTH              *PcieFmGetComplexDataLength;              ///< PcieFmGetComplexDataLength
  F_PCIEFMBUILDCOMPLEXCONFIGURATION         *PcieFmBuildComplexConfiguration;         ///< PcieFmBuildComplexConfiguration
  F_PCIEFMCONFIGUREENGINESLANEALLOCATION    *PcieFmConfigureEnginesLaneAllocation;    ///< PcieFmConfigureEnginesLaneAllocation
  F_PCIEFMCHECKPORTPCIDEVICEMAPPING         *PcieFmCheckPortPciDeviceMapping;         ///< PcieFmCheckPortPciDeviceMapping
  F_PCIEFMMAPPORTPCIADDRESS                 *PcieFmMapPortPciAddress;                 ///< PcieFmMapPortPciAddress
  F_PCIEFMCHECKPORTPCIELANECANBEMUXED       *PcieFmCheckPortPcieLaneCanBeMuxed;       ///< PcieFmCheckPortPcieLaneCanBeMuxed
  F_PCIEFMGETSBCONFIGINFO                   *PcieFmGetSbConfigInfo;                   ///< PcieFmGetSbConfigInfo
} PCIe_FAM_CONFIG_SERVICES;

/// PCIe init services
typedef struct {
  F_PCIEFMGETCORECONFIGURATIONVALUE         *PcieFmGetCoreConfigurationValue;         ///< PcieFmGetCoreConfigurationValue
  F_PCIEFMGETLINKSPEEDCAP                   *PcieFmGetLinkSpeedCap;                   ///< PcieFmGetLinkSpeedCap
  F_PCIEFMGETNATIVEPHYLANEBITMAP            *PcieFmGetNativePhyLaneBitmap;            ///< PcieFmGetNativePhyLaneBitmap
  F_PCIEFMSETLINKSPEEDCAP                   *PcieFmSetLinkSpeedCap;                   ///< PcieFmSetLinkSpeedCap
} PCIe_FAM_INIT_SERVICES;

///PCIe debug services
typedef struct {
  F_PCIEFMDEBUGGETHOSTREGADDRESSSPACESTRING *PcieFmDebugGetHostRegAddressSpaceString; ///< PcieFmGetCoreConfigurationValue
  F_PCIEFMDEBUGGETWRAPPERNAMESTRING         *PcieFmDebugGetWrapperNameString;         ///< PcieFmDebugGetWrapperNameString
  F_PCIEFMDEBUGGETCORECONFIGURATIONSTRING   *PcieFmDebugGetCoreConfigurationString;   ///< PcieFmDebugGetCoreConfigurationString
} PCIe_FAM_DEBUG_SERVICES;

#endif
