/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/cfg.h>
#include <intelblocks/lpss.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

int dw_i2c_soc_dev_to_bus(const struct device *dev)
{
	pci_devfn_t devfn = dev->path.pci.devfn;
	return dw_i2c_soc_devfn_to_bus(devfn);
}

/* Getting I2C bus configuration from devicetree config */
const struct dw_i2c_bus_config *dw_i2c_get_soc_cfg(unsigned int bus)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return &common_config->i2c[bus];
}

/* Get base address for early init of I2C controllers. */
uintptr_t dw_i2c_get_soc_early_base(unsigned int bus)
{
	return EARLY_I2C_BASE(bus);
}

#if !ENV_PAYLOAD_LOADER
static int lpss_i2c_early_init_bus(unsigned int bus)
{
	const struct dw_i2c_bus_config *config;
	pci_devfn_t dev;
	int devfn;
	uintptr_t base;

	/* Find the PCI device for this bus controller */
	devfn = dw_i2c_soc_bus_to_devfn(bus);
	if (devfn < 0) {
		printk(BIOS_ERR, "I2C%u device not found\n", bus);
		return -1;
	}

	/* Look up the controller device in the devicetree */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Skip if not enabled for early init */
	config = dw_i2c_get_soc_cfg(bus);
	if (!config || !config->early_init) {
		printk(BIOS_DEBUG, "I2C%u not enabled for early init\n", bus);
		return -1;
	}

	/* Prepare early base address for access before memory */
	base = dw_i2c_get_soc_early_base(bus);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, base);
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take device out of reset */
	lpss_reset_release(base);

	/* Ensure controller is in D0 state */
	lpss_set_power_state(dev, STATE_D0);

	/* Initialize the controller */
	if (dw_i2c_init(bus, config) != CB_SUCCESS) {
		printk(BIOS_ERR, "I2C%u failed to initialize\n", bus);
		return -1;
	}

	return 0;
}

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	int devfn;
	pci_devfn_t dev;
	uintptr_t base;

	/* Find device+function for this controller */
	devfn = dw_i2c_soc_bus_to_devfn(bus);
	if (devfn < 0)
		return (uintptr_t)NULL;

	/* Form a PCI address for this device */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Read the first base address for this device */
	base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);

	/* Attempt to initialize bus if base is not set yet */
	if (!base && !lpss_i2c_early_init_bus(bus))
		base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0),
				  16);
	return base;
}
#else

uintptr_t dw_i2c_base_address(unsigned int bus)
{
	int devfn;
	struct device *dev;
	struct resource *res;

	/* bus -> devfn */
	devfn = dw_i2c_soc_bus_to_devfn(bus);

	if (devfn < 0)
		return (uintptr_t)NULL;

	/* devfn -> dev */
	dev = pcidev_path_on_root(devfn);
	if (!dev || !dev->enabled)
		return (uintptr_t)NULL;

	/* dev -> bar0 */
	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res)
		return res->base;

	/* No resource found yet, it's possible this is running in the
	 * PAYLOAD_LOADER stage before resources have been assigned yet,
	 * therefore, any early init BAR should still be valid. */

	/* Read the first base address for this device */
	return (uintptr_t)ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);
}

/*
 * This function ensures that the device is actually out of reset and
 * its ready for initialization sequence.
 */
static void dw_i2c_device_init(struct device *dev)
{
	uintptr_t base_address;
	int bus = dw_i2c_soc_dev_to_bus(dev);

	if (bus < 0)
		return;

	base_address = dw_i2c_base_address(bus);
	if (!base_address)
		return;

	/* Ensure controller is in D0 state */
	lpss_set_power_state(PCI_BDF(dev), STATE_D0);

	/* Take device out of reset if its not done before */
	if (lpss_is_controller_in_reset(base_address))
		lpss_reset_release(base_address);

	dw_i2c_dev_init(dev);
}

static struct device_operations i2c_dev_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_static_bus,
	.ops_i2c_bus		= &dw_i2c_bus_ops,
	.ops_pci		= &pci_dev_ops_pci,
	.init			= dw_i2c_device_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= dw_i2c_acpi_fill_ssdt,
