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

/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;       its contributors may be used to endorse or promote products derived 
;       from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; 
;*********************************************************************************/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

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
  AGESA_STATUS      Status;
  UINT64            tdValue;

#ifdef B1_IMAGE
  VOID              *pAltImagePtr;
  CIM_IMAGE_ENTRY   AltImageEntry;

  pAltImagePtr = NULL;
#endif

  Status = AGESA_UNSUPPORTED;
  tdValue = 0x313141324E4448ull;

//#if CIM_DEBUG
//  InitSerialOut ();
//#endif

  Status = AGESA_UNSUPPORTED;
  TRACE ((DMSG_SB_TRACE, "CIM - Hudson-2 Entry\n"));

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
    sbECfancontrolservice ((AMDSBCFG*)pConfig);
  }
#endif
  TRACE ((DMSG_SB_TRACE, "CIMx - SB Exit\n"));
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
    Result = VerifyImage (Signature, (VOID*) (intptr_t) ImagePtr);
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
//  if ( (*((UINT32*)ImagePtr) == 'DMA$' && ((CIMFILEHEADER*)ImagePtr)->CreatorID == Signature) ) {
  if ( (*((UINT32*)ImagePtr) == Int32FromChar('D', 'M', 'A', '$') && ((CIMFILEHEADER*)ImagePtr)->CreatorID == Signature) ) {
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
