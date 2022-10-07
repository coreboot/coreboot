/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
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

enum cb_err phy_gmii_ready(void *base)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, TSN_GMII_TIMEOUT_MS);
	do {
		if (!(read32((base + TSN_MAC_MDIO_ADR)) & TSN_MAC_GMII_BUSY))
			return CB_SUCCESS;

	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "%s Timeout after %lld msec\n", __func__,
			stopwatch_duration_msecs(&sw));
	return CB_ERR;
}

uint16_t tsn_mdio_read(void *base, uint8_t phy_adr, uint8_t reg_adr)
{
	uint16_t data = 0;
	enum cb_err status;

	clrsetbits32(base + TSN_MAC_MDIO_ADR, TSN_MAC_MDIO_ADR_MASK,
			TSN_MAC_PHYAD(phy_adr) | TSN_MAC_REGAD(reg_adr)
			| TSN_MAC_CLK_TRAIL_4 | TSN_MAC_CSR_CLK_DIV_62
			| TSN_MAC_OP_CMD_READ | TSN_MAC_GMII_BUSY);

	/* Wait for MDIO frame transfer to complete before reading MDIO DATA register */
	status = phy_gmii_ready(base);
	if (status == CB_ERR) {
		printk(BIOS_ERR, "%s TSN GMII busy. PHY Adr: 0x%x, Reg 0x%x\n",
				__func__, phy_adr, reg_adr);
	} else {
		data = read16(base + TSN_MAC_MDIO_DATA);
		printk(BIOS_DEBUG, "%s PHY Adr: 0x%x, Reg: 0x%x , Data: 0x%x\n",
				__func__, phy_adr, reg_adr, data);
	}
	return data;
}

void tsn_mdio_write(void *base, uint8_t phy_adr, uint8_t reg_adr, uint16_t data)
{
	enum cb_err status;

	write16(base + TSN_MAC_MDIO_DATA, data);
	clrsetbits32(base + TSN_MAC_MDIO_ADR, TSN_MAC_MDIO_ADR_MASK,
			TSN_MAC_PHYAD(phy_adr) | TSN_MAC_REGAD(reg_adr)
			| TSN_MAC_CLK_TRAIL_4 | TSN_MAC_CSR_CLK_DIV_62
			| TSN_MAC_OP_CMD_WRITE | TSN_MAC_GMII_BUSY);

	/* Wait for MDIO frame transfer to complete before do next */
	status = phy_gmii_ready(base);
	if (status == CB_ERR)
		printk(BIOS_ERR, "%s TSN GMII busy. PHY Adr: 0x%x, Reg 0x%x\n",
				__func__, phy_adr, reg_adr);
	else
		printk(BIOS_DEBUG, "%s PHY Adr: 0x%x, Reg: 0x%x , Data: 0x%x\n",
				__func__, phy_adr, reg_adr, data);
}

static void tsn_set_phy2mac_irq_polarity(void *base, enum tsn_phy_irq_polarity pol)
{
	uint16_t gcr_reg;

	if (pol == RISING_EDGE) {
		/* Read TSN adhoc PHY sublayer register - global configuration register */
		gcr_reg = tsn_mdio_read(base, TSN_MAC_MDIO_ADHOC_ADR, TSN_MAC_MDIO_GCR);
		gcr_reg |= TSN_MAC_PHY2MAC_INTR_POL;
		tsn_mdio_write(base, TSN_MAC_MDIO_ADHOC_ADR, TSN_MAC_MDIO_GCR, gcr_reg);
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
		tsn_set_phy2mac_irq_polarity(io_mem_base, config->pch_tsn_phy_irq_edge);
		break;
	case PCH_DEVFN_PSEGBE0:
		tsn_set_phy2mac_irq_polarity(io_mem_base, config->pse_tsn_phy_irq_edge[0]);
		break;
	case PCH_DEVFN_PSEGBE1:
		tsn_set_phy2mac_irq_polarity(io_mem_base, config->pse_tsn_phy_irq_edge[1]);
		break;
	}
}

static struct device_operations gbe_tsn_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.enable           = gbe_tsn_enable,
	.init             = gbe_tsn_init,
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
