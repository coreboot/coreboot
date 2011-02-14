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
; * @e \$Revision: 33891 $   @e \$Date: 2010-06-26 00:22:54 +0800 (Sat, 26 Jun 2010) $
;
;*****************************************************************************
; 
;  Copyright (c) 2011, Advanced Micro Devices, Inc.
;  All rights reserved.
;  
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
;      * Redistributions of source code must retain the above copyright
;        notice, this list of conditions and the following disclaimer.
;      * Redistributions in binary form must reproduce the above copyright
;        notice, this list of conditions and the following disclaimer in the
;        documentation and/or other materials provided with the distribution.
;      * Neither the name of Advanced Micro Devices, Inc. nor the names of 
;        its contributors may be used to endorse or promote products derived 
;        from this software without specific prior written permission.
;  
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;  DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
;  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;  
;*****************************************************************************


UINT64    TEXTEQU     <QWORD>
UINT32    TEXTEQU     <DWORD>
UINT16    TEXTEQU     <WORD>
UINT8     TEXTEQU     <BYTE>
CHAR8     TEXTEQU     <BYTE>
BOOLEAN   TEXTEQU     <BYTE>
POINTER   TEXTEQU     <DWORD>

 ; AGESA Types and Definitions

    AGESA_REVISION              EQU    "Arch2008"
    AGESA_ID                    EQU    "AGESA"

    LAST_ENTRY                  EQU    0FFFFFFFFh
    IMAGE_SIGNATURE             EQU    'DMA$'
    IOCF8                       EQU    0CF8h
    IOCFC                       EQU    0CFCh

 ; The return status for all AGESA public services.

 ; Services return the most severe status of any logged event.  Status other than SUCCESS, UNSUPPORTED, and BOUNDS_CHK
 ; will have log entries with more detail.

  AGESA_SUCCESS               EQU    0        ; < The service completed normally. Info may be logged.
  AGESA_UNSUPPORTED           EQU    1        ; < The dispatcher or create struct had an unimplemented function requested.
                                              ; <  Not logged.
  AGESA_BOUNDS_CHK            EQU    2        ; < A dynamic parameter was out of range and the service was not provided.
                                              ; < Example, memory address not installed, heap buffer handle not found.
                                              ; < Not Logged.
 ; AGESA_STATUS of greater severity (the ones below this line), always have a log entry available.
  AGESA_ALERT                 EQU    3        ; < An observed condition, but no loss of function.
                                              ; <  See log.  Example, HT CRC.
  AGESA_WARNING               EQU    4        ; < Possible or minor loss of function.  See Log.
  AGESA_ERROR                 EQU    5        ; < Significant loss of function, boot may be possible.  See Log.
  AGESA_CRITICAL              EQU    6        ; < Continue boot only to notify user.  See Log.
  AGESA_FATAL                 EQU    7        ; < Halt booting.  See Log.
  AgesaStatusMax              EQU    8        ; < Not a status, use for limit checking.
AGESA_STATUS        TEXTEQU    <DWORD>

; For checking whether a status is at or above the mandatory log level.
AGESA_STATUS_LOG_LEVEL         EQU   AGESA_ALERT

    CALLOUT_ENTRY       TEXTEQU    <POINTER>
    IMAGE_ENTRY         TEXTEQU    <POINTER>
    MODULE_ENTRY        TEXTEQU    <POINTER>

; This allocation type is used by the AmdCreateStruct entry point
  PreMemHeap            EQU    0        ; < Create heap in cache.
  PostMemDram           EQU    1        ; < Create heap in memory.
  ByHost                EQU    2        ; < Create heap by Host.
