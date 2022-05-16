/* $NoKeywords:$ */
/**
 * @file
 *
 * Misc common definition
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87645 $   @e \$Date: 2013-02-06 13:08:17 -0600 (Wed, 06 Feb 2013) $
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
#ifndef _GNB_H_
#define _GNB_H_

#include <stdlib.h>

#include  "Ids.h"

#pragma pack (push, 1)

#define GNB_DEADLOOP() \
{ \
  VOLATILE BOOLEAN k; \
  k = TRUE; \
  while (k) { \
  } \
}
#ifdef IDSOPT_TRACING_ENABLED
  #if (IDSOPT_TRACING_ENABLED == TRUE)
    #define GNB_TRACE_ENABLE
  #endif
#endif


#ifndef GNB_DEBUG_CODE
  #ifdef GNB_TRACE_ENABLE
    #define  GNB_DEBUG_CODE(Code) Code
  #else
    #define  GNB_DEBUG_CODE(Code)
  #endif
#endif

#define OFF 0

#define PVOID UINTN

#define STRING_TO_UINT32(a, b, c, d) ((UINT32) ((d << 24) | (c << 16) | (b << 8) | a))

#define GnbLibGetHeader(x)  ((AMD_CONFIG_PARAMS*) (x)->StdHeader)

#define AGESA_STATUS_UPDATE(Current, Aggregated) \
if (Current > Aggregated) { \
  Aggregated = Current; \
}

#ifndef offsetof
  #define offsetof(s, m) (UINTN)&(((s *)0)->m)
#endif


//Table properties

#define TABLE_PROPERTY_DEFAULT                     0x00000000ul
#define TABLE_PROPERTY_IGFX_DISABLED               0x00000001ul
#define TABLE_PROPERTY_IOMMU_DISABLED              0x00000002ul
#define TABLE_PROPERTY_LCLK_DEEP_SLEEP             0x00000004ul
#define TABLE_PROPERTY_BAPM                        0x00000100ul
#define TABLE_PROPERTY_NBDPM                       0x00000800ul
#define TABLE_PROPERTY_LOADLINE_ENABLE             0x00001000ul
#define TABLE_PROPERTY_LHTC                        0x00010000ul
#define TABLE_PROPERTY_SVI2                        0x00020000ul

//Register access flags Flags
#define GNB_REG_ACC_FLAG_S3SAVE                    0x00000001ul

// Gnb PCIe Master PLL
#define GNB_PCIE_MASTERPLL_A   0xA
#define GNB_PCIE_MASTERPLL_B   0xB
#define GNB_PCIE_MASTERPLL_C   0xC
#define GNB_PCIE_MASTERPLL_D   0xD

/// LCLK DPM enable control
typedef enum {
  LclkDpmDisabled,          ///<LCLK DPM disabled
  LclkDpmRcActivity,        ///<LCLK DPM enabled and use Root Complex Activity monitor method
} LCLK_DPM_MODE;




/// Topology information
typedef struct {
  BOOLEAN   PhantomFunction;    ///< PCIe topology have device with phantom function
  BOOLEAN   PcieToPciexBridge;  ///< PCIe topology have device with Pcieto Pcix bridge
} GNB_TOPOLOGY_INFO;


/// GNB installable services
typedef enum {
  GnbPcieFamConfigService,                ///< PCIe config service
  GnbPcieFamInitService,                  ///< PCIe Init service
  GnbPcieFamDebugService,                 ///< PCIe Debug service
  GnbRegisterAccessService,               ///< GNB register access service
  GnbIommuService,                        ///< GNB IOMMU config service
  GnbUraService,                          ///< GNB Ura service
  GfxFamService,                          ///< GFX Family service
  GnbPcieMaxPayloadService,               ///< GNB MaxPayload service
  GnbFamTsService                         ///< GNB TimeStamp service
} GNB_SERVICE_ID;

/// GNB service entry
typedef struct _GNB_SERVICE {
  GNB_SERVICE_ID        ServiceId;        ///< Service ID
  UINT64                Family;           ///< CPU family
  CONST VOID                  *ServiceProtocol; ///< Service protocol
  CONST struct  _GNB_SERVICE  *NextService;     ///< Pointer to next service
} GNB_SERVICE;

/// GNB SMU Firmware Pointers
typedef struct _GNB_FW_PTRS {
  VOID                  *FirmwareType1;   ///< Pointer to Type 1 SMU Firmware
  VOID                  *FirmwareType2;   ///< Poitner to Type 2 SMU Firmware
} GNB_FW_PTRS;


#define GNB_STRINGIZE(x)                  #x
#define GNB_SERVICE_DEFINITIONS(x)        GNB_STRINGIZE (Services/x/x.h)
#define GNB_MODULE_DEFINITIONS(x)         GNB_STRINGIZE (Modules/x/x.h)
#define GNB_MODULE_INSTALL(x)             GNB_STRINGIZE (Modules/x/x##Install.h)
#pragma pack (pop)

#endif
