;/**
; * @file
; *
; * Agesa library 32bit
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
; Copyright (C) 2012 Advanced Micro Devices, Inc.
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
.xmm
.model  flat
ASSUME FS:NOTHING
.code

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write IO byte
; *
; *  @param[in]   Address IO port address
; *  @param[in]   Data    IO port Value
; */

public  WriteIo8
WriteIo8        PROC NEAR C USES DX AX Address:WORD, Data:Byte
        mov     dx, Address
        mov     al, Data
        out     dx, al
        ret
WriteIo8        ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write IO word
; *
; *  @param[in]   Address IO port address
; *  @param[in]   Data    IO port Value
; */
public WriteIo16
WriteIo16       PROC NEAR C USES DX AX Address:WORD, Data:WORD
        mov     dx, Address
        mov     ax, Data
        out     dx, ax
        ret
WriteIo16       ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write IO dword
; *
; *  @param[in]   Address IO port address
; *  @param[in]   Data    IO port Value
; */

public WriteIo32
WriteIo32       PROC NEAR C USES DX EAX Address:WORD, Data:DWORD
        mov     dx, Address
        mov     eax, Data
        out     dx, eax
        ret
WriteIo32       ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read IO byte
; *
; *  @param[in] - IO port address
; *  @retval      IO port Value
; */
public ReadIo8
ReadIo8 PROC NEAR C USES DX Address:WORD
        mov     dx, Address
        in      al, dx
        ret
ReadIo8 ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read IO word
; *
; *  @param[in]   Address IO port address
; *  @retval      IO port Value
; */
public ReadIo16
ReadIo16        PROC NEAR C USES DX Address:WORD
        mov     dx, Address
        in      ax, dx
        ret
ReadIo16        ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read IO dword
; *
; *  @param[in]   Address  IO port address
; *  @retval      IO port Value
; */
public ReadIo32
ReadIo32        PROC NEAR C USES DX Address:WORD
        mov     dx, Address
        in      eax, dx
        ret
ReadIo32        ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read MSR
; *
; *  @param[in]  Address  MSR Address
; *  @param[in]  Data     Pointer to data
; *  @param[in]  ConfigPtr (Optional)
; */
public LibAmdMsrRead
LibAmdMsrRead  PROC NEAR C USES ECX ESI EDX Address:DWORD, Value:PTR, ConfigPtr:PTR
        mov     esi, ConfigPtr        ;Dummy read to avoid compilation warning
        mov     ecx, Address
        rdmsr
        mov     esi, Value
        mov     [esi],   eax
        mov     [esi+4], edx
        ret
LibAmdMsrRead  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write MSR
; *
; *  @param[in]  Address    MSR Address
; *  @param[in]  Data       Pointer to data
; *  @param[in]  ConfigPtr  (Optional)
; */
public LibAmdMsrWrite
LibAmdMsrWrite                PROC NEAR C USES ECX ESI EDX Address:DWORD, Data:PTR, ConfigPtr:PTR
        mov     esi, ConfigPtr         ;Dummy read to avoid compilation warning
        mov     ecx, Address
        mov     esi, Data
        mov     eax, [esi]
        mov     edx, [esi+4]
        wrmsr
        ret
LibAmdMsrWrite                ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read CPUID
; *
; *  @param[in]  Func   CPUID function
; *  @param[in]  DATA   Pointer to CPUID_DATA to save cpuid data
; *  @param[in]  ConfigPtr (Optional)
; */
public LibAmdCpuidRead
LibAmdCpuidRead       PROC NEAR C  Func:DWORD, DATA:PTR, ConfigPtr:PTR
        pushad
        mov     esi, ConfigPtr           ;Dummy read to avoid compilation warning
        mov     eax, Func
        cpuid
        mov     esi, DATA
        mov     [esi],   eax
        mov     [esi+4], ebx
        mov     [esi+8], ecx
        mov     [esi+12],edx
        popad
        ret
LibAmdCpuidRead              ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read TSC
; *
; *
; *
; */

public ReadTSC
ReadTSC  PROC    NEAR C
        rdtsc
        ret
ReadTSC  ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Set FS_BASE
; *
; *
; *
; * @param[in]  esi - Low Dword of physical address
; * @param[in]  edi - High Dword of physical address
; */
SetFsBase  PROC NEAR PUBLIC USES EAX EBX ECX EDX EDI

        mov     eax, ecx
        mov     ecx, 0C0010015h ; HWCR
        rdmsr
        mov     ebx, eax
        bts     eax, 17         ; HWCR.Wrap32Dis
        wrmsr
        xchg    edx, edi
        mov     eax, esi
        mov     esi, ebx

        mov     ecx, 0C0000100h ; FS_BASE
        wrmsr
        ret

