/**
 * @file
 *
 * NB Initialization.
 *
 * Init IOAPIC/IOMMU/Misc NB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
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
 *
 ***************************************************************************/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Early post validate input parameters
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbLibEarlyPostInitValidateInput (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  AGESA_STATUS  Status;
  NB_CONFIG     *pNbConfig;
  NB_INFO       NbInfo;

  Status = AGESA_SUCCESS;
  NbInfo = LibNbGetRevisionInfo (pConfig);
  if (NbInfo.Type == NB_UNKNOWN) {
    return  AGESA_FATAL;
  }
  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  if (pNbConfig->sHeader.InitializerID != INITIALIZED_BY_INITIALIZER) {
    Status = NbLibInitializer (pConfig);
  }
  if (pNbConfig->SysMemoryTomBelow4G == 0) {
    Status =  AGESA_FATAL;
  }
  //pNbConfig->sHeader.InitializerID = PH_AmdEarlyPostInit;
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Mid/Late post validate input parameters
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbLibPostInitValidateInput (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  AGESA_STATUS  Status;
  NB_CONFIG     *pNbConfig;
  NB_INFO       NbInfo;

  NbInfo = LibNbGetRevisionInfo (pConfig);
  Status = AGESA_SUCCESS;
  if (NbInfo.Type == NB_UNKNOWN) {
    return  AGESA_FATAL;
  }
  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  if (pNbConfig->sHeader.InitializerID != INITIALIZED_BY_INITIALIZER) {
    Status = AGESA_FATAL;
  }
  //pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  //if (pNbConfig->sHeader.InitializerID != PH_AmdEarlyPostInit) {
  //  return  AGESA_FATAL;
  //}
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Prepare NB to boot to OS.
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbLibPrepareToOS (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG00, AccessS3SaveWidth32, 0xffffffff, BIT7, pConfig);
  return  AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set Multiple NB support
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

VOID
NbMultiNbIocInit (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  UINT32    Value;
  NB_CONFIG *pNbConfig;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  if (GET_BLOCK_CONFIG_PTR (pConfig)->NumberOfNorthbridges > 0) {
    if (pConfig->NbPciAddress.AddressValue == 0) {
      //Primary NB
      Value = BIT3 + (HT_INTERRUPT_ENCODING_OFFSET << 4);
    } else {
      //Secondary NB
      Value = BIT2 + (HT_INTERRUPT_ENCODING_OFFSET << 4);
    }
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG75, AccessS3SaveWidth32, (UINT32)~((0x7f << 2) + BIT28) , Value, pConfig);
  }
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG75, AccessS3SaveWidth32, (UINT32)~(BIT9 + BIT10 + BIT28), pNbConfig->P2PMode << 9, pConfig);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set NB SSID/SVID.
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

VOID
NbLibSetSSID (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  NB_CONFIG *pNbConfig;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  if (pNbConfig->SSID == 0xffffffff) {
    LibNbPciRead (pConfig->NbPciAddress.AddressValue, AccessWidth32, &pNbConfig->SSID, pConfig);
  }
  if (pNbConfig->SSID != 0) {
    LibNbPciWrite (pConfig->NbPciAddress.AddressValue | NB_PCI_REG50, AccessS3SaveWidth32, &pNbConfig->SSID, pConfig);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Setup UnitId clamping
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

VOID
NbLibSetupClumping (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  UINT16    ClumpingCapability;
  UINT16    Value;
  NB_CONFIG *pNbConfig;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  Value = 0;
  if (LibNbGetCpuFamily () == CPU_FAMILY_NPT) {
    return;
  }
  LibNbPciRead (pConfig->NbPciAddress.AddressValue | NB_PCI_REG58, AccessS3SaveWidth16, &ClumpingCapability, pConfig);
  if ((ClumpingCapability & BIT3) != 0 &&
      (pNbConfig->UnitIdClumping & DEV3_CLUMPING) != 0 &&
      !LibNbIsDevicePresent (PcieLibGetPortPciAddress (3, pConfig), pConfig)) {
    Value |= BIT3;
  }
  if ((ClumpingCapability & BIT12) != 0 &&
      (pNbConfig->UnitIdClumping & DEV12_CLUMPING) != 0 &&
      !LibNbIsDevicePresent (PcieLibGetPortPciAddress (12, pConfig), pConfig)) {
    Value |= BIT12;
  }
  if (Value != 0) {
    LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REG5C, AccessS3SaveWidth16, 0xffff, Value, pConfig);
    LibNbPciRMW (MAKE_SBDFO (0, 0, pConfig->NbHtPath.NodeID + 0x18, 0, HT_PATH_LINK_ID (pConfig->NbHtPath) * 4 + HT_PATH_SUBLINK_ID (pConfig->NbHtPath) * 0x10 + 0x110), AccessS3SaveWidth16, 0xffff, Value, pConfig);
  }
}



/*----------------------------------------------------------------------------------------*/
/**
 * Set top of memory  in NB.
 *    NB will not pass to CPU any upstream DMA request to address above TOM and TOM2
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbLibSetTopOfMemory (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  NB_CONFIG *pNbConfig;
  UINT32    RD890_TOM2;
  UINT32    RD890_TOM3;

  RD890_TOM2 = 0;
  RD890_TOM3 = 0;
  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  if (pNbConfig->SysMemoryTomBelow4G != 0) {
    LibNbPciRMW (pConfig->NbPciAddress.AddressValue | NB_PCI_REG90, AccessS3SaveWidth32, 0, ((UINT32)pNbConfig->SysMemoryTomBelow4G) << 20, pConfig);
  }
  if (pNbConfig->SysMemoryTomAbove4G != 0) {
    if ((pNbConfig->SysMemoryTomAbove4G - 1) <= 0xfffff) {
      RD890_TOM2 = pNbConfig->SysMemoryTomAbove4G;
    } else {
      RD890_TOM2 = (UINT32) (0xFD00000000 >> 20);
      RD890_TOM3 = pNbConfig->SysMemoryTomAbove4G;
    }
  }
  if (RD890_TOM2 != 0) {
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG31, AccessS3SaveWidth32, 0, (RD890_TOM2 >> 12), pConfig);
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG30, AccessS3SaveWidth32, 0, (RD890_TOM2 << 20) | 1, pConfig);
  }
  if (RD890_TOM3 != 0) {
    LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG4E, AccessS3SaveWidth32, 0, ((UINT32)pNbConfig->SysMemoryTomAbove4G >> 2) | BIT31, pConfig);
  }
  return  AGESA_SUCCESS;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Loget COre APic ID and dtore to scratch
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
NbLibGetCore0ApicId (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  UINT64    Value;
  UINT32    Apic20;
  NB_CONFIG *pNbConfig;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  LibAmdMsrRead (0x0000001B, &Value, (AMD_CONFIG_PARAMS *)pConfig);
  LibNbMemRead ((Value & 0xfffffffff000) + 0x20, AccessWidth32, &Apic20, pConfig);
  pNbConfig->Reserved = (UINT16) (Apic20 >> 24);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Direct NMI message to Core 0
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */
VOID
NbLibSetNmiRouting (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  NB_CONFIG *pNbConfig;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  LibNbPciIndexRMW (
    pConfig->NbPciAddress.AddressValue | NB_MISC_INDEX,
    NB_MISC_REG12,
    AccessS3SaveWidth32,
    0x00ffffff,
    (UINT32)pNbConfig->Reserved << 24,
    pConfig);
}

/*----------------------------------------------------------------------------------------*/
/**
 * AMD structures initializer for all NB.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdNbInitializer (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  Status = LibNbApiCall (NbLibInitializer, ConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NB config structure initializer
 *
 *
 *
 * @param[in] pConfig           Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
NbLibInitializer (
  IN OUT   AMD_NB_CONFIG *pConfig
  )
{
  UINT64    Value64;
  NB_CONFIG *pNbConfig;

  pNbConfig = GET_NB_CONFIG_PTR (pConfig);
  if (pNbConfig == NULL) {
    return  AGESA_WARNING;
  }
  if (pNbConfig->sHeader.InitializerID == INITIALIZED_BY_INITIALIZER) {
    return  AGESA_SUCCESS;
  }
  LibAmdMemFill (pNbConfig, 0, sizeof (NB_CONFIG), (AMD_CONFIG_PARAMS *)&(pNbConfig->sHeader));
  pNbConfig->sHeader.InitializerID = INITIALIZED_BY_INITIALIZER;
  // Get TOM and TOM2
  LibAmdMsrRead (0xC001001a, &Value64, (AMD_CONFIG_PARAMS *)pConfig);
  pNbConfig->SysMemoryTomBelow4G = (UINT16) (Value64 >> 20);
  LibAmdMsrRead (0xC0010010, &Value64, (AMD_CONFIG_PARAMS *)pConfig);
  if ((Value64 & BIT21) !=  0) {
    LibAmdMsrRead (0xC001001d, &Value64, (AMD_CONFIG_PARAMS *)pConfig);
    pNbConfig->SysMemoryTomAbove4G = (UINT32) (Value64 >> 20);
  }
  pNbConfig->P2PMode = 1;
  pNbConfig->UnitIdClumping = 3;
  return AGESA_SUCCESS;
}