#endif
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_SPT_I2C0,
	PCI_DID_INTEL_SPT_I2C1,
	PCI_DID_INTEL_SPT_I2C2,
	PCI_DID_INTEL_SPT_I2C3,
	PCI_DID_INTEL_SPT_I2C4,
	PCI_DID_INTEL_SPT_I2C5,
	PCI_DID_INTEL_UPT_H_I2C0,
	PCI_DID_INTEL_UPT_H_I2C1,
	PCI_DID_INTEL_UPT_H_I2C2,
	PCI_DID_INTEL_UPT_H_I2C3,
	PCI_DID_INTEL_APL_I2C0,
	PCI_DID_INTEL_APL_I2C1,
	PCI_DID_INTEL_APL_I2C2,
	PCI_DID_INTEL_APL_I2C3,
	PCI_DID_INTEL_APL_I2C4,
	PCI_DID_INTEL_APL_I2C5,
	PCI_DID_INTEL_APL_I2C6,
	PCI_DID_INTEL_APL_I2C7,
	PCI_DID_INTEL_CNL_I2C0,
	PCI_DID_INTEL_CNL_I2C1,
	PCI_DID_INTEL_CNL_I2C2,
	PCI_DID_INTEL_CNL_I2C3,
	PCI_DID_INTEL_CNL_I2C4,
	PCI_DID_INTEL_CNL_I2C5,
	PCI_DID_INTEL_GLK_I2C0,
	PCI_DID_INTEL_GLK_I2C1,
	PCI_DID_INTEL_GLK_I2C2,
	PCI_DID_INTEL_GLK_I2C3,
	PCI_DID_INTEL_GLK_I2C4,
	PCI_DID_INTEL_GLK_I2C5,
	PCI_DID_INTEL_GLK_I2C6,
	PCI_DID_INTEL_GLK_I2C7,
	PCI_DID_INTEL_CNP_H_I2C0,
	PCI_DID_INTEL_CNP_H_I2C1,
	PCI_DID_INTEL_CNP_H_I2C2,
	PCI_DID_INTEL_CNP_H_I2C3,
	PCI_DID_INTEL_ICP_I2C0,
	PCI_DID_INTEL_ICP_I2C1,
	PCI_DID_INTEL_ICP_I2C2,
	PCI_DID_INTEL_ICP_I2C3,
	PCI_DID_INTEL_ICP_I2C4,
	PCI_DID_INTEL_ICP_I2C5,
	PCI_DID_INTEL_CMP_I2C0,
	PCI_DID_INTEL_CMP_I2C1,
	PCI_DID_INTEL_CMP_I2C2,
	PCI_DID_INTEL_CMP_I2C3,
	PCI_DID_INTEL_CMP_I2C4,
	PCI_DID_INTEL_CMP_I2C5,
	PCI_DID_INTEL_CMP_H_I2C0,
	PCI_DID_INTEL_CMP_H_I2C1,
	PCI_DID_INTEL_CMP_H_I2C2,
	PCI_DID_INTEL_CMP_H_I2C3,
	PCI_DID_INTEL_TGP_I2C0,
	PCI_DID_INTEL_TGP_I2C1,
	PCI_DID_INTEL_TGP_I2C2,
	PCI_DID_INTEL_TGP_I2C3,
	PCI_DID_INTEL_TGP_I2C4,
	PCI_DID_INTEL_TGP_I2C5,
	PCI_DID_INTEL_TGP_I2C6,
	PCI_DID_INTEL_TGP_I2C7,
	PCI_DID_INTEL_TGP_H_I2C0,
	PCI_DID_INTEL_TGP_H_I2C1,
	PCI_DID_INTEL_TGP_H_I2C2,
	PCI_DID_INTEL_TGP_H_I2C3,
	PCI_DID_INTEL_TGP_H_I2C4,
	PCI_DID_INTEL_TGP_H_I2C5,
	PCI_DID_INTEL_TGP_H_I2C6,
	PCI_DID_INTEL_MCC_I2C0,
	PCI_DID_INTEL_MCC_I2C1,
	PCI_DID_INTEL_MCC_I2C2,
	PCI_DID_INTEL_MCC_I2C3,
	PCI_DID_INTEL_MCC_I2C4,
	PCI_DID_INTEL_MCC_I2C5,
	PCI_DID_INTEL_MCC_I2C6,
	PCI_DID_INTEL_MCC_I2C7,
	PCI_DID_INTEL_JSP_I2C0,
	PCI_DID_INTEL_JSP_I2C1,
	PCI_DID_INTEL_JSP_I2C2,
	PCI_DID_INTEL_JSP_I2C3,
	PCI_DID_INTEL_JSP_I2C4,
	PCI_DID_INTEL_JSP_I2C5,
	PCI_DID_INTEL_ADP_P_I2C0,
	PCI_DID_INTEL_ADP_P_I2C1,
	PCI_DID_INTEL_ADP_P_I2C2,
	PCI_DID_INTEL_ADP_P_I2C3,
	PCI_DID_INTEL_ADP_P_I2C4,
	PCI_DID_INTEL_ADP_P_I2C5,
	PCI_DID_INTEL_ADP_P_I2C6,
	PCI_DID_INTEL_ADP_P_I2C7,
	PCI_DID_INTEL_ADP_S_I2C0,
	PCI_DID_INTEL_ADP_S_I2C1,
	PCI_DID_INTEL_ADP_S_I2C2,
	PCI_DID_INTEL_ADP_S_I2C3,
	PCI_DID_INTEL_ADP_S_I2C4,
	PCI_DID_INTEL_ADP_S_I2C5,
	PCI_DID_INTEL_ADP_M_N_I2C0,
	PCI_DID_INTEL_ADP_M_N_I2C1,
	PCI_DID_INTEL_ADP_M_N_I2C2,
	PCI_DID_INTEL_ADP_M_N_I2C3,
	PCI_DID_INTEL_ADP_M_N_I2C4,
	PCI_DID_INTEL_ADP_M_N_I2C5,
	0,
};

static const struct pci_driver pch_i2c __pci_driver = {
	.ops	 = &i2c_dev_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
#endif
