/* $NoKeywords:$ */
/**
 * @file
 *
 * mfidendimm.c
 *
 * Translate physical system address to dimm identification.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
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
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "heapManager.h"
#include "mfidendimm.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_IDENDIMM_MFIDENDIMM_FILECODE
extern MEM_NB_SUPPORT memNBInstalled[];

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DCTS_PER_DIE        2   ///< Max DCTs per die
#define MAX_CHLS_PER_DCT        1   ///< Max Channels per DCT

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
STATIC
MemFTransSysAddrToCS (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify,
  IN       MEM_MAIN_DATA_BLOCK *mmPtr
  );

UINT32
STATIC
MemFGetPCI (
  IN   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 NodeID,
  IN   UINT8 DctNum,
  IN   BIT_FIELD_NAME BitFieldName
  );

UINT8
STATIC
MemFUnaryXOR (
  IN   UINT32 address
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*-----------------------------------------------------------------------------*/
/**
*
*   This function identifies the dimm on which the given memory address locates.
*
*   @param[in, out]   *AmdDimmIdentify - Pointer to the parameter structure AMD_IDENTIFY_DIMM
*
*   @retval           AGESA_SUCCESS - Successfully translate physical system address
*                                     to dimm identification.
*                     AGESA_BOUNDS_CHK - Targeted address is out of bound.
*
*/

