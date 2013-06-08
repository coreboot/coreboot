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
