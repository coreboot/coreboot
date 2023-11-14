/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/pci_clk_req.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/platform_descriptors.h>

/* Update gpp glk req config based on DXIO descriptors and enabled devices. */
void pcie_gpp_dxio_update_clk_req_config(enum gpp_clk_req *gpp_clk_config,
					 size_t gpp_clk_config_num)
{
	const fsp_dxio_descriptor *dxio_descs = NULL;
	const fsp_ddi_descriptor *ddi_descs = NULL;
	size_t dxio_num = 0;
	size_t ddi_num = 0;

	mainboard_get_dxio_ddi_descriptors(&dxio_descs, &dxio_num, &ddi_descs, &ddi_num);
	if (dxio_descs == NULL) {
		printk(BIOS_WARNING,
		       "No DXIO descriptors found, GPP clk req may not reflect enabled devices\n");
		return;
	}

	for (int i = 0; i < dxio_num; i++) {
		const fsp_dxio_descriptor *dxio_desc = &dxio_descs[i];

		/*  Only consider PCIe and unused engine types. */
		if (dxio_desc->engine_type != PCIE_ENGINE
		    && dxio_desc->engine_type != UNUSED_ENGINE)
			continue;
		enum cpm_clk_req dxio_clk_req = dxio_desc->clk_req;

		/* CLK_DISABLE means there's no corresponding clk req line in use */
		if (dxio_clk_req == CLK_DISABLE)
			continue;

		/*
		 * dxio_clk_req is only 4 bits so having CLK_ENABLE as a value for
		 * a descriptor should cause a compiler error. 0xF isn't a
		 * valid clk_req value according to AMD's internal code either.
		 * This is here to draw attention in case this code is ever used
		 * in a situation where this has changed.
		 */
		if (dxio_clk_req == (CLK_ENABLE & 0xF)) {
			printk(BIOS_WARNING,
			       "CLK_ENABLE is an invalid clk_req value for PCIe device %d.%d, DXIO descriptor %d\n",
			       dxio_desc->device_number, dxio_desc->function_number, i);
			continue;
		}

		/* cpm_clk_req 0 is CLK_DISABLE */
		int gpp_req_index = dxio_clk_req - CLK_REQ0;
		/* Ensure that our index is valid */
		if (gpp_req_index < 0 || gpp_req_index >= gpp_clk_config_num) {
			printk(BIOS_ERR,
			       "Failed to convert DXIO clk req value %d to GPP clk req index for PCIe device %d.%d, DXIO descriptor %d, clk req settings may be incorrect\n",
			       dxio_clk_req, dxio_desc->device_number,
			       dxio_desc->function_number, i);
			continue;
		}

		const struct device *pci_device = pcidev_path_on_root(
			PCI_DEVFN(dxio_desc->device_number, dxio_desc->function_number));
		if (pci_device == NULL) {
			gpp_clk_config[gpp_req_index] = GPP_CLK_OFF;
			printk(BIOS_WARNING,
			       "Cannot find PCIe device %d.%d, disabling GPP clk req %d, DXIO descriptor %d\n",
			       dxio_desc->device_number, dxio_desc->function_number, i,
			       gpp_req_index);
			continue;
		}

		/* PCIe devices haven't been fully set up yet, so directly read the vendor id
		 * and device id to determine if a device is physically present. If a device
		 * is not present then the id should be 0xffffffff. 0x00000000, 0xffff0000,
		 * and 0x0000ffff are there to account for any odd failure cases. */
		u32 id = pci_read_config32(pci_device, PCI_VENDOR_ID);
		bool enabled = pci_device->enabled && (id != 0xffffffff) && (id != 0x00000000)
			       && (id != 0x0000ffff) && (id != 0xffff0000);

		/* Inform of possible mismatches between devices and SoC gpp_clk_config. */
		if (!enabled && gpp_clk_config[gpp_req_index] != GPP_CLK_OFF) {
			gpp_clk_config[gpp_req_index] = GPP_CLK_OFF;
			printk(BIOS_INFO,
			       "PCIe device %d.%d disabled, disabling GPP clk req %d, DXIO descriptor %d\n",
			       dxio_desc->device_number, dxio_desc->function_number,
			       gpp_req_index, i);
		} else if (enabled && gpp_clk_config[gpp_req_index] == GPP_CLK_OFF) {
			printk(BIOS_INFO,
			       "PCIe device %d.%d enabled, GPP clk req is off, DXIO descriptor %d\n",
			       dxio_desc->device_number, dxio_desc->function_number, i);
		}
	}
}
