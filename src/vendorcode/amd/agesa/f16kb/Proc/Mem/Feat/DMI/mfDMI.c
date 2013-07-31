/* $NoKeywords:$ */
/**
 * @file
 *
 * mfDMI.c
 *
 * Memory DMI table support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "Ids.h"
#include "heapManager.h"
#include "cpuServices.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_DMI_MFDMI_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#define MAX_DCTS_PER_DIE 2

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
UINT64
MemFGetNodeMemBase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeLimit
  );

UINT64
MemFGetDctMemBase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 DctLimit
  );

UINT64
MemFGetDctInterleavedMemSize (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT64
MemFGetDctInterleavedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT64 DctMemBase,
  IN       UINT64 DctInterleavedMemSize
  );

BOOLEAN
MemFDMISupport3 (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
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
 *  This function gets DDR3 DMI information from SPD buffer and stores the info into heap
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemFDMISupport3 (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8  i;
  UINT8  Dimm;
  UINT8  Socket;
  UINT8  NodeId;
  UINT8  Dct;
  UINT8  Channel;
  UINT8  temp;
  UINT16 LocalHandle;
  UINT8  MaxPhysicalDimms;
  UINT8  MaxLogicalDimms;
  UINT8  NumLogicalDimms;
  UINT32 *TotalMemSizePtr;
  UINT8  *DmiTable;
  UINT8  MaxChannelsPerSocket;
  UINT8  MaxDimmsPerChannel;
  UINT8  FormFactor;
  UINT16 TotalWidth;
  UINT16 Capacity;
  UINT16 Width;
  UINT16 Rank;
  UINT16 BusWidth;
  UINT64 ManufacturerIdCode;
  UINT32 MaxSockets;
  UINT32 Address;
  UINT8  ChipSelectPairScale;
  UINT32 TotalSize;
  UINT32 DimmSize;
  UINT32 CsBaseAddrReg0;
  UINT32 CsBaseAddrReg1;
  UINT64 AddrValue;
  UINT64 DctMemBase;
  UINT64 NodeMemBase;
  UINT64 SysMemSize;
  INT32  MTB_ps;
  INT32  FTB_ps;
  INT32  Value32;
  BOOLEAN DctInterleaveEnabled;
  UINT64  DctInterleavedMemSize;
  BOOLEAN NodeInterleaveEnabled;
  UINT16 T17HandleMatrix[MAX_SOCKETS_SUPPORTED][MAX_CHANNELS_PER_SOCKET][MAX_DIMMS_PER_CHANNEL];

  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  SPD_DEF_STRUCT *SpdDataStructure;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_DMI_PHYSICAL_DIMM_INFO *DmiPhysicalDimmInfoTable;
  MEM_DMI_LOGICAL_DIMM_INFO *DmiLogicalDimmInfoTable;
  DMI_T17_MEMORY_TYPE *DmiType17MemTypePtr;

  NBPtr = MemMainPtr->NBPtr;
  MemPtr = MemMainPtr->MemPtr;
  SpdDataStructure = MemPtr->SpdDataStructure;
  MCTPtr = NBPtr->MCTPtr;
  RefPtr = MemPtr->ParameterListPtr;

  // Initialize local variables
  LocalHandle = 1;
  MaxPhysicalDimms = 0;
  MaxLogicalDimms = 0;
  NumLogicalDimms = 0;
  TotalSize = 0;
  NodeMemBase = 0;
  SysMemSize = 0;

  AGESA_TESTPOINT (TpProcMemDmi, &MemPtr->StdHeader);

  ASSERT (NBPtr != NULL);

  MaxSockets = (UINT8) (0x000000FF & GetPlatformNumberOfSockets ());
  for (Socket = 0; Socket < MaxSockets; Socket++) {
    for (Channel = 0; Channel < GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader); Channel++) {
      temp = (UINT8) GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      MaxPhysicalDimms = MaxPhysicalDimms + temp;
      MaxLogicalDimms += MAX_DIMMS_PER_CHANNEL;
    }
  }

  // Allocate heap for memory DMI table 16, 17, 19, 20
  AllocHeapParams.RequestedBufferSize = sizeof (UINT8) +                                       // Storage for MaxPhysicalDimms
                                        sizeof (UINT8) +                                       // Storage for Type Detail
                                        sizeof (UINT32) +                                      // Storage for Total Memory Size
                                        sizeof (DMI_T17_MEMORY_TYPE) +
                                        MaxPhysicalDimms * sizeof (MEM_DMI_PHYSICAL_DIMM_INFO) +
                                        sizeof (UINT8) +                                       // Storage for MaxLogicalDimms
                                        MaxLogicalDimms * sizeof (MEM_DMI_LOGICAL_DIMM_INFO);

  AllocHeapParams.BufferHandle = AMD_DMI_MEM_DEV_INFO_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    PutEventLog (AGESA_CRITICAL, MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_CRITICAL, MCTPtr);
    // Could not allocate heap for memory DMI table 16,17,19 and 20 for DDR3
    ASSERT (FALSE);
    return FALSE;
  }

  DmiTable = (UINT8 *) AllocHeapParams.BufferPtr;
  // Max number of physical DIMMs
  *DmiTable = MaxPhysicalDimms;
  DmiTable ++;
  // Type Detail;
  *DmiTable = 0;
  DmiTable ++;
  // Pointer to total memory size
  TotalMemSizePtr = (UINT32 *) DmiTable;
  DmiTable += sizeof (UINT32);
  // Memory Type
  DmiType17MemTypePtr = (DMI_T17_MEMORY_TYPE *) DmiTable;
  *DmiType17MemTypePtr = Ddr3MemType;
  DmiType17MemTypePtr ++;
  DmiTable = (UINT8 *) DmiType17MemTypePtr;
  // Pointer to DMI info of Physical DIMMs
  DmiPhysicalDimmInfoTable = (MEM_DMI_PHYSICAL_DIMM_INFO *) DmiTable;

  //
  // Gather physical DIMM DMI info
  //
  for (Socket = 0; Socket < MaxSockets; Socket ++) {
    MaxChannelsPerSocket = GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader);
    DctMemBase = 0;
    for (Channel = 0; Channel < MaxChannelsPerSocket; Channel ++) {
      //
      // Get Node number and Dct number for this channel
      //
      ChannelPtr = MemPtr->SocketList[Socket].ChannelPtr[Channel];
      NodeId = ChannelPtr->MCTPtr->NodeId;
      NBPtr = MemMainPtr->NBPtr;
      Dct = ChannelPtr->Dct;
      NBPtr = &NBPtr[NodeId];
      NBPtr->SwitchDCT (NBPtr, Dct);

      MaxDimmsPerChannel = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      for (Dimm = 0; Dimm < MaxDimmsPerChannel; Dimm ++, SpdDataStructure ++, LocalHandle++) {
        //
        // Initialize default value for DMI fields
        //
        DmiPhysicalDimmInfoTable->DimmPresent = 0;
        DmiPhysicalDimmInfoTable->Socket = Socket;
        DmiPhysicalDimmInfoTable->Channel = Channel;
        DmiPhysicalDimmInfoTable->Dimm = Dimm;
        DmiPhysicalDimmInfoTable->TotalWidth = 0xFFFF;
        DmiPhysicalDimmInfoTable->DataWidth = 0xFFFF;
        DmiPhysicalDimmInfoTable->MemorySize = 0;
        DmiPhysicalDimmInfoTable->Speed = 0;
        DmiPhysicalDimmInfoTable->ManufacturerIdCode = 0;
        DmiPhysicalDimmInfoTable->Attributes = 0;
        DmiPhysicalDimmInfoTable->ConfigSpeed = 0;
        DmiPhysicalDimmInfoTable->ExtSize = 0;
        DmiPhysicalDimmInfoTable->FormFactor = UnknowFormFactor;
        DmiPhysicalDimmInfoTable->Handle = LocalHandle;
        T17HandleMatrix[Socket][Channel][Dimm] = LocalHandle;

        for (i = 0; i < 4; i++) {
          DmiPhysicalDimmInfoTable->SerialNumber[i] = 0xFF;
        }

        for (i = 0; i < 18; i++) {
          DmiPhysicalDimmInfoTable->PartNumber[i] = 0x0;
        }

        if (SpdDataStructure->DimmPresent) {
          // Total Width (offset 08h) & Data Width (offset 0Ah)
          TotalWidth = (UINT16) SpdDataStructure->Data[8];
          if ((TotalWidth & 0x18) == 0) {
            // non ECC
            if ((TotalWidth & 0x07) == 0) {
              DmiPhysicalDimmInfoTable->TotalWidth = 8;           // 8 bits
            } else if ((TotalWidth & 0x07) == 1) {
              DmiPhysicalDimmInfoTable->TotalWidth  = 16;         // 16 bits
            } else if ((TotalWidth & 0x07) == 2) {
              DmiPhysicalDimmInfoTable->TotalWidth  = 32;         // 32 bits
            } else if ((TotalWidth & 0x07) == 3) {
              DmiPhysicalDimmInfoTable->TotalWidth  = 64;         // 64 bits
            }
            DmiPhysicalDimmInfoTable->DataWidth = DmiPhysicalDimmInfoTable->TotalWidth ;
          } else {
            // ECC
            if ((TotalWidth & 0x07) == 0) {
              DmiPhysicalDimmInfoTable->TotalWidth  = 8 + 8;      // 8 bits
            } else if ((TotalWidth & 0x07) == 1) {
              DmiPhysicalDimmInfoTable->TotalWidth = 16 + 8;      // 16 bits
            } else if ((TotalWidth & 0x07) == 2) {
              DmiPhysicalDimmInfoTable->TotalWidth  = 32 + 8;     // 32 bits
            } else if ((TotalWidth & 0x07) == 3) {
              DmiPhysicalDimmInfoTable->TotalWidth  = 64 + 8;     // 64 bits
            }
            DmiPhysicalDimmInfoTable->DataWidth = DmiPhysicalDimmInfoTable->TotalWidth - 8;
          }

          // Memory Size (offset 0Ch)
          Capacity = 0;
          BusWidth = 0;
          Width = 0;
          Rank = 0;
          temp = (UINT8) SpdDataStructure->Data[4];
          if ((temp & 0x0F) == 0) {
            Capacity = 0x0100;                              // 256M
          } else if ((temp & 0x0F) == 1) {
            Capacity = 0x0200;                              // 512M
          } else if ((temp & 0x0F) == 2) {
            Capacity = 0x0400;                              // 1G
          } else if ((temp & 0x0F) == 3) {
            Capacity = 0x0800;                              // 2G
          } else if ((temp & 0x0F) == 4) {
            Capacity = 0x1000;                              // 4G
          } else if ((temp & 0x0F) == 5) {
            Capacity = 0x2000;                              // 8G
          } else if ((temp & 0x0F) == 6) {
            Capacity = 0x4000;                              // 16G
          }

          temp = (UINT8) SpdDataStructure->Data[8];
          if ((temp & 0x07) == 0) {
            BusWidth = 8;                                   // 8 bits
          } else if ((temp & 0x07) == 1) {
            BusWidth = 16;                                  // 16 bits
          } else if ((temp & 0x07) == 2) {
            BusWidth = 32;                                  // 32 bits
          } else if ((temp & 0x07) == 3) {
            BusWidth = 64;                                  // 64 bits
          }

          temp = (UINT8) SpdDataStructure->Data[7];
          if ((temp & 0x07) == 0) {
            Width = 4;                                      // 4 bits
          } else if ((temp & 0x07) == 1) {
            Width = 8;                                      // 8 bits
          } else if ((temp & 0x07) == 2) {
            Width = 16;                                     // 16 bits
          } else if ((temp & 0x07) == 3) {
            Width = 32;                                     // 32 bits
          }

          temp = (UINT8) SpdDataStructure->Data[7];
          if (((temp >> 3) & 0x07) == 0) {
            Rank = 1;                                       // 4 bits
            DmiPhysicalDimmInfoTable->Attributes = 1;        // Single Rank Dimm
          } else if (((temp >> 3) & 0x07) == 1) {
            Rank = 2;                                       // 8 bits
            DmiPhysicalDimmInfoTable->Attributes = 2;        // Dual Rank Dimm
          } else if (((temp >> 3) & 0x07) == 2) {
            Rank = 3;                                       // 16 bits
          } else if (((temp >> 3) & 0x07) == 3) {
            Rank = 4;                                       // 32 bits
            DmiPhysicalDimmInfoTable->Attributes = 4;        // Quad Rank Dimm
          }

          DimmSize = (UINT32) (Capacity / 8 * BusWidth / Width * Rank);
          if (DimmSize < 0x7FFF) {
            DmiPhysicalDimmInfoTable->MemorySize = (UINT16) DimmSize;
          } else {
            DmiPhysicalDimmInfoTable->MemorySize = 0x7FFF;
            DmiPhysicalDimmInfoTable->ExtSize = DimmSize;
          }

          // Form Factor (offset 0Eh)
          FormFactor = (UINT8) SpdDataStructure->Data[3];
          if (((FormFactor & 0x0F) == 0x01) || ((FormFactor & 0x0F) == 0x02) || ((FormFactor & 0x0F) == 0x0B)) {
            DmiPhysicalDimmInfoTable->FormFactor = 0x09;         // RDIMM or UDIMM or LRDIMM
          } else if (((FormFactor & 0x0F) == 0x03) || ((FormFactor & 0x0F) == 0x08) || ((FormFactor & 0x0F) == 0x09) || ((FormFactor & 0x0F) == 0x0A)) {
            DmiPhysicalDimmInfoTable->FormFactor = 0x0D;         // SO-DIMM, SO-UDIMM, SO-RDIMM and SO-CDIMM
          } else {
            DmiPhysicalDimmInfoTable->FormFactor = 0x02;         // UNKNOWN
          }

          // DIMM Present
          DmiPhysicalDimmInfoTable->DimmPresent = 1;

          // Type Detail (offset 13h)
          if (((FormFactor & 0x0F) == 0x01) || ((FormFactor & 0x0F) == 0x0B)) {
            *((UINT8 *) (AllocHeapParams.BufferPtr) + 1) = 1;      // Registered (Buffered)
          } else {
            *((UINT8 *) (AllocHeapParams.BufferPtr) + 1) = 2;      // Unbuffered (Unregistered)
          }

          // Speed (offset 15h)
          MTB_ps = ((INT32) SpdDataStructure->Data[10] * 1000) / SpdDataStructure->Data[11];
          FTB_ps = (SpdDataStructure->Data[9] >> 4) / (SpdDataStructure->Data[9] & 0xF);
          Value32 = (MTB_ps * SpdDataStructure->Data[12]) + (FTB_ps * (INT8) SpdDataStructure->Data[34]) ;
          if (Value32 <= 938) {
            DmiPhysicalDimmInfoTable->Speed = 1067;              // DDR3-2133
          } else if (Value32 <= 1071) {
            DmiPhysicalDimmInfoTable->Speed = 933;               // DDR3-1866
          } else if (Value32 <= 1250) {
            DmiPhysicalDimmInfoTable->Speed = 800;               // DDR3-1600
          } else if (Value32 <= 1500) {
            DmiPhysicalDimmInfoTable->Speed = 667;               // DDR3-1333
          } else if (Value32 <= 1875) {
            DmiPhysicalDimmInfoTable->Speed = 533;               // DDR3-1066
          } else if (Value32 <= 2500) {
            DmiPhysicalDimmInfoTable->Speed = 400;               // DDR3-800
          }

          // Manufacturer (offset 17h)
          ManufacturerIdCode = (UINT64) SpdDataStructure->Data[118];
          DmiPhysicalDimmInfoTable->ManufacturerIdCode = (ManufacturerIdCode << 8) | ((UINT64) SpdDataStructure->Data[117]);

          // Serial Number (offset 18h)
          for (i = 0; i < 4; i++) {
            DmiPhysicalDimmInfoTable->SerialNumber[i] = (UINT8) SpdDataStructure->Data[i + 122];
          }
          // Part Number (offset 1Ah)
          for (i = 0; i < 18; i++) {
            DmiPhysicalDimmInfoTable->PartNumber[i] = (UINT8) SpdDataStructure->Data[i + 128];
          }

          // Configured Memory Clock Speed (offset 20h)
          DmiPhysicalDimmInfoTable->ConfigSpeed = NBPtr->DCTPtr->Timings.Speed;
        }  // Dimm present
        TotalSize += (UINT32) DmiPhysicalDimmInfoTable->MemorySize;
        DmiPhysicalDimmInfoTable ++;
      }    // Dimm loop
    }      // Channel loop
  }        // Socket loop

  // Max number of logical DIMMs
  DmiTable = (UINT8 *) DmiPhysicalDimmInfoTable;
  *DmiTable = MaxLogicalDimms;
  DmiTable ++;

  // Pointer to DMI info of Logical DIMMs
  DmiLogicalDimmInfoTable = (MEM_DMI_LOGICAL_DIMM_INFO *) DmiTable;

  // Calculate the total memory size in the system
  SysMemSize = MemFGetNodeMemBase (MemMainPtr->NBPtr, MemPtr->DieCount);
  //
  // Gather logical DIMM DMI info
  //
  for (Socket = 0; Socket < MaxSockets; Socket ++) {
    MaxChannelsPerSocket = GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader);
    DctMemBase = 0;
    for (Channel = 0; Channel < MaxChannelsPerSocket; Channel ++) {
      MaxDimmsPerChannel = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      //
      // Get Node number and Dct number for this channel
      //
      ChannelPtr = MemPtr->SocketList[Socket].ChannelPtr[Channel];
      NodeId = ChannelPtr->MCTPtr->NodeId;
      NBPtr = MemMainPtr->NBPtr;
      Dct = ChannelPtr->Dct;
      NodeMemBase = MemFGetNodeMemBase (NBPtr, NodeId);
      NBPtr = &NBPtr[NodeId];

      DctMemBase = MemFGetDctMemBase (NBPtr, Dct);
      DctInterleavedMemSize = MemFGetDctInterleavedMemSize (NBPtr);
      NBPtr->SwitchDCT (NBPtr, Dct);
      NodeInterleaveEnabled = (NBPtr->GetBitField (NBPtr, BFDramIntlvEn) == 0) ? FALSE : TRUE;
      DctInterleaveEnabled = (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) == 0) ? FALSE : TRUE;

      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm ++, DmiLogicalDimmInfoTable ++) {
        //
        // Initialize default value for DMI fields
        //
        DmiLogicalDimmInfoTable->DimmPresent = 0;
        DmiLogicalDimmInfoTable->Socket = Socket;
        DmiLogicalDimmInfoTable->Channel = Channel;
        DmiLogicalDimmInfoTable->Dimm = Dimm;
        DmiLogicalDimmInfoTable->StartingAddr = 0;
        DmiLogicalDimmInfoTable->EndingAddr = 0;
        DmiLogicalDimmInfoTable->ExtStartingAddr = 0;
        DmiLogicalDimmInfoTable->ExtEndingAddr = 0;
        //
        // Starting/Ending Address for each DIMM
        //
        // If Ending Address >= 0xFFFFFFFF, update Starting Address & Ending Address to 0xFFFFFFFF,
        // and use the Extended Starting Address & Extended Ending Address instead.
        //
        CsBaseAddrReg0 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + 2 * Dimm);
        CsBaseAddrReg1 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + 2 * Dimm + 1);
        if ((CsBaseAddrReg0 & 1) != 0 || (CsBaseAddrReg1 & 1) != 0) {
          ASSERT (NumLogicalDimms < MaxLogicalDimms);
          NumLogicalDimms ++;
          DmiLogicalDimmInfoTable->DimmPresent = 1;
          if (Dimm < MaxDimmsPerChannel) {
            // The logical DIMM is mapped from a SR/QR physical DIMM
            DmiLogicalDimmInfoTable->MemoryDeviceHandle = T17HandleMatrix[Socket][Channel][Dimm];
          } else {
            // The logical DIMM is mapped from a QR physical DIMM
            DmiLogicalDimmInfoTable->MemoryDeviceHandle = T17HandleMatrix[Socket][Channel][Dimm - 2];
          }
          //
          // For DR and QR DIMMs,  DIMM size should be scaled based on the CS pair presence
          //
          ChipSelectPairScale = ((CsBaseAddrReg0 & 1) != 0 && (CsBaseAddrReg1 & 1) != 0) ? 1 : 0;
          Address = ((CsBaseAddrReg0 & 1) != 0 ? CsBaseAddrReg0 : CsBaseAddrReg1) & NBPtr->CsRegMsk;
          Address = (Address & 0xFFFF0000) >> 2;
          if (DctInterleaveEnabled) {
            //
            // When channel interleaving is enabled, all the DIMMs on the node share the same starting address
            //
            Address = (UINT32)NodeMemBase;
          } else {
            if (NBPtr->DCTPtr->BkIntDis == FALSE && NBPtr->RefPtr->EnableBankIntlv == TRUE) {
              //
              // When bank interleaving is enabled, all the chip selects share the same starting address
              //
              Address = (UINT32) (NodeMemBase + DctMemBase);
            } else {
              Address += (UINT32) (NodeMemBase + DctMemBase);
            }
          }
          if (NodeInterleaveEnabled) {
            //
            // When node interleaving is enabled, all the DIMMs in the system share the same starting address
            //
            Address = 0;
          }
          DmiLogicalDimmInfoTable->StartingAddr = Address;
          AddrValue = (UINT64) Address +
                      ((UINT64) ((NBPtr->GetBitField (NBPtr, BFCSMask0Reg + Dimm) & 0xFFFF0000) + 0x00080000) >>
                      (2 - ChipSelectPairScale) ) - 1;
          if (NBPtr->DCTPtr->BkIntDis == FALSE && NBPtr->RefPtr->EnableBankIntlv == TRUE) {
            //
            // When bank interleaving is enabled, all the chip selects share the same ending address
            //
            AddrValue = NodeMemBase + DctMemBase + (NBPtr->DCTPtr->Timings.DctMemSize << 6) - 1;
          }
          if (DctInterleaveEnabled) {
            //
            // When channel interleaving is enabled, the interleaved range is accounted for in the ending address of each DCT
            //
            AddrValue = NodeMemBase + MemFGetDctInterleavedLimit (NBPtr, Dct, DctMemBase, DctInterleavedMemSize);
          }
          if (NodeInterleaveEnabled) {
            //
            // When node interleaving is enabled, all the DIMMs in the system share the same ending address
            //
            AddrValue = SysMemSize - 1;
          }
          if (AddrValue >= ((UINT64) 0xFFFFFFFF)) {
            DmiLogicalDimmInfoTable->StartingAddr = 0xFFFFFFFFUL;
            DmiLogicalDimmInfoTable->EndingAddr = 0xFFFFFFFFUL;
            DmiLogicalDimmInfoTable->ExtStartingAddr = (UINT64) Address;
            DmiLogicalDimmInfoTable->ExtEndingAddr = AddrValue;
          } else {
            DmiLogicalDimmInfoTable->EndingAddr = (UINT32) AddrValue;
          }
        }
      }    // Dimm loop
    }      // Channel loop
  }        // Socket loop

  // Max Capacity
  *TotalMemSizePtr = TotalSize;

  return TRUE;
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/**
 *
 *
 *   This function obtains the memory size RJ6 contributed by the previous nodes
 *
 *
 *     @param[in,out]   *NBPtr        - Pointer to the MEM_NB_BLOCK
 *     @param[in]        NodeLimit    - Node number
 *
 */
