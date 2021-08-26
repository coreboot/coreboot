/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/gpio.h>
#include <soc/qcom_qmp_phy.h>
#include <soc/pcie.h>
#include <timer.h>

#define ROOT_PORT_BDF		0x0
#define ATU_CTRL2		PCIE_ATU_UNR_REGION_CTRL2

static struct qcom_pcie_cntlr_t qcom_pcie_cfg;

static inline void dw_pcie_dbi_rd_wr(bool enable)
{
	uint32_t val;
	pcie_cntlr_cfg_t *pcierc = qcom_pcie_cfg.cntlr_cfg;

	val = read32(pcierc->dbi_base + PCIE_DBI_MISC_CONTROL_1_OFF);

	if (enable)
		val |= PCIE_DBI_RO_WR_EN;
	else
		val &= ~PCIE_DBI_RO_WR_EN;

	write32(pcierc->dbi_base + PCIE_DBI_MISC_CONTROL_1_OFF, val);
}

static void dw_pcie_writel_iatu(unsigned int index, uint32_t reg, uint32_t val)
{
	pcie_cntlr_cfg_t *pcierc = qcom_pcie_cfg.cntlr_cfg;
	unsigned int offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	write32(pcierc->atu_base + offset + reg, val);
}

static uint32_t dw_pcie_readl_iatu(unsigned int index, uint32_t reg)
{
	pcie_cntlr_cfg_t *pcierc = qcom_pcie_cfg.cntlr_cfg;
	unsigned int offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	return read32(pcierc->atu_base + offset + reg);
}

static void qcom_dw_pcie_prog_outbound_atu(unsigned int index,
					   unsigned int type, uint64_t cpu_addr,
					   uint64_t pcie_addr, uint32_t size)
{
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_LOWER_BASE, lower_32_bits(cpu_addr));
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_UPPER_BASE, upper_32_bits(cpu_addr));
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_LIMIT, lower_32_bits(cpu_addr + size - 1));
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_LOWER_TARGET, lower_32_bits(pcie_addr));
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_UPPER_TARGET, upper_32_bits(pcie_addr));
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_REGION_CTRL1, type);
	dw_pcie_writel_iatu(index, PCIE_ATU_UNR_REGION_CTRL2, PCIE_ATU_ENABLE);

	/* Ensure ATU enable takes effect before any subsequent config access */
	if (retry(LINK_WAIT_MAX_IATU_RETRIES,
		  (dw_pcie_readl_iatu(index, ATU_CTRL2) & PCIE_ATU_ENABLE),
		  udelay(LINK_WAIT_IATU)))
		return;

	printk(BIOS_ERR, "PCIe o/b iATU couldn't be enabled after 5ms\n");
}

static void qcom_pcie_configure_gpios(pcie_cntlr_cfg_t *cfg)
{
	gpio_configure(cfg->perst, 0, GPIO_NO_PULL, GPIO_16MA, GPIO_OUTPUT);
}

/**
 * qcom_dw_pcie_configure() - Configure link capabilities and speed
 *
 * Configure the link capabilities and speed in the PCIe root complex.
 */
static void qcom_dw_pcie_configure(uint32_t cap_speed)
{
	pcie_cntlr_cfg_t *pcierc = qcom_pcie_cfg.cntlr_cfg;

	dw_pcie_dbi_rd_wr(true);

	clrsetbits32(pcierc->dbi_base + PCIE_LINK_CAPABILITY,
		     TARGET_LINK_SPEED_MASK, cap_speed);

	clrsetbits32(pcierc->dbi_base + PCIE_LINK_CTL_2,
		     TARGET_LINK_SPEED_MASK, cap_speed);

	dw_pcie_dbi_rd_wr(false);
	printk(BIOS_INFO, "PCIe Link speed configured in Gen %d\n", cap_speed);
}

/**
 * is_pcie_link_up() - Return the link state
 *
 * Return: true for active link and false for no link
 */
static bool is_pcie_link_up(struct qcom_pcie_cntlr_t *pci)
{
	/* Read link status register */
	return !!(read32(pci->cntlr_cfg->elbi + PCIE3X2_ELBI_SYS_STTS) & XMLH_LINK_UP);
}

/**
 * wait_link_up() - Wait for the link to come up
 *
 * Return: true for active line and false for no link (timeout)
 */
static bool wait_link_up(struct qcom_pcie_cntlr_t *pci)
{
	/* Check if the link is up or not */
	if (retry(LINK_WAIT_MAX_RETRIES, is_pcie_link_up(pci), mdelay(PCIE_LINK_UP_MS)))
		return true;

	printk(BIOS_ERR, "PCIe link is not up even after 1sec\n");
	return false;
}

