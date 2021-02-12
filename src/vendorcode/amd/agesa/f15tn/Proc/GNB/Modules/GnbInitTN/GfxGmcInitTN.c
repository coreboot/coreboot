/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 64152 $   @e \$Date: 2012-01-16 21:38:07 -0600 (Mon, 16 Jan 2012) $
 *
 */
/*
*****************************************************************************
*
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
* ***************************************************************************
*
*/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbRegisterAccTN.h"
#include  "cpuFamilyTranslation.h"
#include  "GnbRegistersTN.h"
#include  "GfxLibTN.h"
#include  "GfxGmcInitTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GFXGMCINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_TABLE ROMDATA GfxGmcColockGatingDisableTN [];
extern GNB_TABLE ROMDATA GfxGmcInitTableTN [];
extern GNB_TABLE ROMDATA GfxGmcColockGatingEnableTN [];


#define GNB_GFX_DRAM_CH_0_PRESENT 1
#define GNB_GFX_DRAM_CH_1_PRESENT 2

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

DCT_REGISTER_ENTRY DctRegisterTable [] = {
  {
    TYPE_D18F2_dct0,
    D18F2x94_dct0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x94_dct0)
  },
  {
    TYPE_D18F2_dct1,
    D18F2x94_dct1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x94_dct1)
  },
  {
    TYPE_D18F2_dct0,
    D18F2x2E0_dct0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x2E0_dct0)
  },
  {
    TYPE_D18F2_dct1,
    D18F2x2E0_dct1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x2E0_dct1)
  },
  {
    TYPE_D18F2_dct0_mp0,
    D18F2x200_dct0_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x200_dct0_mp0)
  },
  {
    TYPE_D18F2_dct0_mp1,
    D18F2x200_dct0_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x200_dct0_mp1)
  },
  {
    TYPE_D18F2_dct1_mp0,
    D18F2x200_dct1_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x200_dct1_mp0)
  },
  {
    TYPE_D18F2_dct1_mp1,
    D18F2x200_dct1_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x200_dct1_mp1)
  },
  {
    TYPE_D18F2_dct0_mp0,
    D18F2x204_dct0_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x204_dct0_mp0)
  },
  {
    TYPE_D18F2_dct0_mp1,
    D18F2x204_dct0_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x204_dct0_mp1)
  },
  {
    TYPE_D18F2_dct1_mp0,
    D18F2x204_dct1_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x204_dct1_mp0)
  },
  {
    TYPE_D18F2_dct1_mp1,
    D18F2x204_dct1_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x204_dct1_mp1)
  },
  {
    TYPE_D18F2_dct0_mp0,
    D18F2x22C_dct0_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x22C_dct0_mp0)
  },
  {
    TYPE_D18F2_dct0_mp1,
    D18F2x22C_dct0_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x22C_dct0_mp1)
  },
  {
    TYPE_D18F2_dct1_mp0,
    D18F2x22C_dct1_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x22C_dct1_mp0)
  },
  {
    TYPE_D18F2_dct1_mp1,
    D18F2x22C_dct1_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x22C_dct1_mp1)
  },
  {
    TYPE_D18F2_dct0_mp0,
    D18F2x21C_dct0_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x21C_dct0_mp0)
  },
  {
    TYPE_D18F2_dct0_mp1,
    D18F2x21C_dct0_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x21C_dct0_mp1)
  },
  {
    TYPE_D18F2_dct1_mp0,
    D18F2x21C_dct1_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x21C_dct1_mp0)
  },
  {
    TYPE_D18F2_dct1_mp1,
    D18F2x21C_dct1_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x21C_dct1_mp1)
  },
  {
    TYPE_D18F2_dct0_mp0,
    D18F2x20C_dct0_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x20C_dct0_mp0)
  },
  {
    TYPE_D18F2_dct0_mp1,
    D18F2x20C_dct0_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x20C_dct0_mp1)
  },
  {
    TYPE_D18F2_dct1_mp0,
    D18F2x20C_dct1_mp0_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x20C_dct1_mp0)
  },
  {
    TYPE_D18F2_dct1_mp1,
    D18F2x20C_dct1_mp1_ADDRESS,
    (UINT16) offsetof (DCT_CHANNEL_INFO, D18F2x20C_dct1_mp1)
  }
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize Fb location
 *
 *
 *
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcInitializeFbLocationTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx2024_STRUCT   GMMx2024;
  GMMx2068_STRUCT   GMMx2068;
  GMMx2C04_STRUCT   GMMx2C04;
  GMMx5428_STRUCT   GMMx5428;
  UINT64            FBBase;
  UINT64            FBTop;
  FBBase = 0x0F00000000;
  FBTop = FBBase + Gfx->UmaInfo.UmaSize - 1;
  GMMx2024.Value = 0;
  GMMx2C04.Value = 0;
  GMMx2024.Field.FB_BASE = (UINT16) (FBBase >> 24);
  GMMx2024.Field.FB_TOP = (UINT16) (FBTop >> 24);
  GMMx2068.Field.FB_OFFSET = (UINT32) (Gfx->UmaInfo.UmaBase >> 22);
  GMMx2C04.Field.NONSURF_BASE = (UINT32) (FBBase >> 8);
  GMMx5428.Field.CONFIG_MEMSIZE = Gfx->UmaInfo.UmaSize >> 20;
  GnbRegisterWriteTN (GMMx2024_TYPE, GMMx2024_ADDRESS, &GMMx2024.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (GMMx2068_TYPE, GMMx2068_ADDRESS, &GMMx2068.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (GMMx2C04_TYPE, GMMx2C04_ADDRESS, &GMMx2C04.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (GMMx5428_TYPE, GMMx5428_ADDRESS, &GMMx5428.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get Sequencer model info
 *
 *
 * @param[out] DctChannelInfo  Various DCT/GMM info
 * @param[in]  Gfx             Pointer to global GFX configuration
 */

STATIC VOID
GfxGmcDctMemoryChannelInfoTN (
     OUT   DCT_CHANNEL_INFO              *DctChannelInfo,
  IN       GFX_PLATFORM_CONFIG           *Gfx
  )
{

  UINT32       Index;
  UINT32       Value;

  for (Index = 0; Index < ARRAY_SIZE(DctRegisterTable); Index++) {
    GnbRegisterReadTN (
      DctRegisterTable[Index].RegisterSpaceType,
      DctRegisterTable[Index].Address,
      &Value,
      0,
      GnbLibGetHeader (Gfx)
    );
    *(UINT32 *)((UINT8 *) DctChannelInfo + DctRegisterTable[Index].DctChannelInfoTableOffset) = Value;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize sequencer model
 *
 *
 *
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcInitializeSequencerTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{

  UINT32                    memps0_freq;
  UINT32                    memps1_freq;
  UINT32                    scale_mp0;
  UINT32                    scale_mp1;
  UINT8                     DramChannelPresent;
  ex1047_STRUCT      ex1047      ;
  ex1048_STRUCT          ex1048 ;
  ex1060_STRUCT        ex1060    ;
  ex1061_STRUCT        ex1061    ;
  ex1062_STRUCT        ex1062    ;
  DCT_CHANNEL_INFO          DctChannel;
  D18F5x170_STRUCT          D18F5x170;
  ex1012_STRUCT         ex1012  ;
  ex1034_STRUCT        ex1034    ;

  GfxGmcDctMemoryChannelInfoTN (&DctChannel, Gfx);

  DramChannelPresent = 0;
  if (!DctChannel.D18F2x94_dct1.Field.DisDramInterface) {
    DramChannelPresent |= GNB_GFX_DRAM_CH_1_PRESENT;
  }

  if (!DctChannel.D18F2x94_dct0.Field.DisDramInterface) {
    //if (channel 0 present)
    //memps0_freq = extract frequency from DRAM Configuration High <D18F2x094_dct[0]>[4:0] encoding
    //memps1_freq = extract frequency from Memory P-state Control Status <D18F2x2E0_dct[0]>[28:24] encoding
    DramChannelPresent |= GNB_GFX_DRAM_CH_0_PRESENT;
    memps0_freq = GfxLibExtractDramFrequency ((UINT8) DctChannel.D18F2x94_dct0.Field.MemClkFreq, GnbLibGetHeader (Gfx));
    memps1_freq = GfxLibExtractDramFrequency ((UINT8) DctChannel.D18F2x2E0_dct0.Field.M1MemClkFreq, GnbLibGetHeader (Gfx));
  } else {
    //memps0_freq = extract frequency from DRAM Configuration High <D18F2x094_dct[1]>[4:0] encoding
    //memps1_freq = extract frequency from Memory P-state Control Status <D18F2x2E0_dct[1]>[28:24] encoding
    memps0_freq = GfxLibExtractDramFrequency ((UINT8) DctChannel.D18F2x94_dct1.Field.MemClkFreq, GnbLibGetHeader (Gfx));
    memps1_freq = GfxLibExtractDramFrequency ((UINT8) DctChannel.D18F2x2E0_dct1.Field.M1MemClkFreq, GnbLibGetHeader (Gfx));
  }

  GnbRegisterReadTN (D18F5x170_TYPE, D18F5x170_ADDRESS, &D18F5x170.Value, 0, GnbLibGetHeader (Gfx));
  if (D18F5x170.Field.MemPstateDis == 1) {
    memps1_freq = memps0_freq;
  }

  //scale_mp0 = sclk_max_freq / memps0_freq
  //scale_mp1 = sclk_max_freq / memps1_freq
  //Multiply it by 100 to avoid dealing with floating point values
  scale_mp0 = (GfxLibGetMaxSclk (GnbLibGetHeader (Gfx)) * 100) / memps0_freq;
  scale_mp1 = (GfxLibGetMaxSclk (GnbLibGetHeader (Gfx)) * 100) / memps1_freq;

  GnbRegisterReadTN (TYPE_GMM , 0x2774 , &ex1047.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadTN (TYPE_GMM , 0x2778 , &ex1048.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadTN (TYPE_GMM , 0x27f0 , &ex1060.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadTN (TYPE_GMM , 0x27fc , &ex1061.Value, 0, GnbLibGetHeader (Gfx));

  if (((DramChannelPresent & GNB_GFX_DRAM_CH_0_PRESENT) != 0) && ((DramChannelPresent & GNB_GFX_DRAM_CH_1_PRESENT) != 0)) {
    ex1047.Field.ex1047_0 = (MIN_UNSAFE (DctChannel.D18F2x200_dct0_mp0.Field.Trcd, DctChannel.D18F2x200_dct1_mp0.Field.Trcd) * scale_mp0) / 100;
    ex1047.Field.ex1047_1 = ex1047.Field.ex1047_0;
    ex1047.Field.ex1047_2 = (MIN_UNSAFE ((DctChannel.D18F2x204_dct0_mp0.Field.Trc - DctChannel.D18F2x200_dct0_mp0.Field.Trcd),
                                     (DctChannel.D18F2x204_dct1_mp0.Field.Trc - DctChannel.D18F2x200_dct1_mp0.Field.Trcd)) * scale_mp0) / 100;
    ex1047.Field.ex1047_3 = ex1047.Field.ex1047_2;

    ex1048.Field.ex1048_0 = (MIN_UNSAFE (DctChannel.D18F2x204_dct0_mp0.Field.Trc, DctChannel.D18F2x204_dct1_mp0.Field.Trc) * scale_mp0) / 100;
    ex1048.Field.ex1048_1 = (MIN_UNSAFE (DctChannel.D18F2x200_dct0_mp0.Field.Trp, DctChannel.D18F2x200_dct1_mp0.Field.Trp) * scale_mp0) / 100;
    ex1048.Field.ex1048_2 = (MIN_UNSAFE ((DctChannel.D18F2x22C_dct0_mp0.Field.Twr + DctChannel.D18F2x200_dct0_mp0.Field.Trp),
                                    (DctChannel.D18F2x22C_dct1_mp0.Field.Twr + DctChannel.D18F2x200_dct1_mp0.Field.Trp)) * scale_mp0) / 100;
    ex1048.Field.ex1048_3 = ((MIN_UNSAFE ((DctChannel.D18F2x20C_dct0_mp0.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct0_mp0.Field.Twtr + DctChannel.D18F2x21C_dct0_mp0.Field.TrwtTO),
                                     (DctChannel.D18F2x20C_dct1_mp0.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct1_mp0.Field.Twtr + DctChannel.D18F2x21C_dct1_mp0.Field.TrwtTO)) / 2) * scale_mp0) / 100;

    ex1060.Field.ex1060_0 = (MIN_UNSAFE (DctChannel.D18F2x200_dct0_mp1.Field.Trcd, DctChannel.D18F2x200_dct1_mp1.Field.Trcd) * scale_mp1) / 100;
    ex1060.Field.ex1060_1 = ex1060.Field.ex1060_0;
    ex1060.Field.ex1060_2 = (MIN_UNSAFE ((DctChannel.D18F2x204_dct0_mp1.Field.Trc - DctChannel.D18F2x200_dct0_mp1.Field.Trcd),
                                     (DctChannel.D18F2x204_dct1_mp1.Field.Trc - DctChannel.D18F2x200_dct1_mp1.Field.Trcd)) * scale_mp1) / 100;
    ex1060.Field.ex1060_3 = ex1060.Field.ex1060_2;

    ex1061.Field.ex1061_0 = (MIN_UNSAFE (DctChannel.D18F2x204_dct0_mp1.Field.Trc, DctChannel.D18F2x204_dct1_mp1.Field.Trc) * scale_mp1) / 100;
    ex1061.Field.ex1061_1 = (MIN_UNSAFE (DctChannel.D18F2x200_dct0_mp1.Field.Trp, DctChannel.D18F2x200_dct1_mp1.Field.Trp) * scale_mp1) / 100;
    ex1061.Field.ex1061_2 = (MIN_UNSAFE ((DctChannel.D18F2x22C_dct0_mp1.Field.Twr + DctChannel.D18F2x200_dct0_mp1.Field.Trp),
                                    (DctChannel.D18F2x22C_dct1_mp1.Field.Twr + DctChannel.D18F2x200_dct1_mp1.Field.Trp)) * scale_mp1) / 100;
    ex1061.Field.ex1061_3 = ((MIN_UNSAFE ((DctChannel.D18F2x20C_dct0_mp1.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct0_mp1.Field.Twtr + DctChannel.D18F2x21C_dct0_mp1.Field.TrwtTO),
                                     (DctChannel.D18F2x20C_dct1_mp1.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct1_mp1.Field.Twtr + DctChannel.D18F2x21C_dct1_mp1.Field.TrwtTO)) / 2) * scale_mp1) / 100;

  } else if ((DramChannelPresent & GNB_GFX_DRAM_CH_0_PRESENT) != 0) {
    ex1047.Field.ex1047_0 = (DctChannel.D18F2x200_dct0_mp0.Field.Trcd * scale_mp0) / 100;
    ex1047.Field.ex1047_1 = ex1047.Field.ex1047_0;
    ex1047.Field.ex1047_2 = ((DctChannel.D18F2x204_dct0_mp0.Field.Trc - DctChannel.D18F2x200_dct0_mp0.Field.Trcd) * scale_mp0) / 100;
    ex1047.Field.ex1047_3 = ex1047.Field.ex1047_2;

    ex1048.Field.ex1048_0 = (DctChannel.D18F2x204_dct0_mp0.Field.Trc * scale_mp0) / 100;
    ex1048.Field.ex1048_1 = (DctChannel.D18F2x200_dct0_mp0.Field.Trp * scale_mp0) / 100;
    ex1048.Field.ex1048_2 = ((DctChannel.D18F2x22C_dct0_mp0.Field.Twr + DctChannel.D18F2x200_dct0_mp0.Field.Trp) * scale_mp0) / 100;
    ex1048.Field.ex1048_3 = (((DctChannel.D18F2x20C_dct0_mp0.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct0_mp0.Field.Twtr + DctChannel.D18F2x21C_dct0_mp0.Field.TrwtTO) / 2) * scale_mp0) / 100;

    ex1060.Field.ex1060_0 = (DctChannel.D18F2x200_dct0_mp1.Field.Trcd * scale_mp1) / 100;
    ex1060.Field.ex1060_1 = ex1060.Field.ex1060_0;
    ex1060.Field.ex1060_2 = ((DctChannel.D18F2x204_dct0_mp1.Field.Trc - DctChannel.D18F2x200_dct0_mp1.Field.Trcd) * scale_mp1) / 100;
    ex1060.Field.ex1060_3 = ex1060.Field.ex1060_2;

    ex1061.Field.ex1061_0 = (DctChannel.D18F2x204_dct0_mp1.Field.Trc * scale_mp1) / 100;
    ex1061.Field.ex1061_1 = (DctChannel.D18F2x200_dct0_mp1.Field.Trp * scale_mp1) / 100;
    ex1061.Field.ex1061_2 = ((DctChannel.D18F2x22C_dct0_mp1.Field.Twr + DctChannel.D18F2x200_dct0_mp1.Field.Trp) * scale_mp1) / 100;
    ex1061.Field.ex1061_3 = (((DctChannel.D18F2x20C_dct0_mp1.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct0_mp1.Field.Twtr + DctChannel.D18F2x21C_dct0_mp1.Field.TrwtTO) / 2) * scale_mp1) / 100;

  } else {
    ex1047.Field.ex1047_0 = (DctChannel.D18F2x200_dct1_mp0.Field.Trcd * scale_mp0) / 100;
    ex1047.Field.ex1047_1 = ex1047.Field.ex1047_0;
    ex1047.Field.ex1047_2 = ((DctChannel.D18F2x204_dct1_mp0.Field.Trc - DctChannel.D18F2x200_dct1_mp0.Field.Trcd) * scale_mp0) / 100;
    ex1047.Field.ex1047_3 = ex1047.Field.ex1047_2;

    ex1048.Field.ex1048_0 = (DctChannel.D18F2x204_dct1_mp0.Field.Trc * scale_mp0) / 100;
    ex1048.Field.ex1048_1 = (DctChannel.D18F2x200_dct1_mp0.Field.Trp * scale_mp0) / 100;
    ex1048.Field.ex1048_2 = ((DctChannel.D18F2x22C_dct1_mp0.Field.Twr + DctChannel.D18F2x200_dct1_mp0.Field.Trp) * scale_mp0) / 100;
    ex1048.Field.ex1048_3 = (((DctChannel.D18F2x20C_dct1_mp0.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct1_mp0.Field.Twtr + DctChannel.D18F2x21C_dct1_mp0.Field.TrwtTO) / 2) * scale_mp0) / 100;

    ex1060.Field.ex1060_0 = (DctChannel.D18F2x200_dct1_mp1.Field.Trcd * scale_mp1) / 100;
    ex1060.Field.ex1060_1 = ex1060.Field.ex1060_0;
    ex1060.Field.ex1060_2 = ((DctChannel.D18F2x204_dct1_mp1.Field.Trc - DctChannel.D18F2x200_dct1_mp1.Field.Trcd) * scale_mp1) / 100;
    ex1060.Field.ex1060_3 = ex1060.Field.ex1060_2;

    ex1061.Field.ex1061_0 = (DctChannel.D18F2x204_dct1_mp1.Field.Trc * scale_mp1) / 100;
    ex1061.Field.ex1061_1 = (DctChannel.D18F2x200_dct1_mp1.Field.Trp * scale_mp1) / 100;
    ex1061.Field.ex1061_2 = ((DctChannel.D18F2x22C_dct1_mp1.Field.Twr + DctChannel.D18F2x200_dct1_mp1.Field.Trp) * scale_mp1) / 100;
    ex1061.Field.ex1061_3 = (((DctChannel.D18F2x20C_dct1_mp1.Field.Tcwl + 4 + DctChannel.D18F2x20C_dct1_mp1.Field.Twtr + DctChannel.D18F2x21C_dct1_mp1.Field.TrwtTO) / 2) * scale_mp1) / 100;
  }

  GnbRegisterWriteTN (TYPE_GMM , 0x2774 , &ex1047.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (TYPE_GMM , 0x2778 , &ex1048.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (TYPE_GMM , 0x27f0 , &ex1060.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (TYPE_GMM , 0x27fc , &ex1061.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  ex1062.Field.ex1062_0  = GfxLibGetNumberOfSclkPerDramBurst (scale_mp0, GnbLibGetHeader (Gfx));
  ex1062.Field.ex1062_1  = GfxLibGetNumberOfSclkPerDramBurst (scale_mp1, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (TYPE_GMM , 0x2808 , &ex1062.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));


  //MC Performance settings base on memory channel configuration
  //If 1 channel
  ex1012.Value = 0x210;
  ex1034.Value = 0x3;
  if (((DramChannelPresent & GNB_GFX_DRAM_CH_0_PRESENT) != 0) && ((DramChannelPresent & GNB_GFX_DRAM_CH_1_PRESENT) != 0)) {
    //If 2 channels
    ex1012.Value = 0x1210;
    ex1034.Value = 0xC3;
  }
  GnbRegisterWriteTN (TYPE_GMM , 0x2004 , &ex1012.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteTN (TYPE_GMM , 0x2214 , &ex1034.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
}

/*----------------------------------------------------------------------------------------*/
/**
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

STATIC VOID
GfxGmcSecureGarlicAccessTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  ex1064_STRUCT    ex1064 ;
  ex1065_STRUCT    ex1065 ;
  GMMx287C_STRUCT   GMMx287C;

  ex1064.Value = (UINT32) (Gfx->UmaInfo.UmaBase >> 20);
  GnbRegisterWriteTN (TYPE_GMM , 0x2868 , &ex1064.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  ex1065.Value = (UINT32) (((Gfx->UmaInfo.UmaBase + Gfx->UmaInfo.UmaSize) >> 20) - 1);
  GnbRegisterWriteTN (TYPE_GMM , 0x286c , &ex1065.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  // Areag FB - 32K reserved by VBIOS for SBIOS to use
  GMMx287C.Value = (UINT32) ((Gfx->UmaInfo.UmaBase + Gfx->UmaInfo.UmaSize - 32 * 1024) >> 12);
  GnbRegisterWriteTN (GMMx287C_TYPE, GMMx287C_ADDRESS, &GMMx287C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize C6 aperture location
 *
 *
 *
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcInitializeC6LocationTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D18F2x118_STRUCT    D18F2x118;
  D18F1x44_STRUCT     D18F1x44;
  GMMx2870_STRUCT     GMMx2870;
  GMMx2874_STRUCT     GMMx2874;

  // From D18F1x[144:140,44:40] DRAM Base/Limit,
  // {DramBase[47:24], 00_0000h} <= address[47:0] <= {DramLimit[47:24], FF_FFFFh}.
  GnbRegisterReadTN (D18F1x44_TYPE, D18F1x44_ADDRESS, &D18F1x44.Value, 0, GnbLibGetHeader (Gfx));
  //
  // base 39:20, base = Dram Limit + 1
  // ex: system 256 MB on Node 0, D18F1x44.Field.DramLimit_39_24_ = 0xE (240MB -1)
  // Node DRAM     D18F1x[144:140,44:40]    CC6DRAMRange         D18F4x128   D18F1x120 D18F1x124
  //    0 256MB    0MB ~ 240 MB - 1        240 MB ~ 256 MB - 1     0           0 MB,    256 MB - 1
  //

  // base 39:20
  GMMx2870.Value = ((D18F1x44.Field.DramLimit_39_24_ + 1) << 4);
  // top 39:20
  GMMx2874.Value = (((D18F1x44.Field.DramLimit_39_24_ + 1) << 24) + (16 * 0x100000) - 1) >> 20;

  // Check C6 enable,  D18F2x118[CC6SaveEn]
  GnbRegisterReadTN (TYPE_D18F2 , 0x118 , &D18F2x118.Value, 0, GnbLibGetHeader (Gfx));

  if (D18F2x118.Field.CC6SaveEn) {

    GnbRegisterWriteTN (GMMx2874_TYPE, GMMx2874_ADDRESS, &GMMx2874.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbRegisterWriteTN (GMMx2870_TYPE, GMMx2870_ADDRESS, &GMMx2870.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize GMC
 *
 *
 *
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */

AGESA_STATUS
GfxGmcInitTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx28D8_STRUCT   GMMx28D8;
  ex1017_STRUCT    ex1017 ;
  GNB_HANDLE        *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInitTN Enter\n");
   GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
   ASSERT (GnbHandle != NULL);
   GnbProcessTable (
     GnbHandle,
     GfxGmcColockGatingDisableTN,
     0,
     GNB_TABLE_FLAGS_FORCE_S3_SAVE,
     GnbLibGetHeader (Gfx)
     );
   GfxGmcInitializeSequencerTN (Gfx);
   GfxGmcInitializeFbLocationTN (Gfx);
   GfxGmcSecureGarlicAccessTN (Gfx);
   GfxGmcInitializeC6LocationTN (Gfx);
   GnbProcessTable (
     GnbHandle,
     GfxGmcInitTableTN,
     0,
     GNB_TABLE_FLAGS_FORCE_S3_SAVE,
     GnbLibGetHeader (Gfx)
     );
   if (Gfx->GmcClockGating) {
     GnbProcessTable (
       GnbHandle,
       GfxGmcColockGatingEnableTN,
       0,
       GNB_TABLE_FLAGS_FORCE_S3_SAVE,
       GnbLibGetHeader (Gfx)
       );
   }
   if (Gfx->UmaSteering == excel993 ) {
     ex1017.Value = 0x2;
     GnbRegisterWriteTN (TYPE_GMM , 0x206c , &ex1017.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
   }
   IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_GMM_REGISTER_OVERRIDE, Gfx, GnbLibGetHeader (Gfx));
   if (Gfx->GmcLockRegisters) {
     GnbRegisterReadTN (GMMx28D8_TYPE, GMMx28D8_ADDRESS, &GMMx28D8.Value, 0, GnbLibGetHeader (Gfx));
     GMMx28D8.Field.CRITICAL_REGS_LOCK = 1;
     GnbRegisterWriteTN (GMMx28D8_TYPE, GMMx28D8_ADDRESS, &GMMx28D8.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
   }
   if (Gfx->GmcPowerGating != GmcPowerGatingDisabled) {
   }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInitTN Exit\n");
  return AGESA_SUCCESS;
}
