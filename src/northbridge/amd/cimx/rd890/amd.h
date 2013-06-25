/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _AMD_H_
#define _AMD_H_

#include <cpu/amd/common/cbtypes.h>

#define VOLATILE  volatile
#define CALLCONV
#define ROMDATA
#define CIMXAPI   EFIAPI

//
//
// AGESA Types and Definitions
//
//
#include <stddef.h>

#define LAST_ENTRY 0xFFFFFFFF
#define IOCF8 0xCF8
#define IOCFC 0xCFC
#define IN
#define OUT
#define IMAGE_SIGNATURE 'DMA$'

typedef UINTN AGESA_STATUS;


#define AGESA_SUCCESS       ((AGESA_STATUS) 0x0)
#define AGESA_ALERT         ((AGESA_STATUS) 0x40000000)
#define AGESA_WARNING       ((AGESA_STATUS) 0x40000001)
#define AGESA_UNSUPPORTED   ((AGESA_STATUS) 0x80000003)
#define AGESA_ERROR         ((AGESA_STATUS) 0xC0000001)
#define AGESA_CRITICAL      ((AGESA_STATUS) 0xC0000002)
#define AGESA_FATAL         ((AGESA_STATUS) 0xC0000003)

typedef AGESA_STATUS (*CALLOUT_ENTRY) (UINT32 Param1, UINTN Param2, VOID* ConfigPtr);
typedef AGESA_STATUS (*IMAGE_ENTRY) (IN OUT VOID* ConfigPtr);
typedef AGESA_STATUS (*MODULE_ENTRY) (IN OUT VOID* ConfigPtr);

///This allocation type is used by the AmdCreateStruct entry point
typedef enum {
  PreMemHeap = 0,                                           ///< Create heap in cache.
  PostMemDram,                                              ///< Create heap in memory.
  ByHost                                                    ///< Create heap by Host.
} ALLOCATION_METHOD;

/// These width descriptors are used by the library function, and others, to specify the data size
typedef enum ACCESS_WIDTH {
  AccessWidth8 = 1,                                         ///< Access width is 8 bits.
  AccessWidth16,                                            ///< Access width is 16 bits.
  AccessWidth32,                                            ///< Access width is 32 bits.
  AccessWidth64,                                            ///< Access width is 64 bits.

  AccessS3SaveWidth8 = 0x81,                                ///< Save 8 bits data.
  AccessS3SaveWidth16,                                      ///< Save 16 bits data.
  AccessS3SaveWidth32,                                      ///< Save 32 bits data.
  AccessS3SaveWidth64,                                      ///< Save 64 bits data.
} ACCESS_WIDTH;


// AGESA Structures
/// The standard header AMD NB UEFI drivers
typedef struct _AMD_CONFIG_PARAMS {
  VOID            **PeiServices;              ///< Pointer to PEI service table
  VOID            *StallPpi;                  ///< Pointer to Stall PPI
//  UINT32          Func;
  VOID            *PcieBasePtr;               ///< TBD
  CALLOUT_ENTRY   CalloutPtr;                 ///<pointer to local driver callback function
  CALLOUT_ENTRY   InterfaceCalloutPtr;        ///<pointer to external interface driver callback function
} AMD_CONFIG_PARAMS;


/// AGESA Binary module header structure
typedef struct _AMD_IMAGE_HEADER {
  IN  UINT32  Signature;                          ///< Binary Signature
  IN  CHAR8   CreatorID[8];                       ///< 8 characters ID
  IN  CHAR8   Version[12];                        ///< 12 characters version
  IN  UINT32  ModuleInfoOffset;                   ///< Offset of module
  IN  UINT32  EntryPointAddress;                  ///< Entry address
  IN  UINT32  ImageBase;                          ///< Image base
  IN  UINT32  RelocTableOffset;                   ///< Relocate Table offset
  IN  UINT32  ImageSize;                          ///< Size
  IN  UINT16  Checksum;                           ///< Checksum
  IN  UINT8   ImageType;                          ///< Type
  IN  UINT8   V_Reserved;                         ///< Reserved
} AMD_IMAGE_HEADER;


/// AGESA Binary module header structure
typedef struct _AMD_MODULE_HEADER {
  IN  UINT32          ModuleHeaderSignature;      ///< Module signature
  IN  CHAR8           ModuleIdentifier[8];        ///< 8 characters ID
  IN  CHAR8           ModuleVersion[12];          ///< 12 characters version
  IN  MODULE_ENTRY    ModuleDispatcherPtr;        ///< A pointer point to dispatcher
  IN  struct _AMD_MODULE_HEADER  *NextBlockPtr;   ///< Next module header link
} AMD_MODULE_HEADER;

