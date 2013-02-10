/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Microcode Patch Related Functions
 *
 * Contains code to program a microcode into the CPU
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
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

/*---------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *---------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuEarlyInit.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUMICROCODEPATCH_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
typedef union {
  UINT64           RawData;
  PATCH_LOADER_MSR BitFields;
} PATCH_LOADER;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
STATIC
LoadMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN
STATIC
GetPatchEquivalentId (
  IN OUT   UINT16             *ProcessorEquivalentId,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

BOOLEAN
STATIC
ValidateMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN       UINT16             ProcessorEquivalentId,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
STATIC
GetMicrocodeVersion (
     OUT   UINT32             *pMicrocodeVersion,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
LoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );


/* -----------------------------------------------------------------------------*/
/**
 *  Update microcode patch in current processor.
 *
 *  Then reads the patch id, and compare it to the expected, in the Microprocessor
 *  patch block.
 *
 *  @param[in] StdHeader   - Config handle for library and services.
 *
 *  @retval    TRUE   - Patch Loaded Successfully.
 *  @retval    FALSE  - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
LoadMicrocodePatch (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8    PatchNumber;
  UINT8    TotalPatches;
  UINT16   ProcessorEquivalentId;
  BOOLEAN  Status;
  MICROCODE_PATCH **MicrocodePatchPtr;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  Status = FALSE;

  if (IsCorePairPrimary (FirstCoreIsComputeUnitPrimary, StdHeader)) {
    // Get the patch pointer
    GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
    FamilySpecificServices->GetMicroCodePatchesStruct (FamilySpecificServices, (const VOID **) &MicrocodePatchPtr, &TotalPatches, StdHeader);

    IDS_OPTION_HOOK (IDS_UCODE, &TotalPatches, StdHeader);

    // Get the processor microcode path equivalent ID
    if (GetPatchEquivalentId (&ProcessorEquivalentId, StdHeader)) {
      // parse the patch table to see if we have one for the current cpu
      for (PatchNumber = 0; PatchNumber < TotalPatches; PatchNumber++) {
        if (ValidateMicrocode (MicrocodePatchPtr[PatchNumber], ProcessorEquivalentId, StdHeader)) {
          if (LoadMicrocode (MicrocodePatchPtr[PatchNumber], StdHeader)) {
            Status = TRUE;
          } else {
            PutEventLog (AGESA_ERROR,
                         CPU_ERROR_MICRO_CODE_PATCH_IS_NOT_LOADED,
                         0, 0, 0, 0, StdHeader);
          }
          break; // Once we find a microcode patch that matches the processor, exit the for loop
        }
      }
    }
  }
  return Status;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  LoadMicrocode
 *
 *    Update microcode patch in current processor, then reads the
 *    patch id, and compare it to the expected, in the Microprocessor
 *    patch block.
 *
 *    @param[in]       MicrocodePatchPtr  - Pointer to Microcode Patch.
 *    @param[in,out]   StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - Patch Loaded Successfully.
 *    @retval          FALSE - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
STATIC
LoadMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32       MicrocodeVersion;
  PATCH_LOADER PatchLoaderMsr;

  // Load microcode patch into CPU
  PatchLoaderMsr.RawData = (UINT64) (intptr_t) MicrocodePatchPtr;
  PatchLoaderMsr.BitFields.SBZ = 0;
  LibAmdMsrWrite (MSR_PATCH_LOADER, &PatchLoaderMsr.RawData, StdHeader);

  // Do ucode patch Authentication
  // Read microcode version back from CPU, determine if
  // it is the same patch level as contained in the source
  // microprocessor patch block passed in
  GetMicrocodeVersion (&MicrocodeVersion, StdHeader);
  if (MicrocodeVersion == MicrocodePatchPtr->PatchID) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  GetPatchEquivalentId
 *
 *    Return the equivalent ID for microcode patching
 *
 *    @param[in,out]   ProcessorEquivalentId   - Pointer to Processor Equivalent ID table.
 *    @param[in,out]   StdHeader               - Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - ID Found.
 *    @retval          FALSE - ID Not Found.
 *
 */
BOOLEAN
STATIC
GetPatchEquivalentId (
  IN OUT   UINT16             *ProcessorEquivalentId,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8        i;
  UINT8        EquivalencyEntries;
  UINT16       ProcessorRevisionId;
  UINT16       *MicrocodeEquivalenceTable;
  CPUID_DATA   CpuIdData;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  //
  // compute the processor revision ID
  //
  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuIdData, StdHeader);
  // high byte contains extended model and extended family
  ProcessorRevisionId  = (UINT16) ((CpuIdData.EAX_Reg & (CPU_EMODEL | CPU_EFAMILY)) >> 8);
  // low byte contains model and family
  ProcessorRevisionId |= (CpuIdData.EAX_Reg & (CPU_STEPPING | CPU_MODEL));

  //
  // find the equivalent ID for microcode purpose using the equivalence table
  //
  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  FamilySpecificServices->GetMicrocodeEquivalenceTable (FamilySpecificServices,
                                                        (const VOID **)&MicrocodeEquivalenceTable,
                                                        &EquivalencyEntries,
                                                        StdHeader);

  // parse the equivalence table
  for (i = 0; i < (EquivalencyEntries * 2); i += 2) {
    // check for equivalence
    if (ProcessorRevisionId == MicrocodeEquivalenceTable[i]) {
      *ProcessorEquivalentId = MicrocodeEquivalenceTable[i + 1];
      return (TRUE);
    }
  }
  // end of table reach, this processor is not supported
  *ProcessorEquivalentId = 0x0000;
  return (FALSE);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 *  ValidateMicrocode
 *
 *    Determine if the microcode patch block, currently pointed to
 *    is valid, and is appropriate for the current processor

 *    @param[in]       MicrocodePatchPtr      - Pointer to Microcode Patch.
 *    @param[in]       ProcessorEquivalentId  - Pointer to Processor Equivalent ID table.
 *    @param[in,out]   StdHeader              - Pointer to AMD_CONFIG_PARAMS struct.
 *
 *    @retval          TRUE  - Patch Found.
 *    @retval          FALSE - Patch Not Found.
 *
 */
