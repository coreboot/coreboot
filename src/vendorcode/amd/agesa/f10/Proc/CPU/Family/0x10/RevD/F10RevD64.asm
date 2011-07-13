;/**
; * @file
; *
; * AGESA Family 10h Revision D support routines.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  CPU/F10
; * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
; */
;*****************************************************************************
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
;*****************************************************************************

    .XLIST
    .LIST

;===============================================
;===============================================
;==
;== M E M O R Y   P R E S E N T   S E G M E N T
;==
;===============================================
;===============================================
    .CODE

;======================================================================
; F10RevDProbeFilterCritical:  Performs critical sequence for probe
;                              filter initialization.
;
;   In:
;       PciAddress      Full PCI address of the node to init
;       PciRegister     Current value of F3x1D4
;
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
PUBLIC F10RevDProbeFilterCritical
F10RevDProbeFilterCritical PROC

  push   rax
  push   rcx
  push   rdx
  push   rsi
  push   rdi

  mov    esi, ecx
  mov    edi, edx

  mov    ecx, 0C001001Fh
  rdmsr
  push   rax
  push   rcx
  push   rdx
  or     dh, 40h
  wrmsr

  mov    eax, 810003D4h

  mov    ecx, edi
  mov    edx, esi

  shr    edx, 4
  and    dh, 0F8h
  or     ah, dh

  or     cl, 2
  mfence

  mov    dx, 0CF8h        ; Set Reg Config Space
  mfence

  out    dx, eax
  mfence

  mov    dl, 0FCh         ; Set DX to Pci Config Data
  mov    eax, ecx         ;Set config Reg data
  mfence

  out    dx, eax          ; move data to return position
  mfence

  pop    rdx
  pop    rcx
  pop    rax
  wrmsr

  pop    rdi
  pop    rsi
  pop    rdx
  pop    rcx
  pop    rax
  ret

F10RevDProbeFilterCritical ENDP

END