/// Extended PCI address format
typedef struct {
  IN OUT  UINT32      Register:12;                ///< Register offset
  IN OUT  UINT32      Function:3;                 ///< Function number
  IN OUT  UINT32      Device:5;                   ///< Device number
  IN OUT  UINT32      Bus:8;                      ///< Bus number
  IN OUT  UINT32      Segment:4;                  ///< Segment
} EXT_PCI_ADDR;

/// Union type for PCI address
typedef union _PCI_ADDR {
  IN  UINT32          AddressValue;               ///< Formal address
  IN  EXT_PCI_ADDR    Address;                    ///< Extended address
} PCI_ADDR;

#define FUNC_0    0   // bit-placed for PCI address creation
#define FUNC_1    1
#define FUNC_2    2
#define FUNC_3    3
#define FUNC_4    4
#define FUNC_5    5
#define FUNC_6    6
#define FUNC_7    7

//   SBDFO - Segment Bus Device Function Offset
//   31:28   Segment (4-bits)
//   27:20   Bus     (8-bits)
//   19:15   Device  (5-bits)
//   14:12   Function(3-bits)
//   11:00   Offset  (12-bits)

#define MAKE_SBDFO(Seg, Bus, Dev, Fun, Off) ((((UINT32) (Seg)) << 28) | (((UINT32) (Bus)) << 20) | \
                   (((UINT32)(Dev)) << 15) | (((UINT32)(Fun)) << 12) | ((UINT32)(Off)))
#define ILLEGAL_SBDFO 0xFFFFFFFF

/// CPUID data received registers format
typedef struct _CPUID_DATA {
  IN  OUT UINT32          EAX_Reg;                ///< CPUID instruction result in EAX
  IN  OUT UINT32          EBX_Reg;                ///< CPUID instruction result in EBX
  IN  OUT UINT32          ECX_Reg;                ///< CPUID instruction result in ECX
  IN  OUT UINT32          EDX_Reg;                ///< CPUID instruction result in EDX
} CPUID_DATA;

#define WARM_RESET 1
#define COLD_RESET 2

/// HT frequency for external callbacks
typedef enum {
  HT_FREQUENCY_200M          = 0,                 ///< HT speed 200 for external callbacks
  HT_FREQUENCY_400M          = 2,                 ///< HT speed 400 for external callbacks
  HT_FREQUENCY_600M          = 4,                 ///< HT speed 600 for external callbacks
  HT_FREQUENCY_800M          = 5,                 ///< HT speed 800 for external callbacks
  HT_FREQUENCY_1000M         = 6,                 ///< HT speed 1000 for external callbacks
  HT_FREQUENCY_1200M         = 7,                 ///< HT speed 1200 for external callbacks
  HT_FREQUENCY_1400M         = 8,                 ///< HT speed 1400 for external callbacks
  HT_FREQUENCY_1600M         = 9,                 ///< HT speed 1600 for external callbacks
  HT_FREQUENCY_1800M         = 10,                ///< HT speed 1800 for external callbacks
  HT_FREQUENCY_2000M         = 11,                ///< HT speed 2000 for external callbacks
  HT_FREQUENCY_2200M         = 12,                ///< HT speed 2200 for external callbacks
  HT_FREQUENCY_2400M         = 13,                ///< HT speed 2400 for external callbacks
  HT_FREQUENCY_2600M         = 14,                ///< HT speed 2600 for external callbacks
  HT_FREQUENCY_2800M         = 17,                ///< HT speed 2800 for external callbacks
  HT_FREQUENCY_3000M         = 18,                ///< HT speed 3000 for external callbacks
  HT_FREQUENCY_3200M         = 19                 ///< HT speed 3200 for external callbacks
} HT_FREQUENCIES;

#ifndef BIT0
  #define BIT0        0x0000000000000001ull
#endif
#ifndef BIT1
  #define BIT1        0x0000000000000002ull
#endif
#ifndef BIT2
  #define BIT2        0x0000000000000004ull
#endif
#ifndef BIT3
  #define BIT3        0x0000000000000008ull
#endif
#ifndef BIT4
  #define BIT4        0x0000000000000010ull
#endif
#ifndef BIT5
  #define BIT5        0x0000000000000020ull
#endif
#ifndef BIT6
  #define BIT6        0x0000000000000040ull
#endif
#ifndef BIT7
  #define BIT7        0x0000000000000080ull
#endif
#ifndef BIT8
  #define BIT8        0x0000000000000100ull
#endif
#ifndef BIT9
  #define BIT9        0x0000000000000200ull