UINT64
MemFGetNodeMemBase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeLimit
  )
{
  UINT8  Node;
  UINT64 NodeMemBase;

  NodeMemBase = 0;
  // Calculate the total memory size in the system
  for (Node = 0; Node < NodeLimit; Node++) {
    NodeMemBase += (NBPtr[Node].MCTPtr->NodeMemSize << 6);
  }

  return NodeMemBase;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function obtains the memory size RJ6 contributed by the previous dcts on
 *   the current node
 *
 *
 *     @param[in,out]   *NBPtr        - Pointer to the MEM_NB_BLOCK
 *     @param[in]        DctLimit     - Dct number
 *
 */
UINT64
MemFGetDctMemBase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 DctLimit
  )
{
  UINT8  Dct;
  UINT64 DctMemBase;

  DctMemBase = 0;
  // Calculate the total memory size in the system
  for (Dct = 0; Dct < DctLimit; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    DctMemBase += (NBPtr->DCTPtr->Timings.DctMemSize << 6);
  }

  return DctMemBase;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function obtains the interleaved memory size RJ6 contributed on the
 *   current dct
 *
 *
 *     @param[in,out]   *NBPtr        - Pointer to the MEM_NB_BLOCK
 *
 */
UINT64
MemFGetDctInterleavedMemSize (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8  Dct;
  UINT64 DctInterleavedMemSize;

  DctInterleavedMemSize = NBPtr->MCTPtr->NodeMemSize << 6;
  // Find minimum memory size among the interleaved DCTs
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (DctInterleavedMemSize > (NBPtr->DCTPtr->Timings.DctMemSize << 6)) {
      DctInterleavedMemSize = (NBPtr->DCTPtr->Timings.DctMemSize << 6);
    }
  }

  return DctInterleavedMemSize;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function obtains the memory ending address RJ6 contributed by a dct
 *   under channel interleaving
 *
 *
 *     @param[in,out]   *NBPtr                   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        Dct                     - Dct number
 *     @param[in]        DctMemBase              - Dct memory base
 *     @param[in]        DctInterleavedMemSize   - Dct interleaved memory size
 *
 */
UINT64
MemFGetDctInterleavedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT64 DctMemBase,
  IN       UINT64 DctInterleavedMemSize
  )
{
  UINT64 DctMemLimit;
  UINT8 i;

  DctMemLimit = 0;
  if (DctInterleavedMemSize == NBPtr->DCTPtr->Timings.DctMemSize << 6) {
    // The whole memory range is interleaved for the DCTs with the minimum memory size
    for (i = 0; i < NBPtr->DctCount; i++) {
      DctMemLimit += DctInterleavedMemSize;
    }
    DctMemLimit -= 1;
  } else {
    // Part of the memory range is interleaved for the DCTs with memory size larger than
    // the minimum. The remaining is treated as non-interleaved.
    for (i = 0; i < NBPtr->DctCount - 1 - Dct; i++) {
      DctMemLimit += DctInterleavedMemSize;
    }
    DctMemLimit += DctMemBase + (NBPtr->DCTPtr->Timings.DctMemSize << 6) - 1;
  }

  return DctMemLimit;
}
