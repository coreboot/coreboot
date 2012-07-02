/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SATA controller
 *
 * Init SATA features.
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
#define FILECODE PROC_FCH_SATA_SATAMID_FILECODE



/**
 * FchInitMidSata - Config SATA controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidSata (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        SataPciCommandByte;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  if ( LocalCfgPtr->Sata.SataMode.SataEnable == 0 ) {
    return;                                                ///return if SATA controller is disabled.
  }

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccess (StdHeader);

  //
  // Set Sata Controller Memory & IO access enable
  //
  ReadPci (((SATA_BUS_DEV_FUN << 16) + 0x04), AccessWidth8, &SataPciCommandByte, StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04), AccessWidth8, 0xFF, 0x03, StdHeader);

  //
  // Sata Bar5 register setting for Index 0xFC
  //
  SataBar5RegSet ( LocalCfgPtr );

  FchInitMidProgramSataRegs ( LocalCfgPtr );

  //
  // Set Sata port mode (each) for Gen1/Gen2/Gen3
  //
  SataSetPortGenMode ( LocalCfgPtr );

  //
  // Call Sub-function for each Sata mode
  //
  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitMidSataAhci ( LocalCfgPtr );
  }

  if (( LocalCfgPtr->Sata.SataClass == SataIde2Ahci) || (LocalCfgPtr->Sata.SataClass == SataIde2Ahci7804 )) {
    FchInitMidSataIde2Ahci ( LocalCfgPtr );
  }

  if (( LocalCfgPtr->Sata.SataClass == SataNativeIde) || (LocalCfgPtr->Sata.SataClass == SataLegacyIde )) {
    FchInitMidSataIde ( LocalCfgPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitMidSataRaid ( LocalCfgPtr );
  }

#ifdef SATA_BUS_DEV_FUN_FPGA
  FchSataDriveFpga ( LocalCfgPtr );
#endif

  //
  // Restore Sata Controller Memory & IO access status
  //
  WritePci (((SATA_BUS_DEV_FUN << 16) + 0x04), AccessWidth8, &SataPciCommandByte, StdHeader);

  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccess (StdHeader);
}

/**
 * SataSetDeviceNumMsi - Program Sata controller support device
 * number cap & MSI cap
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
SataSetDeviceNumMsi (
  IN  VOID     *FchDataPtr
  )
{
  FchSataSetDeviceNumMsi ( FchDataPtr );
}

/**
 * SataDriveDetection - Sata drive detection
 *
 *   - Sata Ide & Sata Ide to Ahci only
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] *Bar5Ptr Sata BAR5 base address.
 *
 */
VOID
SataDriveDetection (
  IN  VOID       *FchDataPtr,
  IN  UINT32     *Bar5Ptr
  )
{
  FchSataDriveDetection ( FchDataPtr, Bar5Ptr );
}

/**
 * shutdownUnconnectedSataPortClock - Shutdown unconnected Sata port clock
 *
 *   - Sata Ide & Sata Ide to Ahci only
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] Bar5 Sata BAR5 base address.
 *
 */
VOID
ShutdownUnconnectedSataPortClock (
  IN  VOID       *FchDataPtr,
  IN  UINT32     Bar5
  )
{
  FchShutdownUnconnectedSataPortClock ( FchDataPtr, Bar5);
}

/**
 * SataSetPortGenMode - Set Sata port mode (each) for
 * Gen1/Gen2/Gen3
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
SataSetPortGenMode (
  IN  VOID     *FchDataPtr
  )
{
  FchSataSetPortGenMode ( FchDataPtr );
}
