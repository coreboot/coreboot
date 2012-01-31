/**
 * @file
 *
 * Recovery support
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 *
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/// PCI registers init table
CONST REGISTER_ENTRY NbRecoveryPorPciTable[] = {
  {NB_PCI_REG04,0xFD,0x02},
//Reg84h[4]=1 (EV6MODE) to allow decode of 640k-1MB
  {NB_PCI_REG84,0xEF,0x10},
//Reg4Ch[1]=1 (APIC_ENABLE) force cpu request with address 0xFECx_xxxx to south-bridge
//Reg4Ch[6]=1 (BMMsgEn) enable BM_Set message generation
  {NB_PCI_REG4C,0x00,0x42},
//Reg4Ch[16]=1 (WakeC2En) enable Wake_from_C2 message generation.
//Reg4Ch[18]=1 (P4IntEnable) Enable north-bridge to accept MSI with address 0xFEEx_xxxx from south-bridge
  {NB_PCI_REG4E,0xFF,0x05},
//Set temporary NB TOM to 0xE0000000
  {NB_PCI_REG90 + 3, 0x00, 0xE0}
};

/// MISCIND registers init table
CONST INDIRECT_REG_ENTRY NbRecoveryPorMiscTable[] = {
// NB_MISC_IND_WR_EN + IOC_PCIE_CNTL
// Block non-snoop DMA request if PMArbDis is set.
// Set BMSetDis
  {NB_MISC_REG0B, 0xFFFF0000, 0x00000180},
// NBCFG (NBMISCIND 0x0): NB_CNTL -
//   HIDE_NB_AGP_CAP  ([0], default=1)HIDE
//   HIDE_P2P_AGP_CAP ([1], default=1)HIDE
//   HIDE_NB_GART_BAR ([2], default=1)HIDE
//   HIDE_MMCFG_BAR   ([3], default=1)HIDE
//   AGPMODE30        ([4], default=0)DISABLE
//   AGP30ENCHANCED   ([5], default=0)DISABLE
//   HIDE_AGP_CAP     ([8], default=1)ENABLE
  {NB_MISC_REG00, 0xFFFF0000, 0x0000010e},
  {NB_MISC_REG01, 0xFFFFFFFF, 0x00000010},
  {NB_MISC_REG0C, 0xFFFFFFFF, 0x001f00FC},
//NBMISIND:0x40 Bit[8]=1 and Bit[10]=1 following bits are required to set in order to allow PWM features to work.
  {NB_MISC_REG40, 0xffffffff, 0x00000500},
};

/// HTIUIND registers init table
INDIRECT_REG_ENTRY NbRecoveryPorHtiuTable[] = {
//HTIU x 06 [0] = 0x0  Enables writes to pass in-progress reads
//HTIU x 06 [1] = 0x1  Enables streaming of CPU writes
//HTIU x 06 [9] = 0x1  Enables extended write buffer for CPU writes
//HTIU x 06 [13] = 0x1 Enables additional response buffers
//HTIU x 06 [17] = 0x1 Enables special reads to pass writes
//HTIU x 06 [16:15] = 0x3  Enables decoding of C1e/C3 and FID cycles
//HTIU x 06 [25] = 0x1 Enables HTIU-display handshake bypass.
//HTIU x 06 [30] = 0x1 Enables tagging fix
  {NB_HTIU_REG06, 0xFFFFFFFE,  0x04203A202},
//HTIU x 07 [0] = 0x1  Enables byte-write optimization for IOC requests
//HTIU x 07 [1] = 0x0  Disables delaying STPCLK de-assert during FID sequence. Needed when enhanced UMA arbitration is used.
//HTIU x 07 [2] = 0x0  Disables upstream system-management delay
  {NB_HTIU_REG07, 0xFFFFFFF9,  0x0001     },
//HTIU x 1C [31:17]=0xfff i.e. 0001 1111 1111 111 or 1FFE  Enables all traffic to be detected as GSM traffic.
  {NB_HTIU_REG1C, 0xFFFFFFFF,  0x1ffe0000 },    //vsj-2007-09-04
//Enable transmit PHY to reinitialize in HT1 mode when tristate is enabled
  {NB_HTIU_REG16, 0xFFFFFFFF,  BIT11      },
//HTIU x 2A [1:0]=0x1 Optimize chipset HT transmitter drive strength
  {NB_HTIU_REG2A, 0xfffffffc, 0x00000001  }
};

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge Power on Reset Initialization for all NB in system.
 *
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */


AGESA_STATUS
AmdPowerOnResetInit (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  REGISTER_ENTRY  *pTable;
  AMD_NB_CONFIG   *pConfig;
  UINTN           i;

  pConfig = &ConfigPtr->Northbridges[0];
  pTable = (REGISTER_ENTRY*)FIX_PTR_ADDR (&NbRecoveryPorPciTable[0], NULL);
  for (i = 0; i < (sizeof (NbRecoveryPorPciTable) / sizeof (REGISTER_ENTRY)); i++) {
    LibNbPciRMW (pConfig->NbPciAddress.AddressValue | pTable->Register, AccessWidth8, pTable->Mask, pTable->Data, pConfig);
    ++pTable;
  }
  //Init Misc registers
  LibNbIndirectTableInit (
    ConfigPtr->Northbridges[0].NbPciAddress.AddressValue | NB_MISC_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&NbRecoveryPorMiscTable[0],NULL),
    (sizeof (NbRecoveryPorMiscTable) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );

   //Init Htiu registers
  LibNbIndirectTableInit (
    ConfigPtr->Northbridges[0].NbPciAddress.AddressValue  | NB_HTIU_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&NbRecoveryPorHtiuTable[0], NULL),
    (sizeof (NbRecoveryPorHtiuTable) / sizeof (INDIRECT_REG_ENTRY)),
    pConfig
    );

  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Initialize misc setting
 *
 *
 *
 * @param[in] NbConfigPtr   Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
MiscRecoveryInitializer (
  IN OUT   AMD_NB_CONFIG   *NbConfigPtr
  )
{
  return  AGESA_SUCCESS;
}


