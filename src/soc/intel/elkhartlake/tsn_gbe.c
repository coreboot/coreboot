/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mdio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/lpss.h>
#include <soc/soc_chip.h>
#include <soc/tsn_gbe.h>
#include <timer.h>

static void program_mac_address(struct device *dev, void *base)
{
	enum cb_err status;
	uint8_t mac[MAC_ADDR_LEN];

	/* Check first whether there is a valid MAC address available */
	status = mainboard_get_mac_address(dev, mac);
	if (status != CB_SUCCESS) {
		printk(BIOS_INFO, "TSN GbE: No valid MAC address found\n");
		return;
	}

	printk(BIOS_DEBUG, "TSN GbE: Programming MAC Address...\n");

	/* Write the upper 16 bits of the first 6-byte MAC address */
	clrsetbits32(base + TSN_MAC_ADD0_HIGH, 0xffff, ((mac[5] << 8) | mac[4]));
	/* Write the lower 32 bits of the first 6-byte MAC address */
	clrsetbits32(base + TSN_MAC_ADD0_LOW, 0xffffffff,
			(mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0]);
}


static void tsn_set_phy2mac_irq_polarity(struct device *dev, enum tsn_phy_irq_polarity pol)
{
	uint16_t gcr_reg;
	const struct mdio_bus_operations *mdio_ops;

	mdio_ops = dev_get_mdio_ops(dev);
	if (!mdio_ops)
		return;

	if (pol == RISING_EDGE) {
		/* Read TSN adhoc PHY sublayer register - global configuration register */
		gcr_reg = mdio_ops->read(dev, TSN_MAC_MDIO_ADHOC_ADR, TSN_MAC_MDIO_GCR);
		gcr_reg |= TSN_MAC_PHY2MAC_INTR_POL;
		mdio_ops->write(dev, TSN_MAC_MDIO_ADHOC_ADR, TSN_MAC_MDIO_GCR, gcr_reg);
	}
}

static void gbe_tsn_enable(struct device *dev)
{
	/* Ensure controller is in D0 state. */
	lpss_set_power_state(PCI_DEV(0, PCI_SLOT(dev->path.pci.devfn),
			PCI_FUNC(dev->path.pci.devfn)), STATE_D0);
}

static void gbe_tsn_init(struct device *dev)
{
	/* Get the base address of the I/O registers in memory space */
	struct resource *gbe_tsn_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	void *io_mem_base = (void *)(uintptr_t)gbe_tsn_res->base;
	config_t *config = config_of(dev);

	/* Program MAC address */
	program_mac_address(dev, io_mem_base);

	/* Set PHY-to-MAC IRQ polarity according to devicetree */
	switch (dev->path.pci.devfn) {
	case PCH_DEVFN_GBE:
		tsn_set_phy2mac_irq_polarity(dev, config->pch_tsn_phy_irq_edge);
		break;
	case PCH_DEVFN_PSEGBE0:
		tsn_set_phy2mac_irq_polarity(dev, config->pse_tsn_phy_irq_edge[0]);
		break;
	case PCH_DEVFN_PSEGBE1:
		tsn_set_phy2mac_irq_polarity(dev, config->pse_tsn_phy_irq_edge[1]);
		break;
	}
}

static enum cb_err phy_gmii_ready(void *base)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, GMII_TIMEOUT_MS);
	do {
		if (!(read32((base + MAC_MDIO_ADR)) & MAC_GMII_BUSY))
			return CB_SUCCESS;
		mdelay(1);
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "%s Timeout after %lld msec\n", __func__,
			stopwatch_duration_msecs(&sw));
	return CB_ERR;
}

static uint16_t tsn_mdio_read(struct device *dev, uint8_t phy_adr, uint8_t reg_adr)
{
	uint16_t data = 0;
	struct resource *gbe_tsn_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	void *mmio_base = res2mmio(gbe_tsn_res, 0, 0);

	if (!mmio_base)
		return data;

	clrsetbits32(mmio_base + MAC_MDIO_ADR, MAC_MDIO_ADR_MASK,
			MAC_PHYAD(phy_adr) | MAC_REGAD(reg_adr)
			| MAC_CLK_TRAIL_4 | MAC_CSR_CLK_DIV_102
			| MAC_OP_CMD_READ | MAC_GMII_BUSY);

	/* Wait for MDIO frame transfer to complete before reading MDIO DATA register. */
	if (phy_gmii_ready(mmio_base) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s TSN GMII busy. PHY Adr: 0x%x, Reg 0x%x\n",
				__func__, phy_adr, reg_adr);
	} else {
		data = read16(mmio_base + MAC_MDIO_DATA);
		printk(BIOS_SPEW, "%s PHY Adr: 0x%x, Reg: 0x%x , Data: 0x%x\n",
				__func__, phy_adr, reg_adr, data);
	}
	return data;
}

static void tsn_mdio_write(struct device *dev, uint8_t phy_adr, uint8_t reg_adr, uint16_t data)
{
	struct resource *gbe_tsn_res = find_resource(dev, PCI_BASE_ADDRESS_0);
	void *mmio_base = res2mmio(gbe_tsn_res, 0, 0);

	if (!mmio_base)
		return;

	write16(mmio_base + MAC_MDIO_DATA, data);
	clrsetbits32(mmio_base + MAC_MDIO_ADR, MAC_MDIO_ADR_MASK,
			MAC_PHYAD(phy_adr) | MAC_REGAD(reg_adr)
			| MAC_CLK_TRAIL_4 | MAC_CSR_CLK_DIV_102
			| MAC_OP_CMD_WRITE | MAC_GMII_BUSY);

	/* Wait for MDIO frame transfer to complete before exit. */
	if (phy_gmii_ready(mmio_base) != CB_SUCCESS)
		printk(BIOS_ERR, "%s TSN GMII busy. PHY Adr: 0x%x, Reg 0x%x\n",
				__func__, phy_adr, reg_adr);
	else
		printk(BIOS_SPEW, "%s PHY Adr: 0x%x, Reg: 0x%x , Data: 0x%x\n",
				__func__, phy_adr, reg_adr, data);
}

static struct mdio_bus_operations mdio_ops = {
	.read			= tsn_mdio_read,
	.write			= tsn_mdio_write,
};

static struct device_operations gbe_tsn_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.scan_bus         = scan_generic_bus,
	.enable           = gbe_tsn_enable,
	.init             = gbe_tsn_init,
	.ops_mdio         = &mdio_ops,
};

static const unsigned short gbe_tsn_device_ids[] = {
	PCI_DID_INTEL_EHL_GBE_HOST,
	PCI_DID_INTEL_EHL_GBE_PSE_0,
	PCI_DID_INTEL_EHL_GBE_PSE_1,
	0
};

static const struct pci_driver gbe_tsn_driver __pci_driver = {
	.ops     = &gbe_tsn_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = gbe_tsn_device_ids,
};
