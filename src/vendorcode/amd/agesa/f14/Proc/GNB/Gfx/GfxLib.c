/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize PP/DPM fuse table.
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
#include  "Ids.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbFuseTable.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GfxLib.h"
#include  "GnbRegistersON.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_GFXLIB_FILECODE
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

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate main PLL VCO
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 * @retval    main PLL COF in Mhz
 */

UINT32
GfxLibGetMainPllFreq (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32            MainPllFreq;
  D18F3xD4_STRUCT   D18F3xD4;
  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3xD4_ADDRESS),
    AccessWidth32,
    &D18F3xD4.Value,
    StdHeader
    );
  if (D18F3xD4.Field.MainPllOpFreqIdEn == 1) {
    MainPllFreq = 100 * (D18F3xD4.Field.MainPllOpFreqId + 0x10);
  } else {
    MainPllFreq = 1600;
  }
  return  MainPllFreq;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate clock from main VCO
 *
 *
 *
 * @param[in] Did           Fuse Divider
 * @param[in] MainPllVco    Main Pll COF in 10KHz
 * @retval                  Clock in 10KHz
 */

UINT32
GfxLibCalculateClk (
  IN      UINT8                   Did,
  IN      UINT32                  MainPllVco
  )
{
  UINT32  Divider;
  if (Did >= 8 && Did <= 0x3F) {
    Divider = Did * 25;
  } else if (Did > 0x3F && Did <= 0x5F) {
    Divider = (Did - 64) * 50 + 1600;
  } else if (Did > 0x5F && Did <= 0x7E) {
    Divider = (Did - 96) * 100 + 3200;
  } else if (Did == 0x7f) {
    Divider = 128 * 100;
  } else {
    ASSERT (FALSE);
    return 200 * 100;
  }
  ASSERT (Divider != 0);
  return (((MainPllVco * 100) + (Divider - 1)) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate did from main VCO
 *
 *
 *
 * @param[in] Vco           Vco in 10Khz
 * @param[in] MainPllVco    Main Pll COF in 10Khz
 * @retval                  DID
 */

UINT8
GfxLibCalculateDid (
  IN      UINT32                  Vco,
  IN      UINT32                  MainPllVco
  )
{
  UINT32  Divider;
  UINT8   Did;
  ASSERT (Vco != 0);
  Divider = ((MainPllVco * 100) + (Vco - 1)) / Vco;
  Did = 0;
  if (Divider < 200) {
  } else if (Divider <= 1575) {
    Did = (UINT8) (Divider / 25);
  } else if (Divider <= 3150) {
    Did = (UINT8) ((Divider - 1600) / 50) + 64;
  } else if (Divider <= 6200) {
    Did = (UINT8) ((Divider - 3200) / 100) + 96;
  } else {
    Did = 0x7f;
  }
  return Did;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if GFX controller fused off
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval    TRUE            Gfx controller present and available
 */
BOOLEAN
GfxLibIsControllerPresent (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  return GnbLibPciIsDevicePresent (MAKE_SBDFO (0, 0, 1, 0, 0), StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get max non 0 VID index
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    NBVDD VID index
 */
UINT8
GfxLibMaxVidIndex (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8           MaxVid;
  UINT8           MaxVidIndex;
  UINT8           SclkVidArray[4];
  UINTN           Index;

  GnbLibPciRead (
    MAKE_SBDFO ( 0, 0, 0x18, 3, D18F3x15C_ADDRESS),
    AccessWidth32,
    &SclkVidArray[0],
    StdHeader
    );
  MaxVidIndex = 0;
  MaxVid = 0xff;
  for (Index = 0; Index < 4; Index++) {
    if (SclkVidArray[Index] != 0 && SclkVidArray[Index] < MaxVid) {
      MaxVid = SclkVidArray[Index];
      MaxVidIndex = (UINT8) Index;
    }
  }
  return MaxVidIndex;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get min SCLK
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    Min SCLK in 10 khz
 */
UINT32
GfxLibGetMinSclk (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32  WrCkClk;
  UINT32  MinSclkClk;
  WrCkClk = GfxLibGetWrCk (StdHeader);

  if ((2 * WrCkClk) < (8 * 100)) {
    MinSclkClk = 8 * 100;
  } else {
    MinSclkClk = 2 * WrCkClk + 100;
  }
  return MinSclkClk;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get min WRCK
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    Min WRCK in 10 khZ
 */
UINT32
GfxLibGetWrCk (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PP_FUSE_ARRAY   *PpFuseArray;
  UINT8           WrCk;
  PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray != NULL) {
    if (PpFuseArray->WrCkDid == 0x0) {
      WrCk = 2;
    } else if (PpFuseArray->WrCkDid <= 0x10) {
      WrCk = PpFuseArray->WrCkDid + 1;
    } else if (PpFuseArray->WrCkDid <= 0x1C) {
      WrCk = 24 + 8 * (PpFuseArray->WrCkDid - 0x10);
    } else {
      WrCk = 128;
    }
  } else {
    WrCk = 2;
  }
  return 100 * 100 / WrCk;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate NCLK clock from main VCO
 *
 *
 *
 * @param[in] Did           NCLK Divider
 * @param[in] MainPllVco    Main Pll COF in 10KHz
 * @retval                  Clock in 10KHz
 */

UINT32
GfxLibCalculateNclk (
  IN      UINT8                   Did,
  IN      UINT32                  MainPllVco
  )
{
  UINT32  Divider;
  if (Did >= 8 && Did <= 0x3F) {
    Divider = Did * 25;
  } else if (Did > 0x3F && Did <= 0x5F) {
    Divider = (Did - 64) * 50 + 1600;
  } else if (Did > 0x5F && Did <= 0x7F) {
    Divider = (Did - 64) * 100;
  } else {
    ASSERT (FALSE);
    return 200 * 100;
  }
  ASSERT (Divider != 0);
  return ((MainPllVco * 100) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate idle NCLK clock from main VCO
 *
 *
 *
 * @param[in] Did           NCLK Divider
 * @param[in] MainPllVco    Main Pll COF in 10KHz
 * @retval                  Clock in 10KHz
 */

UINT32
GfxLibCalculateIdleNclk (
  IN      UINT8                   Did,
  IN      UINT32                  MainPllVco
  )
{
  UINT32  Divider;
  switch (Did) {
  case 0x20:
    Divider = 8;
    break;
  case 0x40:
    Divider = 16;
    break;
  case 0x60:
    Divider = 32;
    break;
  case 0x78:
    Divider = 56;
    break;
  case 0x7F:
    Divider = 128;
    break;
  default:
    ASSERT (FALSE);
    return 200 * 100;
    break;
  }

  return (MainPllVco / Divider);
}
