;/**
; * @file
; *
; * Agesa library 64bit
; *
; * Contains AMD AGESA Library
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Lib
; * @e \$Revision: 17071 $   @e \$Date: 2009-07-30 10:13:11 -0700 (Thu, 30 Jul 2009) $
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

.code
;/*++

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write IO byte
; *
; *  @param[in]   CX    IO port address
; *  @param[in]   DL    IO port Value
; */

PUBLIC  WriteIo8
WriteIo8        PROC
        mov     al, dl
        mov     dx, cx
        out     dx, al
        ret
WriteIo8        ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write IO word
; *
; *  @param[in]   CX      IO port address
; *  @param[in]   DX      IO port Value
; */
PUBLIC  WriteIo16
WriteIo16       PROC
        mov     ax, dx
        mov     dx, cx
        out     dx, ax
        ret
WriteIo16       ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write IO dword
; *
; *  @param[in]   CX      IO port address
; *  @param[in]   EDX     IO port Value
; */

PUBLIC WriteIo32
WriteIo32       PROC
        mov     eax, edx
        mov     dx, cx
        out     dx, eax
        ret
WriteIo32       ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read IO byte
; *
; *  @param[in] CX  IO port address
; *  @retval    AL  IO port Value
; */
PUBLIC ReadIo8
ReadIo8 PROC
        mov     dx, cx
        in      al, dx
        ret
ReadIo8 ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read IO word
; *
; *  @param[in]   CX  IO port address
; *  @retval      AX  IO port Value
; */
PUBLIC ReadIo16
ReadIo16        PROC
        mov     dx, cx
        in      ax, dx
        ret
ReadIo16        ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read IO dword
; *
; *  @param[in]   CX  IO port address
; *  @retval      EAX IO port Value
; */
PUBLIC ReadIo32
ReadIo32        PROC
        mov     dx, cx
        in      eax, dx
        ret
ReadIo32        ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read MSR
; *
; *  @param[in]  RCX      MSR Address
; *  @param[in]  RDX      Pointer to data
; *  @param[in]  R8D      ConfigPtr (Optional)
; */
PUBLIC LibAmdMsrRead
LibAmdMsrRead  PROC
    push rsi
    mov     rsi, rdx
    rdmsr
    mov     [rsi], eax
    mov     [rsi+4], edx
    pop rsi
    ret
LibAmdMsrRead  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write MSR
; *
; *  @param[in]  RCX        MSR Address
; *  @param[in]  RDX        Pointer to data
; *  @param[in]  R8D        ConfigPtr  (Optional)
; */
PUBLIC LibAmdMsrWrite
LibAmdMsrWrite                PROC
    push rsi
    mov rsi, rdx
    mov eax, [rsi]
    and rax, 0ffffffffh
    mov edx, [rsi+4]
    and rdx, 0ffffffffh
    wrmsr
    pop rsi
    ret
LibAmdMsrWrite                ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read CPUID
; *
; *  @param[in]  RCX    CPUID function
; *  @param[in]  RDX    Pointer to CPUID_DATA to save cpuid data
; *  @param[in]  R8D    ConfigPtr (Optional)
; */
PUBLIC LibAmdCpuidRead
LibAmdCpuidRead       PROC

    push rbx
    push rsi
    mov  rsi, rdx
    mov  rax, rcx
    cpuid
    mov [rsi],   eax
    mov [rsi+4], ebx
    mov [rsi+8], ecx
    mov [rsi+12],edx
    pop rsi
    pop rbx
    ret

LibAmdCpuidRead              ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read TSC
; *
; *
; * @retval     RAX Time stamp counter value
; */

PUBLIC ReadTSC
ReadTSC  PROC
    rdtsc
    and  rax, 0ffffffffh
    shl  rdx, 32
    or   rax, rdx
    ret
ReadTSC  ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read memory/MMIO byte
; *
; * @param[in]  RCX - Memory Address
; * @retval     Memory byte at given address
; */
PUBLIC  Read64Mem8
Read64Mem8  PROC

     xor  rax, rax
     mov  al, [rcx]
     ret

Read64Mem8  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read memory/MMIO word
; *
; * @param[in]  RCX - Memory Address
; * @retval     Memory word at given address
; */
PUBLIC  Read64Mem16
Read64Mem16  PROC

        xor     rax, rax
        mov     ax, [rcx]
        ret

Read64Mem16  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read memory/MMIO dword
; *
; * @param[in]  RCX - Memory Address
; * @retval     Memory dword at given address
; */
PUBLIC  Read64Mem32
Read64Mem32  PROC

        xor     rax, rax
        mov     eax, [rcx]
        ret

Read64Mem32  ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write memory/MMIO byte
; *
; * @param[in]  RCX   Memory Address
; * @param[in]  DL    Value to write
; */

PUBLIC  Write64Mem8
Write64Mem8  PROC

        xor     rax, rax
        mov     rax, rdx
        mov     [rcx], al
        ret

Write64Mem8  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write memory/MMIO word
; *
; * @param[in]  RCX   Memory Address
; * @param[in]  DX    Value to write
; */
PUBLIC  Write64Mem16
Write64Mem16  PROC

        xor     rax, rax
        mov     rax, rdx
        mov     [rcx], ax
        ret

Write64Mem16  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write memory/MMIO dword
; *
; * @param[in]  RCX   Memory Address
; * @param[in]  EDX   Value to write
; */
PUBLIC  Write64Mem32
Write64Mem32  PROC

        xor     rax, rax
        mov     rax, rdx
        mov     [rcx], eax
        ret

