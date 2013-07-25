;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  $Workfile:: mu.asm   $ $Revision:: 841#$  $Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
; Description: Main memory controller system configuration for AGESA
;
;
;*****************************************************************************
;
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
;memUOutPort:
;
; Do a 32 Bit IO Out operation using edx.
; NOTE: This function will be obsolete in the future.
;
;             In: Port  - port number
;                 Value - value to be written
;
;            Out:
;
; All registers preserved.
;===============================================================================
MemUOutPort PROC CALLCONV PUBLIC Port:DWORD, Value:DWORD
    pushad
    mov edx,Port
    mov eax,Value
    out dx,al
    popad
    ret
MemUOutPort ENDP


;----------------------------------------------------------------------------
; _SFENCE();
;
_SFENCE macro
    db  0Fh,0AEh,0F8h
    endm

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
;MemUWriteCachelines:
;   Write a test pattern to DRAM
;
;             In: Pattern   - pointer to the write pattern
;                 Address   - Physical address to be read
;                 ClCount   - number of cachelines to be read
;            Out:
;
;All registers preserved.
;===============================================================================
MemUWriteCachelines PROC CALLCONV PUBLIC Address:DWORD, Pattern:NEAR PTR DWORD, ClCount:WORD
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
        xor ecx, ecx
        mov cx,ClCount
        shl ecx,2
        @@:
        db 66h, 0Fh,6Fh,06           ;MOVDQA xmm0,[esi]
        db 64h, 66h, 0Fh,0E7h,00      ;MOVNTDQ fs:[eax],xmm0  (xmm0 is 128 bits)
        add eax,edx
        add esi,edx
        loop @B

        pop ds
        popad
        ret
MemUWriteCachelines ENDP

