/**
 * @file
 *
 * Southbridge IO access common routine
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
#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

/**< SbStall - Reserved  */
VOID
SbStall (
  IN       UINT32 uSec
  )
{
  UINT16 timerAddr;
  UINT32 startTime;
  UINT32 elapsedTime;

  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG64, AccWidthUint16, &timerAddr);
  if ( timerAddr == 0 ) {
    uSec = uSec / 2;
    while ( uSec != 0 ) {
      ReadIO (0x80, AccWidthUint8, (UINT8 *) (&startTime));
      uSec--;
    }
  } else {
    ReadIO (timerAddr, AccWidthUint32, &startTime);
    for ( ;; ) {
      ReadIO (timerAddr, AccWidthUint32, &elapsedTime);
      if ( elapsedTime < startTime ) {
        elapsedTime = elapsedTime + 0xFFFFFFFF - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }
      if ( (elapsedTime * 28 / 100) > uSec ) {
        break;
      }
    }
  }
}

/**< cimSbStall - Reserved  */
VOID
cimSbStall (
  IN       UINT32 uSec
  )
{
  UINT16 timerAddr;
  UINT32 startTime;
  UINT32 elapsedTime;

  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG64, AccWidthUint16, &timerAddr);
  if ( timerAddr == 0 ) {
    uSec = uSec / 2;
    while ( uSec != 0 ) {
      ReadIo8 (0x80);
      uSec--;
    }
  } else {
    startTime = ReadIo32 (timerAddr);
    for ( ;; ) {
      elapsedTime = ReadIo32 (timerAddr);
      if ( elapsedTime < startTime ) {
        elapsedTime = elapsedTime + 0xFFFFFFFF - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }
      if ( (elapsedTime * 28 / 100) > uSec ) {
        break;
      }
    }
  }
}

/**< SbReset - Reserved  */
VOID
SbReset ()
{
  RWIO (0xcf9, AccWidthUint8, 0x0, 0x06);
}

/**< outPort80 - Reserved  */
VOID
outPort80 (
  IN  UINT32 pcode
  )
{
  WriteIO (0x80, AccWidthUint8, &pcode);
  return;
}

/**< outPort1080 - Reserved  */
VOID
outPort1080 (
  IN  UINT32 pcode
  )
{
  WriteIo32 (0x1080, pcode);
  return;
}

/**< AmdSbCopyMem - Reserved  */
VOID
AmdSbCopyMem (
  IN      VOID*   pDest,
  IN      VOID*   pSource,
  IN      UINTN   Length
  )
{
  UINTN  i;
  UINT8  *Ptr;
  UINT8  *Source;
  Ptr = (UINT8*)pDest;
  Source = (UINT8*)pSource;
  for (i = 0; i < Length; i++) {
    *Ptr = *Source;
    Source++;
    Ptr++;
  }
}

/** GetRomSigPtr - Reserved **/
VOID*
GetRomSigPtr (
  IN UINTN* RomSigPtr
  )
{
  UINTN  RomPtr;
  UINT32 RomSig;
  RomPtr = 0xFFF20000;  // 1M
  ReadMEM ((UINT32)RomPtr, AccWidthUint32, &RomSig);
  if ( RomSig != 0x55AA55AA ) {
    RomPtr = 0xFFE20000; //2M
    ReadMEM ((UINT32)RomPtr, AccWidthUint32, &RomSig);
    if ( RomSig != 0x55AA55AA ) {
      RomPtr = 0xFFC20000; //4M
      ReadMEM ((UINT32)RomPtr, AccWidthUint32, &RomSig);
      if ( RomSig != 0x55AA55AA ) {
        RomPtr = 0xFF820000; //8M
        ReadMEM ((UINT32)RomPtr, AccWidthUint32, &RomSig);
        if ( RomSig != 0x55AA55AA ) {
          RomPtr = 0xFF020000; //16M
          ReadMEM ((UINT32)RomPtr, AccWidthUint32, &RomSig);
          if ( RomSig != 0x55AA55AA ) {
            RomPtr = 0x0; // not found
          }
        }
      }
    }
  }
  *RomSigPtr = RomPtr;
  return RomSigPtr;
}

VOID
RWXhciIndReg (
  IN       UINT32 Index,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32  IndReg;
  WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x48, AccWidthUint32, &Index);
  ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
  IndReg &= AndMask;
  IndReg |= OrMask;
  WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);

#ifndef XHCI_SUPPORT_ONE_CONTROLLER
  WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x48, AccWidthUint32, &Index);
  ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
  IndReg &= AndMask;
  IndReg |= OrMask;
  WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
#endif
}

VOID
RWXhci0IndReg (
  IN       UINT32 Index,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32  IndReg;
  WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x48, AccWidthUint32, &Index);
  ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
  IndReg &= AndMask;
  IndReg |= OrMask;
  WritePCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
}

