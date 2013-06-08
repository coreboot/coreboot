/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
 ******************************************************************************
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#ifndef _IDSREGACC_H_
#define _IDSREGACC_H_
#include "mm.h"
#include "mn.h"
///AP service Time Point
typedef enum {
  IDS_REG_AP_SERVICE_EARLY,       ///< Amdinitearly
  IDS_REG_AP_SERVICE_POST,        ///< Amdinitpost
  IDS_REG_AP_SERVICE_LATE,        ///< After AmdInitPost
} IDS_REG_AP_SERVICE_TIMEPOINT;

///Structure define for MSR register
typedef struct _IDS_REG_MSR {
  IDS_REG_AP_SERVICE_TIMEPOINT TimePoint; ///< TimePoint
  UINT8 Socket;                           ///< Socket
  UINT8 Core;                             ///< Core
  UINT32 MsrAddr;                         ///< Address of MSR Register
  UINT64 AndMask;                       ///< And Mask
  UINT64 OrMask;                        ///< Or Mask
} IDS_REG_MSR;

///Enum for Mem Register access Type
typedef enum {
  IDS_REG_MEM_NB,           ///< PCI access
  IDS_REG_MEM_PHY,          ///< Memory Phy access
  IDS_REG_MEM_EXTRA,        ///< Memory Extra register access
  IDS_REG_MEM_END,          ///< End
} IDS_REG_MEM_ACCESS_TYPE;

///Structure define for Mem register
typedef struct _IDS_REG_MEM {
  IDS_REG_MEM_ACCESS_TYPE  Type;    ///< Type
  UINT8 Module;                     ///< Module
  UINT8 Dct;                        ///< Dct
  union {
    struct {
      UINT8 Func;                      ///< PCI function
      UINT16 Offset;                   ///< PCI offset
    } PciAddr;                         ///< Pci Address
    UINT32 Index;                      ///< index of indirect access
  } Addr;                             ///< address
  UINT32 AndMask;                   ///< And Mask
  UINT32 OrMask;                    ///< Or Mask
} IDS_REG_MEM;

///Family register type
typedef enum {
  IDS_FAM_REG_TYPE_GMMX,           ///< GMMX register access
  IDS_FAM_REG_TYPE_END,            ///< End
} IDS_FAM_REG_TYPE;

///Structure define for GMMX register
typedef struct _IDS_REG_GMMX {
  UINT32 Offset;                    ///< Offset of GMMX register
  UINT32 AndMask;                   ///< And Mask
  UINT32 OrMask;                    ///< Or Mask
} IDS_REG_GMMX;

///Structure define for family specific register
typedef struct _IDS_FAM_REG {
  IDS_FAM_REG_TYPE Type;           ///< Register type
  union {
    IDS_REG_GMMX Gmmx;             ///< GMMX
  } Reg;
} IDS_FAM_REG;

///Structure define for PCI indirect register
typedef struct _IDS_REG_PCI_INDIRECT {
  ACCESS_WIDTH  Width;        ///< access width
  UINT32  PciAddr;            ///< PCI address
  UINT32  IndirectRegOff;     ///< PCI indirect register offset
  UINT32  WriteEnBit;         ///< Write Enable bit
  UINT32  AndMask;            ///< And Mask
  UINT32  OrMask;             ///< Or Mask
} IDS_REG_PCI_INDIRECT;

VOID
IdsRegSetMsrCmnTask (
  IN       IDS_REG_MSR  *PRegMsr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
);

VOID
IdsRegSetMsr (
  IN       IDS_REG_MSR *PMsrReg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
);

AGESA_STATUS
IdsRegSetMemBitField (
  IN       IDS_REG_MEM *PMemReg,
  IN OUT   MEM_NB_BLOCK *NBPtr
);

VOID
IdsFamRegAccess (
  IN       IDS_FAM_REG *PFamReg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
);

VOID
IdsRegSetPciIndirect (
  IN       IDS_REG_PCI_INDIRECT *PPciIndirectReg,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
);

#endif  //_IDSREGACC_H_


