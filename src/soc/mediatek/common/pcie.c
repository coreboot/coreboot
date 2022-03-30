/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/stdlib.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/resource.h>
#include <delay.h>
#include <lib.h>
#include <soc/addressmap.h>
#include <soc/early_init.h>
#include <soc/pcie.h>
#include <soc/pcie_common.h>
#include <soc/soc_chip.h>
#include <stdlib.h>
#include <types.h>

#define PCIE_SETTING_REG		0x80
#define PCIE_PCI_IDS_1			0x9c
#define PCI_CLASS(class)		((class) << 8)
#define PCIE_RC_MODE			BIT(0)

#define PCIE_CFGNUM_REG			0x140
#define PCIE_CFG_DEVFN(devfn)		((devfn) & GENMASK(7, 0))
#define PCIE_CFG_BUS(bus)		(((bus) << 8) & GENMASK(15, 8))
#define PCIE_CFG_OFFSET_ADDR		0x1000
#define PCIE_CFG_HEADER(bus, devfn) \
	(PCIE_CFG_BUS(bus) | PCIE_CFG_DEVFN(devfn))

#define PCIE_RST_CTRL_REG		0x148
#define PCIE_MAC_RSTB			BIT(0)
#define PCIE_PHY_RSTB			BIT(1)
#define PCIE_BRG_RSTB			BIT(2)
#define PCIE_PE_RSTB			BIT(3)

#define PCIE_LTSSM_STATUS_REG		0x150
#define PCIE_LTSSM_STATE(val)		(((val) >> 24) & 0x1f)

#define PCIE_LINK_STATUS_REG		0x154
#define PCIE_CTRL_LINKUP		BIT(8)

#define PCI_NUM_INTX			4
#define PCIE_INT_ENABLE_REG		0x180
#define PCIE_INTX_SHIFT			24
#define PCIE_INTX_ENABLE \
	GENMASK(PCIE_INTX_SHIFT + PCI_NUM_INTX - 1, PCIE_INTX_SHIFT)

#define PCIE_TRANS_TABLE_BASE_REG	0x800
#define PCIE_ATR_SRC_ADDR_MSB_OFFSET	0x4
#define PCIE_ATR_TRSL_ADDR_LSB_OFFSET	0x8
#define PCIE_ATR_TRSL_ADDR_MSB_OFFSET	0xc
#define PCIE_ATR_TRSL_PARAM_OFFSET	0x10
#define PCIE_ATR_TLB_SET_OFFSET		0x20

#define PCIE_MAX_TRANS_TABLES		8
#define PCIE_ATR_EN			BIT(0)
#define PCIE_ATR_SIZE(size) \
	(((((size) - 1) << 1) & GENMASK(6, 1)) | PCIE_ATR_EN)
#define PCIE_ATR_ID(id)			((id) & GENMASK(3, 0))
#define PCIE_ATR_TYPE_MEM		PCIE_ATR_ID(0)
#define PCIE_ATR_TYPE_IO		PCIE_ATR_ID(1)
#define PCIE_ATR_TLP_TYPE(type)		(((type) << 16) & GENMASK(18, 16))
#define PCIE_ATR_TLP_TYPE_MEM		PCIE_ATR_TLP_TYPE(0)
#define PCIE_ATR_TLP_TYPE_IO		PCIE_ATR_TLP_TYPE(2)

/* LTSSM state in PCIE_LTSSM_STATUS_REG bit[28:24] */
static const char *const ltssm_str[] = {
	"detect.quiet",			/* 0x00 */
	"detect.active",		/* 0x01 */
	"polling.active",		/* 0x02 */
	"polling.compliance",		/* 0x03 */
	"polling.configuration",	/* 0x04 */
	"config.linkwidthstart",	/* 0x05 */
	"config.linkwidthaccept",	/* 0x06 */
	"config.lanenumwait",		/* 0x07 */
	"config.lanenumaccept",		/* 0x08 */
	"config.complete",		/* 0x09 */
	"config.idle",			/* 0x0A */
	"recovery.receiverlock",	/* 0x0B */
	"recovery.equalization",	/* 0x0C */
	"recovery.speed",		/* 0x0D */
	"recovery.receiverconfig",	/* 0x0E */
	"recovery.idle",		/* 0x0F */
	"L0",				/* 0x10 */
	"L0s",				/* 0x11 */
	"L1.entry",			/* 0x12 */
	"L1.idle",			/* 0x13 */
	"L2.idle",			/* 0x14 */
	"L2.transmitwake",		/* 0x15 */
	"disable",			/* 0x16 */
	"loopback.entry",		/* 0x17 */
	"loopback.active",		/* 0x18 */
	"loopback.exit",		/* 0x19 */
	"hotreset",			/* 0x1A */
};

