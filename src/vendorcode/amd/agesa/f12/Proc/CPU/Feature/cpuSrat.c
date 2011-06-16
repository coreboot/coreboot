/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD SRAT, ACPI table related API functions.
 *
 * Contains code that Create the APCI SRAT Table after early reset.
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
 ***************************************************************************/


/*----------------------------------------------------------------------------
 * This file provides functions, that will generate SRAT tables
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "OptionSrat.h"
#include "heapManager.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuLateInit.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUSRAT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_SRAT_CONFIGURATION OptionSratConfiguration;  // global user config record

#define NodeID 0x60
#define FOURGB 0x010000

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *  All of the DATA should be defined in _CODE segment.
 *  Use ROMDATA to specify that it belongs to _CODE.
 *----------------------------------------------------------------------------
 */
STATIC CPU_SRAT_HEADER  ROMDATA CpuSratHdrStruct =
{
  {'S','R','A','T'},
  0,
  2,
  0,
  {'A','M','D',' ',' ',' '},
  {'A','G','E','S','A',' ',' ',' '},
  1,
  {'A','M','D',' '},
  1,
  1,
  {0, 0, 0, 0, 0, 0, 0, 0}
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT8
STATIC
*MakeApicEntry (
  IN UINT8 ApicId,
  IN UINT8 Domain,
  IN UINT8 *BufferLocPtr
  );

UINT8
STATIC
*FillMemoryForCurrentNode (
  IN      UINT8 *PDomain,
  IN OUT  UINT8 *PDomainForBase640K,
  IN      UINT8 Node,
  IN OUT  UINT8 *BufferLocPtr,
  IN OUT  AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
STATIC
*MakeMemEntry (
  IN UINT8  PDomain,
  IN UINT8  Node,
  IN UINT32 Base,
  IN UINT32 Size,
  IN UINT8  *BufferLocPtr
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GetAcpiSratStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   VOID                  **SratPtr
  );

AGESA_STATUS
GetAcpiSratMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   VOID                  **SratPtr
  );

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will generate a complete Static Resource Affinity Table
 * i.e. SRAT into a memory buffer. After completion, this table must be set
 * by the system BIOS into its internal ACPI name space.
 *
 *    @param[in, out] StdHeader         Standard Head Pointer
 *    @param[in, out] SratPtr           Point to Srat Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
CreateAcpiSrat (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   VOID                  **SratPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntrySrat, StdHeader);
  return ((*(OptionSratConfiguration.SratFeature)) (StdHeader, SratPtr));
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This is the default routine for use when the SRAT option is NOT requested.
 *
 * The option install process will create and fill the transfer vector with
 * the address of the proper routine (Main or Stub). The link optimizer will
 * strip out of the .DLL the routine that is not used.
 *
 *    @param[in, out] StdHeader      Standard Head Pointer
 *    @param[in, out] SratPtr           Point to Srat Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
GetAcpiSratStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   VOID                  **SratPtr
  )
{
  return  AGESA_UNSUPPORTED;
}
/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will generate a complete Static Resource Affinity Table
 * i.e. SRAT into a memory buffer. After completion, this table must be set
 * by the system BIOS into its internal ACPI name space.
 *
 *    @param[in, out] StdHeader         Standard Head Pointer
 *    @param[in, out] SratPtr           Point to Srat Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */
