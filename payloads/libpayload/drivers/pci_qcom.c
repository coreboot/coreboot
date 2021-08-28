/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>
#include <pci.h>

/*
 * iATU Unroll-specific register definitions
 */
#define PCIE_ATU_UNR_REGION_CTRL1	0x00
#define PCIE_ATU_UNR_REGION_CTRL2	0x04
#define PCIE_ATU_UNR_LOWER_BASE		0x08
#define PCIE_ATU_UNR_UPPER_BASE		0x0C
#define PCIE_ATU_UNR_LIMIT		0x10
#define PCIE_ATU_UNR_LOWER_TARGET	0x14
#define PCIE_ATU_UNR_UPPER_TARGET	0x18
#define PCIE_ATU_REGION_INDEX0		0x0
#define PCIE_ATU_TYPE_CFG0		0x4
#define PCIE_ATU_TYPE_CFG1		0x5
#define PCIE_ATU_ENABLE			BIT(31)
#define ATU_CTRL2			PCIE_ATU_UNR_REGION_CTRL2
#define ATU_ENABLE			PCIE_ATU_ENABLE

#define PCIE_ATU_BUS(x)			(((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)			(((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)		(((x) & 0x7) << 16)
#define LINK_WAIT_IATU_US		1000
#define LINK_WAIT_MAX_IATU_RETRIES	5

/* Register address builder */
#define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region) ((region) << 9)

#define lower_32_bits(n)		((u32)(n))
#define upper_32_bits(n)		((u32)(((n) >> 16) >> 16))

/*
 * ATU & endpoint config space base address offsets relative to
 * PCIe controller base address.
 */
#define QCOM_ATU_BASE_OFFSET		0x1000
#define QCOM_EP_CFG_OFFSET		0x100000
#define QCOM_EP_CFG_SIZE		0x1000    /* 4K */

static void dw_pcie_writel_iatu(void *atu_base, unsigned short index,
				uint32_t reg, uint32_t val)
{
	uint32_t offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	write32(atu_base + offset + reg, val);
}

static uint32_t dw_pcie_readl_iatu(void *atu_base, unsigned short index,
				   uint32_t reg)
{
	uint32_t offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	return read32(atu_base + offset + reg);
}

static void dw_pcie_prog_outbound_atu(void *atu_base, unsigned short index,
				      unsigned int type, uint64_t cfg_addr,
				      uint64_t pcie_addr, uint32_t cfg_size)
{
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_LOWER_BASE,
			    lower_32_bits(cfg_addr));
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_UPPER_BASE,
			    upper_32_bits(cfg_addr));
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_LIMIT,
			    lower_32_bits(cfg_addr + cfg_size - 1));
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_LOWER_TARGET,
			    lower_32_bits(pcie_addr));
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_UPPER_TARGET,
			    upper_32_bits(pcie_addr));
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_REGION_CTRL1, type);
	dw_pcie_writel_iatu(atu_base, index, PCIE_ATU_UNR_REGION_CTRL2,
			    PCIE_ATU_ENABLE);
	/*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 */
	if (retry(LINK_WAIT_MAX_IATU_RETRIES,
		  (dw_pcie_readl_iatu(atu_base, index, ATU_CTRL2) & ATU_ENABLE),
		  udelay(LINK_WAIT_IATU_US)))
		return;

	printf("outbound iATU is couldn't be enabled after 5ms\n");
}

/* Get PCIe MMIO configuration space base address */
uintptr_t pci_map_bus(pcidev_t dev)
{
	unsigned int atu_type, busdev;
	uint32_t config_size;
	void *cntrlr_base, *config_base, *atu_base;
	unsigned int current_bus = PCI_BUS(dev);
	unsigned int devfn = (PCI_SLOT(dev) << 3) | PCI_FUNC(dev);
	static pcidev_t current_dev;

	/*
	 * Extract PCIe controller base from coreboot and derive the ATU and
	 * endpoint config base addresses from it.
	 */
	cntrlr_base = (void *)lib_sysinfo.pcie_ctrl_base;
	config_base = (void *)cntrlr_base + QCOM_EP_CFG_OFFSET;
	config_size = (uint32_t)QCOM_EP_CFG_SIZE;
	atu_base = (void *)cntrlr_base + QCOM_ATU_BASE_OFFSET;

	/*
	 * Cache the dev. For same dev, ATU mapping is not needed for each
	 * request.
	 */
	if (current_dev == dev)
		goto out;

	current_dev = dev;

	busdev = PCIE_ATU_BUS(current_bus)   |
		 PCIE_ATU_DEV(PCI_SLOT(dev)) |
		 PCIE_ATU_FUNC(PCI_FUNC(dev));

	atu_type = current_bus == 1 ? PCIE_ATU_TYPE_CFG0 : PCIE_ATU_TYPE_CFG1;

	dw_pcie_prog_outbound_atu(atu_base, PCIE_ATU_REGION_INDEX0, atu_type,
				  (uint64_t)config_base, busdev, config_size);
out:
	return (uintptr_t)config_base + (QCOM_EP_CFG_SIZE * devfn);
}
