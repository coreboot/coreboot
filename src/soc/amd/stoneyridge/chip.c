/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <amdblocks/i2c.h>
#include <amdblocks/post_codes.h>

#include "chip.h"

struct device_operations stoneyridge_cpu_bus_ops = {
	.read_resources	  = noop_read_resources,
	.set_resources	  = noop_set_resources,
	.init		  = mp_cpu_bus_init,
	.acpi_fill_ssdt   = generate_cpu_entries,
};

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			/* Root Hub */
			return "RHUB";
		case 2:
			/* USB2 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "HS01";
			case 1: return "HS02";
			case 2: return "HS03";
			case 3: return "HS04";
			case 4: return "HS05";
			case 5: return "HS06";
			case 6: return "HS07";
			case 7: return "HS08";
			}
			break;
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "SS01";
			case 1: return "SS02";
			case 2: return "SS03";
			}
			break;
		}
		return NULL;
	}

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case GFX_DEVFN:
		return "IGFX";
	case PCIE0_DEVFN:
		return "PBR4";
	case PCIE1_DEVFN:
		return "PBR5";
	case PCIE2_DEVFN:
		return "PBR6";
	case PCIE3_DEVFN:
		return "PBR7";
	case PCIE4_DEVFN:
		return "PBR8";
	case EHCI1_DEVFN:
		return "EHC0";
	case SD_DEVFN:
		return "SDCN";
	case XHCI_DEVFN:
		return "XHC0";
	default:
		return NULL;
	}
};

struct device_operations stoneyridge_pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.enable_resources = domain_enable_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name	  = soc_acpi_name,
};

static void soc_init(void *chip_info)
{
	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
	fch_final(chip_info);
	fam15_finalize(chip_info);
}

struct chip_operations soc_amd_stoneyridge_ops = {
	CHIP_NAME("AMD StoneyRidge SOC")
	.init = soc_init,
	.final = soc_final
};

static void earliest_ramstage(void *unused)
{
	if (!acpi_is_wakeup_s3()) {
		post_code(POST_PSP_LOAD_SMU);
		if (CONFIG(SOC_AMD_PSP_SELECTABLE_SMU_FW))
			psp_load_named_blob(BLOB_SMU_FW2, "smu_fw2");

		post_code(POST_AGESA_AMDINITENV);
		do_agesawrapper(AMD_INIT_ENV, "amdinitenv");
	} else {
		/* Complete the initial system restoration */
		post_code(POST_AGESA_AMDS3LATERESTORE);
		do_agesawrapper(AMD_S3LATE_RESTORE, "amds3laterestore");
	}
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, earliest_ramstage, NULL);
