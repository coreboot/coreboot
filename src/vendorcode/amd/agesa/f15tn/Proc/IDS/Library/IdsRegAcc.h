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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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


