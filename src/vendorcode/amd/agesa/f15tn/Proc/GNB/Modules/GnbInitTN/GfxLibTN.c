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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "S3SaveState.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GfxLibTN.h"
#include  "GnbCommonLib.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GFXLIBTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
CONST UINT16 GfxMemClockFrequencyDefinitionTable [][8] = {
{0 ,  0 ,  0 ,  0 ,  333,  0,  400,  0 },
{0 ,  0 ,  533,  0 ,  0 ,  0 ,  667,  0 },
{0 ,  0 ,   800,  0 ,  0 ,  0 ,  0 ,  0 },
{0 , 1050, 1066, 0 , 0,    0 , 0,    1200}
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GfxFmDisableController (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
GfxFmCalculateClock (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

BOOLEAN
GfxFmIsVbiosPosted (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );
/*----------------------------------------------------------------------------------------*/
/**
 * Disable GFX controller
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 */

VOID
GfxFmDisableController (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  GnbLibPciRMW (
    MAKE_SBDFO (0, 0, 0, 0,D0F0x7C_ADDRESS),
    AccessS3SaveWidth32,
    0xffffffff,
    1 << D0F0x7C_ForceIntGFXDisable_OFFSET,
    StdHeader
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get system PLL COF
 *
 *
 *
 * @param[in] StdHeader  Standard configuration header
 * @retval    System PLL COF
 */
UINT32
GfxLibGetSytemPllCofTN (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  D0F0xBC_xFF000000_STRUCT  D0F0xBC_xFF000000;
  GnbRegisterReadTN (D0F0xBC_xFF000000_TYPE,  D0F0xBC_xFF000000_ADDRESS, &D0F0xBC_xFF000000.Value, 0, StdHeader);
  return 100 * (D0F0xBC_xFF000000.Field.MainPllOpFreqIdStartup + 0x10);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate COF for DFS out of Main PLL
 *
 *
 *
 * @param[in] Did         Did
 * @param[in] StdHeader   Standard Configuration Header
 * @retval                COF in 10khz
 */

UINT32
GfxFmCalculateClock (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32  Divider;
  UINT32  SystemPllCof;
  SystemPllCof = GfxLibGetSytemPllCofTN (StdHeader) * 100;
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
  return (((SystemPllCof * 100) + (Divider - 1)) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set idle voltage mode for GFX
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 */

BOOLEAN
GfxFmIsVbiosPosted (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32  Value;

  GnbRegisterReadTN (GMMx670_TYPE, GMMx670_ADDRESS, &Value, 0, GnbLibGetHeader (Gfx));
  return ((Value & BIT16) == 0) ? TRUE : FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Extract DRAM frequency
 *
 *
 *
 * @param[in] Encoding   Memory Clock Frequency Value Definition
 * @param[in] StdHeader  Standard configuration header
 * @retval    Dram fraquency Mhz
 */
UINT32
GfxLibExtractDramFrequency (
  IN       UINT8                  Encoding,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  if (Encoding >= (sizeof (GfxMemClockFrequencyDefinitionTable) / sizeof (UINT16))) {
    ASSERT (FALSE);
    return 0;
  }
  return GfxMemClockFrequencyDefinitionTable[Encoding / 8][Encoding % 8];
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get max SCLK
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval                    Max SCLK in Mhz
 */
UINT32
GfxLibGetMaxSclk (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32                    MaxSclkClk;
  D0F0xBC_xFF000000_STRUCT  D0F0xBC_xFF000000;
  D0F0xBC_xE0003048_STRUCT  D0F0xBC_xE0003048;

  GnbRegisterReadTN (TYPE_D0F0xBC, D0F0xBC_xFF000000_ADDRESS, &D0F0xBC_xFF000000.Value, 0, StdHeader);
  GnbRegisterReadTN (TYPE_D0F0xBC, D0F0xBC_xE0003048_ADDRESS, &D0F0xBC_xE0003048.Value, 0, StdHeader);
  //sclk_max_freq = 100 * (GCLK_PLL_FUSES.MainPllOptFreqIdStartup + 16) /
  //                (((SCLK_MIN_DIV.INT<<12 + SCLK_MIN_DIV.FRAC)>>12)
  MaxSclkClk = 100 * (D0F0xBC_xFF000000.Field.MainPllOpFreqIdStartup + 16);
  MaxSclkClk /= ((D0F0xBC_xE0003048.Field.Intv << 12) + D0F0xBC_xE0003048.Field.Fracv) >> 12;
  return MaxSclkClk;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of SCLK Dram burst
 *
 *
 * @param[in] ScaleMp         Scaled number of sclk_max_freq / memps_freq
 * @param[in] StdHeader       Standard configuration header
 * @retval                    number of sclks (*2) per dram burst, except (0,1,2,3)=(1,1.25,1.5,1.75)
 */
UINT32
GfxLibGetNumberOfSclkPerDramBurst (
  IN       UINT32                ScaleMp,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{

  if ((2 * ScaleMp)  < 125) {
    return 0; //STATE0 = 0
  } else if ((2 * ScaleMp)  < 150) {
    return 1; //STATE0 = 1
  } else if ((2 * ScaleMp)  < 175) {
    return 2; //STATE0 = 2
  } else if ((2 * ScaleMp)  < 200) {
    return 3; //STATE0 = 3
  } else {
    //STATE0 = floor(4*scale_mp[0] )
    return ((4 * ScaleMp) / 100);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Calculate TN NCLK clock
 *
 *
 *
 * @param[in] NbFid         NbFid
 * @param[in] NbDid         NbDid
 * @retval                  Clock in 10KHz
 */

UINT32
GfxLibGetNclkTN (
  IN      UINT8                   NbFid,
  IN      UINT8                   NbDid
  )
{
  UINT32  Divider;
  //i.e. NBCOF[0] = (100 * (D18F5x160[NbFid] + 4h) / (2^D18F5x160[NbDid])) Mhz
  if (NbDid == 1) {
    Divider = 2;
  } else if (NbDid == 0) {
    Divider = 1;
  } else {
    Divider = 1;
  }
  ASSERT (NbDid == 0 || NbDid == 1);
  return ((10000 * (NbFid + 4)) / Divider);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set VID through CG client
 *
 *
 * @param[in] Vid             VID code
 * @param[in] StdHeader       Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GfxRequestVoltageTN (
  IN       UINT8                 Vid,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  GMMx770_STRUCT  GMMx770;
  GMMx774_STRUCT  GMMx774;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxRequestVoltageTN Enter\n");

  GnbRegisterReadTN (GMMx770_TYPE, GMMx770_ADDRESS, &GMMx770, 0, StdHeader);
  GMMx770.Field.VoltageChangeEn = 1;
  GnbRegisterWriteTN (GMMx770_TYPE, GMMx770_ADDRESS, &GMMx770, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  GMMx770.Field.VoltageLevel = Vid;
  GMMx770.Field.VoltageChangeReq = ~GMMx770.Field.VoltageChangeReq;
  GnbRegisterWriteTN (GMMx770_TYPE, GMMx770_ADDRESS, &GMMx770, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  do {
    GnbRegisterReadTN (GMMx774_TYPE, GMMx774_ADDRESS, &GMMx774, 0, StdHeader);
  } while (GMMx774.Field.VoltageChangeAck != GMMx770.Field.VoltageChangeReq);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxRequestVoltageTN Exit\n");
  return AGESA_SUCCESS;
}



/*----------------------------------------------------------------------------------------*/
/**
 * Set SCLK
 *
 *
 * @param[in] Did             Devider
 * @param[in] StdHeader       Standard configuration header
 * @retval    previous DID
 */

UINT8
GfxRequestSclkTNS3Save (
  IN       UINT8                 Did,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  S3_SAVE_DISPATCH (StdHeader, GfxRequestSclkTNS3Script_ID, sizeof (Did), &Did);
  return GfxRequestSclkTN (Did, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set SCLK
 *
 *
 * @param[in] Did             Devider
 * @param[in] StdHeader       Standard configuration header
 * @retval    AGESA_STATUS
 */

UINT8
GfxRequestSclkTN (
  IN       UINT8                 Did,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  GMMx600_STRUCT  GMMx600;
  GMMx604_STRUCT  GMMx604;
  UINT8           OriginalDid;
  do {
    GnbRegisterReadTN (GMMx604_TYPE, GMMx604_ADDRESS, &GMMx604, 0, StdHeader);
  } while (GMMx604.Field.SclkStatus == 0);
  GnbRegisterReadTN (GMMx600_TYPE, GMMx600_ADDRESS, &GMMx600, 0, StdHeader);
  OriginalDid = (UINT8) GMMx600.Field.IndClkDiv;
  GMMx600.Field.IndClkDiv = Did;
  GnbRegisterWriteTN (GMMx600_TYPE, GMMx600_ADDRESS, &GMMx600, 0, StdHeader);
  do {
    GnbRegisterReadTN (GMMx604_TYPE, GMMx604_ADDRESS, &GMMx604, 0, StdHeader);
  } while (GMMx604.Field.SclkStatus == 0);
  return OriginalDid;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set Sclk in S3 script
 *
 *
 *
 * @param[in]  StdHeader      Standard configuration header
 * @param[in]  ContextLength  Context Length (not used)
 * @param[in]  Context        pointer to UINT32 number of us
 */
VOID
GfxRequestSclkTNS3Script (
  IN      AMD_CONFIG_PARAMS    *StdHeader,
  IN      UINT16               ContextLength,
  IN      VOID*                Context
  )
{
  GfxRequestSclkTN (* ((UINT8*) Context), StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Calculate did from main VCO
 *
 *
 *
 * @param[in] Vco           Vco in 10Khz
 * @param[in] StdHeader     Standard configuration header
 * @retval                  DID
 */

UINT8
GfxLibCalculateDidTN (
  IN       UINT32                Vco,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32  Divider;
  UINT32  SystemPllCof;
  UINT8   Did;
  ASSERT (Vco != 0);
  SystemPllCof = GfxLibGetSytemPllCofTN (StdHeader) * 100;
  Divider = ((SystemPllCof * 100) + (Vco - 1)) / Vco;
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