ALLOCATION_METHOD  TEXTEQU  <DWORD>

 ; These width descriptors are used by the library function, and others, to specify the data size
  AccessWidth8          EQU    1        ; < Access width is 8 bits.
  AccessWidth16         EQU    2        ; < Access width is 16 bits.
  AccessWidth32         EQU    3        ; < Access width is 32 bits.
  AccessWidth64         EQU    4        ; < Access width is 64 bits.

  AccessS3SaveWidth8    EQU    81h      ; < Save 8 bits data.
  AccessS3SaveWidth16   EQU    130      ; < Save 16 bits data.
  AccessS3SaveWidth32   EQU    131      ; < Save 32 bits data.
  AccessS3SaveWidth64   EQU    132      ; < Save 64 bits data.
ACCESS_WIDTH  TEXTEQU  <DWORD>

 ; AGESA struct name

   ; AGESA BASIC FUNCTIONS
  AMD_INIT_RECOVERY     EQU    00020000h
  AMD_CREATE_STRUCT     EQU    00020001h
  AMD_INIT_EARLY        EQU    00020002h
  AMD_INIT_ENV          EQU    00020003h
  AMD_INIT_LATE         EQU    00020004h
  AMD_INIT_MID          EQU    00020005h
  AMD_INIT_POST         EQU    00020006h
  AMD_INIT_RESET        EQU    00020007h
  AMD_INIT_RESUME       EQU    00020008h
  AMD_RELEASE_STRUCT    EQU    00020009h
  AMD_S3LATE_RESTORE    EQU    0002000Ah
  AMD_S3_SAVE           EQU    0002000Bh
  AMD_GET_APIC_ID       EQU    0002000Ch
  AMD_GET_PCI_ADDRESS   EQU    0002000Dh
  AMD_IDENTIFY_CORE     EQU    0002000Eh
  AMD_READ_EVENT_LOG    EQU    0002000Fh
  AMD_GET_EXECACHE_SIZE EQU    00020010h
  AMD_LATE_RUN_AP_TASK  EQU    00020011h
  AMD_IDENTIFY_DIMMS    EQU    00020012h
AGESA_STRUCT_NAME  TEXTEQU  <DWORD>


  ; ResetType constant values
    WARM_RESET_WHENEVER         EQU    1
    COLD_RESET_WHENEVER         EQU    2
    WARM_RESET_IMMEDIATELY      EQU    3
    COLD_RESET_IMMEDIATELY      EQU    4


 ; AGESA Structures

 ; The standard header for all AGESA services.
AMD_CONFIG_PARAMS    STRUCT
  ImageBasePtr                  UINT32 ?        ; < The AGESA Image base address.
  Func                          UINT32 ?        ; < The service desired, @sa dispatch.h.
  AltImageBasePtr               UINT32 ?        ; < Alternate Image location
  CalloutPtr                    CALLOUT_ENTRY ?         ; < For Callout from AGESA
  HeapStatus                    UINT8 ?         ; < For heap status from boot time slide.
  HeapBasePtr                   UINT64 ?        ; < Location of the heap
  Reserved                      UINT8 (7) DUP (?)      ; < This space is reserved for future use.
AMD_CONFIG_PARAMS    ENDS


 ; Create Struct Interface.
AMD_INTERFACE_PARAMS    STRUCT
  StdHeader                     AMD_CONFIG_PARAMS {}    ; < Config header
  AgesaFunctionName             AGESA_STRUCT_NAME ?     ; < The service to init, @sa dispatch.h
  AllocationMethod              ALLOCATION_METHOD ?     ; < How to handle buffer allocation
  NewStructSize                 UINT32 ?        ; < The size of the allocated data, in for ByHost, else out only.
  NewStructPtr                  POINTER ?       ; < The struct for the service.
                                                   ; < The struct to init for ByHost allocation,
                                                   ; < the initialized struct on return.
AMD_INTERFACE_PARAMS    ENDS

    FUNC_0                      EQU    0        ; bit-placed for PCI address creation
    FUNC_1                      EQU    1
    FUNC_2                      EQU    2
    FUNC_3                      EQU    3
    FUNC_4                      EQU    4
    FUNC_5                      EQU    5
    FUNC_6                      EQU    6
    FUNC_7                      EQU    7

 ; AGESA Binary module header structure
