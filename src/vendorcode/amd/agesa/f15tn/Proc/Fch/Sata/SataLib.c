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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_SATA_SATALIB_FILECODE



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
  ReadPci (((SATA_BUS_DEV_FUN << 16) + 0x24), AccessWidth32, Bar5Ptr, StdHeader);

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
    WritePci (((SATA_BUS_DEV_FUN << 16) + 0x24), AccessWidth32, Bar5Ptr, StdHeader);
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
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040 ), AccessWidth8, 0xff, BIT0, StdHeader);
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
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040 ), AccessWidth8, (UINT32)~BIT0, 0, StdHeader);
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
  ReadPci (((SATA_BUS_DEV_FUN_FPGA << 16) + 0x24), AccWidthUint32, Bar5);

  //Assign temporary BAR if is not already assigned
  if ( (*Bar5 == 0) || (*Bar5 == - 1) ) {
    //assign temporary BAR5
    if ( (LocalCfgPtr->Sata.TempMMIO == 0) || (LocalCfgPtr->Sata.TempMMIO == - 1) ) {
      *Bar5 = 0xFEC01000;
    } else {
      *Bar5 = LocalCfgPtr->Sata.TempMMIO;
    }
    WritePci (((SATA_BUS_DEV_FUN_FPGA << 16) + 0x24), AccWidthUint32, Bar5);
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
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN_FPGA << 16) + 0x18), AccWidthUint16, &IoBase);
      } else {
        //this port belongs to primary channel
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN_FPGA << 16) + 0x10), AccWidthUint16, &IoBase);
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

