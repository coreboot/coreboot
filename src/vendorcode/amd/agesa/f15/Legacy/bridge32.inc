; ****************************************************************************
; *
; * @file
; *
; * Agesa structures and definitions
; *
; * Contains AMD AGESA core interface
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Include
; * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
;
; ****************************************************************************
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

PARAM1          textequ <[bp+8]>
PARAM2          textequ <[bp+12]>
PARAM3          textequ <[bp+16]>
RETAddress      textequ <[bp+4]>

AMD_PRIVATE_PARAMS STRUCT
    Gate16_CS   DW ?                ; Segment of AMD_BRIDGE_32 and AMD_CALLOUT_16
    Gate16_SS   DW ?                ; RM stack segment
    Router_Seg  DW ?                ; Segment of oem router
    Router_Off  DW ?                ; Offset of oem router
AMD_PRIVATE_PARAMS ENDS

; OEM may pre-define the GDT and selector offsets. If they do not, use our defaults.
IFNDEF  AGESA_SELECTOR_GDT
        AGESA_SELECTOR_GDT         EQU 00h
ENDIF
IFNDEF  AGESA_SELECTOR_CODE16
        AGESA_SELECTOR_CODE16      EQU 08h
ENDIF
IFNDEF  AGESA_SELECTOR_DATA16
        AGESA_SELECTOR_DATA16      EQU 10h
ENDIF
IFNDEF  AGESA_SELECTOR_CODE32
        AGESA_SELECTOR_CODE32      EQU 18h
ENDIF
IFNDEF  AGESA_SELECTOR_DATA32
        AGESA_SELECTOR_DATA32      EQU 20h
ENDIF


AMD_BRIDGE_32_GDT MACRO  GDT_Name:REQ

        GDT_Name LABEL BYTE
            DD 000000000h, 000000000h       ; NULL descriptor
            DD 00000ffffh, 000009b00h       ; 16-bit code, fixed up
            DD 00000ffffh, 000009300h       ; 16-bit data, fixed up
            DD 00000ffffh, 000CF9B00h       ; 32-bit protected mode code
            DD 00000ffffh, 000CF9300h       ; 32-bit protected mode data
        GDT_Length EQU ($-GDT_Name)

ENDM

;+-------------------------------------------------------------------------
;
;       AMD_BRIDGE_32 - Execute Agesa through Pushhigh interface
;
;   Processing:
;       The following steps are taken:
;       1) Enter 32bit Protected Mode (PM32)
;       2) Run AGESA code
;       3) Restore Real Mode (RM)
;
; Entry:
;       [big real mode] : ds, es set to base 0 limit 4G segment
;       EDX - if not 0, provides a FAR PTR to oem router (Seg | Offset)
;       ESI - configuration block pointer
;
; Exit:
;       EAX - return value
;       ESI - configuration block pointer
;       ds, es, fs, gs - Set to 4GB segment limit for Big Real Mode
;
; Modified:
;      None
;

AMD_BRIDGE_32 MACRO GDT_Name

        local   copyGDT
        local   flushTo16PM
        local   agesaReturnAddress
        local   leave32bitPM
        local   flush2RM

        push    gs
        push    fs
        push    ebx
        push    ecx
        push    edi
        mov     eax, esp
        push    eax
        movzx   esp, sp
;
; Do not use any locals here, BP will be changed frequently during RM->PM32->RM
;
        pushf
        cli                         ; Disable interrupts during AGESA
        cld                         ; Need known direction flag during AGESA

;
; Save the FAR PTR input parameter
;
        mov     gs, dx                  ; Offset
        shr     edx, 16
        mov     fs, dx                  ; Segment
;
; Determine where our binary file is and get entry point
;
        mov     edx, (AMD_CONFIG_PARAMS PTR [esi]).ImageBasePtr
        add     edx, (AMD_IMAGE_HEADER PTR [edx]).EntryPointAddress
;
; Figure out the return address we will use after calling AGESA
; and store it in ebx until we have our stack set up properly
;
        mov     ebx, cs
        shl     ebx, 4
        add     ebx, OFFSET agesaReturnAddress
