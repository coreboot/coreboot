/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD ALIB option API.
 *
 * Contains structures and values used to control the ALIB option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

#ifndef _OPTION_GNB_H_
#define _OPTION_GNB_H_

/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */


typedef AGESA_STATUS OPTION_GNB_FEATURE (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

typedef AGESA_STATUS F_ALIB_UPDATE (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

typedef VOID* F_ALIB_GET (
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/// The Option Configuration
typedef struct {
  UINT64                Type;                   ///< Type
  OPTION_GNB_FEATURE    *GnbFeature;            ///< The GNB Feature
} OPTION_GNB_CONFIGURATION;

/// The Build time options configuration
typedef struct {
  BOOLEAN               IgfxModeAsPcieEp;                   ///< Itegrated Gfx mode Pcie EP or Legacy
  BOOLEAN               LclkDeepSleepEn;                    ///< Default for LCLK deep sleep
  BOOLEAN               LclkDpmEn;                          ///< Default for LCLK DPM
  UINT8                 GmcPowerGating;                     ///< Control GMC power gating
  BOOLEAN               SmuSclkClockGatingEnable;           ///< Control SMU SCLK gating
  BOOLEAN               PcieAspmBlackListEnable;            ///< Control Pcie Aspm Black List
  BOOLEAN               IvrsRelativeAddrNamesSupport;       ///< Support for relative address names
  BOOLEAN               GnbLoadRealFuseTable;               ///< Support for fuse table loading
  UINT32                CfgGnbLinkReceiverDetectionPooling; ///< Receiver pooling detection time in us.
  UINT32                CfgGnbLinkL0Pooling;                ///< Pooling for link to get to L0 in us
  UINT32                CfgGnbLinkGpioResetAssertionTime;   ///< Gpio reset assertion time in us
  UINT32                CfgGnbLinkResetToTrainingTime;      ///< Time duration between deassert GPIO reset and release training in us
  UINT8                 CfgGnbTrainingAlgorithm;            ///< distribution of training across interface calls
  BOOLEAN               CfgForceCableSafeOff;               ///< Force cable safe off
  BOOLEAN               CfgOrbClockGatingEnable;            ///< Control ORB clock gating
  UINT8                 CfgPciePowerGatingFlags;            ///< Pcie Power gating flags
  BOOLEAN               CfgIocLclkClockGatingEnable;        ///< Control IOC LCLK clock gating
  BOOLEAN               CfgIocSclkClockGatingEnable;        ///< Control IOC SCLK clock gating
  BOOLEAN               CfgIommuL1ClockGatingEnable;        ///< Control IOMMU L1 clock gating
  BOOLEAN               CfgIommuL2ClockGatingEnable;        ///< Control IOMMU L2 clock gating
  BOOLEAN               CfgAltVddNb;                        ///< AltVDDNB support
  BOOLEAN               CfgBapmSupport;                     ///< BAPM support
  BOOLEAN               CfgUnusedSimdPowerGatingEnable;     ///< Control unused SIMD power gate
  BOOLEAN               CfgUnusedRbPowerGatingEnable;       ///< Control unused SIMD power gate
  BOOLEAN               CfgNbdpmEnable;                     ///< NBDPM refers to dynamically reprogramming High and Low NB Pstates under different system usage scenarios
  BOOLEAN               CfgGmcClockGating;                  ///< Control GMC clock power gate
  BOOLEAN               CfgMaxPayloadEnable;                ///< Enables configuration of Max_Payload_Size in PCIe device links
  BOOLEAN               CfgOrbDynWakeEnable;                ///< Enables ORB Dynamic wake up
  BOOLEAN               CfgLoadlineEnable;                  ///< Enable Loadline Optimization
} GNB_BUILD_OPTIONS;


/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

#endif  // _OPTION_GNB_H_