AGESA_STATUS
AmdIdentifyDimm (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify
  )
{
  UINT8 i;
  AGESA_STATUS RetVal;
  MEM_MAIN_DATA_BLOCK mmData;             // Main Data block
  MEM_NB_BLOCK *NBPtr;
  MEM_DATA_STRUCT MemData;
  LOCATE_HEAP_PTR LocHeap;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT8 Node;
  UINT8 Dct;
  UINT8 Die;
  UINT8 DieCount;

  LibAmdMemCopy (&(MemData.StdHeader), &(AmdDimmIdentify->StdHeader), sizeof (AMD_CONFIG_PARAMS), &(AmdDimmIdentify->StdHeader));
  mmData.MemPtr = &MemData;
  RetVal = MemSocketScan (&mmData);
  if (RetVal == AGESA_FATAL) {
    return RetVal;
  }
  DieCount = mmData.DieCount;

  // Search for AMD_MEM_AUTO_HANDLE on the heap first.
  // Only apply for space on the heap if cannot find AMD_MEM_AUTO_HANDLE on the heap.
  LocHeap.BufferHandle = AMD_MEM_AUTO_HANDLE;
  if (HeapLocateBuffer (&LocHeap, &AmdDimmIdentify->StdHeader) == AGESA_SUCCESS) {
    // NB block has already been constructed by main block.
    // No need to construct it here.
    NBPtr = (MEM_NB_BLOCK *)LocHeap.BufferPtr;
    mmData.NBPtr = NBPtr;
  } else {
    AllocHeapParams.RequestedBufferSize = (DieCount * (sizeof (MEM_NB_BLOCK)));
    AllocHeapParams.BufferHandle = AMD_MEM_AUTO_HANDLE;
    AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
    if (HeapAllocateBuffer (&AllocHeapParams, &AmdDimmIdentify->StdHeader) != AGESA_SUCCESS) {
      PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_IDENTIFY_DIMM_MEM_NB_BLOCK, 0, 0, 0, 0, &AmdDimmIdentify->StdHeader);
      ASSERT(FALSE); // Could not allocate heap space for NB block for Identify DIMM
      return AGESA_FATAL;
    }
    NBPtr = (MEM_NB_BLOCK *)AllocHeapParams.BufferPtr;
    mmData.NBPtr = NBPtr;
    // Construct each die.
    for (Die = 0; Die < DieCount; Die ++) {
      i = 0;
      while (memNBInstalled[i].MemIdentifyDimmConstruct != 0) {
        if (memNBInstalled[i].MemIdentifyDimmConstruct (&NBPtr[Die], &MemData, Die)) {
          break;
        }
        i++;
      };
      if (memNBInstalled[i].MemIdentifyDimmConstruct == 0) {
        PutEventLog (AGESA_FATAL, MEM_ERROR_NO_CONSTRUCTOR_FOR_IDENTIFY_DIMM, Die, 0, 0, 0, &AmdDimmIdentify->StdHeader);
        ASSERT(FALSE); // No Identify DIMM constructor found
        return AGESA_FATAL;
      }
    }
  }

  if ((RetVal = MemFTransSysAddrToCS (AmdDimmIdentify, &mmData)) == AGESA_SUCCESS) {
    // Translate Node, DCT and Chip select number to Socket, Channel and Dimm number.
    Node = AmdDimmIdentify->SocketId;
    Dct = AmdDimmIdentify->MemChannelId;
    AmdDimmIdentify->SocketId = MemData.DiesPerSystem[Node].SocketId;
    AmdDimmIdentify->MemChannelId = NBPtr[Node].GetSocketRelativeChannel (&NBPtr[Node], Dct, 0);
    AmdDimmIdentify->DimmId /= 2;
  }

  return RetVal;
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------*/
/**
*
*   This function translates the given physical system address to
*   a node, channel select, chip select, bank, row, and column address.
*
*   @param[in, out]   *AmdDimmIdentify - Pointer to the parameter structure AMD_IDENTIFY_DIMM
*   @param[in, out]   *mmPtr - Pointer to the MEM_MAIN_DATA_BLOCK
*
*   @retval           AGESA_SUCCESS - The chip select address is found
*   @retval           AGESA_BOUNDS_CHK - Targeted address is out of bound.
*
*/
AGESA_STATUS
STATIC
MemFTransSysAddrToCS (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify,
  IN       MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  BOOLEAN CSFound;
  BOOLEAN DctSelHiRngEn;
  BOOLEAN DctSelIntLvEn;
  BOOLEAN DctGangEn;
  BOOLEAN HiRangeSelected;
  BOOLEAN DramHoleValid;
  BOOLEAN CSEn;
  BOOLEAN SwapDone;
  BOOLEAN IntLvRgnSwapEn;
  UINT8 DctSelHi;
  UINT8 DramEn;
  UINT8 range;
  UINT8 IntlvEn;
  UINT8 IntlvSel;
  UINT8 ILog;
  UINT8 DctSelIntLvAddr;
  UINT8 DctNum;
  UINT8 cs;
  UINT8 BadDramCs;
  UINT8 spare;
  UINT8 IntLvRgnBaseAddr;
  UINT8 IntLvRgnLmtAddr;
  UINT8 IntLvRgnSize;
  UINT32 temp;
  UINT32 DramHoleOffset;
  UINT32 DramHoleBase;
  UINT64 DramBase;
  UINT64 DramLimit;
  UINT64 DramLimitSysAddr;
  UINT64 DctSelBaseAddr;
  UINT64 DctSelBaseOffset;
  UINT64 ChannelAddr;
  UINT64 CSBase;
  UINT64 CSMask;
  UINT64 InputAddr;
  UINT64 ChannelOffset;
  MEM_NB_BLOCK *NBPtr;
  UINT8 Die;

  UINT64 SysAddr;
  UINT8 *NodeID;
  UINT8 *ChannelSelect;
  UINT8 *ChipSelect;

  SysAddr = AmdDimmIdentify->MemoryAddress;
  NodeID = &(AmdDimmIdentify->SocketId);
  ChannelSelect = &(AmdDimmIdentify->MemChannelId);
  ChipSelect = &(AmdDimmIdentify->DimmId);
  CSFound = FALSE;
  ILog = 0;
  NBPtr = mmPtr->NBPtr;

  NBPtr->FamilySpecificHook[FixupSysAddr] (NBPtr, &SysAddr);

  // Loop to determine the dram range
  for (Die = 0; Die < mmPtr->DieCount; Die ++) {
    range = NBPtr[Die].Node;

    // DRAM Base
    temp = MemFGetPCI (NBPtr, 0, 0, BFDramBaseReg0 + range);
    DramEn = (UINT8) (temp & 0x3);
    IntlvEn = (UINT8) ((temp >> 8) & 0x7);

    DramBase = ((UINT64) (MemFGetPCI (NBPtr, 0, 0, BFDramBaseHiReg0 + range) & 0xFF) << 40) |
                 (((UINT64) temp & 0xFFFF0000) << 8);

    // DRAM Limit
    temp = MemFGetPCI (NBPtr, 0, 0, BFDramLimitReg0 + range);
    *NodeID = (UINT8) (temp & 0x7);
    IntlvSel = (UINT8) ((temp >> 8) & 0x7);
    DramLimit = ((UINT64) (MemFGetPCI (NBPtr, 0, 0, BFDramLimitHiReg0 + range) & 0xFF) << 40) |
                  (((UINT64) temp << 8) | 0xFFFFFF);
    DramLimitSysAddr = (((UINT64) MemFGetPCI (NBPtr, *NodeID, 0, BFDramLimitAddr)) << 27) | 0x7FFFFFF;
    ASSERT (DramLimit <= DramLimitSysAddr);

    if ((DramEn != 0) && (DramBase <= SysAddr) && (SysAddr <= DramLimitSysAddr) &&
        ((IntlvEn == 0) || (IntlvSel == ((SysAddr >> 12) & IntlvEn)))) {
      // Determine the number of bit positions consumed by Node Interleaving
      switch (IntlvEn) {

      case 0x0:
        ILog = 0;
        break;

      case 0x1:
        ILog = 1;
        break;

      case 0x3:
        ILog = 2;
        break;

      case 0x7:
        ILog = 3;
        break;

      default:
        IDS_ERROR_TRAP;
      }

      DramHoleOffset = MemFGetPCI (NBPtr, *NodeID, 0, BFDramHoleOffset) << 23;
      DramHoleValid = (BOOLEAN) MemFGetPCI (NBPtr, *NodeID, 0, BFDramHoleValid);
      DramHoleBase = MemFGetPCI (NBPtr, *NodeID, 0, BFDramHoleBase) << 24;
      // Address belongs to this node based on DramBase/Limit,
      // but is in the memory hole so it doesn't map to DRAM
      if (DramHoleValid && (DramHoleBase <= SysAddr) && (SysAddr < 0x100000000)) {
        return AGESA_BOUNDS_CHK;
      }

      // F2x10C Swapped Interleaved Region
      IntLvRgnSwapEn = (BOOLEAN) MemFGetPCI (NBPtr, *NodeID, 0, BFIntLvRgnSwapEn);
      if (IntLvRgnSwapEn) {
        IntLvRgnBaseAddr = (UINT8) MemFGetPCI (NBPtr, *NodeID, 0, BFIntLvRgnBaseAddr);
        IntLvRgnLmtAddr = (UINT8) MemFGetPCI (NBPtr, *NodeID, 0, BFIntLvRgnLmtAddr);
        IntLvRgnSize = (UINT8) MemFGetPCI (NBPtr, *NodeID, 0, BFIntLvRgnSize);
        ASSERT (IntLvRgnSize == (IntLvRgnLmtAddr - IntLvRgnBaseAddr + 1));
        if (((SysAddr >> 34) == 0) &&
          ((((SysAddr >> 27) >= IntLvRgnBaseAddr) && ((SysAddr >> 27) <= IntLvRgnLmtAddr))
           || ((SysAddr >> 27) < IntLvRgnSize))) {
          SysAddr ^= (UINT64) IntLvRgnBaseAddr << 27;
        }
      }

      // Extract variables from F2x110 DRAM Controller Select Low Register
      DctSelHiRngEn = (BOOLEAN) MemFGetPCI (NBPtr, *NodeID, 0, BFDctSelHiRngEn);
      DctSelHi = (UINT8) MemFGetPCI (NBPtr, *NodeID, 0, BFDctSelHi);
      DctSelIntLvEn = (BOOLEAN) MemFGetPCI (NBPtr, *NodeID, 0, BFDctSelIntLvEn);
      DctGangEn = (BOOLEAN) MemFGetPCI (NBPtr, *NodeID, 0, BFDctGangEn);
      DctSelIntLvAddr = (UINT8) MemFGetPCI (NBPtr, *NodeID, 0, BFDctSelIntLvAddr);
      DctSelBaseAddr = (UINT64) MemFGetPCI (NBPtr, *NodeID, 0, BFDctSelBaseAddr) << 27;
      DctSelBaseOffset = (UINT64) MemFGetPCI (NBPtr, *NodeID, 0, BFDctSelBaseOffset) << 26;


      // Determine if high DCT address range is being selected
      if (DctSelHiRngEn && !DctGangEn && (SysAddr >= DctSelBaseAddr)) {
        HiRangeSelected = TRUE;
      } else {
        HiRangeSelected = FALSE;
      }

      // Determine Channel
      if (DctGangEn) {
        *ChannelSelect = (UINT8) ((SysAddr >> 3) & 0x1);
      } else if (HiRangeSelected) {
        *ChannelSelect = DctSelHi;
      } else if (DctSelIntLvEn && (DctSelIntLvAddr == 0)) {
        *ChannelSelect = (UINT8) ((SysAddr >> 6) & 0x1);
      } else if (DctSelIntLvEn && (((DctSelIntLvAddr >> 1) & 0x1) != 0)) {
        temp = MemFUnaryXOR ((UINT32) ((SysAddr >> 16) & 0x1F));
        if ((DctSelIntLvAddr & 0x1) != 0) {
          *ChannelSelect = (UINT8) (((SysAddr >> 9) & 0x1) ^ temp);
        } else {
          *ChannelSelect = (UINT8) (((SysAddr >> 6) & 0x1) ^ temp);
        }
      } else if (DctSelIntLvEn) {
        *ChannelSelect = (UINT8) ((SysAddr >> (12 + ILog)) & 0x1);
      } else if (DctSelHiRngEn) {
        *ChannelSelect = ~DctSelHi & 0x1;
      } else {
        *ChannelSelect = 0;
      }
      ASSERT (*ChannelSelect < NBPtr[*NodeID].DctCount);

      // Determine base address offset
      if (HiRangeSelected) {
        if ((DctSelBaseAddr < DramHoleBase) && DramHoleValid && (SysAddr >= (UINT64) 0x100000000)) {
          ChannelOffset = (UINT64) DramHoleOffset;
        } else {
          ChannelOffset = DctSelBaseOffset;
        }
      } else {
        if (DramHoleValid && (SysAddr >= (UINT64) 0x100000000)) {
          ChannelOffset = (UINT64) DramHoleOffset;
        } else {
          ChannelOffset = DramBase;
        }
      }

      // Remove hoisting offset and normalize to DRAM bus addresses
      ChannelAddr = SysAddr - ChannelOffset;

      // Remove node interleaving
      if (IntlvEn != 0) {
        ChannelAddr = ((ChannelAddr >> (12 + ILog)) << 12) | (ChannelAddr & 0xFFF);
      }

      // Remove channel interleave
      if (DctSelIntLvEn && !HiRangeSelected && !DctGangEn) {
        if ((DctSelIntLvAddr & 1) != 1) {
          // A[6] Select or Hash 6
          ChannelAddr = ((ChannelAddr >> 7) << 6) | (ChannelAddr & 0x3F);
        } else if (DctSelIntLvAddr == 1) {
          // A[12]
          ChannelAddr = ((ChannelAddr >> 13) << 12) | (ChannelAddr & 0xFFF);
        } else {
          // Hash 9
          ChannelAddr = ((ChannelAddr >> 10) << 9) | (ChannelAddr & 0x1FF);
        }
      }

      // Determine the Chip Select
      for (cs = 0; cs < MAX_CS_PER_CHANNEL; ++ cs) {
        DctNum = DctGangEn ? 0 : *ChannelSelect;

        // Obtain the CS Base
        temp = MemFGetPCI (NBPtr, *NodeID, DctNum, BFCSBaseAddr0Reg + cs);
        CSEn = (BOOLEAN) (temp & 0x1);
        CSBase = ((UINT64) temp & NBPtr->CsRegMsk) << 8;

        // Obtain the CS Mask
        CSMask = ((UINT64) MemFGetPCI (NBPtr, *NodeID, DctNum, BFCSMask0Reg + (cs >> 1)) & NBPtr->CsRegMsk) << 8;

        // Adjust the Channel Addr for easy comparison
        InputAddr = ((ChannelAddr >> 8) & NBPtr->CsRegMsk) << 8;

        if (CSEn && ((InputAddr & ~CSMask) == (CSBase & ~CSMask))) {
          CSFound = TRUE;

          *ChipSelect = cs;

          temp = MemFGetPCI (NBPtr, *NodeID, 0, BFOnLineSpareControl);
          SwapDone = (BOOLEAN) ((temp >> (1 + 2 * (*ChannelSelect))) & 0x1);
          BadDramCs = (UINT8) ((temp >> (4 + 4 * (*ChannelSelect))) & 0x7);
          if (SwapDone && (cs == BadDramCs)) {
            // Find the spare rank for the channel
            for (spare = 0; spare < MAX_CS_PER_CHANNEL; ++spare) {
              if ((MemFGetPCI (NBPtr, *NodeID, DctNum, BFCSBaseAddr0Reg + spare) & 0x2) != 0) {
                *ChipSelect = spare;
                break;
              }
            }
          }
          ASSERT (*ChipSelect < MAX_CS_PER_CHANNEL);

          break;
        }
      }
    }
    if (CSFound) {
      break;
    }
  }

  // last ditch sanity check
  ASSERT (!CSFound || ((*NodeID < mmPtr->DieCount) && (*ChannelSelect < NBPtr[*NodeID].DctCount) && (*ChipSelect < MAX_CS_PER_CHANNEL)));
  if (CSFound) {
    return AGESA_SUCCESS;
  } else {
    return AGESA_BOUNDS_CHK;
  }

}


