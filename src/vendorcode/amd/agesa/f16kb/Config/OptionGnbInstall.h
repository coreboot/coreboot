/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: GNB
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 87849 $   @e \$Date: 2013-02-11 15:37:58 -0600 (Mon, 11 Feb 2013) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#ifndef _OPTION_GNB_INSTALL_H_
#define _OPTION_GNB_INSTALL_H_

#include "S3SaveState.h"
/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */

//---------------------------------------------------------------------------------------------------
// Family installation
//---------------------------------------------------------------------------------------------------



#define GNB_TYPE_TN   FALSE
#define GNB_TYPE_BK   FALSE
#define GNB_TYPE_KV   FALSE
#define GNB_TYPE_KB   FALSE
#define GNB_TYPE_BK   FALSE
#define GNB_TYPE_ML   FALSE

#if (OPTION_FAMILY15H_TN == TRUE)
  #undef  GNB_TYPE_TN
  #define GNB_TYPE_TN  TRUE
#endif


#if (OPTION_FAMILY16H_KB == TRUE)
  #undef  GNB_TYPE_KB
  #define GNB_TYPE_KB  TRUE
#endif


#if (GNB_TYPE_KB == TRUE || GNB_TYPE_TN == TRUE)
//---------------------------------------------------------------------------------------------------
// Service installation
//---------------------------------------------------------------------------------------------------

  #include "Gnb.h"
  #include "GnbPcie.h"
  #include "GnbGfx.h"

  #define SERVICES_POINTER  NULL
  #if (GNB_TYPE_TN == TRUE)
    #include "GnbInitTNInstall.h"
  #endif
  #if (GNB_TYPE_KB == TRUE)
    #include "GnbInitKBInstall.h"
  #endif
  CONST GNB_SERVICE   * CONST ServiceTable = SERVICES_POINTER;