;
; Save our current RM stack AND entry EBP
;
        push    ebp
;       push    esp
        push    ss

;
; BEGIN --- STACK MUST BE BALANCED AT THIS POINT --- BEGIN
;
; Copy the GDT onto the stack for modification
;
        mov     cx, GDT_Length
        sub     sp, cx
        mov     bp, sp
        lea     di, GDT_Name
copyGDT:
        mov     al, cs:[di]
        mov     [bp], al
        inc     di
        inc     bp
        loop    copyGDT
;
; Patch 16-bit code and data descriptors on stack.  We will
; fix up CS and SS for PM16 during the callout if applicable.
;
        mov     bp, sp

        mov     eax, cs
        shl     eax, 4
        mov     [bp+AGESA_SELECTOR_CODE16+2], ax
        shr     eax, 16
        mov     [bp+AGESA_SELECTOR_CODE16+4], al

        mov     eax, ss
        shl     eax, 4
        mov     [bp+AGESA_SELECTOR_DATA16+2], ax
        shr     eax, 16
        mov     [bp+AGESA_SELECTOR_DATA16+4], al
;
; Need to place Length and Address on GDT
;
        mov     eax, ss
        shl     eax, 4
        add     eax, esp
        push    eax
        push    WORD PTR (GDT_Length-1)
;
; Load the GDT
;
        mov     bp, sp
        lgdt    FWORD PTR [bp]
;
; TABLE 1
;
; Place PRIVATE DATA on stack DIRECTLY following GDT
; During this routine, stack data is critical.  If
; order is changed or additional added, bad things
; will happen!
;
; HIGHEST PHYSICAL ADDRESS
;
; | ...                  |
; ------------------------
; | old RM SP            |
; | old RM SS            |
; ------------------------ sp + SIZEOF AMD_PRIVATE_PARAMS + (SIZEOF GDT_LENGTH + 6 {size, address})
; | GDT_DATA32           |
; | ...                  |
; | GDT_NULL             |
; | GDT Addr, Length     |
; ------------------------ sp + SIZEOF AMD_PRIVATE_PARAMS
; | Priv.Gate16_SS       |
; | Priv.Gate16_CS       |
; ------------------------ sp
; ------ THEN PUSH -------
; | Return to 16-bit CS  |
; | Return to 16-bit Off |
; | ...                  |
;
; LOWEST  PHYSICAL ADDRESS
;
        mov     edi, esp
        sub     edi, SIZEOF AMD_PRIVATE_PARAMS
        mov     ax, cs
        mov     (AMD_PRIVATE_PARAMS PTR ss:[edi]).Gate16_CS, ax
        mov     ax, ss
        mov     (AMD_PRIVATE_PARAMS PTR ss:[edi]).Gate16_SS, ax
        mov     (AMD_PRIVATE_PARAMS PTR ss:[edi]).Router_Off, gs
        mov     (AMD_PRIVATE_PARAMS PTR ss:[edi]).Router_Seg, fs

        mov     esp, edi
;
; Save an address for returning to 16 bit real mode on stack,
; we'll use it in a far ret after turning off CR0.PE so that
; we can take our address off and force a far jump.  Be sure
; no unexpected data is on the stack after this!
;
        mov     ax, cs
        push    cs
        lea     ax, flush2RM
        push    ax
;
; Convert ss:esp to "flat"
;

        mov     ax, sp
        push    ax
        mov     eax, ss
        shl     eax, 4
        add     eax, esp
        mov     esp, eax                ; Load the zero based ESP

;
; Set CR0.PE
;
        mov     eax, CR0                ; Get CPU control word 0
        or      al, 01                  ; Enable CPU protected mode
        mov     CR0, eax                ; Write back to CPU control word 0
        jmp     flushTo16PM

flushTo16PM:
;
; 16-bit protected mode
;
        mov     ax, AGESA_SELECTOR_DATA32
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
;
; Push our parameters RIGHT TO LEFT, and then return address
;
        push    esi                     ; AGESA configuration block pointer (data)
        push    ebx                     ; after AGESA return offset (32PM flat) - consumed by dispatcher ret
        pushd   AGESA_SELECTOR_CODE32   ; AGESA entry selector (32PM flat)
        push    edx                     ; AGESA entry point (32PM flat)

        DB      066h
        retf                            ; <><><> Enter AGESA 32-bit code!!! <><><>

