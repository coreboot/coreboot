;*****************************************************************************
; AMD Generic Encapsulated Software Architecture
;
;  Workfile: arch2008.asm     $Revision: 84150 $    $Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
;
; Description: ARCH2008.ASM - AGESA Architecture 2008 Wrapper Template
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

    .XLIST
    INCLUDE agesa.inc
    INCLUDE acwrapg.inc       ; Necessary support file as part of wrapper, including but not limited to segment start/end macros.
    INCLUDE acwrap.inc        ; IBVs may specify host BIOS-specific include files required when building.
    INCLUDE cpcarmac.inc
    INCLUDE bridge32.inc
    .LIST
    .586p
    .mmx


;----------------------------------------------------------------------------
; Local definitions
;----------------------------------------------------------------------------

sOemCallout STRUCT
    FuncName   DD   ?         ; Call out function name
    FuncPtr    DW   ?         ; Call out function pointer
sOemCallout ENDS

sOemEventHandler STRUCT
    ClassCode  DD   ?         ; AGESA event log sub-class code
    FuncPtr    DW   ?         ; Event handler function pointer
sOemEventHandler ENDS

;; A typical legacy BIOS implementation may require the E000 and F000 segments
;; to be cached.
EXE_CACHE_REGION_BASE_0  EQU  0E0000h
EXE_CACHE_REGION_SIZE_0  EQU  20000h

;; In this sample implementation, the B1 and B2 images are placed next to each
;; other in the BIOS ROM to help with the maximization of cached code.
EXE_CACHE_REGION_BASE_1  EQU  AGESA_B1_ADDRESS
EXE_CACHE_REGION_SIZE_1  EQU  40000h

;; The third region is not needed in our example.
EXE_CACHE_REGION_BASE_2  EQU  0
EXE_CACHE_REGION_SIZE_2  EQU  0


;----------------------------------------------------------------------------
;    PERSISTENT SEGMENT
; This segment is required to be present throughout all BIOS execution.
;----------------------------------------------------------------------------

AMD_PERSISTENT_START


;----------------------------------------------------------------------------
; Instantiate the global descriptor table
;----------------------------------------------------------------------------

AMD_BRIDGE_32_GDT AMD_GDT         ; Instantiate the global descriptor table
                                  ; required by the push-high mechanism.


;----------------------------------------------------------------------------
; Declare the external routines required in the persistent segment
;----------------------------------------------------------------------------

;+-------------------------------------------------------------------------
;
;   AmdDfltRet
;
;   Entry:
;     None
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     Near stub procedure.  Simply perform a retn instruction.
;
EXTERN AmdDfltRet:NEAR


;+-------------------------------------------------------------------------
;
;   AmdDfltRetFar
;
;   Entry:
;     None
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     Far stub procedure.  Simply perform a retf instruction.
;
EXTERN AmdDfltRetFar:FAR


;----------------------------------------------------------------------------
; Declare the optional external routines in the persistent segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   myModuleTypeMismatchHandler (Example)
;
;   Entry:
;     ESI - Pointer to the EVENT_PARAMS structure of the failure.
;     [ESI].DataParam1 - Socket
;     [ESI].DataParam2 - DCT
;     [ESI].DataParam3 - Channel
;     [ESI].DataParam4 - 0x00000000
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure can be used to react to a memory module type
;     mismatch error discovered by the AGESA code.  Actions taken
;     may include, but are not limited to:
;       Logging the event to NV for display later
;       Reset, excluding the mismatch on subsequent reboot
;       Do nothing
;
;   Dependencies:
;     None
;
EXTERN myModuleTypeMismatchHandler(AmdDfltRet):NEAR

;+---------------------------------------------------------------------------
;
;   oemPlatformConfigInit (Optional)
;
;   Entry:
;     EDI - 32-bit flat pointer to the PLATFORM_CONFIGURATION to be
;           passed in to the next AGESA entry point.
;
;           typedef struct {
;             IN PERFORMANCE_PROFILE PlatformProfile;
;             IN CPU_HT_DEEMPHASIS_LEVEL *PlatformDeemphasisList;
;             IN UINT8               CoreLevelingMode;
;             IN PLATFORM_C1E_MODES  C1eMode;
;             IN UINT32              C1ePlatformData;
;             IN UINT32              C1ePlatformData1;
;             IN UINT32              C1ePlatformData2;
;             IN UINT32              C1ePlatformData3;
;             IN BOOLEAN             UserOptionDmi;
;             IN BOOLEAN             UserOptionPState;
;             IN BOOLEAN             UserOptionSrat;
;             IN BOOLEAN             UserOptionSlit;
;             IN BOOLEAN             UserOptionWhea;
;             IN UINT32              PowerCeiling;
;             IN BOOLEAN             PstateIndependent;
;           } PLATFORM_CONFIGURATION;
;
;           typedef struct {
;             IN UINT8 Socket;
;             IN UINT8 Link;
;             IN UINT8 LoFreq;
;             IN UINT8 HighFreq;
;             IN PLATFORM_DEEMPHASIS_LEVEL     ReceiverDeemphasis;
;             IN PLATFORM_DEEMPHASIS_LEVEL     DcvDeemphasis;
;           } CPU_HT_DEEMPHASIS_LEVEL;
;
;           typedef struct {
;             IN PLATFORM_CONTROL_FLOW PlatformControlFlowMode;
;             IN BOOLEAN               UseHtAssist;
;             IN BOOLEAN               UseAtmMode;
;             IN BOOLEAN               UseNbrCache;
;             IN BOOLEAN               Use32ByteRefresh;
;             IN BOOLEAN               UseVariableMctIsocPriority;
;           } PERFORMANCE_PROFILE;
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     Provide a single hook routine to modify the parameters of a
;     PLATFORM_CONFIGURATION structure before any entry point that
;     has such a structure as an input.
;
;   Dependencies:
;     None
;
;   Example:
;     If your platform is running in UMA mode, the following code
;     may be added:
;       mov  (PLATFORM_CONFIGURATION PTR [edi]).PlatformProfile.PlatformControlFlowMode, UmaDr
;
EXTERN oemPlatformConfigInit(AmdDfltRetFar):FAR

;+---------------------------------------------------------------------------
;
;   oemCallout (Optional)
;
;   Entry:
;     ECX - Callout function number
;     EDX - Function-specific UINTN
;     ESI - Pointer to function specific data
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     None
;
;   Purpose:
;     The default call out router function which resides in the same
;     segment as the push-high bridge code.
;
;   Dependencies:
;     None
;
EXTERN oemCallout(AmdDfltRet):NEAR


;----------------------------------------------------------------------------
; Define the sample wrapper routines for the persistent segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   AmdBridge32
;
;   Entry:
;     EDX - A Real Mode FAR pointer using seg16:Offset16 format that
;           points to a local host environment call-out router. If
;           this pointer is not equal to zero, then this pointer is
;           used as the call-out router instead of the standard
;           OemCallout. This may be useful when the call-out router
;           is not located in the same segment as the AmdBridge32 and
;           AmdCallout16 routines.
;     ESI - A Flat Mode pointer (32-bit address) that points to the
;           configuration block (AMD_CONFIG_PARAMS) for the AGESA
;           software function.
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     None
;
;   Purpose:
;     Execute an AGESA software function through the Push-High interface.
;
;   Dependencies:
;     This procedure requires a stack. The host environment must use the
;     provided service function to establish the stack environment prior
;     to making the call to this procedure.
;
AmdBridge32 PROC FAR PUBLIC
    AMD_BRIDGE_32 AMD_GDT             ; use the macro for the body
    ret
AmdBridge32 ENDP


;+---------------------------------------------------------------------------
;
;   AmdEnableStack
;
;   Entry:
;     BX - Return address
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;     SS:ESP - Points to the private stack location for this processor core.
;     ECX - Upon success, contains this processor core's stack size in bytes.
;
;   Modified:
;     EAX, ECX, EDX, EDI, ESI, ESP, DS, ES
;
;   Purpose:
;     This procedure is used to establish the stack within the host environment.
;
;   Dependencies:
;     The host environment must use this procedure and not rely on any other
;     sources to create the stack region.
;
AmdEnableStack PROC NEAR PUBLIC
    AMD_ENABLE_STACK
    ;; EAX = AGESA_SUCCESS, The stack space has been allocated for this core.
    ;; EAX = AGESA_WARNING, The stack has already been set up.  SS:ESP is set
    ;;       to stack top, and ECX is the stack size in bytes.
    jmp   bx
AmdEnableStack ENDP


