/* $NoKeywords:$ */
/**
 * @file
 *
 * Fch SATA controller Library
 *
 * SATA Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 46088 $   @e \$Date: 2011-01-28 11:24:26 +0800 (Fri, 28 Jan 2011) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_SATA_SATALIB_FILECODE

extern  VOID  FchSataSetDeviceNumMsi (IN VOID  *FchDataPtr);

/**
 * sataBar5setting - Config SATA BAR5
 *
 *
 * @param[in] FchDataPtr - Fch configuration structure pointer.
 * @param[in] *Bar5Ptr   - SATA BAR5 buffer.
 *
 */
VOID
SataBar5setting (
  IN  VOID       *FchDataPtr,
  IN  UINT32     *Bar5Ptr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  //Get BAR5 value
  //
  ReadPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG24), AccessWidth32, Bar5Ptr, StdHeader);

  //
  //Assign temporary BAR if is not already assigned
  //
  if ( (*Bar5Ptr == 0) || (*Bar5Ptr == - 1) ) {
    //
    //assign temporary BAR5
    //
    if ( (LocalCfgPtr->Sata.TempMmio == 0) || (LocalCfgPtr->Sata.TempMmio == - 1) ) {
      *Bar5Ptr = 0xFEC01000;
    } else {
      *Bar5Ptr = LocalCfgPtr->Sata.TempMmio;
    }
    WritePci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG24), AccessWidth32, Bar5Ptr, StdHeader);
  }

  //
  //Clear Bits 9:0
  //
  *Bar5Ptr = *Bar5Ptr & 0xFFFFFC00;
}

/**
 * sataEnableWriteAccess - Enable Sata PCI configuration space
 *
 * @param[in]   StdHeader
 *
 */
VOID
SataEnableWriteAccess (
  IN  AMD_CONFIG_PARAMS   *StdHeader
  )
{
  //
  // BIT0 Enable write access to PCI header
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG40), AccessWidth8, 0xff, BIT0, StdHeader);
}

/**
 * sataDisableWriteAccess - Disable Sata PCI configuration space
 *
 * @param[in]   StdHeader
 *
 */
VOID
SataDisableWriteAccess (
  IN  AMD_CONFIG_PARAMS   *StdHeader
  )
{
  //
  // Disable write access to PCI header
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG40), AccessWidth8, ~BIT0, 0, StdHeader);
}



#ifdef SATA_BUS_DEV_FUN_FPGA

/**
 * FchSataBar5settingFpga
 *
 * @param[in]   LocalCfgPtr
 * @param[in]   Bar5
 *
 */
VOID
FchSataBar5settingFpga (
  IN       FCH_DATA_BLOCK     *LocalCfgPtr,
  IN       UINT32             *Bar5
  )
{
  UINT8  Value;

  //Get BAR5 value
  ReadPci (((SATA_BUS_DEV_FUN_FPGA << 16) + FCH_SATA_REG24), AccWidthUint32, Bar5);

  //Assign temporary BAR if is not already assigned
  if ( (*Bar5 == 0) || (*Bar5 == - 1) ) {
    //assign temporary BAR5
    if ( (LocalCfgPtr->Sata.TempMMIO == 0) || (LocalCfgPtr->Sata.TempMMIO == - 1) ) {
      *Bar5 = 0xFEC01000;
    } else {
      *Bar5 = LocalCfgPtr->Sata.TempMMIO;
    }
    WritePci (((SATA_BUS_DEV_FUN_FPGA << 16) + FCH_SATA_REG24), AccWidthUint32, Bar5);
  }

  //Clear Bits 9:0
  *Bar5 = *Bar5 & 0xFFFFFC00;
  Value = 0x07;
  WritePci (((SATA_BUS_DEV_FUN_FPGA << 16) + 0x04), AccWidthUint8, &Value);
  WritePci (((PCIB_BUS_DEV_FUN << 16) + 0x04), AccWidthUint8, &Value);
}

/**
 * FchSataDriveDetectionFpga
 *
 * @param[in]   LocalCfgPtr
 * @param[in]   Bar5
 *
 */
VOID
FchSataDriveDetectionFpga (
  IN       FCH_DATA_BLOCK     *LocalCfgPtr,
  IN       UINT32             *Bar5
  )
{
  UINT32                 SataBarFpgaInfo;
  UINT8                  PortNum;
  UINT8                  SataFpaPortType;
  UINT16                 IoBase;
  UINT16                 SataFpgaLoopVarWord;
  AMD_CONFIG_PARAMS      *StdHeader;

  StdHeader = LocalCfgPtr->StdHeader;

  TRACE ((DMSG_FCH_TRACE, "FCH - Entering sata drive detection procedure\n\n"));
  TRACE ((DMSG_FCH_TRACE, "SATA BAR5 is %X \n", *pBar5));

  for ( PortNum = 0; PortNum < 4; PortNum++ ) {
    ReadMem (*Bar5 + FCH_SATA_BAR5_REG128 + PortNum * 0x80, AccWidthUint32, &SataBarFpgaInfo);
    if ( ( SataBarFpgaInfo & 0x0F ) == 0x03 ) {
      if ( PortNum & BIT0 ) {
        //this port belongs to secondary channel
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN_FPGA << 16) + FCH_SATA_REG18), AccWidthUint16, &IoBase);
      } else {
        //this port belongs to primary channel
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN_FPGA << 16) + FCH_SATA_REG10), AccWidthUint16, &IoBase);
      }

      //if legacy ide mode, then the bar registers don't contain the correct values. So we need to hardcode them
      if ( LocalCfgPtr->Sata.SataClass == SataLegacyIde ) {
        IoBase = ( (0x170) | ((UINT16) ( (~((UINT8) (PortNum & BIT0) << 7)) & 0x80 )) );
      }

      if ( PortNum & BIT1 ) {
        //this port is slave
        SataFpaPortType = 0xB0;
      } else {
        //this port is master
        SataFpaPortType = 0xA0;
      }

      IoBase &= 0xFFF8;
      LibAmdIoWrite (AccessWidth8, IoBase + 6, &SataFpaPortType, StdHeader);

      //Wait in loop for 30s for the drive to become ready
      for ( SataFpgaLoopVarWord = 0; SataFpgaLoopVarWord < 300000; SataFpgaLoopVarWord++ ) {
        LibAmdIoRead (AccessWidth8, IoBase + 7, &SataFpaPortType, StdHeader);
        if ( (SataFpaPortType & 0x88) == 0 ) {
          break;
        }
        FchStall (100, StdHeader);
      }
    }
  }
}

/**
 * FchSataDriveFpga -
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSataDriveFpga (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                 Bar5;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  Bar5 = 0;
  SataBar5setting (LocalCfgPtr, &Bar5);

  FchSataBar5settingFpga (LocalCfgPtr, &Bar5);
  FchSataDriveDetectionFpga (LocalCfgPtr, &Bar5);
}

#endif