agesaReturnAddress:
;
; Returns from the Agesa 32-bit code still PM32
;
        DB      0EAh
        DD      OFFSET leave32bitPM
        DW      AGESA_SELECTOR_CODE16

leave32bitPM:
;
; Now in 16-bit PM
;
        add     esp, 4                  ; +4 to remove our config block pointer
;
; Eax reserve AGESA_STATUS return code, save it
;
        mov     ebx, eax
;
; Turn off CR0.PE, restore 64K stack limit
;
        pop     ax
        mov     sp, ax
        mov     ax, AGESA_SELECTOR_DATA16
        mov     ss, ax

        mov     eax, CR0
        and     al, NOT 1               ; Disable protected mode
        mov     CR0, eax                ; Write back CR0.PE
;
; Jump far to enter RM, we saved this address on the stack
; already.  Hopefully stack is balanced through AGESA
; nor were any params added by pushing them on the stack and
; not removing them between BEGIN-END comments.
;
        retf

flush2RM:
;
; Set segments registers for big real mode before returning
;
        xor     ax, ax
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
;
; Discard GDT, +6 for GDT pointer/size, privates
;
        add     esp, GDT_Length + 6 + SIZEOF AMD_PRIVATE_PARAMS
;
; Restore real mode stack and entry EBP
;
        pop     cx
;       mov     esp, [esp]
        mov     ss, cx
        pop     ebp
;
; Restore AGESA_STATUS return code to eax
;
        mov     eax, ebx
;
; END --- STACK MUST BE BALANCED TO THIS POINT --- END
;

        popf
        pop     ebx
        mov     esp, ebx
        pop     edi
        pop     ecx
        pop     ebx
        pop     fs
        pop     gs
                            ; EXIT AMD_BRIDGE_32
ENDM
;+-------------------------------------------------------------------------
;
; AMD_CALLOUT_16 - Execute Callback from Pushhigh interface
;
;   Processing:
;       The following steps are taken:
;       1) Enter PM16
;       2) Setup stack, get private params
;       3) Enter RM
;       4) Get 3 params
;       5) Call oemCallout OR oem router
;       6) Enter PM32
;       7) Return to Agesa PH
;
; Entry:
;       [32-bit protected mode]
;       [esp+8] Func
;       [esp+12] Data
;       [esp+16] Configuration Block
;       [esp+4] return address to Agesa
;
; Exit:
;       [32-bit protected mode]
;
; Modified:
;     None
;
AMD_CALLOUT_16 MACRO  LocalOemCalloutRouter
;
; Note that we are still PM32, so MASM may work strangely
;

        push    bp                     ; Save our original SP to access params
        mov     bp, sp
        push    bx
        push    si
        push    di
        push    cx
        push    dx
        push    di

        DB      066h, 0EAh
        DW      OFFSET PM16Entry
        DW      AGESA_SELECTOR_CODE16

PM16Entry:
;
; PM16 CS, but still PM32 SS, as we need to access our private params
; before we enter RM.
;
; Note: we are working below the stack temporarily, and and it will
; not affect our ability to get entry params
;
        xor     ecx, ecx
        xor     edx, edx
;
; SGDT will give us the original location of the GDT on our CAS stack.
; We need this value because our private parameters are located just
; below the GDT.
;
        mov     edi, esp
        sub     edi, GDT_Length + 6
        sgdt    FWORD PTR [edi]        ; [edi] = word size, dword address
        mov     edi, DWORD PTR [edi+2]  ; Get the PM32 address only
        sub     edi, SIZEOF AMD_PRIVATE_PARAMS + 6
