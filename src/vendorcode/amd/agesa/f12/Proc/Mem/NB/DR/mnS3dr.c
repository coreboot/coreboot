/* $NoKeywords:$ */
/**
 * @file
 *
 * mns3dr.c
 *
 * DR memory specific function to support S3 resume
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DR)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "S3.h"
#include "mfs3.h"
#include "mndr.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "mnS3dr.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_NB_DR_MNS3DR_FILECODE

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
UINT16
STATIC
MemNS3GetRegLstPtrDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  );

AGESA_STATUS
STATIC
MemNS3GetDeviceRegLstDr (
  IN       UINT32 RegisterLstID,
     OUT   VOID **RegisterHeader
  );

VOID
STATIC
MemNS3SetSpecialPCIRegDr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  );

VOID
STATIC
MemNS3ExitSelfRefRegDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
PCI_SPECIAL_CASE PciSpecialCaseFuncDr[] = {
  {MemNS3GetCSRNb, MemNS3SetCSRNb},
  {MemNS3GetCSRNb, MemNS3SetSpecialPCIRegDr},
  {MemNS3GetBitFieldNb, MemNS3SetBitFieldNb}
};

PCI_REG_DESCRIPTOR ROMDATA S3PciPreSelfRefDescriptorDr[] = {
  {{0, 0, 0}, FUNC_2, 0x110, 0xFFFFFFFF},
  {{0, 0, 0}, FUNC_1, 0x40,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x48,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x50,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x58,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x60,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x68,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x70,  0xFFFF3F03},
  {{0, 0, 0}, FUNC_1, 0x78,  0xFFFF3F03},
  {{0, 1, 0}, FUNC_1, 0x140, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x148, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x150, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x158, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x160, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x168, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x170, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x178, 0x000000FF},
  {{0, 0, 0}, FUNC_1, 0x44,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x4C,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x54,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x5C,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x64,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x6C,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x74,  0xFFFF07FF},
  {{0, 0, 0}, FUNC_1, 0x7C,  0xFFFF07FF},
  {{0, 1, 0}, FUNC_1, 0x144, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x14C, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x154, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x15C, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x164, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x16C, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x174, 0x000000FF},
  {{0, 1, 0}, FUNC_1, 0x17C, 0x000000FF},
  {{0, 0, 0}, FUNC_1, 0xF0,  0xFF00FF83},
  {{0, 0, 0}, FUNC_1, 0x120, 0x00FFFFFF},
  {{0, 0, 0}, FUNC_1, 0x124, 0x07FFFFFF},
  {{0, 0, 0}, FUNC_2, 0x114, 0xFFFFFC00},
  {{0, 0, 0}, FUNC_2, 0x118, 0xF773FFFF},
  {{0, 0, 0}, FUNC_2, 0x11C, 0xFFFFFFFF},
  {{0, 0, 0}, FUNC_2, 0x1B0, 0xFFD3FF3F}
};

CONST PCI_REGISTER_BLOCK_HEADER ROMDATA S3PciPreSelfRefDr = {
  0,
  (sizeof (S3PciPreSelfRefDescriptorDr) / sizeof (PCI_REG_DESCRIPTOR)),
  S3PciPreSelfRefDescriptorDr,
  NULL
};

CONDITIONAL_PCI_REG_DESCRIPTOR ROMDATA S3CPciPreSelfDescriptorDr[] = {
   // DCT 0
  {{0, 0, 0}, FUNC_2, 0x40,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x44,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x48,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x4C,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x50,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x54,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x58,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x5C,  0x1FF83FEF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x60,  0x1FF83FE0, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x64,  0x1FF83FE0, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x68,  0x1FF83FE0, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x6C,  0x1FF83FE0, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x78,  0xFFCDBF0F, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x7C,  0xFFF7FFFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 2, 0}, FUNC_2, 0x80,  0x0000FFFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x84,  0x07FFEFFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x88,  0xFFFFFFFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x8C,  0xFFFF7FFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x90,  0x00FFFFFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0xA8,  0x0007FFFF, DCT0_MASK, DCT0_ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x00),  0x30333333, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0A),  0x3FFFFFFF, DCT0_MASK + DCT1_MASK, ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0C),  0x001FBFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x04),  0x003F3F3F, DCT0_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT0,   BFPhyClkConfig0, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT0,   BFPhyClkConfig1, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT0,   BFPhyClkConfig2, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT0,   BFPhyClkConfig3, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  // errata 322
  {{2, 2, 1}, DCT0,   BFErr322I, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT0,   BFErr322II, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},
  // errata 263
  {{2, 2, 1}, DCT0,   BFErr263, 0x0000FFFF, DCT0_MASK, ANY_DIMM_MASK},

   // DCT 1
  {{0, 0, 0}, FUNC_2, 0x140, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x144, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x148, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x14C, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x150, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x154, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x158, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x15C, 0x1FF83FEF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x160, 0x1FF83FE0, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x164, 0x1FF83FE0, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x168, 0x1FF83FE0, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x16C, 0x1FF83FE0, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x178, 0xFFCDBF0F, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x17C, 0xFFF7FFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 2, 0}, FUNC_2, 0x180, 0x0000FFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x184, 0x07FFEFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x188, 0xFFFFFFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x18C, 0xFFF7FFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x190, 0x00FFFFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x1A8, 0x0007FFFF, DCT1_MASK, DCT1_ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x00), 0x30333333, DCT1_MASK, ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x0C), 0x001FBFFF, DCT1_MASK, ANY_DIMM_MASK},
  {{1, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x04), 0x003F3F3F, DCT1_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT1,   BFPhyClkConfig0, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT1,   BFPhyClkConfig1, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT1,   BFPhyClkConfig2, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT1,   BFPhyClkConfig3, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},
  // errata 322
  {{2, 2, 1}, DCT1,   BFErr322I, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},
  {{2, 2, 1}, DCT1,   BFErr322II, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},
  // errata 263
  {{2, 2, 1}, DCT1,   BFErr263, 0x0000FFFF, DCT1_MASK, ANY_DIMM_MASK},

  // Restore F2x[1,0]94 right before exit self refresh
  {{0, 0, 0}, FUNC_2, 0x94,  0xFFFFFF07, ANY_DIMM_MASK, ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_2, 0x194, 0xFFFFFF07, ANY_DIMM_MASK, ANY_DIMM_MASK}
};

CONST CPCI_REGISTER_BLOCK_HEADER ROMDATA S3CPciPreSelfRefDr = {
  0,
  (sizeof (S3CPciPreSelfDescriptorDr) / sizeof (CONDITIONAL_PCI_REG_DESCRIPTOR)),
  S3CPciPreSelfDescriptorDr,
  PciSpecialCaseFuncDr
};

CONDITIONAL_PCI_REG_DESCRIPTOR ROMDATA S3CPciPostSelfDescriptorDr[] = {
  // DCT0
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x10),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x11),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x12),  0x000001FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x13),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x14),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x15),  0x000001FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x16),  0x01FF01FF, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x17),  0x01FF01FF, DCT0_MASK, 0x10},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x18),  0x000001FF, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x19),  0x01FF01FF, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x1A),  0x01FF01FF, DCT0_MASK, 0x40},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x1B),  0x000001FF, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x20),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x21),  0x01FF01FF, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x23),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x24),  0x01FF01FF, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x26),  0x01FF01FF, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x27),  0x01FF01FF, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x29),  0x01FF01FF, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x2A),  0x01FF01FF, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x01),  0x7F7F7F7F, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x02),  0x7F7F7F7F, DCT0_MASK, 0x01},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x03),  0x0000007F, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x101), 0x7F7F7F7F, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x102), 0x7F7F7F7F, DCT0_MASK, 0x04},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x103), 0x0000007F, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x201), 0x7F7F7F7F, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x202), 0x7F7F7F7F, DCT0_MASK, 0x10},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x203), 0x0000007F, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x301), 0x7F7F7F7F, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x302), 0x7F7F7F7F, DCT0_MASK, 0x40},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x303), 0x0000007F, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x05),  0x3F3F3F3F, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x06),  0x3F3F3F3F, DCT0_MASK, 0x01},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x07),  0x0000003F, DCT0_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x105), 0x3F3F3F3F, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x106), 0x3F3F3F3F, DCT0_MASK, 0x04},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x107), 0x0000003F, DCT0_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x205), 0x3F3F3F3F, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x206), 0x3F3F3F3F, DCT0_MASK, 0x10},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x207), 0x0000003F, DCT0_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x305), 0x3F3F3F3F, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x306), 0x3F3F3F3F, DCT0_MASK, 0x40},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x307), 0x0000003F, DCT0_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0A),  0xFFFFFFFF, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0D),  0x23772377, DCT0_MASK, ANY_DIMM_MASK},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x30),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x31),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x32),  0x000000FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x33),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x34),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x35),  0x000000FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x36),  0x00FF00FF, DCT0_DDR3_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x37),  0x00FF00FF, DCT0_DDR3_MASK, 0x10},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x38),  0x000000FF, DCT0_DDR3_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x39),  0x00FF00FF, DCT0_DDR3_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x3A),  0x00FF00FF, DCT0_DDR3_MASK, 0x40},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x3B),  0x000000FF, DCT0_DDR3_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x40),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x41),  0x00FF00FF, DCT0_DDR3_MASK, 0x01},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x43),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x44),  0x00FF00FF, DCT0_DDR3_MASK, 0x04},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x46),  0x00FF00FF, DCT0_DDR3_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x47),  0x00FF00FF, DCT0_DDR3_MASK, 0x10},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x49),  0x00FF00FF, DCT0_DDR3_MASK, 0x40},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x4A),  0x00FF00FF, DCT0_DDR3_MASK, 0x40},

    // DCT1
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x10),  0x01FF01FF, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x11),  0x01FF01FF, DCT1_MASK, 0x02},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x12),  0x000001FF, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x13),  0x01FF01FF, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x14),  0x01FF01FF, DCT1_MASK, 0x08},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x15),  0x000001FF, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x16),  0x01FF01FF, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x17),  0x01FF01FF, DCT1_MASK, 0x20},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x18),  0x000001FF, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x19),  0x01FF01FF, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x1A),  0x01FF01FF, DCT1_MASK, 0x80},
  {{0, 2, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x1B),  0x000001FF, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x20),  0x01FF01FF, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x21),  0x01FF01FF, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x23),  0x01FF01FF, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x24),  0x01FF01FF, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x26),  0x01FF01FF, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x27),  0x01FF01FF, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x29),  0x01FF01FF, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x2A),  0x01FF01FF, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x01),  0x7F7F7F7F, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x02),  0x7F7F7F7F, DCT1_MASK, 0x02},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x03),  0x0000007F, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x101),  0x7F7F7F7F, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x102),  0x7F7F7F7F, DCT1_MASK, 0x08},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x103),  0x0000007F, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x201),  0x7F7F7F7F, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x202),  0x7F7F7F7F, DCT1_MASK, 0x20},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x203),  0x0000007F, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x301),  0x7F7F7F7F, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x302),  0x7F7F7F7F, DCT1_MASK, 0x80},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x303),  0x0000007F, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x05),  0x3F3F3F3F, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x06),  0x3F3F3F3F, DCT1_MASK, 0x02},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x07),  0x0000003F, DCT1_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x105),  0x3F3F3F3F, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x106),  0x3F3F3F3F, DCT1_MASK, 0x08},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x107),  0x0000003F, DCT1_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x205),  0x3F3F3F3F, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x206),  0x3F3F3F3F, DCT1_MASK, 0x20},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x207),  0x0000003F, DCT1_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x305),  0x3F3F3F3F, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x306),  0x3F3F3F3F, DCT1_MASK, 0x80},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x307),  0x0000003F, DCT1_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x0D),  0x23772377, DCT1_MASK, ANY_DIMM_MASK},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x30),  0x00FF00FF, DCT1_DDR3_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x31),  0x00FF00FF, DCT1_DDR3_MASK, 0x02},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x32),  0x000000FF, DCT1_DDR3_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x33),  0x00FF00FF, DCT1_DDR3_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x34),  0x00FF00FF, DCT1_DDR3_MASK, 0x08},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x35),  0x000000FF, DCT1_DDR3_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x36),  0x00FF00FF, DCT1_DDR3_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x37),  0x00FF00FF, DCT1_DDR3_MASK, 0x20},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x38),  0x000000FF, DCT1_DDR3_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x39),  0x00FF00FF, DCT1_DDR3_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x3A),  0x00FF00FF, DCT1_DDR3_MASK, 0x80},
  {{0, 1, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x3B),  0x000000FF, DCT1_DDR3_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x40),  0x00FF00FF, DCT1_DDR3_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x41),  0x00FF00FF, DCT1_DDR3_MASK, 0x02},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x43),  0x00FF00FF, DCT1_DDR3_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x44),  0x00FF00FF, DCT1_DDR3_MASK, 0x08},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x46),  0x00FF00FF, DCT1_DDR3_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x47),  0x00FF00FF, DCT1_DDR3_MASK, 0x20},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x49),  0x00FF00FF, DCT1_DDR3_MASK, 0x80},
  {{0, 0, 1}, FUNC_2, SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x4A),  0x00FF00FF, DCT1_DDR3_MASK, 0x80},

  // Restore scrubber related registers after restoring training related registers
  {{0, 0, 0}, FUNC_3, 0x44,  0xFFFFFFFE, ANY_DIMM_MASK, ANY_DIMM_MASK},
  {{0, 0, 0}, FUNC_3, 0x58,  0x1F1F1F1F, ANY_DIMM_MASK, ANY_DIMM_MASK},
  {{2, 1, 1}, DCT0,   BFScrubReDirEn, 0x00000001, ANY_DIMM_MASK, ANY_DIMM_MASK},
};

CONST CPCI_REGISTER_BLOCK_HEADER ROMDATA S3CPciPostSelfRefDr = {
  0,
  (sizeof (S3CPciPostSelfDescriptorDr) / sizeof (CONDITIONAL_PCI_REG_DESCRIPTOR)),
  S3CPciPostSelfDescriptorDr,
  PciSpecialCaseFuncDr
};

MSR_REG_DESCRIPTOR ROMDATA S3MSRPreSelfRefDescriptorDr[] = {
  {{0, 0, 0}, 0xC0010010, 0x00000000007F07FF},
  {{0, 0, 0}, 0xC001001A, 0x0000FFFFFF800000},
  {{0, 0, 0}, 0xC001001D, 0x0000FFFFFF800000},
  {{0, 0, 0}, 0xC001001F, 0xC047F87FFF527FFF}
};

CONST MSR_REGISTER_BLOCK_HEADER ROMDATA S3MSRPreSelfRefDr = {
  0,
  (sizeof (S3MSRPreSelfRefDescriptorDr) / sizeof (MSR_REG_DESCRIPTOR)),
  S3MSRPreSelfRefDescriptorDr,
  NULL
};

VOID *MemS3RegListDr[] = {
  (VOID *)&S3PciPreSelfRefDr,
  NULL,
  (VOID *)&S3CPciPreSelfRefDr,
  (VOID *)&S3CPciPostSelfRefDr,
  (VOID *)&S3MSRPreSelfRefDr,
  NULL,
  NULL,
  NULL
};

CONST UINT16 ROMDATA SpecialCasePCIRegDr[] = {
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x00),
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0A),
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x0C),
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 0, 0x04),
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x00),
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x0C),
  SET_S3_SPECIAL_OFFSET (DCT_PHY_FLAG, 1, 0x04)
};
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the northbridge block for S3 resume
 *
 *     @param[in,out]   *S3NBPtr   - Pointer to MEM_NB_BLOCK.
 *     @param[in,out]   *MemPtr  - Pointer to MEM_DATA_STRUCT.
 *     @param[in]       NodeID   - Node ID of the target node.
 *
 *      @return         BOOLEAN
 *                         TRUE - This is the correct constructor for the targeted node.
 *                         FALSE - This isn't the correct constructor for the targeted node.
 */
