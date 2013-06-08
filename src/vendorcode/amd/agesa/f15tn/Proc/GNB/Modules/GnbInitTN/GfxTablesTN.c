/* $NoKeywords:$ */
/**
 * @file
 *
 * GFx tables
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 64726 $   @e \$Date: 2012-01-30 01:00:01 -0600 (Mon, 30 Jan 2012) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbRegistersTN.h"
#include  "cpuFamilyTranslation.h"
#include  "GnbInitTN.h"

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


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */

GNB_TABLE ROMDATA GfxGmcColockGatingDisableTN [] = {
  //2.1 Disable clock-gating
  GNB_ENTRY_WR (TYPE_GMM , 0x20c0 , 0x00000C80),
  GNB_ENTRY_WR (TYPE_GMM , 0x2478 , 0x00000400),
  GNB_ENTRY_WR (TYPE_GMM , 0x20b8 , 0x00000400),
  GNB_ENTRY_WR (TYPE_GMM , 0x20bc , 0x00000400),
  GNB_ENTRY_WR (TYPE_GMM , 0x2648 , 0x00000400),
  GNB_ENTRY_WR (TYPE_GMM , 0x264c , 0x00000400),
  GNB_ENTRY_WR (TYPE_GMM , 0x2650 , 0x00000400),
  GNB_ENTRY_WR (TYPE_GMM , 0x15c0 , 0x00001401),
  GNB_ENTRY_TERMINATE
};