static enum cb_err qcom_pcie_dw_link_up(struct qcom_pcie_cntlr_t *pcie)
{
	if (is_pcie_link_up(pcie)) {
		printk(BIOS_INFO, "PCIe Link is already up\n");
		return CB_SUCCESS;
	}

	/* DW pre link configurations */
	qcom_dw_pcie_configure(LINK_SPEED_GEN_2);

	/* enable link training */
	setbits32(pcie->cntlr_cfg->parf + PCIE_PARF_LTSSM, LTSSM_EN);

	/* Check that link was established */
	if (wait_link_up(pcie)) {
		printk(BIOS_INFO, "PCIe link is up\n");
		return CB_SUCCESS;
	}

	/*
	 * Link can be established in Gen 1 as it failed to establish in Gen2.
	 * So allow some time to do it.
	 */
	udelay(100);

	return CB_ERR;
}

/**
 * Returns root port config space address or endpoint config space address.
 * For endpoint config address, mapping would be done with ATU.
 */
static void *qcom_dw_pcie_get_config_addr(struct qcom_pcie_cntlr_t *pcierc,
					  pci_devfn_t dev)
{
	unsigned int atu_type, cfg_size;
	void *cfg_address;
	uint32_t bus, busdev, devfn;
	pcie_cntlr_cfg_t *cntlr = pcierc->cntlr_cfg;
	bus = PCI_DEV2SEGBUS(dev);
	devfn = PCI_DEV2DEVFN(dev);

	busdev = PCIE_ATU_BUS(bus) |
		 PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		 PCIE_ATU_FUNC(PCI_FUNC(devfn));

	/* Accessing root port configuration space */
	if (!bus && !devfn) {
		cfg_address = pcierc->cntlr_cfg->dbi_base;
		return cfg_address;
	}

	/* For local bus use CFG0 type */
	atu_type = (bus == 1) ? PCIE_ATU_TYPE_CFG0 : PCIE_ATU_TYPE_CFG1;

	cfg_address = cntlr->cfg_base + (cntlr->cfg_size * devfn);
	cfg_size = cntlr->cfg_size;

	qcom_dw_pcie_prog_outbound_atu(PCIE_ATU_REGION_INDEX1, atu_type,
				       (uint64_t)cfg_address, busdev, cfg_size);
	return cfg_address;
}

static void qcom_dw_pcie_setup_rc(struct qcom_pcie_cntlr_t *pcie)
{
	uint32_t val;
	pcie_cntlr_cfg_t *pcierc = pcie->cntlr_cfg;
	/*
	 * Enable DBI read-only registers for writing/updating configuration.
	 * Write permission gets disabled towards the end of this function.
	 */
	dw_pcie_dbi_rd_wr(true);

	val = read32(pcierc->dbi_base + PCIE_PORT_LINK_CONTROL);
	/* Set the number of lanes */
	val &= ~PORT_LINK_MODE_MASK;

	switch (pcierc->lanes) {
	case 1:
		val |= PORT_LINK_MODE_1_LANES;
		break;
	case 2:
		val |= PORT_LINK_MODE_2_LANES;
		break;
	case 4:
		val |= PORT_LINK_MODE_4_LANES;
		break;
	case 8:
		val |= PORT_LINK_MODE_8_LANES;
		break;
	default:
		printk(BIOS_INFO, "PCIe num-lanes %u: invalid value\n",
		       pcierc->lanes);
		return;
	}

	write32(pcierc->dbi_base + PCIE_PORT_LINK_CONTROL, val);

	/* Set link width speed control register */
	val = read32(pcierc->dbi_base + PCIE_LINK_WIDTH_SPEED_CONTROL);
	val &= ~PORT_LOGIC_LINK_WIDTH_MASK;

	switch (pcierc->lanes) {
	case 1:
		val |= PORT_LOGIC_LINK_WIDTH_1_LANES;
		break;
	case 2:
		val |= PORT_LOGIC_LINK_WIDTH_2_LANES;
		break;
	case 4:
		val |= PORT_LOGIC_LINK_WIDTH_4_LANES;
		break;
	case 8:
		val |= PORT_LOGIC_LINK_WIDTH_8_LANES;
		break;
	default:
		printk(BIOS_INFO, "PCIe invalid lanes option\n");
		return;
	}

	write32(pcierc->dbi_base + PCIE_LINK_WIDTH_SPEED_CONTROL, val);

	/* Setup bus numbers */
	clrsetbits32(pcierc->dbi_base + PCI_PRIMARY_BUS, TYPE1_HDR_BUS_NUM_MASK,
		     ROOT_PORT_BUS_NUM);

	/* Disable SMMU */
	write32(pcierc->parf + PCIE_PARF_BDF_TO_SID_CFG, BDF_TO_SID_BYPASS);

	/* Configure ATU for outbound accesses */
	qcom_dw_pcie_prog_outbound_atu(PCIE_ATU_REGION_INDEX0,
				       PCIE_ATU_TYPE_MEM,
				       pcierc->mem.phys_start,
				       ROOT_PORT_BDF, pcierc->mem.size);

	/* Program correct class for RC */
	write16(pcierc->dbi_base + PCI_CLASS_DEVICE, PCI_CLASS_BRIDGE_PCI);

	/* Disable write permission right after the update */
	dw_pcie_dbi_rd_wr(false);
}

