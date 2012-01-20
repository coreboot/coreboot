/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Family_10 Family Specific Workaround table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x10
 * @e \$Revision: 57155 $   @e \$Date: 2011-07-28 02:27:47 -0600 (Thu, 28 Jul 2011) $
 *
 */
/*
 ******************************************************************************
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
#include "Table.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "Topology.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_CPU_FAMILY_0X10_CPUF10WORKAROUNDSTABLE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * A Family Specific Workaround method, to sync internal node 1 SbiAddr setting.
 *
 * @param[in] Data      The table data value (unused in this routine)
 * @param[in] StdHeader Config handle for library and services
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
STATIC
F10RevDSyncInternalNode1SbiAddr (
  IN       UINT32            Data,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32       Socket;
  UINT32       Module;
  UINT32       DataOr;
  UINT32       DataAnd;
  UINT32       ModuleType;
  PCI_ADDR     PciAddress;
  AGESA_STATUS AgesaStatus;
  UINT32       SyncToModule;
  AP_MAIL_INFO ApMailboxInfo;
  UINT32       LocalPciRegister;

  ApMailboxInfo.Info = 0;

  GetApMailbox (&ApMailboxInfo.Info, StdHeader);
  ASSERT (ApMailboxInfo.Fields.Socket < MAX_SOCKETS);
  ASSERT (ApMailboxInfo.Fields.Module < MAX_DIES);
  Socket = ApMailboxInfo.Fields.Socket;
  Module = ApMailboxInfo.Fields.Module;
  ModuleType = ApMailboxInfo.Fields.ModuleType;

  // sync is just needed on multinode cpu
  if (ModuleType != 0) {
    // check if it is internal node 0 of every socket
    if (Module == 0) {
      if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &AgesaStatus)) {
        PciAddress.Address.Function = FUNC_3;
        PciAddress.Address.Register = 0x1E4;
        // read internal node 0 F3x1E4[6:4]
        LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
        DataOr = LocalPciRegister & ((UINT32) (7 << 4));
        DataAnd = ~(UINT32) (7 << 4);
        for (SyncToModule = 1; SyncToModule < GetPlatformNumberOfModules (); SyncToModule++) {
          if (GetPciAddress (StdHeader, Socket, SyncToModule, &PciAddress, &AgesaStatus)) {
            PciAddress.Address.Function = FUNC_3;
            PciAddress.Address.Register = 0x1E4;
            // sync the other internal node F3x1E4[6:4]
            LibAmdPciRead (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
            LocalPciRegister &= DataAnd;
            LocalPciRegister |= DataOr;
            LibAmdPciWrite (AccessWidth32, PciAddress, &LocalPciRegister, StdHeader);
          }
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//  F a m i l y  S p e c i f i c  W o r k a r o u n d    T a b l e s
// -----------------------------------------------------------------

STATIC CONST FAM_SPECIFIC_WORKAROUND_TYPE_ENTRY_INITIALIZER ROMDATA F10Workarounds[] =
{
// F0x6C - Link Initialization Control Register
// Request for warm reset in AmdInitEarly
// [5, BiosRstDet] = 1b
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_10,                        // CpuFamily
      AMD_F10_ALL                           // CpuRevision
    },
	{AMD_PF_ALL},                             // platformFeatures
    {{
      SetWarmResetAtEarly,                  // function call
      0x00000000,                           // data
    }}
  },
  // Internal Node 1 SbiAddr sync for RevD
  {
    FamSpecificWorkaround,
    {
      AMD_FAMILY_10_HY,
      AMD_F10_HY_ALL
    },
    {AMD_PF_ALL},
    {{
      F10RevDSyncInternalNode1SbiAddr,
      0x00000000
    }}
  },
};

CONST REGISTER_TABLE ROMDATA F10WorkaroundsTable = {
  PrimaryCores,
  (sizeof (F10Workarounds) / sizeof (TABLE_ENTRY_FIELDS)),
  (TABLE_ENTRY_FIELDS *)F10Workarounds,
};
