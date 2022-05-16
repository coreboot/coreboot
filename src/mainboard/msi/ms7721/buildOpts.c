/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/agesa/f15tn/AGESA.h>

/* Include the files that instantiate the configuration definitions. */
#include <vendorcode/amd/agesa/f15tn/Include/AdvancedApi.h>
#include <vendorcode/amd/agesa/f15tn/Proc/CPU/cpuFamilyTranslation.h>
#include <vendorcode/amd/agesa/f15tn/Proc/CPU/Feature/cpuFeatures.h>
#include <vendorcode/amd/agesa/f15tn/Proc/CPU/heapManager.h>
/* AGESA nonsense: the next two headers depend on heapManager.h */
#include <vendorcode/amd/agesa/f15tn/Proc/Common/CreateStruct.h>
#include <vendorcode/amd/agesa/f15tn/Proc/CPU/cpuEarlyInit.h>
/* These tables are optional and may be used to adjust memory timing settings */
#include <vendorcode/amd/agesa/f15tn/Proc/Mem/mm.h>
#include <vendorcode/amd/agesa/f15tn/Proc/Mem/mn.h>

/* Select the CPU family */
#define INSTALL_FAMILY_15_MODEL_1x_SUPPORT	TRUE

/* Select the CPU socket type */
#define INSTALL_FM2_SOCKET_SUPPORT		TRUE

//#define BLDOPT_REMOVE_UDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_SODIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_RDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_LRDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_ECC_SUPPORT		TRUE
#define BLDOPT_REMOVE_SRAT			FALSE
#define BLDOPT_REMOVE_WHEA			FALSE
#define BLDOPT_REMOVE_CRAT			TRUE

/* Build configuration values here */
#define BLDCFG_AMD_PLATFORM_TYPE		AMD_PLATFORM_MOBILE

#define BLDCFG_MEMORY_RDIMM_CAPABLE		FALSE
#define BLDCFG_MEMORY_UDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE		FALSE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING	TRUE
#define BLDCFG_MEMORY_CLOCK_SELECT		DDR1600_FREQUENCY
#define BLDCFG_ENABLE_ECC_FEATURE		FALSE
#define BLDCFG_ECC_SYNC_FLOOD			FALSE

#define BLDCFG_UMA_ALLOCATION_MODE		UMA_SPECIFIED
#define BLDCFG_UMA_ALLOCATION_SIZE		0x2000 /* (0x2000 << 16) = 512M */

#define BLDCFG_IOMMU_SUPPORT			TRUE

#define BLDCFG_CFG_GNB_HD_AUDIO			TRUE

/* Customized OEM build configurations for FCH component */
#define BLDCFG_FCH_GPP_LINK_CONFIG		PortA1B1C1D1
#define BLDCFG_FCH_GPP_PORT0_PRESENT		TRUE
#define BLDCFG_FCH_GPP_PORT1_PRESENT		TRUE

CONST GPIO_CONTROL ms7721_m_gpio[] = {
	{-1}
};

#define BLDCFG_FCH_GPIO_CONTROL_LIST		(ms7721_m_gpio)

/*
 * Process the options...
 * This file include MUST occur AFTER the user option selection settings.
 * AGESA nonsense: Moving this include up will break AGESA.
 */
#include <PlatformInstall.h>