;+---------------------------------------------------------------------------
;
;   AmdDisableStack
;
;   Entry:
;     BX - Return address
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     EAX, ECX, EDX, ESI, ESP
;
;   Purpose:
;     This procedure is used to remove the pre-memory stack from within the
;     host environment.
;     The exit state for the BSP is described as follows:
;       Memory region 00000-9FFFF MTRRS are set as WB memory.
;       Processor Cache is enabled (CD bit is cleared).
;       MTRRs used for execution cache are kept.
;       Cache content is flushed (invalidated without write-back).
;       Any family-specific clean-up done.
;     The exit state for the APs is described as follows:
;       Memory region 00000-9FFFF MTRRS are set as WB memory.
;       Memory region A0000-DFFFF MTRRS are set as UC IO.
;       Memory region E0000-FFFFF MTRRS are set as UC memory.
;       MTRRs used for execution cache are cleared.
;       Processor Cache is disabled (CD bit is set).
;       Top-of-Memory (TOM) set to the system top of memory as determined
;         by the memory initialization routines.
;       System lock command is enabled.
;       Any family-specific clean-up done.
;
;   Dependencies:
;     The host environment must use this procedure and not rely on any other
;     sources to break down the stack region.
;     If executing in 16-bit code, the host environment must establish the
;     "Big Real" mode of 32-bit addressing of data.
;
AmdDisableStack PROC NEAR PUBLIC
    AMD_DISABLE_STACK
    ;; EAX = AGESA_SUCCESS, The stack space has been disabled for this core.
    jmp   bx
AmdDisableStack ENDP


;+---------------------------------------------------------------------------
;
;   AmdCallout16
;
;   Entry:
;     [esp+8] - Func
;     [esp+12] - Data
;     [esp+16] - Configuration Block
;     [esp+4] - Return address to AGESA
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     None
;
;   Purpose:
;     Execute callback from the push-high interface.
;
;   Dependencies:
;     None
;
AmdCallout16 PROC FAR PUBLIC ; declare the procedure
    AMD_CALLOUT_16 oemCallout ; use the macro for the body
    ret
AmdCallout16 ENDP


;+---------------------------------------------------------------------------
;
;   AmdProcessAgesaErrors (Optional)
;
;   Entry:
;     AL - Heap status of the AGESA entry point that was just invoked.
;     EBX - AGESA image base address.
;     EDX - Segment / Offset of the appropriate callout router function.
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure is used to handle any errors that may have occurred
;     during an AGESA entry point.
;
;   Dependencies:
;     None
;
AmdProcessAgesaErrors PROC FAR PUBLIC
    LOCAL localCpuInterfaceBlock:EVENT_PARAMS

    pushad
    xor   edi, edi
    mov   di, ss
    shl   edi, 4
    lea   esi, localCpuInterfaceBlock
    add   esi, edi

    ; Fill default config block
    mov   (EVENT_PARAMS PTR [esi]).StdHeader.Func, AMD_READ_EVENT_LOG
    mov   (EVENT_PARAMS PTR [esi]).StdHeader.ImageBasePtr, ebx
    mov   (EVENT_PARAMS PTR [esi]).StdHeader.AltImageBasePtr, 0
    mov   (EVENT_PARAMS PTR [esi]).StdHeader.HeapStatus, al
    mov   edi, SEG AmdCallout16
    shl   edi, 4
    add   edi, OFFSET AmdCallout16
    mov   (EVENT_PARAMS PTR [esi]).StdHeader.CalloutPtr, edi

    ; Flush the event log searching for, and handling all monitored events
    xor   eax, eax
    .while (eax == 0)
        push  edx
        call  AmdBridge32
        pop   edx
        .if (eax == AGESA_SUCCESS)
            mov   eax, (EVENT_PARAMS PTR [esi]).EventInfo
            .if (eax != 0)
                lea   di, cs:AgesaEventTable

loopThruTable:
                cmp   di, OFFSET cs:AgesaEventTableEnd
                jae   unhandledEvent

                cmp   eax, cs:[di].sOemEventHandler.ClassCode
                je    FoundMatch
                add   di, SIZEOF sOemEventHandler
                jmp   loopThruTable

FoundMatch:
                mov   bx, cs:[di].sOemEventHandler.FuncPtr
                call  bx

unhandledEvent:
                xor   eax, eax
            .else
                mov   al, 1
            .endif
        .endif
    .endw
    popad
    ret

AmdProcessAgesaErrors ENDP


;----------------------------------------------------------------------------
; Define the error handler table
;----------------------------------------------------------------------------

AgesaEventTable LABEL BYTE
    ;; Add entries as desired
    ;;---------
    ;; EXAMPLE
    ;;---------
    sOemEventHandler <MEM_ERROR_MODULE_TYPE_MISMATCH_DIMM, OFFSET myModuleTypeMismatchHandler>
AgesaEventTableEnd LABEL BYTE


AMD_PERSISTENT_END




;----------------------------------------------------------------------------
;    RECOVERY SEGMENT
; This segment resides in the classic 'boot-block,' and is used
; for recovery.
;----------------------------------------------------------------------------

AMD_RECOVERY_START


;----------------------------------------------------------------------------
; Declare the external routines required in the recovery segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   myReadSPDRecovery (Required for proper recovery mode operation)
;
;   Entry:
;     ESI - Pointer to an AGESA_READ_SPD_PARAMS structure.
;
;     typedef struct {
;       IN OUT AMD_CONFIG_PARAMS StdHeader;
;       IN       UINT8 SocketId;
;       IN       UINT8 MemChannelId;
;       IN       UINT8 DimmId;
;       IN OUT   UINT8 *Buffer;
;       IN OUT   MEM_DATA_STRUCT *MemData;
;     } AGESA_READ_SPD_PARAMS;
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS Indicates the SPD block for the indicated
;                         DIMM was read successfully.
;           AGESA_BOUNDS_CHK The specified DIMM is not present.
;           AGESA_UNSUPPORTED This is a required function, so this
;                             value being returned causes a critical
;                             error response value from the AGESA
;                             software function and no memory initialized.
;           AGESA_ERROR The DIMM SPD read process has generated
;                       communication errors.
;
;   Modified:
;     None
;
;   Purpose:
;     This call out reads a block of memory SPD data and places it
;     into the provided buffer.
;
;   Dependencies:
;     None
;
EXTERN myReadSPDRecovery:NEAR