static uintptr_t mtk_pcie_get_controller_base(pci_devfn_t devfn)
{
	struct device *root_dev;
	const mtk_soc_config_t *config;
	static uintptr_t base = 0;

	if (!base) {
		root_dev = pcidev_path_on_root(devfn);
		config = config_of(root_dev);
		base = config->pcie_config.base;
	}

	return base;
}

volatile union pci_bank *pci_map_bus(pci_devfn_t dev)
{
	u32 val, devfn, bus;
	uintptr_t base;

	devfn = PCI_DEV2DEVFN(dev);
	bus = PCI_DEV2SEGBUS(dev);
	val = PCIE_CFG_HEADER(bus, devfn);

	base = mtk_pcie_get_controller_base(dev);
	write32p(base + PCIE_CFGNUM_REG, val);

	return (void *)(base + PCIE_CFG_OFFSET_ADDR);
}

static int mtk_pcie_set_trans_window(struct device *dev, uintptr_t table,
				     const struct mtk_pcie_mmio_res *mmio_res)
{
	const char *range_type;
	uint32_t table_attr;

	if (!mmio_res)
		return -1;

	if (mmio_res->type == IORESOURCE_IO) {
		range_type = "IO";
		table_attr = PCIE_ATR_TYPE_IO | PCIE_ATR_TLP_TYPE_IO;
	} else if (mmio_res->type == IORESOURCE_MEM) {
		range_type = "MEM";
		table_attr = PCIE_ATR_TYPE_MEM | PCIE_ATR_TLP_TYPE_MEM;
	} else {
		printk(BIOS_ERR, "%s: Unknown trans table type %#lx\n",
		       __func__, mmio_res->type);
		return -1;
	}

	write32p(table, mmio_res->cpu_addr |
		 PCIE_ATR_SIZE(__fls(mmio_res->size)));
	write32p(table + PCIE_ATR_SRC_ADDR_MSB_OFFSET, 0);
	write32p(table + PCIE_ATR_TRSL_ADDR_LSB_OFFSET, mmio_res->pci_addr);
	write32p(table + PCIE_ATR_TRSL_ADDR_MSB_OFFSET, 0);
	write32p(table + PCIE_ATR_TRSL_PARAM_OFFSET, table_attr);

	printk(BIOS_INFO,
	       "%s: set %s trans window: cpu_addr = %#x, pci_addr = %#x, size = %#x\n",
	       __func__, range_type, mmio_res->cpu_addr, mmio_res->pci_addr,
	       mmio_res->size);

	return 0;
}

static void mtk_pcie_domain_new_res(struct device *dev, unsigned int index,
				    const struct mtk_pcie_mmio_res *mmio_res)
{
	struct resource *res;

	if (!mmio_res)
		return;

	res = new_resource(dev, index);
	res->base = mmio_res->cpu_addr;
	res->limit = mmio_res->cpu_addr + mmio_res->size - 1;
	res->flags = mmio_res->type | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED;
}

void mtk_pcie_domain_read_resources(struct device *dev)
{
	const mtk_soc_config_t *config = config_of(dev);
	const struct mtk_pcie_config *conf = &config->pcie_config;

	mtk_pcie_domain_new_res(dev, IOINDEX_SUBTRACTIVE(0, 0),
				&conf->mmio_res_io);

	mtk_pcie_domain_new_res(dev, IOINDEX_SUBTRACTIVE(1, 0),
				&conf->mmio_res_mem);
}