VOID
RWXhci1IndReg (
  IN       UINT32 Index,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask
  )
{
  UINT32  IndReg;
  WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x48, AccWidthUint32, &Index);
  ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
  IndReg &= AndMask;
  IndReg |= OrMask;
  WritePCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x4C, AccWidthUint32, &IndReg);
}

VOID
AcLossControl (
  IN       UINT8 AcLossControlValue
  )
{
  AcLossControlValue &= 0x03;
  AcLossControlValue |= BIT2;
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG5B, AccWidthUint8, 0xF0, AcLossControlValue);
}

VOID
SbVgaInit (
  VOID
  )
{
  // OBS194249 Cobia_Nutmeg_DP-VGA Electrical SI validation_Lower RGB Luminance level BGADJ=0x1F & DACADJ=0x1B
  //
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, 0xff, BIT5 );
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8, AccWidthUint8, 0x00, 0x17 );
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD9, AccWidthUint8, 0x00, ((BGADJ << 2) + (((DACADJ & 0xf0) >> 4) & 0x3)));
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD8, AccWidthUint8, 0x00, 0x16 );
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD9, AccWidthUint8, 0x0f, ((DACADJ & 0x0f) << 4));

  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x00))) = (0x08 << 4) + (UINT8) ((EFUS_DAC_ADJUSTMENT_CONTROL >> 16) & 0xff);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x01))) = (UINT8) ((EFUS_DAC_ADJUSTMENT_CONTROL >> 8) & 0xff);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x02))) = (UINT8) ((EFUS_DAC_ADJUSTMENT_CONTROL >> 0) & 0xff);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x03))) = (UINT8) (0x03);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x04))) = (UINT8) (((EFUS_DAC_ADJUSTMENT_CONTROL_DATA) >> 0) & 0xff);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x05))) = (UINT8) (((EFUS_DAC_ADJUSTMENT_CONTROL_DATA) >> 8) & 0xff);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x06))) = (UINT8) (((EFUS_DAC_ADJUSTMENT_CONTROL_DATA) >> 16) & 0xff);
  *((UINT8*) ((UINTN)(PKT_DATA_REG + 0x07))) = (UINT8) (((EFUS_DAC_ADJUSTMENT_CONTROL_DATA) >> 24) & 0xff);
  *((UINT8*) ((UINTN)(PKT_LEN_REG))) = 0x08;
  *((UINT8*) ((UINTN)(PKT_CTRL_REG))) = 0x01;
  //RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC4, AccWidthUint8, ~(BIT5), 0 );
}

VOID
RecordSbConfigPtr (
  IN       UINT32 SbConfigPtr
  )
{
  RWMEM (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x08, AccWidthUint8, 0, (UINT8) ((SbConfigPtr >> 0) & 0xFF) );
  RWMEM (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x09, AccWidthUint8, 0, (UINT8) ((SbConfigPtr >> 8) & 0xFF) );
  RWMEM (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0A, AccWidthUint8, 0, (UINT8) ((SbConfigPtr >> 16) & 0xFF) );
  RWMEM (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0B, AccWidthUint8, 0, (UINT8) ((SbConfigPtr >> 24) & 0xFF) );
}

VOID
SbGpioInit (
  IN       SB_GPIO_INIT_ENTRY *SbGpioInitTable
  )
{
  while ( SbGpioInitTable->GpioPin < 0xFF ) {
    RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SbGpioInitTable->GpioPin, AccWidthUint8, 0, SbGpioInitTable->GpioMux );
    RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SbGpioInitTable->GpioPin, AccWidthUint8, ~ (BIT5 + BIT6), ((SbGpioInitTable->GpioOutEnB + (SbGpioInitTable->GpioOut << 1)) << 5) );
    SbGpioInitTable ++;
  }
}

VOID
SbGpioControl (
  IN       SB_GPIO_CONTROL_ENTRY *SbGpio
  )
{
  UINT8 GpioCurrent;
  ReadMEM (ACPI_MMIO_BASE + GPIO_BASE + SbGpio->GpioPin, AccWidthUint8, &GpioCurrent );
  if ((GpioCurrent & BIT5) == 0) {
    RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SbGpio->GpioPin, AccWidthUint8, ~ BIT6, (SbGpio->GpioControl << 6) );
  }
  GpioCurrent &= BIT7;
  SbGpio->GpioControl = GpioCurrent >> 7;
}

VOID
SbFlashUsbSmi (
  VOID
  )
{
  if ( ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED) & (BIT4) ) {
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED) &= ~ (BIT4);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED) |= (BIT4);
  }
}

VOID
SbEnableUsbIrq1Irq12ToPicApic (
  VOID
  )
{
  ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGED) |= (BIT1);
}