//---------------------------------------------------------------------------------------------------
// BUILD options
//---------------------------------------------------------------------------------------------------

  #ifndef CFG_IGFX_AS_PCIE_EP
    #define CFG_IGFX_AS_PCIE_EP                           TRUE
  #endif

  #ifndef CFG_LCLK_DEEP_SLEEP_EN
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_LCLK_DEEP_SLEEP_EN                       FALSE
    #else
      #define  CFG_LCLK_DEEP_SLEEP_EN                       TRUE
    #endif
  #endif

  #ifndef CFG_LCLK_DPM_EN
    #define CFG_LCLK_DPM_EN                                 TRUE
  #endif

  #ifndef CFG_GMC_POWER_GATING
    #if ((GNB_TYPE_TN == TRUE) || (GNB_TYPE_KB == TRUE))
      #define CFG_GMC_POWER_GATING                          GmcPowerGatingWithStutter
    #else
      #define CFG_GMC_POWER_GATING                          GmcPowerGatingDisabled
    #endif
  #endif

  #ifndef CFG_SMU_SCLK_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_SMU_SCLK_CLOCK_GATING_ENABLE           TRUE
    #else
      #define CFG_SMU_SCLK_CLOCK_GATING_ENABLE            FALSE
    #endif
  #endif

  #ifndef CFG_PCIE_ASPM_BLACK_LIST_ENABLE
    #define CFG_PCIE_ASPM_BLACK_LIST_ENABLE               TRUE
  #endif

  #ifndef CFG_GNB_IVRS_RELATIVE_ADDR_NAMES_SUPPORT
    #define CFG_GNB_IVRS_RELATIVE_ADDR_NAMES_SUPPORT      FALSE
  #endif

  #ifndef CFG_GNB_LOAD_REAL_FUSE
    #define  CFG_GNB_LOAD_REAL_FUSE                       TRUE
  #endif

  #ifndef CFG_GNB_PCIE_LINK_RECEIVER_DETECTION_POOLING
    #define CFG_GNB_PCIE_LINK_RECEIVER_DETECTION_POOLING  (60 * 1000)
  #endif

  #ifndef CFG_GNB_PCIE_LINK_L0_POOLING
    #define CFG_GNB_PCIE_LINK_L0_POOLING                  (60 * 1000)
  #endif

  #ifndef CFG_GNB_PCIE_LINK_GPIO_RESET_ASSERT_TIME
    #define CFG_GNB_PCIE_LINK_GPIO_RESET_ASSERT_TIME      (2 * 1000)
  #endif

  #ifndef CFG_GNB_PCIE_LINK_RESET_TO_TRAINING_TIME
    #define CFG_GNB_PCIE_LINK_RESET_TO_TRAINING_TIME      (2 * 1000)
  #endif

  #ifdef BLDCFG_PCIE_TRAINING_ALGORITHM
    #define CFG_GNB_PCIE_TRAINING_ALGORITHM               BLDCFG_PCIE_TRAINING_ALGORITHM
  #else
    #define CFG_GNB_PCIE_TRAINING_ALGORITHM               PcieTrainingDistributed
  #endif

  #ifndef CFG_GNB_FORCE_CABLESAFE_OFF
    #define CFG_GNB_FORCE_CABLESAFE_OFF                   FALSE
  #endif

  #ifndef CFG_ORB_CLOCK_GATING_ENABLE
      #define CFG_ORB_CLOCK_GATING_ENABLE                 TRUE
  #endif

  #ifndef CFG_GNB_PCIE_POWERGATING_FLAGS
    #define CFG_GNB_PCIE_POWERGATING_FLAGS                0x0
  #endif


  #ifndef CFG_IOC_SCLK_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_IOC_SCLK_CLOCK_GATING_ENABLE           TRUE
    #else
      #define CFG_IOC_SCLK_CLOCK_GATING_ENABLE            FALSE
    #endif
  #endif

  #ifndef CFG_IOMMU_L1_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_IOMMU_L1_CLOCK_GATING_ENABLE           TRUE
    #else
      #define CFG_IOMMU_L1_CLOCK_GATING_ENABLE            FALSE
    #endif
  #endif

  #ifndef CFG_IOMMU_L2_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_IOMMU_L2_CLOCK_GATING_ENABLE           TRUE
    #else
      #define CFG_IOMMU_L2_CLOCK_GATING_ENABLE            FALSE
    #endif
  #endif

  #ifndef CFG_GNB_ALTVDDNB_SUPPORT
    #define CFG_GNB_ALTVDDNB_SUPPORT                      TRUE
  #endif

  #ifndef CFG_GNB_BAPM_SUPPORT
    #if ((GNB_TYPE_TN == TRUE) || (GNB_TYPE_KB == TRUE))
      #define CFG_GNB_BAPM_SUPPORT                        TRUE
    #else
      #define CFG_GNB_BAPM_SUPPORT                        FALSE
    #endif
  #endif

  #ifndef CFG_GNB_LHTC_SUPPORT
    #if (GNB_TYPE_KB == TRUE)
      #define CFG_GNB_LHTC_SUPPORT                        TRUE
    #else
      #define CFG_GNB_LHTC_SUPPORT                        FALSE
    #endif
  #endif

  #ifndef CFG_UNUSED_SIMD_POWERGATING_ENABLE
    #define CFG_UNUSED_SIMD_POWERGATING_ENABLE            TRUE
  #endif

  #ifndef CFG_UNUSED_RB_POWERGATING_ENABLE
    #define CFG_UNUSED_RB_POWERGATING_ENABLE              FALSE
  #endif

  #ifndef CFG_NBDPM_ENABLE
    #if ((GNB_TYPE_KB == TRUE))
      #define CFG_NBDPM_ENABLE                              FALSE
    #else
      #define CFG_NBDPM_ENABLE                              TRUE
    #endif
  #endif

  #ifndef CFG_MAX_PAYLOAD_ENABLE
    #define CFG_MAX_PAYLOAD_ENABLE                        TRUE
  #endif


  #ifndef CFG_ORB_DYN_WAKE_ENABLE
    #if (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      #define  CFG_ORB_DYN_WAKE_ENABLE                    TRUE
    #else
      #define CFG_ORB_DYN_WAKE_ENABLE                     TRUE
    #endif
  #endif

  #ifndef CFG_LOADLINE_ENABLE
    #define CFG_LOADLINE_ENABLE                           TRUE
  #endif

  #ifndef CFG_PCIE_PHY_ISOLATION_SUPPORT
    #if (GNB_TYPE_KB == TRUE)
      #define CFG_PCIE_PHY_ISOLATION_SUPPORT             TRUE
    #else
      #define CFG_PCIE_PHY_ISOLATION_SUPPORT             FALSE
    #endif
  #endif

  #ifndef CFG_SVI_REVISION
    #if (GNB_TYPE_KB == TRUE || GNB_TYPE_TN == TRUE)
      #define CFG_SVI_REVISION             2
    #else
      #define CFG_SVI_REVISION             1
    #endif
  #endif

  #ifndef CFG_SCS_SUPPORT
    #if ((GNB_TYPE_KB == TRUE))
      #define CFG_SCS_SUPPORT             TRUE
    #else
      #define CFG_SCS_SUPPORT             FALSE
    #endif
  #endif

  #ifndef CFG_SAMU_PATCH_ENABLED
    #define CFG_SAMU_PATCH_ENABLED      TRUE
  #endif

  #ifndef CFG_GNB_TDC_SUPPORT
    #define CFG_GNB_TDC_SUPPORT          TRUE
  #endif
  #ifndef CFG_NATIVE_GEN1_PLL_ENABLE
    #define CFG_NATIVE_GEN1_PLL_ENABLE   TRUE
  #endif

  #ifndef CFG_UMA_STEERING
    #define CFG_UMA_STEERING             0
  #endif

  CONST GNB_BUILD_OPTIONS ROMDATA GnbBuildOptions = {
    CFG_IGFX_AS_PCIE_EP,
    CFG_LCLK_DEEP_SLEEP_EN,
    CFG_LCLK_DPM_EN,
    CFG_GMC_POWER_GATING,
    CFG_SMU_SCLK_CLOCK_GATING_ENABLE,
    CFG_PCIE_ASPM_BLACK_LIST_ENABLE,
    CFG_GNB_IVRS_RELATIVE_ADDR_NAMES_SUPPORT,
    CFG_GNB_LOAD_REAL_FUSE,
    CFG_GNB_PCIE_LINK_RECEIVER_DETECTION_POOLING,
    CFG_GNB_PCIE_LINK_L0_POOLING,
    CFG_GNB_PCIE_LINK_GPIO_RESET_ASSERT_TIME,
    CFG_GNB_PCIE_LINK_RESET_TO_TRAINING_TIME,
    CFG_GNB_PCIE_TRAINING_ALGORITHM,
    CFG_GNB_FORCE_CABLESAFE_OFF,
    CFG_ORB_CLOCK_GATING_ENABLE,
    CFG_GNB_PCIE_POWERGATING_FLAGS,
    TRUE,
    CFG_IOC_SCLK_CLOCK_GATING_ENABLE,
    CFG_IOMMU_L1_CLOCK_GATING_ENABLE,
    CFG_IOMMU_L2_CLOCK_GATING_ENABLE,
    CFG_GNB_ALTVDDNB_SUPPORT,
    CFG_GNB_BAPM_SUPPORT,
    CFG_UNUSED_SIMD_POWERGATING_ENABLE,
    CFG_UNUSED_RB_POWERGATING_ENABLE,
    CFG_NBDPM_ENABLE,
    TRUE,
    CFG_MAX_PAYLOAD_ENABLE,
    CFG_ORB_DYN_WAKE_ENABLE,
    CFG_LOADLINE_ENABLE,
    CFG_PCIE_PHY_ISOLATION_SUPPORT,
    CFG_GNB_LHTC_SUPPORT,
    CFG_SVI_REVISION,
    CFG_SCS_SUPPORT,
    CFG_SAMU_PATCH_ENABLED,
    {CFG_ACPI_SET_OEM_ID},
    {CFG_ACPI_SET_OEM_TABLE_ID},
    CFG_GNB_TDC_SUPPORT,
    TRUE,
    CFG_NATIVE_GEN1_PLL_ENABLE,
    CFG_UMA_STEERING
  };

  //---------------------------------------------------------------------------------------------------
  // SMU Firmware
  //---------------------------------------------------------------------------------------------------



  //---------------------------------------------------------------------------------------------------
  // Module entries
  //---------------------------------------------------------------------------------------------------

  #if (AGESA_ENTRY_INIT_EARLY == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_EARLY_INIT
      #define OPTION_NB_EARLY_INIT TRUE
    #endif
    #if (OPTION_NB_EARLY_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbEarlyInterfaceTN;
      #define OPTION_GNBEARLYINTERFACETN_ENTRY            {AMD_FAMILY_TN, GnbEarlyInterfaceTN, TpGnbEarlyInterface},
    #else
      #define OPTION_GNBEARLYINTERFACETN_ENTRY
    #endif
    #if (OPTION_NB_EARLY_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GnbEarlyInterfaceKB;
      #define OPTION_GNBEARLYINTERFACEKB_ENTRY            {AMD_FAMILY_KB, GnbEarlyInterfaceKB, TpGnbEarlyInterface},
    #else
      #define OPTION_GNBEARLYINTERFACEKB_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CONFIG_MAP
      #define OPTION_PCIE_CONFIG_MAP TRUE
    #endif
    #if (OPTION_PCIE_CONFIG_MAP == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                 PcieConfigurationMap;
      #define OPTION_PCIECONFIGURATIONMAP_ENTRY          {AMD_FAMILY_TN | AMD_FAMILY_KB, PcieConfigurationMap, TpGnbPcieConfigurationMap},
    #else
      #define OPTION_PCIECONFIGURATIONMAP_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_EARLY_INIT
      #define OPTION_PCIE_EARLY_INIT TRUE
    #endif
    #if (OPTION_PCIE_EARLY_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PcieEarlyInterfaceTN;
      #define OPTION_PCIEEARLYINTERFACETN_ENTRY           {AMD_FAMILY_TN, PcieEarlyInterfaceTN, TpGnbPcieEarlyInterface},
    #else
      #define  OPTION_PCIEEARLYINTERFACETN_ENTRY
    #endif
    #if (OPTION_PCIE_EARLY_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieEarlyInterfaceKB;
      #define OPTION_PCIEEARLYINTERFACEKB_ENTRY           {AMD_FAMILY_KB, PcieEarlyInterfaceKB, TpGnbPcieEarlyInterface},
    #else
      #define OPTION_PCIEEARLYINTERFACEKB_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    CONST OPTION_GNB_CONFIGURATION  GnbEarlyFeatureTable[] = {
      OPTION_GNBEARLYINTERFACETN_ENTRY
      OPTION_GNBEARLYINTERFACEKB_ENTRY
      OPTION_PCIECONFIGURATIONMAP_ENTRY
      OPTION_PCIEEARLYINTERFACETN_ENTRY
      OPTION_PCIEEARLYINTERFACEKB_ENTRY
      {0, NULL, EndGnbTestPoints}
    };

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CONFIG_INIT
      #define OPTION_PCIE_CONFIG_INIT TRUE
    #endif
    #if (OPTION_PCIE_CONFIG_INIT == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieConfigurationInit;
      #define OPTION_PCIECONFIGURATIONINIT_ENTRY          {AMD_FAMILY_TN | AMD_FAMILY_KB, PcieConfigurationInit, TpGnbEarlierPcieConfigurationInit},
    #else
      #define OPTION_PCIECONFIGURATIONINIT_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_EARLIER_INIT
      #define OPTION_NB_EARLIER_INIT TRUE
    #endif
    #if (OPTION_NB_EARLIER_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbEarlierInterfaceTN;
      #define OPTION_GNBEARLIERINTERFACETN_ENTRY          {AMD_FAMILY_TN, GnbEarlierInterfaceTN, TpGnbEarlierInterface},
    #else
      #define OPTION_GNBEARLIERINTERFACETN_ENTRY
    #endif
    #if (OPTION_NB_EARLIER_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GnbEarlierInterfaceKB;
      #define OPTION_GNBEARLIERINTERFACEKB_ENTRY          {AMD_FAMILY_KB, GnbEarlierInterfaceKB, TpGnbEarlierInterface},
    #else
      #define OPTION_GNBEARLIERINTERFACEKB_ENTRY
    #endif

    #if (OPTION_NB_EARLIER_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  OptionGnbInstall581;
      #define OPTION_GNBSCSINTERFACEKB_ENTRY          {AMD_FAMILY_KB, OptionGnbInstall581, TpGnbEarlierInterface},
    #else
      #define OPTION_GNBSCSINTERFACEKB_ENTRY
    #endif


    CONST OPTION_GNB_CONFIGURATION  GnbEarlierFeatureTable[] = {
      OPTION_PCIECONFIGURATIONINIT_ENTRY
      OPTION_GNBEARLIERINTERFACETN_ENTRY
      OPTION_GNBEARLIERINTERFACEKB_ENTRY
      OPTION_GNBSCSINTERFACEKB_ENTRY
      {0, NULL, EndGnbTestPoints}
    };
  #endif

  #if (AGESA_ENTRY_INIT_POST == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_POST_INIT
      #define OPTION_GFX_CONFIG_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxConfigPostInterface;
      #define OPTION_GFXCONFIGPOSTINTERFACE_ENTRY         {AMD_FAMILY_TN | AMD_FAMILY_KB, GfxConfigPostInterface, TpGnbGfxConfigPostInterface},
    #else
      #define OPTION_GFXCONFIGPOSTINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_POST_INIT
      #define OPTION_GFX_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxPostInterfaceTN;
      #define OPTION_GFXPOSTINTERFACETN_ENTRY             {AMD_FAMILY_TN, GfxPostInterfaceTN, TpGnbGfxPostInterface},
    #else
      #define OPTION_GFXPOSTINTERFACETN_ENTRY
    #endif
    #if (OPTION_GFX_POST_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxPostInterfaceKB;
      #define OPTION_GFXPOSTINTERFACEKB_ENTRY             {AMD_FAMILY_KB, GfxPostInterfaceKB, TpGnbGfxPostInterface},
    #else
      #define OPTION_GFXPOSTINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_POST_INIT
      #define OPTION_NB_POST_INIT TRUE
    #endif
    #if (OPTION_NB_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbPostInterfaceTN;
      #define OPTION_GNBPOSTINTERFACETN_ENTRY             {AMD_FAMILY_TN, GnbPostInterfaceTN, TpGnbPostInterface},
    #else
      #define OPTION_GNBPOSTINTERFACETN_ENTRY
    #endif
    #if (OPTION_NB_POST_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GnbPostInterfaceKB;
      #define OPTION_GNBPOSTINTERFACEKB_ENTRY             {AMD_FAMILY_KB, GnbPostInterfaceKB, TpGnbPostInterface},
    #else
      #define OPTION_GNBPOSTINTERFACEKB_ENTRY
    #endif

    //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POST_EARLY_INIT
      #define OPTION_PCIE_POST_EARLY_INIT TRUE
    #endif
    #if (OPTION_PCIE_POST_EARLY_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PciePostEarlyInterfaceTN;
      #define OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY       {AMD_FAMILY_TN, PciePostEarlyInterfaceTN, TpGnbPciePostEarlyInterface},
    #else
      #define OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY
    #endif
    #if (OPTION_PCIE_POST_EARLY_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PciePostEarlyInterfaceKB;
      #define OPTION_PCIEPOSTEARLYINTERFACEKB_ENTRY       {AMD_FAMILY_KB, PciePostEarlyInterfaceKB, TpGnbPciePostEarlyInterface},
    #else
      #define OPTION_PCIEPOSTEARLYINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POST_INIT
      #define OPTION_PCIE_POST_INIT TRUE
    #endif
    #if (OPTION_PCIE_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PciePostInterfaceTN;
      #define OPTION_PCIEPOSTINTERFACETN_ENTRY            {AMD_FAMILY_TN, PciePostInterfaceTN, TpGnbPciePostInterface},
    #else
      #define OPTION_PCIEPOSTINTERFACETN_ENTRY
    #endif
    #if (OPTION_PCIE_POST_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PciePostInterfaceKB;
      #define OPTION_PCIEPOSTINTERFACEKB_ENTRY            {AMD_FAMILY_KB, PciePostInterfaceKB, TpGnbPciePostInterface},
    #else
      #define OPTION_PCIEPOSTINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    CONST OPTION_GNB_CONFIGURATION  GnbPostFeatureTable[] = {
      OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY
      OPTION_PCIEPOSTEARLYINTERFACEKB_ENTRY
      OPTION_GFXCONFIGPOSTINTERFACE_ENTRY
      OPTION_GFXPOSTINTERFACETN_ENTRY
      OPTION_GFXPOSTINTERFACEKB_ENTRY
      {0, NULL, EndGnbTestPoints}
    };

    CONST OPTION_GNB_CONFIGURATION  GnbPostAfterDramFeatureTable[] = {
      OPTION_GNBPOSTINTERFACETN_ENTRY
      OPTION_GNBPOSTINTERFACEKB_ENTRY
      OPTION_PCIEPOSTINTERFACETN_ENTRY
      OPTION_PCIEPOSTINTERFACEKB_ENTRY
      {0, NULL, EndGnbTestPoints}
    };
  #endif

  #if (AGESA_ENTRY_INIT_ENV == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_ENV_INIT
      #define OPTION_NB_ENV_INIT TRUE
    #endif
    #if (OPTION_NB_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbEnvInterfaceTN;
      #define OPTION_GNBENVINTERFACETN_ENTRY              {AMD_FAMILY_TN, GnbEnvInterfaceTN, TpGnbEnvInterface},
    #else
      #define OPTION_GNBENVINTERFACETN_ENTRY
    #endif
    #if (OPTION_NB_ENV_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GnbEnvInterfaceKB;
      #define OPTION_GNBENVINTERFACEKB_ENTRY              {AMD_FAMILY_KB, GnbEnvInterfaceKB, TpGnbEnvInterface},
    #else
      #define OPTION_GNBENVINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_ENV_INIT
      #define OPTION_GFX_CONFIG_ENV_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                   GfxConfigEnvInterface;
      #define OPTION_GFXCONFIGENVINTERFACE_ENTRY          {AMD_FAMILY_TN | AMD_FAMILY_KB, GfxConfigEnvInterface, TpGnbGfxConfigEnvInterface},
    #else
      #define  OPTION_GFXCONFIGENVINTERFACE_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_ENV_INIT
      #define OPTION_GFX_ENV_INIT TRUE
    #endif
    #if (OPTION_GFX_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxEnvInterfaceTN;
      #define OPTION_GFXENVINTERFACETN_ENTRY              {AMD_FAMILY_TN, GfxEnvInterfaceTN, TpGnbGfxEnvInterface},
    #else
      #define OPTION_GFXENVINTERFACETN_ENTRY
    #endif
    #if (OPTION_GFX_ENV_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxEnvInterfaceKB;
      #define OPTION_GFXENVINTERFACEKB_ENTRY              {AMD_FAMILY_KB, GfxEnvInterfaceKB, TpGnbGfxEnvInterface},
    #else
      #define OPTION_GFXENVINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_ENV_INIT
      #define OPTION_PCIE_ENV_INIT TRUE
    #endif
    #if (OPTION_PCIE_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE PcieEnvInterfaceTN;
      #define OPTION_PCIEENVINTERFACETN_ENTRY             {AMD_FAMILY_TN, PcieEnvInterfaceTN, TpGnbPcieEnvInterface},
    #else
      #define OPTION_PCIEENVINTERFACETN_ENTRY
    #endif
    #if (OPTION_PCIE_ENV_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieEnvInterfaceKB;
      #define OPTION_PCIEENVINTERFACEKB_ENTRY             {AMD_FAMILY_KB, PcieEnvInterfaceKB, TpGnbPcieEnvInterface},
    #else
      #define OPTION_PCIEENVINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------

    CONST OPTION_GNB_CONFIGURATION  GnbEnvFeatureTable[] = {
      OPTION_GNBENVINTERFACETN_ENTRY
      OPTION_GNBENVINTERFACEKB_ENTRY
      OPTION_PCIEENVINTERFACETN_ENTRY
      OPTION_PCIEENVINTERFACEKB_ENTRY
      OPTION_GFXCONFIGENVINTERFACE_ENTRY
      OPTION_GFXENVINTERFACETN_ENTRY
      OPTION_GFXENVINTERFACEKB_ENTRY
      {0, NULL, EndGnbTestPoints}
    };
  #endif

  #if (AGESA_ENTRY_INIT_MID == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_MID_INIT
      #define OPTION_GFX_MID_INIT TRUE
    #endif
    #if (OPTION_GFX_MID_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxMidInterfaceTN;
      #define OPTION_GFXMIDINTERFACETN_ENTRY              {AMD_FAMILY_TN, GfxMidInterfaceTN, TpGnbGfxMidInterface},
    #else
      #define OPTION_GFXMIDINTERFACETN_ENTRY
    #endif
    #if (OPTION_GFX_MID_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxMidInterfaceKB;
      #define OPTION_GFXMIDINTERFACEKB_ENTRY              {AMD_FAMILY_KB, GfxMidInterfaceKB, TpGnbGfxMidInterface},
    #else
      #define OPTION_GFXMIDINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_INTEGRATED_TABLE_INIT
      #define OPTION_GFX_INTEGRATED_TABLE_INIT TRUE
    #endif
    #if (OPTION_GFX_INTEGRATED_TABLE_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxIntInfoTableInterfaceTN;
      #define OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY     {AMD_FAMILY_TN, GfxIntInfoTableInterfaceTN},
    #else
      #define OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY
    #endif
    #if (OPTION_GFX_INTEGRATED_TABLE_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxIntInfoTableInterfaceKB;
      #define OPTION_GFXINTINFOTABLEINTERFACEKB_ENTRY     {AMD_FAMILY_KB, GfxIntInfoTableInterfaceKB},
    #else
      #define OPTION_GFXINTINFOTABLEINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIe_MID_INIT
      #define OPTION_PCIe_MID_INIT TRUE
    #endif
    #if (OPTION_PCIe_MID_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PcieMidInterfaceTN;
      #define OPTION_PCIEMIDINTERFACETN_ENTRY             {AMD_FAMILY_TN, PcieMidInterfaceTN, TpPcieMidInterface},
    #else
      #define OPTION_PCIEMIDINTERFACETN_ENTRY
    #endif
    #if (OPTION_PCIe_MID_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieMidInterfaceKB;
      #define OPTION_PCIEMIDINTERFACEKB_ENTRY             {AMD_FAMILY_KB, PcieMidInterfaceKB, TpPcieMidInterface},
    #else
      #define OPTION_PCIEMIDINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_MID_INIT
      #define OPTION_NB_MID_INIT TRUE
    #endif
    #if (OPTION_NB_MID_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbMidInterfaceTN;
      #define OPTION_GNBMIDINTERFACETN_ENTRY              {AMD_FAMILY_TN, GnbMidInterfaceTN, TpGnbMidInterface},
    #else
      #define OPTION_GNBMIDINTERFACETN_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_POST_INIT
      #define OPTION_GFX_CONFIG_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxConfigMidInterface;
      #define OPTION_GFXCONFIGMIDINTERFACE_ENTRY         {AMD_FAMILY_TN | AMD_FAMILY_KB, GfxConfigMidInterface, TpGnbGfxConfigMidInterface},
    #else
      #define OPTION_GFXCONFIGMIDINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #if (OPTION_NB_MID_INIT == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GnbMidInterfaceKB;
      #define OPTION_GNBMIDINTERFACEKB_ENTRY              {AMD_FAMILY_KB, GnbMidInterfaceKB, TpGnbMidInterface},
    #else
      #define OPTION_GNBMIDINTERFACEKB_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_MAXPAYLOAD_INTERFACE
      #define OPTION_PCIE_MAXPAYLOAD_INTERFACE TRUE
    #endif
    #if (OPTION_PCIE_MAXPAYLOAD_INTERFACE == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieMaxPayloadInterface;
      #define OPTION_PCIEMAXPAYLOADINTERFACE_ENTRY        {AMD_FAMILY_TN | AMD_FAMILY_KB, PcieMaxPayloadInterface, TpGnbPcieMaxPayloadInterface},
    #else
      #define OPTION_PCIEMAXPAYLOADINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CLK_PM_INTERFACE
      #define OPTION_PCIE_CLK_PM_INTERFACE FALSE
      #if (GNB_TYPE_TN == TRUE  &&  (OPTION_FS1_SOCKET_SUPPORT == TRUE || OPTION_FP1_SOCKET_SUPPORT == TRUE))
        #undef  OPTION_PCIE_CLK_PM_INTERFACE
        #define OPTION_PCIE_CLK_PM_INTERFACE TRUE
      #endif
      #if (GNB_TYPE_KB == TRUE)
        #undef  OPTION_PCIE_CLK_PM_INTERFACE
        #define OPTION_PCIE_CLK_PM_INTERFACE TRUE
      #endif
    #endif

    #if (OPTION_PCIE_CLK_PM_INTERFACE == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieClkPmInterface;
      #define OPTION_PCIECLKPMINTERFACE_ENTRY             {AMD_FAMILY_TN | AMD_FAMILY_KB, PcieClkPmInterface, TpGnbPcieClkPmInterface},
    #else
      #define OPTION_PCIECLKPMINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_ASPM_INTERFACE
      #define OPTION_PCIE_ASPM_INTERFACE TRUE
    #endif
    #if (OPTION_PCIE_ASPM_INTERFACE == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  PcieAspmInterface;
      #define OPTION_PCIEASPMINTERFACE_ENTRY              {AMD_FAMILY_TN | AMD_FAMILY_KB, PcieAspmInterface, TpGnbPcieAspmInterface},
    #else
      #define OPTION_PCIEASPMINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GNB_IOAPIC_INTERFACE
      #define OPTION_GNB_IOAPIC_INTERFACE TRUE
    #endif
    #if (OPTION_GNB_IOAPIC_INTERFACE == TRUE) && (GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GnbNbIoapicInterface;
      #define OPTION_GNBNBIOAPICINTERFACE_ENTRY          {AMD_FAMILY_KB, GnbNbIoapicInterface, TpGnbNbIoapicInterface},
    #else
      #define OPTION_GNBNBIOAPICINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    CONST OPTION_GNB_CONFIGURATION  GnbMidFeatureTable[] = {
      OPTION_GFXCONFIGMIDINTERFACE_ENTRY
      OPTION_GFXMIDINTERFACETN_ENTRY
      OPTION_GFXMIDINTERFACEKB_ENTRY
      OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY
      OPTION_GFXINTINFOTABLEINTERFACEKB_ENTRY
      OPTION_PCIEMIDINTERFACETN_ENTRY
      OPTION_PCIEMIDINTERFACEKB_ENTRY
      OPTION_GNBMIDINTERFACETN_ENTRY
      OPTION_GNBMIDINTERFACEKB_ENTRY
      OPTION_PCIEMAXPAYLOADINTERFACE_ENTRY
      OPTION_PCIECLKPMINTERFACE_ENTRY
      OPTION_PCIEASPMINTERFACE_ENTRY
      OPTION_GNBNBIOAPICINTERFACE_ENTRY
      {0, NULL, EndGnbTestPoints}
    };
  #endif

  #if (AGESA_ENTRY_INIT_LATE == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_ALIB
      #define OPTION_ALIB FALSE
    #endif
    #if (OPTION_ALIB == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      #define ALIB_CALL_TABLE
      #define ALIB_CALL_TABLEV2
      #if (GNB_TYPE_TN == TRUE)
        #if ((OPTION_FM2_SOCKET_SUPPORT == TRUE) || (OPTION_FM2r2_SOCKET_SUPPORT == TRUE))
          extern F_ALIB_UPDATE PcieAlibUpdatePcieMmioInfo;
          extern F_ALIB_GET PcieAlibGetBaseTableTNFM2;
          F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTableTNFM2;
          #undef ALIB_CALL_TABLE
          #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo,
        #else
          extern F_ALIB_UPDATE PcieAlibUpdatePcieMmioInfo;
          extern F_ALIB_GET PcieAlibGetBaseTableTNFS1;
          F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTableTNFS1;
          extern F_ALIB_UPDATE PcieAlibUpdateVoltageInfo;
          extern F_ALIB_UPDATE PcieAlibUpdatePcieInfo;
          #undef ALIB_CALL_TABLE
          #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo, \
                                  PcieAlibUpdateVoltageInfo, \
                                  PcieAlibUpdatePcieInfo,

        #endif
      #endif


      #if (GNB_TYPE_KB == TRUE)
        extern F_ALIB_GET PcieAlibGetBaseTableKB;
        F_ALIB_GET  * CONST AlibGetBaseTableV2 = PcieAlibGetBaseTableKB;
        extern F_ALIB_UPDATE PcieAlibUpdateVoltageData;
        extern F_ALIB_UPDATE PcieAlibUpdatePcieData;
        #undef ALIB_CALL_TABLEV2
        #define ALIB_CALL_TABLEV2 PcieAlibUpdateVoltageData, \
                                  PcieAlibUpdatePcieData,
      #endif


      F_ALIB_UPDATE* CONST AlibDispatchTable [] = {
        ALIB_CALL_TABLE
        NULL
      };
      F_ALIB_UPDATE* CONST AlibDispatchTableV2 [] = {
        ALIB_CALL_TABLEV2
        NULL
      };
      #if (GNB_TYPE_TN == TRUE)
        OPTION_GNB_FEATURE                                  PcieAlibFeature;
        #define OPTION_PCIEALIBFEATURE_ENTRY                {AMD_FAMILY_TN, PcieAlibFeature, TpGnbPcieAlibFeature},
      #endif
      #if ((GNB_TYPE_KB == TRUE))
        OPTION_GNB_FEATURE                                  PcieAlibV2Feature;
        #define OPTION_PCIEALIBV2FEATURE_ENTRY              {AMD_FAMILY_KB, PcieAlibV2Feature, TpGnbPcieAlibFeature},
      #endif
    #else
      F_ALIB_GET  * CONST AlibGetBaseTable = NULL;
      F_ALIB_GET  * CONST AlibGetBaseTableV2 = NULL;
      F_ALIB_UPDATE* CONST AlibDispatchTable [] = {
        NULL
      };
      F_ALIB_UPDATE* CONST AlibDispatchTableV2 [] = {
        NULL
      };
      #define OPTION_PCIEALIBFEATURE_ENTRY
      #define OPTION_PCIEALIBV2FEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_IOMMU_ACPI_IVRS
      #if (CFG_IOMMU_SUPPORT == TRUE)
        #define OPTION_IOMMU_ACPI_IVRS TRUE
      #else
        #define OPTION_IOMMU_ACPI_IVRS FALSE
      #endif
    #endif
    #if (OPTION_IOMMU_ACPI_IVRS == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbIommuIvrsTable;
      #define OPTIONIOMMUACPIIVRSLATE_ENTRY               {AMD_FAMILY_TN, GnbIommuIvrsTable},
    #else
      #define OPTIONIOMMUACPIIVRSLATE_ENTRY
    #endif
    #if (CFG_IOMMU_SUPPORT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbIommuScratchMemoryRangeInterface;
      #define OPTIONIOMMUSCRATCHMEMORYLATE_ENTRY          {AMD_FAMILY_TN, GnbIommuScratchMemoryRangeInterface, TpGnbIommuIvrsTable},
    #else
      #define OPTIONIOMMUSCRATCHMEMORYLATE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    CONST OPTION_GNB_CONFIGURATION  GnbLateFeatureTable[] = {
      #if (GNB_TYPE_TN == TRUE)
        OPTION_PCIEALIBFEATURE_ENTRY
      #endif
      #if ((GNB_TYPE_KB == TRUE))
        OPTION_PCIEALIBV2FEATURE_ENTRY
      #endif
      OPTIONIOMMUSCRATCHMEMORYLATE_ENTRY
      OPTIONIOMMUACPIIVRSLATE_ENTRY
      {0, NULL, EndGnbTestPoints}
    };
  #endif

  #if (AGESA_ENTRY_INIT_S3SAVE == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_INIT_SVIEW
      #define OPTION_GFX_INIT_SVIEW FALSE
    #endif
    #if (OPTION_GFX_INIT_SVIEW == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
      OPTION_GNB_FEATURE                                  GfxInitSview;
      #define OPTION_GFXINITSVIEW_ENTRY                   {AMD_FAMILY_TN | AMD_FAMILY_KB, GfxInitSview},
    #else
      #define OPTION_GFXINITSVIEW_ENTRY
    #endif

    CONST OPTION_GNB_CONFIGURATION  GnbS3SaveFeatureTable[] = {
      OPTION_GFXINITSVIEW_ENTRY
      {0, NULL, EndGnbTestPoints}
    };
  #endif

  #if  (GNB_TYPE_TN == TRUE || GNB_TYPE_KB == TRUE)
    #define GNBS3RESTOREV4
    #define GNBS3RESTOREV7
    #if (GNB_TYPE_TN == TRUE)
      S3_DISPATCH_FUNCTION  GnbSmuServiceRequestV4S3Script;
      #undef  GNBS3RESTOREV4
      #define GNBS3RESTOREV4 {GnbSmuServiceRequestV4S3Script_ID, GnbSmuServiceRequestV4S3Script},
    #endif
    #if (GNB_TYPE_KB == TRUE)
      S3_DISPATCH_FUNCTION  GnbSmuServiceRequestV7S3Script;
      #undef  GNBS3RESTOREV7
      #define GNBS3RESTOREV7 {GnbSmuServiceRequestV7S3Script_ID, GnbSmuServiceRequestV7S3Script},
    #endif
    S3_DISPATCH_FUNCTION  GnbLibStallS3Script;
    #define PCIELATERESTORETN
    #define PCIELATERESTOREKB
    #define GFXSCLKRESTORETN
    #if (GNB_TYPE_TN == TRUE)
      S3_DISPATCH_FUNCTION  PcieLateRestoreInitTNS3Script;
      S3_DISPATCH_FUNCTION  GfxRequestSclkTNS3Script;
      #undef  PCIELATERESTORETN
      #define PCIELATERESTORETN {PcieLateRestoreTNS3Script_ID, PcieLateRestoreInitTNS3Script},
      #undef  GFXSCLKRESTORETN
      #define GFXSCLKRESTORETN  {GfxRequestSclkTNS3Script_ID,  GfxRequestSclkTNS3Script     },
    #endif
    #if (GNB_TYPE_KB == TRUE)
      S3_DISPATCH_FUNCTION  PcieLateRestoreInitKBS3Script;
      #undef  PCIELATERESTOREKB
      #define PCIELATERESTOREKB {PcieLateRestoreKBS3Script_ID, PcieLateRestoreInitKBS3Script},
    #endif
    #define GNB_S3_DISPATCH_FUNCTION_TABLE \
    GNBS3RESTOREV4      \
    GNBS3RESTOREV7      \
    PCIELATERESTORETN \
    GFXSCLKRESTORETN  \
    PCIELATERESTOREKB \
    {GnbLibStallS3Script_ID, GnbLibStallS3Script},



#endif

#endif
#endif  // _OPTION_GNB_INSTALL_H_