AMD_IMAGE_HEADER    STRUCT
  Signature                     UINT32 ?        ; < Binary Signature
  CreatorID                     CHAR8 (8) DUP (?)       ; < 8 characters ID
  Version                       CHAR8 (12) DUP (?)      ; < 12 characters version
  ModuleInfoOffset              UINT32 ?        ; < Offset of module
  EntryPointAddress             UINT32 ?        ; < Entry address
  ImageBase                     UINT32 ?        ; < Image base
  RelocTableOffset              UINT32 ?        ; < Relocate Table offset
  ImageSize                     UINT32 ?        ; < Size
  Checksum                      UINT16 ?        ; < Checksum
  ImageType                     UINT8 ?         ; < Type
  V_Reserved                    UINT8 ?         ; < Reserved
AMD_IMAGE_HEADER    ENDS
 ; AGESA Binary module header structure
AMD_MODULE_HEADER    STRUCT
  ModuleHeaderSignature         UINT32 ?        ; < Module signature
  ModuleIdentifier              CHAR8 (8) DUP (?)       ; < 8 characters ID
  ModuleVersion                 CHAR8 (12) DUP (?)      ; < 12 characters version
  ModuleDispatcher              POINTER ?       ; < A pointer point to dispatcher
  NextBlock                     POINTER ?       ; < Next module header link
AMD_MODULE_HEADER    ENDS

; AMD_CODE_HEADER Signatures.
AGESA_CODE_SIGNATURE    TEXTEQU  <'!', '!', 'A', 'G', 'E', 'S', 'A', ' '>
CIMXNB_CODE_SIGNATURE   TEXTEQU  <'!', '!', 'C', 'I', 'M', 'X', 'N', 'B'>
CIMXSB_CODE_SIGNATURE   TEXTEQU  <'!', '!', 'C', 'I', 'M', 'X', 'S', 'B'>

; AGESA_CODE_SIGNATURE
AMD_CODE_HEADER    STRUCT
  Signature             CHAR8 (8) DUP (?)           ; < code header Signature
  ComponentName         CHAR8 (8) DUP (?)           ; < 8 character name of the code module
  Version               CHAR8 (12) DUP (?)          ; < 12 character version string
  TerminatorNull        CHAR8 ?                     ; < null terminated string
  VerReserved           CHAR8 (7) DUP (?)           ; < reserved space
AMD_CODE_HEADER    ENDS

 ; Extended PCI address format
EXT_PCI_ADDR    STRUCT
  Register                      UINT32 ?
;   IN OUT  UINT32      Register:12;                           ; < Register offset
;   IN OUT  UINT32      Function:3;                     ; < Function number
;   IN OUT  UINT32      Device:5;                       ; < Device number
;   IN OUT  UINT32      Bus:8;                          ; < Bus number
;   IN OUT  UINT32      Segment:4;                      ; < Segment
EXT_PCI_ADDR    ENDS

 ; Union type for PCI address
PCI_ADDR    UNION
  AddressValue                  UINT32 ?        ; < Formal address
  Address                       EXT_PCI_ADDR {}         ; < Extended address
PCI_ADDR    ENDS

   ; SBDFO - Segment Bus Device Function Offset
   ; 31:28   Segment (4-bits)
   ; 27:20   Bus     (8-bits)
   ; 19:15   Device  (5-bits)
   ; 14:12   Function(3-bits)
   ; 11:00   Offset  (12-bits)



    ILLEGAL_SBDFO               EQU    0FFFFFFFFh

 ; CPUID data received registers format
CPUID_DATA    STRUCT
  EAX_Reg                       UINT32 ?        ; < CPUID instruction result in EAX
  EBX_Reg                       UINT32 ?        ; < CPUID instruction result in EBX
  ECX_Reg                       UINT32 ?        ; < CPUID instruction result in ECX
  EDX_Reg                       UINT32 ?        ; < CPUID instruction result in EDX
