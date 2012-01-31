/**
 * @file
 *
 * Power on Reset register initialization.
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
#include "amdDebugOutLib.h"

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
AGESA_STATUS
NbPorInitValidateInput (
  IN      AMD_NB_CONFIG *pConfig
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/// PCI registers init table
CONST REGISTER_ENTRY NbPorPciTable[] = {
  {NB_PCI_REG04, 0xFD, 0x02},
//Reg84h[4]=1 (EV6MODE) to allow decode of 640k-1MB
  {NB_PCI_REG84, 0xEF, 0x10},
//Reg4Ch[1]=1 (APIC_ENABLE) force cpu request with address 0xFECx_xxxx to south-bridge
//Reg4Ch[6]=1 (BMMsgEn) enable BM_Set message generation
  {NB_PCI_REG4C, 0x00, 0x42},
//Reg4Ch[16]=1 (WakeC2En) enable Wake_from_C2 message generation.
//Reg4Ch[18]=1 (P4IntEnable) Enable north-bridge to accept MSI with address 0xFEEx_xxxx from south-bridge
  {NB_PCI_REG4E, 0xFF, 0x05},
//Set temporary NB TOM to 0xE0000000
  {NB_PCI_REG90 + 3, 0x00, 0xE0}
};

/// MISCIND registers init table
CONST INDIRECT_REG_ENTRY NbPorMiscTable[] = {
// NB_MISC_IND_WR_EN + IOC_PCIE_CNTL
// Block non-snoop DMA request if PMArbDis is set.
// Set BMSetDis
  {
    NB_MISC_REG0B,
    0xFFFF0000,
    0x00000180
  },
// NBCFG (NBMISCIND 0x0): NB_CNTL -
//   HIDE_NB_AGP_CAP  ([0], default=1)HIDE
//   HIDE_P2P_AGP_CAP ([1], default=1)HIDE
//   HIDE_NB_GART_BAR ([2], default=1)HIDE
//   HIDE_MMCFG_BAR   ([3], default=1)HIDE
//   AGPMODE30        ([4], default=0)DISABLE
//   AGP30ENCHANCED   ([5], default=0)DISABLE
//   HIDE_AGP_CAP     ([8], default=1)ENABLE
  {
    NB_MISC_REG00,
    0xFFFF0000,
    0x0000010e
  },
//NBMISIND:0x01 Bit[8]=1 IOC will forward the byte-enable (BE), which is 16'b0 for zero-byte reads, of the PCIE DMA request upstream to HTIU.
//NBMISIND:0x01 Bit[9]=1 zero-byte reads.
  {
    NB_MISC_REG01,
    0xFFFFFFFF,
    0x00000310
  },
//NBMISIND:0x40 Bit[8]=1 and Bit[10]=1 following bits are required to set in order to allow PWM features to work.
  {
    NB_MISC_REG40,
    0xffffffff,
    0x00000500
  },
//Enable slot power message
  {
    NB_MISC_REG51,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG53,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG55,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG57,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG59,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG5B,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG5D,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG5F,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG61,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG63,
    0x00000000,
    0x00100106
  },
  {
    NB_MISC_REG1F,
    0x00000000,
    0x00100106
  },
//NBMISCIND:0x0C[13]= 1 Enables GSM Mode.
  {
    NB_MISC_REG0C,
    0xffffffff,
    BIT13 + BIT20
  },
//NBMISCIND:0x12[16]= 1 ReqID for GPP1 and GPP2
//NBMISCIND:0x12[17]= 1 ReqID for  GPP3a, GPP3b, SB
//NBMISCIND:0x12[18]= 0 ReqID override for SB
//NBMISCIND:0x12[19]= 1 Enable INT accumulators
//NBMISCIND:0x12[20, 21, 23]= 1 4103, 4125, 4155 4186 (A21).
//NBMISCIND:0x12[22]=0  Prevent spurious DR of UMA request (RPR 5.9.3)
  {
    NB_MISC_REG12,
    (UINT32)~(BIT18 + BIT22),
    0xBB0000
  },
//NBMISCIND:0x75[15.13,16..18,21..19,24..22,25..25] = 0x4  Enable AER
//NBMISCIND:0x75[29]= 1 Device ID for hotplug and PME message.
  {
    NB_MISC_REG75,
    (UINT32)~BIT28,
    (4 << 13) | (4 << 16) | (4 << 19) | (4 << 22) | (4 << 25) | BIT29
  },
//PCIe CDR setting
  {
    NB_MISC_REG38,
    0xffffffff,
    BIT6 + BIT7 + BIT14 + BIT15 + BIT22 + BIT23
  },
  {
    NB_MISC_REG67,
    0xffffffff,
    BIT21 + BIT22
  },
  {
    NB_MISC_REG2C,
    (UINT32)~(BIT0 + BIT1 + BIT19),
    BIT0 + BIT1
  },
  {
    NB_MISC_REG6C,
    (UINT32)~(BIT10),
    0x0
  },
  {
    NB_MISC_REG34,
    (UINT32)~(BIT7 + BIT15 + BIT23),
    0x0
  },
  {
    NB_MISC_REG37,
    (UINT32)~(0xffful << 20),
    (0xdddul << 20)
  },
  {
    NB_MISC_REG68,
    (UINT32)~(0xful << 16),
    (0xd << 16)
    },
  {
    NB_MISC_REG2B,
    (UINT32)~(0xful << 24 ),
    (0xd << 24)
  },
  // Enable ACS capability
  {
    NB_MISC_REG6A,
    0xffffffff,
    BIT2
  }
};

/// HTIUIND registers init table
CONST INDIRECT_REG_ENTRY NbPorHtiuTable[] = {
//HTIU x 05 [8] = 0x0 Enables PC checking for FCB release.
//HTIU x 05 [13,13,3,14,10,12,17,18,15,4,6,19] = 0x1 Misc (A21)
  {NB_HTIU_REG05, 0xFFFFFEFF,  BIT8 + BIT16 + BIT13 + BIT3 + BIT14 + BIT10 + BIT12 + BIT17 + BIT18 + BIT15 +
                               BIT4 + BIT6 + BIT19 },
  //HTIU x 06 [0] = 0x0 Enables writes to pass in-progress reads
//HTIU x 06 [1] = 0x1 Enables streaming of CPU writes
//HTIU x 06 [9] = 0x1 Enables extended write buffer for CPU writes
//HTIU x 06 [13] = 0x1 Enables additional response buffers
//HTIU x 06 [17] = 0x1 Enables special reads to pass writes
//HTIU x 06 [16:15] = 0x3 Enables decoding of C1e/C3 and FID cycles
//HTIU x 06 [25] = 0x1 Enables HTIU-display handshake bypass.
//HTIU x 06 [30] = 0x1 Enables tagging fix
  {NB_HTIU_REG06, 0xFFFFFFFE,  0x04203A202},
//HTIU x 07 [0] = 0x1 Enables byte-write optimization for IOC requests
//HTIU x 07 [1] = 0x0 Disables delaying STPCLK de-assert during FID sequence. Needed when enhanced UMA arbitration is used.
//HTIU x 07 [2] = 0x0 Disables upstream system-management delay
  {NB_HTIU_REG07, 0xFFFFFFF9,  0x0001     },
//HTIU x 1C [31:17]=0xfff i.e. 0001 1111 1111 111 or 1FFE Enables all traffic to be detected as GSM traffic.
  {NB_HTIU_REG1C, 0xFFFFFFFF,  0x1ffe0000 },
//HTIU x 15 [27]=0x1 Powers down the chipset DLLs in the LS2 state.
  {NB_HTIU_REG15, 0xFFFFFFFF,  BIT27      },
//Enable transmit PHY to reinitialize in HT1 mode when tristate is enabled
//HTIU x 16 [10]=0x1 enable proper DLL reset sequence.
  {NB_HTIU_REG16, 0xFFFFFFFF,  BIT11 + BIT10},
//HTIU x 2A [1:0]=0x1 Optimize chipset HT transmitter drive strength
  {NB_HTIU_REG2A, 0xfffffffc, 0x00000001  }
};

/*----------------------------------------------------------------------------------------*/
/**
 * Amd Power on Reset Initialization for all NB.
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
  AGESA_STATUS Status;

  Status = LibNbApiCall (NbPowerOnResetInit, ConfigPtr);
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * NB Power on Reset Initialization.
 *    Basic registers initialization.
 *
 *
 *
 * @param[in] NbConfigPtr   Northbridge configuration structure pointer.
 *
 */


