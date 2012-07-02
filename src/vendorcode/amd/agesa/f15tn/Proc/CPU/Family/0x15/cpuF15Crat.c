/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CRAT Record Creation API, and related functions for Family 15h.
 *
 * Contains code that produce the CRAT related information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Family/0x15
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "cpuRegisters.h"
#include "cpuFamilyTranslation.h"
#include "cpuLateInit.h"
#include "OptionCrat.h"
#include "cpuCrat.h"
#include "cpuServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FAMILY_0X15_CPUF15CRAT_FILECODE


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
extern CONST UINT8 ROMDATA L2L3Associativity[];

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  generate CRAT cache entry for F15 processor
 *
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
STATIC
F15GenerateCratCacheEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8     i;
  UINT8     NodeNum;
  UINT8     NodeCount;
  UINT8     CoreNumPerCU;
  UINT32    Socket;
  UINT32    Module;
  UINT32    CoreNum;
  UINT32    LowCore;
  UINT32    HighCore;
  UINT32    RegVal;
  CPUID_DATA L1CpuId;
  CPUID_DATA L2L3CpuId;
  CRAT_CACHE *EntryPtr;
  AMD_APIC_PARAMS ApicParams;
  PCI_ADDR  PciAddress;

  // Get Node count
  PciAddress.AddressValue = MAKE_SBDFO (0, 0, LOW_NODE_DEVICEID, FUNC_0, 0x60);
  LibAmdPciRead (AccessWidth32 , PciAddress, &RegVal, StdHeader);
  NodeCount = (UINT8) (((RegVal >> 4) & 0x7) + 1);

  // Get compute unit info
  switch (GetComputeUnitMapping (StdHeader)) {
  case AllCoresMapping:
    CoreNumPerCU = 1;
    break;
  case EvenCoresMapping:
    CoreNumPerCU = 2;
    break;
  default:
    CoreNumPerCU = 1;
  }
  // Get L1 L2 cache information from CPUID
  LibAmdCpuidRead (AMD_CPUID_TLB_L1Cache, &L1CpuId, StdHeader);
  LibAmdCpuidRead (AMD_CPUID_L2L3Cache_L2TLB, &L2L3CpuId, StdHeader);

  NodeNum = 0;
  ApicParams.StdHeader = *StdHeader;
  while (NodeNum < NodeCount) {
    GetSocketModuleOfNode ((UINT32) NodeNum, &Socket, &Module, StdHeader);
    GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader);

    for (CoreNum = LowCore; CoreNum <= HighCore; CoreNum++) {
      ApicParams.Socket = (UINT8) Socket;
      ApicParams.Core = (UINT8) CoreNum;
      AmdGetApicId (&ApicParams);
      if (ApicParams.IsPresent) {
        // L1 Data cache
        EntryPtr = (CRAT_CACHE *) AddOneCratEntry (CRAT_TYPE_CACHE, CratHeaderStructPtr, TableEnd, StdHeader);
        EntryPtr->Flags.Enabled = 1;
        EntryPtr->Flags.DataCache = 1;
        EntryPtr->Flags.CpuCache = 1;
        EntryPtr->ProcessorIdLow = ApicParams.ApicAddress;
        i = ApicParams.ApicAddress / 8;
        EntryPtr->SiblingMap[i] = 1 << (ApicParams.ApicAddress % 8);
        EntryPtr->CacheSize = L1CpuId.ECX_Reg >> 24; // bits[31:24] L1 data cache size
        EntryPtr->CacheLevel = 1;
        EntryPtr->LinesPerTag = (UINT8) ((L1CpuId.ECX_Reg >> 8) & 0xFF); // bits[15:8] L1 data cache lines per tag;
        EntryPtr->CacheLineSize = (UINT16) (L1CpuId.ECX_Reg & 0xFF); // bits[7:0] L1 data cache line size;
        EntryPtr->Associativity = (UINT8) ((L1CpuId.ECX_Reg >> 16) & 0xFF); // bits[23:16] L1 data cache associativity;
        /// @todo which value should set here?
        //EntryPtr->CacheProperties = ;
        EntryPtr->CacheLatency = 1;

        if (CoreNum % CoreNumPerCU == 0) {
          // L1 Instruction cache, shared by compute unit
          EntryPtr = (CRAT_CACHE *) AddOneCratEntry (CRAT_TYPE_CACHE, CratHeaderStructPtr, TableEnd, StdHeader);
          EntryPtr->Flags.Enabled = 1;
          EntryPtr->Flags.DataCache = 1;
          EntryPtr->Flags.CpuCache = 1;
          EntryPtr->ProcessorIdLow = ApicParams.ApicAddress;
          i = ApicParams.ApicAddress / 8;
          EntryPtr->SiblingMap[i] = 3 << (ApicParams.ApicAddress % 8);
          EntryPtr->CacheSize = L1CpuId.EDX_Reg >> 24; // bits[31:24] L1 instruction cache size
          EntryPtr->CacheLevel = 1;
          EntryPtr->LinesPerTag = (UINT8) ((L1CpuId.EDX_Reg >> 8) & 0xFF); // bits[15:8] L1 instruction cache lines per tag
          EntryPtr->CacheLineSize = (UINT16) (L1CpuId.EDX_Reg & 0xFF); // bits[7:0] L1 data instruction line size
          EntryPtr->Associativity = (UINT8) ((L1CpuId.EDX_Reg >> 16) & 0xFF); // bits[23:16] L1 instruction cache associativity
          /// @todo which value should be set here?
          //EntryPtr->CacheProperties = ;
          EntryPtr->CacheLatency = 1;

          // L2 cache, shared by compute unit
          EntryPtr = (CRAT_CACHE *) AddOneCratEntry (CRAT_TYPE_CACHE, CratHeaderStructPtr, TableEnd, StdHeader);
          EntryPtr->Flags.Enabled = 1;
          EntryPtr->Flags.CpuCache = 1;
          EntryPtr->ProcessorIdLow = ApicParams.ApicAddress;
          i = ApicParams.ApicAddress / 8;
          EntryPtr->SiblingMap[i] = 3 << (ApicParams.ApicAddress % 8);
          EntryPtr->CacheSize = L2L3CpuId.ECX_Reg >> 16; // bits[31:16] L2 cache size
          EntryPtr->CacheLevel = 2;
          EntryPtr->LinesPerTag = (UINT8) ((L2L3CpuId.ECX_Reg >> 8) & 0xF); // bits[11:8] L2 cache lines per tag
          EntryPtr->CacheLineSize = (UINT16) (L2L3CpuId.ECX_Reg & 0xFF); // bits[7:0] L2 cache line size
          EntryPtr->Associativity = L2L3Associativity[(L2L3CpuId.ECX_Reg >> 12) & 0xF]; // bits[15:12] L2 cache associativity
        }
        // L3 cache, shared by node
        // bits[31:18] L3 cache size
        if (((L2L3CpuId.EDX_Reg & 0xFFFC0000) != 0) && (CoreNum == 0)) {
          EntryPtr = (CRAT_CACHE *) AddOneCratEntry (CRAT_TYPE_CACHE, CratHeaderStructPtr, TableEnd, StdHeader);
          EntryPtr->Flags.Enabled = 1;
          EntryPtr->Flags.CpuCache = 1;
          EntryPtr->ProcessorIdLow = ApicParams.ApicAddress;
          i = ApicParams.ApicAddress / 8;
          EntryPtr->SiblingMap[i] = ((1 << (UINT8) (HighCore - LowCore + 1)) - 1) << (ApicParams.ApicAddress % 8);
          EntryPtr->CacheSize = (L2L3CpuId.EDX_Reg >> 18) * 512; // bits[31:18] L3 cache size
          EntryPtr->CacheLevel = 3;
          EntryPtr->LinesPerTag = (UINT8) ((L2L3CpuId.EDX_Reg >> 8) & 0xF); // bits[11:8] L3 cache lines per tag
          EntryPtr->CacheLineSize = (UINT16) (L2L3CpuId.EDX_Reg & 0xFF); // bits[7:0] L3 cache line size
          EntryPtr->Associativity = L2L3Associativity[(L2L3CpuId.EDX_Reg >> 12) & 0xF]; // bits[15:12] L3 cache associativity

        }
      }
    }

    NodeNum++;
  }

  return;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  generate CRAT TLB entry for F15 processor
 *
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
STATIC
F15GenerateCratTLBEntry (
  IN       CRAT_HEADER *CratHeaderStructPtr,
  IN OUT   UINT8       **TableEnd,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return;
}

CONST CRAT_FAMILY_SERVICES ROMDATA F15CratSupport =
{
    0,
    F15GenerateCratCacheEntry,
    F15GenerateCratTLBEntry
};