GNB_TABLE ROMDATA GfxGmcInitTableTN [] = {
  GNB_ENTRY_RMW (D18F5x178_TYPE, D18F5x178_ADDRESS, D18F5x178_SwGfxDis_MASK, 0 << D18F5x178_SwGfxDis_OFFSET),
  //2.2 System memory address translation
  GNB_ENTRY_COPY (GMMx2814_TYPE, GMMx2814_ADDRESS,  0, 32, D18F2x40_dct0_TYPE, D18F2x40_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2818_TYPE, GMMx2818_ADDRESS,  0, 32, D18F2x40_dct1_TYPE, D18F2x40_dct1_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx281C_TYPE, GMMx281C_ADDRESS,  0, 32, D18F2x44_dct0_TYPE, D18F2x44_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2820_TYPE, GMMx2820_ADDRESS,  0, 32, D18F2x44_dct1_TYPE, D18F2x44_dct1_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2824_TYPE, GMMx2824_ADDRESS,  0, 32, D18F2x48_dct0_TYPE, D18F2x48_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2828_TYPE, GMMx2828_ADDRESS,  0, 32, D18F2x48_dct1_TYPE, D18F2x48_dct1_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx282C_TYPE, GMMx282C_ADDRESS,  0, 32, D18F2x4C_dct0_TYPE, D18F2x4C_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2830_TYPE, GMMx2830_ADDRESS,  0, 32, D18F2x4C_dct1_TYPE, D18F2x4C_dct1_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2834_TYPE, GMMx2834_ADDRESS,  0, 32, D18F2x60_dct0_TYPE, D18F2x60_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2838_TYPE, GMMx2838_ADDRESS,  0, 32, D18F2x64_dct0_TYPE, D18F2x64_dct0_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx283C_TYPE, GMMx283C_ADDRESS,  0, 32, D18F2x60_dct1_TYPE, D18F2x60_dct1_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2840_TYPE, GMMx2840_ADDRESS,  0, 32, D18F2x64_dct1_TYPE, D18F2x64_dct1_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2844_TYPE, GMMx2844_ADDRESS,  0,  8, D18F2x80_dct0_TYPE, D18F2x80_dct0_ADDRESS,  0,  8),
  GNB_ENTRY_COPY (GMMx2844_TYPE, GMMx2844_ADDRESS, 16,  1, D18F2x94_dct0_TYPE, D18F2x94_dct0_ADDRESS, 22,  1),
  GNB_ENTRY_COPY (GMMx2844_TYPE, GMMx2844_ADDRESS, 19,  1, D18F2xA8_dct0_TYPE, D18F2xA8_dct0_ADDRESS, 20,  1),
  GNB_ENTRY_COPY (GMMx2848_TYPE, GMMx2848_ADDRESS,  0,  8, D18F2x80_dct1_TYPE, D18F2x80_dct1_ADDRESS,  0,  8),
  GNB_ENTRY_COPY (GMMx2848_TYPE, GMMx2848_ADDRESS, 16,  1, D18F2x94_dct1_TYPE, D18F2x94_dct1_ADDRESS, 22,  1),
  GNB_ENTRY_COPY (GMMx2848_TYPE, GMMx2848_ADDRESS, 19,  1, D18F2xA8_dct1_TYPE, D18F2xA8_dct1_ADDRESS, 20,  1),
  GNB_ENTRY_COPY (GMMx284C_TYPE, GMMx284C_ADDRESS,  0, 32, TYPE_D18F2 , 0x110 ,  0, 32),
  GNB_ENTRY_COPY (GMMx2850_TYPE, GMMx2850_ADDRESS,  0, 32, D18F2x114_TYPE, D18F2x114_ADDRESS,  0, 32),
  GNB_ENTRY_COPY (GMMx2854_TYPE, GMMx2854_ADDRESS,  0, 32, D18F1xF0_TYPE,  D18F1xF0_ADDRESS,   0, 32),
  //GNB_ENTRY_COPY (GMMx2858_TYPE, GMMx2858_ADDRESS,  0, 32, ????, ????,  0, 32),
  GNB_ENTRY_COPY (GMMx285C_TYPE, GMMx285C_ADDRESS,  0, 32, TYPE_D18F2 , 0x10c ,  0, 32),
  // 2.4 RENG init
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000000),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x001b0a05),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000001D),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00080500),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000027),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0001050c),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000002a),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x1000051e),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000000ff),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000000ff),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000002e),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00010536),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000031),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0001053e),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000034),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00010546),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000037),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x001a054e),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000053),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0001056f),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000056),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00010572),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000059),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00020575),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000005d),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00000800),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000005f),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x001a0801),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000007b),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0001082a),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000007e),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0014082d),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000094),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00040843),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000009a),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00170851),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000b3),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x001d086a),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000d2),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00000891),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000d4),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00000893),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000d6),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00020895),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000da),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x00020899),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000de),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0002089d),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000e2),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000208a1),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000000e6),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x006808cd),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000150),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0016094d),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000168),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000d096d),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000177),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x0009097f),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x00000182),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000a098a),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000018e),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000d0998),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x0000019d),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000409a7),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000001a3),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x003709cd),
  GNB_ENTRY_WR (TYPE_GMM , 0x28cc , 0x000001dc),
  GNB_ENTRY_WR (TYPE_GMM , 0x28d0 , 0x000f0a21),
  GNB_ENTRY_WR (GMMx28D4_TYPE, GMMx28D4_ADDRESS, 0x7b1ec000),
  GNB_ENTRY_WR (GMMx28D8_TYPE, GMMx28D8_ADDRESS, 0x200cf01d),
  // 2.5
  GNB_ENTRY_RMW (GMMx5490_TYPE, GMMx5490_ADDRESS, GMMx5490_FB_WRITE_EN_MASK | GMMx5490_FB_READ_EN_MASK, (1 << GMMx5490_FB_READ_EN_OFFSET) | (1 << GMMx5490_FB_WRITE_EN_OFFSET)),
  // 2.6 Perfromance tuning
  GNB_ENTRY_WR (TYPE_GMM , 0x27d0 , 0x10734847),
  GNB_ENTRY_WR (TYPE_GMM , 0x27c0 , 0x00032005),
  GNB_ENTRY_WR (TYPE_GMM , 0x27c4 , 0x00C12008),
  GNB_ENTRY_WR (TYPE_GMM , 0x27d4 , 0x00003d3c),
  GNB_ENTRY_WR (TYPE_GMM , 0x277c , 0x00000007),
  GNB_ENTRY_WR (TYPE_GMM , 0x2198 , 0x000221b1),
  GNB_ENTRY_WR (TYPE_GMM , 0x2750 , 0x00080A20),
  GNB_ENTRY_WR (TYPE_GMM , 0x201c , 0x66660006),
  GNB_ENTRY_WR (TYPE_GMM , 0x2020 , 0x70770007),
  GNB_ENTRY_WR (TYPE_GMM , 0x2018 , 0x66070050),
  GNB_ENTRY_WR (TYPE_GMM , 0x2014 , 0x77550000),
  GNB_ENTRY_WR (TYPE_GMM , 0x2794 , 0xfcfcfdfc),
  GNB_ENTRY_WR (TYPE_GMM , 0x2798 , 0xfcfcfdfc),
  GNB_ENTRY_WR (TYPE_GMM , 0x27a4 , 0x00ffffff),
  GNB_ENTRY_WR (TYPE_GMM , 0x27a8 , 0x00ffffff),
  GNB_ENTRY_WR (TYPE_GMM , 0x278c , 0x00000004),
  GNB_ENTRY_WR (TYPE_GMM , 0x2790 , 0x00000004),
  GNB_ENTRY_WR (TYPE_GMM , 0x2628 , 0x44111222),
  GNB_ENTRY_WR (TYPE_GMM , 0x25e0 , 0x00000004),
  GNB_ENTRY_WR (TYPE_GMM , 0x262c , 0x11222111),
  GNB_ENTRY_WR (TYPE_GMM , 0x25e4 , 0x00000002),
  //2.7 Miscellaneous programming
  GNB_ENTRY_WR (TYPE_GMM , 0x20b4 , 0x00000000),
  //2.8 Enabling garlic interface
  GNB_ENTRY_RMW (TYPE_GMM , 0x2878 , 0x1 , 1 << 0 ),
  // Limit number of garlic credits to 12
  GNB_ENTRY_WR (TYPE_GMM , 0x276c , 0x000000ff),
  GNB_ENTRY_WR (TYPE_GMM , 0x2898 , 0x01800360),
  GNB_ENTRY_RMW (TYPE_GMM , 0x289c , 0x8000 , 1 << 15 ),
  GNB_ENTRY_REV_RMW (0x0000000000000100ull , TYPE_GMM , 0x289c , 0x8000 , 0 << 15 ),
  GNB_ENTRY_RMW (GMMxC64_TYPE, GMMxC64_ADDRESS, GMMxC64_MCIFMEM_CACHE_MODE_DIS_MASK, 0 << GMMxC64_MCIFMEM_CACHE_MODE_DIS_OFFSET),
  GNB_ENTRY_REV_RMW (0x0000000000000100ull , GMMxC64_TYPE, GMMxC64_ADDRESS, GMMxC64_MCIFMEM_CACHE_MODE_DIS_MASK, 1 << GMMxC64_MCIFMEM_CACHE_MODE_DIS_OFFSET),
  //2.10 UVD and VCE latency
  //These settings are to improve UVD and VCE latency.
  //They need these settings to get good memory performance.
  GNB_ENTRY_WR (TYPE_GMM , 0x2750 , 0x00080200),
  GNB_ENTRY_WR (TYPE_GMM , 0x2190 , 0x001EA1A1),
  GNB_ENTRY_WR (TYPE_GMM , 0x2180 , 0x0000A1E1),
  GNB_ENTRY_WR (TYPE_GMM , 0x218c , 0x000FA1E1),
  GNB_ENTRY_WR (GMMx2188_TYPE, GMMx2188_ADDRESS, 0x0000A1E1),
  GNB_ENTRY_WR (TYPE_GMM , 0x21f0 , 0x0000A1F1),
  GNB_ENTRY_WR (TYPE_GMM , 0x21ec , 0x0000A1F1),
  GNB_ENTRY_WR (TYPE_GMM , 0x21f8 , 0x0000A1E1),
  GNB_ENTRY_WR (TYPE_GMM , 0x21f4 , 0x0000A1E1),
  GNB_ENTRY_RMW (TYPE_GMM , 0x690 , 0x20000000 , 1 << 29 ),
  GNB_ENTRY_RMW (TYPE_GMM , 0x21a8 , 0x4 , 0),