AGESA_STATUS
GetAcpiSratMain (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader,
  IN OUT   VOID                  **SratPtr
  )
{
  UINT8     *BufferPtr;
  UINT8     NodeNum;
  UINT8     NodeCount;
  UINT8     PDomain;
  UINT8     PDomainForBase640K;
  UINT32    Socket;
  UINT32    Module;
  UINT32    LowCore;
  UINT32    HighCore;
  UINT32    CoreNum;
  UINT32    RegVal;
  UINT32    tempVar_32;
  AMD_APIC_PARAMS ApicParams;
  PCI_ADDR  PciAddress;
  CPU_SRAT_HEADER  *CpuSratHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;

  // Get Node count
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, LOW_NODE_DEVICEID, FUNC_0, NodeID);
  LibAmdPciRead (AccessWidth32 , PciAddress, &RegVal, StdHeader);
  NodeCount = (UINT8) (((RegVal >> 4) & 0x7) + 1);

  // The worst-case buffer size to request is for the SRAT table header, one
  // entree for special region (base 640k block), two memory
  // regions per node, and APIC entries for each core in the system.
  tempVar_32 = (sizeof (CPU_SRAT_HEADER)) + (sizeof (CPU_SRAT_MEMORY_ENTRY))
               + ((UINT32) NodeCount * (2 * (sizeof (CPU_SRAT_MEMORY_ENTRY))
               + ((UINT32) GetActiveCoresInCurrentModule (StdHeader) * sizeof (CPU_SRAT_APIC_ENTRY))));

  if (*SratPtr == NULL) {
    //
    // Allocate a buffer
    //
    AllocParams.RequestedBufferSize = tempVar_32;
    AllocParams.BufferHandle = AMD_SRAT_INFO_BUFFER_HANDLE;
    AllocParams.Persist = HEAP_SYSTEM_MEM;

    AGESA_TESTPOINT (TpProcCpuBeforeAllocateSratBuffer, StdHeader);
    if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
      return AGESA_ERROR;
    }
    AGESA_TESTPOINT (TpProcCpuAfterAllocateSratBuffer, StdHeader);

    *SratPtr = AllocParams.BufferPtr;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  SRAT is created\n");

  CpuSratHeaderStructPtr = (CPU_SRAT_HEADER *) *SratPtr;
  BufferPtr = (UINT8 *) *SratPtr;

  // Copy acpiSRATHeader -> data buffer
  LibAmdMemCopy (*SratPtr, (VOID *) &CpuSratHdrStruct, (UINTN) (sizeof (CPU_SRAT_HEADER)), StdHeader);

  BufferPtr += sizeof (CPU_SRAT_HEADER);

  // Place all memory and IO affinity entries
  NodeNum = 0;
  PDomain = 0;
  PDomainForBase640K = 0xFF;
  ApicParams.StdHeader = *StdHeader;
  while (NodeNum < NodeCount) {
    GetSocketModuleOfNode ((UINT32) NodeNum, &Socket, &Module, StdHeader);
    GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader);
    BufferPtr = FillMemoryForCurrentNode (&PDomain, &PDomainForBase640K, NodeNum, BufferPtr, StdHeader);
    for (CoreNum = LowCore; CoreNum <= HighCore; CoreNum++) {
      ApicParams.Socket = (UINT8) Socket;
      ApicParams.Core = (UINT8) CoreNum;
      AmdGetApicId (&ApicParams);
      if (ApicParams.IsPresent) {
        BufferPtr = MakeApicEntry (ApicParams.ApicAddress, PDomain, BufferPtr);
      }
    }

    NodeNum++;
    PDomain = NodeNum;
  }

  // Store size in table (current buffer offset - buffer start offset)
  CpuSratHeaderStructPtr->TableLength = (UINT32) (BufferPtr - (UINT8 *) CpuSratHeaderStructPtr);

  //Update SSDT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) CpuSratHeaderStructPtr, StdHeader);

  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------
 *                            L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will build Memory entry for current node.
 * Note that we only create a memory affinity entry if we find one
 * that matches the current node.  This makes an easier to read table
 * though it is not necessary.
 *
 *    @param[in]      PDomain            Proximity Domain
 *    @param[in, out] PDomainForBase640K The PDomain for Base 640K
 *    @param[in]      Node               The number of Node
 *    @param[in, out] BufferLocPtr       Point to the address of buffer
 *    @param[in, out] StdHeader          Standard Head Pointer
 *
 *    @retval         UINT8 *(New buffer location ptr)
 */
