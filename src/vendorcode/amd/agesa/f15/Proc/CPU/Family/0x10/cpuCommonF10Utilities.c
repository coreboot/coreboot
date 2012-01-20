/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 specific utility functions.
 *
 * Provides numerous utility functions specific to family 10h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
#include "cpuFamilyTranslation.h"
#include "cpuCommonF10Utilities.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUCOMMONF10UTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/**
 * Node ID MSR register fields.
 * Provide the layout of fields in the Node ID MSR.
 */
typedef struct {
  UINT64    NodeId:3;                 ///< The core is on the node with this node id.
  UINT64    NodesPerProcessor:3;      ///< The number of Nodes in this processor.
  UINT64    HeapIndex:6;              ///< The AP core heap index.
  UINT64    :(63 - 11);               ///< Reserved.
} NODE_ID_MSR_FIELDS;

/// Node ID MSR.
typedef union {
  NODE_ID_MSR_FIELDS   Fields;        ///< Access the register as individual fields
  UINT64               Value;         ///< Access the register value.
} NODE_ID_MSR;

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
F10SetAgesaWarmResetFlag (
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
 *  @param[in]   FamilySpecificServices   The current Family Specific Services.
 *  @param[in]   StdHeader                Config handle for library and services
 *  @param[out]  Request                  Indicate warm reset status
 *
 */
VOID
F10GetAgesaWarmResetFlag (
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
 *  Use the Mailbox Register to get the Ap Mailbox info for the current core.
 *
 *  @CpuServiceMethod{::F_CPU_AMD_GET_AP_MAILBOX_FROM_HARDWARE}.
 *
 *  Access the mailbox register used with this NB family.  This is valid until the
 *  point that some init code initializes the mailbox register for its normal use.
 *  The Machine Check Misc (Thresholding) register is available as both a PCI config
 *  register and a MSR, so it can be used as a mailbox from HT to other functions.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[out]    ApMailboxInfo           The AP Mailbox info
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F10GetApMailboxFromHardware (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
     OUT   AP_MAILBOXES           *ApMailboxInfo,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64 MailboxInfo;

  LibAmdMsrRead (MSR_MC_MISC_LINK_THRESHOLD, &MailboxInfo, StdHeader);
  // Mailbox info is in bits 32 thru 43, 12 bits.
  ApMailboxInfo->ApMailInfo.Info = (((UINT32) (MailboxInfo >> 32)) & (UINT32)0x00000FFF);
  LibAmdMsrRead (MSR_MC_MISC_L3_THRESHOLD, &MailboxInfo, StdHeader);
  // Mailbox info is in bits 32 thru 43, 12 bits.
  ApMailboxInfo->ApMailExtInfo.Info = (((UINT32) (MailboxInfo >> 32)) & (UINT32)0x00000FFF);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Set the system AP core number in the AP's Mailbox.
 *
 *  @CpuServiceMethod{::F_CPU_SET_AP_CORE_NUMBER}.
 *
 *  Access the mailbox register used with this NB family.  This is only intended to
 *  run on the BSC at the time of initial AP launch.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     Socket                  The AP's socket
 *  @param[in]     Module                  The AP's module
 *  @param[in]     ApCoreNumber            The AP's unique core number
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F10SetApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       UINT32                 Socket,
  IN       UINT32                 Module,
  IN       UINT32                 ApCoreNumber,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32   LocalPciRegister;
  PCI_ADDR PciAddress;
  AGESA_STATUS IgnoredStatus;

  GetPciAddress (StdHeader, Socket, Module, &PciAddress, &IgnoredStatus);
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = 0x170;
  LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  ((AP_MAIL_EXT_INFO *) &LocalPciRegister)->Fields.HeapIndex = ApCoreNumber;
  LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Get this AP's system core number from hardware.
 *
 *  @CpuServiceMethod{::F_CPU_GET_AP_CORE_NUMBER}.
 *
 *  Returns the system core number from the scratch MSR, where
 *  it was saved at heap initialization.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 *  @return        The AP's unique core number
 */
UINT32
F10GetApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  NODE_ID_MSR NodeIdMsr;

  LibAmdMsrRead (0xC001100C, &NodeIdMsr.Value, StdHeader);
  return (UINT32) NodeIdMsr.Fields.HeapIndex;
}


/*---------------------------------------------------------------------------------------*/
/**
 *  Move the AP's core number from the mailbox to hardware.
 *
 *  @CpuServiceMethod{::F_CPU_TRANSFER_AP_CORE_NUMBER}.
 *
 *  Transfers this AP's system core number from the mailbox to
 *  the NodeId MSR and initializes the other NodeId fields.
 *
 *  @param[in]     FamilySpecificServices  The current Family Specific Services.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
VOID
F10TransferApCoreNumber (
  IN       CPU_SPECIFIC_SERVICES  *FamilySpecificServices,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AP_MAILBOXES Mailboxes;
  NODE_ID_MSR NodeIdMsr;
  UINT64 ExtFeatures;

  NodeIdMsr.Value = 0;
  FamilySpecificServices->GetApMailboxFromHardware (FamilySpecificServices, &Mailboxes, StdHeader);
  NodeIdMsr.Fields.HeapIndex = Mailboxes.ApMailExtInfo.Fields.HeapIndex;
  NodeIdMsr.Fields.NodeId = Mailboxes.ApMailInfo.Fields.Node;
  NodeIdMsr.Fields.NodesPerProcessor = Mailboxes.ApMailInfo.Fields.ModuleType;
  LibAmdMsrWrite (0xC001100C, &NodeIdMsr.Value, StdHeader);

  // Indicate that the NodeId MSR is supported.
  LibAmdMsrRead (MSR_CPUID_EXT_FEATS, &ExtFeatures, StdHeader);
  ExtFeatures = (ExtFeatures | BIT51);
  LibAmdMsrWrite (MSR_CPUID_EXT_FEATS, &ExtFeatures, StdHeader);
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
F10CpuAmdCoreIdPositionInInitialApicId (
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
