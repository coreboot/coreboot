/**
 * @file
 *
 *  Function dispatcher.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */

/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "SBPLATFORM.h"

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
*/


//
// Declaration of local functions
//

VOID  saveConfigPointer (IN AMDSBCFG* pConfig);
VOID* VerifyImage (IN UINT64 Signature, IN VOID* ImagePtr);
VOID* LocateImage (IN UINT64 Signature);

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * AmdSbDispatcher - Dispatch Southbridge function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
AGESA_STATUS
AmdSbDispatcher (
  IN       VOID *pConfig
  )
{
  AGESA_STATUS Status;

#ifdef B1_IMAGE
  VOID *pAltImagePtr;
  CIM_IMAGE_ENTRY   AltImageEntry;
#endif

  UINT64  tdValue;
  tdValue = 0x32314130384253ULL;

#ifdef B1_IMAGE
  pAltImagePtr = NULL;
#endif
  Status = AGESA_UNSUPPORTED;

#ifdef B1_IMAGE
  if ((UINT32) (UINTN) (((AMD_CONFIG_PARAMS*)pConfig)->AltImageBasePtr) != 0xffffffff ) {
    if ( ((AMD_CONFIG_PARAMS*)pConfig)->AltImageBasePtr ) {
      pAltImagePtr = VerifyImage ( tdValue, (VOID*) (UINTN) ((AMD_CONFIG_PARAMS*)pConfig)->AltImageBasePtr);
    }
    if ( pAltImagePtr == NULL ) {
      pAltImagePtr = LocateImage ( tdValue );
    }
    if ( pAltImagePtr != NULL ) {
      ((AMD_CONFIG_PARAMS*)pConfig)->ImageBasePtr = (UINT32) (UINTN) pAltImagePtr;
      AltImageEntry = (CIM_IMAGE_ENTRY) (UINTN) ((UINT32) (UINTN) pAltImagePtr + (UINT32) (((AMD_IMAGE_HEADER*) (UINTN) pAltImagePtr)->EntryPointAddress));
      (*AltImageEntry) (pConfig);
      return Status;
    }
  }
#endif
  saveConfigPointer (pConfig);

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_POWERON_INIT ) {
    sbPowerOnInit ((AMDSBCFG*) pConfig);
  }

#ifndef B1_IMAGE
  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_BEFORE_PCI_INIT ) {
    sbBeforePciInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_AFTER_PCI_INIT ) {
    sbAfterPciInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_MID_POST_INIT ) {
    sbMidPostInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_LATE_POST_INIT ) {
    sbLatePost ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_BEFORE_PCI_RESTORE_INIT ) {
    sbBeforePciRestoreInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_AFTER_PCI_RESTORE_INIT ) {
    sbAfterPciRestoreInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_SMM_SERVICE ) {
    sbSmmService ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_SMM_ACPION ) {
    sbSmmAcpiOn ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_EC_FANCONTROL ) {
    sbECfancontrolservice((AMDSBCFG*)pConfig);;
  }
#endif
  return Status;
}

/**
 * LocateImage - Locate Southbridge CIMx module
 *
 *
 *
 * @param[in] Signature   Southbridge CIMx image signature.
 *
 */
VOID*
LocateImage (
  IN       UINT64 Signature
  )
{
  VOID  *Result;
  UINT32   ImagePtr;
  ImagePtr = 0xffffffff - (IMAGE_ALIGN - 1);

  while ( ImagePtr >= (0xfffffff - (NUM_IMAGE_LOCATION * IMAGE_ALIGN - 1)) ) {
#ifdef x64
    12346789
#else
    Result = VerifyImage (Signature, (VOID*) ImagePtr);
#endif
    if ( Result != NULL ) {
      return Result;
    }
    ImagePtr -= IMAGE_ALIGN;
  }
  return NULL;
}

/**
 * VerifyImage - Verify Southbridge CIMx module
 *
 *
 * @param[in] Signature   Southbridge CIMx image signature.
 * @param[in] ImagePtr    Southbridge CIMx image address.
 *
 */
VOID*
VerifyImage (
  IN       UINT64 Signature,
  IN       VOID* ImagePtr
  )
{
  UINT16 *TempImagePtr;
  UINT16 Sum;
  UINT32  i;
  Sum = 0;
  //if ( (*((UINT32*)ImagePtr) == 'DMA$' && ((CIMFILEHEADER*)ImagePtr)->CreatorID == Signature) ) { //gcc multi-character character constant warning
  if ( (*((UINT32*)ImagePtr) == 0x444d4124 && ((CIMFILEHEADER*)ImagePtr)->CreatorID == Signature) ) {//'DMA$'
    //GetImage Image size
    TempImagePtr = (UINT16*)ImagePtr;
    for ( i = 0; i < (((CIMFILEHEADER*)ImagePtr)->ImageSize); i += 2 ) {
      Sum = Sum + *TempImagePtr;
      TempImagePtr++;
    }
    if ( Sum == 0 ) {
      return ImagePtr;
    }
  }
  return NULL;
}

/**
 * saveConfigPointer - Verify Southbridge CIMx module
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID
saveConfigPointer (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8   dbReg;
  UINT8   i;
  UINT32   ddValue;

  ddValue =  (UINT32) (UINTN)pConfig;
  dbReg = SB_ECMOS_REG08;

  for ( i = 0; i <= 3; i++ ) {
    WriteIO (SB_IOMAP_REG72, AccWidthUint8, &dbReg);
    WriteIO (SB_IOMAP_REG73, AccWidthUint8, (UINT8*)&ddValue);
    ddValue = (ddValue >> 8);
    dbReg++;
  }
}
