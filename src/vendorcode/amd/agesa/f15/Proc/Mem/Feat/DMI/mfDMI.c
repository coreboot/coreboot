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
 * @e \$Revision: 60218 $ @e \$Date: 2011-10-10 23:12:47 -0600 (Mon, 10 Oct 2011) $
 *
 **/
/*****************************************************************************
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
BOOLEAN
MemFDMISupport3 (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

BOOLEAN
MemFDMISupport2 (
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
  UINT8  MaxDimms;
  UINT8  DimmIndex;
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
  UINT32 TotalSize;
  UINT32 DimmSize;
  UINT64 AddrValue;
  INT32  MTB_ps;
  INT32  FTB_ps;
  INT32  Value32;

  MEM_NB_BLOCK  *NBPtr;
  MEM_DATA_STRUCT *MemPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_DMI_INFO *DmiTable;
  MEM_PARAMETER_STRUCT *RefPtr;

  DIE_STRUCT *MCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  SPD_DEF_STRUCT *SpdDataStructure;

  NBPtr = MemMainPtr->NBPtr;
  MemPtr = MemMainPtr->MemPtr;
  SpdDataStructure = MemPtr->SpdDataStructure;
  MCTPtr = NBPtr->MCTPtr;
  RefPtr = MemPtr->ParameterListPtr;

  // Initialize local variables
  MaxDimms = 0;
  TotalSize = 0;

  AGESA_TESTPOINT (TpProcMemDmi, &MemPtr->StdHeader);

  ASSERT (NBPtr != NULL);

  MaxSockets = (UINT8) (0x000000FF & GetPlatformNumberOfSockets ());
  for (Socket = 0; Socket < MaxSockets; Socket++) {
    for (Channel = 0; Channel < GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader); Channel++) {
      temp = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      MaxDimms = MaxDimms + temp;
    }
  }

  // Allocate heap for memory DMI table 16, 17, 19, 20
  AllocHeapParams.RequestedBufferSize = MaxDimms * sizeof (MEM_DMI_INFO) + 6 + sizeof (DMI_T17_MEMORY_TYPE);

  AllocHeapParams.BufferHandle = AMD_DMI_MEM_DEV_INFO_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    PutEventLog (AGESA_CRITICAL, MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR3, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_CRITICAL, MCTPtr);
    ASSERT(FALSE); // Could not allocate heap for memory DMI table 16,17,19 and 20 for DDR3
    return FALSE;
  }

  DmiTable = (MEM_DMI_INFO *) ((UINT8 *) (AllocHeapParams.BufferPtr) + 6 + sizeof (DMI_T17_MEMORY_TYPE));
  *((UINT16 *) (AllocHeapParams.BufferPtr)) = MaxDimms;          // Number of memory devices
  *((DMI_T17_MEMORY_TYPE *) ((UINT8 *) (AllocHeapParams.BufferPtr) + 6)) = Ddr3MemType;  // Memory type

  //
  // Gather memory DMI info
  //
  DimmIndex = 0;
  for (Socket = 0; Socket < MaxSockets; Socket++) {
    MaxChannelsPerSocket = GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader);
    for (Channel = 0; Channel < MaxChannelsPerSocket; Channel++) {
      //
      // Get Node number and Dct number for this channel
      //
      ChannelPtr = MemPtr->SocketList[Socket].ChannelPtr[Channel];
      NodeId = ChannelPtr->MCTPtr->NodeId;
      Dct = ChannelPtr->Dct;
      NBPtr[NodeId].SwitchDCT (&NBPtr[NodeId], Dct);
      MaxDimmsPerChannel = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      for (Dimm = 0; Dimm < MaxDimmsPerChannel; Dimm++, DimmIndex++) {
        DmiTable[DimmIndex].TotalWidth = 0xFFFF;
        DmiTable[DimmIndex].DataWidth = 0xFFFF;
        DmiTable[DimmIndex].MemorySize = 0;
        DmiTable[DimmIndex].Speed = 0;
        DmiTable[DimmIndex].ManufacturerIdCode = 0;
        DmiTable[DimmIndex].Attributes = 0;
        DmiTable[DimmIndex].StartingAddr = 0;
        DmiTable[DimmIndex].EndingAddr = 0;
        DmiTable[DimmIndex].DimmPresent = 0;
        DmiTable[DimmIndex].Socket = Socket;
        DmiTable[DimmIndex].Channel = Channel;
        DmiTable[DimmIndex].Dimm = Dimm;
        DmiTable[DimmIndex].ConfigSpeed = 0;
        DmiTable[DimmIndex].ExtSize = 0;
        DmiTable[DimmIndex].ExtStartingAddr = 0;
        DmiTable[DimmIndex].ExtEndingAddr = 0;

        for (i = 0; i < 4; i++) {
          DmiTable[DimmIndex].SerialNumber[i] = 0xFF;
        }

        for (i = 0; i < 18; i++) {
          DmiTable[DimmIndex].PartNumber[i] = 0x0;
        }

        if (SpdDataStructure[DimmIndex].DimmPresent) {
          // Total Width (offset 08h) & Data Width (offset 0Ah)
          TotalWidth = (UINT16) SpdDataStructure[DimmIndex].Data[8];
          if ((TotalWidth & 0x18) == 0) {
            // non ECC
            if ((TotalWidth & 0x07) == 0) {
              DmiTable[DimmIndex].TotalWidth = 8;           // 8 bits
            } else if ((TotalWidth & 0x07) == 1) {
              DmiTable[DimmIndex].TotalWidth  = 16;         // 16 bits
            } else if ((TotalWidth & 0x07) == 2) {
              DmiTable[DimmIndex].TotalWidth  = 32;         // 32 bits
            } else if ((TotalWidth & 0x07) == 3) {
              DmiTable[DimmIndex].TotalWidth  = 64;         // 64 bits
            }
            DmiTable[DimmIndex].DataWidth = DmiTable[DimmIndex].TotalWidth ;
          } else {
            // ECC
            if ((TotalWidth & 0x07) == 0) {
              DmiTable[DimmIndex].TotalWidth  = 8 + 8;      // 8 bits
            } else if ((TotalWidth & 0x07) == 1) {
              DmiTable[DimmIndex].TotalWidth = 16 + 8;      // 16 bits
            } else if ((TotalWidth & 0x07) == 2) {
              DmiTable[DimmIndex].TotalWidth  = 32 + 8;     // 32 bits
            } else if ((TotalWidth & 0x07) == 3) {
              DmiTable[DimmIndex].TotalWidth  = 64 + 8;     // 64 bits
            }
            DmiTable[DimmIndex].DataWidth = DmiTable[DimmIndex].TotalWidth - 8;
          }

          // Memory Size (offset 0Ch)
          Capacity = 0;
          BusWidth = 0;
          Width = 0;
          Rank = 0;
          temp = (UINT8) SpdDataStructure[DimmIndex].Data[4];
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

          temp = (UINT8) SpdDataStructure[DimmIndex].Data[8];
          if ((temp & 0x07) == 0) {
            BusWidth = 8;                                   // 8 bits
          } else if ((temp & 0x07) == 1) {
            BusWidth = 16;                                  // 16 bits
          } else if ((temp & 0x07) == 2) {
            BusWidth = 32;                                  // 32 bits
          } else if ((temp & 0x07) == 3) {
            BusWidth = 64;                                  // 64 bits
          }

          temp = (UINT8) SpdDataStructure[DimmIndex].Data[7];
          if ((temp & 0x07) == 0) {
            Width = 4;                                      // 4 bits
          } else if ((temp & 0x07) == 1) {
            Width = 8;                                      // 8 bits
          } else if ((temp & 0x07) == 2) {
            Width = 16;                                     // 16 bits
          } else if ((temp & 0x07) == 3) {
            Width = 32;                                     // 32 bits
          }

          temp = (UINT8) SpdDataStructure[DimmIndex].Data[7];
          if (((temp >> 3) & 0x07) == 0) {
            Rank = 1;                                       // 4 bits
            DmiTable[DimmIndex].Attributes = 1;             // Single Rank Dimm
          } else if (((temp >> 3) & 0x07) == 1) {
            Rank = 2;                                       // 8 bits
            DmiTable[DimmIndex].Attributes = 2;             // Dual Rank Dimm
          } else if (((temp >> 3) & 0x07) == 2) {
            Rank = 3;                                       // 16 bits
          } else if (((temp >> 3) & 0x07) == 3) {
            Rank = 4;                                       // 32 bits
            DmiTable[DimmIndex].Attributes = 4;             // Quad Rank Dimm
          }

          DimmSize = (UINT32) (Capacity / 8 * BusWidth / Width * Rank);
          if (DimmSize < 0x7FFF) {
            DmiTable[DimmIndex].MemorySize = (UINT16) DimmSize;
          } else {
            DmiTable[DimmIndex].MemorySize = 0x7FFF;
            DmiTable[DimmIndex].ExtSize = DimmSize;
          }

          // Form Factor (offset 0Eh)
          FormFactor = (UINT8) SpdDataStructure[DimmIndex].Data[3];
          if ((FormFactor & 0x0F) == 0x01 || (FormFactor & 0x0F) == 0x02) {
            DmiTable[DimmIndex].FormFactor = 0x09;         // RDIMM or UDIMM
          } else if ((FormFactor & 0x0F) == 0x03) {
            DmiTable[DimmIndex].FormFactor = 0x0D;         // SO-DIMM
          }

          // DIMM Present
          DmiTable[DimmIndex].DimmPresent = 1;

          // Speed (offset 15h)
          MTB_ps = ((INT32) SpdDataStructure[DimmIndex].Data[10] * 1000) / SpdDataStructure[DimmIndex].Data[11];
          FTB_ps = (SpdDataStructure[DimmIndex].Data[9] >> 4) / (SpdDataStructure[DimmIndex].Data[9] & 0xF);
          Value32 = (MTB_ps * SpdDataStructure[DimmIndex].Data[12]) + (FTB_ps * (INT8) SpdDataStructure[DimmIndex].Data[34]) ;
          if (Value32 <= 938) {
            DmiTable[DimmIndex].Speed = 1067;              // DDR3-2133
          } else if (Value32 <= 1071) {
            DmiTable[DimmIndex].Speed = 933;               // DDR3-1866
          } else if (Value32 <= 1250) {
            DmiTable[DimmIndex].Speed = 800;               // DDR3-1600
          } else if (Value32 <= 1500) {
            DmiTable[DimmIndex].Speed = 667;               // DDR3-1333
          } else if (Value32 <= 1875) {
            DmiTable[DimmIndex].Speed = 533;               // DDR3-1066
          } else if (Value32 <= 2500) {
            DmiTable[DimmIndex].Speed = 400;               // DDR3-800
          }

          // Manufacturer (offset 17h)
          ManufacturerIdCode = (UINT64) SpdDataStructure[DimmIndex].Data[118];
          DmiTable[DimmIndex].ManufacturerIdCode = (ManufacturerIdCode << 8) | ((UINT64) SpdDataStructure[DimmIndex].Data[117]);

          // Serial Number (offset 18h)
          for (i = 0; i < 4; i++) {
            DmiTable[DimmIndex].SerialNumber[i] = (UINT8) SpdDataStructure[DimmIndex].Data[i + 122];
          }
          // Part Number (offset 1Ah)
          for (i = 0; i < 18; i++) {
            DmiTable[DimmIndex].PartNumber[i] = (UINT8) SpdDataStructure[DimmIndex].Data[i + 128];
          }

          // Configured Memory Clock Speed (offset 20h)
          DmiTable[DimmIndex].ConfigSpeed = NBPtr[NodeId].DCTPtr->Timings.Speed;

          // Starting/Ending Address for each DIMM
          // If Ending Address >= 0xFFFFFFFF, update Starting Address & Ending Address to 0xFFFFFFFF,
          // and use the Extended Starting Address & Extended Ending Address instead.
          if ((NBPtr[NodeId].GetBitField (&NBPtr[NodeId], BFCSBaseAddr0Reg + 2 * Dimm) & 1) != 0) {
            Address = (NBPtr[NodeId].GetBitField (&NBPtr[NodeId], BFCSBaseAddr0Reg + 2 * Dimm)) & NBPtr->CsRegMsk;
            Address = (Address & 0xFFFF0000) >> 2;
            DmiTable[DimmIndex].StartingAddr = Address;
            if (RefPtr->EnableBankIntlv && !NBPtr[NodeId].MCTPtr->DctData[Dct].BkIntDis) {
              AddrValue = (UINT64) Address + ((UINT64) ((NBPtr[NodeId].GetBitField (&NBPtr[NodeId], BFCSMask0Reg + Dimm) & 0xFFFF0000) + 0x00080000) >> 2) - 1;
              if (AddrValue >= (UINT64) 0xFFFFFFFF) {
                DmiTable[DimmIndex].StartingAddr = 0xFFFFFFFF;
                DmiTable[DimmIndex].EndingAddr = 0xFFFFFFFF;
                DmiTable[DimmIndex].ExtStartingAddr = (UINT64) Address;
                DmiTable[DimmIndex].ExtEndingAddr = AddrValue;
              } else {
                DmiTable[DimmIndex].EndingAddr = (UINT32) AddrValue;
              }
            } else {
              AddrValue = (UINT64) Address + (UINT64) (DmiTable[DimmIndex].MemorySize * 0x0400) - 1;
              if (AddrValue >= (UINT64) 0xFFFFFFFF) {
                DmiTable[DimmIndex].StartingAddr = 0xFFFFFFFF;
                DmiTable[DimmIndex].EndingAddr = 0xFFFFFFFF;
                DmiTable[DimmIndex].ExtStartingAddr = (UINT64) Address;
                DmiTable[DimmIndex].ExtEndingAddr = AddrValue;
              } else {
                DmiTable[DimmIndex].EndingAddr = (UINT32) AddrValue;
              }
            }
          }
        }  // Dimm present
        TotalSize += (UINT32) DmiTable[DimmIndex].MemorySize;
      }    // Dimm loop
    }      // Channel loop
  }          // Socket loop

  *((UINT32 *) ((UINT8 *) (AllocHeapParams.BufferPtr) + 2)) = TotalSize;      // Max Capacity

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function gets DDR2 DMI information from SPD buffer and stores the info into heap
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemFDMISupport2 (
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
  UINT8  MaxDimms;
  UINT8  DimmIndex;
  UINT8  MaxChannelsPerSocket;
  UINT8  MaxDimmsPerChannel;
  UINT8  FormFactor;
  UINT8  Temp;
  UINT8  Rank;
  UINT16 TotalWidth;
  UINT32 Speed;
  UINT32 MaxSockets;
  UINT32 Address;

  MEM_NB_BLOCK  *NBPtr;
  MEM_DATA_STRUCT *MemPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_DMI_INFO *DmiTable;
  DIE_STRUCT *MCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  SPD_DEF_STRUCT *SpdDataStructure;
  MEM_PARAMETER_STRUCT *RefPtr;

  NBPtr = MemMainPtr->NBPtr;
  MemPtr = MemMainPtr->MemPtr;
  SpdDataStructure = MemPtr->SpdDataStructure;
  MCTPtr = NBPtr->MCTPtr;
  RefPtr = MemPtr->ParameterListPtr;

  // Initialize local variables
  MaxDimms = 0;

  ASSERT (NBPtr != NULL);

  MaxSockets = (UINT8) (0x000000FF & GetPlatformNumberOfSockets ());
  for (Socket = 0; Socket < MaxSockets; Socket++) {
    for (Channel = 0; Channel < GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader); Channel++) {
      temp = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      MaxDimms = MaxDimms + temp;
    }
  }

  // Allocate heap for memory DMI table 16, 17, 19, 20
  AllocHeapParams.RequestedBufferSize = MaxDimms * sizeof (MEM_DMI_INFO) + 3;

  AllocHeapParams.BufferHandle = AMD_DMI_MEM_DEV_INFO_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    PutEventLog (AGESA_CRITICAL, MEM_ERROR_HEAP_ALLOCATE_FOR_DMI_TABLE_DDR2, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_CRITICAL, MCTPtr);
    ASSERT(FALSE); // Could not allocate heap for memory DMI table 16,17,19 and 20 for DDR2
    return FALSE;
  }

  DmiTable = (MEM_DMI_INFO *) ((UINT8 *) (AllocHeapParams.BufferPtr) + 2 + sizeof (DMI_T17_MEMORY_TYPE));
  *((UINT16 *) (AllocHeapParams.BufferPtr)) = MaxDimms;          // Number of memory devices
  *((DMI_T17_MEMORY_TYPE *) ((UINT8 *) (AllocHeapParams.BufferPtr) + 2)) = Ddr2MemType;  // Memory type

  //
  // DMI TYPE 17
  //
  DimmIndex = 0;
  for (Socket = 0; Socket < MaxSockets; Socket++) {
    MaxChannelsPerSocket = GetMaxChannelsPerSocket (RefPtr->PlatformMemoryConfiguration, Socket, &MemPtr->StdHeader);
    for (Channel = 0; Channel < MaxChannelsPerSocket; Channel++) {
      //
      // Get Node number and Dct number for this channel
      //
      ChannelPtr = MemPtr->SocketList[Socket].ChannelPtr[Channel];
      NodeId = ChannelPtr->MCTPtr->NodeId;
      Dct = ChannelPtr->Dct;
      NBPtr[NodeId].SwitchDCT (&NBPtr[NodeId], Dct);
      NBPtr[NodeId].SwitchDCT (&NBPtr[NodeId], Dct);
      MaxDimmsPerChannel = GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, Socket, Channel);
      for (Dimm = 0; Dimm < MaxDimmsPerChannel; Dimm++, DimmIndex++) {
        DmiTable[DimmIndex].TotalWidth = 0xFFFF;
        DmiTable[DimmIndex].DataWidth = 0xFFFF;
        DmiTable[DimmIndex].MemorySize = 0xFFFF;
        DmiTable[DimmIndex].Speed = 0;
        DmiTable[DimmIndex].ManufacturerIdCode = 0;
        DmiTable[DimmIndex].Attributes = 0;
        DmiTable[DimmIndex].StartingAddr = 0xFFFFFFFF;
        DmiTable[DimmIndex].EndingAddr = 0xFFFFFFFF;
        DmiTable[DimmIndex].DimmPresent = 0;
        DmiTable[DimmIndex].ConfigSpeed = 0;

        for (i = 0; i < 4; i++) {
          DmiTable[DimmIndex].SerialNumber[i] = 0xFF;
        }

        for (i = 0; i < 18; i++) {
          DmiTable[DimmIndex].PartNumber[i] = 0x0;
        }

        if (SpdDataStructure[DimmIndex].DimmPresent) {
          // Total Width (offset 08h) & Data Width (offset 0Ah)
          TotalWidth = (UINT16) SpdDataStructure[DimmIndex].Data[13];
          if ((TotalWidth & 0x04) != 0) {
            DmiTable[DimmIndex].TotalWidth = 4;          // 4 bits
          } else if ((TotalWidth & 0x08) != 0) {
            DmiTable[DimmIndex].TotalWidth = 8;          // 8 bits
          } else if ((TotalWidth & 0x10) != 0) {
            DmiTable[DimmIndex].TotalWidth = 16;         // 16 bits
          } else if ((TotalWidth & 0x20) != 0) {
            DmiTable[DimmIndex].TotalWidth = 32;         // 32 bits
          }
          DmiTable[DimmIndex].DataWidth = DmiTable[DimmIndex].TotalWidth;

          // Memory Size (offset 0Ch), Attributes (offset 1Bh)
          Rank = (UINT8) SpdDataStructure[DimmIndex].Data[5] & 0x07;
          if (Rank == 0) {
            DmiTable[DimmIndex].Attributes = 1;          // Single Rank Dimm
          } else if (Rank == 1) {
            DmiTable[DimmIndex].Attributes = 2;          // Dual Rank Dimm
          } else if (Rank == 3) {
            DmiTable[DimmIndex].Attributes = 4;          // Quad Rank Dimm
          }

          Temp = (UINT8) SpdDataStructure[DimmIndex].Data[31];
          for (i = 0; i < 8; i++) {
            if ((Temp & 0x01) == 1) {
              DmiTable[DimmIndex].MemorySize = 0x80 * (i + 1) * (Rank + 1);
            }
            Temp = Temp >> 1;
          }

          // Form Factor (offset 0Eh)
          FormFactor = (UINT8) SpdDataStructure[DimmIndex].Data[20];
          if ((FormFactor & 0x04) == 4) {
            DmiTable[DimmIndex].FormFactor = 0x0D;       // SO-DIMM
          } else {
            DmiTable[DimmIndex].FormFactor = 0x09;       // RDIMM or UDIMM
          }

          // DIMM Present
          DmiTable[DimmIndex].DimmPresent = 1;

          // DIMM Index
          DmiTable[DimmIndex].Socket = Socket;
          DmiTable[DimmIndex].Channel = Channel;
          DmiTable[DimmIndex].Dimm = Dimm;

          // Speed (offset 15h)
          Speed = NBPtr[NodeId].GetBitField (&NBPtr[NodeId], BFDramConfigHiReg);
          Speed = Speed & 0x00000007;
          if (Speed == 0) {
            DmiTable[DimmIndex].Speed = 400;              // 400MHz
          } else if (Speed == 1) {
            DmiTable[DimmIndex].Speed = 533;              // 533MHz
          } else if (Speed == 2) {
            DmiTable[DimmIndex].Speed = 667;              // 667MHz
          } else if (Speed == 3) {
            DmiTable[DimmIndex].Speed = 800;              // 800MHz
          }

          // Manufacturer (offset 17h)
          DmiTable[DimmIndex].ManufacturerIdCode = (UINT64) SpdDataStructure[DimmIndex].Data[64];

          // Serial Number (offset 18h)
          for (i = 0; i < 4; i++) {
            DmiTable[DimmIndex].SerialNumber[i] = (UINT8) SpdDataStructure[DimmIndex].Data[i + 95];
          }

          // Part Number (offset 1Ah)
          for (i = 0; i < 18; i++) {
            DmiTable[DimmIndex].PartNumber[i] = (UINT8) SpdDataStructure[DimmIndex].Data[i + 73];
          }

          // Configured Memory Clock Speed (offset 20h)
          DmiTable[DimmIndex].ConfigSpeed = NBPtr[NodeId].DCTPtr->Timings.Speed;

          // AGESA does NOT support this feature when bank interleaving is enabled.
          if (!RefPtr->EnableBankIntlv) {
            if ((NBPtr[NodeId].GetBitField (&NBPtr[NodeId], BFCSBaseAddr0Reg + 2 * Dimm) & 1) != 0) {
              Address = (NBPtr[NodeId].GetBitField (&NBPtr[NodeId], BFCSBaseAddr0Reg + 2 * Dimm)) & NBPtr->CsRegMsk;
              Address = Address >> 2;
              DmiTable[DimmIndex].StartingAddr = Address;
              DmiTable[DimmIndex].EndingAddr = Address + (UINT32) (DmiTable[DimmIndex].MemorySize * 0x0400);
            }
          }

        }        // DIMM Present
      }          // DIMM loop
    }
  }

  return TRUE;
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