;===============================================================================
;MemUReadCachelines:
;
; Read a pattern of 72 bit times (per DQ), to test dram functionality.  The
;pattern is a stress pattern which exercises both ISI and crosstalk.  The number
;of cache lines to fill is dependent on DCT width mode and burstlength.
;
;             In: Buffer    - pointer to a buffer where read data will be stored
;                 Address   - Physical address to be read
;                 ClCount   - number of cachelines to be read
;            Out:
;
;All registers preserved.
;===============================================================================
MemUReadCachelines PROC CALLCONV PUBLIC Buffer:NEAR PTR DWORD, Address:DWORD, ClCount:WORD
LOCAL Count:BYTE
        pushad
        ; First, issue continuous dummy reads to fill up the cache
        mov eax,Address
        .if (ClCount > 18)
            mov cx,ClCount
            shr cx,4
            mov Count,cl
            .while (Count != 0)
                push eax
                mov edi,eax
                add edi,128                     ;bias value (to account for signed displacement)
                                                ;clflush opcode=0F AE /7
                mov esi,edi
                mov ebx,esi
                mov ecx,esi
                mov edx,esi
                add edi,4*64                    ;TestAddr+4 cache lines
                add ebx,8*64                    ;TestAddr+8 cache lines
                add ecx,12*64                   ;TestAddr+12 cache lines
                add edx,16*64                   ;TestAddr+16 cache lines
                sub edx,128
                _EXECFENCE
                mov eax,fs:[esi-128]            ;TestAddr
                _MFENCE
                mov eax,fs:[esi-64]             ;TestAddr+1 cache line
                _MFENCE
                mov eax,fs:[esi]                ;TestAddr+2 cache lines
                _MFENCE
                mov eax,fs:[esi+64]             ;TestAddr+3 cache lines
                _MFENCE
                mov eax,fs:[edi-128]            ;TestAddr+4 cache lines
                _MFENCE
                mov eax,fs:[edi-64]             ;TestAddr+5 cache lines
                _MFENCE
                mov eax,fs:[edi]                ;TestAddr+6 cache lines
                _MFENCE
                mov eax,fs:[edi+64]             ;TestAddr+7 cache lines
                _MFENCE
                mov eax,fs:[ebx-128]            ;TestAddr+8 cache lines
                _MFENCE
                mov eax,fs:[ebx-64]             ;TestAddr+9 cache lines
                _MFENCE
                mov eax,fs:[ebx]                ;TestAddr+10 cache lines
                _MFENCE
                mov eax,fs:[ebx+64]             ;TestAddr+11 cache lines
                _MFENCE
                mov eax,fs:[ecx-128]            ;TestAddr+12 cache lines
                _MFENCE
                mov eax,fs:[ecx-64]             ;TestAddr+13 cache lines
                _MFENCE
                mov eax,fs:[ecx]                ;TestAddr+14 cache lines
                _MFENCE
                mov eax,fs:[ecx+64]             ;TestAddr+15 cache lines
                _MFENCE
                pop eax
                add eax,(16*64)                 ;Next 16CL
                dec Count
            .endw
        .else
            mov edi,eax
            add edi,128                     ;bias value (to account for signed displacement)
                                            ;clflush opcode=0F AE /7
            mov esi,edi
            mov ebx,esi
            mov ecx,esi
            mov edx,esi
            add edi,4*64                    ;TestAddr+4 cache lines
            add ebx,8*64                    ;TestAddr+8 cache lines
            add ecx,12*64                   ;TestAddr+12 cache lines
            add edx,16*64                   ;TestAddr+16 cache lines
            sub edx,128
            .if(ClCount == 1)
                _MFENCE
                mov eax,fs:[esi-128]            ;TestAddr
                _MFENCE
            .elseif(ClCount == 3)
                _EXECFENCE
                mov eax,fs:[esi-128]            ;TestAddr
                _MFENCE
                mov eax,fs:[esi-64]             ;TestAddr+1 cache line
                _MFENCE
                mov eax,fs:[esi]                ;TestAddr+2 cache lines
                _MFENCE
            .elseif(ClCount == 6)
                _EXECFENCE
                mov eax,fs:[esi-128]            ;TestAddr
                _MFENCE
                mov eax,fs:[esi-64]             ;TestAddr+1 cache line
                _MFENCE
                mov eax,fs:[esi]                ;TestAddr+2 cache lines
                _MFENCE
                mov eax,fs:[esi+64]             ;TestAddr+3 cache lines
                _MFENCE
                mov eax,fs:[edi-128]            ;TestAddr+4 cache lines
                _MFENCE
                mov eax,fs:[edi-64]             ;TestAddr+5 cache lines
                _MFENCE
            .elseif(ClCount == 9)
                _EXECFENCE
                mov eax,fs:[esi-128]            ;TestAddr
                _MFENCE
                mov eax,fs:[esi-64]             ;TestAddr+1 cache line
                _MFENCE
                mov eax,fs:[esi]                ;TestAddr+2 cache lines
                _MFENCE
                mov eax,fs:[esi+64]             ;TestAddr+3 cache lines
                _MFENCE
                mov eax,fs:[edi-128]            ;TestAddr+4 cache lines
                _MFENCE
                mov eax,fs:[edi-64]             ;TestAddr+5 cache lines
                _MFENCE
                mov eax,fs:[edi]                ;TestAddr+6 cache lines
                _MFENCE
                mov eax,fs:[edi+64]             ;TestAddr+7 cache lines
                _MFENCE
                mov eax,fs:[ebx-128]            ;TestAddr+8 cache lines
                _MFENCE
            .elseif(ClCount == 18)
                _EXECFENCE
                mov eax,fs:[esi-128]            ;TestAddr
                _MFENCE
                mov eax,fs:[esi-64]             ;TestAddr+1 cache line
                _MFENCE
                mov eax,fs:[esi]                ;TestAddr+2 cache lines
                _MFENCE
                mov eax,fs:[esi+64]             ;TestAddr+3 cache lines
                _MFENCE
                mov eax,fs:[edi-128]            ;TestAddr+4 cache lines
                _MFENCE
                mov eax,fs:[edi-64]             ;TestAddr+5 cache lines
                _MFENCE
                mov eax,fs:[edi]                ;TestAddr+6 cache lines
                _MFENCE
                mov eax,fs:[edi+64]             ;TestAddr+7 cache lines
                _MFENCE
                mov eax,fs:[ebx-128]            ;TestAddr+8 cache lines
                _MFENCE
                mov eax,fs:[ebx-64]             ;TestAddr+9 cache lines
                _MFENCE
                mov eax,fs:[ebx]                ;TestAddr+10 cache lines
                _MFENCE
                mov eax,fs:[ebx+64]             ;TestAddr+11 cache lines
                _MFENCE
                mov eax,fs:[ecx-128]            ;TestAddr+12 cache lines
                _MFENCE
                mov eax,fs:[ecx-64]             ;TestAddr+13 cache lines
                _MFENCE
                mov eax,fs:[ecx]                ;TestAddr+14 cache lines
                _MFENCE
                mov eax,fs:[ecx+64]             ;TestAddr+15 cache lines
                _MFENCE
                mov eax,fs:[edx]                ;TestAddr+16 cache lines
                _MFENCE
                mov eax,fs:[edx+64]             ;TestAddr+17 cache lines
                _MFENCE
            .endif
        .endif
        _MFENCE

        ; Then, copy data to buffer
        mov esi,Address
        xor edx,edx
        mov edx,DWORD PTR Buffer
        mov edi,edx
        xor ecx, ecx
        mov cx,ClCount
        shl ecx,6
        @@:
        mov al,fs:[esi]
        mov ss:[edi],al
        inc esi
        inc edi
        loop @B

        popad
        ret
