/* $NoKeywords:$ */
/**
 * @file
 *
 * NB Lclk/Nclk Ratios
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 41658 $   @e \$Date: 2010-11-09 06:39:38 +0800 (Tue, 09 Nov 2010) $
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
#include  "Ids.h"
#include  "heapManager.h"
#include  "GnbFuseTable.h"
#include  "Gnb.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GfxLib.h"
#include  "GnbRegistersON.h"
#include  "F14NbLclkNclkRatio.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_NB_FAMILY_0X14_F14NBLCLKNCLKRATIO_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINT8  NclkDiv;
  UINT8  LclkDid;
} NLCK_SCLK;


/*----------------------------------------------------------------------------------------*/
/**
 * Power gate unused blocks
 *
 *
 *
 * @param[in] Nclk10kHz        NCLK
 * @param[in] Lclk10kHz        LCLK
 * @param[in] LclkNclk         NCLK/LCLK array
 * @retval    AGESA_STATUS
 */

VOID
STATIC
F14NbLclkNclkAllocatePair (
  IN       UINT8      NclkDiv,
  IN       UINT8      LclkDid,
  IN OUT   NLCK_SCLK  *LclkNclk
  )
{
  UINTN   Index;
  for (Index = 0; Index < 8 ; Index++) {
    if (LclkNclk[Index].LclkDid == 0 && LclkNclk[Index].NclkDiv == 0) {
      LclkNclk[Index].LclkDid = LclkDid;
      LclkNclk[Index].NclkDiv = NclkDiv;
	  break;
    } else if (LclkNclk[Index].LclkDid == LclkDid && LclkNclk[Index].NclkDiv == NclkDiv) {
      break;
    } 
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power gate unused blocks
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
F14NbLclkNclkRatioFeature (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PP_FUSE_ARRAY     *PpFuseArray;
  D18F3xD4_STRUCT   D18F3xD4;
  D18F3xDC_STRUCT   D18F3xDC;
  D18F6x90_STRUCT   D18F6x90;
  D18F6x110_STRUCT  D18F6x110;
  UINT32            MainPllFreq10kHz;
  UINT8             NclkDiv[2];
  INT32             Nclk_offset;
  INT32             Lclk_offset;
  UINT8             Index;
  UINT8             LclkIndex;
  UINT32            Lclk_period;
  UINT32            Nclk_period;
  NLCK_SCLK         LclkNclk [8];
  IDS_HDT_CONSOLE (GNB_TRACE, "F14NbLclkNclkRatioFeature Enter\n");
  PpFuseArray = (PP_FUSE_ARRAY *) GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Location\n");
    return AGESA_ERROR;
  }

  //main PLL COF in 10kHz
  MainPllFreq10kHz = GfxLibGetMainPllFreq (StdHeader) * 100;

  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xD4_ADDRESS),
    AccessWidth32,
    &D18F3xD4.Value,
    StdHeader
    );
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xDC_ADDRESS),
    AccessWidth32,
    &D18F3xDC.Value,
    StdHeader
    );
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x90_ADDRESS),
    AccessWidth32,
    &D18F6x90.Value,
    StdHeader
    );

  NclkDiv[0] = (UINT8) D18F3xDC.Field.NbPs0NclkDiv;
  NclkDiv[1] = (UINT8) D18F6x90.Field.NbPs1NclkDiv;

  LibAmdMemFill(&LclkNclk, 0x0, sizeof (LclkNclk), StdHeader);

  for (Index = 0; Index < 2; Index++) {
    if (NclkDiv[Index] != 0) {
      for (LclkIndex = 0; LclkIndex < 4; LclkIndex++) {
        if ((PpFuseArray->LclkDpmValid[LclkIndex] != 0) && (PpFuseArray->LclkDpmDid[LclkIndex] != 0)) {
          F14NbLclkNclkAllocatePair (NclkDiv[Index], PpFuseArray->LclkDpmDid[LclkIndex], &LclkNclk[0]);
        }
      }
    }
  };
  for (Index = 0; Index < 8; Index++) {
    if (LclkNclk[Index].NclkDiv != 0 && LclkNclk[Index].LclkDid != 0) {
      UINT32  Nclk10kHz;
      UINT32  Lclk10kHz;
      Nclk10kHz = GfxLibCalculateNclk (LclkNclk[Index].NclkDiv, MainPllFreq10kHz);
      Lclk10kHz = GfxLibCalculateClk (LclkNclk[Index].LclkDid, MainPllFreq10kHz);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Offset for Nclk = %d Lclk = %d\n", Nclk10kHz / 100, Lclk10kHz / 100);
      Lclk_period = 100000000 / Lclk10kHz;
      Nclk_period = 100000000 / Nclk10kHz;

      if ((Nclk10kHz * 2) >= Lclk10kHz) {
        Nclk_offset = (Nclk_period * 35 - 30110) / (Lclk_period * 10);
        Lclk_offset = - 1 - (INT32) ((491 * 10 + Nclk_period * 65 + 3052 * 10 - 1) / (Lclk_period * 10) + 1);
      } else {
        Nclk_offset = - (INT32) (MIN (2 * (961 * 10 + 175 * Lclk_period + 3011 * 10 - 1) / (Nclk_period * 10) + 1,
          2 * (961 * 10 + 165 * Lclk_period + 3011 * 10 - 1) / (Nclk_period * 10) + 1 + 1));
        Lclk_offset = MAX (2 * (35 * Lclk_period  - 3052 * 10) / (Nclk_period * 10),
          2 * (45 * Lclk_period  - 3052 * 10) / (Nclk_period * 10) - 1);
      }
      Nclk_offset = Nclk_offset % 8;
      Lclk_offset = Lclk_offset % 8;

      D18F6x110.Field.NclkFreqType = 1;
      D18F6x110.Field.NclkFreq = LclkNclk[Index].NclkDiv;
      D18F6x110.Field.LclkFreqType = 1;
      D18F6x110.Field.LclkFreq = LclkNclk[Index].LclkDid;
      D18F6x110.Field.Enable = 1;
      D18F6x110.Field.PllMult = D18F3xD4.Field.MainPllOpFreqId + 16;
      D18F6x110.Field.LclkFifoOff = Lclk_offset & 0x7;
      D18F6x110.Field.NclkFifoOff = Nclk_offset & 0x7;

      GnbLibPciWrite (
        MAKE_SBDFO ( 0, 0, 0x18, 6, D18F6x110_ADDRESS + Index * 4),
        AccessS3SaveWidth32,
        &D18F6x110.Value,
        StdHeader
        );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "F14NbLclkNclkRatioFeature Exit\n");
  return AGESA_SUCCESS;
}