UINT8
STATIC
*FillMemoryForCurrentNode (
  IN      UINT8 *PDomain,
  IN OUT  UINT8 *PDomainForBase640K,
  IN      UINT8 Node,
  IN OUT  UINT8 *BufferLocPtr,
  IN OUT  AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32    ValueLimit;
  UINT32    ValueTOM;
  BOOLEAN   isModified;
  UINT8     Domain;
  UINT32    RegVal;
  UINT32    DramLeng;
  UINT32    DramBase;
  UINT32    DramLimit;
  UINT32    OffsetRegs;
  PCI_ADDR  PciAddress;
  UINT64    MsrValue;
  UINT32    TopOfMemoryAbove4Gb;

  Domain = *PDomain;

  PciAddress.Address.Segment = 0;
  PciAddress.Address.Bus = 0;
  PciAddress.Address.Device = LOW_NODE_DEVICEID;
  PciAddress.Address.Function = FUNC_1;

  for (OffsetRegs = DRAMBase0; OffsetRegs < MMIOBase0; OffsetRegs += 8) {
    isModified = FALSE;        // FALSE means normal update procedure
    // Get DRAM Base Address
    PciAddress.Address.Register = OffsetRegs;
    LibAmdPciRead (AccessWidth32, PciAddress, &DramBase, StdHeader);
    if ((DramBase & 3) != 3) {
      // 0:1 set if memory range enabled
      // Not set, so we don't have an enabled range
      continue;   // Proceed to next Base register
    }

    // Get DRAM Limit
    PciAddress.Address.Register = OffsetRegs + 4;
    LibAmdPciRead (AccessWidth32, PciAddress, &DramLimit, StdHeader);
    if (DramLimit == 0xFFFFFFFF) {
      // Node not installed(all FF's)?
      continue;   // Proceed to next Base register
    }

    if ((DramLimit & 0xFF) != Node) {
      // Check if Destination Node ID is current node
      continue;   // Proceed to next Base register
    }

    //    We only add an entry now if detected range belongs to current node/PDomain
    PciAddress.Address.Register = OffsetRegs + 0x104;
    LibAmdPciRead (AccessWidth32, PciAddress, &RegVal, StdHeader);

    DramLimit = (((RegVal & 0xFF) << 16) | (DramLimit >> 16));  // Get DRAM Limit addr [47:24]
    DramLimit++;                     // Add 1 for potential length
    DramLimit <<= 8;

    // Get DRAM Base Address
    PciAddress.Address.Register = OffsetRegs + 0x100;
    LibAmdPciRead (AccessWidth32, PciAddress, &RegVal, StdHeader);
    DramBase = ((((RegVal & 0xFF) << 24) | (DramBase >> 8)) & 0xFFFFFF00);   // Get DRAM Base Base value [47:24]
    DramLeng = DramLimit - DramBase;    // Subtract base from limit to get length

    //    Leave hole for conventional memory (Less than 640K).  It must be on CPU 0.
    if (DramBase == 0) {
      if (*PDomainForBase640K == 0xFF) {
        // It is the first time that the range start at 0.
        // If Yes, then Place 1MB memory gap and save Domain to PDomainForBase640K
        BufferLocPtr = MakeMemEntry (
                         Domain,
                         Node,
                         0,              // Base = 0
                         0xA0000 >> 16,  // Put it into format used in DRAM regs..
                         BufferLocPtr
                         );
        DramBase += 0x10;       // Add 1MB, so range = 1MB to Top of Region
        DramLeng -= 0x10;       // Also subtract 1MB from the length
        *PDomainForBase640K = Domain;        // Save Domain number for memory Less than 640K
      } else {
        // If No, there are more than one memory range less than 640K, it should that
        // node interleaving is enabled. All nodes have the same memory ranges
        // and all cores in these nodes belong to the same domain.
        *PDomain = *PDomainForBase640K;
        return (BufferLocPtr);
      }
    }
    LibAmdMsrRead (TOP_MEM, &MsrValue, StdHeader);
    ValueTOM   = (UINT32) MsrValue >> 16;    // Save it in 39:24 format
    ValueLimit = DramBase + DramLeng;        // We need to know how large region is

    LibAmdMsrRead (SYS_CFG, &MsrValue, StdHeader);
    if ((MsrValue & BIT21) != 0) {
      LibAmdMsrRead (TOP_MEM2, &MsrValue, StdHeader);
      TopOfMemoryAbove4Gb = (UINT32) (MsrValue >> 16);    // Save it in 47:16 format
    } else {
      TopOfMemoryAbove4Gb = 0xFFFFFFFF;
    }

    //  SPECIAL CASES:
    //
    //  Several conditions require that we process the values of the memory range differently.
    //  Here are descriptions of the corner cases.
    //
    //  1. TRUNCATE LOW - Memory range starts below TOM, ends in TOM (memory hole).  For this case,
    //     the range must be truncated to end at TOM.
    //  *******************************            *******************************
    //  *                    *        *      ->    *                             *
    //  *******************************            *******************************
    //  2                    TOM      4            2                             TOM
    //
    //  2. TRUNCATE HIGH - Memory range starts below 4GB, ends above 4GB.  This is handled by changing the
    //     start base to 4GB.
    //          ****************                        **********
    //          *     *        *      ->                *        *
    //          ****************                        **********
    //  TOM     3.8   4        6            TOM   3.8   4        6
    //
    //  3. Memory range starts below TOM, ends above 4GB.  For this case, the range must be truncated
    //     to end at TOM.  Note that this scenario creates two ranges, as the second comparison below
    //     will find that it ends above 4GB since base and limit have been restored after first truncation,
    //     and a second range will be written based at 4GB ending at original end address.
    //  *******************************            ****************          **********
    //  *              *     *        *      ->    *              *          *        *
    //  *******************************            ****************          **********
    //  2              TOM   4        6            2              TOM        4        6
    //
    //  4. Memory range starts above TOM, ends below or equal to 4GB.  This invalid range should simply
    //     be ignored.
    //          *******
    //          *     *      ->    < NULL >
    //          *******
    //  TOM     3.8   4
    //
    //  5. Memory range starts below TOM2, and ends beyond TOM2.  This range must be truncated to TOM2.
    //  ************************         *******************************
    //  *              *       *   ->    *                             *
    //  ************************         *******************************
    //  768          TOM2   1024         768                        TOM2
    //
    //  6. Memory range starts above TOM2.  This invalid range should simply be ignored.
    //          ********************
    //          *                  *    ->    < NULL >
    //          ********************
    //  TOM2    1024            1280

    if (((DramBase < ValueTOM) && (ValueLimit <= FOURGB) && (ValueLimit > ValueTOM))
        || ((DramBase < ValueTOM) && (ValueLimit > FOURGB))) {
      //   TRUNCATE LOW!!! Shrink entry below TOM...
      //   Base = DramBase, Size = TOM - DramBase
      BufferLocPtr = MakeMemEntry (Domain, Node, DramBase, (ValueTOM - DramBase), BufferLocPtr);
      isModified = TRUE;
    }

    if ((ValueLimit > FOURGB) && (DramBase < FOURGB)) {
      //   TRUNCATE HIGH!!! Shrink entry above 4GB...
      //   Size = Base + Size - 4GB, Base = 4GB
      BufferLocPtr = MakeMemEntry (Domain, Node, FOURGB, (DramLeng + DramBase - FOURGB), BufferLocPtr);
      isModified = TRUE;
    }

    if ((DramBase >= ValueTOM) && (ValueLimit <= FOURGB)) {
      //   IGNORE!!!  Entry located entirely within memory hole
      isModified = TRUE;
    }

    if ((DramBase < TopOfMemoryAbove4Gb) && (ValueLimit > TopOfMemoryAbove4Gb)) {
      //   Truncate to TOM2
      //   Base = DramBase, Size = TOM2 - DramBase
      BufferLocPtr = MakeMemEntry (Domain, Node, DramBase, (TopOfMemoryAbove4Gb - DramBase), BufferLocPtr);
      isModified = TRUE;
    }

    if (DramBase >= TopOfMemoryAbove4Gb) {
      //   IGNORE!!!  Entry located entirely above TOM2
      isModified = TRUE;
    }

    //    If special range(isModified), we are done.
    //    If not, finally write the memory entry.
    if (isModified == FALSE) {
      // Finally write the memory entry.
      BufferLocPtr = MakeMemEntry (Domain, Node, DramBase, DramLeng, BufferLocPtr);
    }

  } // for ( OffsetRegs = DRAMBase0; ... )

  return (BufferLocPtr);
} // FillMemoryForCurrentNode()