#endif
#ifndef BIT10
  #define BIT10       0x0000000000000400ull
#endif
#ifndef BIT11
  #define BIT11       0x0000000000000800ull
#endif
#ifndef BIT12
  #define BIT12       0x0000000000001000ull
#endif
#ifndef BIT13
  #define BIT13       0x0000000000002000ull
#endif
#ifndef BIT14
  #define BIT14       0x0000000000004000ull
#endif
#ifndef BIT15
  #define BIT15       0x0000000000008000ull
#endif
#ifndef BIT16
  #define BIT16       0x0000000000010000ull
#endif
#ifndef BIT17
  #define BIT17       0x0000000000020000ull
#endif
#ifndef BIT18
  #define BIT18       0x0000000000040000ull
#endif
#ifndef BIT19
  #define BIT19       0x0000000000080000ull
#endif
#ifndef BIT20
  #define BIT20       0x0000000000100000ull
#endif
#ifndef BIT21
  #define BIT21       0x0000000000200000ull
#endif
#ifndef BIT22
  #define BIT22       0x0000000000400000ull
#endif
#ifndef BIT23
  #define BIT23       0x0000000000800000ull
#endif
#ifndef BIT24
  #define BIT24       0x0000000001000000ull
#endif
#ifndef BIT25
  #define BIT25       0x0000000002000000ull
#endif
#ifndef BIT26
  #define BIT26       0x0000000004000000ull
#endif
#ifndef BIT27
  #define BIT27       0x0000000008000000ull
#endif
#ifndef BIT28
  #define BIT28       0x0000000010000000ull
#endif
#ifndef BIT29
  #define BIT29       0x0000000020000000ull
#endif
#ifndef BIT30
  #define BIT30       0x0000000040000000ull
#endif
#ifndef BIT31
  #define BIT31       0x0000000080000000ull
#endif
#ifndef BIT32
  #define BIT32       0x0000000100000000ull
#endif
#ifndef BIT33
  #define BIT33       0x0000000200000000ull
#endif
#ifndef BIT34
  #define BIT34       0x0000000400000000ull
#endif
#ifndef BIT35
  #define BIT35       0x0000000800000000ull
#endif
#ifndef BIT36
  #define BIT36       0x0000001000000000ull
#endif
#ifndef BIT37
  #define BIT37       0x0000002000000000ull
#endif
#ifndef BIT38
  #define BIT38       0x0000004000000000ull
#endif
#ifndef BIT39
  #define BIT39       0x0000008000000000ull
#endif
#ifndef BIT40
  #define BIT40       0x0000010000000000ull
#endif
#ifndef BIT41
  #define BIT41       0x0000020000000000ull
#endif
#ifndef BIT42
  #define BIT42       0x0000040000000000ull
#endif
#ifndef BIT43
  #define BIT43       0x0000080000000000ull
#endif
#ifndef BIT44
  #define BIT44       0x0000100000000000ull
#endif
#ifndef BIT45
  #define BIT45       0x0000200000000000ull
#endif
#ifndef BIT46
  #define BIT46       0x0000400000000000ull
#endif
#ifndef BIT47
  #define BIT47       0x0000800000000000ull
#endif
#ifndef BIT48
  #define BIT48       0x0001000000000000ull
#endif
#ifndef BIT49
  #define BIT49       0x0002000000000000ull
#endif
#ifndef BIT50
  #define BIT50       0x0004000000000000ull
#endif
#ifndef BIT51
  #define BIT51       0x0008000000000000ull
#endif
#ifndef BIT52
  #define BIT52       0x0010000000000000ull
#endif
#ifndef BIT53
  #define BIT53       0x0020000000000000ull
#endif
#ifndef BIT54
  #define BIT54       0x0040000000000000ull
#endif
#ifndef BIT55
  #define BIT55       0x0080000000000000ull
#endif
#ifndef BIT56
  #define BIT56       0x0100000000000000ull
#endif
#ifndef BIT57
  #define BIT57       0x0200000000000000ull
#endif
#ifndef BIT58
  #define BIT58       0x0400000000000000ull
#endif
#ifndef BIT59
  #define BIT59       0x0800000000000000ull
#endif
#ifndef BIT60
  #define BIT60       0x1000000000000000ull
#endif
#ifndef BIT61
  #define BIT61       0x2000000000000000ull
#endif
#ifndef BIT62
  #define BIT62       0x4000000000000000ull
#endif
#ifndef BIT63
  #define BIT63       0x8000000000000000ull
#endif

#ifdef  ASSERT
  #undef  ASSERT
  #define ASSERT(x)
#endif

#endif
