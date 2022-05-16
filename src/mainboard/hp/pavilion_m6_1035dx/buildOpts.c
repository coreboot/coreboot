/* SPDX-License-Identifier: GPL-2.0-only */

#include "mainboard.h"

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
#define INSTALL_FS1_SOCKET_SUPPORT		TRUE
#define INSTALL_FP2_SOCKET_SUPPORT		TRUE

//#define BLDOPT_REMOVE_UDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_RDIMMS_SUPPORT		TRUE
#define BLDOPT_REMOVE_LRDIMMS_SUPPORT		TRUE
//#define BLDOPT_REMOVE_ECC_SUPPORT		TRUE
#define BLDOPT_REMOVE_SRAT			FALSE
#define BLDOPT_REMOVE_WHEA			FALSE
#define BLDOPT_REMOVE_CRAT			TRUE

/* Build configuration values here. */
#define BLDCFG_AMD_PLATFORM_TYPE		AMD_PLATFORM_MOBILE

#define BLDCFG_MEMORY_RDIMM_CAPABLE		FALSE
#define BLDCFG_MEMORY_UDIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE		TRUE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING	TRUE
#define BLDCFG_ENABLE_ECC_FEATURE		TRUE
#define BLDCFG_ECC_SYNC_FLOOD			FALSE

#define BLDCFG_UMA_ALLOCATION_MODE		UMA_SPECIFIED
#define BLDCFG_UMA_ALLOCATION_SIZE		0x2000 /* (0x2000 << 16) = 512M */

#define BLDCFG_IOMMU_SUPPORT			TRUE

#define BLDCFG_CFG_GNB_HD_AUDIO			TRUE

/*
 * The GPIO control is not well documented in AGESA, but is in the BKDG
 *
 * Eg. FANIN1/GPIO57 on datasheet means power-on default (Function0) is to route
 * from this ball to hardware monitor as FAN1 tacho input. Selecting Function1
 * routes this to the GPIO block instead. Seems ACPI GPIOs and related GEVENTs
 * are mostly in Function1, sometimes Function2.
 *
 * Note that the GpioOut bit does not mean that the GPIO is an output. That bit
 * actually controls the output value, so GpioOut means "default to set".
 * PullUpB is an inverted logic, so setting this bit means we're actually
 * disabling the internal pull-up. The PullDown bit is NOT inverted logic.
 * The output driver can be disabled with the GpioOutEnB bit, which is again,
 * inverted logic. To make the list more readable, we define a few local macros
 * to state what we mean.
 */
#define OUTPUT_HIGH	(GpioOut)
#define OUTPUT_LOW	(0)
#define INPUT		(GpioOutEnB)
#define PULL_UP		(0)
#define PULL_DOWN	(PullDown | PullUpB)
#define PULL_NONE	(PullUpB)

CONST GPIO_CONTROL pavilion_m6_1035dx_gpio[] = {
	{57, Function1, OUTPUT_HIGH | PULL_NONE},	/* WLAN enable */
	{-1}
};
#define BLDCFG_FCH_GPIO_CONTROL_LIST		(pavilion_m6_1035dx_gpio)

/*
 * These definitions could be moved to a common Hudson header, should we decide
 * to provide our own, saner SCI mapping function
 */
#define GEVENT_PIN(gpe)		((gpe) + 0x40)
#define SCI_MAP_OHCI_12_0	0x58
#define SCI_MAP_OHCI_13_0	0x59
#define SCI_MAP_XHCI_10_0	0x78
#define SCI_MAP_PWRBTN		0x73

CONST SCI_MAP_CONTROL m6_1035dx_sci_map[] = {
	{GEVENT_PIN(EC_SCI_GEVENT), EC_SCI_GPE},
	{GEVENT_PIN(EC_LID_GEVENT), EC_LID_GPE},
	{GEVENT_PIN(PCIE_GEVENT), PCIE_GPE},
	{SCI_MAP_OHCI_12_0, PME_GPE},
	{SCI_MAP_OHCI_13_0, PME_GPE},
	{SCI_MAP_XHCI_10_0, PME_GPE},
	{SCI_MAP_PWRBTN, PME_GPE},
};
#define BLDCFG_FCH_SCI_MAP_LIST			(m6_1035dx_sci_map)

/*
 * Process the options...
 * This file include MUST occur AFTER the user option selection settings.
 * AGESA nonsense: Moving this include up will break AGESA.
 */
#include <PlatformInstall.h>
