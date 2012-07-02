/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Imc controller
 *
 * Init Imc Controller features.
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
#define FILECODE PROC_FCH_IMC_IMCENV_FILECODE



//
// Declaration of local functions
//


/**
 * FchInitEnvImc - Config Imc controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvImc (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        PortStatusByte;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  GetChipSysMode (&PortStatusByte, LocalCfgPtr->StdHeader);

  //
  // Software IMC enable
  //
  if (((LocalCfgPtr->Imc.ImcEnableOverWrite == 1) && ((PortStatusByte & ChipSysEcEnable) == 0)) || ((LocalCfgPtr->Imc.ImcEnableOverWrite == 2) && ((PortStatusByte & ChipSysEcEnable) == ChipSysEcEnable))) {
    if (ValidateImcFirmware (LocalCfgPtr)) {
      SoftwareToggleImcStrapping (LocalCfgPtr);
    }
  }

  FchInitEnvEc (LocalCfgPtr);
}

/**
 * ValidateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 * @retval        TRUE   Pass
 * @retval        FALSE  Failed
 */
BOOLEAN
ValidateImcFirmware (
  IN  VOID     *FchDataPtr
  )
{
  UINT32   ImcSig;
  UINT32   ImcSigAddr;
  UINT32   ImcAddr;
  UINT32   CurAddr;
  UINT32   ImcBinSig0;
  UINT32   ImcBinSig1;
  UINT16   ImcBinSig2;
  UINT8    IMCChecksumeByte;
  UINT8    IMCByte;

  ImcAddr = 0;

  //
  // Software IMC enable
  //
  ImcSigAddr = 0x80000;                          /// start from 512k to 64M
  ImcSig = 0x0;

  while ( ( ImcSig != 0x55aa55aa ) && ( ImcSigAddr <= 0x4000000 ) ) {
    CurAddr = 0xffffffff - ImcSigAddr + 0x20001;
    ReadMem (CurAddr, AccessWidth32, &ImcSig);
    ReadMem ((CurAddr + 4), AccessWidth32, &ImcAddr);
    ImcSigAddr <<= 1;
  }

  IMCChecksumeByte = 0xff;

  if ( ImcSig == 0x55aa55aa ) {
    //
    // "_AMD_IMC_C" at offset 0x2000 of the binary
    //
    ReadMem ((ImcAddr + 0x2000), AccessWidth32, &ImcBinSig0);
    ReadMem ((ImcAddr + 0x2004), AccessWidth32, &ImcBinSig1);
    ReadMem ((ImcAddr + 0x2008), AccessWidth16, &ImcBinSig2);

    if ((ImcBinSig0 == 0x444D415F) && (ImcBinSig1 == 0x434D495F) && (ImcBinSig2 == 0x435F) ) {
      IMCChecksumeByte = 0;

      for ( CurAddr = ImcAddr; CurAddr < ImcAddr + 0x10000; CurAddr++ ) {
        ReadMem (CurAddr, AccessWidth8, &IMCByte);
        IMCChecksumeByte = IMCChecksumeByte + IMCByte;
      }
    }
  }

  if ( IMCChecksumeByte ) {
    return  FALSE;
  } else {
    return  TRUE;
  }
}