/*---------------------------------------------------------------------------------------*/
/**
 * This function will add APIC entry.
 *
 *    @param[in]      ApicId            APIC ID number
 *    @param[in]      Domain            Domain number
 *    @param[in]      BufferLocPtr      Point to the address of buffer
 *
 *    @retval         UINT8 *(New buffer location ptr)
 */
UINT8
STATIC
*MakeApicEntry (
  IN      UINT8 ApicId,
  IN      UINT8 Domain,
  IN      UINT8 *BufferLocPtr
  )
{
  CPU_SRAT_APIC_ENTRY *psSratApicEntry;
  UINT8 ReservedBytes;

  psSratApicEntry = (CPU_SRAT_APIC_ENTRY *)BufferLocPtr;

  psSratApicEntry->Type = AE_APIC;
  psSratApicEntry->Length = (UINT8)sizeof (CPU_SRAT_APIC_ENTRY);
  psSratApicEntry->Domain = Domain;
  psSratApicEntry->ApicId = ApicId;
  psSratApicEntry->Flags = ENABLED;
  psSratApicEntry->LSApicEid = 0;
  for (ReservedBytes = 0; ReservedBytes < (UINT8)sizeof (psSratApicEntry->Reserved); ReservedBytes++) {
    psSratApicEntry->Reserved[ReservedBytes] = 0;
  }
  return (BufferLocPtr + (UINT8)sizeof (CPU_SRAT_APIC_ENTRY));
} // MakeApicEntry


