/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA CPU Preserve Registers used for AP Mailbox.
 *
 * Save and Restore the normal feature content of the registers being used for
 * the AP Mailbox.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
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
#include "Topology.h"
#include "GeneralServices.h"
#include "OptionMultiSocket.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuFeatures.h"
#include "PreserveMailbox.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_PRESERVEMAILBOX_FILECODE
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
extern CPU_FAMILY_SUPPORT_TABLE PreserveMailboxFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *  The contents of the mailbox registers should always be preserved.
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               Always TRUE
 *
 */
BOOLEAN
STATIC
IsPreserveAroundMailboxEnabled (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  return TRUE;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Save and Restore or Initialize the content of the mailbox registers.
 *
 * The registers used for AP mailbox should have the content related to their function
 * preserved.
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       AGESA_SUCCESS      Always succeeds.
 *
 */
AGESA_STATUS
STATIC
PreserveMailboxes (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PRESERVE_MAILBOX_FAMILY_SERVICES *FamilySpecificServices;
  UINT32 Socket;
  UINT32 Module;
  PCI_ADDR BaseAddress;
  PCI_ADDR MailboxRegister;
  PRESERVE_MAILBOX_FAMILY_REGISTER *NextRegister;
  AGESA_STATUS IgnoredStatus;
  AGESA_STATUS HeapStatus;
  UINT32 Value;
  ALLOCATE_HEAP_PARAMS AllocateParams;
  LOCATE_HEAP_PTR LocateParams;
  UINT32 RegisterEntryIndex;

  BaseAddress.AddressValue = ILLEGAL_SBDFO;

  if (EntryPoint == CPU_FEAT_AFTER_COHERENT_DISCOVERY) {
    // The save step.  Save either the register content or zero (for cold boot, if family specifies that).
    AllocateParams.BufferHandle = PRESERVE_MAIL_BOX_HANDLE;
    AllocateParams.RequestedBufferSize = (sizeof (UINT32) * (MAX_PRESERVE_REGISTER_ENTRIES * (MAX_SOCKETS * MAX_DIES)));
    AllocateParams.Persist = HEAP_SYSTEM_MEM;
    HeapStatus = HeapAllocateBuffer (&AllocateParams, StdHeader);
    ASSERT ((HeapStatus == AGESA_SUCCESS) && (AllocateParams.BufferPtr != NULL));
    LibAmdMemFill (AllocateParams.BufferPtr, 0xFF, AllocateParams.RequestedBufferSize, StdHeader);
    RegisterEntryIndex = 0;
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (StdHeader, Socket, Module, &BaseAddress, &IgnoredStatus)) {
          GetFeatureServicesOfSocket (&PreserveMailboxFamilyServiceTable, Socket, (CONST VOID **)&FamilySpecificServices, StdHeader);
          ASSERT (FamilySpecificServices != NULL);
          NextRegister = FamilySpecificServices->RegisterList;
          while (NextRegister->Register.AddressValue != ILLEGAL_SBDFO) {
            ASSERT (RegisterEntryIndex <
                    (MAX_PRESERVE_REGISTER_ENTRIES * GetPlatformNumberOfSockets () * GetPlatformNumberOfModules ()));
            if (FamilySpecificServices->IsZeroOnCold && (!IsWarmReset (StdHeader))) {
              Value = 0;
            } else {
              MailboxRegister = BaseAddress;
              MailboxRegister.Address.Function = NextRegister->Register.Address.Function;
              MailboxRegister.Address.Register = NextRegister->Register.Address.Register;
              LibAmdPciRead (AccessWidth32, MailboxRegister, &Value, StdHeader);
              Value &= NextRegister->Mask;
            }
            (* (MAILBOX_REGISTER_SAVE_ENTRY) AllocateParams.BufferPtr) [RegisterEntryIndex] = Value;
            RegisterEntryIndex++;
            NextRegister++;
          }
        }
      }
    }
  } else if ((EntryPoint == CPU_FEAT_INIT_LATE_END) || (EntryPoint == CPU_FEAT_AFTER_RESUME_MTRR_SYNC)) {
    // The restore step.  Just write out the saved content in the buffer.
    LocateParams.BufferHandle = PRESERVE_MAIL_BOX_HANDLE;
    HeapStatus = HeapLocateBuffer (&LocateParams, StdHeader);
    ASSERT ((HeapStatus == AGESA_SUCCESS) && (LocateParams.BufferPtr != NULL));
    RegisterEntryIndex = 0;
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (StdHeader, Socket, Module, &BaseAddress, &IgnoredStatus)) {
          GetFeatureServicesOfSocket (&PreserveMailboxFamilyServiceTable, Socket, (CONST VOID **)&FamilySpecificServices, StdHeader);
          NextRegister = FamilySpecificServices->RegisterList;
          while (NextRegister->Register.AddressValue != ILLEGAL_SBDFO) {
            ASSERT (RegisterEntryIndex <
                    (MAX_PRESERVE_REGISTER_ENTRIES * GetPlatformNumberOfSockets () * GetPlatformNumberOfModules ()));
            MailboxRegister = BaseAddress;
            MailboxRegister.Address.Function = NextRegister->Register.Address.Function;
            MailboxRegister.Address.Register = NextRegister->Register.Address.Register;
            LibAmdPciRead (AccessWidth32, MailboxRegister, &Value, StdHeader);
            Value = ((Value & ~NextRegister->Mask) | (* (MAILBOX_REGISTER_SAVE_ENTRY) LocateParams.BufferPtr) [RegisterEntryIndex]);
            LibAmdPciWrite (AccessWidth32, MailboxRegister, &Value, StdHeader);
            RegisterEntryIndex++;
            NextRegister++;
          }
        }
      }
    }
    HeapStatus = HeapDeallocateBuffer (PRESERVE_MAIL_BOX_HANDLE, StdHeader);
  }
  return AGESA_SUCCESS;
}


CONST CPU_FEATURE_DESCRIPTOR ROMDATA CpuFeaturePreserveAroundMailbox =
{
  PreserveAroundMailbox,
  (CPU_FEAT_AFTER_COHERENT_DISCOVERY | CPU_FEAT_INIT_LATE_END | CPU_FEAT_AFTER_RESUME_MTRR_SYNC),
  IsPreserveAroundMailboxEnabled,
  PreserveMailboxes
};