;
; cx = code segment of this code in RM
; dx = stack segment of CAS in RM
; fs = code segment of oem router (save for later)
; gs = offset of oem router (save for later)
; fs and gs are loaded after switch to real mode because we can't
; use them as scratch pad registers in protected mode
;
        mov     cx, (AMD_PRIVATE_PARAMS PTR ss:[edi]).Gate16_CS
        mov     dx, (AMD_PRIVATE_PARAMS PTR ss:[edi]).Gate16_SS

        mov     eax, edi             ; Save edi in eax for after RM switch
        mov     edi, esp             ; Save our current ESP for RM

        movzx   ebx, dx
        shl     ebx, 4
        sub     esp, ebx

;
; We had been accessing the stack in PM32, we will now change to PM16 so we
; will make the stack segment 64KB limit so SP needs to be fixed made PM16
; compatible.
;
        mov     bx, AGESA_SELECTOR_DATA16
        mov     ss, bx

;
; Save the RM segment and RM offset of the jump we will need to make in
; order to enter RM so that code in this segment is relocatable.
;
; BEGIN --- Don't unbalance the stack --- BEGIN
;
        push    cx
        pushw   OFFSET RMEntry

        mov     ebx, CR0
        and     bl, NOT 1
        mov     CR0, ebx                ; CR0.PE cleared
;
; Far jump to clear segment descriptor cache and enter RM
;
        retf

RMEntry:
;
; We are in RM, setup RM stack
;
        movzx   ebx, dx               ; Get RM SS in ebx
        shl     ebx, 4                  ; Get our stack top on entry in EBP to
        sub     ebp, ebx                ; access our entry parameters
        sub     eax, ebx                ; save copy of parameters address
        mov     ss, dx                  ; Set stack segment
;
; We are going to figure out the address to use when we return
; and have to go back into PM32 while we have access to it
;
        movzx   ebx, cx               ; Get original CS in ebx
        shl     ebx, 4
        add     ebx, OFFSET PM32Entry
;
; Now we put our data, func, block params into calling convention
; for our hook
;
; ECX = Func
; EDX = Data
; ESI = config pointer
;
        mov     ecx, PARAM1             ; Func
        mov     edx, PARAM2             ; Data
        mov     esi, PARAM3             ; pointer

        push    ebx                    ; Save PM32 mode switch address
        push    edi                    ; Save PM32 stack pointer
        pushf
;
; Get Router Function Address
;
        mov     edi, eax
        mov     ax, (AMD_PRIVATE_PARAMS PTR ss:[edi]).Router_Seg
        mov     fs, ax
        mov     ax, (AMD_PRIVATE_PARAMS PTR ss:[edi]).Router_Off
        mov     gs, ax

        mov     eax, AGESA_UNSUPPORTED  ; Default return value
;
; If AMD_BRIDGE_32 EDX == 0 call oemCallout
; otherwise call FAR PTR EDX
;
; Critical:
;   sp+2 - EDI aka PM32 stack address
;   sp+4 - address of PM32Entry in PM32
;
        mov     bx, fs
        shl     ebx, 16
        mov     bx, gs

        .if (ebx == 0)
            call        LocalOemCalloutRouter
        .else
;
; Make far call to Router function
;
        push    cs
        push    offset CalloutReturn
        push    ebx
        retf
CalloutReturn:
    .endif
;
; Restore PM32 esp from RM stack
;
        popf
        pop     edi                     ; Our PM32 stack pointer
        pop     edx                     ; Our PM32 mode switch address

        mov     ebx, CR0
        or      bl, 1                   ; CR0.PE set
        mov     CR0, ebx

        mov     ebx, AGESA_SELECTOR_DATA32
        pushd   AGESA_SELECTOR_CODE32 ; PM32 selector
        push    edx                    ; PM32 entry point

        DB      066h
        retf                        ; Far jump to enter PM32

PM32Entry:
;
; END --- Don't unbalance the stack --- END
; We are now PM32, so remember MASM is assembling in 16-bit again
;
        mov     ss, bx
        mov     ds, bx
        mov     es, bx
        mov     fs, bx
        mov     gs, bx

        mov     sp, di
        pop     di
        pop     dx
        pop     cx
        pop     di
        pop     si
        pop     bx
        pop     bp
                            ; EXIT AMD_CALLOUT_16
ENDM