/*-----------------------------------------------------------------------------*/
/**
*
*   This function is the interface to call the PCI register access function
*   defined in NB block.
*
*   @param[in]   *NBPtr - Pointer to the parameter structure MEM_NB_BLOCK
*   @param[in]   NodeID - Node ID number of the target Northbridge
*   @param[in]   DctNum - DCT number if applicable, otherwise, put 0
*   @param[in]   BitFieldName - targeted bitfield
*
*   @retval      UINT32 - 32 bits PCI register value
*
*/
UINT32
STATIC
MemFGetPCI (
  IN   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 NodeID,
  IN   UINT8 DctNum,
  IN   BIT_FIELD_NAME BitFieldName
  )
{
  MEM_NB_BLOCK *LocalNBPtr;
  UINT8 Die;

  // Find NBBlock that associates with node NodeID
  for (Die = 0; (Die < MAX_NODES_SUPPORTED) && (NBPtr[Die].Node != NodeID); Die ++);
  ASSERT (Die < MAX_NODES_SUPPORTED);

  // Get the northbridge pointer for the targeted node.
  LocalNBPtr = &NBPtr[Die];
  LocalNBPtr->FamilySpecificHook[DCTSelectSwitch] (LocalNBPtr, &DctNum);
  LocalNBPtr->Dct = DctNum;
  // The caller of this function will take care of the ganged/unganged situation.
  // So Ganged is set to be false here, and do PCI read on the DCT specified by DctNum.
  return LocalNBPtr->GetBitField (LocalNBPtr, BitFieldName);
}

/*-----------------------------------------------------------------------------*/
/**
*
*   This function returns an even parity bit (making the total # of 1's even)
*   {0, 1} = number of set bits in argument is {even, odd}.
*
*   @param[in]   address - the address on which the parity bit will be calculated
*
*   @retval      UINT8 - parity bit
*
*/

UINT8
STATIC
MemFUnaryXOR (
  IN   UINT32 address
  )
{
  UINT8 parity;
  UINT8 index;
  parity = 0;
  for (index = 0; index < 32; ++ index) {
    parity = (UINT8) (parity ^ (address & 0x1));
    address = address >> 1;
  }
  return parity;
}
