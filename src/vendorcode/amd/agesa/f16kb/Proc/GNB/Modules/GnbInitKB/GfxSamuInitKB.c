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
 * @e \$Revision: 86584 $   @e \$Date: 2013-01-23 12:34:28 -0600 (Wed, 23 Jan 2013) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include  "amdlib.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbRegisterAccKB.h"
#include  "cpuFamilyTranslation.h"
#include  "GnbRegistersKB.h"
#include  "GfxLibKB.h"
#include  "GfxSamuInitKB.h"
#include  "GnbSamuPatchKB.h"
#include  "OptionGnb.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXSAMUINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
#define LENGTH_1MBYTE             0x0100000ul
#define MASK_1MBYTE               0x0FFFFFul

#define SAM_IND_INDEX             0x22000ul
#define SAM_IND_DATA              0x22004ul

#define SAM_SAB_IND_INDEX         0x22008ul
#define SAM_SAB_IND_DATA          0x2200Cul

#define SMU_TOOLS_INDEX           0x238ul
#define SMU_TOOLS_DATA            0x23Cul

typedef struct {
  UINT32  BootControl;     ///<
  UINT32  KeySelect;       ///<
  UINT32  KernelAddrLo;    ///<
  UINT32  KernelAddrHi;    ///<
  UINT32  TweakSelect;     ///<
} SAMU_BOOT_CONTROL;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


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
GfxSamuInit (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32                    D0F0xBC_xC00C0000;
  GNB_HANDLE                *GnbHandle;
  VOID                      *ControlXBuffer;
  VOID                      *AlignedControlXBuffer;
  VOID                      *PatchYBuffer;
  VOID                      *AlignedPatchYBuffer;
  SAMU_BOOT_CONTROL         *SamuBootControl;

  UINT32                    D0F0xBC_x800000A4;
  UINT32                    GMMx22000;
  UINT32                    GMMx22004;
  UINT32                    GMMx22008;
  UINT32                    GMMx2200C;
  UINT32                    LoopCount;
  BOOLEAN                   SamuUseF1dPatch;
  BOOLEAN                   SamuPatchEnabled;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbSamuInit Enter\n");

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  ASSERT (GnbHandle != NULL);
  GnbRegisterReadKB (GnbHandle, 0x4, 0xc00c0000,
      &D0F0xBC_xC00C0000, 0, GnbLibGetHeader (Gfx));

  SamuPatchEnabled = GnbBuildOptions.CfgSamuPatchEnabled;
  IDS_OPTION_HOOK (IDS_GNB_LOAD_SAMU_PATCH, &SamuPatchEnabled, GnbLibGetHeader (Gfx));

  if ((((D0F0xBC_xC00C0000) & BIT24) == 0) &&
      (SamuPatchEnabled == TRUE)) {

    // Decide which version of the patch to use
    SamuUseF1dPatch = TRUE;

    GMMx22008 = 0x29;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22008,
        &GMMx22008, 0, GnbLibGetHeader (Gfx));
    GnbRegisterReadKB (GnbHandle, 0x12, 0x2200C,
        &GMMx2200C, 0, GnbLibGetHeader (Gfx));
    IDS_HDT_CONSOLE (GNB_TRACE, " SAMSAB:29=%08x\n", GMMx2200C);

    if (GMMx2200C == 0x80000001) {
      SamuUseF1dPatch = FALSE;
    }

    ControlXBuffer = GnbAllocateHeapBufferAndClear (AMD_GNB_SAMU_BOOT_CONTROL_HANDLE, 2 * LENGTH_1MBYTE, GnbLibGetHeader (Gfx));
    ASSERT (ControlXBuffer != NULL);
    if (ControlXBuffer == NULL) {
      return  AGESA_ERROR;
    }
    AlignedControlXBuffer = (VOID *) (((UINTN)ControlXBuffer + LENGTH_1MBYTE) & (~MASK_1MBYTE));
    PatchYBuffer = GnbAllocateHeapBuffer (AMD_GNB_SAMU_PATCH_HANDLE, 2 * LENGTH_1MBYTE, GnbLibGetHeader (Gfx));
    ASSERT (PatchYBuffer != NULL);
    if (PatchYBuffer == NULL) {
      return  AGESA_ERROR;
    }
    AlignedPatchYBuffer = (VOID *) (((UINTN)PatchYBuffer + LENGTH_1MBYTE) & (~MASK_1MBYTE));

    // Copy samu firmware patch to PatchYBuffer
    if (SamuUseF1dPatch == TRUE) {
      LibAmdMemCopy (AlignedPatchYBuffer, &SamuPatchKB[0],
          SamuPatchKBHeader[1], GnbLibGetHeader (Gfx));
    } else {
      LibAmdMemCopy (AlignedPatchYBuffer, &SamuPatchKBUnf1[0],
          SamuPatchKBHeaderUnf1[1], GnbLibGetHeader (Gfx));
    }

    // WBINVD
    LibAmdWriteBackInvalidateCache ();

    // Load boot control structure
    SamuBootControl = (SAMU_BOOT_CONTROL *)AlignedControlXBuffer;
    SamuBootControl->BootControl = 0x3;
    SamuBootControl->KernelAddrLo = (UINT32) (AlignedPatchYBuffer);
    SamuBootControl->KernelAddrHi = 0; //(UINT32) ((((UINT64) AlignedPatchYBuffer) >> 32) & 0xFF);
    if (SamuUseF1dPatch == TRUE) {
      SamuBootControl->TweakSelect = 0xBB027E1F;
      SamuBootControl->KeySelect = 0x8E174F83;
    } else {
      SamuBootControl->TweakSelect = 0x0;
      SamuBootControl->KeySelect = 0x0;
    }


    // Write 0x0 to SAM_CGC_HOST_CTRL to release the clock-gating of SAMU
    GMMx22000 = 0x3;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22000, &GMMx22000, 0, GnbLibGetHeader (Gfx));
    GMMx22004 = 0x0;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22004, &GMMx22004, 0, GnbLibGetHeader (Gfx));

    // Write (physical address of boot control structure)>>8 into SAM_SAB_INIT_TLB_CONFIG (Location X >> 8)
    GMMx22008 = 0x4;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22008, &GMMx22008, 0, GnbLibGetHeader (Gfx));
    GMMx2200C = ((UINT32) ((UINT32) AlignedControlXBuffer)) >> 8;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x2200C, &GMMx2200C, 0, GnbLibGetHeader (Gfx));

    // Write 0x0 to SAM_RST_HOST_SOFT_RESET
    GMMx22000 = 0x1;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22000, &GMMx22000, 0, GnbLibGetHeader (Gfx));
    GMMx22004 = 0x0;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22004, &GMMx22004, 0, GnbLibGetHeader (Gfx));

    // Write 0x2 to SAM_SCRATCH_0 to start the firmware boot
    GMMx22000 = 0x38;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22000, &GMMx22000, 0, GnbLibGetHeader (Gfx));
    GMMx22004 = 0x2;
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x22004, &GMMx22004, 0, GnbLibGetHeader (Gfx));

    // Poll SAM_RST_HOST_SOFT_RST_RDY and wait for HOST_RDY
    do {
      // Write 0x2 to SAM_SCRATCH_0 to start the firmware boot
      GMMx22000 = 0x51;
      GnbRegisterWriteKB (GnbHandle, 0x12, 0x22000, &GMMx22000, 0, GnbLibGetHeader (Gfx));
      GnbRegisterReadKB (GnbHandle, 0x12, 0x22004, &GMMx22004, 0, GnbLibGetHeader (Gfx));
    } while ((GMMx22004 & BIT0) == 0);

    // Clear the allocated memory ranges, locations X and Y (write 0), issue WBINVD
    LibAmdMemFill (ControlXBuffer, 0, 2 * LENGTH_1MBYTE, GnbLibGetHeader (Gfx));
    LibAmdMemFill (PatchYBuffer, 0, 2 * LENGTH_1MBYTE, GnbLibGetHeader (Gfx));
    LibAmdWriteBackInvalidateCache ();

    // Confirm read of SMC_DRAM_ACCESS_CNTL is 0x1
    D0F0xBC_x800000A4 = 0;
    for (LoopCount = 0; LoopCount < 0x00FFFFFF; LoopCount++) {
      GnbRegisterReadKB (GnbHandle, 0x4, 0x800000A4, &D0F0xBC_x800000A4, 0, GnbLibGetHeader (Gfx));
      if ((D0F0xBC_x800000A4 & BIT0) != 0)  {
        break;
      }
    }
    ASSERT ((D0F0xBC_x800000A4 & BIT0) != 0);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbSamuInit Exit\n");
  return AGESA_SUCCESS;
}
