;/**
; * @file
; *
; * Ids Assembly library 32bit
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  IDS
; * @e \$Revision: 14305 $   @e \$Date: 2009-05-24 02:20:55 +0800 (Sun, 24 May 2009) $
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

.586p
.model  flat
ASSUME FS:NOTHING
.code
public IdsDelay
IdsDelay  PROC  NEAR C USES EAX EDX
Local targetedx:dword, targeteax:dword
  rdtsc
;set target time
  add eax,1500000000
  adc edx,0
  mov targetedx,edx
  mov targeteax,eax

  rdtsc
;set "Si!=0" skip below loop
  .while(1)
    .if(si != 0)
      jmp delay_exit
    .endif
    .if(edx > targetedx)
      jmp delay_exit
    .elseif (edx == targetedx)
      .if(eax > targeteax)
        jmp delay_exit
      .endif
    .endif
    rdtsc
  .endw
delay_exit:
  ret
IdsDelay    ENDP
;/*++
;
;Routine Description:
;
;  IdsErrorStop -- Function for Assert
;
;Arguments:
;  Filecode
;
;Returns:
;
;  None
;
;--*/
public IdsErrorStop
IdsErrorStop PROC NEAR  C filecode:dword
local tmpebx:dword,tmpedx:dword
  pushad

  mov     si,0                      ; Si is used as control flag, "Si!=0" skip postcode loop
; send debug port 1st, then fire SimNow breakpoint
  mov     ax,  0deadh
  out     0e0h, ax
  mov     eax, filecode
  out     84h, eax
  mov     eax, 0BACCD00Bh         ; Backdoor in SimNow
  mov     ebx, 2                  ; Select breakpoint feature
  cpuid

  mov   ebx,0dead0000h
  mov   edx,filecode
  ror   edx,16
  mov   bx,dx
  mov   dx,0
;ebx:edx = deadxxxxyyyy0000 xxxx is the filecode yyyy is the line num
  mov   tmpebx,ebx
  mov   tmpedx,edx

  xor   eax,eax
  mov   cl,6

  .while((cl != 0) && (si == 0))
    .if(cl <= 2)
      shld  eax,edx,8
      shl   edx,8
    .else
      shld  eax,ebx,8
      shl   ebx,8
    .endif

    out   80h,eax
    call IdsDelay
    dec cl
    .if(cl == 0)
      mov   cl,6
      mov   ebx,tmpebx
      mov   edx,tmpedx
    .endif
  .endw

  popad
  xor eax,eax
  ret
IdsErrorStop  endp


END
