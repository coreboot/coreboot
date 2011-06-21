/* $NoKeywords:$ */
/**
 * @file
 *
 * Various NB Recovery initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
 *
 */
/*
 *****************************************************************************
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
 * 
 * ***************************************************************************
 *
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_RECOVERY_GNB_NBINITRECOVERY_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  UINT8   Reg;
  UINT32  Mask;
  UINT32  Data;
} NB_REGISTER_RECOVERY_ENTRY;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */



CONST NB_REGISTER_RECOVERY_ENTRY NbPciInitRecoveryTable [] = {
  {
   D0F0x04_ADDRESS,
    0xffffffff,
    0x1 << D0F0x04_MemAccessEn_WIDTH
  }
};

CONST NB_REGISTER_RECOVERY_ENTRY  NbMiscInitRecoveryTable [] = {
  {
    D0F0x64_x51_ADDRESS,
    0xffffffff,
    1 << D0F0x64_x51_SetPowEn_OFFSET
  }
};


/*----------------------------------------------------------------------------------------*/
/**
 * Init NB at Power On
 *
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */


VOID
NbInitOnPowerOnRecovery (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN Index;
  // Init NBCONFIG
  for (Index = 0; Index < (sizeof (NbPciInitRecoveryTable) / sizeof (NB_REGISTER_RECOVERY_ENTRY)); Index++) {
    GnbLibPciRMW (
      NbPciAddress.AddressValue | NbPciInitRecoveryTable[Index].Reg,
      AccessWidth32,
      NbPciInitRecoveryTable[Index].Mask,
      NbPciInitRecoveryTable[Index].Data,
      StdHeader
    );
  }

  // Init MISCIND
  for (Index = 0; Index < (sizeof (NbMiscInitRecoveryTable) / sizeof (NB_REGISTER_RECOVERY_ENTRY)); Index++) {
    GnbLibPciIndirectRMW (
      NbPciAddress.AddressValue | D0F0x60_ADDRESS,
      NbMiscInitRecoveryTable[Index].Reg | IOC_WRITE_ENABLE,
      AccessWidth32,
      NbMiscInitRecoveryTable[Index].Mask,
      NbMiscInitRecoveryTable[Index].Data,
      StdHeader
    );
  }
  return;
}