/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will add Memory entry.
 *
 *  Parameters:
 *    @param[in]  PDomain               Proximity Domain
 *    @param[in]  Node                  The number of Node
 *    @param[in]  Base                  Memory Base
 *    @param[in]  Size                  Memory Size
 *    @param[in]  BufferLocPtr          Point to the address of buffer
 *
 *    @retval       UINT8 * (new buffer location ptr)
 */
UINT8
STATIC
*MakeMemEntry (
  IN  UINT8   PDomain,
  IN  UINT8   Node,
  IN  UINT32  Base,
  IN  UINT32  Size,
  IN  UINT8   *BufferLocPtr
  )
{
  CPU_SRAT_MEMORY_ENTRY *psSratMemEntry;
  UINT8 ReservedBytes;

  psSratMemEntry = (CPU_SRAT_MEMORY_ENTRY *)BufferLocPtr;

  psSratMemEntry->Type = AE_MEMORY;                       // [0] = Memory Entry
  psSratMemEntry->Length = (UINT8)sizeof (CPU_SRAT_MEMORY_ENTRY); // [1] = 40
  psSratMemEntry->Domain = PDomain;                            // [2] = Proximity Domain

  // [6-7] = Reserved
  for (ReservedBytes = 0; ReservedBytes < (UINT8)sizeof (psSratMemEntry->Reserved1); ReservedBytes++) {
    psSratMemEntry->Reserved1[ReservedBytes] = 0;
  }

  // [8-11] = Keep 31:0 of address only -> Base Addr Low
  psSratMemEntry->BaseAddrLow  = Base << 16;

  // [12-15] = Keep 39:32 of address only -> Base Addr High
  psSratMemEntry->BaseAddrHigh = Base >> 16;

  // [16-19] = Keep 31:0 of address only -> Length Low
  psSratMemEntry->LengthAddrLow  = Size << 16;

  // [20-23] = Keep 39:32 of address only -> Length High
  psSratMemEntry->LengthAddrHigh = Size >> 16;

  // [24-27] = Reserved
  for (ReservedBytes = 0; ReservedBytes < (UINT8)sizeof (psSratMemEntry->Reserved2); ReservedBytes++) {
    psSratMemEntry->Reserved2[ReservedBytes] = 0;
  }

  // [28-31] = Flags
  psSratMemEntry->Flags = ENABLED;

  // [32-40] = Reserved
  for (ReservedBytes = 0; ReservedBytes < (UINT8)sizeof (psSratMemEntry->Reserved3); ReservedBytes++) {
    psSratMemEntry->Reserved3[ReservedBytes] = 0;
  }
  return (BufferLocPtr + (UINT8)sizeof (CPU_SRAT_MEMORY_ENTRY));
} // MakeMemEntry()

