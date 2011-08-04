/* $NoKeywords:$ */
/**
 * @file
 *
 * Implements the workaround for encrypted microcode patch loading.
 *
 * Returns the table of initialization steps to perform at
 * AmdInitEarly.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/FAMILY/0x14/ON
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
 *
 */
/*
 *****************************************************************************
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
 * 
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuEarlyInit.h"
#include "GnbRegistersON.h"
#include "F14OnInitEarlyTable.h"
#include "OptionFamily14hEarlySample.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FAMILY_0X14_ON_F14ONINITEARLYTABLE_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Field Data
#define D18F4x164_FixedErrata_0_OFFSET                          0
#define D18F4x164_FixedErrata_0_WIDTH                           1
#define D18F4x164_FixedErrata_0_MASK                            0x00000001
#define D18F4x164_Reserved_31_1_OFFSET                          1
#define D18F4x164_Reserved_31_1_WIDTH                           31
#define D18F4x164_Reserved_31_1_MASK                            0xFFFFFFFE

extern F14_ES_CORE_SUPPORT F14EarlySampleCoreSupport;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GetF14OnEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  );

VOID
F14OnLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
F14OnProductionErrataAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern F_PERFORM_EARLY_INIT_ON_CORE McaInitializationAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE SetRegistersFromTablesAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE SetBrandIdRegistersAtEarly;
extern F_PERFORM_EARLY_INIT_ON_CORE LocalApicInitializationAtEarly;

CONST S_PERFORM_EARLY_INIT_ON_CORE ROMDATA F14OnEarlyInitOnCoreTable[] =
{
  {McaInitializationAtEarly,       PERFORM_EARLY_ANY_CONDITION},
  {SetRegistersFromTablesAtEarly,  PERFORM_EARLY_ANY_CONDITION},
  {SetBrandIdRegistersAtEarly,     PERFORM_EARLY_ANY_CONDITION},
  {LocalApicInitializationAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F14OnLoadMicrocodePatchAtEarly, PERFORM_EARLY_ANY_CONDITION},
  {F14NbBufferAllocationAtEarly,   PERFORM_EARLY_ANY_CONDITION},
  {F14OnProductionErrataAtEarly,   PERFORM_EARLY_ANY_CONDITION},
  {NULL, 0}
};

/*------------------------------------------------------------------------------------*/
/**
 * Initializer routine that may be invoked at AmdCpuEarly to return the steps that a
 * processor that uses the standard initialization steps should take.
 *
 *  @CpuServiceMethod{::F_GET_EARLY_INIT_TABLE}.
 *
 * @param[in]       FamilyServices    The current Family Specific Services.
 * @param[out]      Table             Table of appropriate init steps for the executing core.
 * @param[in]       EarlyParams       Service Interface structure to initialize.
 * @param[in]       StdHeader         Opaque handle to standard config header.
 *
 */