;----------------------------------------------------------------------------
; Define the sample wrapper routines for the recovery segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   AmdInitResetWrapper
;
;   Entry:
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     A minimal initialization of the processor core is performed. This
;     procedure must be called by all processor cores. The code path
;     separates the BSP from the APs and performs a separate and appropriate
;     list of tasks for each class of core.
;     For the BSP, the following actions are performed:
;       Internal heap sub-system initialization
;       Primary non-coherent HyperTransportT link initialization
;       Return to the host environment to test for Recovery Mode.
;     The AP processor cores do not participate in the recovery process.
;     However, they execute this routine after being released to execute
;     by the BSP during the main boot process. Their actions include the
;     following:
;       Internal heap sub-system initialization
;       Proceed to a wait loop waiting for commands from the BSP
;
;     For the cache regions, up to three regions of execution cache can be
;     allocated following the following rules:
;     1. Once a region is allocated, it cannot be de-allocated. However, it
;        can be expanded.
;     2. At most, two of the three regions can be located above 1 MByte. A
;        region failing this rule is ignored.
;     3. All region addresses must be at or above the 0x000D0000 linear
;        address. A region failing this rule is ignored.
;     4. The address is aligned on a 32-KByte boundary. Starting addresses
;        is rounded down to the nearest 32-Kbyte boundary.
;     5. The execution cache size must be a multiple of 32 KByte. Size is
;        rounded up to the next multiple of 32 KByte.
;     6. A region must not span either the 1-MByte boundary or the 4-GByte
;        boundary. Allocated size is truncated to not span the boundary.
;     7. The granted cached execution regions, address, and size are calculated
;        based on the available cache resources of the processor core.
;        Allocations are made up to the limit of cache available on the
;        installed processor.
;     Warning: Enabling instruction cache outside of this interface can cause
;              data corruption.
;
;   Dependencies:
;     This procedure is expected to be executed soon after a system reset
;     for the main boot path or resume path of execution.
;
;     This procedure requires a stack.
;
;     Because the heap system is not yet operational at the point of the
;     interface call, the host environment must allocate the storage for
;     the AMD_RESET_PARAMS structure before making the first call to
;     AmdCreateStruct.  This is the ByHost method of allocation.
;
AmdInitResetWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS
    local localResetParams:AMD_RESET_PARAMS

    pushad

    ; Prepare for the call to initialize the input parameters for AmdInitReset
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B1_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    ; Use the 'ByHost' allocation method because the heap has not been initialized as of yet.
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_RESET
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, ByHost
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, sizeof AMD_RESET_PARAMS
    lea   edx, localResetParams
    add   edx, eax
    push  edx
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr, edx
    mov   dx, SEG AmdCalloutRouterRecovery
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterRecovery
    push  edx
    call  AmdBridge32
    pop   edx
    pop   esi

    ; The structure has been initialized.  Now modify the default settings as desired.

    ; Allocate the execution cache to maximize the amount of code in ROM that is cached.
    ; Placing the B1 and B2 images near one another is a good way to ensure the AGESA code
    ; is cached.
    mov   (AMD_RESET_PARAMS ptr [esi]).CacheRegion.ExeCacheStartAddr, EXE_CACHE_REGION_BASE_0
    mov   (AMD_RESET_PARAMS ptr [esi]).CacheRegion.ExeCacheSize, EXE_CACHE_REGION_SIZE_0
    mov   (AMD_RESET_PARAMS ptr [esi + sizeof EXECUTION_CACHE_REGION]).CacheRegion.ExeCacheStartAddr, EXE_CACHE_REGION_BASE_1
    mov   (AMD_RESET_PARAMS ptr [esi + sizeof EXECUTION_CACHE_REGION]).CacheRegion.ExeCacheSize, EXE_CACHE_REGION_SIZE_1
    mov   (AMD_RESET_PARAMS ptr [esi + (2 * sizeof EXECUTION_CACHE_REGION)]).CacheRegion.ExeCacheStartAddr, EXE_CACHE_REGION_BASE_2
    mov   (AMD_RESET_PARAMS ptr [esi + (2 * sizeof EXECUTION_CACHE_REGION)]).CacheRegion.ExeCacheSize, EXE_CACHE_REGION_SIZE_2

    ; Call in to the AmdInitReset entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS Early initialization completed successfully.
    ;;  AGESA_WARNING One or more of the execution cache allocation
    ;;                rules were violated, but an adjustment was made
    ;;                and space was allocated.
    ;;  AGESA_ERROR One or more of the execution cache allocation rules
    ;;              were violated, which resulted in a requested cache
    ;;              region to not be allocated.
    ;;              The storage space allocated for the AMD_RESET_PARAMS
    ;;              structure is insufficient.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_RESET_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B1_ADDRESS
        call  AmdProcessAgesaErrors
    .endif


    ;; Here are what the MTRRs should look like based off of the CacheRegions specified above:

    ;; Fixed-Range MTRRs
    ;; Name                 Address         Value
    ;; ----------------     --------        ----------------
    ;; MTRRfix4k_E0000      0000026C        0505050505050505
    ;; MTRRfix4k_E8000      0000026D        0505050505050505
    ;; MTRRfix4k_F0000      0000026E        0505050505050505
    ;; MTRRfix4k_F8000      0000026F        0505050505050505
    ;; MTRRdefType          000002FF        0000000000000C00
    ;;
    ;; Variable-Range MTRRs and IO Range
    ;;       MTRRphysBase(n)             MTRRphysMask(n)
    ;;       -----------------           -----------------
    ;; n=0   0000000000000000            0000000000000000
    ;; n=1   0000000000000000            0000000000000000
    ;; n=2   0000000000000000            0000000000000000
    ;; n=3   0000000000000000            0000000000000000
    ;; n=4   0000000000000000            0000000000000000
    ;; n=5   Heap Base (Varies by core)  0000FFFFFFFF0800
    ;; n=6   AGESA_B1_ADDRESS | 6        0000FFFFFFFC0800
    ;; n=7   0000000000000000            0000000000000000


    ;; Because the allocation method is 'ByHost,' the call to AMD_RELEASE_STRUCT is
    ;; not necessary.  Stack space reclamation is left up to the host BIOS.

    popad
    ret


AmdInitResetWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdInitRecoveryWrapper
;
;   Entry:
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     Perform a minimum initialization of the processor and memory to
;     support a recovery mode flash ROM update.
;     For the BSP, the following actions are performed:
;       Configuration of CPU core for recovery process
;       Minimal initialization of some memory
;     The AP processor cores do not participate in the recovery process.
;     No actions or tasks are performed by the AP cores for this time point.
;
;   Dependencies:
;     This procedure requires a stack. The host environment must use one of
;     the provided service functions to establish the stack environment prior
;     to making the call to this procedure.
;
AmdInitRecoveryWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitRecovery
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B1_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_RECOVERY
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PreMemHeap
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterRecovery
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterRecovery
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.


    ; Call in to the AmdInitRecovery entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS The function has completed successfully.
    ;;  AGESA_WARNING One or more of the allocation rules were violated,
    ;;                but an adjustment was made and space was allocated.
    ;;  AGESA_ERROR One or more of the allocation rules were violated,
    ;;              which resulted in a requested cache region to not be
    ;;              allocated.
    ;;  AGESA_FATAL No memory was found in the system.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_RECOVERY_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B1_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    ; Allow AGESA to free the space used by AmdInitRecovery
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32

    popad
    ret
AmdInitRecoveryWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdCalloutRouterRecovery
;
;   Entry:
;     ECX - Callout function number
;     EDX - Function-specific UINTN
;     ESI - Pointer to function specific data
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     None
;
;   Purpose:
;     The call out router function for AmdInitReset and
;     AmdInitRecovery.
;
;   Dependencies:
;     None
;
AmdCalloutRouterRecovery PROC FAR PUBLIC USES ECX EBX ESI BX DI DS ES
    xor   ax, ax
    mov   ds, ax
    mov   es, ax
    lea   di, cs:CalloutRouterTableRecovery
    mov   eax, AGESA_UNSUPPORTED

loopThruTable:
    cmp   di, OFFSET cs:CalloutRouterTableRecoveryEnd
    jae   amdCpuCalloutExit ; exit with AGESA_UNSUPPORTED
    cmp   ecx, cs:[di].sOemCallout.FuncName
    je    FoundMatch
    add   di, SIZEOF sOemCallout
    jmp   loopThruTable

FoundMatch:
    mov   bx, cs:[di].sOemCallout.FuncPtr
    call  bx

amdCpuCalloutExit:
    ret
AmdCalloutRouterRecovery ENDP


;----------------------------------------------------------------------------
; Define the callout dispatch table for the recovery segment
;----------------------------------------------------------------------------

CalloutRouterTableRecovery LABEL BYTE
    ;; Standard B1 implementations only need the SPD reader call out function to be implemented.
    sOemCallout <AGESA_READ_SPD, OFFSET myReadSPDRecovery>
CalloutRouterTableRecoveryEnd LABEL BYTE


AMD_RECOVERY_END



;----------------------------------------------------------------------------
;    PRE-MEMORY SEGMENT
; This segment must be uncompressed in the ROM image.
;----------------------------------------------------------------------------

AMD_PREMEM_START


;----------------------------------------------------------------------------
; Declare the external routines required in the recovery segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   myReadSPDPremem (Required)
;
;   Entry:
;     ESI - Pointer to an AGESA_READ_SPD_PARAMS structure
;
;     typedef struct {
;       IN OUT AMD_CONFIG_PARAMS StdHeader;
;       IN       UINT8 SocketId;
;       IN       UINT8 MemChannelId;
;       IN       UINT8 DimmId;
;       IN OUT   UINT8 *Buffer;
;       IN OUT   MEM_DATA_STRUCT *MemData;
;     } AGESA_READ_SPD_PARAMS;
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS Indicates the SPD block for the indicated
;                         DIMM was read successfully.
;           AGESA_BOUNDS_CHK The specified DIMM is not present.
;           AGESA_UNSUPPORTED This is a required function, so this
;                             value being returned causes a critical
;                             error response value from the AGESA
;                             software function and no memory initialized.
;           AGESA_ERROR The DIMM SPD read process has generated
;                       communication errors.
;
;   Modified:
;     None
;
;   Purpose:
;     This call out reads a block of memory SPD data and places it
;     into the provided buffer.
;
;   Dependencies:
;     None
;
EXTERN myReadSPDPremem:NEAR

;+-------------------------------------------------------------------------
;
;   AmdDfltRetPremem
;
;   Entry:
;     None
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     Near stub procedure in the prememory segment.  Simply perform a
;     retn instruction.
;
EXTERN AmdDfltRetPremem:NEAR

