/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_15 Trinity IO C-state feature support functions.
 *
 * Provides the functions necessary to initialize the IO C-state feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15/TN
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
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuFeatures.h"
#include "cpuIoCstate.h"
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "CommonReturns.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)
#define FILECODE PROC_CPU_FAMILY_0X15_TN_F15TNIOCSTATE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
F15TnInitializeIoCstateOnCore (
  IN       VOID *CstateBaseMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
F15TnIsCsdObjGenerated (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE            IoCstateFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable IO Cstate on a family 15h Trinity CPU.
 *  Implement BIOS Requirements for Initialization of C-states
 *
 * @param[in]    IoCstateServices   Pointer to this CPU's IO Cstate family services.
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
F15TnInitializeIoCstate (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT64                    EntryPoint,
  IN       PLATFORM_CONFIGURATION    *PlatformConfig,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT64                              LocalMsrRegister;
  AP_TASK                             TaskPtr;
  PCI_ADDR                            PciAddress;
  CSTATE_POLICY_CTRL1_REGISTER        CstatePolicyCtrl1;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    // Initialize F4x128
    // bit[1]   CoreCstatePolicy = 0
    // bit[4:2] HaltCstateIndex = 0
    // bit[31]  CstateMsgDis = 1
    PciAddress.AddressValue = CSTATE_POLICY_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &CstatePolicyCtrl1, StdHeader);
    CstatePolicyCtrl1.CoreCstatePolicy = 0;
    CstatePolicyCtrl1.HaltCstateIndex = 0;
    CstatePolicyCtrl1.CstateMsgDis = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &CstatePolicyCtrl1, StdHeader);

    // Initialize MSRC001_0073[CstateAddr] on each core to a region of
    // the IO address map with 8 consecutive available addresses.
    LocalMsrRegister = 0;

    IDS_HDT_CONSOLE (CPU_TRACE, "    Init IO C-state Base at 0x%x\n", PlatformConfig->CStateIoBaseAddress);
    ((CSTATE_ADDRESS_MSR *) &LocalMsrRegister)->CstateAddr = PlatformConfig->CStateIoBaseAddress;

    TaskPtr.FuncAddress.PfApTaskI = F15TnInitializeIoCstateOnCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 2;
    TaskPtr.DataTransfer.DataPtr = &LocalMsrRegister;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, NULL);
  }
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable CState on a family 15h Trinity core.
 *
 * @param[in]    CstateBaseMsr      MSR value to write to C001_0073 as determined by core 0.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F15TnInitializeIoCstateOnCore (
  IN       VOID *CstateBaseMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  // Initialize MSRC001_0073[CstateAddr] on each core
  LibAmdMsrWrite (MSR_CSTATE_ADDRESS, (UINT64 *) CstateBaseMsr, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Returns the size of CST object
 *
 * @param[in]    IoCstateServices   IO Cstate services.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       CstObjSize         Size of CST Object
 *
 */
UINT32
STATIC
F15TnGetAcpiCstObj (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       PLATFORM_CONFIGURATION    *PlatformConfig,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  BOOLEAN                   GenerateCsdObj;
  UINT32                    CStateAcpiObjSize;
  IO_CSTATE_FAMILY_SERVICES *FamilyServices;
  ACPI_CST_GET_INPUT CstGetInput;

  CstGetInput.IoCstateServices = IoCstateServices;
  CstGetInput.PlatformConfig = PlatformConfig;
  CstGetInput.CStateAcpiObjSizePtr = &CStateAcpiObjSize;

  IDS_SKIP_HOOK (IDS_CST_SIZE, &CstGetInput, StdHeader) {
    CStateAcpiObjSize = CST_HEADER_SIZE + CST_BODY_SIZE;

    // If CSD Object is generated, add the size of CSD Object to the total size of
    // CState ACPI Object size
    GetFeatureServicesOfCurrentCore (&IoCstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);
    GenerateCsdObj = FamilyServices->IsCsdObjGenerated (FamilyServices, StdHeader);

    if (GenerateCsdObj) {
      CStateAcpiObjSize += CSD_HEADER_SIZE + CSD_BODY_SIZE;
    }
  }
  return CStateAcpiObjSize;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Routine to generate the C-State ACPI objects
 *
 * @param[in]      IoCstateServices       IO Cstate services.
 * @param[in]      LocalApicId            Local Apic Id for each core.
 * @param[in, out] **PstateAcpiBufferPtr  Pointer to the Acpi Buffer Pointer.
 * @param[in]      StdHeader              Config Handle for library, services.
 *
 */
VOID
STATIC
F15TnCreateAcpiCstObj (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT8                     LocalApicId,
  IN OUT   VOID                      **PstateAcpiBufferPtr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT64                MsrData;
  BOOLEAN               GenerateCsdObj;
  CST_HEADER_STRUCT     *CstHeaderPtr;
  CST_BODY_STRUCT       *CstBodyPtr;
  CSD_HEADER_STRUCT     *CsdHeaderPtr;
  CSD_BODY_STRUCT       *CsdBodyPtr;
  IO_CSTATE_FAMILY_SERVICES    *FamilyServices;
  ACPI_CST_CREATE_INPUT  CstInput;

  CstInput.IoCstateServices = IoCstateServices;
  CstInput.LocalApicId = LocalApicId;
  CstInput.PstateAcpiBufferPtr = PstateAcpiBufferPtr;

  IDS_SKIP_HOOK (IDS_CST_CREATE, &CstInput, StdHeader) {
    // Read from MSR C0010073 to obtain CstateAddr
    LibAmdMsrRead (MSR_CSTATE_ADDRESS, &MsrData, StdHeader);

    // Typecast the pointer
    CstHeaderPtr = (CST_HEADER_STRUCT *) *PstateAcpiBufferPtr;

    // Set CST Header
    CstHeaderPtr->NameOpcode  = NAME_OPCODE;
    CstHeaderPtr->CstName_a__ = CST_NAME__;
    CstHeaderPtr->CstName_a_C = CST_NAME_C;
    CstHeaderPtr->CstName_a_S = CST_NAME_S;
    CstHeaderPtr->CstName_a_T = CST_NAME_T;

    // Typecast the pointer
    CstHeaderPtr++;
    CstBodyPtr = (CST_BODY_STRUCT *) CstHeaderPtr;

    // Set CST Body
    CstBodyPtr->PkgOpcode      = PACKAGE_OPCODE;
    CstBodyPtr->PkgLength      = CST_LENGTH;
    CstBodyPtr->PkgElements    = CST_NUM_OF_ELEMENTS;
    CstBodyPtr->BytePrefix     = BYTE_PREFIX_OPCODE;
    CstBodyPtr->Count          = CST_COUNT;
    CstBodyPtr->PkgOpcode2     = PACKAGE_OPCODE;
    CstBodyPtr->PkgLength2     = CST_PKG_LENGTH;
    CstBodyPtr->PkgElements2   = CST_PKG_ELEMENTS;
    CstBodyPtr->BufferOpcode   = BUFFER_OPCODE;
    CstBodyPtr->BufferLength   = CST_SUBPKG_LENGTH;
    CstBodyPtr->BufferElements = CST_SUBPKG_ELEMENTS;
    CstBodyPtr->BufferOpcode2  = BUFFER_OPCODE;
    CstBodyPtr->GdrOpcode      = GENERIC_REG_DESCRIPTION;
    CstBodyPtr->GdrLength      = CST_GDR_LENGTH;
    CstBodyPtr->AddrSpaceId    = GDR_ASI_SYSTEM_IO;
    CstBodyPtr->RegBitWidth    = 0x08;
    CstBodyPtr->RegBitOffset   = 0x00;
    CstBodyPtr->AddressSize    = GDR_ASZ_BYTE_ACCESS;
    CstBodyPtr->RegisterAddr   = ((CSTATE_ADDRESS_MSR *) &MsrData)->CstateAddr + 1;
    CstBodyPtr->EndTag         = 0x0079;
    CstBodyPtr->BytePrefix2    = BYTE_PREFIX_OPCODE;
    CstBodyPtr->Type           = CST_C2_TYPE;
    CstBodyPtr->WordPrefix     = WORD_PREFIX_OPCODE;
    CstBodyPtr->Latency        = 100;
    CstBodyPtr->DWordPrefix    = DWORD_PREFIX_OPCODE;
    CstBodyPtr->Power          = 0;

    CstBodyPtr++;
    //Update the pointer
    *PstateAcpiBufferPtr = CstBodyPtr;


  // Check whether CSD object should be generated
    GetFeatureServicesOfCurrentCore (&IoCstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
    ASSERT (FamilyServices != NULL);
    GenerateCsdObj = FamilyServices->IsCsdObjGenerated (FamilyServices, StdHeader);

    if (GenerateCsdObj) {
      CsdHeaderPtr = (CSD_HEADER_STRUCT *) *PstateAcpiBufferPtr;

      // Set CSD Header
      CsdHeaderPtr->NameOpcode  = NAME_OPCODE;
      CsdHeaderPtr->CsdName_a__ = CST_NAME__;
      CsdHeaderPtr->CsdName_a_C = CST_NAME_C;
      CsdHeaderPtr->CsdName_a_S = CST_NAME_S;
      CsdHeaderPtr->CsdName_a_D = CSD_NAME_D;

      CsdHeaderPtr++;
      CsdBodyPtr = (CSD_BODY_STRUCT *) CsdHeaderPtr;

      // Set CSD Body
      CsdBodyPtr->PkgOpcode         = PACKAGE_OPCODE;
      CsdBodyPtr->PkgLength         = CSD_BODY_SIZE - 1;
      CsdBodyPtr->PkgElements       = 1;
      CsdBodyPtr->PkgOpcode2        = PACKAGE_OPCODE;
      CsdBodyPtr->PkgLength2        = CSD_BODY_SIZE - 4; // CSD_BODY_SIZE - Package() - Package Opcode
      CsdBodyPtr->PkgElements2      = 6;
      CsdBodyPtr->BytePrefix        = BYTE_PREFIX_OPCODE;
      CsdBodyPtr->NumEntries        = 6;
      CsdBodyPtr->BytePrefix2       = BYTE_PREFIX_OPCODE;
      CsdBodyPtr->Revision          = 0;
      CsdBodyPtr->DWordPrefix       = DWORD_PREFIX_OPCODE;
      CsdBodyPtr->Domain            = (LocalApicId & 0xFE) >> 1;
      CsdBodyPtr->DWordPrefix2      = DWORD_PREFIX_OPCODE;
      CsdBodyPtr->CoordType         = CSD_COORD_TYPE_HW_ALL;
      CsdBodyPtr->DWordPrefix3      = DWORD_PREFIX_OPCODE;
      CsdBodyPtr->NumProcessors     = 0x2;
      CsdBodyPtr->DWordPrefix4      = DWORD_PREFIX_OPCODE;
      CsdBodyPtr->Index             = 0x0;

      CsdBodyPtr++;

      // Update the pointer
      *PstateAcpiBufferPtr = CsdBodyPtr;
    }
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Routine to check whether CSD object should be created.
 *
 * @param[in]      IoCstateServices      IO Cstate services.
 * @param[in]      StdHeader             Config Handle for library, services.
 *
 * @retval         TRUE                  CSD Object should be created.
 * @retval         FALSE                 CSD Object should not be created.
 *
 */
BOOLEAN
F15TnIsCsdObjGenerated (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  // CSD Object should only be created when there are two cores per compute unit
  if (GetComputeUnitMapping (StdHeader) == EvenCoresMapping) {
    return TRUE;
  }
  return FALSE;
}

CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F15TnIoCstateSupport =
{
  0,
  (PF_IO_CSTATE_IS_SUPPORTED) CommonReturnTrue,
  F15TnInitializeIoCstate,
  F15TnGetAcpiCstObj,
  F15TnCreateAcpiCstObj,
  F15TnIsCsdObjGenerated
};