VOID
GetF14OnEarlyInitOnCoreTable (
  IN       CPU_SPECIFIC_SERVICES                *FamilyServices,
     OUT   CONST S_PERFORM_EARLY_INIT_ON_CORE   **Table,
  IN       AMD_CPU_EARLY_PARAMS                 *EarlyParams,
  IN       AMD_CONFIG_PARAMS                    *StdHeader
  )
{
  *Table = F14OnEarlyInitOnCoreTable;

  F14EarlySampleCoreSupport.F14GetEarlyInitTableHook ((const VOID **)Table, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Update microcode patch in current processor for Family14h ON.
 *
 * This function acts as a wrapper for calling the LoadMicrocodePatch
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F14OnLoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT64 MsrValue;

  AGESA_TESTPOINT (TpProcCpuLoadUcode, StdHeader);
  // To load a microcode patch while using the cache as general storage,
  // the following steps are followed:
  // 1. Program MSRC001_102B[L2AllocDcFlushVictim]=1.
  // 2. Load the microcode patch.
  // 3. Program MSRC001_102B[L2AllocDcFlushVictim]=0.
  LibAmdMsrRead (MSR_BU_CFG3, &MsrValue, StdHeader);
  MsrValue = MsrValue | BIT7;
  LibAmdMsrWrite (MSR_BU_CFG3, &MsrValue, StdHeader);

  LoadMicrocodePatch (StdHeader);

  LibAmdMsrRead (MSR_BU_CFG3, &MsrValue, StdHeader);
  MsrValue = MsrValue & ~((UINT64)BIT7);
  LibAmdMsrWrite (MSR_BU_CFG3, &MsrValue, StdHeader);
}

/**
 * North bridge bufer allocation for Family14h ON.
 *
 * This function programs North bridge buffer allocation registers and provides
 * hook routine for override at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F14NbBufferAllocationAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  //Buffer allocations cannot be decreased through software, so move these register setting from register table
  //to here to make IDS easy override
  NB_BUFFER_ALLOCATION NbBufAllocation;
  PCI_ADDR PciAddr;
  AGESA_STATUS Ignored;

  if (IsBsp (StdHeader, &Ignored)) {
    PciAddr.AddressValue = MAKE_SBDFO (0, 0, 24, FUNC_3, D18F3x6C_ADDRESS);
    LibAmdPciRead (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x6C.Value, StdHeader);
    PciAddr.Address.Register = D18F3x74_ADDRESS;
    LibAmdPciRead (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x74.Value, StdHeader);
    PciAddr.Address.Register = D18F3x7C_ADDRESS;
    LibAmdPciRead (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x7C.Value, StdHeader);
    PciAddr.Address.Register = D18F3x17C_ADDRESS;
    LibAmdPciRead (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x17C.Value, StdHeader);
    //Recommend value for NB buffer allocation
    // D18F3x6C - Upstream Data Buffer Count
    // bits[3:0]   UpLoPreqDBC = 0x0E
    // bits[7:4]   UpLoNpReqDBC = 1
    // bits[11:8]  UpLoRespDBC = 1
    // bits[19:16] UpHiPreqDBC = 0
    // bits[23:20] UpHiNpReqDBC = 0
    NbBufAllocation.D18F3x6C.Field.UpLoPreqDBC = 0x0E;
    NbBufAllocation.D18F3x6C.Field.UpLoNpreqDBC = 1;
    NbBufAllocation.D18F3x6C.Field.UpLoRespDBC = 1;
    NbBufAllocation.D18F3x6C.Field.UpHiPreqDBC = 0;
    NbBufAllocation.D18F3x6C.Field.UpHiNpreqDBC = 0;

    // D18F3x74 - Upstream Command Buffer Count
    // bits[3:0]   UpLoPreqCBC = 7
    // bits[7:4]   UpLoNpreqCBC = 9
    // bits[11:8]  UpLoRespCBC = 8
    // bits[19:16] UpHiPreqCBC = 0
    // bits[23:20] UpHiNpreqCBC = 0
    NbBufAllocation.D18F3x74.Field.UpLoPreqCBC = 7;
    NbBufAllocation.D18F3x74.Field.UpLoNpreqCBC = 9;
    NbBufAllocation.D18F3x74.Field.UpLoRespCBC = 8;
    NbBufAllocation.D18F3x74.Field.UpHiPreqCBC = 0;
    NbBufAllocation.D18F3x74.Field.UpHiNpreqCBC = 0;

    // D18F3x7C - In-Flight Queue Buffer Allocation
    // bits[5:0]  CpuBC = 1
    // bits[13:8] LoPriPBC = 1
    // bits[21:16] LoPriNPBC = 1
    // bits[29:24] FreePoolBC = 0x19
    NbBufAllocation.D18F3x7C.Field.CpuBC = 1;
    NbBufAllocation.D18F3x7C.Field.LoPriPBC = 1;
    NbBufAllocation.D18F3x7C.Field.LoPriNPBC = 1;
    NbBufAllocation.D18F3x7C.Field.FreePoolBC = 0x19;

    // D18F3x17C - In-Flight Queue Extended Buffer Allocation
    // bits[5:0]  HiPriPBC = 0
    // bits[13:8] HiPriNPBC = 0
    NbBufAllocation.D18F3x17C.Field.HiPriPBC = 0;
    NbBufAllocation.D18F3x17C.Field.HiPriNPBC = 0;

    IDS_OPTION_HOOK (IDS_NBBUFFERALLOCATIONATEARLY, &NbBufAllocation, StdHeader);

    PciAddr.AddressValue = MAKE_SBDFO (0, 0, 24, FUNC_3, D18F3x6C_ADDRESS);
    LibAmdPciWrite (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x6C.Value, StdHeader);
    PciAddr.Address.Register = D18F3x74_ADDRESS;
    LibAmdPciWrite (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x74.Value, StdHeader);
    PciAddr.Address.Register = D18F3x7C_ADDRESS;
    LibAmdPciWrite (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x7C.Value, StdHeader);
    PciAddr.Address.Register = D18F3x17C_ADDRESS;
    LibAmdPciWrite (AccessWidth32, PciAddr, &NbBufAllocation.D18F3x17C.Value, StdHeader);
  }
}

/**
 * Production Erratum for Family14h ON.
 *
 * This function implements production errata for Family14h ON.
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
F14OnProductionErrataAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  CPU_LOGICAL_ID  LogicalId;
  PCI_ADDR        PciAddr;
  UINT32          PciData;
  UINT64          MsrValue;

  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  PciAddr.AddressValue = MAKE_SBDFO (0, 0, 24, FUNC_4, D18F4x164_ADDRESS);
  LibAmdPciRead (AccessWidth32, PciAddr, &PciData, StdHeader);
  if (((PciData & D18F4x164_FixedErrata_0_MASK) == 1) &&
      ((LogicalId.Revision & ~(AMD_F14_ON_Ax | AMD_F14_UNKNOWN)) != 0)) {
    // Program MSRC001_1020[18] = 1 only when D18F4x164[0] == 1 on ON B0 and later parts.
    LibAmdMsrRead (MSR_LS_CFG, &MsrValue, StdHeader);
    MsrValue = MsrValue | BIT18;
    LibAmdMsrWrite (MSR_LS_CFG, &MsrValue, StdHeader);
  }
}