BOOLEAN
MemS3ResumeConstructNBBlockDr (
  IN OUT   VOID *S3NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       UINT8 NodeID
  )
{
  INT32 i;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = ((S3_MEM_NB_BLOCK *)S3NBPtr)->NBPtr;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedDr (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->MCTPtr = &(MemPtr->DiesPerSystem[NodeID]);
  NBPtr->PciAddr.AddressValue = MemPtr->DiesPerSystem[NodeID].PciAddr.AddressValue;
  InitNBRegTableDr (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->Ganged = FALSE;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_DR;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_DR;

  for (i = 0; i < EnumSize; i++) {
    NBPtr->IsSupported[i] = FALSE;
  }

  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK *, VOID *)) memDefTrue;
  }

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &MemPtr->StdHeader);

  NBPtr->IsSupported[CheckDllSpeedUp] = FALSE;
  NBPtr->SwitchDCT = MemNSwitchDCTNb;
  NBPtr->SwitchChannel = MemNSwitchChannelNb;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldDr;
  NBPtr->MemNIsIdSupportedNb = MemNIsIdSupportedDr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3ExitSelfRefReg = MemNS3ExitSelfRefRegDr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetConPCIMask = MemNS3GetConPCIMaskNb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetConMSRMask = (VOID (*) (MEM_NB_BLOCK*, DESCRIPTOR_GROUP*)) memDefRet;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3Resume = MemNS3ResumeNb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3RestoreScrub = MemNS3RestoreScrubNb;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetRegLstPtr = MemNS3GetRegLstPtrDr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3GetDeviceRegLst = MemNS3GetDeviceRegLstDr;
  ((S3_MEM_NB_BLOCK *)S3NBPtr)->MemS3SpecialCaseHeapSize = (sizeof (SpecialCasePCIRegDr) / sizeof (UINT16)) * sizeof (UINT32);

  MemNSwitchDCTNb (NBPtr, 0);

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns the register list for each device for DR
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in, out]  *DescriptPtr - Pointer to DESCRIPTOR_GROUP
 *     @return          UINT16 - size of the device descriptor on the target node.
 */