BOOLEAN
STATIC
ValidateMicrocode (
  IN       MICROCODE_PATCH    *MicrocodePatchPtr,
  IN       UINT16             ProcessorEquivalentId,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  BOOLEAN   Chipset1Matched;
  BOOLEAN   Chipset2Matched;
  PCI_ADDR  PciAddress;
  UINT32    PciDeviceVidDid;
  UINT8     PciDeviceRevision;
  UINT8     DevCount;
  UINT8     FunCount;
  UINT32    Chipset1DeviceID;
  UINT32    Chipset2DeviceID;
  UINT8     MulitFunction;

  Chipset1Matched = FALSE;
  Chipset2Matched = FALSE;
  PciDeviceVidDid = 0;
  PciDeviceRevision = 0;
  Chipset1DeviceID = MicrocodePatchPtr->Chipset1DeviceID;
  Chipset2DeviceID = MicrocodePatchPtr->Chipset2DeviceID;
  MulitFunction = 0;

  //
  // parse the supplied microcode to see if it is compatible with the processor
  //
  if (MicrocodePatchPtr->ProcessorRevisionID !=  ProcessorEquivalentId) {
    return (FALSE);
  }

  if (Chipset1DeviceID == 0) {
    Chipset1Matched = TRUE;
  }
  if (Chipset2DeviceID == 0) {
    Chipset2Matched = TRUE;
  }

  if ((!Chipset1Matched) || (!Chipset2Matched)) {
    //
    // Scan all PCI devices in Bus 0, try to find out matched case.
    //
    for (DevCount = 0; DevCount < 32; DevCount++) {
      for (FunCount = 0; FunCount < 8; FunCount++) {
        PciAddress.AddressValue = MAKE_SBDFO (0, 0, DevCount, FunCount, 0);
        LibAmdPciRead (AccessWidth32, PciAddress, &PciDeviceVidDid, StdHeader);
        if (PciDeviceVidDid == 0xFFFFFFFF) {
          if (FunCount == 0) {
            break;
          } else {
            continue;
          }
        }
        PciAddress.Address.Register = 0x8;
        LibAmdPciRead (AccessWidth8, PciAddress, &PciDeviceRevision, StdHeader);
        if ((!Chipset1Matched) && (PciDeviceVidDid == Chipset1DeviceID)) {
          if (PciDeviceRevision == MicrocodePatchPtr->Chipset1RevisionID) {
            Chipset1Matched = TRUE;
          }
        }
        if ((!Chipset2Matched) && (PciDeviceVidDid == Chipset2DeviceID)) {
          if (PciDeviceRevision == MicrocodePatchPtr->Chipset2RevisionID) {
            Chipset2Matched = TRUE;
          }
        }
        if (Chipset1Matched && Chipset2Matched) {
          break;
        }
        //
        // Check multi-function. If it doesen't exist, we don't have to loop functions to 7.
        //
        if (FunCount == 0) {
          MulitFunction = 0;
          PciAddress.Address.Register = 0xE;
          LibAmdPciRead (AccessWidth8, PciAddress, &MulitFunction, StdHeader);
          if ((MulitFunction & 0x80) == 0) {
            break;
          }
        }
      } // end FunCount for loop.

      if (Chipset1Matched && Chipset2Matched) {
        break;
      }
    }  // end DevCount for loop.
  }

  return (Chipset1Matched && Chipset2Matched);
}


/*---------------------------------------------------------------------------------------*/
/**
 *
 *  GetMicrocodeVersion
 *
 *    Return the version of the currently loaded microcode patch, if any.
 *    Read from the patch level MSR, return the value in eax. If no patch
 *    has been loaded, 0 will be returned.
 *
 *    @param[out]      pMicrocodeVersion  - Pointer to Microcode Version.
 *    @param[in,out]   StdHeader          - Pointer to AMD_CONFIG_PARAMS struct.
 *
 */
VOID
STATIC
GetMicrocodeVersion (
     OUT   UINT32             *pMicrocodeVersion,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT64  MsrData;

  MsrData = 0;
  LibAmdMsrRead (MSR_PATCH_LEVEL, &MsrData, StdHeader);

  *pMicrocodeVersion = (UINT32) MsrData;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Update microcode patch in current processor.
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
LoadMicrocodePatchAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_TESTPOINT (TpProcCpuLoadUcode, StdHeader);
  LoadMicrocodePatch (StdHeader);
}