AGESA_STATUS
NbPowerOnResetInit (
  IN      AMD_NB_CONFIG *NbConfigPtr
  )
{
  AGESA_STATUS    Status;
  REGISTER_ENTRY  *pTable;
  UINTN           i;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPOR_TRACE), "[NBPOR]NbPowerOnResetInit Enter\n"));
  Status = NbPorInitValidateInput (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return  Status;
  }
  //Init Pci Registers
  pTable = (REGISTER_ENTRY*)FIX_PTR_ADDR (&NbPorPciTable[0], NULL);
  for (i = 0; i < (sizeof (NbPorPciTable) / sizeof (REGISTER_ENTRY)); i++) {
    LibNbPciRMW (NbConfigPtr->NbPciAddress.AddressValue | pTable->Register, AccessWidth8, pTable->Mask, pTable->Data, NbConfigPtr);
    ++pTable;
  }
  //Init Misc registers
  LibNbIndirectTableInit (
    NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&NbPorMiscTable[0], NULL),
    (sizeof (NbPorMiscTable) / sizeof (INDIRECT_REG_ENTRY)),
    NbConfigPtr
    );

   //Init Htiu registers
  LibNbIndirectTableInit (
    NbConfigPtr->NbPciAddress.AddressValue | NB_HTIU_INDEX,
    0,
    (INDIRECT_REG_ENTRY*)FIX_PTR_ADDR (&NbPorHtiuTable[0],NULL),
    (sizeof (NbPorHtiuTable) / sizeof (INDIRECT_REG_ENTRY)),
    NbConfigPtr
    );

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPOR_TRACE), "[NBPOR]NbPowerOnResetInit Exit\n"));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Validate input parameters
 *
 *
 *
 *
 *  @param[in] pConfig  Northbridge configuration structure pointer.
 */


AGESA_STATUS
NbPorInitValidateInput (
  IN      AMD_NB_CONFIG *pConfig
  )
{
  return (LibNbGetRevisionInfo (pConfig).Type == NB_UNKNOWN)?AGESA_FATAL:AGESA_SUCCESS;
}
