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
 *
 ***************************************************************************/


#include "Platform.h"

void DispatcherEntry(void *pConfig){

#ifdef  B1_IMAGE
        void    *pB2ImagePtr = NULL;
        CIM_IMAGE_ENTRY pB2ImageEntry;
#endif

//#if CIM_DEBUG
//        InitSerialOut();
//#endif

        TRACE((DMSG_SB_TRACE, "CIM - SB700 Entry\n"));

#ifdef  B1_IMAGE
        if ((UINT32)(((STDCFG*)pConfig)->pB2ImageBase) != 0xffffffff){
                if (((STDCFG*)pConfig)->pB2ImageBase)
                        pB2ImagePtr = CheckImage('007S',(void*)((STDCFG*)pConfig)->pB2ImageBase);
                if (pB2ImagePtr == NULL)
                        pB2ImagePtr = LocateImage('007S');
                if (pB2ImagePtr!=NULL){
                        TRACE((DMSG_SB_TRACE, "CIM - SB700 Redirect to B2 Image\n"));
                        ((STDCFG*)pConfig)->pImageBase = (UINT32)pB2ImagePtr;
                        pB2ImageEntry = (CIM_IMAGE_ENTRY)(*((UINT32*)pB2ImagePtr+1) + (UINT32)pB2ImagePtr);
                        (*pB2ImageEntry)(pConfig);
                        return;
                }
        }
#endif
        saveConfigPointer(pConfig);

        if      (((STDCFG*)pConfig)->Func == SB_POWERON_INIT)
                sbPowerOnInit((AMDSBCFG*)pConfig);

#ifndef B1_IMAGE
        if      (((STDCFG*)pConfig)->Func == SB_BEFORE_PCI_INIT)
                sbBeforePciInit((AMDSBCFG*)pConfig);
        if      (((STDCFG*)pConfig)->Func == SB_AFTER_PCI_INIT)
                sbAfterPciInit((AMDSBCFG*)pConfig);
        if      (((STDCFG*)pConfig)->Func == SB_LATE_POST_INIT)
                sbLatePost((AMDSBCFG*)pConfig);
        if      (((STDCFG*)pConfig)->Func == SB_BEFORE_PCI_RESTORE_INIT)
                sbBeforePciRestoreInit((AMDSBCFG*)pConfig);
        if      (((STDCFG*)pConfig)->Func == SB_AFTER_PCI_RESTORE_INIT)
                sbAfterPciRestoreInit((AMDSBCFG*)pConfig);
        if      (((STDCFG*)pConfig)->Func == SB_SMM_SERVICE)
        {
                // sbSmmService((AMDSBCFG*)pConfig);
        }
        if      (((STDCFG*)pConfig)->Func == SB_SMM_ACPION)
                sbSmmAcpiOn((AMDSBCFG*)pConfig);
#endif
        TRACE((DMSG_SB_TRACE, "CIMx - SB Exit\n"));
}


void* LocateImage(UINT32 Signature){
        void    *Result;
        UINT8   *ImagePtr = (UINT8*)(0xffffffff - (IMAGE_ALIGN-1));
        while   ((UINT32)ImagePtr>=(0xfffffff - (NUM_IMAGE_LOCATION*IMAGE_ALIGN -1))){
                Result = CheckImage(Signature,(void*)ImagePtr);
                if (Result != NULL)
                        return Result;
                ImagePtr -= IMAGE_ALIGN;
        }
        return NULL;
}


void* CheckImage(UINT32 Signature, void* ImagePtr){
        UINT8   *TempImagePtr;
        UINT8   Sum = 0;
        UINT32  i;
//        if      ((*((UINT32*)ImagePtr) == 'ITA$' && ((CIMFILEHEADER*)ImagePtr)->ModuleLogo == Signature)){
        if      ((*((UINT32*)ImagePtr) == Int32FromChar ('$', 'A', 'T', 'I')) && (((CIMFILEHEADER*)ImagePtr)->ModuleLogo == Signature)){
                //GetImage Image size
                TempImagePtr = (UINT8*)ImagePtr;
                for (i=0;i<(((CIMFILEHEADER*)ImagePtr)->ImageSize);i++){
                        Sum += *TempImagePtr;
                        TempImagePtr++;
                }
                if      (Sum == 0)
                        return ImagePtr;
        }
        return NULL;
}


UINT32 GetPciebase(){
        AMDSBCFG* Result;
        Result = getConfigPointer();
        return Result->StdHeader.pPcieBase;
}


void saveConfigPointer(AMDSBCFG* pConfig){
        UINT8   dbReg, i;
        UINT32  ddValue;

        ddValue =  ((UINT32) pConfig);
        dbReg = SB_ECMOS_REG08;

        for (i=0; i<=3; i++){
                WriteIO(SB_IOMAP_REG72, AccWidthUint8, &dbReg);
                WriteIO(SB_IOMAP_REG73, AccWidthUint8, (UINT8 *)&ddValue);
                ddValue >>= 8;
                dbReg++;
        }
}


AMDSBCFG* getConfigPointer(){
        UINT8   dbReg, dbValue, i;
        UINT32  ddValue=0;

        dbReg = SB_ECMOS_REG08;
        for (i=0; i<=3; i++){
                WriteIO(SB_IOMAP_REG72, AccWidthUint8, &dbReg);
                ReadIO(SB_IOMAP_REG73, AccWidthUint8, &dbValue);
                ddValue |= (dbValue<<(i*8));
                dbReg++;
        }
        return( (AMDSBCFG*) ddValue);
}

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
  AGESA_STATUS Status = AGESA_SUCCESS;

  saveConfigPointer (pConfig);

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_POWERON_INIT ) {
    sbPowerOnInit ((AMDSBCFG*) pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_BEFORE_PCI_INIT ) {
    sbBeforePciInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_AFTER_PCI_INIT ) {
    sbAfterPciInit ((AMDSBCFG*)pConfig);
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

  return Status;
}