UINT16
STATIC
MemNS3GetRegLstPtrDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   DESCRIPTOR_GROUP *DescriptPtr
  )
{
  UINT8 i;
  UINT16 Size;
  Size = 0;
  for (i = PRESELFREF; i <= POSTSELFREF; i ++) {
    DescriptPtr->PCIDevice[i].Type = (UINT8) (DEV_TYPE_PCI_PRE_ESR + i);
    DescriptPtr->PCIDevice[i].Node = NBPtr->Node;
    DescriptPtr->PCIDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((PCI_REGISTER_BLOCK_HEADER *) MemS3RegListDr[PCI_LST_ESR_DR - PCI_LST_ESR_DR + i] != NULL) {
      DescriptPtr->PCIDevice[i].RegisterListID = PCI_LST_ESR_DR + i;
      Size += sizeof (PCI_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->CPCIDevice[i].Type = (UINT8) (DEV_TYPE_CPCI_PRE_ESR + i);
    DescriptPtr->CPCIDevice[i].Node = NBPtr->Node;
    DescriptPtr->CPCIDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((CPCI_REGISTER_BLOCK_HEADER *) MemS3RegListDr[CPCI_LST_ESR_DR - PCI_LST_ESR_DR + i] != NULL) {
      DescriptPtr->CPCIDevice[i].RegisterListID = CPCI_LST_ESR_DR + i;
      Size += sizeof (CONDITIONAL_PCI_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->MSRDevice[i].Type = (UINT8) (DEV_TYPE_MSR_PRE_ESR + i);
    DescriptPtr->MSRDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((MSR_REGISTER_BLOCK_HEADER *) MemS3RegListDr[MSR_LST_ESR_DR - PCI_LST_ESR_DR + i] != NULL) {
      DescriptPtr->MSRDevice[i].RegisterListID = MSR_LST_ESR_DR + i;
      Size += sizeof (MSR_DEVICE_DESCRIPTOR);
    }
    DescriptPtr->CMSRDevice[i].Type = (UINT8) (DEV_TYPE_CMSR_PRE_ESR + i);
    DescriptPtr->CMSRDevice[i].RegisterListID = 0xFFFFFFFF;
    if ((CMSR_REGISTER_BLOCK_HEADER *) MemS3RegListDr[CMSR_LST_ESR_DR - PCI_LST_ESR_DR + i] != NULL) {
      DescriptPtr->CMSRDevice[i].RegisterListID = CMSR_LST_ESR_DR + i;
      Size += sizeof (CONDITIONAL_MSR_DEVICE_DESCRIPTOR);
    }
  }
  return Size;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function return the register list according to the register ID.
 *
 *     @param[in]   RegisterLstID - value of the Register list ID.
 *     @param[out]  **RegisterHeader - pointer to the address of the register list.
 *     @return      none
 */
AGESA_STATUS
STATIC
MemNS3GetDeviceRegLstDr (
  IN       UINT32 RegisterLstID,
     OUT   VOID **RegisterHeader
  )
{
  if (RegisterLstID >= (sizeof (MemS3RegListDr) / sizeof (VOID *))) {
    ASSERT(FALSE); // RegisterListID exceeded size of Register list
    return AGESA_FATAL;
  }
  if (MemS3RegListDr[RegisterLstID] != NULL) {
    *RegisterHeader = MemS3RegListDr[RegisterLstID];
    return AGESA_SUCCESS;
  }
  ASSERT(FALSE); // Device register list error
  return AGESA_FATAL;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function stores special case register on the heap.
 *
 *     @param[in]   AccessWidth - Access width of the register
 *     @param[in]   Address - address of the CSR register in PCI_ADDR format.
 *     @param[in]   *Value - Pointer to the value be read.
 *     @param[in, out]  *ConfigPtr - Pointer to Config handle.
 *      @return         none
 */
VOID
STATIC
MemNS3SetSpecialPCIRegDr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR Address,
  IN       VOID *Value,
  IN OUT   VOID *ConfigPtr
  )
{
  LOCATE_HEAP_PTR LocateBufferPtr;
  UINT8 i;
  UINT8 NodeID;
  UINT8 Offset;
  S3_SPECIAL_CASE_HEAP_HEADER *SpecialHeapHeader;

  Offset = 0;
  LocateBufferPtr.BufferHandle = AMD_MEM_S3_DATA_HANDLE;
  if (HeapLocateBuffer (&LocateBufferPtr, ConfigPtr) == AGESA_SUCCESS) {
    SpecialHeapHeader = (S3_SPECIAL_CASE_HEAP_HEADER *) LocateBufferPtr.BufferPtr;
    // Get the node ID of the target die.
    NodeID = (UINT8) (Address.Address.Device - 24);
    for (i = 0; i < MAX_NODES_SUPPORTED_DR; i ++) {
      if (SpecialHeapHeader[i].Node == NodeID) {
        // Get the offset in the heap for the target die.
        Offset = SpecialHeapHeader[i].Offset;
        break;
      }
    }
    ASSERT (i < MAX_NODES_SUPPORTED_DR);
    // Save the value in the heap at appropriate offset based on the index
    // of the target register in the special case array.
    if (Offset != 0) {
      for (i = 0; i < (sizeof (SpecialCasePCIRegDr) / sizeof (UINT16)); i ++) {
        if (SpecialCasePCIRegDr[i] == Address.Address.Register) {
          *(UINT32 *) (LocateBufferPtr.BufferPtr + Offset + (i << 2)) = *(UINT32 *) Value;
        }
      }
    }
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function stores special case register on the heap.
 *
 *     @param[in,out]  *NBPtr - Pointer to the northbridge block.
 *     @param[in,out]  *StdHeader - Config handle for library and services.
 *     @return         none
 */
VOID
STATIC
MemNS3ExitSelfRefRegDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  LOCATE_HEAP_PTR LocateBufferPtr;
  UINT8 i;
  PCI_ADDR PciAddr;
  UINT32 Value;
  UINT8 NodeID;
  UINT8 Offset;
  S3_SPECIAL_CASE_HEAP_HEADER *SpecialHeapHeader;

  Offset = 0;
  PciAddr.Address.Device = NBPtr->PciAddr.Address.Device;
  PciAddr.Address.Bus = NBPtr->PciAddr.Address.Bus;
  PciAddr.Address.Segment = NBPtr->PciAddr.Address.Segment;
  PciAddr.Address.Function = 2;
  LocateBufferPtr.BufferHandle = AMD_MEM_S3_DATA_HANDLE;
  if (HeapLocateBuffer (&LocateBufferPtr, StdHeader) == AGESA_SUCCESS) {
    SpecialHeapHeader = (S3_SPECIAL_CASE_HEAP_HEADER *) LocateBufferPtr.BufferPtr;
    // Get the node ID of the target die.
    NodeID = (UINT8) (PciAddr.Address.Device - 24);
    for (i = 0; i < MAX_NODES_SUPPORTED_DR; i ++) {
      if (SpecialHeapHeader[i].Node == NodeID) {
        // Get the offset in the heap for the target die.
        Offset = SpecialHeapHeader[i].Offset;
        break;
      }
    }
    ASSERT (i < MAX_NODES_SUPPORTED_DR);
    // Restore the value one by one in the sequence of the special case register array.
    if (Offset != 0) {
      for (i = 0; i < (sizeof (SpecialCasePCIRegDr) / sizeof (UINT16)); i ++) {
        PciAddr.Address.Register = SpecialCasePCIRegDr[i];
        Value = *(UINT32 *) (LocateBufferPtr.BufferPtr + Offset + (i << 2));
        MemNS3SetCSRNb (AccessS3SaveWidth32, PciAddr, &Value, StdHeader);
      }
    }
  }
}