SetFsBase  ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Restore MSR0C001_0015
; *
; * @param[in]  esi - Low Dword
; * @param[in]  edi - High Dword
; */
RestoreHwcr PROC NEAR PUBLIC USES EAX ECX EDX

        mov     ecx, 0C0010015h
        mov     eax, esi
        mov     edx, edi
        wrmsr
        ret

RestoreHwcr ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read memory/MMIO byte
; *
; * @param[in]  Address - Memory Address
; * @retval     Memory byte at given address
; */
Read64Mem8  PROC NEAR C PUBLIC USES EBX EDI ESI Address:QWORD

        mov     esi, DWORD PTR Address[0]
        mov     edi, DWORD PTR Address[4]
        test    edi, edi
        jz      AccesBelow4G

        push    fs
        call    SetFsBase
        xor     ebx, ebx
        mov     al, fs:[ebx]
        call    RestoreHwcr
        pop     fs
        jmp     Done
AccesBelow4G:
        mov     al, ds:[esi]
Done:
        ret

Read64Mem8  ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read memory/MMIO word
; *
; * @param[in]  Address - Memory Address
; * @retval     Memory word at given address
; */
Read64Mem16  PROC NEAR C PUBLIC USES EBX EDI ESI Address:QWORD

        mov     esi, DWORD PTR Address[0]
        mov     edi, DWORD PTR Address[4]
        test    edi, edi
        jz      AccesBelow4G

        push    fs
        call    SetFsBase
        xor     ebx, ebx
        mov     ax, fs:[ebx]
        call    RestoreHwcr
        pop     fs
        jmp     Done
AccesBelow4G:
        mov     ax, ds:[esi]
Done:

        ret

Read64Mem16  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read memory/MMIO dword
; *
; * @param[in]  Address - Memory Address
; * @retval     Memory dword at given address
; */
Read64Mem32  PROC NEAR C PUBLIC USES EBX EDI ESI Address:QWORD

        mov     esi, DWORD PTR Address[0]
        mov     edi, DWORD PTR Address[4]
        test    edi, edi
        jz      AccesBelow4G

        push    fs
        call    SetFsBase
        xor     ebx, ebx
        mov     eax, fs:[ebx]
        call    RestoreHwcr
        pop     fs
        jmp     Done
AccesBelow4G:
        mov     eax, ds:[esi]
Done:
        ret

Read64Mem32  ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write memory/MMIO byte
; *
; * @param[in]  Address - Memory Address
; * @param[in]  Value   - Value to write
; */

Write64Mem8  PROC NEAR C PUBLIC USES EBX EDI ESI Address:QWORD, Data:BYTE

        mov     esi, DWORD PTR Address[0]
        mov     edi, DWORD PTR Address[4]
        test    edi, edi
        jz      AccesBelow4G

        push    fs
        call    SetFsBase
        xor     ebx, ebx
        mov     al, Data
        mov     fs:[ebx], al
        call    RestoreHwcr
        pop     fs
        jmp     Done
AccesBelow4G:
        mov     al, Data
        mov     ds:[esi], al
Done:

        ret

Write64Mem8  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write memory/MMIO word
; *
; * @param[in]  Address - Memory Address
; * @param[in]  Value   - Value to write
; */
Write64Mem16  PROC NEAR C PUBLIC USES EBX EDI ESI Address:QWORD, Data:WORD

        mov     esi, DWORD PTR Address[0]
        mov     edi, DWORD PTR Address[4]
        test    edi, edi
        jz      AccesBelow4G

        push    fs
        call    SetFsBase
        xor     ebx, ebx
        mov     ax, Data
        mov     fs:[ebx], ax
        call    RestoreHwcr
        pop     fs
        jmp     Done
AccesBelow4G:
        mov     ax, Data
        mov     ds:[esi], ax
Done:
        ret

Write64Mem16  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write memory/MMIO dword
; *
; * @param[in]  Address - Memory Address
; * @param[in]  Value   - Value to write
; */
Write64Mem32  PROC NEAR C PUBLIC USES EBX EDI ESI Address:QWORD, Data:DWORD

        mov     esi, DWORD PTR Address[0]
        mov     edi, DWORD PTR Address[4]
        test    edi, edi
        jz      AccesBelow4G

        push    fs
        call    SetFsBase
        xor     ebx, ebx
        mov     eax, Data
        mov     fs:[ebx], eax
        call    RestoreHwcr
        pop     fs
        jmp     Done
AccesBelow4G:
        mov     eax, Data
        mov     ds:[esi], eax

Done:

        ret

Write64Mem32  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read various CPU registers
; *
; * @param[in]  Reg     Register ID (0/4 - CR0/CR4, 10h/11h/12h/13h/17h - DR0/DR1/DR2/DR3/DR7)
; * @param[in]  Value   Value to write
; */