CPUID_DATA    ENDS

 ; HT frequency for external callbacks
;typedef enum {
  HT_FREQUENCY_200M     EQU    0        ; < HT speed 200 for external callbacks
  HT_FREQUENCY_400M     EQU    2        ; < HT speed 400 for external callbacks
  HT_FREQUENCY_600M     EQU    4        ; < HT speed 600 for external callbacks
  HT_FREQUENCY_800M     EQU    5        ; < HT speed 800 for external callbacks
  HT_FREQUENCY_1000M    EQU    6        ; < HT speed 1000 for external callbacks
  HT_FREQUENCY_1200M    EQU    7        ; < HT speed 1200 for external callbacks
  HT_FREQUENCY_1400M    EQU    8        ; < HT speed 1400 for external callbacks
  HT_FREQUENCY_1600M    EQU    9        ; < HT speed 1600 for external callbacks
  HT_FREQUENCY_1800M    EQU    10       ; < HT speed 1800 for external callbacks
  HT_FREQUENCY_2000M    EQU    11       ; < HT speed 2000 for external callbacks
  HT_FREQUENCY_2200M    EQU    12       ; < HT speed 2200 for external callbacks
  HT_FREQUENCY_2400M    EQU    13       ; < HT speed 2400 for external callbacks
  HT_FREQUENCY_2600M    EQU    14       ; < HT speed 2600 for external callbacks
  HT_FREQUENCY_2800M    EQU    17       ; < HT speed 2800 for external callbacks
  HT_FREQUENCY_3000M    EQU    18       ; < HT speed 3000 for external callbacks
  HT_FREQUENCY_3200M    EQU    19       ; < HT speed 3200 for external callbacks
  HT_FREQUENCY_MAX      EQU    20       ; < Limit Check.
HT_FREQUENCIES  TEXTEQU  <DWORD> ;} HT_FREQUENCIES;

HT3_FREQUENCY_MIN     EQU    HT_FREQUENCY_1200M

IFNDEF   BIT0
    BIT0                        EQU    0000000000000001h
ENDIF
IFNDEF   BIT1
    BIT1                        EQU    0000000000000002h
ENDIF
IFNDEF   BIT2
    BIT2                        EQU    0000000000000004h
ENDIF
IFNDEF   BIT3
    BIT3                        EQU    0000000000000008h
ENDIF
IFNDEF   BIT4
    BIT4                        EQU    0000000000000010h
ENDIF
IFNDEF   BIT5
    BIT5                        EQU    0000000000000020h
ENDIF
IFNDEF   BIT6
    BIT6                        EQU    0000000000000040h
ENDIF
IFNDEF   BIT7
    BIT7                        EQU    0000000000000080h
ENDIF
IFNDEF   BIT8
    BIT8                        EQU    0000000000000100h
ENDIF
IFNDEF   BIT9
    BIT9                        EQU    0000000000000200h
ENDIF
IFNDEF   BIT10
    BIT10                       EQU    0000000000000400h
ENDIF
IFNDEF   BIT11
    BIT11                       EQU    0000000000000800h
ENDIF
IFNDEF   BIT12
    BIT12                       EQU    0000000000001000h
ENDIF
IFNDEF   BIT13
    BIT13                       EQU    0000000000002000h
ENDIF
IFNDEF   BIT14
    BIT14                       EQU    0000000000004000h
ENDIF
IFNDEF   BIT15
    BIT15                       EQU    0000000000008000h
ENDIF
IFNDEF   BIT16
    BIT16                       EQU    0000000000010000h
ENDIF
IFNDEF   BIT17
    BIT17                       EQU    0000000000020000h
ENDIF
IFNDEF   BIT18
    BIT18                       EQU    0000000000040000h
ENDIF
IFNDEF   BIT19
    BIT19                       EQU    0000000000080000h
ENDIF
IFNDEF   BIT20
    BIT20                       EQU    0000000000100000h
ENDIF
IFNDEF   BIT21
    BIT21                       EQU    0000000000200000h