static void qcom_qmp_phy_config_lane(void *base, const struct qcom_qmp_phy_init_tbl tbl[],
				     unsigned int num, uint8_t ln_mask)
{
	unsigned int i;
	const struct qcom_qmp_phy_init_tbl *t = tbl;

	for (i = 0; i < num; i++, t++) {
		if (!(t->lane_mask & ln_mask))
			continue;
		write32(base + t->offset, t->val);
	}
}

static void qcom_qmp_phy_configure(void *base, const struct qcom_qmp_phy_init_tbl tbl[],
				   unsigned int num)
{
	qcom_qmp_phy_config_lane(base, tbl, num, 0xff);
}

static enum cb_err qcom_qmp_phy_power_on(pcie_qmp_phy_cfg_t *qphy)
{
	uint64_t lock_us;

	/* Release powerdown mode and allow endpoint refclk drive */
	write32(qphy->pcs + QPHY_PCS_PWR_DWN_CNTRL, SW_PWRDN | REFCLK_DRV_DSBL);

	/* Serdes configuration */
	qcom_qmp_phy_configure(qphy->serdes, qphy->serdes_tbl, qphy->serdes_tbl_num);

	/* Tx, Rx, and PCS configurations */
	qcom_qmp_phy_config_lane(qphy->tx0, qphy->tx_tbl, qphy->tx_tbl_num, 1);
	qcom_qmp_phy_config_lane(qphy->tx0, qphy->tx_tbl_sec, qphy->tx_tbl_num_sec, 1);
	qcom_qmp_phy_config_lane(qphy->tx1, qphy->tx_tbl, qphy->tx_tbl_num, 2);
	qcom_qmp_phy_config_lane(qphy->tx1, qphy->tx_tbl_sec, qphy->tx_tbl_num_sec, 2);
	qcom_qmp_phy_config_lane(qphy->rx0, qphy->rx_tbl, qphy->rx_tbl_num, 1);
	qcom_qmp_phy_config_lane(qphy->rx0, qphy->rx_tbl_sec, qphy->rx_tbl_num_sec, 1);
	qcom_qmp_phy_config_lane(qphy->rx1, qphy->rx_tbl, qphy->rx_tbl_num, 2);
	qcom_qmp_phy_config_lane(qphy->rx1, qphy->rx_tbl_sec, qphy->rx_tbl_num_sec, 2);
	qcom_qmp_phy_configure(qphy->pcs, qphy->pcs_tbl, qphy->pcs_tbl_num);
	qcom_qmp_phy_configure(qphy->pcs, qphy->pcs_tbl_sec, qphy->pcs_tbl_num_sec);
	qcom_qmp_phy_configure(qphy->pcs_misc, qphy->pcs_misc_tbl, qphy->pcs_misc_tbl_num);
	qcom_qmp_phy_configure(qphy->pcs_misc, qphy->pcs_misc_tbl_sec, qphy->pcs_tbl_num_sec);

	/* Release software reset of PCS/Serdes */
	clrbits32(qphy->pcs + QPHY_SW_RESET, SW_RESET);

	/* start PCS/Serdes to operation mode */
	write32(qphy->pcs  + QPHY_START_CTRL, PCS_START | SERDES_START);

	/*
	 * Wait for PHY initialization to be done
	 * PCS_STATUS: wait for 1ms for PHY STATUS bit to be set
	 */
	lock_us = wait_us(1000, !(read32(qphy->qmp_phy_base + QPHY_PCS_STATUS) & PHY_STATUS));
	if (!lock_us) {
		printk(BIOS_ERR, "PCIe QMP PHY PLL failed to lock in 1ms\n");
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "PCIe QPHY Initialized took %lldus\n", lock_us);

	qcom_dw_pcie_enable_pipe_clock();

	return CB_SUCCESS;
}

/*
 * Reset the PCIe PHY core.
 * Allow 100us delay to ensure reset is asserted.
 */