;+---------------------------------------------------------------------------
;
;   myDoReset (Required)
;
;   Entry:
;     EDX - Reset type
;           1 - Warm reset whenever
;           2 - Cold reset whenever
;           3 - Warm reset immediately
;           4 - Cold reset immediately
;     ESI - Pointer to an AMD_CONFIG_PARAMS structure.
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The function has completed successfully.
;           AGESA_UNSUPPORTED This is a required function, so this
;                             value being returned causes a critical
;                             error response value from the AGESA
;                             software function.
;
;   Modified:
;     None
;
;   Purpose:
;     This host environment function must initiate the specified type
;     of system reset.
;
;     Implementation of this function by the host environment is
;     REQUIRED. Some host environments may record this as a request
;     allowing other elements in the system to perform some additional
;     tasks before the actual reset is issued.
;
;   Dependencies:
;     The AMD processor contains 3 bits (BiosRstDet[2:0]) in a PCI
;     register (F0x6C Link Initialization Control Register) that
;     indicate the reset status. These bits are reserved for use by
;     the AGESA software and should not be modified by the host
;     environment.
;
EXTERN myDoReset:NEAR


;+---------------------------------------------------------------------------
;
;   myGetNonVolatileS3Context (Required for proper S3 operation)
;
;   Entry:
;     None
;
;   Exit:
;     EBX - Pointer to the non-volatile S3 context block
;     ECX - Size in bytes of the non-volatile S3 context block
;
;   Modified:
;     None
;
;   Purpose:
;     The host environment must return the pointer to the data
;     saved during the mySaveNonVolatileS3Context routine.
;
;   Dependencies:
;     None
;
EXTERN myGetNonVolatileS3Context:NEAR


;----------------------------------------------------------------------------
; Declare the optional external routines in the prememory segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   myAgesaHookBeforeExitSelfRefresh (Optional)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - 44h
;     ESI - Pointer to a data structure containing the memory information
;
;   Exit:
;     After returning control to AGESA, AGESA will display: - AGESA_TESTPOINT - 45h
;     EAX - Contains the AGESA_STATUS return code
;           AGESA_SUCCESS The function has completed successfully
;           AGESA_UNSUPPORTED This function is not implemented by the host environment
;           AGESA_WARNING A non-critical issue has occued in the host environment
;
;   Modified:
;     None
;
;   Purpose:
;     General purpose hook called before the exiting self refresh
;     This procedure is called once per channel
;
;     Implementation of this function is optional for the host environment
;     This call-out is an opportunity for the host environment to make dynamic
;     modifications to the memory timing settings specific to the board or host
;     environment before exiting self refresh on S3 resume
;
;   Dependencies:
;     This procedure is called before the exit self refresh bit is set in the resume
;     sequence. The host environment must initiate the OS restart process. This procedure
;     requires a stack. The host environment must establish the stack environment prior
;     to making the call to this procedure
;
EXTERN myAgesaHookBeforeExitSelfRefresh(AmdDfltRetPremem):NEAR


;+---------------------------------------------------------------------------
;
;   myHookBeforeDramInit (Optional)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - 40h
;     ESI - Pointer to a data structure containing the memory information
;
;   Exit:
;     After returning control to AGESA, AGESA will display - AGESA_TESTPOINT - 41h
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The function has completed successfully.
;           AGESA_UNSUPPORTED This function is not implemented by the host environment
;
;   Modified:
;     None
;
;   Purpose:
;     General-purpose hook called before the DRAM_Init bit is set. Called
;     once per MCT
;
;     Implementation of this function is optional for the host environment
;     This call-out is an opportunity for the host environment to make
;     dynamic modifications to the memory timing settings specific to the
;     board or host environment
;
;   Dependencies:
;     None
;
EXTERN myHookBeforeDramInit(AmdDfltRetPremem):NEAR


;+---------------------------------------------------------------------------
;
;   myHookBeforeDQSTraining (Optional)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - 42h
;     ESI - Pointer to a data structure containing the memory information.
;
;   Exit:
;     After returning control to AGESA, AGESA will display - AGESA_TESTPOINT - 43h
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The function has completed successfully.
;           AGESA_UNSUPPORTED This function is not implemented by the
;                             host environment.
;
;   Modified:
;     None
;
;   Purpose:
;     General-purpose hook called just before the memory training processes
;     begin. Called once per MCT.
;
;     Implementation of this function is optional for the host environment.
;     This call-out is an opportunity for the host environment to make
;     dynamic modifications to the memory timing settings specific to the
;     board or host environment.
;
;     The host environment may also use this call-out for some board-
;     specific features that should be activated at this time point,
;     such as:
;       Low voltage DIMMs-the host environment should set the recommended
;       voltages found in the memory data structure for each memory
;       channel. This needs to occur before training begins.
;
;   Dependencies:
;     None
;
EXTERN myHookBeforeDQSTraining(AmdDfltRetPremem):NEAR


;----------------------------------------------------------------------------
; Define the sample wrapper routines for the prememory segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   AmdInitEarlyWrapper
;
;   Entry:
;     On Entry to "AmdInitEarly" AGESA will display AGESA_TESTPOINT - C4h
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdInitEarly" AGESA will display AGESA_TESTPOINT - C5h
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     A full initialization of the processor is performed. Action details
;     differ for the BSP and AP processor cores.
;     For the BSP, the following actions are performed:
;       Full HyperTransportT link initialization, coherent and non-coherent
;       Processor register loading
;       Microcode patch load
;       Errata workaround processing
;       Launch all processor cores
;       Configure the processor power management capabilities
;       Request a warm reset if needed
;     For the AP, the following actions are performed:
;       Processor register loading
;       Microcode patch load
;       Errata workaround processing
;       Configure the processor power management capabilities
;
;   Dependencies:
;     This procedure is expected to be called before main memory initialization
;     and before the system warm reset. Prior to this, the basic configuration
;     done by the AmdInitReset routine must be completed.
;
;     This procedure requires a stack. The host environment must use one of the
;     provided service functions to establish the stack environment prior to
;     making the call to this procedure.
;
;     The processes performed at this time point require communication between
;     processor cores.
;
;     The host environment must recognize that all processor cores are running
;     in parallel and avoid activities that might interfere with the core-to-core
;     communication, such as modifying the MTRR settings or writing to the APIC
;     registers.
;
AmdInitEarlyWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitEarly
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_EARLY
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PreMemHeap
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPremem
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPremem
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, (SIZEOF AMD_CONFIG_PARAMS + (3 * (SIZEOF EXECUTION_CACHE_REGION)))
    call  oemPlatformConfigInit

    ; Call in to the AmdInitEarly entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS The function has completed successfully.
    ;;  AGESA_ALERT An HyperTransportT link CRC error was observed.
    ;;  AGESA_WARNING One of more of the allocation rules were violated,
    ;;                but an adjustment was made and space was allocated.
    ;;                Or a HyperTransport device does not have the expected
    ;;                capabilities, or unusable redundant HyperTransport
    ;;                links were found.
    ;;  AGESA_ERROR One or more of the allocation rules were violated, which
    ;;              resulted in a requested cache region to not be allocated.
    ;;              Or, a HyperTransport device failed to initialize.
    ;;  AGESA_CRITICAL An illegal or unsupported mixture of processor types was
    ;;                 found, or the processors installed were found to have an
    ;;                 insufficient MP capability rating for this platform.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_EARLY_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    ; Allow AGESA to free the space used by AmdInitEarly
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32


    popad
    ret
AmdInitEarlyWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdInitPostWrapper
;
;   Entry:
;     On Entry to "AmdInitPost" AGESA will display AGESA_TESTPOINT - C6h
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdInitPost" AGESA will display AGESA_TESTPOINT - C7h
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     The main system memory is located, initialized, and brought on-line.
;     The processor(s) are prepared for full operation and control by the
;     host environment. Action details differ for the BSP and AP processor
;     cores.
;     For the BSP, the following actions are performed:
;       Full memory initialization and configuration. BSP is the master for
;       this process and may delegate some tasks to APs.
;       AP collection of data for use later.
;       Transfer the HOBs including the artifact data out of the pre-memory
;       cache storage into a temporary holding buffer in the main memory.
;       Check the BIST status of the BSP
;       Shut down the APs.
;       Prepare for the host environment to begin main boot activity.
;       Disable the pre-memory stack.
;     For the APs, the following actions are performed:
;       Report core identity information.
;       Execute indicated memory initialization processes as directed.
;       Check the BIST status of the AP
;       Disable the pre-memory stack.
;       Prepare to halt, giving control to host environment.
;     The entire range of system memory is enabled for Write-Back cache.
;     The fixed MTRRs and the variable MTRRs[7:6] are not changed in order
;     to leave in place any flash ROM region currently set for Write-Protect
;     execution cache.
;
;   Dependencies:
;     This procedure is called after the host environment has determined that
;     a normal boot to operating system should be performed after any system
;     warm reset is completed and after the configuration done by AmdInitEarly
;     has completed.
;
;     This procedure requires a stack. The host environment must use one of the
;     provided service functions to establish the stack environment prior to
;     making the call to this procedure.
;
;     The processes performed at this time point require communication between
;     processor cores.  The host environment must recognize that all processor
;     cores are running in parallel and avoid activities that might interfere
;     with the core-to-core communication, such as modifying the MTRR settings
;     or writing to the APIC registers.
;
AmdInitPostWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitPost
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_POST
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PreMemHeap
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPremem
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPremem
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, SIZEOF AMD_CONFIG_PARAMS
    call  oemPlatformConfigInit

    ; Call in to the AmdInitPost entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS The function has completed successfully.
    ;;  AGESA_ALERT   A BIST error was found on one of the cores.
    ;;  AGESA_WARNING HT Assist feature is running sub-optimally.
    ;;  AGESA_FATAL   Memory initialization failed.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_POST_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    ; Allow AGESA to free the space used by AmdInitPost
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32


    popad
    ret
AmdInitPostWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdInitResumeWrapper
;
;   Entry:
;     On Entry to "AmdInitResume" AGESA will display AGESA_TESTPOINT - D0h
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdInitResume" AGESA will display AGESA_TESTPOINT - D1h
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure initializes or re-initializes the silicon components
;     for the resume boot path.  For the processor, main memory is brought
;     out of self-refresh mode. This procedure will use the context data
;     in the NvStorage area of the input structure to re-start the main
;     memory.  The host environment must fill the AMD_S3_PARAMS NvStorage
;     and VolatileStorage pointers and related size elements to describe
;     the location of the context data. Note that for this procedure, the
;     two data areas do not need to be contained in one buffer zone, they
;     can be anywhere in the accessible memory address space. If the host
;     environment uses a non-volatile storage device accessed on the system
;     address bus such as flashROM, then the context data does not need to
;     be moved prior to this call. If the host environment uses a non-
;     volatile storage device not located on the system address bus (e.g.
;     CMOS or SSEPROM) then the host environment must transfer the context
;     data to a buffer in main memory prior to calling this procedure.
;
;   Dependencies:
;     The host environment must have determined that the system should take
;     the resume path prior to calling this procedure. The configuration
;     done by AmdInitEarly and any necessary warm reset must be complete.
;     After this procedure, execution proceeds to general system restoration.
;
;     This procedure requires a stack. The host environment must use one of
;     the provided service functions to establish the stack environment prior
;     to making the call to this procedure.
;
AmdInitResumeWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitResume
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_RESUME
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PreMemHeap
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPremem
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPremem
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, (SIZEOF AMD_CONFIG_PARAMS + SIZEOF AMD_S3_PARAMS)
    call  oemPlatformConfigInit

    call  myGetNonVolatileS3Context
    mov   (AMD_RESUME_PARAMS ptr [esi]).S3DataBlock.NvStorage, ebx
    mov   (AMD_RESUME_PARAMS ptr [esi]).S3DataBlock.NvStorageSize, ecx

    ; Call in to the AmdInitResume entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS Re-initialization has been completed successfully.
    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_RESUME_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif


    ; Allow AGESA to free the space used by AmdInitResume
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32


    popad
    ret
AmdInitResumeWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdCalloutRouterPremem
;
;   Entry:
;     ECX - Callout function number
;     EDX - Function-specific UINTN
;     ESI - Pointer to function specific data
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     None
;
;   Purpose:
;     The call out router function for AmdInitEarly,
;     AmdInitPost, and AmdInitResume.
;
;   Dependencies:
;     None
;
AmdCalloutRouterPremem PROC FAR PUBLIC USES ECX EBX ESI BX DI DS ES
    xor   ax, ax
    mov   ds, ax
    mov   es, ax
    lea   di, cs:CalloutRouterTablePremem
    mov   eax, AGESA_UNSUPPORTED

loopThruTable:
    cmp   di, OFFSET cs:CalloutRouterTablePrememEnd
    jae   amdCpuCalloutExit ; exit with AGESA_UNSUPPORTED
    cmp   ecx, cs:[di].sOemCallout.FuncName
    je    FoundMatch
    add   di, SIZEOF sOemCallout
    jmp   loopThruTable

FoundMatch:
    mov   bx, cs:[di].sOemCallout.FuncPtr
    call  bx

amdCpuCalloutExit:
    ret
AmdCalloutRouterPremem ENDP


;----------------------------------------------------------------------------
; Define the callout dispatch table for the prememory segment
;----------------------------------------------------------------------------

CalloutRouterTablePremem LABEL BYTE
    ;; Add entries as desired.
    sOemCallout <AGESA_READ_SPD, OFFSET myReadSPDPremem>
    sOemCallout <AGESA_HOOKBEFORE_DRAM_INIT, OFFSET myHookBeforeDramInit>
    sOemCallout <AGESA_HOOKBEFORE_DQS_TRAINING, OFFSET myHookBeforeDQSTraining>
    sOemCallout <AGESA_HOOKBEFORE_EXIT_SELF_REF, OFFSET myAgesaHookBeforeExitSelfRefresh>
    sOemCallout <AGESA_DO_RESET, OFFSET myDoReset>
    sOemCallout <AGESA_EXTERNAL_2D_TRAIN_VREF_CHANGE, OFFSET my2DTrainVrefChange>
CalloutRouterTablePrememEnd LABEL BYTE



AMD_PREMEM_END


;----------------------------------------------------------------------------
;    POST SEGMENT
; This segment may be decompressed and run from system RAM.
;----------------------------------------------------------------------------

AMD_POST_START


;----------------------------------------------------------------------------
; Declare the external routines required in the POST segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   myAllocateBuffer (Required)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - E2h
;     ESI - Pointer to an AGESA_BUFFER_PARAMS structure.
;
;           typedef struct {
;             IN OUT   AMD_CONFIG_PARAMS StdHeader;
;             IN       UINT32 BufferLength;
;             IN       UINT32 BufferHandle;
;                OUT   VOID *BufferPointer;
;           } AGESA_BUFFER_PARAMS;
;
;   Exit:
;     After this hook, AGESA will display - AGESA_TESTPOINT - E3h
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The requested size of memory has been
;                         successfully allocated.
;           AGESA_UNSUPPORTED This is a required function, so this
;                             value being returned causes a critical
;                             error response value from the AGESA
;                             software function.
;           AGESA_ERROR Less than the requested amount of memory
;                       was allocated.
;
;   Modified:
;     EAX
;
;   Purpose:
;     This function is used after main memory has been initialized
;     and the host environment has taken control of memory allocation.
;     This function must allocate a buffer of the requested size or
;     larger. This function is required to be implemented by the host
;     environment.
;
;   Dependencies:
;     The following call-outs must work together in the host system.
;     Parameters of the same name have the same function and must be
;     treated the same in each function:
;       AgesaAllocateBuffer
;       AgesaDeallocateBuffer
;       AgesaLocateBuffer
;       AgesaRunFcnOnAp
;     The host environment may need to reserve a location in the buffer
;     to store any host environment specific value(s). The returned
;     pointer must not include this reserved space. The host environment
;     on the AgesaDeallocateBuffer call needs to account for the reserved
;     space.  This reserved space may be an identifier or the "handle"
;     used to identify the specific memory block.
;
EXTERN myAllocateBuffer:NEAR

;+---------------------------------------------------------------------------
;
;   myDeallocateBuffer (Required)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - E4h
;     ESI - Pointer to an AGESA_BUFFER_PARAMS structure.
;
;           typedef struct {
;             IN OUT   AMD_CONFIG_PARAMS StdHeader;
;             IN       UINT32 BufferLength;
;             IN       UINT32 BufferHandle;
;                OUT   VOID *BufferPointer;
;           } AGESA_BUFFER_PARAMS;
;
;   Exit:
;     After this hook, AGESA will display - AGESA_TESTPOINT - E5h
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The function has completed successfully.
;           AGESA_BOUNDS_CHK The BufferHandle is invalid. The AGESA
;                            software continues with its function.
;           AGESA_UNSUPPORTED This is a required function, so this
;                             value being returned causes a critical
;                             error response value from the AGESA
;                             software function.
;
;   Modified:
;     EAX
;
;   Purpose:
;     This function is used after main memory has been initialized
;     and the host environment has taken control of memory allocation.
;     This function releases a valid working buffer. This function is
;     required for the host environment to implement.
;
;   Dependencies:
;     The following call-outs must work together in the host system.
;     Parameters of the same name have the same function and must be
;     treated the same in each function:
;       AgesaAllocateBuffer
;       AgesaDeallocateBuffer
;       AgesaLocateBuffer
;       AgesaRunFcnOnAp
;
EXTERN myDeallocateBuffer:NEAR