Write64Mem32  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read various CPU registers
; *
; * @param[in]  CL     Register ID (0/4 - CR0/CR4, 10h/11h/12h/13h/17h - DR0/DR1/DR2/DR3/DR7)
; * @param[in]  RDX    Pointer to value
; */

PUBLIC  LibAmdReadCpuReg
LibAmdReadCpuReg PROC

        push    rax
        xor     rax, rax
Reg00h:
        cmp     cl, 00h
        jne     Reg04h
        mov     rax, cr0
        jmp     RegRead
Reg04h:
        cmp     cl, 04h
        jne     Reg10h
        mov     rax, cr4
        jmp     RegRead
Reg10h:
        cmp     cl, 10h
        jne     Reg11h
        mov     rax, dr0
        jmp     RegRead
Reg11h:
        cmp     cl, 11h
        jne     Reg12h
        mov     rax, dr1
        jmp     RegRead
Reg12h:
        cmp     cl, 12h
        jne     Reg13h
        mov     rax, dr2
        jmp     RegRead
Reg13h:
        cmp     cl, 13h
        jne     Reg17h
        mov     rax, dr3
        jmp     RegRead
Reg17h:
        cmp     cl, 17h
        jne     RegRead
        mov     rax, dr7
RegRead:
        mov     [rdx], eax
        pop     rax
        ret
LibAmdReadCpuReg ENDP



;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write various CPU registers
; *
; * @param[in]  CL    Register ID (0/4 - CR0/CR4, 10h/11h/12h/13h/17h - DR0/DR1/DR2/DR3/DR7)
; * @param[in]  RDX   Value to write
; */

PUBLIC  LibAmdWriteCpuReg
LibAmdWriteCpuReg PROC

        push    rax
Reg00h:
        cmp     cl, 00h
        jne     Reg04h
        mov     rax, cr0
        mov     eax, edx
        mov     cr0, rax
        jmp     Done
Reg04h:
        cmp     cl, 04h
        jne     Reg10h
        mov     rax, cr4
        mov     eax, edx
        mov     cr4, rax
        jmp     Done
Reg10h:
        cmp     cl, 10h
        jne     Reg11h
        mov     rax, dr0
        mov     eax, edx
        mov     dr0, rax
        jmp     Done
Reg11h:
        cmp     cl, 11h
        jne     Reg12h
        mov     rax, dr1
        mov     eax, edx
        mov     dr1, rax
        jmp     Done
Reg12h:
        cmp     cl, 12h
        jne     Reg13h
        mov     rax, dr2
        mov     eax, edx
        mov     dr2, rax
        jmp     Done
Reg13h:
        cmp     cl, 13h
        jne     Reg17h
        mov     rax, dr3
        mov     eax, edx
        mov     dr3, rax
        jmp     Done
Reg17h:
        cmp     cl, 17h
        jne     Done
        mov     rax, dr7
        mov     eax, edx
        mov     dr7, rax
Done:
        pop     rax
        ret
LibAmdWriteCpuReg ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write back invalidate caches using wbinvd.
; *
; *
; *
; */

PUBLIC LibAmdWriteBackInvalidateCache
LibAmdWriteBackInvalidateCache PROC
    wbinvd
    ret
LibAmdWriteBackInvalidateCache ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Stop CPU
; *
; *
; *
; */

PUBLIC StopHere
StopHere PROC
@@:
    jmp short @b
StopHere ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Enter debugger on SimNow
; *
; *
; *
; */
PUBLIC LibAmdSimNowEnterDebugger
LibAmdSimNowEnterDebugger PROC
    pushfq
    mov     rax, 0BACCD00Bh         ; Backdoor in SimNow
    mov     rbx, 2                  ; Select breakpoint feature
    cpuid
@@:
    jmp short @b
    popfq
    ret
LibAmdSimNowEnterDebugger ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  IDS IO port write
; *
; * @param[in]  ECX     IO Port Address
; * @param[in]  EDX     Value to write
; * @param[in]  R8D     IDS flags
; *
; */

PUBLIC IdsOutPort
IdsOutPort PROC
    push rbx
    push rax

    mov ebx, r8d
    mov eax, edx
    mov edx, ecx
    out dx, eax

    pop rax
    pop rbx
    ret
IdsOutPort  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Force breakpoint on HDT
; *
; *
; */
PUBLIC LibAmdHDTBreakPoint
LibAmdHDTBreakPoint PROC

    push rbx

    mov rcx, 0C001100Ah             ;bit 0 = HDT redirect
    mov rdi, 09C5A203Ah             ;Password
    rdmsr
    and rax, 0ffffffffh
    or rax, 1

    wrmsr

    mov rax, 0B2h                  ;Marker = B2
    db 0F1h                        ;ICEBP

    pop rbx
    ret

LibAmdHDTBreakPoint ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most right hand side non-zero bit with
; *
; * @param[in]  ECX       Value
; */
PUBLIC LibAmdBitScanForward
LibAmdBitScanForward PROC
    bsf eax, ecx
    jnz nonZeroSource
    mov al,32
nonZeroSource:
    ret
LibAmdBitScanForward  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most left hand side non-zero bit.
; *
; * @param[in]  ECX       Value
; */
PUBLIC LibAmdBitScanReverse
LibAmdBitScanReverse PROC
    bsr eax, ecx
    jnz nonZeroSource
    mov al,0FFh
nonZeroSource:
    ret
LibAmdBitScanReverse  ENDP

END