static void qcom_qmp_reset_phy(struct qcom_pcie_cntlr_t *pcie)
{
	clock_reset_bcr(pcie->cntlr_cfg->qmp_phy_bcr, 1);
	udelay(100);
	clock_reset_bcr(pcie->cntlr_cfg->qmp_phy_bcr, 0);
}

/*
 * Reset the PCIe controller core.
 * Allow 100us delay to ensure reset is asserted.
 */
static void qcom_dw_pcie_reset_core(struct qcom_pcie_cntlr_t *pcie)
{
	clock_reset_bcr(pcie->cntlr_cfg->pcie_bcr, 1);
	udelay(100);
	clock_reset_bcr(pcie->cntlr_cfg->pcie_bcr, 0);
}

static enum cb_err qcom_dw_pcie_enable(struct qcom_pcie_cntlr_t *pcie)
{
	int ret;

	pcie_cntlr_cfg_t *pcierc = pcie->cntlr_cfg;
	pcie_qmp_phy_cfg_t *qmp_phy = pcie->qmp_phy_cfg;

	/* assert PCIe reset link to keep EP in reset */
	gpio_set(pcierc->perst, 0);

	/* Enable PCIe controller and SoC specific clocks */
	ret = qcom_dw_pcie_enable_clock();
	if (ret) {
		printk(BIOS_ERR, "Enable PCIe clocks failed\n");
		return ret;
	}

	/* Reset the controller */
	qcom_dw_pcie_reset_core(pcie);

	/* configure PCIe to RC mode */
	write32(pcierc->parf + PCIE_PARF_DEVICE_TYPE, DEVICE_TYPE_RC);

	/* Power on the PHY */
	clrbits32(pcierc->parf + PCIE_PARF_PHY_CTRL, PHY_PWR_DOWN);

	/* MAC PHY_POWERDOWN MUX DISABLE */
	clrbits32(pcierc->parf + PCIE_PARF_SYS_CTRL, MAC_PHY_PWRDOWN_MUX_EN);

	/* Bypass MHI as its not needed */
	setbits32(pcierc->parf + PCIE_PARF_MHI_CLOCK_RESET_CTRL, MHI_BYPASS);

	qcom_qmp_reset_phy(pcie);

	/* Initialize QMP PHY */
	ret = qcom_qmp_phy_power_on(qmp_phy);
	if (ret) {
		printk(BIOS_ERR, "PCIe PHY init failed\n");
		return ret;
	}

	qcom_dw_pcie_setup_rc(pcie);

	/* de-assert PCIe reset link to bring EP out of reset */
	gpio_set(pcierc->perst, 1);

	/* establisth the link */
	ret = qcom_pcie_dw_link_up(pcie);
	if (ret) {
		printk(BIOS_ERR, "PCIe Link init failed\n");
		return ret;
	}

	return CB_SUCCESS;
}

/**
 * Fill coreboot table with PCIe info.
 * It allows exporting this info to payloads.
 */
enum cb_err lb_fill_pcie(struct lb_pcie *pcie)
{
	pcie_cntlr_cfg_t *pcierc = qcom_pcie_cfg.cntlr_cfg;
	pcie->ctrl_base = (uintptr_t)pcierc->dbi_base;
	return CB_SUCCESS;
}

/* map_bus function for mapping pcie_s_{read/write}_configXX() functions */
volatile union pci_bank *pci_map_bus(pci_devfn_t dev)
{
	void *config_addr = NULL;

	config_addr = qcom_dw_pcie_get_config_addr(&qcom_pcie_cfg, dev);
	return (void *)config_addr;
}

/* PCI domain ops read_resources callback */
void qcom_pci_domain_read_resources(struct device *dev)
{
	struct resource *res;
	pcie_cntlr_cfg_t *pcierc = qcom_pcie_cfg.cntlr_cfg;

	/* Initialize the system-wide I/O space constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = pcierc->io.phys_start;
	res->limit = pcierc->io.size - 1;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system-wide memory resources constraints. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = pcierc->mem.phys_start;
	res->limit = pcierc->mem.size - 1;
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

/* PCI domain ops enable callback */
void qcom_setup_pcie_host(struct device *dev)
{
	gcom_pcie_get_config(&qcom_pcie_cfg);

	/* Ensure PCIe endpoints are powered-on before initiating PCIe link training */
	gcom_pcie_power_on_ep();

	printk(BIOS_INFO, "Setup PCIe in RC mode\n");

	/* Configure PERST gpio */
	qcom_pcie_configure_gpios(qcom_pcie_cfg.cntlr_cfg);

	if (!qcom_dw_pcie_enable(&qcom_pcie_cfg))
		printk(BIOS_NOTICE, "PCIe enumerated succussfully..\n");
	else
		printk(BIOS_EMERG, "Failed to enable PCIe\n");

}