LibAmdReadCpuReg PROC NEAR C Reg:BYTE, Value:NEAR PTR DWORD
    pushad
    push ds

    .if(Reg == 00h)
        mov eax, cr0
    .elseif(Reg == 04h)
        mov eax, cr4
    .elseif(Reg == 10h)
        mov eax, dr0
    .elseif(Reg == 11h)
        mov eax, dr1
    .elseif(Reg == 12h)
        mov eax, dr2
    .elseif(Reg == 13h)
        mov eax, dr3
    .elseif(Reg == 17h)
        mov eax, dr7
    .else
        xor eax,eax
    .endif

    mov edi, Value
    mov [edi], eax

    pop ds
    popad
    ret
LibAmdReadCpuReg ENDP



;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write various CPU registers
; *
; * @param[in]  Reg     Register ID (0/4 - CR0/CR4, 10h/11h/12h/13h/17h - DR0/DR1/DR2/DR3/DR7)
; * @param[in]  Value   Value to write
; */

LibAmdWriteCpuReg PROC NEAR C Reg:BYTE, Value:DWORD
    mov eax, Value

    .if(Reg == 00h)
        mov cr0, eax
    .elseif(Reg == 4)
        mov cr4, eax
    .elseif(Reg == 10h)
        mov dr0, eax
    .elseif(Reg == 11h)
        mov dr1, eax
    .elseif(Reg == 12h)
        mov dr2, eax
    .elseif(Reg == 13h)
        mov dr3, eax
    .elseif(Reg == 17h)
        mov dr7, eax
    .endif
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
LibAmdWriteBackInvalidateCache PROC NEAR C
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
StopHere PROC NEAR C
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
LibAmdSimNowEnterDebugger PROC NEAR C
    pushad
    mov     eax, 0BACCD00Bh         ; Backdoor in SimNow
    mov     ebx, 2                  ; Select breakpoint feature
    cpuid
@@:
    jmp short @b
    popad
    ret
LibAmdSimNowEnterDebugger ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  IDS IO port write
; *
; * @param[in]  Address     IO Port Address
; * @param[in]  Value       Value to write
; * @param[in]  Flag        IDS flags
; *
; */

PUBLIC IdsOutPort
IdsOutPort PROC NEAR C Address:DWORD, Value:DWORD ,Flag:DWORD
    push edx
    push eax
    push ebx
    mov edx, Address
    mov eax, Value
    mov ebx, Flag
    out dx, eax
    pop ebx
    pop eax
    pop edx
    ret
IdsOutPort  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Force breakpoint on HDT
; *
; *
; */
PUBLIC LibAmdHDTBreakPoint
LibAmdHDTBreakPoint PROC NEAR C

    pushad

    mov ecx, 0C001100Ah             ;bit 0 = HDT redirect
    mov edi, 09C5A203Ah             ;Password
    RDMSR                           ;
    or al, 1                        ;
    WRMSR                           ;
    mov al, 0B2h                    ;Marker = B2
    db 0F1h                         ;ICEBP

    popad
    ret

LibAmdHDTBreakPoint ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most right hand side non-zero bit with .
; *
; * @param[in]  Value       Value
; */
PUBLIC LibAmdBitScanForward
LibAmdBitScanForward PROC NEAR C Value:DWORD
    mov eax, Value
    bsf eax, Value
    .if (Zero?)
      mov al,32
    .endif
    ret
LibAmdBitScanForward  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most left hand side non-zero bit.
; *
; * @param[in]  Value       Value
; */
PUBLIC LibAmdBitScanReverse
LibAmdBitScanReverse PROC NEAR C Value:DWORD
    mov eax, Value
    bsr eax, Value
    .if (Zero?)
      mov al,0FFh
    .endif
    ret
LibAmdBitScanReverse  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Flush specified number of cache line
; *
; *  @param[in]  Address      Physical address to be flushed
; *  @param[in]  Count        number of cachelines to be flushed
; */
PUBLIC LibAmdCLFlush
LibAmdCLFlush  PROC NEAR C Address:QWORD, Count:BYTE
    pushad
    mov ecx, 0C0010015h ; HWCR
    rdmsr
    mov esi, eax
    mov edi, edx
    bts eax, 17         ; HWCR.Wrap32Dis
    wrmsr
    xor eax, eax
    mov edx, DWORD PTR Address[4]
    mov ecx, 0C0000100h ; FS_BASE
    wrmsr
    mov eax, DWORD PTR Address[0]
    movzx ecx, Count
    @@:
    mfence
    clflush fs:[eax]
    mfence
    add eax,64
    loop @B
    call RestoreHwcr
    popad
    ret
LibAmdCLFlush  ENDP

END
