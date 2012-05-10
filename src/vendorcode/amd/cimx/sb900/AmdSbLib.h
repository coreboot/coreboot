/**
 * @file
 *
 * Southbridge IO access common routine define file
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


//AMDSBLIB Routines

/*--------------------------- Documentation Pages ---------------------------*/
/**< SbStall - Reserved  */
void  SbStall (IN unsigned int uSec);

/**< cimSbStall - Reserved  */
void  cimSbStall (IN unsigned int uSec);

/**< SbReset - Reserved  */
void SbReset (void);

/**< outPort80 - Reserved  */
void outPort80 (IN unsigned int pcode);

/**< outPort80 - Reserved  */
void outPort1080 (IN unsigned int pcode);

/**< getEfuseStatue - Reserved  */
void getEfuseStatus (IN void* Value);

/**< getEfuseByte - Reserved  */
unsigned char getEfuseByte (IN unsigned char Index);

/**< AmdSbDispatcher - Reserved  */
AGESA_STATUS  AmdSbDispatcher (IN void *pConfig);

/**< AmdSbCopyMem - Reserved  */
void AmdSbCopyMem (IN void* pDest, IN void* pSource, IN unsigned int Length);

/**< GetRomSigPtr - Reserved  */
void* GetRomSigPtr (IN unsigned int* RomSigPtr);

/**< RWXhciIndReg - Reserved  */
void RWXhciIndReg (IN unsigned int Index, IN unsigned int AndMask, IN unsigned int OrMask);

/**< RWXhciIndReg - Reserved  */
void RWXhci0IndReg (IN unsigned int Index, IN unsigned int AndMask, IN unsigned int OrMask);

/**< RWXhciIndReg - Reserved  */
void RWXhci1IndReg (IN unsigned int Index, IN unsigned int AndMask, IN unsigned int OrMask);

/**< AcLossControl - Reserved  */
void AcLossControl (IN unsigned char AcLossControlValue);

/**< SbVgaInit - Reserved  */
void SbVgaInit (void);

/**< RecordSbConfigPtr - Reserved  */
void RecordSbConfigPtr (IN unsigned int SbConfigPtr);

/**< SbGpioInit - Reserved  */
void
SbGpioInit (
  IN       SB_GPIO_INIT_ENTRY *SbGpioInitTable
  );

/**< SbGpioControl - Reserved  */
void
SbGpioControl (
  IN       SB_GPIO_CONTROL_ENTRY *SbGpio
  );

/**< SbFlashUsbSmi - Reserved  */
void SbFlashUsbSmi (void);

/**< SbEnableUsbIrq1Irq12ToPicApic - Reserved  */
void SbEnableUsbIrq1Irq12ToPicApic (void);

