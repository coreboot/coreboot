/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_12 IO C-state feature support functions.
 *
 * Provides the functions necessary to initialize the IO C-state feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F12
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "cpuIoCstate.h"
#include "cpuF12PowerMgmt.h"
#include "cpuLateInit.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "CommonReturns.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FAMILY_0X12_F12IOCSTATE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
F12InitializeIoCstateOnCore (
  IN       VOID *CstateBaseMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable IO Cstate on a family 12h CPU.
 *  Implement steps 1 to 3 of BKDG section 2.5.3.2.9 BIOS Requirements for Initialization
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
F12InitializeIoCstate (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT64                    EntryPoint,
  IN       PLATFORM_CONFIGURATION    *PlatformConfig,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT32   i;
  UINT32   MaxEnabledPstate;
  UINT32   LocalPciRegister;
  UINT64   LocalMsrRegister;
  AP_TASK  TaskPtr;
  PCI_ADDR PciAddress;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    for (i = MSR_PSTATE_7; i > MSR_PSTATE_0; i--) {
      LibAmdMsrRead (i, &LocalMsrRegister, StdHeader);
      if (((PSTATE_MSR *) &LocalMsrRegister)->PsEnable == 1) {
        break;
      }
    }
    MaxEnabledPstate = i - MSR_PSTATE_0;
    // Initialize MSRC001_0073[CstateAddr] on each core to a region of
    // the IO address map with 8 consecutive available addresses.
    LocalMsrRegister = 0;
    ((CSTATE_ADDRESS_MSR *) &LocalMsrRegister)->CstateAddr = PlatformConfig->CStateIoBaseAddress;
    ASSERT ((((CSTATE_ADDRESS_MSR *) &LocalMsrRegister)->CstateAddr != 0) &&
            (((CSTATE_ADDRESS_MSR *) &LocalMsrRegister)->CstateAddr <= 0xFFF8));

    TaskPtr.FuncAddress.PfApTaskI = F12InitializeIoCstateOnCore;
    TaskPtr.DataTransfer.DataSizeInDwords = 2;
    TaskPtr.DataTransfer.DataPtr = &LocalMsrRegister;
    TaskPtr.DataTransfer.DataTransferFlags = 0;
    TaskPtr.ExeFlags = WAIT_FOR_CORE;
    ApUtilRunCodeOnAllLocalCoresAtEarly (&TaskPtr, StdHeader, NULL);

    // Program D18F4x1A8[PService] to the index of lowest-performance
    // P-state with MSRC001_00[6B:64][PstateEn]==1 on core 0.
    PciAddress.AddressValue = CPU_STATE_PM_CTRL0_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ((CPU_STATE_PM_CTRL0_REGISTER *) &LocalPciRegister)->PService = MaxEnabledPstate;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);

    // Program D18F4x1AC[CstPminEn] to 1.
    PciAddress.AddressValue = CPU_STATE_PM_CTRL1_PCI_ADDR;
    LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
    ((CPU_STATE_PM_CTRL1_REGISTER *) &LocalPciRegister)->CstPminEn = 1;
    LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
  }
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Enable C-State on a family 12h core.
 *
 * @param[in]    CstateBaseMsr      MSR value to write to C001_0073 as determined by core 0.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F12InitializeIoCstateOnCore (
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
 * @param[in]    IoCstateServices   IoCstate services.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       CstObjSize         Size of CST Object
 *
 */
UINT32
STATIC
F12GetAcpiCstObj (
  IN       IO_CSTATE_FAMILY_SERVICES   *IoCstateServices,
  IN       PLATFORM_CONFIGURATION      *PlatformConfig,
  IN       AMD_CONFIG_PARAMS           *StdHeader
  )
{
  return (CST_HEADER_SIZE + CST_BODY_SIZE);
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Routine to generate the ACPI C-State objects
 *
 * @param[in]      IoCstateServices      IO Cstate services.
 * @param[in]      LocalApicId           Local Apic Id
 * @param[in, out] PstateAcpiBufferPtr   Pointer to Pstate data buffer.
 * @param[in]      StdHeader             Config Handle for library, services.
 *
 */
VOID
STATIC
F12CreateAcpiCstObj (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT8                     LocalApicId,
  IN OUT   VOID                      **PstateAcpiBufferPtr,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT64            MsrData;
  CST_HEADER_STRUCT *CstHeaderPtr;
  CST_BODY_STRUCT   *CstBodyPtr;

  // Read from MSR C0010073 to obtain CstateAddr
  LibAmdMsrRead (MSR_CSTATE_ADDRESS, &MsrData, StdHeader);
  ASSERT ((((CSTATE_ADDRESS_MSR *) &MsrData)->CstateAddr != 0) &&
          (((CSTATE_ADDRESS_MSR *) &MsrData)->CstateAddr <= 0xFFF8));

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
  CstBodyPtr->Latency        = 0x64;
  CstBodyPtr->DWordPrefix    = DWORD_PREFIX_OPCODE;
  CstBodyPtr->Power          = 0;

  CstBodyPtr++;

  //Update the pointer
  *PstateAcpiBufferPtr = CstBodyPtr;
}

CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F12IoCstateSupport =
{
  0,
  (PF_IO_CSTATE_IS_SUPPORTED) CommonReturnTrue,
  F12InitializeIoCstate,
  F12GetAcpiCstObj,
  F12CreateAcpiCstObj,
  (PF_IO_CSTATE_IS_CSD_GENERATED) CommonReturnFalse
};

