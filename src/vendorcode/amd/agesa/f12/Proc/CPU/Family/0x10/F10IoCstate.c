/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 IO C-state feature support functions.
 *
 * Provides the functions necessary to initialize the IO C-state feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/F10
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
#include "cpuFeatures.h"
#include "cpuIoCstate.h"
#include "cpuF10PowerMgmt.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "CommonReturns.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_CPU_FAMILY_0X10_F10IOCSTATE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
F10InitializeIoCstateOnCore (
  IN       VOID *CstateBaseMsr,
  IN       AMD_CONFIG_PARAMS *StdHeader
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
 *  Enable IO Cstate on a family 10h CPU.
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
F10InitializeIoCstate (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT64                    EntryPoint,
  IN       PLATFORM_CONFIGURATION    *PlatformConfig,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT64   LocalMsrRegister;
  AP_TASK  TaskPtr;

  if ((EntryPoint & CPU_FEAT_AFTER_PM_INIT) != 0) {
    // Initialize MSRC001_0073[CstateAddr] on each core to a region of
    // the IO address map with 8 consecutive available addresses.
    LocalMsrRegister = 0;

    ((CSTATE_ADDRESS_MSR *) &LocalMsrRegister)->CstateAddr = PlatformConfig->CStateIoBaseAddress;

    TaskPtr.FuncAddress.PfApTaskI = F10InitializeIoCstateOnCore;
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
 *  Enable CState on a family 10h core.
 *
 * @param[in]    CstateBaseMsr      MSR value to write to C001_0073 as determined by core 0.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 */
VOID
STATIC
F10InitializeIoCstateOnCore (
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
F10GetAcpiCstObj (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       PLATFORM_CONFIGURATION    *PlatformConfig,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  return (CST_HEADER_SIZE + CST_BODY_SIZE);
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
F10CreateAcpiCstObj (
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
  CstBodyPtr->RegisterAddr   = ((CSTATE_ADDRESS_MSR *) &MsrData)->CstateAddr;
  CstBodyPtr->EndTag         = 0x0079;
  CstBodyPtr->BytePrefix2    = BYTE_PREFIX_OPCODE;
  CstBodyPtr->Type           = CST_C2_TYPE;
  CstBodyPtr->WordPrefix     = WORD_PREFIX_OPCODE;
  CstBodyPtr->Latency        = 0x4B;
  CstBodyPtr->DWordPrefix    = DWORD_PREFIX_OPCODE;
  CstBodyPtr->Power          = 0;

  CstBodyPtr++;

  //Update the pointer
  *PstateAcpiBufferPtr = CstBodyPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Routine to check whether IO Cstate should be supported.
 *
 * @param[in]      IoCstateServices      IO Cstate services.
 * @param[in]      Socket                Zero-based socket number.
 * @param[in]      StdHeader             Config Handle for library, services.
 *
 * @retval         TRUE                  Support IO Cstate.
 * @retval         FALSE                 Do not support IO Cstate.
 *
 */
BOOLEAN
F10IsIoCstateFeatureSupported (
  IN       IO_CSTATE_FAMILY_SERVICES *IoCstateServices,
  IN       UINT32                    Socket,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT64 LocalMsrRegister;
  CPUID_DATA     CpuId;
  CPU_LOGICAL_ID LogicalId;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  // Only Rev.E processor with CPB enabled and ucode 010000BF or later loaded
  // MSR_C001_0073 can be programmed
  if ((LogicalId.Revision & AMD_F10_Ex) != 0) {
    LibAmdCpuidRead (AMD_CPUID_APM, &CpuId, StdHeader);
    if (((CpuId.EDX_Reg & 0x00000200) >> 9) == 1) {
      LibAmdMsrRead (MSR_PATCH_LEVEL, &LocalMsrRegister, StdHeader);
      if ((LocalMsrRegister & 0xffffffff) >= 0x010000BF) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

CONST IO_CSTATE_FAMILY_SERVICES ROMDATA F10IoCstateSupport =
{
  0,
  F10IsIoCstateFeatureSupported,
  F10InitializeIoCstate,
  F10GetAcpiCstObj,
  F10CreateAcpiCstObj,
  (PF_IO_CSTATE_IS_CSD_GENERATED) CommonReturnFalse
};