//MC Performance settings base on memory channel configuration, so, move settings to GfxGmcInitializeSequencerTN()
//  GNB_ENTRY_WR (TYPE_GMM , 0x2214 , 0x00000003),
  GNB_ENTRY_WR (TYPE_GMM , 0x2218 , 0x0000000C),
  GNB_ENTRY_WR (GMMx2888_TYPE, GMMx2888_ADDRESS, 0x000007DE),
  GNB_ENTRY_WR (GMMx25C8_TYPE, GMMx25C8_ADDRESS, 0x00403932),
  GNB_ENTRY_WR (GMMx2114_TYPE, GMMx2114_ADDRESS, 0x00000015),
  //2.11 Remove blackout
  GNB_ENTRY_WR (GMMx25C0_TYPE, GMMx25C0_ADDRESS, 0x00000000),
  GNB_ENTRY_WR (TYPE_GMM , 0x20ec , 0x000001DC),
  GNB_ENTRY_WR (TYPE_GMM , 0x20d4 , 0x00000016),
  GNB_ENTRY_WR (TYPE_GMM , 0x20ac , 0x00000000),
  GNB_ENTRY_RMW (TYPE_GMM , 0x2760 , 0x3 , 1 << 0 ),
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GfxGmcColockGatingEnableTN [] = {
  GNB_ENTRY_WR (TYPE_GMM , 0x20c0 , 0x00040c80),
  GNB_ENTRY_WR (TYPE_GMM , 0x2478 , 0x00040400),
  GNB_ENTRY_WR (TYPE_GMM , 0x20b8 , 0x00040400),
  GNB_ENTRY_WR (TYPE_GMM , 0x20bc , 0x00040400),
  GNB_ENTRY_WR (TYPE_GMM , 0x2648 , 0x00040400),
  GNB_ENTRY_WR (TYPE_GMM , 0x264c , 0x00040400),
  GNB_ENTRY_WR (TYPE_GMM , 0x2650 , 0x00040400),
  GNB_ENTRY_WR (TYPE_GMM , 0x15c0 , 0x00041401),
  //In addition to above registers it is necessary to reset override bits for VMC, MCB, and MCD blocks
  //Implement in GnbCgttOverrideTN
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GfxEnvInitTableTN [] = {
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    TYPE_GMM ,
    0xe60 ,
    0x0,
    (0x1 << 1 ) | (0x1 << 0 ) |
    (0x1 << 5 )  | (0x1 << 2 ) |
    (0x1 << 7 ) | (0x1 << 6 ) |
    (0x1 << 9 ) | (0x1 << 8 ) |
    (0x1 << 11 )  | (0x1 << 10 ) |
    (0x1 << 14 ) | (0x1 << 13 ) |
    (0x1 << 17 ) | (0x1 << 15 ) |
    (0x1 << 19 ) | (0x1 << 18 ) |
    (0x1 << 24 ) | (0x1 << 20 )
    ),
//---------------------------------------------------------------------------
// Configure GMC Power Island
  GNB_ENTRY_WR (
    D0F0xBC_xE0300004_TYPE,
    D0F0xBC_xE0300004_ADDRESS,
    (10 << 0 ) | (4 << 8 ) |
    (5 << 16 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300000_TYPE,
    D0F0xBC_xE0300000_ADDRESS,
    (0xff << D0F0xBC_xE0300000_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300000_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE0300000_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300004_TYPE,
    D0F0xBC_xE0300004_ADDRESS,
    (90 << 0 ) | (50 << 12 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300000_TYPE,
    D0F0xBC_xE0300000_ADDRESS,
    (0xff << D0F0xBC_xE0300000_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300000_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE0300000_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300004_TYPE,
    D0F0xBC_xE0300004_ADDRESS,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300000_TYPE,
    D0F0xBC_xE0300000_ADDRESS,
    (0xff << D0F0xBC_xE0300000_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300000_WriteOp_OFFSET) | (1 << D0F0xBC_xE0300000_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
// Shutdown GMC if integrated GFX disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x1
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300000_TYPE,
    D0F0xBC_xE0300000_ADDRESS,
    (0xff << D0F0xBC_xE0300000_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300000_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE0300000_P1Select_OFFSET) | (1 << D0F0xBC_xE0300000_P2Select_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300200_TYPE,
    D0F0xBC_xE0300200_ADDRESS,
    D0F0xBC_xE0300200_P1IsoN_MASK,
    0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x0
    ),
//---------------------------------------------------------------------------
// Configure UVD Power Island
  GNB_ENTRY_WR (
    D0F0xBC_xE0300040_TYPE,
    D0F0xBC_xE0300040_ADDRESS,
    (10 << 0 ) | (50 << 8 ) |
    (5 << 16 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030003C_TYPE,
    D0F0xBC_xE030003C_ADDRESS,
    (0xff << D0F0xBC_xE030003C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030003C_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE030003C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300040_TYPE,
    D0F0xBC_xE0300040_ADDRESS,
    (50 << 0 ) | (50 << 12 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030003C_TYPE,
    D0F0xBC_xE030003C_ADDRESS,
    (0xff << D0F0xBC_xE030003C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030003C_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE030003C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300040_TYPE,
    D0F0xBC_xE0300040_ADDRESS,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030003C_TYPE,
    D0F0xBC_xE030003C_ADDRESS,
    (0xff << D0F0xBC_xE030003C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030003C_WriteOp_OFFSET) | (1 << D0F0xBC_xE030003C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
// Shutdown UVD if integrated GFX disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x1
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE030003C_TYPE,
    D0F0xBC_xE030003C_ADDRESS,
    (0xff << D0F0xBC_xE030003C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030003C_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE030003C_P1Select_OFFSET) | (1 << D0F0xBC_xE030003C_P2Select_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300218_TYPE,
    D0F0xBC_xE0300218_ADDRESS,
    D0F0xBC_xE0300218_P1IsoN_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300324_TYPE,
    D0F0xBC_xE0300324_ADDRESS,
    D0F0xBC_xE0300324_UvdPgfsmClockEn_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x0
    ),
//---------------------------------------------------------------------------
// Configure VCE Power Island
  GNB_ENTRY_WR (
    D0F0xBC_xE0300028_TYPE,
    D0F0xBC_xE0300028_ADDRESS,
    (10 << 0 ) | (50 << 8 ) |
    (5 << 16 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300024_TYPE,
    D0F0xBC_xE0300024_ADDRESS,
    (0xff << D0F0xBC_xE0300024_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300024_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE0300024_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300028_TYPE,
    D0F0xBC_xE0300028_ADDRESS,
    (50 << 0 ) | (50 << 12 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300024_TYPE,
    D0F0xBC_xE0300024_ADDRESS,
    (0xff << D0F0xBC_xE0300024_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300024_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE0300024_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300028_TYPE,
    D0F0xBC_xE0300028_ADDRESS,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300024_TYPE,
    D0F0xBC_xE0300024_ADDRESS,
    (0xff << D0F0xBC_xE0300024_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300024_WriteOp_OFFSET) | (1 << D0F0xBC_xE0300024_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
// Shutdown VCE if integrated GFX disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x1
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300024_TYPE,
    D0F0xBC_xE0300024_ADDRESS,
    (0xff << D0F0xBC_xE0300024_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300024_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE0300024_P1Select_OFFSET) | (1 << D0F0xBC_xE0300024_P2Select_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE030020C_TYPE,
    D0F0xBC_xE030020C_ADDRESS,
    D0F0xBC_xE030020C_P1IsoN_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300324_TYPE,
    D0F0xBC_xE0300324_ADDRESS,
    D0F0xBC_xE0300324_VcePgfsmClockEn_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x0
    ),

//---------------------------------------------------------------------------
// Configure DCE Power Island
  // Step 1: Take control over DC2 PGFSM. By default display sends power up/down commands.
  GNB_ENTRY_WR (
    D0F0xBC_xE03002DC_TYPE,
    D0F0xBC_xE03002DC_ADDRESS,
    (1 << D0F0xBC_xE03002DC_DC2_PGFSM_CONTROL_OFFSET)
    ),
  //Step 2: Read CC_RCU_FUSES register
  //If Internal GPU is fused off go to Step 3, ELSE Go to Step 4.

  //Step 3: Enable PGFSM commands during reset
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x1
    ),
  //Step 4:
  GNB_ENTRY_WR (
    D0F0xBC_xE0300034_TYPE,
    D0F0xBC_xE0300034_ADDRESS,
    (10 << 0 ) | (50 << 8 ) |
    (5 << 16 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300030_TYPE,
    D0F0xBC_xE0300030_ADDRESS,
    (0xff << D0F0xBC_xE0300030_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300030_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE0300030_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300034_TYPE,
    D0F0xBC_xE0300034_ADDRESS,
    (50 << 0 ) | (50 << 12 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300030_TYPE,
    D0F0xBC_xE0300030_ADDRESS,
    (0xff << D0F0xBC_xE0300030_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300030_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE0300030_WriteOp_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300034_TYPE,
    D0F0xBC_xE0300034_ADDRESS,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300030_TYPE,
    D0F0xBC_xE0300030_ADDRESS,
    (0xff << D0F0xBC_xE0300030_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300030_WriteOp_OFFSET) | (1 << D0F0xBC_xE0300030_WriteOp_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  //Step 5: IF (cc_rcu_fuses.f.gpu_dis == 0x1) Skip Step6 ELSE Go to Step 6
  //Step 6: Disable PGFSM commands during reset. Move to after shutdown DCE.
  //Step 7: Release control over DC2 PGFSM. Move to after shutdown DCE.

// Shutdown DCE if integrated GFX disabled
  //Step 1: Take control over DC2 PGFSM. By default display sends power up down commands.
  //Step 2: Read CC_RCU_FUSES register
  //Step 3: Enable PGFSM commands during reset
  //Step 4: Make sure SCLK frequency is below 400Mhz
  //Step 5: Enable PGFSM clock
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300324_TYPE,
    D0F0xBC_xE0300324_ADDRESS,
    D0F0xBC_xE0300324_Dc2PgfsmClockEn_MASK,
    (1 << D0F0xBC_xE0300324_Dc2PgfsmClockEn_OFFSET)
    ),
  //Step 6
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300030_TYPE,
    D0F0xBC_xE0300030_ADDRESS,
    (0xff << D0F0xBC_xE0300030_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300030_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE0300030_P1Select_OFFSET) | (1 << D0F0xBC_xE0300030_P2Select_OFFSET)
    ),
  //Step 7
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300210_TYPE,
    D0F0xBC_xE0300210_ADDRESS,
    D0F0xBC_xE0300210_P1IsoN_MASK,
    (0 << D0F0xBC_xE0300210_P1IsoN_OFFSET)
    ),
  //Step 8: Restore previous SCLK divider
  //Step 9: Wait PSO daughter to be asserted
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    TYPE_D0F0xBC ,
    0xe0300210 ,
    0x2000 ,
    (1 << 13 )
    ),
  //Step 10: Turn off PGFSM clock
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300324_TYPE,
    D0F0xBC_xE0300324_ADDRESS,
    D0F0xBC_xE0300324_Dc2PgfsmClockEn_MASK,
    (0 << D0F0xBC_xE0300324_Dc2PgfsmClockEn_OFFSET)
    ),
  //Step 11: Disable PGFSM commands during reset. Same final 2 step as DCE power island
  GNB_ENTRY_RMW (
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03002DC_TYPE,
    D0F0xBC_xE03002DC_ADDRESS,
    (0 << D0F0xBC_xE03002DC_DC2_PGFSM_CONTROL_OFFSET)
    ),

//---------------------------------------------------------------------------
// Configure GFX Power Island

  //Step 3
  GNB_ENTRY_WR (
    D0F0xBC_xE0300058_TYPE,
    D0F0xBC_xE0300058_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300054_TYPE,
    D0F0xBC_xE0300054_ADDRESS,
    (0xff << D0F0xBC_xE0300054_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300054_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE0300054_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300058_TYPE,
    D0F0xBC_xE0300058_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300054_TYPE,
    D0F0xBC_xE0300054_ADDRESS,
    (0xff << D0F0xBC_xE0300054_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300054_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE0300054_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300058_TYPE,
    D0F0xBC_xE0300058_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300054_TYPE,
    D0F0xBC_xE0300054_ADDRESS,
    (0xff << D0F0xBC_xE0300054_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300054_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE0300054_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 4
  GNB_ENTRY_WR (
    D0F0xBC_xE0300074_TYPE,
    D0F0xBC_xE0300074_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300070_TYPE,
    D0F0xBC_xE0300070_ADDRESS,
    (0xff << D0F0xBC_xE0300070_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300070_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE0300070_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300074_TYPE,
    D0F0xBC_xE0300074_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300070_TYPE,
    D0F0xBC_xE0300070_ADDRESS,
    (0xff << D0F0xBC_xE0300070_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300070_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE0300070_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300074_TYPE,
    D0F0xBC_xE0300074_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300070_TYPE,
    D0F0xBC_xE0300070_ADDRESS,
    (0xff << D0F0xBC_xE0300070_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300070_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE0300070_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 5
  GNB_ENTRY_WR (
    D0F0xBC_xE0300090_TYPE,
    D0F0xBC_xE0300090_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030008C_TYPE,
    D0F0xBC_xE030008C_ADDRESS,
    (0xff << D0F0xBC_xE030008C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030008C_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE030008C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300090_TYPE,
    D0F0xBC_xE0300090_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030008C_TYPE,
    D0F0xBC_xE030008C_ADDRESS,
    (0xff << D0F0xBC_xE030008C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030008C_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE030008C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300090_TYPE,
    D0F0xBC_xE0300090_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030008C_TYPE,
    D0F0xBC_xE030008C_ADDRESS,
    (0xff << D0F0xBC_xE030008C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030008C_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE030008C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 6
  GNB_ENTRY_WR (
    D0F0xBC_xE03000AC_TYPE,
    D0F0xBC_xE03000AC_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000A8_TYPE,
    D0F0xBC_xE03000A8_ADDRESS,
    (0xff << D0F0xBC_xE03000A8_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000A8_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE03000A8_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000AC_TYPE,
    D0F0xBC_xE03000AC_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000A8_TYPE,
    D0F0xBC_xE03000A8_ADDRESS,
    (0xff << D0F0xBC_xE03000A8_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000A8_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE03000A8_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000AC_TYPE,
    D0F0xBC_xE03000AC_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000A8_TYPE,
    D0F0xBC_xE03000A8_ADDRESS,
    (0xff << D0F0xBC_xE03000A8_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000A8_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE03000A8_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 7
  GNB_ENTRY_WR (
    D0F0xBC_xE03000C8_TYPE,
    D0F0xBC_xE03000C8_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000C4_TYPE,
    D0F0xBC_xE03000C4_ADDRESS,
    (0xff << D0F0xBC_xE03000C4_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000C4_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE03000C4_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000C8_TYPE,
    D0F0xBC_xE03000C8_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000C4_TYPE,
    D0F0xBC_xE03000C4_ADDRESS,
    (0xff << D0F0xBC_xE03000C4_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000C4_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE03000C4_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000C8_TYPE,
    D0F0xBC_xE03000C8_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000C4_TYPE,
    D0F0xBC_xE03000C4_ADDRESS,
    (0xff << D0F0xBC_xE03000C4_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000C4_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE03000C4_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 8
  GNB_ENTRY_WR (
    D0F0xBC_xE03000E4_TYPE,
    D0F0xBC_xE03000E4_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000E0_TYPE,
    D0F0xBC_xE03000E0_ADDRESS,
    (0xff << D0F0xBC_xE03000E0_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000E0_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE03000E0_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000E4_TYPE,
    D0F0xBC_xE03000E4_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000E0_TYPE,
    D0F0xBC_xE03000E0_ADDRESS,
    (0xff << D0F0xBC_xE03000E0_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000E0_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE03000E0_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000E4_TYPE,
    D0F0xBC_xE03000E4_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000E0_TYPE,
    D0F0xBC_xE03000E0_ADDRESS,
    (0xff << D0F0xBC_xE03000E0_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000E0_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE03000E0_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 9
  GNB_ENTRY_WR (
    D0F0xBC_xE0300100_TYPE,
    D0F0xBC_xE0300100_ADDRESS,
    (5 << 16 ) | (4 << 8 ) |
    (10 << 0 ) //reg0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000FC_TYPE,
    D0F0xBC_xE03000FC_ADDRESS,
    (0xff << D0F0xBC_xE03000FC_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000FC_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE03000FC_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300100_TYPE,
    D0F0xBC_xE0300100_ADDRESS,
    (50 << 0 ) | (50 << 12 ) //reg1
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000FC_TYPE,
    D0F0xBC_xE03000FC_ADDRESS,
    (0xff << D0F0xBC_xE03000FC_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000FC_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE03000FC_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300100_TYPE,
    D0F0xBC_xE0300100_ADDRESS,
    0 // control
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE03000FC_TYPE,
    D0F0xBC_xE03000FC_ADDRESS,
    (0xff << D0F0xBC_xE03000FC_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000FC_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE03000FC_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  // Step 10
    GNB_ENTRY_RMW (
    TYPE_D0F0xBC ,
    0xe0300328 ,
    0x1  | 0x2  |
    0x4  | 0x8  |
    0x10  | 0x20  |
    0x40 ,
    0x0
    ),
  // Step 12
  GNB_ENTRY_RMW (
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x0
    ),
// Shutdown Gfx if integrated GFX disabled
  // Step 2
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x1
    ),
  // Step 3: Save current SCLK. Make sure SCLK frequency is below 400Mhz
  // Step 5
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    TYPE_D0F0xBC ,
    0xe0300328 ,
    0x1  | 0x2  |
    0x4  | 0x8  |
    0x10  | 0x20  |
    0x40 ,
    (1 << 0 ) | (1 << 1 ) |
    (1 << 2 ) | (1 << 3 ) |
    (1 << 4 ) | (1 << 5 ) |
    (1 << 6 )
    ),
  // Step 6
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300054_TYPE,
    D0F0xBC_xE0300054_ADDRESS,
    (0xff << D0F0xBC_xE0300054_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300054_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE0300054_P1Select_OFFSET) | (1 << D0F0xBC_xE0300054_P2Select_OFFSET)
    ),
  // Step 7
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0300070_TYPE,
    D0F0xBC_xE0300070_ADDRESS,
    (0xff << D0F0xBC_xE0300070_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300070_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE0300070_P1Select_OFFSET) | (1 << D0F0xBC_xE0300070_P2Select_OFFSET)
    ),
  // Step 8
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE030008C_TYPE,
    D0F0xBC_xE030008C_ADDRESS,
    (0xff << D0F0xBC_xE030008C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030008C_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE030008C_P1Select_OFFSET) | (1 << D0F0xBC_xE030008C_P2Select_OFFSET)
    ),
  // Step 9
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE03000A8_TYPE,
    D0F0xBC_xE03000A8_ADDRESS,
    (0xff << D0F0xBC_xE03000A8_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000A8_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE03000A8_P1Select_OFFSET) | (1 << D0F0xBC_xE03000A8_P2Select_OFFSET)
    ),
  // Step 10
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE03000C4_TYPE,
    D0F0xBC_xE03000C4_ADDRESS,
    (0xff << D0F0xBC_xE03000C4_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000C4_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE03000C4_P1Select_OFFSET) | (1 << D0F0xBC_xE03000C4_P2Select_OFFSET)
    ),
  // Step 11
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE03000E0_TYPE,
    D0F0xBC_xE03000E0_ADDRESS,
    (0xff << D0F0xBC_xE03000E0_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000E0_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE03000E0_P1Select_OFFSET) | (1 << D0F0xBC_xE03000E0_P2Select_OFFSET)
    ),
  // Step 12
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE03000FC_TYPE,
    D0F0xBC_xE03000FC_ADDRESS,
    (0xff << D0F0xBC_xE03000FC_FsmAddr_OFFSET) | (1 << D0F0xBC_xE03000FC_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE03000FC_P1Select_OFFSET) | (1 << D0F0xBC_xE03000FC_P2Select_OFFSET)
    ),
  // Step 13
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE03002F4_TYPE,
    D0F0xBC_xE03002F4_ADDRESS,
    0xFFFFFFFF,
    0
    ),
  // Step 14
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE03002F0_TYPE,
    D0F0xBC_xE03002F0_ADDRESS,
    0xFFFFFFFF,
    0
    ),
  // Step 15: Restore SCLK that is saved in step 4
  // Step 16
  GNB_ENTRY_FULL_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    (AMD_F15_TN_ALL & 0x0000000000000100ull) /* AMD_F15_TN_GT_A0 */,
    D0F0xBC_xE03002FC_TYPE,
    D0F0xBC_xE03002FC_ADDRESS,
    0xFFFFFFFF,
    0x3FFFFFFF
    ),
  // Step 17
  GNB_ENTRY_FULL_POLL (
    TABLE_PROPERTY_IGFX_DISABLED,
    (AMD_F15_TN_ALL & 0x0000000000000100ull) /* AMD_F15_TN_GT_A0 */,
    D0F0xBC_xE03002E4_TYPE,
    D0F0xBC_xE03002E4_ADDRESS,
    0xFFFFFFFF,
    0x3FFFF
    ),
  // Step 18
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    TYPE_D0F0xBC ,
    0xe0300328 ,
    0x1  | 0x2  |
    0x4  | 0x8  |
    0x10  | 0x20  |
    0x40 ,
    0
    ),
  // Step 19
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003024_TYPE,
    D0F0xBC_xE0003024_ADDRESS,
    0x1,
    0x0
    ),
//---------------------------------------------------------------------------
// Isolate DC, SYS and CP tile when Internal Graphics is disabled
  // Step 2: Reduce SCLK frequency to 100Mhz. Save current SCLK divider.
  // Step 3
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F0xBC_xE0003034_TYPE,
    D0F0xBC_xE0003034_ADDRESS,
    D0F0xBC_xE0003034_SysIso_MASK | D0F0xBC_xE0003034_CpIso_MASK |
    D0F0xBC_xE0003034_Dc0Iso_MASK | D0F0xBC_xE0003034_Dc1Iso_MASK |
    D0F0xBC_xE0003034_DciIso_MASK | D0F0xBC_xE0003034_DcipgIso_MASK,
    (1 << D0F0xBC_xE0003034_SysIso_OFFSET) | (1 << D0F0xBC_xE0003034_CpIso_OFFSET) |
    (1 << D0F0xBC_xE0003034_Dc0Iso_OFFSET) | (1 << D0F0xBC_xE0003034_Dc1Iso_OFFSET) |
    (1 << D0F0xBC_xE0003034_DciIso_OFFSET) | (1 << D0F0xBC_xE0003034_DcipgIso_OFFSET)
    ),
  //Step 4: Restore pervious SCLK frequency

//---------------------------------------------------------------------------
// For IOMMU add logic of GfxDis
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    D0F2xF4_x57_TYPE,
    D0F2xF4_x57_ADDRESS,
    D0F2xF4_x57_L1ImuIntGfxDis_MASK,
    (0x1 << D0F2xF4_x57_L1ImuIntGfxDis_OFFSET)
    ),

  GNB_ENTRY_TERMINATE
};
