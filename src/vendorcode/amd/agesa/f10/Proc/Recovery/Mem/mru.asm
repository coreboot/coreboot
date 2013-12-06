;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  $Workfile:: mu.asm   $ $Revision:: 443#$  $Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
; Description: Main memory controller system configuration for AGESA DDR 2
;
;
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
;============================================================================


    .XLIST
    .LIST
    .686p
    .MODEL FLAT
    .CODE
     ASSUME FS: NOTHING


; Define the calling convention used for the C library modules
;@attention - This should be in a central include file
CALLCONV    EQU     NEAR C


;===============================================================================
;MemRecUOutPort:
;
; Do a 32 Bit IO Out operation using edx.
; NOTE: This function will be obsolete in the future.
;
;             In: Port - port number
;                 Value - value to be written to port
;
;            Out:
;
;All registers preserved except for "Out:"
;===============================================================================
MemRecUOutPort PROC CALLCONV PUBLIC Port:DWORD, Value:DWORD
    pushad
    mov edx,Port
    mov eax,Value
    out dx,al
    popad
    ret
MemRecUOutPort ENDP



;----------------------------------------------------------------------------
; _MFENCE();
;
_MFENCE macro
    db  0Fh,0AEh,0F0h
    endm

;----------------------------------------------------------------------------
; _EXECFENCE();
;
_EXECFENCE macro
    out 0EDh,al             ;prevent speculative execution of following instructions
    endm

;===============================================================================
;MemRecUWrite1CL:
;
;   Write data from buffer to a system address
;
;             In: Address - System address to read from
;                 Pattern - pointer pattern.
;
;            Out:
;
;All registers preserved except for "Out:"
;===============================================================================
MemRecUWrite1CL PROC CALLCONV PUBLIC Address:DWORD, Pattern:NEAR PTR DWORD
        pushad
        push ds

        mov eax,Address
        push ss
        pop ds
        xor edx,edx
        mov edx, DWORD PTR Pattern
        mov esi,edx
        mov edx,16
        _EXECFENCE
        mov ecx,4
        @@:
        db 66h,0Fh,6Fh,06           ;MOVDQA xmm0,[esi]
        db 64h,66h,0Fh,0E7h,00      ;MOVNTDQ fs:[eax],xmm0  (xmm0 is 128 bits)
        add eax,edx
        add esi,edx
        loop @B

        pop ds
        popad
        ret
MemRecUWrite1CL ENDP

;===============================================================================
;MemRecURead1CL:
;
; Read one cacheline to buffer
;
;             In: Buffer - pointer buffer.
;               : Address - System address to read from
;
;            Out:
;
;All registers preserved except for "Out:"
;===============================================================================
MemRecURead1CL PROC CALLCONV PUBLIC Buffer:NEAR PTR DWORD, Address:DWORD

        pushad

        mov esi,Address
        xor edx,edx
        mov edx,DWORD PTR Buffer
        mov edi,edx
        mov ecx,64
        @@:
        mov al,fs:[esi]
        mov ss:[edi],al
        inc esi
        inc edi
        loop @B

        popad
        ret
MemRecURead1CL ENDP


;===============================================================================
;MemRecUFlushPattern:
;
; Flush one cache line
;
;             In:   Address - System address [31:0]
;            Out:
;
;All registers preserved except for "Out:"
;===============================================================================
MemRecUFlushPattern PROC CALLCONV PUBLIC Address:DWORD
        pushad
        mov eax,Address
        _EXECFENCE
                                        ;clflush fs:[eax]
        db  064h                        ;access relative to FS BASE prefix
        db  00Fh                        ;opcode
        db  0AEh                        ;opcode
        db  038h                        ;eax indirect addressing
        _MFENCE
        popad
        ret
MemRecUFlushPattern ENDP



    END