void mtk_pcie_domain_set_resources(struct device *dev)
{
	const mtk_soc_config_t *config = config_of(dev);
	const struct mtk_pcie_config *conf = &config->pcie_config;
	uintptr_t table;

	/* Initialize I/O space constraints. */
	table = conf->base + PCIE_TRANS_TABLE_BASE_REG;
	if (mtk_pcie_set_trans_window(dev, table, &conf->mmio_res_io) < 0)
		printk(BIOS_ERR, "%s: Failed to set IO window\n", __func__);

	/* Initialize memory resources constraints. */
	table = conf->base + PCIE_TRANS_TABLE_BASE_REG +
		PCIE_ATR_TLB_SET_OFFSET;
	if (mtk_pcie_set_trans_window(dev, table, &conf->mmio_res_mem) < 0)
		printk(BIOS_ERR, "%s: Failed to set MEM window\n", __func__);

	pci_domain_set_resources(dev);
}

void mtk_pcie_domain_enable(struct device *dev)
{
	const mtk_soc_config_t *config = config_of(dev);
	const struct mtk_pcie_config *conf = &config->pcie_config;
	const char *ltssm_state;
	long perst_time_us;
	size_t tries = 0;
	uint32_t val;

	/* Set as RC mode */
	val = read32p(conf->base + PCIE_SETTING_REG);
	val |= PCIE_RC_MODE;
	write32p(conf->base + PCIE_SETTING_REG, val);

	/* Set class code */
	val = read32p(conf->base + PCIE_PCI_IDS_1);
	val &= ~GENMASK(31, 8);
	val |= PCI_CLASS(PCI_CLASS_BRIDGE_PCI << 8);
	write32p(conf->base + PCIE_PCI_IDS_1, val);

	/* Mask all INTx interrupts */
	val = read32p(conf->base + PCIE_INT_ENABLE_REG);
	val &= ~PCIE_INTX_ENABLE;
	write32p(conf->base + PCIE_INT_ENABLE_REG, val);

	perst_time_us = early_init_get_elapsed_time_us(EARLY_INIT_PCIE);
	printk(BIOS_DEBUG, "%s: %ld us elapsed since assert PERST#\n",
	       __func__, perst_time_us);

	/*
	 * Described in PCIe CEM specification sections 2.2
	 * (PERST# Signal) and 2.2.1 (Initial Power-Up (G3 to S0)).
	 * The deassertion of PERST# should be delayed 100ms (TPVPERL)
	 * for the power and clock to become stable.
	 */
	const long min_perst_time_us = 100000; /* 100 ms */
	if (perst_time_us < min_perst_time_us) {
		if (!perst_time_us) {
			printk(BIOS_WARNING,
			       "%s: PCIe early init data not found, sleeping 100ms\n",
			       __func__);
			mtk_pcie_reset(conf->base + PCIE_RST_CTRL_REG, true);
		} else {
			printk(BIOS_WARNING,
			       "%s: Need an extra %ld us delay to meet PERST# deassertion requirement\n",
			       __func__, min_perst_time_us - perst_time_us);
		}

		udelay(min_perst_time_us - perst_time_us);
	}

	/* De-assert reset signals */
	mtk_pcie_reset(conf->base + PCIE_RST_CTRL_REG, false);

	if (!retry(100,
		   (tries++, read32p(conf->base + PCIE_LINK_STATUS_REG) &
		    PCIE_CTRL_LINKUP), mdelay(1))) {
		val = read32p(conf->base + PCIE_LTSSM_STATUS_REG);
		ltssm_state = PCIE_LTSSM_STATE(val) >= ARRAY_SIZE(ltssm_str) ?
			    "Unknown state" : ltssm_str[PCIE_LTSSM_STATE(val)];
		printk(BIOS_ERR, "%s: PCIe link down, current ltssm state: %s\n",
		       __func__, ltssm_state);
		return;
	}

	printk(BIOS_INFO, "%s: PCIe link up success (%ld tries)\n", __func__,
	       tries);
}
