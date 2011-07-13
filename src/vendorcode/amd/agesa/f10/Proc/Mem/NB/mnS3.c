/**
 * @file
 *
 * mnS3.c
 *
 * Common Northbridge S3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 7081 $ @e \$Date: 2008-07-31 01:47:27 -0500 (Thu, 31 Jul 2008) $
 *
 **/
/*****************************************************************************
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "mfs3.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_MNS3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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
VOID
STATIC
MemNS3GetSetBitField (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       BOOLEAN IsSet,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

BOOLEAN
STATIC
MemNS3GetDummyReadAddr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
     OUT   UINT64 *TestAddr
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes the S3 resume for a node
 *
 *     @param[in,out]   *S3NBPtr - Pointer to the S3_MEM_NB_BLOCK
 *     @param[in]       NodeID - The Node id of the target die
 *
 *     @return         BOOLEAN
 *                         TRUE - This is the correct constructor for the targeted node.
 *                         FALSE - This isn't the correct constructor for the targeted node.
 */

BOOLEAN
MemNS3ResumeNb (
  IN OUT   S3_MEM_NB_BLOCK *S3NBPtr,
  IN       UINT8 NodeID
  )
{
  UINT8 DCT;
  BOOLEAN GangedEn;
  UINT64 TestAddr;
  MEM_NB_BLOCK *NBPtr;
  MEM_DATA_STRUCT *MemPtr;

  NBPtr = S3NBPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  GangedEn = (MemNGetBitFieldNb (NBPtr, BFDctGangEn) == 1) ? TRUE : FALSE;

  // Errata before S3 resume sequence

  // Resume Sequence
  // 1. Program F2x[1,0]9C_x08[DisAutoComp]=1
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 1);

  // Program F2x[1, 0]94[MemClkFreqVal] = 1.
  // 2. Wait for F2x[1,0]94[FreqChgInPrg]=0
  for (DCT = 0; DCT < NBPtr->DctCount; DCT ++) {
    MemNSwitchDCTNb (NBPtr, DCT);
    if ((MemNGetBitFieldNb (NBPtr, BFDisDramInterface) == 0) && !((DCT == 1) && GangedEn)) {
      MemNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 1);
      while (MemNGetBitFieldNb (NBPtr, BFFreqChgInProg) != 0) {}
    }
  }

  // Program F2x9C_x08[DisAutoComp]=0
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 0);
  //    BIOS must wait 750 us for the phy compensation engine
  //    to reinitialize.
  MemFS3Wait10ns (75000, NBPtr->MemPtr);

  // 3. Restore F2x[1,0]90_x00, F2x9C_x0A, and F2x[1,0]9C_x0C
  // 4. Restore F2x[1,0]9C_x04
  // Get the register value from the heap.
  S3NBPtr->MemS3ExitSelfRefReg (NBPtr, &MemPtr->StdHeader);

  // Add a hook here
  AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeExitSelfRef, &MemPtr->StdHeader);
  if (AgesaHookBeforeExitSelfRefresh (0, MemPtr) == AGESA_SUCCESS) {
  }
  AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeExitSelfRef, &MemPtr->StdHeader);

  // 5. Set F2x[1,0]90[ExitSelfRef]
  // 6. Wait for F2x[1,0]90[ExitSelfRef]=0
  for (DCT = 0; DCT < NBPtr->DctCount; DCT ++) {
    MemNSwitchDCTNb (NBPtr, DCT);
    if ((MemNGetBitFieldNb (NBPtr, BFDisDramInterface) == 0) && !((DCT == 1) && GangedEn)) {
      MemNSetBitFieldNb (NBPtr, BFExitSelfRef, 1);
      while (MemNGetBitFieldNb (NBPtr, BFExitSelfRef) != 0) {}
    }
    if ((MemNGetBitFieldNb (NBPtr, BFMemClkFreq) == DDR1333_FREQUENCY) && (NBPtr->IsSupported[CheckDllSpeedUp])) {
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D080F11, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D080F11) | 0x2000));
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D080F10, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D080F10) | 0x2000));
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D088F30, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D088F30) | 0x2000));
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D08C030, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D08C030) | 0x2000));
      if (DCT == 0) {
        MemNSetBitFieldNb (NBPtr, BFPhy0x0D082F30, (MemNGetBitFieldNb (NBPtr, BFPhy0x0D082F30) | 0x2000));
      }
      // NOTE: wait 512 clocks for DLL-relock
      MemFS3Wait10ns (50000, NBPtr->MemPtr);  // wait 500us
    }
  }

  // Errata After S3 resume sequence
  // Errata 350
  for (DCT = 0; DCT < NBPtr->DctCount; DCT ++) {
    MemNSwitchDCTNb (NBPtr, DCT);
    if (MemNGetBitFieldNb (NBPtr, BFDisDramInterface) == 0) {
      if (!((DCT == 1) && GangedEn)) {
        if (MemNS3GetDummyReadAddr (NBPtr, &TestAddr)) {
          // Do dummy read
          Read64Mem8 (TestAddr);
        }
      }
      MemNSetBitFieldNb (NBPtr, BFErr350, 0x8000);
      MemFS3Wait10ns (60, NBPtr->MemPtr);   // Wait 300ns
      MemNSetBitFieldNb (NBPtr, BFErr350, 0x0000);
      MemFS3Wait10ns (400, NBPtr->MemPtr);  // Wait 2us
    }
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns the conditional PCI device mask
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in, out]  *DescriptPtr - Pointer to DESCRIPTOR_GROUP
 *      @return         none
 */