MemUReadCachelines ENDP

;===============================================================================
;MemUDummyCLRead:
;
;   Perform a single cache line read from a given physical address.
;
;             In: Address   - Physical address to be read
;                 ClCount   - number of cachelines to be read
;            Out:
;
;All registers preserved.
;===============================================================================
MemUDummyCLRead PROC CALLCONV PUBLIC Address:DWORD
    _SFENCE
    pushad
    mov eax,Address
    mov dl,fs:[eax]
    popad
    ret
MemUDummyCLRead ENDP

;===============================================================================
;MemUFlushPattern:
;
; Flush a pattern of 72 bit times (per DQ) from cache.  This procedure is used
;to ensure cache miss on the next read training.
;
;             In: Address   - Physical address to be flushed
;                 ClCount   - number of cachelines to be flushed
;            Out:
;
;All registers preserved.
;===============================================================================
MemUFlushPattern PROC CALLCONV PUBLIC Address:DWORD, ClCount:WORD
        pushad
        mov edi,Address
        movzx ecx,ClCount
        @@:
        _MFENCE                     ; Force strong ordering of clflush
        db  64h,0Fh,0AEh,3Fh        ; MemUClFlush fs:[edi]
        _MFENCE
        add edi,64
        loop @B
        popad
        ret
MemUFlushPattern ENDP


;===============================================================================
;MemUGetWrLvNblErr:
;   Read ClCount number of cachelines then return the bitmap that indicates
;   the write leveling result of each byte lane.
;
;   IN:     ErrBitmap - pointer to a DWORD that will be assigned with WL result
;           Address   - Physical address to be sampled
;           ClCount   - number of cachelines to be read
;
;   OUT:    ErrBitmap - WL result
;
;All registers preserved
;===============================================================================
MemUGetWrLvNblErr PROC CALLCONV PUBLIC ErrBitmap:NEAR PTR DWORD, Address:DWORD, ClCount:WORD
LOCAL ZeroCount[32]:WORD

        pushad
        mov esi,Address
        _EXECFENCE
    ;Cache fill
        movzx ecx,ClCount
        @@:
        mov eax,fs:[esi]
        add esi,64
        loop @B
        _MFENCE

    ; Then, count the number of 0's
        ;push es
        ;push ss
        ;pop es
        lea edi,ZeroCount
        mov cx,SIZEOF ZeroCount
        mov al,0
        rep stosb
        ;pop es

        mov esi,Address
        lea edi,ZeroCount
        mov cx,ClCount
        shl cx,6
        .while(cx > 0)
            mov al,fs:[esi]
            test al,00Fh        ;check lower nibble
            .if(ZERO?)
                inc WORD PTR [edi]
            .endif
            add edi,2
            test al,0F0h        ;check upper nibble
            .if(ZERO?)
                inc WORD PTR [edi]
            .endif
            add edi,2
            inc esi
            dec cx
            test cx,07h
            .if(ZERO?)
                sub edi,(16*2)
                sub cx,8
                add esi,8
            .endif
        .endw

    ; Then, average and compress data to error bits
        lea esi,ZeroCount
        mov dx,ClCount
        shl dx,1
        xor eax,eax
        xor ecx,ecx
        mov cl,0
        .while(cl<16)
            .if(WORD PTR [esi] < dx)
                bts eax,ecx
            .endif
            add esi,2
            inc cl
        .endw
        xor edx,edx
        mov dx,WORD PTR ErrBitmap
        mov [edx], ax

        popad
        ret
MemUGetWrLvNblErr ENDP

;===============================================================================
;AlignPointerTo16Byte:
;   Modifies BufferPtr to be 16 byte aligned
;
;             In: BufferPtrPtr - Pointer to buffer pointer
;            Out: BufferPtrPtr - Pointer to buffer pointer that has been 16 byte aligned
;
;All registers preserved.
;===============================================================================
AlignPointerTo16Byte PROC CALLCONV PUBLIC BufferPtrPtr:NEAR PTR DWORD
        push edx
        push eax
        mov edx, BufferPtrPtr
        mov eax, [edx]
        add eax, 16
        and ax, 0FFF0h
        mov [edx], eax
        pop eax
        pop edx
        ret
AlignPointerTo16Byte ENDP

;===============================================================================
;MemUMFenceInstr:
;   Serialize instruction
;
;             In:
;            Out:
;
;All registers preserved.
;===============================================================================
MemUMFenceInstr PROC CALLCONV PUBLIC
        _MFENCE
        ret
MemUMFenceInstr ENDP

    END

