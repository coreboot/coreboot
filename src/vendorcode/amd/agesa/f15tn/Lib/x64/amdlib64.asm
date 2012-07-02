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
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
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

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Flush specified number of cache line
; *
; *  @param[in]  RCX      Physical address to be flushed
; *  @param[in]  DL       number of cachelines to be flushed
; */
PUBLIC LibAmdCLFlush
LibAmdCLFlush  PROC
    push rax
    mov rax, rcx
    movzx rcx, dl
    @@:
    mfence
    clflush [rax]
    mfence
    add rax,64
    loop @B
    pop rax
    ret
LibAmdCLFlush  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  FPU init
; *
; *
; */
PUBLIC LibAmdFinit
LibAmdFinit  PROC
    finit
    ret
LibAmdFinit  ENDP

END