VOID
MemNS3GetConPCIMaskNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  )
{
  BIT_FIELD_NAME bitfield;
  UINT32 RegVal;
  UINT8 DCT;
  UINT8 DimmMask;
  UINT8 BadDimmMask;
  UINT8 DctGangEn;
  BOOLEAN IsDDR3;

  IsDDR3 = FALSE;
  DimmMask = 0;
  BadDimmMask = 0;
  for (DCT = 0; DCT < NBPtr->DctCount; DCT ++) {
    NBPtr->SwitchDCT (NBPtr, DCT);
    if (MemNGetBitFieldNb (NBPtr, BFMemClkFreqVal)) {
      if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode) == 1) {
        IsDDR3 = TRUE;
      }
      for (bitfield = BFCSBaseAddr0Reg; bitfield <= BFCSBaseAddr7Reg; bitfield ++) {
        RegVal = MemNGetBitFieldNb (NBPtr, bitfield);
        if (RegVal & 0x3) {
          DimmMask |= (UINT8) (1 << ((((bitfield - BFCSBaseAddr0Reg) >> 1) << 1) + DCT));
        } else if (RegVal & 0x4) {
          BadDimmMask |= (UINT8) (1 << ((((bitfield - BFCSBaseAddr0Reg) >> 1) << 1) + DCT));
        }
      }
    }
  }

  NBPtr->SwitchDCT (NBPtr, 0);
  DctGangEn = (UINT8) MemNGetBitFieldNb (NBPtr, BFDctGangEn);
  // Set channel mask
  DescriptPtr->CPCIDevice[PRESELFREF].Mask1 = 0;
  DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 = 0;
  for (DCT = 0; DCT < NBPtr->DctCount; DCT ++) {
    if (DimmMask & (0x55 << DCT)) {
      // Set mask before exit self refresh
      DescriptPtr->CPCIDevice[PRESELFREF].Mask1 |= 1 << DCT;
      // Set mask after exit self refresh
      DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 |= 1 << DCT;
      // Set DDR3 mask if Dimms present are DDR3
      if (IsDDR3) {
        DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 |= (DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 << 4);
      }
    } else if (BadDimmMask & (0x55 << DCT)) {
      // Need to save function 2 registers for bad dimm
      DescriptPtr->CPCIDevice[PRESELFREF].Mask1 |= 1 << DCT;
    }
  }
  // Set dimm mask
  DescriptPtr->CPCIDevice[PRESELFREF].Mask2 = DimmMask;
  DescriptPtr->CPCIDevice[POSTSELFREF].Mask2 = DimmMask;
  if (DctGangEn) {
    // Need to set channel mask bit to 1 on DCT1 in ganged mode as some registers
    // need to be restored on both channels in ganged mode
    DescriptPtr->CPCIDevice[PRESELFREF].Mask1 |= 2;
    DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 |= 2;
    if (IsDDR3) {
      DescriptPtr->CPCIDevice[PRESELFREF].Mask1 |= (2 << 4);
      DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 |= (2 << 4);
    }
    // Before exit self refresh, do not copy dimm mask to DCT1 as registers restored
    // in that time frame don't care about individual dimm population. We want to
    // skip registers that are not needed to be restored for DCT1 in ganged mode.
    //
    // After exit self refresh, training registers will be restored and will only be
    // restored for slots which have dimms on it. So dimm mask needs to be copied to DCT1.
    //
    DescriptPtr->CPCIDevice[POSTSELFREF].Mask2 |= DimmMask << 1;
  }

  // Adjust the mask if there is no dimm on the node
  if ((DescriptPtr->CPCIDevice[PRESELFREF].Mask2 == 0) &&
    (DescriptPtr->CPCIDevice[POSTSELFREF].Mask2 == 0)) {
    DescriptPtr->CPCIDevice[PRESELFREF].Mask1 = DescriptPtr->CPCIDevice[PRESELFREF].Mask2 = NODE_WITHOUT_DIMM_MASK;
    DescriptPtr->CPCIDevice[POSTSELFREF].Mask1 = DescriptPtr->CPCIDevice[POSTSELFREF].Mask2 = NODE_WITHOUT_DIMM_MASK;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function read the value of CSR register.
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3GetCSRNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT32 ExtendOffset;
  UINT32 ValueRead;
  UINT8 DataPort;

  ValueRead = 0;
  ExtendOffset = Address.Address.Register;
  if (ExtendOffset & 0x800) {
    Address.Address.Register = 0xF0;
    DataPort = 0xF4;
  } else {
    Address.Address.Register = 0x98;
    DataPort = 0x9C;
  }
  if (ExtendOffset & 0x400) {
    Address.Address.Register |= 0x100;
  }
  ExtendOffset &= 0x3FF;
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &ExtendOffset, ConfigPtr);
  while (((ValueRead >> 31) & 1) == 0) {
    LibAmdPciRead (AccessS3SaveWidth32, Address, &ValueRead, ConfigPtr);
  }
  Address.Address.Register = (Address.Address.Register & 0xF00) | DataPort;
  LibAmdPciRead (AccessWidth, Address, Value, ConfigPtr);
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function write to a CSR register
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetCSRNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  UINT32 ExtendOffset;
  UINT32 ValueRead;
  UINT32 ValueWrite;
  UINT8 DataOffset;

  ValueRead = 0;
  ExtendOffset = Address.Address.Register;
  // Check the flag and see the type of the access
  if (ExtendOffset & 0x800) {
    Address.Address.Register = 0xF4;
    DataOffset = 0xF0;
  } else {
    Address.Address.Register = 0x9C;
    DataOffset = 0x98;
  }
  if (ExtendOffset & 0x400) {
    Address.Address.Register |= 0x100;
  }
  ExtendOffset &= 0x3FF;
  ExtendOffset |= 0x40000000;
  switch (AccessWidth) {
  case AccessS3SaveWidth8:
    ValueWrite = *(UINT8 *) Value;
    break;
  case AccessS3SaveWidth16:
    ValueWrite = *(UINT16 *) Value;
    break;
  case AccessS3SaveWidth32:
    ValueWrite = *(UINT32 *) Value;
    break;
  default:
    ASSERT (FALSE);
  }
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &ValueWrite, ConfigPtr);
  Address.Address.Register = (Address.Address.Register & 0xF00) | DataOffset;
  LibAmdPciWrite (AccessS3SaveWidth32, Address, &ExtendOffset, ConfigPtr);
  while (((ValueRead >> 31) & 1) == 0) {
    LibAmdPciRead (AccessS3SaveWidth32, Address, &ValueRead, ConfigPtr);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function reads register bitfield
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3GetBitFieldNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MemNS3GetSetBitField (AccessWidth, Address, FALSE, Value, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function writes register bitfield
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in, out]  *Value - Pointer to the value to be written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
MemNS3SetBitFieldNb (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  MemNS3GetSetBitField (AccessWidth, Address, TRUE, Value, ConfigPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function restores scrubber base register
 *
 *     @param[in,out]   *NBPtr - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Node - The Node id of the target die
 *
 */
VOID
MemNS3RestoreScrubNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Node
  )
{
  UINT32 ScrubAddrRJ16;

  ScrubAddrRJ16 = (MemNGetBitFieldNb (NBPtr, BFDramBaseReg0 + Node) & 0xFFFF0000) >> 8;
  ScrubAddrRJ16 |= MemNGetBitFieldNb (NBPtr, BFDramBaseHiReg0 + Node) << 24;
  MemNSetBitFieldNb (NBPtr, BFScrubAddrLoReg, ScrubAddrRJ16 << 16);
  MemNSetBitFieldNb (NBPtr, BFScrubAddrHiReg, ScrubAddrRJ16 >> 16);
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function reads and writes register bitfield
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in]   IsSet - if this is a register read or write
 *     @param[in, out]  *Value - Pointer to the value be read or  written.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *     @return         none
 */
VOID
STATIC
MemNS3GetSetBitField (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       BOOLEAN IsSet,
  IN OUT   VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  BIT_FIELD_NAME BitField;
  MEM_NB_BLOCK *NBPtr;
  LOCATE_HEAP_PTR LocateBufferPtr;
  S3_MEM_NB_BLOCK *S3NBPtr;
  UINT32 RegValue;
  UINT8 Die;

  RegValue = 0;
  // See which Node should be accessed
  Die = (UINT8) (Address.Address.Device - 24);

  LocateBufferPtr.BufferHandle = AMD_MEM_S3_NB_HANDLE;
  if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
    S3NBPtr = (S3_MEM_NB_BLOCK *) LocateBufferPtr.BufferPtr;
    NBPtr = S3NBPtr[Die].NBPtr;

    // Function field contains the DCT number
    NBPtr->SwitchDCT (NBPtr, (UINT8) Address.Address.Function);

    // Get the bitfield name to be accessed
    // Register field contains the bitfield name
    BitField = (BIT_FIELD_NAME) Address.Address.Register;

    if (IsSet) {
      switch (AccessWidth) {
      case AccessS3SaveWidth8:
        RegValue = *(UINT8 *) Value;
        break;
      case AccessS3SaveWidth16:
        RegValue = *(UINT16 *) Value;
        break;
      case AccessS3SaveWidth32:
        RegValue = *(UINT32 *) Value;
      default:
        ASSERT (FALSE);
      }
      MemNSetBitFieldNb (NBPtr, BitField, RegValue);
    } else {
      RegValue = MemNGetBitFieldNb (NBPtr, BitField);

      switch (AccessWidth) {
      case AccessS3SaveWidth8:
        *(UINT8 *) Value = (UINT8) RegValue;
        break;
      case AccessS3SaveWidth16:
        *(UINT16 *) Value = (UINT16) RegValue;
        break;
      case AccessS3SaveWidth32:
        *(UINT32 *) Value = RegValue;
        break;
      default:
        ASSERT (FALSE);
      }
    }
  } else {
    ASSERT (FALSE);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the dummy read address for a channel of a node.
 *
 *     @param[in, out]  *NBPtr - Pointer to northbridge block
 *     @param[out]      *TestAddr - Pointer to the test address
 *
 *     @retval           TRUE - Dummy read address can be found
 *     @retval           FALSE - Dummy read address cannot be found
 *
 */
BOOLEAN
STATIC
MemNS3GetDummyReadAddr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
     OUT   UINT64 *TestAddr
  )
{
  BOOLEAN DctSelIntlvEn;
  UINT8 DramIntlvEn;
  UINT8 DctSelIntlvAddr;
  UINT8 IntLvRgnBaseAddr;
  UINT8 IntLvRgnLmtAddr;
  UINT8 IntLvRgnSize;
  UINT32 DctSelBaseAddr;
  UINT64 TOM;
  BOOLEAN AddrFound;

  AddrFound = TRUE;
  // Check if Node interleaving is enabled
  DramIntlvEn = (UINT8) MemNGetBitFieldNb (NBPtr, BFDramIntlvEn);
  if (DramIntlvEn != 0) {
    // Set the address bits that identify the node
    *TestAddr = (UINT64) MemNGetBitFieldNb (NBPtr, BFDramIntlvSel) << 12;
  } else {
    *TestAddr = (UINT64) MemNGetBitFieldNb (NBPtr, BFDramBaseAddr) << 27;
  }

  // Check if channel interleaving is enabled
  DctSelIntlvEn = (BOOLEAN) MemNGetBitFieldNb (NBPtr, BFDctSelIntLvEn);
  DctSelBaseAddr = MemNGetBitFieldNb (NBPtr, BFDctSelBaseAddr);
  if (!DctSelIntlvEn) {
    if ((NBPtr->Dct == 1) && ((UINT8) MemNGetBitFieldNb (NBPtr, BFDctSelHi) == 1)) {
      *TestAddr = ((UINT64) DctSelBaseAddr << 27) | (*TestAddr & 0xFFFFFFF);
    }
  } else {
    DctSelIntlvAddr = (UINT8) MemNGetBitFieldNb (NBPtr, BFDctSelIntLvAddr);
    // Set the address bits that identify the channel
    if ((DctSelIntlvAddr == 0) || (DctSelIntlvAddr == 2)) {
      *TestAddr |= (UINT64) NBPtr->Dct << 6;
    } else if (DctSelIntlvAddr == 1) {
      *TestAddr |= (UINT64) NBPtr->Dct << (12 + LibAmdBitScanReverse (DramIntlvEn + 1));
    } else if (DctSelIntlvAddr == 3) {
      *TestAddr |= (UINT64) NBPtr->Dct << 9;
    }
  }
  // Adding 2M to avoid conflict
  *TestAddr += 0x200000;

  // If memory hoisting is disabled, the address can fall into MMIO area
  // Need to find an address out of MMIO area but belongs to the channel
  // If the whole channel is in MMIO, then do not do dummy read.
  //
  LibAmdMsrRead (TOP_MEM, &TOM, &NBPtr->MemPtr->StdHeader);
  if ((*TestAddr >= TOM) && (*TestAddr < ((UINT64) _4GB_RJ16 << 16))) {
    if ((NBPtr->Dct == 1) && ((UINT8) MemNGetBitFieldNb (NBPtr, BFDctSelHi) == 1)) {
      // This is the DCT that goes to high address range
      if (DctSelBaseAddr >= (_4GB_RJ16 >> (27 - 16))) {
        // When DctSelBaseAddr is higher than 4G, choose DctSelBaseAddr as the dummy read addr
        if (DctSelIntlvEn) {
          *TestAddr = ((UINT64) DctSelBaseAddr << 27) | (*TestAddr & 0xFFFFFFF);
        }
      } else if (MemNGetBitFieldNb (NBPtr, BFDramLimitAddr) > (UINT32) (_4GB_RJ16 >> (27 - 16))) {
        // if DctSelBase is smaller than 4G, but Dram limit is larger than 4G, then choose 4G as
        // dummy read address
        *TestAddr =  ((UINT64) _4GB_RJ16 << 16) | (*TestAddr & 0xFFFFFF);
      } else {
        AddrFound = FALSE;
      }
    } else {
      // This is the DCT that only goes to low address range
      if (DctSelBaseAddr > (_4GB_RJ16 >> (27 - 16))) {
        // When DctSelBaseAddr is larger than 4G, choose 4G as the dummy read address
        // Keep the lower bits for node and channel selection
        *TestAddr = ((UINT64) _4GB_RJ16 << 16) | (*TestAddr & 0xFFFFFF);
      } else {
        AddrFound = FALSE;
      }
    }
  }

  // Interleaved Swap Region handling
  if ((BOOLEAN) MemNGetBitFieldNb (NBPtr, BFIntLvRgnSwapEn)) {
    IntLvRgnBaseAddr = (UINT8) MemNGetBitFieldNb (NBPtr, BFIntLvRgnBaseAddr);
    IntLvRgnLmtAddr = (UINT8) MemNGetBitFieldNb (NBPtr, BFIntLvRgnLmtAddr);
    IntLvRgnSize = (UINT8) MemNGetBitFieldNb (NBPtr, BFIntLvRgnSize);
    ASSERT (IntLvRgnSize == (IntLvRgnLmtAddr - IntLvRgnBaseAddr + 1));
    if (((*TestAddr >> 34) == 0) &&
      ((((*TestAddr >> 27) >= IntLvRgnBaseAddr) && ((*TestAddr >> 27) <= IntLvRgnLmtAddr))
       || ((*TestAddr >> 27) < IntLvRgnSize))) {
      *TestAddr ^= (UINT64) IntLvRgnBaseAddr << 27;
    }
  }

  return AddrFound;
}