;+---------------------------------------------------------------------------
;
;   myLocateBuffer (Required)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - E6h
;     ESI - Pointer to an AGESA_BUFFER_PARAMS structure.
;
;           typedef struct {
;             IN OUT   AMD_CONFIG_PARAMS StdHeader;
;             IN       UINT32 BufferLength;
;             IN       UINT32 BufferHandle;
;                OUT   VOID *BufferPointer;
;           } AGESA_BUFFER_PARAMS;
;
;   Exit:
;     After this hook, AGESA will display - AGESA_TESTPOINT - E7h
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The function has completed successfully.
;           AGESA_BOUNDS_CHK The presented handle is invalid or the
;                            buffer could not be located.
;
;   Modified:
;     EAX
;
;   Purpose:
;     This function is used after main memory has been initialized
;     and the host environment has taken control of memory allocation.
;     This function must locate the buffer related to the indicated
;     handle and return the address of the buffer and its length.
;     This function is required to be implemented in the host
;     environment.
;
;   Dependencies:
;     The following call-outs must work together in the host system.
;     Parameters of the same name have the same function and must be
;     treated the same in each function:
;       AgesaAllocateBuffer
;       AgesaDeallocateBuffer
;       AgesaLocateBuffer
;       AgesaRunFcnOnAp
;
EXTERN myLocateBuffer:NEAR


;+---------------------------------------------------------------------------
;
;   myRunFuncOnAp (Required)
;
;   Entry:
;     EDX - Local APIC ID of the target core.
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     The host environment must route execution to the target AP and
;     have that AP call the AmdLateRunApTaskWrapper routine defined
;     above.
;
;   Dependencies:
;     None
;
EXTERN myRunFuncOnAp:NEAR

;+---------------------------------------------------------------------------
;
;   mySaveNonVolatileS3Context (Required for proper S3 operation)
;
;   Entry:
;     EBX - Pointer to the non-volatile S3 context block
;     ECX - Size in bytes of the non-volatile S3 context block
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     The host environment must save the non-volatile data to an area
;     that will not lose context while in the ACPI S3 sleep state, but
;     cannot be placed in system RAM.  This data will need to be
;     available during the call to AmdInitResume.
;
;   Dependencies:
;     None
;
EXTERN mySaveNonVolatileS3Context:NEAR

;+---------------------------------------------------------------------------
;
;   mySaveVolatileS3Context (Required for proper S3 operation)
;
;   Entry:
;     EBX - Pointer to the volatile S3 context block
;     ECX - Size in bytes of the volatile S3 context block
;
;   Exit:
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     The host environment must save the volatile data to an area
;     that will not lose context while in the ACPI S3 sleep state.
;     This data will need to be available during the call to
;     AmdS3LateRestore.
;
;   Dependencies:
;     None
;
EXTERN mySaveVolatileS3Context:NEAR

;+---------------------------------------------------------------------------
;
;   myGetVolatileS3Context (Required for proper S3 operation)
;
;   Entry:
;     None
;
;   Exit:
;     EBX - Pointer to the volatile S3 context block
;     ECX - Size in bytes of the volatile S3 context block
;
;   Modified:
;     None
;
;   Purpose:
;     The host environment must return the pointer to the data
;     saved during the mySaveVolatileS3Context routine.
;
;   Dependencies:
;     None
;
EXTERN myGetVolatileS3Context:NEAR


;----------------------------------------------------------------------------
; Define the sample wrapper routines for the POST segment
;----------------------------------------------------------------------------

;+---------------------------------------------------------------------------
;
;   AmdInitEnvWrapper
;
;   Entry:
;     On Entry to "AmdInitEnv" AGESA will display AGESA_TESTPOINT - C8h
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdInitEnv" AGESA will display AGESA_TESTPOINT - C9h
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure uses the AgesaAllocateBuffer call-out to acquire
;     permanent buffer space for the UEFI Hand-Off Blocks (HOBs). This
;     is also known as, or includes, artifact data being used by the
;     AGESA software. Upon entry to this procedure, the data is being
;     held in a temporary memory location and it must be moved to a
;     location controlled and protected by the host environment.
;
;     These actions are performed by the BSP. The APs are not assigned
;     any tasks at this time point.
;
;   Dependencies:
;     This procedure must be called after full memory is initialized and
;     the host environment has taken control of main memory allocation.
;     This procedure should be called before the PCI enumeration takes
;     place and as soon as possible after the host environment memory
;     allocation sub-system has started.
;
;     This procedure requires a stack. The host environment must use one
;     of the provided service functions to establish the stack environment
;     prior to making the call to this procedure.
;
AmdInitEnvWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitEnv
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_ENV
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PostMemDram
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPost
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPost
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, SIZEOF AMD_CONFIG_PARAMS
    call  oemPlatformConfigInit

    ; Call in to the AmdInitEnv entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS The function has completed successfully.
    ;;  AGESA_ERROR The artifact data could not be found or the host
    ;;  environment failed to allocate sufficient buffer space.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_ENV_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    ; Allow AGESA to free the space used by AmdInitEnv
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32


    popad
    ret
AmdInitEnvWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdInitMidWrapper
;
;   Entry:
;     On Entry to "AmdInitMid" AGESA will display AGESA_TESTPOINT - CAh
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdInitMid" AGESA will display AGESA_TESTPOINT - CBh
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure call performs special configuration requirements for
;     the graphics display hardware.
;
;     These actions are performed by the BSP. The APs are not assigned any
;     tasks at this time point.
;
;   Dependencies:
;     This procedure must be called after PCI enumeration has allocated
;     resources, but before the video BIOS call is performed.
;
;     This procedure requires a stack. The host environment must use one
;     of the provided service functions to establish the stack environment
;     prior to making the call to this procedure.
;
AmdInitMidWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitMid
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_MID
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PostMemDram
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPost
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPost
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, SIZEOF AMD_CONFIG_PARAMS
    call  oemPlatformConfigInit

    ; Call in to the AmdInitMid entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS The function has completed successfully.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_MID_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    ; Allow AGESA to free the space used by AmdInitMid
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32


    popad
    ret

AmdInitMidWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdInitLateWrapper
;
;   Entry:
;     On Entry to "AmdInitLate" AGESA will display AGESA_TESTPOINT - CCh
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdInitLate" AGESA will display AGESA_TESTPOINT - CDh
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     The main purpose of this function is to generate informational
;     data tables used by the operating system. The individual tables
;     can be selected for generation through the user selection entries
;     on the input parameters.
;
;     This routine uses the Call-Out AgesaAllocateBuffer to allocate a
;     buffer of the proper size to contain the data.
;
;     The code path separates the BSP from the APs and perform a separate
;     and appropriate list of tasks for each class of core.
;     For the BSP, the following actions are performed:
;       Allocate buffer space for the tables.
;       Generate the table contents.
;       Make sure that the CPU is in a known good power state before
;       proceeding to boot the OS.
;     For the APs, the following actions are performed:
;       Final register settings preparing for entry to OS.
;       Establish the final PState for entry to OS.
;
;   Dependencies:
;     This routine is expected to be executed late in the boot sequence
;     after main memory has been initialized, after PCI enumeration has
;     completed, after the host environment ACPI sub-system has started,
;     after the host environment has taken control of the APs, but just
;     before the start of OS boot.
;
;     The host environment must provide the required call-outs listed in
;     the "Required Call-Out Procedures" section of the AGESA interface
;     specification to provide the buffer space in main memory and execute
;     code on the APs. The host environment must register the created ACPI
;     table in the main ACPI pointer tables. This may require moving the
;     generated tables to another location in memory.
;
;     This procedure requires a stack. The host environment must establish
;     the stack environment prior to making the call to this procedure.
;     Some functions depend upon the preservation of the heap data across
;     the shift from pre-memory environment to a post-memory environment.
;     If that data was not preserved, then those functions cannot complete
;     and an error is returned.
;
AmdInitLateWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdInitLate
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_INIT_LATE
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PostMemDram
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPost
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPost
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, SIZEOF AMD_CONFIG_PARAMS
    call  oemPlatformConfigInit

    ; Call in to the AmdInitLate entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS The function has completed successfully.
    ;;  AGESA_ALERT
    ;;  AGESA_ERROR The system could not allocate the needed amount of
    ;;  buffer space; or could not locate the artifact data block in
    ;;  memory. Likely cause: the host environment may not have preserved
    ;;  the data properly.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_LATE_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    push  es
    mov   ax, SEG AmdAcpiSratPointer
    mov   es, ax

    mov   ebx, (AMD_LATE_PARAMS ptr [esi]).AcpiSrat
    mov   es:AmdAcpiSratPointer, ebx
    mov   eax, DWORD PTR [ebx + 4]
    mov   es:AmdAcpiSratSize, eax

    mov   ebx, (AMD_LATE_PARAMS ptr [esi]).AcpiSlit
    mov   es:AmdAcpiSlitPointer, ebx
    mov   eax, DWORD PTR [ebx + 4]
    mov   es:AmdAcpiSlitSize, eax

    mov   ebx, (AMD_LATE_PARAMS ptr [esi]).AcpiPState
    mov   es:AmdAcpiSsdtPointer, ebx
    mov   eax, DWORD PTR [ebx + 4]
    mov   es:AmdAcpiSsdtSize, eax

    xor   eax, eax

    mov   ebx, (AMD_LATE_PARAMS ptr [esi]).AcpiWheaMce
    mov   es:AmdAcpiWheaMcePointer, ebx
    mov   ax, WORD PTR [ebx]
    mov   es:AmdAcpiWheaMceSize, eax

    mov   ebx, (AMD_LATE_PARAMS ptr [esi]).AcpiWheaMce
    mov   es:AmdAcpiWheaCmcPointer, ebx
    mov   ax, WORD PTR [ebx]
    mov   es:AmdAcpiWheaCmcSize, eax

    mov   eax, (AMD_LATE_PARAMS ptr [esi]).DmiTable
    mov   es:AmdDmiInfoPointer, eax
    pop   es


    ; Allow AGESA to free the space used by AmdInitLate
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32

    popad
    ret

AmdInitLateWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdS3SaveWrapper
;
;   Entry:
;     On Entry to "AmdS3Save" AGESA will display AGESA_TESTPOINT - CEh
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Entry to "AmdS3Save" AGESA will display AGESA_TESTPOINT - CFh
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure saves critical registers and/or configuration
;     information for preservation across a system suspend mode. All
;     actions needed to prepare the processor for suspend mode is
;     performed, however this procedure does NOT initiate the suspend
;     process. The host environment is expected to perform that duty.
;
;     These actions are performed by the BSP. The APs are not assigned
;     any tasks at this time point.
;
;     The initializer routine will NULL out the save area pointers and
;     sizes. This procedure will determine the size of storage needed
;     for all the processor context, and make a call out to the environment
;     for allocation of one buffer to store all of the data. Upon exit, the
;     pointers and sizes within the AMD_S3_PARAMS structure will be updated
;     with the appropriate addresses within the buffer that was allocated.
;     The host environment is expected to then transfer the data pointed to
;     by NvStorage to a non-volatile storage area, and the data pointed to
;     by VolatileStorage to either a non-volatile storage area or system
;     RAM that retains its content across suspend.
;
;   Dependencies:
;     The host environment must initiate the suspend process.
;
;     This procedure requires a stack. The host environment must establish
;     the stack environment prior to making the call to this procedure.
;
AmdS3SaveWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdS3Save
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_S3_SAVE
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PostMemDram
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPost
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPost
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, (SIZEOF AMD_CONFIG_PARAMS + SIZEOF AMD_S3_PARAMS)
    call  oemPlatformConfigInit

    ; Call in to the AmdS3Save entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS All suspend duties have been completed successfully.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_S3SAVE_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    mov   ecx, (AMD_S3SAVE_PARAMS ptr [esi]).S3DataBlock.NvStorageSize
    .if (ecx != 0)
        mov   ebx, (AMD_S3SAVE_PARAMS ptr [esi]).S3DataBlock.NvStorage
        call  mySaveNonVolatileS3Context
    .endif

    mov   ecx, (AMD_S3SAVE_PARAMS ptr [esi]).S3DataBlock.VolatileStorageSize
    .if (ecx != 0)
        mov   ebx, (AMD_S3SAVE_PARAMS ptr [esi]).S3DataBlock.VolatileStorage
        call  mySaveVolatileS3Context
    .endif

    ; Allow AGESA to free the space used by AmdS3Save
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32

    popad
    ret

AmdS3SaveWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdS3LateRestoreWrapper
;
;   Entry:
;     On Entry to "AmdS3LateRestore" AGESA will display AGESA_TESTPOINT - D2h
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     On Exit from "AmdS3LateRestore" AGESA will display AGESA_TESTPOINT - D3h
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This procedure restores the processor state, reloads critical
;     silicon component registers, and performs any re-initialization
;     required by the silicon. This procedure will use the context data
;     in the VolatileStorage area of the input structure to restore the
;     processor registers.
;
;     The host environment must fill the AMD_S3_PARAMS NvStorage and
;     VolatileStorage pointers and related size elements to describe
;     the location of the context data. Note that for this procedure,
;     the two data areas do not need to be contained in one buffer zone,
;     they can be anywhere in the accessible memory address space. If
;     the host environment uses a non-volatile storage device accessed
;     on the system address bus such as flashROM, then the context data
;     does not need to be moved prior to this call. If the host
;     environment uses a non-volatile storage device not located on the
;     system address bus (e.g. CMOS or SSEPROM) then the host environment
;     must transfer the context data to a buffer in main memory prior to
;     calling this procedure.
;
;     These actions are performed by the BSP. The APs are not assigned
;     any tasks at this time point.
;
;   Dependencies:
;     This procedure is called late in the resume sequence, after the
;     PCI control space is restored and just before resuming operating
;     system execution.
;
;     The host environment must initiate the OS restart process.
;
;     This procedure requires a stack. The host environment must establish
;     the stack environment prior to making the call to this procedure.
;
AmdS3LateRestoreWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdS3LateRestore
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_S3LATE_RESTORE
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PostMemDram
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPost
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPost
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    mov   edi, esi
    add   edi, (SIZEOF AMD_CONFIG_PARAMS + SIZEOF AMD_S3_PARAMS)
    call  oemPlatformConfigInit

    call  myGetVolatileS3Context
    mov   (AMD_S3LATE_PARAMS ptr [esi]).S3DataBlock.VolatileStorage, ebx
    mov   (AMD_S3LATE_PARAMS ptr [esi]).S3DataBlock.VolatileStorageSize, ecx

    ; Call in to the AmdS3LateRestore entry point
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    ;;  AGESA_SUCCESS All resume processes have been completed successfully.

    .if (eax != AGESA_SUCCESS)
        mov   al, (AMD_S3LATE_PARAMS ptr [esi]).StdHeader.HeapStatus
        mov   ebx, AGESA_B2_ADDRESS
        call  AmdProcessAgesaErrors
    .endif

    ; Allow AGESA to free the space used by AmdS3LateRestore
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32

    popad
    ret
