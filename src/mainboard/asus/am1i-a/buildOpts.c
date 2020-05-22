/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/agesa/f16kb/AGESA.h>

/* Include the files that instantiate the configuration definitions. */
#include <vendorcode/amd/agesa/f16kb/Include/AdvancedApi.h>
#include <vendorcode/amd/agesa/f16kb/Include/GnbInterface.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/cpuFamilyTranslation.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/cpuRegisters.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/Family/cpuFamRegisters.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/Feature/cpuFeatures.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/Table.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/heapManager.h>
/* AGESA nonsense: the next three headers depend on heapManager.h */
#include <vendorcode/amd/agesa/f16kb/Proc/Common/CreateStruct.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/cpuEarlyInit.h>
#include <vendorcode/amd/agesa/f16kb/Proc/CPU/cpuLateInit.h>

/* Select the CPU family */
#define INSTALL_FAMILY_16_MODEL_0x_SUPPORT	TRUE

/* Select the CPU socket type */
#define INSTALL_FT3_SOCKET_SUPPORT		TRUE

//#define BLDOPT_REMOVE_UDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_RDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_LRDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_ECC_SUPPORT		TRUE
//#define BLDOPT_REMOVE_SRAT			FALSE
#define BLDOPT_REMOVE_WHEA			FALSE
#define BLDOPT_REMOVE_CRAT			TRUE
#define BLDOPT_REMOVE_CDIT			TRUE

/* Build configuration values here. */
#define BLDCFG_AMD_PLATFORM_TYPE		AMD_PLATFORM_DESKTOP

#define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT	DDR1600_FREQUENCY
#define BLDCFG_MEMORY_RDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_UDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE		FALSE
#define BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING	FALSE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING	FALSE
#define BLDCFG_MEMORY_CLOCK_SELECT		DDR1333_FREQUENCY /* FIXME: Turtle RAM? */
#define BLDCFG_IGNORE_SPD_CHECKSUM		TRUE
#define BLDCFG_ENABLE_ECC_FEATURE		FALSE
#define BLDCFG_ECC_SYNC_FLOOD			FALSE

/*
 * Specify the default values for the VRM controlling the VDDNB plane.
 * If not specified, the values used for the core VRM will be applied
 */
#define BLDCFG_VRM_NB_LOW_POWER_THRESHOLD	0

#define BLDCFG_IOMMU_SUPPORT			FALSE

#define BLDCFG_CFG_GNB_HD_AUDIO			TRUE

/* AGESA nonsense: this header depends on the definitions above */
#include <PlatformInstall.h>
