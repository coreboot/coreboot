/* $NoKeywords:$ */
/**
 * @file
 *
 *  GMC init services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 50763 $   @e \$Date: 2011-04-14 06:25:56 +0800 (Thu, 14 Apr 2011) $
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
//#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbPcie.h"
#include  "GnbGfxFamServices.h"
#include  "GnbCommonLib.h"
#include  "GfxLib.h"
#include  "GfxFamilyServices.h"
#include  "GfxRegisterAcc.h"
#include  "OptionGnb.h"
#include  "GnbRegistersLN.h"
#include  "GfxGmcInit.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_GFX_GFXGMCINIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/// DCT channel information
typedef struct {
  D18F2x094_STRUCT      D18F2x094;          ///< Register 0x94
  D18F2x084_STRUCT      D18F2x084;          ///< Register 0x84
  D18F2x08C_STRUCT      D18F2x08C;          ///< Register 0x8C
  D18F2x0F4_x40_STRUCT  D18F2x0F4_x40;      ///< Register 0x40
  D18F2x0F4_x41_STRUCT  D18F2x0F4_x41;      ///< Register 0x41
} DCT_CHANNEL_INFO;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */



/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
GfxGmcSetMemoryAddressTranslation (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcDisableClockGating (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcInitializeRegisterEngine (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcDctMemoryChannelInfo (
  IN       UINT8                 Channel,
     OUT   DCT_CHANNEL_INFO      *DctChannelInfo,
  IN       GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcInitializeSequencerModel (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcInitializeFbLocation (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcSecureGarlicAccess (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcPerformanceTuning (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcMiscInit (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcLockCriticalRegisters (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcRemoveBlackout (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcEnableClockGating (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcUmaSteering (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcInitializeC6Aperture (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcInitializePowerGating (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

//Family 12 or Family 14 specific tables

extern TABLE_INDIRECT_PTR GmcDisableClockGatingPtr;
extern TABLE_INDIRECT_PTR GmcEnableClockGatingPtr;
extern TABLE_INDIRECT_PTR GmcPerformanceTuningTablePtr;
extern TABLE_INDIRECT_PTR GmcMiscInitTablePtr;
extern TABLE_INDIRECT_PTR GmcRemoveBlackoutTablePtr;
extern TABLE_INDIRECT_PTR GmcRegisterEngineInitTablePtr;
extern TABLE_INDIRECT_PTR CnbToGncRegisterCopyTablePtr;

extern UINT8               NumberOfChannels;
/*----------------------------------------------------------------------------------------*/
/**
 * Init GMC memory address translation
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */
VOID
GfxGmcSetMemoryAddressTranslation (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINTN Index;
  REGISTER_COPY_ENTRY *CnbToGncRegisterCopyTable;
  CnbToGncRegisterCopyTable = CnbToGncRegisterCopyTablePtr.TablePtr;
  for (Index = 0; Index < CnbToGncRegisterCopyTablePtr.TableLength; Index++) {
    UINT32  Value;
    GnbLibPciRead (
      CnbToGncRegisterCopyTable[Index].CpuReg,
      AccessWidth32,
      &Value,
      GnbLibGetHeader (Gfx)
      );
    Value = (Value >> CnbToGncRegisterCopyTable[Index].CpuOffset) & ((1 <<  CnbToGncRegisterCopyTable[Index].CpuWidth) - 1);
    GmmRegisterWriteField (
      CnbToGncRegisterCopyTable[Index].GmmReg,
      CnbToGncRegisterCopyTable[Index].GmmOffset,
      CnbToGncRegisterCopyTable[Index].GmmWidth,
      Value,
      TRUE,
      Gfx
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disable CLock Gating
 *
 *
 *
 * @param[in] Gfx         Graphics configuration
 */

VOID
GfxGmcDisableClockGating (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GmmRegisterTableWrite (
    GmcDisableClockGatingPtr.TablePtr,
    GmcDisableClockGatingPtr.TableLength,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize Register Engine
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcInitializeRegisterEngine (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{

  GmmRegisterTableWrite (
    GmcRegisterEngineInitTablePtr.TablePtr,
    GmcRegisterEngineInitTablePtr.TableLength,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get DCT channel info
 *
 *
 * @param[in]  Channel         DCT channel number
 * @param[out] DctChannelInfo  Various DCT channel info
 * @param[in]  Gfx             Pointer to global GFX configuration
 */

VOID
GfxGmcDctMemoryChannelInfo (
  IN       UINT8                 Channel,
     OUT   DCT_CHANNEL_INFO      *DctChannelInfo,
  IN       GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GnbLibCpuPciIndirectRead (
    MAKE_SBDFO (0, 0, 0x18, 2, (Channel == 0) ? D18F2x0F0_ADDRESS : D18F2x1F0_ADDRESS),
    D18F2x0F4_x40_ADDRESS,
    &DctChannelInfo->D18F2x0F4_x40.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibCpuPciIndirectRead (
    MAKE_SBDFO (0, 0, 0x18, 2, (Channel == 0) ? D18F2x0F0_ADDRESS : D18F2x1F0_ADDRESS),
    D18F2x0F4_x41_ADDRESS,
    &DctChannelInfo->D18F2x0F4_x41.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciRead (
    MAKE_SBDFO (0, 0, 0x18, 2, (Channel == 0) ? D18F2x084_ADDRESS : D18F2x184_ADDRESS),
    AccessWidth32,
    &DctChannelInfo->D18F2x084.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciRead (
    MAKE_SBDFO (0, 0, 0x18, 2, (Channel == 0) ? D18F2x094_ADDRESS : D18F2x194_ADDRESS),
    AccessWidth32,
    &DctChannelInfo->D18F2x094.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciRead (
    MAKE_SBDFO (0, 0, 0x18, 2, (Channel == 0) ? D18F2x08C_ADDRESS : D18F2x18C_ADDRESS),
    AccessWidth32,
    &DctChannelInfo->D18F2x08C.Value,
    GnbLibGetHeader (Gfx)
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize Sequencer Model
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcInitializeSequencerModel (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx277C_STRUCT       GMMx277C;
  GMMx2780_STRUCT       GMMx2780;
  DCT_CHANNEL_INFO      DctChannel[2];
  UINT8                 ActiveChannel;

  GfxGmcDctMemoryChannelInfo (0, &DctChannel[0], Gfx);
  if (NumberOfChannels == 2) {
    GfxGmcDctMemoryChannelInfo (1, &DctChannel[1], Gfx);
  }

  // Find the Active Channels.  For a single channel system, Active channel is 0;
  if (NumberOfChannels == 1) {
    ActiveChannel = 0;
  } else {
    //For two channel system, Active channel could be either 0 or 1 or both (2)
    if (DctChannel[0].D18F2x094.Field.DisDramInterface == 0 &&
      DctChannel[1].D18F2x094.Field.DisDramInterface == 0) {
      ActiveChannel = 2;
    } else {
      ActiveChannel = (DctChannel[0].D18F2x094.Field.DisDramInterface == 0) ? 0 : 1;
    }
  }

  if (ActiveChannel == 2) {
    // Both controllers enabled
    GMMx277C.Field.ActRd = MIN (DctChannel[0].D18F2x0F4_x40.Field.Trcd, DctChannel[1].D18F2x0F4_x40.Field.Trcd) + 5;
    GMMx277C.Field.RasMActRd = MIN ((DctChannel[0].D18F2x0F4_x40.Field.Trc + 11 - (DctChannel[0].D18F2x0F4_x40.Field.Trcd + 5)),
                                    (DctChannel[1].D18F2x0F4_x40.Field.Trc + 11 - (DctChannel[1].D18F2x0F4_x40.Field.Trcd + 5)));
    GMMx2780.Field.Ras2Ras = MIN (DctChannel[0].D18F2x0F4_x40.Field.Trc, DctChannel[1].D18F2x0F4_x40.Field.Trc) + 11 - 1;
    GMMx2780.Field.Rp = MIN (DctChannel[0].D18F2x0F4_x40.Field.Trp, DctChannel[1].D18F2x0F4_x40.Field.Trp) + 5 - 1;
    GMMx2780.Field.WrPlusRp = MIN (
                                ((DctChannel[0].D18F2x084.Field.Twr == 0) ? 16 :
                                ((DctChannel[0].D18F2x084.Field.Twr < 4) ? (DctChannel[0].D18F2x084.Field.Twr + 4) :
                                (DctChannel[0].D18F2x084.Field.Twr * 2)) + DctChannel[0].D18F2x0F4_x40.Field.Trp + 5),
                                ((DctChannel[1].D18F2x084.Field.Twr == 0) ? 16 :
                                ((DctChannel[1].D18F2x084.Field.Twr < 4) ? (DctChannel[1].D18F2x084.Field.Twr + 4) :
                                (DctChannel[1].D18F2x084.Field.Twr * 2)) + DctChannel[1].D18F2x0F4_x40.Field.Trp + 5)
                                ) - 1;
    GMMx2780.Field.BusTurn = (MIN (
                                DctChannel[0].D18F2x084.Field.Tcwl + 5 +
                                DctChannel[0].D18F2x0F4_x41.Field.Twtr + 4 +
                                DctChannel[0].D18F2x08C.Field.TrwtTO + 2 ,
                                DctChannel[1].D18F2x084.Field.Tcwl + 5 +
                                DctChannel[1].D18F2x0F4_x41.Field.Twtr + 4 +
                                DctChannel[1].D18F2x08C.Field.TrwtTO + 2
                                ) + 4) / 2;
  } else {
    // Only one channel is active.
    GMMx277C.Field.ActRd = DctChannel[ActiveChannel].D18F2x0F4_x40.Field.Trcd + 5;
    GMMx277C.Field.RasMActRd = DctChannel[ActiveChannel].D18F2x0F4_x40.Field.Trc + 11 -
                              (DctChannel[ActiveChannel].D18F2x0F4_x40.Field.Trcd + 5);
    GMMx2780.Field.Ras2Ras = DctChannel[ActiveChannel].D18F2x0F4_x40.Field.Trc + 11 - 1;
    GMMx2780.Field.Rp = DctChannel[ActiveChannel].D18F2x0F4_x40.Field.Trp + 5 - 1;
    GMMx2780.Field.WrPlusRp = ((DctChannel[ActiveChannel].D18F2x084.Field.Twr == 0) ? 16 :
                              ((DctChannel[ActiveChannel].D18F2x084.Field.Twr < 4) ? (DctChannel[ActiveChannel].D18F2x084.Field.Twr + 4) :
                               (DctChannel[ActiveChannel].D18F2x084.Field.Twr * 2)) +
                               DctChannel[ActiveChannel].D18F2x0F4_x40.Field.Trp + 5) - 1;
    GMMx2780.Field.BusTurn =  ((DctChannel[ActiveChannel].D18F2x084.Field.Tcwl + 5 +
                                DctChannel[ActiveChannel].D18F2x0F4_x41.Field.Twtr + 4 +
                                DctChannel[ActiveChannel].D18F2x08C.Field.TrwtTO + 2) + 4) / 2;
  }
  GMMx277C.Field.ActWr = GMMx277C.Field.ActRd;
  GMMx277C.Field.RasMActWr = GMMx277C.Field.RasMActRd;

  GmmRegisterWrite (
    GMMx277C_ADDRESS,
    GMMx277C.Value,
    TRUE,
    Gfx
    );
  GmmRegisterWrite (
    GMMx28D8_ADDRESS,
    GMMx277C.Value,
    TRUE,
    Gfx
    );
  GmmRegisterWrite (
    GMMx2780_ADDRESS,
    GMMx2780.Value,
    TRUE,
    Gfx
    );
    GmmRegisterWrite (
    GMMx28DC_ADDRESS,
    GMMx2780.Value,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize Frame Buffer Location
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcInitializeFbLocation (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
 //Logical FB location
  GMMx2024_STRUCT   GMMx2024;
  GMMx2898_STRUCT   GMMx2898;
  GMMx2C04_STRUCT   GMMx2C04;
  GMMx5428_STRUCT   GMMx5428;
  UINT64            FBBase;
  UINT64            FBTop;
  FBBase = 0x0F00000000ull;
  FBTop = FBBase + Gfx->UmaInfo.UmaSize - 1;
  GMMx2024.Value = 0;
  GMMx2898.Value = 0;
  GMMx2C04.Value = 0;
  GMMx5428.Value = 0;
  GMMx2024.Field.Base = (UINT16) (FBBase >> 24);
  GMMx2024.Field.Top = (UINT16) (FBTop >> 24);
  GMMx2898.Field.Offset = (UINT32) (Gfx->UmaInfo.UmaBase >> 20);
  GMMx2898.Field.Top = (UINT32) ((FBTop >> 20) & 0xf);
  GMMx2898.Field.Base = (UINT32) ((FBBase >> 20) & 0xf);
  GMMx2C04.Field.NonsurfBase = (UINT32) (FBBase >> 8);
  GMMx5428.Field.ConfigMemsize = Gfx->UmaInfo.UmaSize;

  GmmRegisterWrite (
    GMMx2024_ADDRESS,
    GMMx2024.Value,
    TRUE,
    Gfx
    );
  GmmRegisterWrite (
    GMMx2898_ADDRESS,
    GMMx2898.Value,
    TRUE,
    Gfx
    );
  GmmRegisterWrite (
    GMMx2C04_ADDRESS,
    GMMx2C04.Value,
    TRUE,
    Gfx
    );
  GmmRegisterWrite (
    GMMx5428_ADDRESS,
    GMMx5428.Value,
    TRUE,
    Gfx
    );
  GmmRegisterWriteField (
    GMMx5490_ADDRESS,
    GMMx5490_FbReadEn_OFFSET,
    GMMx5490_FbReadEn_WIDTH,
    1,
    TRUE,
    Gfx
    );
  GmmRegisterWriteField (
    GMMx5490_ADDRESS,
    GMMx5490_FbWriteEn_OFFSET,
    GMMx5490_FbWriteEn_WIDTH,
    1,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Secure Garlic Access
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcSecureGarlicAccess (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx286C_STRUCT   GMMx286C;
  GMMx287C_STRUCT   GMMx287C;
  GMMx2894_STRUCT   GMMx2894;
  UINT32            Value;
  GMMx286C.Value = (UINT32) (Gfx->UmaInfo.UmaBase >> 20);
  GmmRegisterWrite (GMMx286C_ADDRESS, GMMx286C.Value, TRUE, Gfx);
  GMMx287C.Value = (UINT32) (((Gfx->UmaInfo.UmaBase + Gfx->UmaInfo.UmaSize) >> 20) - 1);
  GmmRegisterWrite (GMMx287C_ADDRESS, GMMx287C.Value, TRUE, Gfx);
  // Areag FB - 20K reserved by VBIOS for SBIOS to use
  GMMx2894.Value = (UINT32) ((Gfx->UmaInfo.UmaBase + Gfx->UmaInfo.UmaSize - 20 * 1024) >> 12);
  GmmRegisterWrite (GMMx2894_ADDRESS, GMMx2894.Value, TRUE, Gfx);
  Value = 0xfffff;
  GmmRegisterWrite (GMMx2870_ADDRESS, Value, TRUE, Gfx);
  GmmRegisterWrite (GMMx2874_ADDRESS, Value, TRUE, Gfx);
  GmmRegisterWrite (GMMx2878_ADDRESS, Value, TRUE, Gfx);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Performance setting
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcPerformanceTuning (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GmmRegisterTableWrite (
    GmcPerformanceTuningTablePtr.TablePtr,
    GmcPerformanceTuningTablePtr.TableLength,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Misc. Initialization
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcMiscInit (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GMMx2114_STRUCT  GMMx2114;

  GMMx2114.Value = GmmRegisterRead (GMMx2114_ADDRESS, Gfx);
  GMMx2114.Field.Stor1Pri = 0xC;
  GmmRegisterWrite (GMMx2114_ADDRESS, GMMx2114.Value, TRUE, Gfx);

  GmmRegisterTableWrite (
    GmcMiscInitTablePtr.TablePtr,
    GmcMiscInitTablePtr.TableLength,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Lock critical registers
 *
 *
 *
 * @param[in] Gfx         Graphics configuration
 */

VOID
GfxGmcLockCriticalRegisters (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GmmRegisterWriteField (
    GMMx2B98_ADDRESS,
    GMMx2B98_CriticalRegsLock_OFFSET,
    GMMx2B98_CriticalRegsLock_WIDTH,
    1,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Remove blackout
 *
 *
 *
 * @param[in] Gfx         Pointer to global GFX configuration
 */

VOID
GfxGmcRemoveBlackout (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GmmRegisterTableWrite (
    GmcRemoveBlackoutTablePtr.TablePtr,
    GmcRemoveBlackoutTablePtr.TableLength,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable clock Gating
 *
 *
 *
 * @param[in] Gfx         Graphics configuration
 */

VOID
GfxGmcEnableClockGating (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  GmmRegisterTableWrite (
    GmcEnableClockGatingPtr.TablePtr,
    GmcEnableClockGatingPtr.TableLength,
    TRUE,
    Gfx
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * UMA steering
 *
 *
 *
 * @param[in] Gfx         Graphics configuration
 */

VOID
GfxGmcUmaSteering (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize C6 aperture
 *
 *
 *
 * @param[in] Gfx         Graphics configuration
 */

VOID
GfxGmcInitializeC6Aperture (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D18F4x12C_STRUCT    D18F4x12C;
  GMMx288C_STRUCT     GMMx288C;
  GMMx2890_STRUCT     GMMx2890;

  GnbLibPciRead (
    MAKE_SBDFO (0, 0, 0x18, 4, D18F4x12C_ADDRESS),
    AccessWidth32,
    &D18F4x12C.Value,
    GnbLibGetHeader (Gfx)
    );
  GMMx288C.Value = D18F4x12C.Field.C6Base_39_24_ << 4;
  // Modify the values only if C6 Base is set
  if (GMMx288C.Value != 0) {
    GMMx2890.Value = (GMMx288C.Value + 16) - 1;
    GmmRegisterWrite (
      GMMx288C_ADDRESS,
      GMMx288C.Value,
      TRUE,
      Gfx
      );
    GmmRegisterWrite (
      GMMx2890_ADDRESS,
      GMMx2890.Value,
      TRUE,
      Gfx
      );
  }
}
/*----------------------------------------------------------------------------------------*/
/**
 * Initialize Power Gating
 *
 *
 *
 * @param[in] Gfx         Graphics configuration
 */

VOID
GfxGmcInitializePowerGating (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  switch (Gfx->GmcPowerGating) {
  case  GmcPowerGatingDisabled:
    break;
  case  GmcPowerGatingStutterOnly:
    GmmRegisterWriteField (
      GMMx2B98_ADDRESS,
      GMMx2B98_StctrlStutterEn_OFFSET,
      GMMx2B98_StctrlStutterEn_WIDTH,
      1,
      TRUE,
      Gfx
      );
    break;
  case GmcPowerGatingWidthStutter:
    GmmRegisterWriteField (
      GMMx2B94_ADDRESS,
      GMMx2B94_RengExecuteOnPwrUp_OFFSET,
      GMMx2B94_RengExecuteOnPwrUp_WIDTH,
      1,
      TRUE,
      Gfx
      );
    GmmRegisterWriteField (
      GMMx2B98_ADDRESS,
      GMMx2B98_RengExecuteOnRegUpdate_OFFSET,
      GMMx2B98_RengExecuteOnRegUpdate_WIDTH,
      1,
      TRUE,
      Gfx
      );
    break;
  default:
    ASSERT (FALSE);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init GMC
 *
 *
 *
 * @param[in] Gfx           Pointer to global GFX configuration
 * @retval    AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GfxGmcInit (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInit Enter\n");
  GfxGmcDisableClockGating (Gfx);
  GfxGmcSetMemoryAddressTranslation (Gfx);
  GfxGmcInitializeSequencerModel (Gfx);
  GfxGmcInitializeRegisterEngine (Gfx);
  GfxGmcInitializeFbLocation (Gfx);
  GfxGmcUmaSteering (Gfx);
  GfxGmcSecureGarlicAccess (Gfx);
  GfxGmcInitializeC6Aperture (Gfx);
  GfxFmGmcAddressSwizzel (Gfx);
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_GMM_REGISTER_OVERRIDE, Gfx, GnbLibGetHeader (Gfx));
  GfxGmcLockCriticalRegisters (Gfx);
  GfxGmcPerformanceTuning (Gfx);
  GfxGmcMiscInit (Gfx);
  GfxGmcRemoveBlackout (Gfx);
  if (Gfx->GmcClockGating == OptionEnabled) {
    GfxGmcEnableClockGating (Gfx);
  }
  GfxGmcInitializePowerGating (Gfx);
  GfxFmGmcAllowPstateHigh (Gfx);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInit Exit\n");
  return  Status;
}