AmdS3LateRestoreWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdLateRunApTaskWrapper
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - D4h
;     DS - 0000 with 4 gigabyte access
;     ES - 0000 with 4 gigabyte access
;
;   Exit:
;     After this hook, AGESA will display - AGESA_TESTPOINT - D5h
;     None
;
;   Modified:
;     None
;
;   Purpose:
;     This entry point is tightly connected with the "AgesaRunFcnOnAp"
;     call out.  The AGESA software will call the call-out "AgesaRunFcnOnAp";
;     the host environment will then call this entry point to have the AP
;     execute the requested function. This is needed late in the Post and
;     Resume branches for running an AP task since the AGESA software has
;     relinquished control of the APs to the host environment.
;
;   Dependencies:
;     The host environment must implement the"AgesaRunFcnOnAp" call-out
;     and route execution to the target AP.
;
AmdLateRunApTaskWrapper PROC NEAR PUBLIC
    local localCfgBlock:AMD_INTERFACE_PARAMS

    pushad

    ; Prepare for the call to create and initialize the input parameters for AmdLateRunApTask
    xor   eax, eax
    mov   ax, ss
    shl   eax, 4
    lea   esi, localCfgBlock
    add   esi, eax
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.ImageBasePtr, AGESA_B2_ADDRESS
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_CREATE_STRUCT
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.AltImageBasePtr, 0
    mov   edx, SEG AmdCallout16
    shl   edx, 4
    add   edx, OFFSET AmdCallout16
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.CalloutPtr, edx

    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AgesaFunctionName, AMD_LATE_RUN_AP_TASK
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).AllocationMethod, PostMemDram
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).NewStructSize, 0
    push  esi
    mov   dx, SEG AmdCalloutRouterPost
    shl   edx, 16
    mov   dx, OFFSET AmdCalloutRouterPost
    push  edx
    call  AmdBridge32
    pop   edx

    mov   esi, (AMD_INTERFACE_PARAMS ptr [esi]).NewStructPtr

    ; The structure has been initialized.  Now modify the default settings as desired.

    push  es
    mov   ax, SEG AmdRunCodeOnApDataPointer
    mov   es, ax
    mov   eax, es:AmdRunCodeOnApDataPointer
    mov   (AP_EXE_PARAMS PTR [esi]).RelatedDataBlock, eax
    mov   eax, es:AmdRunCodeOnApDataSize
    mov   (AP_EXE_PARAMS PTR [esi]).RelatedBlockLength, eax
    mov   eax, es:AmdRunCodeOnApFunction
    mov   (AP_EXE_PARAMS PTR [esi]).FunctionNumber, eax
    pop   es

    ; Call in to the AmdLateRunApTask dispatcher
    push  edx
    call  AmdBridge32
    pop   edx

    ;;  EAX = AGESA_STATUS
    push  es
    mov   bx, SEG AmdRunCodeOnApStatus
    mov   es, bx
    mov   es:AmdRunCodeOnApStatus, eax
    pop   es

    ; Allow AGESA to free the space used by AmdLateRunApTask
    pop   esi
    mov   (AMD_INTERFACE_PARAMS ptr [esi]).StdHeader.Func, AMD_RELEASE_STRUCT
    call  AmdBridge32

    popad
    ret

AmdLateRunApTaskWrapper ENDP


;+---------------------------------------------------------------------------
;
;   AmdRunFuncOnAp (Required)
;
;   Entry:
;     Prior to this hook, AGESA will display - AGESA_TESTPOINT - E8h
;     EDX - Local APIC ID of the target core.
;     ESI - Pointer to an AP_EXE_PARAMS structure.
;
;           typedef struct {
;             IN OUT   AMD_CONFIG_PARAMS StdHeader;
;             IN       UINT32 FunctionNumber;
;             IN       VOID *RelatedDataBlock;
;             IN       UINT32 RelatedDataBlockLength;
;           } AP_EXE_PARAMS;
;
;   Exit:
;     After this hook, AGESA will display - AGESA_TESTPOINT - E9h
;     EAX - Contains the AGESA_STATUS return code.
;           AGESA_SUCCESS The function has completed successfully.
;           AGESA_UNSUPPORTED This is a required function, so this value
;                             being returned causes a critical error
;                             response value from the AGESAT software
;                             function and no memory initialized.
;           AGESA_WARNING The AP did not respond.
;
;   Modified:
;     EAX
;
;   Purpose:
;     This function is used after main memory has been initialized
;     and the host environment has taken control of AP task dispatching.
;     This function must cause the indicated function code to be executed
;     upon the specified Application Processor. This procedure must be
;     executed in 32-bit mode. This function is required to be implemented
;     in the host environment.
;
;   Dependencies:
;     The host environment must route execution to the target AP and
;     have that AP call the"AmdLateRunApTask" entry point.
;
AmdRunFuncOnAp PROC NEAR PUBLIC

    push  es
    mov   ax, SEG AmdRunCodeOnApDataPointer
    mov   es, ax
    mov   eax, (AP_EXE_PARAMS PTR [esi]).RelatedDataBlock
    mov   es:AmdRunCodeOnApDataPointer, eax
    mov   eax, (AP_EXE_PARAMS PTR [esi]).RelatedBlockLength
    mov   es:AmdRunCodeOnApDataSize, eax
    mov   eax, (AP_EXE_PARAMS PTR [esi]).FunctionNumber
    mov   es:AmdRunCodeOnApFunction, eax
    mov   eax, AGESA_UNSUPPORTED
    mov   es:AmdRunCodeOnApStatus, eax
    pop   es

    call  myRunFuncOnAp

    push  es
    mov   ax, SEG AmdRunCodeOnApStatus
    mov   es, ax
    mov   eax, es:AmdRunCodeOnApStatus
    pop   es
    ret
AmdRunFuncOnAp ENDP



;+---------------------------------------------------------------------------
;
;   AmdCalloutRouterPost
;
;   Entry:
;     ECX - Callout function number
;     EDX - Function-specific UINTN
;     ESI - Pointer to function specific data
;
;   Exit:
;     EAX - Contains the AGESA_STATUS return code.
;
;   Modified:
;     None
;
;   Purpose:
;     The call out router function for AmdInitEnv,
;     AmdInitMid, AmdInitLate, AmdS3Save, and
;     AmdS3LateRestore.
;
;   Dependencies:
;     None
;
AmdCalloutRouterPost PROC FAR PUBLIC USES ECX EBX ESI BX DI DS ES
    xor   ax, ax
    mov   ds, ax
    mov   es, ax
    lea   di, cs:CalloutRouterTablePost
    mov   eax, AGESA_UNSUPPORTED

loopThruTable:
    cmp   di, OFFSET cs:CalloutRouterTablePostEnd
    jae   amdCpuCalloutExit ; exit with AGESA_UNSUPPORTED
    cmp   ecx, cs:[di].sOemCallout.FuncName
    je    FoundMatch
    add   di, SIZEOF sOemCallout
    jmp   loopThruTable

FoundMatch:
    mov   bx, cs:[di].sOemCallout.FuncPtr
    call  bx

amdCpuCalloutExit:
    ret
AmdCalloutRouterPost ENDP


;----------------------------------------------------------------------------
; Define the callout dispatch table for the POST segment
;----------------------------------------------------------------------------

CalloutRouterTablePost LABEL BYTE
    ;; Add entries as desired.
    sOemCallout <AGESA_ALLOCATE_BUFFER, OFFSET myAllocateBuffer>
    sOemCallout <AGESA_DEALLOCATE_BUFFER, OFFSET myDeallocateBuffer>
    sOemCallout <AGESA_LOCATE_BUFFER, OFFSET myLocateBuffer>
    sOemCallout <AGESA_RUNFUNC_ONAP, OFFSET AmdRunFuncOnAp>
CalloutRouterTablePostEnd LABEL BYTE

AMD_POST_END


;----------------------------------------------------------------------------
;    CPU DATA SEGMENT
; This segment must be writable, and present at the time that
; AmdInitLate is run.
;----------------------------------------------------------------------------

CPU_DATASEG_START

  ;; Data used to store pointers for later use by the host environment.
  PUBLIC AmdAcpiSratPointer
  PUBLIC AmdAcpiSratSize
  PUBLIC AmdAcpiSlitPointer
  PUBLIC AmdAcpiSlitSize
  PUBLIC AmdAcpiSsdtPointer
  PUBLIC AmdAcpiSsdtSize
  PUBLIC AmdAcpiWheaMcePointer
  PUBLIC AmdAcpiWheaMceSize
  PUBLIC AmdAcpiWheaCmcPointer
  PUBLIC AmdAcpiWheaCmcSize
  PUBLIC AmdDmiInfoPointer
  AmdAcpiSratPointer     DWORD ?
  AmdAcpiSratSize        DWORD ?
  AmdAcpiSlitPointer     DWORD ?
  AmdAcpiSlitSize        DWORD ?
  AmdAcpiSsdtPointer     DWORD ?
  AmdAcpiSsdtSize        DWORD ?
  AmdAcpiWheaMcePointer  DWORD ?
  AmdAcpiWheaMceSize     DWORD ?
  AmdAcpiWheaCmcPointer  DWORD ?
  AmdAcpiWheaCmcSize     DWORD ?
  AmdDmiInfoPointer      DWORD ?

  ;; Data used for communication between the AP and the BSP.
  PUBLIC AmdRunCodeOnApDataPointer
  PUBLIC AmdRunCodeOnApDataSize
  PUBLIC AmdRunCodeOnApFunction
  PUBLIC AmdRunCodeOnApStatus
  AmdRunCodeOnApDataPointer  DWORD ?
  AmdRunCodeOnApDataSize     DWORD ?
  AmdRunCodeOnApFunction     DWORD ?
  AmdRunCodeOnApStatus       DWORD ?

CPU_DATASEG_END


END
