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
 * @e \$Revision: 64464 $   @e \$Date: 2012-01-21 11:28:59 -0600 (Sat, 21 Jan 2012) $
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
 *
 ***************************************************************************/

#ifndef _OPTION_GNB_INSTALL_H_
#define _OPTION_GNB_INSTALL_H_

#include <Proc/Common/S3SaveState.h>
/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition
 */

//---------------------------------------------------------------------------------------------------
// Family installation
//---------------------------------------------------------------------------------------------------



#define GNB_TYPE_TN   FALSE
#define GNB_TYPE_LN   FALSE
#define GNB_TYPE_ON   FALSE

#if (OPTION_FAMILY14H_ON == TRUE)
  #undef  GNB_TYPE_ON
  #define GNB_TYPE_ON  TRUE
#endif

#if (OPTION_FAMILY12H_LN == TRUE)
  #undef  GNB_TYPE_LN
  #define GNB_TYPE_LN  TRUE
#endif

#if (OPTION_FAMILY15H_TN == TRUE)
  #undef  GNB_TYPE_TN
  #define GNB_TYPE_TN  TRUE
#endif



#if (GNB_TYPE_TN == TRUE  || GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
//---------------------------------------------------------------------------------------------------
// Service installation
//---------------------------------------------------------------------------------------------------

  #include <Proc/GNB/Common/Gnb.h>
  #include <Proc/GNB/Common/GnbPcie.h>
  #include <Proc/GNB/Common/GnbGfx.h>

  #define SERVICES_POINTER  NULL
  #if (GNB_TYPE_TN == TRUE)
    #include <Proc/GNB/Modules/GnbInitTN/GnbInitTNInstall.h>
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
    #define CFG_LCLK_DPM_EN                               TRUE
  #endif

  #ifndef CFG_GMC_POWER_GATING
    #if (GNB_TYPE_TN == TRUE)
      #define CFG_GMC_POWER_GATING                          GmcPowerGatingWidthStutter
    #else
      #define CFG_GMC_POWER_GATING                          GmcPowerGatingWidthStutter
    #endif
  #endif

  #ifndef CFG_SMU_SCLK_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE)
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
    #define CFG_GNB_PCIE_TRAINING_ALGORITHM               PcieTrainingStandard
  #endif

  #ifndef CFG_GNB_FORCE_CABLESAFE_OFF
    #define CFG_GNB_FORCE_CABLESAFE_OFF                   FALSE
  #endif

  #ifndef CFG_ORB_CLOCK_GATING_ENABLE
    #define CFG_ORB_CLOCK_GATING_ENABLE                   TRUE
  #endif

  #ifndef CFG_GNB_PCIE_POWERGATING_FLAGS
    #define CFG_GNB_PCIE_POWERGATING_FLAGS                0x0
  #endif

  #ifndef CFG_IOC_LCLK_CLOCK_GATING_ENABLE
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_IOC_LCLK_CLOCK_GATING_ENABLE           TRUE
    #else
      #define CFG_IOC_LCLK_CLOCK_GATING_ENABLE            FALSE
    #endif
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
    #if (GNB_TYPE_TN == TRUE)
      #define CFG_GNB_BAPM_SUPPORT                        TRUE
    #else
      #define CFG_GNB_BAPM_SUPPORT                        FALSE
    #endif
  #endif

  #ifndef CFG_UNUSED_SIMD_POWERGATING_ENABLE
    #define CFG_UNUSED_SIMD_POWERGATING_ENABLE            TRUE
  #endif

  #ifndef CFG_UNUSED_RB_POWERGATING_ENABLE
    #define CFG_UNUSED_RB_POWERGATING_ENABLE              FALSE
  #endif

  #ifndef CFG_NBDPM_ENABLE
    #define CFG_NBDPM_ENABLE                              TRUE
  #endif

  #ifndef CFG_MAX_PAYLOAD_ENABLE
    #define CFG_MAX_PAYLOAD_ENABLE                        TRUE
  #endif

  #ifndef CFG_GMC_CLOCK_GATING
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_GMC_CLOCK_GATING                       TRUE
    #else
      #define CFG_GMC_CLOCK_GATING                        TRUE
    #endif
  #endif

  #ifndef CFG_ORB_DYN_WAKE_ENABLE
    #if (GNB_TYPE_TN == TRUE)
      #define  CFG_ORB_DYN_WAKE_ENABLE                    TRUE
    #else
      #define CFG_ORB_DYN_WAKE_ENABLE                     TRUE
    #endif
  #endif

  #ifndef CFG_LOADLINE_ENABLE
    #define CFG_LOADLINE_ENABLE                           TRUE
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
    CFG_IOC_LCLK_CLOCK_GATING_ENABLE,
    CFG_IOC_SCLK_CLOCK_GATING_ENABLE,
    CFG_IOMMU_L1_CLOCK_GATING_ENABLE,
    CFG_IOMMU_L2_CLOCK_GATING_ENABLE,
    CFG_GNB_ALTVDDNB_SUPPORT,
    CFG_GNB_BAPM_SUPPORT,
    CFG_UNUSED_SIMD_POWERGATING_ENABLE,
    CFG_UNUSED_RB_POWERGATING_ENABLE,
    CFG_NBDPM_ENABLE,
    CFG_GMC_CLOCK_GATING,
    CFG_MAX_PAYLOAD_ENABLE,
    CFG_ORB_DYN_WAKE_ENABLE,
    CFG_LOADLINE_ENABLE
  };

  //---------------------------------------------------------------------------------------------------
  // Module entries
  //---------------------------------------------------------------------------------------------------

  #if (AGESA_ENTRY_INIT_EARLY == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_EARLY_INIT
      #define OPTION_NB_EARLY_INIT TRUE
    #endif
    #if (OPTION_NB_EARLY_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  NbInitAtEarly;
      #define OPTION_NBINITATEARLY_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON , NbInitAtEarly},
    #else
      #define OPTION_NBINITATEARLY_ENTRY
    #endif
    #if (OPTION_NB_EARLY_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbEarlyInterfaceTN;
      #define OPTION_GNBEARLYINTERFACETN_ENTRY            {AMD_FAMILY_TN, GnbEarlyInterfaceTN},
    #else
      #define OPTION_GNBEARLYINTERFACETN_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
  // SMU init
    #ifndef OPTION_SMU
      #define OPTION_SMU  TRUE
    #endif
    #if (OPTION_SMU == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  F12NbSmuInitFeature;
      #define OPTION_F12NBSMUINITFEATURE_ENTRY            {AMD_FAMILY_LN, F12NbSmuInitFeature},
    #else
      #define OPTION_F12NBSMUINITFEATURE_ENTRY
    #endif
    #if (OPTION_SMU == TRUE) && (GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  F14NbSmuInitFeature;
      #define OPTION_F14NBSMUINITFEATURE_ENTRY            {AMD_FAMILY_ON, F14NbSmuInitFeature},
    #else
      #define OPTION_F14NBSMUINITFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CONFIG_MAP
      #define OPTION_PCIE_CONFIG_MAP TRUE
    #endif
    #if (OPTION_PCIE_CONFIG_MAP == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                 PcieConfigurationMap;
      #define OPTION_PCIECONFIGURATIONMAP_ENTRY          {AMD_FAMILY_LN | AMD_FAMILY_ON  | AMD_FAMILY_TN , PcieConfigurationMap},
    #else
      #define OPTION_PCIECONFIGURATIONMAP_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_EARLY_INIT
      #define OPTION_PCIE_EARLY_INIT TRUE
    #endif
    #if (OPTION_PCIE_EARLY_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  PcieInitAtEarly;
      #define OPTION_PCIEINITATEARLY_ENTRY                {AMD_FAMILY_LN | AMD_FAMILY_ON , PcieInitAtEarly},
    #else
      #define  OPTION_PCIEINITATEARLY_ENTRY
    #endif
    #if (OPTION_PCIE_EARLY_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PcieEarlyInterfaceTN;
      #define OPTION_PCIEEARLYINTERFACETN_ENTRY           {AMD_FAMILY_TN, PcieEarlyInterfaceTN},
    #else
      #define  OPTION_PCIEEARLYINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    CONST OPTION_GNB_CONFIGURATION  GnbEarlyFeatureTable[] = {
      OPTION_NBINITATEARLY_ENTRY
      OPTION_GNBEARLYINTERFACETN_ENTRY
      OPTION_F12NBSMUINITFEATURE_ENTRY
      OPTION_F14NBSMUINITFEATURE_ENTRY
      OPTION_PCIECONFIGURATIONMAP_ENTRY
      OPTION_PCIEINITATEARLY_ENTRY
      OPTION_PCIEEARLYINTERFACETN_ENTRY
      {0, NULL}
    };

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CONFIG_INIT
      #define OPTION_PCIE_CONFIG_INIT TRUE
    #endif
    #if (OPTION_PCIE_CONFIG_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                  PcieConfigurationInit;
      #define OPTION_PCIECONFIGURATIONINIT_ENTRY          {AMD_FAMILY_LN | AMD_FAMILY_ON | AMD_FAMILY_TN , PcieConfigurationInit},
    #else
      #define OPTION_PCIECONFIGURATIONINIT_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_EARLIER_INIT
      #define OPTION_NB_EARLIER_INIT TRUE
    #endif
    #if (OPTION_NB_EARLIER_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbEarlierInterfaceTN;
      #define OPTION_GNBEARLIERINTERFACETN_ENTRY          {AMD_FAMILY_TN, GnbEarlierInterfaceTN},
    #else
      #define OPTION_GNBEARLIERINTERFACETN_ENTRY
    #endif

    CONST OPTION_GNB_CONFIGURATION  GnbEarlierFeatureTable[] = {
      OPTION_PCIECONFIGURATIONINIT_ENTRY
      OPTION_GNBEARLIERINTERFACETN_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_POST == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_POST_INIT
      #define OPTION_GFX_CONFIG_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                  GfxConfigPostInterface;
      #define OPTION_GFXCONFIGPOSTINTERFACE_ENTRY         {AMD_FAMILY_LN | AMD_FAMILY_ON  | AMD_FAMILY_TN , GfxConfigPostInterface},
    #else
      #define OPTION_GFXCONFIGPOSTINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_POST_INIT
      #define OPTION_GFX_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  GfxInitAtPost;
      #define OPTION_GFXINITATPOST_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON , GfxInitAtPost},
    #else
      #define  OPTION_GFXINITATPOST_ENTRY
    #endif
    #if (OPTION_GFX_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxPostInterfaceTN;
      #define OPTION_GFXPOSTINTERFACETN_ENTRY             {AMD_FAMILY_TN, GfxPostInterfaceTN},
    #else
      #define OPTION_GFXPOSTINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_POST_INIT
      #define OPTION_NB_POST_INIT TRUE
    #endif
    #if (OPTION_NB_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  NbInitAtPost;
      #define OPTION_NBINITATPOST_ENTRY                   {AMD_FAMILY_LN | AMD_FAMILY_ON , NbInitAtPost},
    #else
      #define OPTION_NBINITATPOST_ENTRY
    #endif
    #if (OPTION_NB_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbPostInterfaceTN;
      #define OPTION_GNBPOSTINTERFACETN_ENTRY             {AMD_FAMILY_TN, GnbPostInterfaceTN},
    #else
      #define OPTION_GNBPOSTINTERFACETN_ENTRY
    #endif

    //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POST_EALRY_INIT
      #define OPTION_PCIE_POST_EALRY_INIT TRUE
    #endif
    #if (OPTION_PCIE_POST_EALRY_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  PcieInitAtPostEarly;
    #define OPTION_PCIEINITATPOSTEARLY_ENTRY              {AMD_FAMILY_LN | AMD_FAMILY_ON , PcieInitAtPostEarly},
    #else
      #define OPTION_PCIEINITATPOSTEARLY_ENTRY
    #endif
    #if (OPTION_PCIE_POST_EALRY_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PciePostEarlyInterfaceTN;
      #define OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY       {AMD_FAMILY_TN, PciePostEarlyInterfaceTN},
    #else
      #define OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POST_INIT
      #define OPTION_PCIE_POST_INIT TRUE
    #endif
    #if (OPTION_PCIE_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  PcieInitAtPost;
      #define OPTION_PCIEINITATPOST_ENTRY                 {AMD_FAMILY_LN | AMD_FAMILY_ON , PcieInitAtPost},
    #else
      #define OPTION_PCIEINITATPOST_ENTRY
    #endif
    #if (OPTION_PCIE_POST_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PciePostInterfaceTN;
      #define OPTION_PCIEPOSTINTERFACETN_ENTRY            {AMD_FAMILY_TN, PciePostInterfaceTN},
    #else
      #define OPTION_PCIEPOSTINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    CONST OPTION_GNB_CONFIGURATION  GnbPostFeatureTable[] = {
      OPTION_PCIEINITATPOSTEARLY_ENTRY
      OPTION_PCIEPOSTEARLYINTERFACETN_ENTRY
      OPTION_GFXCONFIGPOSTINTERFACE_ENTRY
      OPTION_GFXINITATPOST_ENTRY
      OPTION_GFXPOSTINTERFACETN_ENTRY
      {0, NULL}
    };

    CONST OPTION_GNB_CONFIGURATION  GnbPostAfterDramFeatureTable[] = {
      OPTION_NBINITATPOST_ENTRY
      OPTION_GNBPOSTINTERFACETN_ENTRY
      OPTION_PCIEINITATPOST_ENTRY
      OPTION_PCIEPOSTINTERFACETN_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_ENV == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_FUSE_TABLE_INIT
      #define OPTION_FUSE_TABLE_INIT TRUE
    #endif
    #if (OPTION_FUSE_TABLE_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  NbFuseTableFeature;
      #define OPTION_NBFUSETABLEFEATURE_ENTRY             {AMD_FAMILY_LN | AMD_FAMILY_ON , NbFuseTableFeature},
    #else
      #define OPTION_NBFUSETABLEFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_ENV_INIT
      #define OPTION_NB_ENV_INIT TRUE
    #endif
    #if (OPTION_NB_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  NbInitAtEnv;
      #define OPTION_NBINITATENVT_ENTRY                   {AMD_FAMILY_LN | AMD_FAMILY_ON , NbInitAtEnv},
    #else
      #define OPTION_NBINITATENVT_ENTRY
    #endif
    #if (OPTION_NB_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbEnvInterfaceTN;
      #define OPTION_GNBENVINTERFACETN_ENTRY              {AMD_FAMILY_TN, GnbEnvInterfaceTN},
    #else
      #define OPTION_GNBENVINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_ENV_INIT
      #define OPTION_GFX_CONFIG_ENV_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                   GfxConfigEnvInterface;
      #define OPTION_GFXCONFIGENVINTERFACE_ENTRY           {AMD_FAMILY_LN | AMD_FAMILY_ON  | AMD_FAMILY_TN, GfxConfigEnvInterface},
    #else
      #define  OPTION_GFXCONFIGENVINTERFACE_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_ENV_INIT
      #define OPTION_GFX_ENV_INIT TRUE
    #endif
    #if (OPTION_GFX_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  GfxInitAtEnvPost;
      #define OPTION_GFXINITATENVPOST_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON , GfxInitAtEnvPost},
    #else
      #define OPTION_GFXINITATENVPOST_ENTRY
    #endif
    #if (OPTION_GFX_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxEnvInterfaceTN;
      #define OPTION_GFXENVINTERFACETN_ENTRY              {AMD_FAMILY_TN, GfxEnvInterfaceTN},
    #else
      #define OPTION_GFXENVINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_POWER_GATE
      #define OPTION_POWER_GATE TRUE
    #endif
    #if (OPTION_POWER_GATE == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  F12NbPowerGateFeature;
      #define OPTION_F12NBPOWERGATEFEATURE_ENTRY          {AMD_FAMILY_LN, F12NbPowerGateFeature},
    #else
      #define OPTION_F12NBPOWERGATEFEATURE_ENTRY
    #endif
    #if (OPTION_POWER_GATE == TRUE) && (GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  F14NbPowerGateFeature;
      #define OPTION_F14NBPOWERGATEFEATURE_ENTRY          {AMD_FAMILY_ON, F14NbPowerGateFeature},
    #else
      #define OPTION_F14NBPOWERGATEFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_ENV_INIT
      #define OPTION_PCIE_ENV_INIT TRUE
    #endif
    #if (OPTION_PCIE_ENV_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  PcieInitAtEnv;
      #define OPTION_PCIEINITATENV_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON , PcieInitAtEnv},
    #else
      #define OPTION_PCIEINITATENV_ENTRY
    #endif
    #if (OPTION_PCIE_ENV_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE PcieEnvInterfaceTN;
      #define OPTION_PCIEENVINTERFACETN_ENTRY             {AMD_FAMILY_TN, PcieEnvInterfaceTN},
    #else
      #define OPTION_PCIEENVINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------

    OPTION_GNB_CONFIGURATION  GnbEnvFeatureTable[] = {
      OPTION_NBFUSETABLEFEATURE_ENTRY
      OPTION_NBINITATENVT_ENTRY
      OPTION_GNBENVINTERFACETN_ENTRY
      OPTION_PCIEINITATENV_ENTRY
      OPTION_PCIEENVINTERFACETN_ENTRY
      OPTION_GFXCONFIGENVINTERFACE_ENTRY
      OPTION_GFXINITATENVPOST_ENTRY
      OPTION_GFXENVINTERFACETN_ENTRY
      OPTION_F12NBPOWERGATEFEATURE_ENTRY
      OPTION_F14NBPOWERGATEFEATURE_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_MID == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GNB_CABLESAFE
      #define OPTION_GNB_CABLESAFE TRUE
    #endif
    #if (OPTION_GNB_CABLESAFE == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  GnbCableSafeEntry;
      #define OPTION_GNBCABLESAFEENTRY_ENTRY              {AMD_FAMILY_LN, GnbCableSafeEntry},
    #else
      #define OPTION_GNBCABLESAFEENTRY_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_LCLK_NCLK_RATIO
      #define OPTION_NB_LCLK_NCLK_RATIO TRUE
    #endif
    #if (OPTION_NB_LCLK_NCLK_RATIO == TRUE) && (GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  F14NbLclkNclkRatioFeature;
      #define OPTION_F14NBLCLKNCLKRATIOFEATURE_ENTRY      {AMD_FAMILY_ON, F14NbLclkNclkRatioFeature},
    #else
      #define OPTION_F14NBLCLKNCLKRATIOFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_LCLK_DPM_INIT
      #define OPTION_NB_LCLK_DPM_INIT TRUE
    #endif
    #if (OPTION_NB_LCLK_DPM_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  NbLclkDpmFeature;
      #define OPTION_NBLCLKDPMFEATURE_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON , NbLclkDpmFeature},
    #else
      #define OPTION_NBLCLKDPMFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_POWER_GATE
      #define OPTION_PCIE_POWER_GATE TRUE
    #endif
    #if (OPTION_PCIE_POWER_GATE == TRUE) && (GNB_TYPE_LN == TRUE)
      OPTION_GNB_FEATURE                                  PciePowerGateFeature;
      #define OPTION_PCIEPOWERGATEFEATURE_ENTRY           {AMD_FAMILY_LN, PciePowerGateFeature},
    #else
      #define OPTION_PCIEPOWERGATEFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_MID_INIT
      #define OPTION_GFX_MID_INIT TRUE
    #endif
    #if (OPTION_GFX_MID_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  GfxInitAtMidPost;
      #define OPTION_GFXINITATMIDPOST_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON , GfxInitAtMidPost},
    #else
      #define OPTION_GFXINITATMIDPOST_ENTRY
    #endif
    #if (OPTION_GFX_MID_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxMidInterfaceTN;
      #define OPTION_GFXMIDINTERFACETN_ENTRY              {AMD_FAMILY_TN, GfxMidInterfaceTN},
    #else
      #define OPTION_GFXMIDINTERFACETN_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_INTEGRATED_TABLE_INIT
      #define OPTION_GFX_INTEGRATED_TABLE_INIT TRUE
    #endif
    #if (OPTION_GFX_INTEGRATED_TABLE_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                 GfxIntegratedInfoTableEntry;
      #define OPTION_GFXINTEGRATEDINFOTABLE_ENTRY        {AMD_FAMILY_LN | AMD_FAMILY_ON , GfxIntegratedInfoTableEntry},
    #else
      #define OPTION_GFXINTEGRATEDINFOTABLE_ENTRY
    #endif
    #if (OPTION_GFX_INTEGRATED_TABLE_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GfxIntInfoTableInterfaceTN;
      #define OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY     {AMD_FAMILY_TN, GfxIntInfoTableInterfaceTN},
    #else
      #define OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIe_MID_INIT
      #define OPTION_PCIe_MID_INIT TRUE
    #endif
    #if (OPTION_PCIe_MID_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  PcieInitAtMid;
      #define OPTION_PCIEINITATMID_ENTRY                  {AMD_FAMILY_LN | AMD_FAMILY_ON , PcieInitAtMid},
    #else
      #define OPTION_PCIEINITATMID_ENTRY
    #endif
    #if (OPTION_PCIe_MID_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  PcieMidInterfaceTN;
      #define OPTION_PCIEMIDINTERFACETN_ENTRY             {AMD_FAMILY_TN, PcieMidInterfaceTN},
    #else
      #define OPTION_PCIEMIDINTERFACETN_ENTRY
    #endif

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_NB_MID_INIT
      #define OPTION_NB_MID_INIT TRUE
    #endif
    #if (OPTION_NB_MID_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  NbInitAtLatePost;
      #define OPTION_NBINITATLATEPOST_ENTRY               {AMD_FAMILY_LN | AMD_FAMILY_ON , NbInitAtLatePost},
    #else
      #define OPTION_NBINITATLATEPOST_ENTRY
    #endif
    #if (OPTION_NB_MID_INIT == TRUE) && (GNB_TYPE_TN == TRUE)
      OPTION_GNB_FEATURE                                  GnbMidInterfaceTN;
      #define OPTION_GNBMIDINTERFACETN_ENTRY              {AMD_FAMILY_TN, GnbMidInterfaceTN},
    #else
      #define OPTION_GNBMIDINTERFACETN_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_CONFIG_POST_INIT
      #define OPTION_GFX_CONFIG_POST_INIT TRUE
    #endif
    #if (OPTION_GFX_CONFIG_POST_INIT == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_TN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  GfxConfigMidInterface;
      #define OPTION_GFXCONFIGMIDINTERFACE_ENTRY          {AMD_FAMILY_LN | AMD_FAMILY_TN | GNB_TYPE_ON, GfxConfigMidInterface},
    #else
      #define OPTION_GFXCONFIGMIDINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_CLK_PM_INTERFACE
      #define OPTION_PCIE_CLK_PM_INTERFACE FALSE
      #if (GNB_TYPE_ON == TRUE )
        #undef  OPTION_PCIE_CLK_PM_INTERFACE
        #define OPTION_PCIE_CLK_PM_INTERFACE TRUE
      #endif
      #if (GNB_TYPE_TN == TRUE  &&  (OPTION_FS1_SOCKET_SUPPORT == TRUE || OPTION_FP1_SOCKET_SUPPORT == TRUE))
        #undef  OPTION_PCIE_CLK_PM_INTERFACE
        #define OPTION_PCIE_CLK_PM_INTERFACE TRUE
      #endif
    #endif

    #if (OPTION_PCIE_CLK_PM_INTERFACE == TRUE) && (GNB_TYPE_TN == TRUE || GNB_TYPE_ON == TRUE )
      OPTION_GNB_FEATURE                                  PcieClkPmInterface;
      #define OPTION_PCIECLKPMINTERFACE_ENTRY             {AMD_FAMILY_TN  | AMD_FAMILY_ON, PcieClkPmInterface},
    #else
      #define OPTION_PCIECLKPMINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_PCIE_ASPM_INTERFACE
      #define OPTION_PCIE_ASPM_INTERFACE TRUE
    #endif
    #if (OPTION_PCIE_ASPM_INTERFACE == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                  PcieAspmInterface;
      #define OPTION_PCIEASPMINTERFACE_ENTRY              {AMD_FAMILY_LN | AMD_FAMILY_ON  | AMD_FAMILY_TN , PcieAspmInterface},
    #else
      #define OPTION_PCIEASPMINTERFACE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GNB_IOAPIC_INTERFACE
      #define OPTION_GNB_IOAPIC_INTERFACE TRUE
    #endif
  //---------------------------------------------------------------------------------------------------
    OPTION_GNB_CONFIGURATION  GnbMidFeatureTable[] = {
      OPTION_GFXCONFIGMIDINTERFACE_ENTRY
      OPTION_GFXINITATMIDPOST_ENTRY
      OPTION_GFXMIDINTERFACETN_ENTRY
      OPTION_GFXINTEGRATEDINFOTABLE_ENTRY
      OPTION_GFXINTINFOTABLEINTERFACETN_ENTRY
      OPTION_GNBCABLESAFEENTRY_ENTRY
      OPTION_PCIEINITATMID_ENTRY
      OPTION_PCIEMIDINTERFACETN_ENTRY
      OPTION_NBINITATLATEPOST_ENTRY
      OPTION_GNBMIDINTERFACETN_ENTRY
      OPTION_F14NBLCLKNCLKRATIOFEATURE_ENTRY
      OPTION_NBLCLKDPMFEATURE_ENTRY
      OPTION_PCIEPOWERGATEFEATURE_ENTRY
      OPTION_PCIECLKPMINTERFACE_ENTRY
      OPTION_PCIEASPMINTERFACE_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_LATE == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_ALIB
      #define OPTION_ALIB FALSE
    #endif
    #if (OPTION_ALIB == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE  || GNB_TYPE_TN == TRUE)
      extern F_ALIB_UPDATE PcieAlibUpdatePcieMmioInfo;
      #if (GNB_TYPE_LN == TRUE)
        #if (OPTION_FM1_SOCKET_SUPPORT == TRUE)
          extern F_ALIB_GET PcieAlibGetBaseTableLNFM1;
          F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTableLNFM1;
          #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo
        #else
          extern F_ALIB_GET PcieAlibGetBaseTableLNFS1;
          F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTableLNFS1;
          extern F_ALIB_UPDATE PcieAlibUpdateVoltageInfo;
          extern F_ALIB_UPDATE PcieAlibUpdatePcieInfo;
          extern F_ALIB_UPDATE PcieAlibBuildAcpiTableLNFS1;
          #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo, \
                                  PcieAlibUpdateVoltageInfo, \
                                  PcieAlibUpdatePcieInfo, \
                                  PcieAlibBuildAcpiTableLNFS1
        #endif
      #elif (GNB_TYPE_TN == TRUE)
        #if (OPTION_FM2_SOCKET_SUPPORT == TRUE)
          extern F_ALIB_GET PcieAlibGetBaseTableTNFM2;
          F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTableTNFM2;
          #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo
        #else
          extern F_ALIB_GET PcieAlibGetBaseTableTNFS1;
          F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTableTNFS1;
          extern F_ALIB_UPDATE PcieAlibUpdateVoltageInfo;
          extern F_ALIB_UPDATE PcieAlibUpdatePcieInfo;
          extern F_ALIB_UPDATE PcieAlibBuildAcpiTableLNFS2;
          #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo, \
                                  PcieAlibUpdateVoltageInfo, \
                                  PcieAlibUpdatePcieInfo

        #endif
      #else
        extern F_ALIB_GET PcieAlibGetBaseTable;
        F_ALIB_GET  *AlibGetBaseTable = PcieAlibGetBaseTable;
        extern F_ALIB_UPDATE PcieAlibUpdateVoltageInfo;
        extern F_ALIB_UPDATE PcieAlibUpdatePcieInfo;
        extern F_ALIB_UPDATE PcieFmAlibBuildAcpiTable;
        #define ALIB_CALL_TABLE PcieAlibUpdatePcieMmioInfo, \
                                PcieAlibUpdateVoltageInfo, \
                                PcieAlibUpdatePcieInfo, \
                                PcieFmAlibBuildAcpiTable
      #endif
      F_ALIB_UPDATE* AlibDispatchTable [] = {
        ALIB_CALL_TABLE,
        NULL
      };
      OPTION_GNB_FEATURE                                  PcieAlibFeature;
      #define OPTION_PCIEALIBFEATURE_ENTRY                {AMD_FAMILY_LN | AMD_FAMILY_ON  | AMD_FAMILY_TN, PcieAlibFeature},
    #else
      F_ALIB_GET  *AlibGetBaseTable = NULL;
      F_ALIB_UPDATE* AlibDispatchTable [] = {
        NULL
      };
      #define OPTION_PCIEALIBFEATURE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_IOMMU_ACPI_IVRS
      #if (CFG_IOMMU_SUPPORT == TRUE)
        #define OPTION_IOMMU_ACPI_IVRS TRUE
      #else
        #define OPTION_IOMMU_ACPI_IVRS FALSE
      #endif
    #endif
    #if (OPTION_IOMMU_ACPI_IVRS == TRUE) && (GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                  GnbIommuIvrsTable;
      #define OPTIONIOMMUACPIIVRSLATE_ENTRY               {AMD_FAMILY_TN, GnbIommuIvrsTable},
    #else
      #define OPTIONIOMMUACPIIVRSLATE_ENTRY
    #endif
    #if (CFG_IOMMU_SUPPORT == TRUE) && (GNB_TYPE_TN == TRUE )
      OPTION_GNB_FEATURE                                  GnbIommuScratchMemoryRangeInterface;
      #define OPTIONIOMMUSCRATCHMEMORYLATE_ENTRY          {AMD_FAMILY_TN , GnbIommuScratchMemoryRangeInterface},
    #else
      #define OPTIONIOMMUSCRATCHMEMORYLATE_ENTRY
    #endif
  //---------------------------------------------------------------------------------------------------
    OPTION_GNB_CONFIGURATION  GnbLateFeatureTable[] = {
      OPTION_PCIEALIBFEATURE_ENTRY
      OPTIONIOMMUSCRATCHMEMORYLATE_ENTRY
      OPTIONIOMMUACPIIVRSLATE_ENTRY
      {0, NULL}
    };
  #endif

  #if (AGESA_ENTRY_INIT_S3SAVE == TRUE)
  //---------------------------------------------------------------------------------------------------
    #ifndef OPTION_GFX_INIT_SVIEW
      #define OPTION_GFX_INIT_SVIEW TRUE
    #endif
    #if (OPTION_GFX_INIT_SVIEW == TRUE) && (GNB_TYPE_LN == TRUE || GNB_TYPE_TN == TRUE || GNB_TYPE_ON == TRUE)
      OPTION_GNB_FEATURE                                  GfxInitSview;
      #define OPTION_GFXINITSVIEW_ENTRY                   {AMD_FAMILY_LN | AMD_FAMILY_TN, GfxInitSview},
    #else
      #define OPTION_GFXINITSVIEW_ENTRY
    #endif

    OPTION_GNB_CONFIGURATION  GnbS3SaveFeatureTable[] = {
      OPTION_GFXINITSVIEW_ENTRY
      {0, NULL}
    };
  #endif
  #if  (GNB_TYPE_LN == TRUE || GNB_TYPE_ON == TRUE )
    S3_DISPATCH_FUNCTION  NbSmuServiceRequestS3Script;
    S3_DISPATCH_FUNCTION  PcieLateRestoreS3Script;
    S3_DISPATCH_FUNCTION  NbSmuIndirectWriteS3Script;
    #define GNB_S3_DISPATCH_FUNCTION_TABLE \
    {NbSmuIndirectWriteS3Script_ID, NbSmuIndirectWriteS3Script}, \
    {NbSmuServiceRequestS3Script_ID, NbSmuServiceRequestS3Script}, \
    {PcieLateRestoreS3Script_ID, PcieLateRestoreS3Script},
  #endif


  #if  (GNB_TYPE_TN == TRUE )
    S3_DISPATCH_FUNCTION  GnbSmuServiceRequestV4S3Script;
    S3_DISPATCH_FUNCTION  GnbLibStallS3Script;
    #define PCIELATERESTORETN
    #define GFXSCLKRESTORETN
    #if (GNB_TYPE_TN == TRUE)
      S3_DISPATCH_FUNCTION  PcieLateRestoreInitTNS3Script;
      S3_DISPATCH_FUNCTION  GfxRequestSclkTNS3Script;
      #undef  PCIELATERESTORETN
      #define PCIELATERESTORETN {PcieLateRestoreTNS3Script_ID, PcieLateRestoreInitTNS3Script},
      #undef  GFXSCLKRESTORETN
      #define GFXSCLKRESTORETN  {GfxRequestSclkTNS3Script_ID,  GfxRequestSclkTNS3Script     },
    #endif
    #define GNB_S3_DISPATCH_FUNCTION_TABLE \
    {GnbSmuServiceRequestV4S3Script_ID, GnbSmuServiceRequestV4S3Script}, \
    PCIELATERESTORETN \
    GFXSCLKRESTORETN \
    {GnbLibStallS3Script_ID, GnbLibStallS3Script},
     /*these three line should be 1261-1263*/


  #endif

#endif
#endif  // _OPTION_GNB_INSTALL_H_