ENDIF
IFNDEF   BIT22
    BIT22                       EQU    0000000000400000h
ENDIF
IFNDEF   BIT23
    BIT23                       EQU    0000000000800000h
ENDIF
IFNDEF   BIT24
    BIT24                       EQU    0000000001000000h
ENDIF
IFNDEF   BIT25
    BIT25                       EQU    0000000002000000h
ENDIF
IFNDEF   BIT26
    BIT26                       EQU    0000000004000000h
ENDIF
IFNDEF   BIT27
    BIT27                       EQU    0000000008000000h
ENDIF
IFNDEF   BIT28
    BIT28                       EQU    0000000010000000h
ENDIF
IFNDEF   BIT29
    BIT29                       EQU    0000000020000000h
ENDIF
IFNDEF   BIT30
    BIT30                       EQU    0000000040000000h
ENDIF
IFNDEF   BIT31
    BIT31                       EQU    0000000080000000h
ENDIF
IFNDEF   BIT32
    BIT32                       EQU    0000000100000000h
ENDIF
IFNDEF   BIT33
    BIT33                       EQU    0000000200000000h
ENDIF
IFNDEF   BIT34
    BIT34                       EQU    0000000400000000h
ENDIF
IFNDEF   BIT35
    BIT35                       EQU    0000000800000000h
ENDIF
IFNDEF   BIT36
    BIT36                       EQU    0000001000000000h
ENDIF
IFNDEF   BIT37
    BIT37                       EQU    0000002000000000h
ENDIF
IFNDEF   BIT38
    BIT38                       EQU    0000004000000000h
ENDIF
IFNDEF   BIT39
    BIT39                       EQU    0000008000000000h
ENDIF
IFNDEF   BIT40
    BIT40                       EQU    0000010000000000h
ENDIF
IFNDEF   BIT41
    BIT41                       EQU    0000020000000000h
ENDIF
IFNDEF   BIT42
    BIT42                       EQU    0000040000000000h
ENDIF
IFNDEF   BIT43
    BIT43                       EQU    0000080000000000h
ENDIF
IFNDEF   BIT44
    BIT44                       EQU    0000100000000000h
ENDIF
IFNDEF   BIT45
    BIT45                       EQU    0000200000000000h
ENDIF
IFNDEF   BIT46
    BIT46                       EQU    0000400000000000h
ENDIF
IFNDEF   BIT47
    BIT47                       EQU    0000800000000000h
ENDIF
IFNDEF   BIT48
    BIT48                       EQU    0001000000000000h
ENDIF
IFNDEF   BIT49
    BIT49                       EQU    0002000000000000h
ENDIF
IFNDEF   BIT50
    BIT50                       EQU    0004000000000000h
ENDIF
IFNDEF   BIT51
    BIT51                       EQU    0008000000000000h
ENDIF
IFNDEF   BIT52
    BIT52                       EQU    0010000000000000h
ENDIF
IFNDEF   BIT53
    BIT53                       EQU    0020000000000000h
ENDIF
IFNDEF   BIT54
    BIT54                       EQU    0040000000000000h
ENDIF
IFNDEF   BIT55
    BIT55                       EQU    0080000000000000h
ENDIF
IFNDEF   BIT56
    BIT56                       EQU    0100000000000000h
ENDIF
IFNDEF   BIT57
    BIT57                       EQU    0200000000000000h
ENDIF
IFNDEF   BIT58
    BIT58                       EQU    0400000000000000h
ENDIF
IFNDEF   BIT59
    BIT59                       EQU    0800000000000000h
ENDIF
IFNDEF   BIT60
    BIT60                       EQU    1000000000000000h
ENDIF
IFNDEF   BIT61
    BIT61                       EQU    2000000000000000h
ENDIF
IFNDEF   BIT62
    BIT62                       EQU    4000000000000000h
ENDIF
IFNDEF   BIT63
    BIT63                       EQU    8000000000000000h
ENDIF

