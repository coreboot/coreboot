/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PCIE_H_
#define _PCIE_H_

#include <types.h>
#include <device/device.h>
#include <soc/gpio.h>

/*
 * Config, IO and MMIO space offsets relative to controller base and sizes.
 *
 * EP config space starts at 1MB offset from controller base.
 * EP config space size would be 4KB for each endpoint.
 * IO space starts at 2MB offset from controller base and its of 1MB size.
 * MMIO space starts from 3MB offset from controller base and it can be up to
 * end of space reserved for PCIe.
 */
#define PCIE_EP_CONF_OFFSET		0x100000
#define PCIE_EP_CONF_SIZE		0x1000
#define PCIE_IO_SPACE_OFFSET		0x200000
#define PCIE_IO_SPACE_SIZE		0x100000
#define PCIE_MMIO_SPACE_OFFSET		0x300000

/* Parf Registers */
#define PCIE_PARF_SYS_CTRL		0x00
#define MAC_PHY_PWRDOWN_MUX_EN		BIT(29)
#define PCIE_PARF_PHY_CTRL		0x40
#define PHY_PWR_DOWN			BIT(0)
#define PCIE_PARF_MHI_CLOCK_RESET_CTRL	0x174
#define MHI_BYPASS			BIT(4)
#define PCIE_PARF_LTSSM			0x1B0
#define LTSSM_EN			BIT(8)
#define PCIE_PARF_DBI_BASE_ADDR		0x350
#define PCIE_PARF_DEVICE_TYPE		0x1000
#define DEVICE_TYPE_RC			0x4
#define PCIE_PARF_BDF_TO_SID_CFG	0x2C00
#define BDF_TO_SID_BYPASS		BIT(0)
#define PCIE_PARF_PM_CTRL		0x10
#define REQ_NOT_ENTR_L1			BIT(5)
#define PCIE_PARF_SLAVE_AXI_ERR_REPORT	0x3C4
#define AXI_SLAVE_ERR_CRS_BRESP_EN	BIT(14)
#define PCIE_PARF_AXI_MSTR_WR_ADDR_HALT	0x1A8
#define PCIE_SRIS_MODE			0x644
#define PCIE_APP_MARGINING_CTRL		0x2C38
#define APP_MARGINING_READY		BIT(0)
#define APP_MARGINING_SW_READY		BIT(1)

/* ELBI */
#define PCIE3X2_ELBI_SYS_STTS		0x08
#define XMLH_LINK_UP			0x400

/* DBI Registers */
#define PCIE_LINK_CAPABILITY		0x7c
#define PCIE_LINK_CTL_2			0xa0
#define TARGET_LINK_SPEED_MASK		0xf
#define LINK_SPEED_GEN_1		0x1
#define LINK_SPEED_GEN_2		0x2
#define LINK_SPEED_GEN_3		0x3
#define LINK_SPEED_GEN_4		0x4
#define PCIE_LINK_UP_MS			100
#define PCIE_PHY_POLL_US		1000
#define LINK_WAIT_MAX_RETRIES		10
#define PCIE_AMBA_ERR_RESP_DEFLT_OFF	0x8D0
#define AMBA_ERR_RESP_CRS_MASK		0x18
#define PCIE_SPCIE_CAP_OFF_0CH_REG	0x154
#define PCIE_SPCIE_CAP_OFF_10H_REG	0x158
#define PCIE_PL16G_CAP_OFF_20H_REG	0x188
#define PCIE_GEN3_EQ_CONTROL_OFF	0x8A8
#define GEN3_EQ_PSET_REQ_VEC		0x00FFFF00
#define PCIE_GEN3_EQ_FB_MODE_DIR_CHANGE_OFF	0x8AC
#define GEN3_EQ_FMDC_T_MIN_PHASE23	0xF
#define PCIE_GEN3_RELATED_OFF		0x890
#define RATE_SHADOW_SEL_MASK		(BIT(25)|BIT(24))
#define RATE_SHADOW_SEL_VAL		(BIT(24))
#define PCIE_CAP_HOT_PLUG_CAPABLE	BIT(6)
#define PCIE_SLOT_CAPABILITIES_REG	0x84

#define COMMAND_MASK			0xffff

#define TYPE1_HDR_BUS_NUM_MASK		0xffffff
#define RC_PRI_BUS_NUM			0x0
#define RC_SEC_BUS_NUM			0x1
#define RC_SUB_BUS_NUM			0xff
#define ROOT_PORT_BUS_NUM		((RC_SUB_BUS_NUM << 16) | \
					(RC_SEC_BUS_NUM << 8)  | \
					RC_PRI_BUS_NUM)

/* Synopsys-specific PCIe configuration registers */
#define PCIE_DBI_MISC_CONTROL_1_OFF	0x8BC
#define PCIE_DBI_RO_WR_EN		BIT(0)

#define PCIE_3x2_NUM_LANES		2
#define PCIE_1x4_NUM_LANES		4
#define PCIE_LINK_WIDTH_SPEED_CONTROL	0x80C
#define PORT_LOGIC_LINK_WIDTH_MASK	(0x1f << 8)
#define PORT_LOGIC_LINK_WIDTH_1_LANES	(0x1 << 8)
#define PORT_LOGIC_LINK_WIDTH_2_LANES	(0x2 << 8)
#define PORT_LOGIC_LINK_WIDTH_4_LANES	(0x4 << 8)
#define PORT_LOGIC_LINK_WIDTH_8_LANES	(0x8 << 8)

#define PCIE_PORT_LINK_CONTROL		0x710
#define PORT_LINK_MODE_MASK		(0x3f << 16)
#define PORT_LINK_MODE_1_LANES		(0x1 << 16)
#define PORT_LINK_MODE_2_LANES		(0x3 << 16)
#define PORT_LINK_MODE_4_LANES		(0x7 << 16)
#define PORT_LINK_MODE_8_LANES		(0xf << 16)

/*
 * iATU Unroll-specific register definitions
 * From DesignWare PCIe core v4.80, the address translation
 * will be made by unroll
 */
#define PCIE_ATU_REGION_INDEX1		0x1
#define PCIE_ATU_REGION_INDEX0		0x0
#define PCIE_ATU_TYPE_MEM		0x0
#define PCIE_ATU_TYPE_CFG0		0x4
#define PCIE_ATU_TYPE_CFG1		0x5
#define PCIE_ATU_ENABLE			BIT(31)
#define PCIE_ATU_BUS(x)			(((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)			(((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)		(((x) & 0x7) << 16)

#define PCIE_ATU_UNR_REGION_CTRL1	0x00
#define PCIE_ATU_UNR_REGION_CTRL2	0x04
#define PCIE_ATU_UNR_LOWER_BASE		0x08
#define PCIE_ATU_UNR_UPPER_BASE		0x0C
#define PCIE_ATU_UNR_LIMIT		0x10
#define PCIE_ATU_UNR_LOWER_TARGET	0x14
#define PCIE_ATU_UNR_UPPER_TARGET	0x18

#define LINK_WAIT_MAX_IATU_RETRIES	5
#define LINK_WAIT_IATU			1000

/* PHY Specific fields */
#define QPHY_SW_RESET			0x00
#define SW_RESET			BIT(0)
#define QPHY_PCS_STATUS			0x14
#define PHY_STATUS			BIT(6)
#define QPHY_PCS_PWR_DWN_CNTRL		0x40
#define SW_PWRDN			BIT(0)
#define REFCLK_DRV_DSBL			BIT(1)
#define QPHY_START_CTRL			0x44
#define SERDES_START			BIT(0)
#define PCS_START			BIT(1)

/* Register address builder */
#define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region) ((region) << 9)
#define lower_32_bits(n)		((u32)(n))
#define upper_32_bits(n)		((u32)(((n) >> 16) >> 16))

#define QMP_PHY_INIT_CFG(o, v)		\
	{				\
		.offset = o,		\
		.val = v,		\
		.lane_mask = 0xff,	\
	}

typedef uint64_t pci_addr_t;
typedef uint64_t pci_size_t;

struct qcom_qmp_phy_init_tbl {
	unsigned int offset;
	uint32_t val;
	unsigned short lane_mask;
};

struct pcie_region {
	pci_addr_t	bus_start;	/* BDF */
	uint64_t	phys_start;	/* Start in physical address space */
	pci_size_t	size;		/* Size */
};

/**
 * struct pcie_cntlr_cfg_t - QCOM DW PCIe Controller state
 *
 * @lanes : Number of lanes
 * @cfg_size : The size of the configuration space
 * @cfg_base : The base address of config space
 * @dbi_base : The base address of dbi register space
 * @atu_base : The base address of address translation unit
 * @parf : The base address of PARF register space
 * @elbi : The base address of ELBI space
 * @phy  : Base address of the PHY controller
 * @pcie_bcr : address of the block controller register
 * @reset : PERST gpio
 * @io : Base address of the IO region
 * @mem : Base address of memory region
 */
typedef struct {
	unsigned int lanes;
	unsigned int cfg_size;
	void	*cfg_base;
	void	*dbi_base;
	void	*atu_base;
	void	*parf;
	void	*elbi;
	void	*pcie_bcr;
	void	*qmp_phy_bcr;
	gpio_t	perst;
	/* IO and MEM PCI regions */
	struct	pcie_region io;
	struct	pcie_region mem;
} pcie_cntlr_cfg_t;

#if CONFIG(QMP_PHY_2X2_1X4)
typedef struct {
	void *qmp_phy_base;
	void *serdes;
	void *tx0;
	void *rx0;
	void *pcs;
	void *tx1;
	void *rx1;
	void *pcs_misc;
	void *lane0_pcs;
	void *lane1_pcs;
	void *lane0_pcie_pcs;
	void *lane1_pcie_pcs;
	void *ln_shrd;
} pcie_qmp_phy_base_t;
#endif

typedef struct {
#if !CONFIG(QMP_PHY_2X2_1X4)
	void *qmp_phy_base;
	void *serdes;
	void *tx0;
	void *rx0;
	void *pcs;
	void *tx1;
	void *rx1;
	void *pcs_misc;
	void *lane0_pcs;
	void *lane1_pcs;
	void *lane0_pcie_pcs;
	void *lane1_pcie_pcs;
#else
	pcie_qmp_phy_base_t porta;
	pcie_qmp_phy_base_t portb;
#endif
	/* Init sequence for PHY blocks - serdes, tx, rx, pcs */
	const struct qcom_qmp_phy_init_tbl *serdes_tbl;
	unsigned int serdes_tbl_num;
	const struct qcom_qmp_phy_init_tbl *serdes_tbl_sec;
	unsigned int serdes_tbl_num_sec;
	const struct qcom_qmp_phy_init_tbl *tx_tbl;
	unsigned int tx_tbl_num;
	const struct qcom_qmp_phy_init_tbl *tx_tbl_sec;
	unsigned int tx_tbl_num_sec;
	const struct qcom_qmp_phy_init_tbl *rx_tbl;
	unsigned int rx_tbl_num;
	const struct qcom_qmp_phy_init_tbl *rx_tbl_sec;
	unsigned int rx_tbl_num_sec;
	const struct qcom_qmp_phy_init_tbl *pcs_tbl;
	unsigned int pcs_tbl_num;
	const struct qcom_qmp_phy_init_tbl *pcs_tbl_sec;
	unsigned int pcs_tbl_num_sec;
	const struct qcom_qmp_phy_init_tbl *pcs_misc_tbl;
	unsigned int pcs_misc_tbl_num;
	const struct qcom_qmp_phy_init_tbl *pcs_misc_tbl_sec;
	unsigned int pcs_misc_tbl_num_sec;
	const struct qcom_qmp_phy_init_tbl *pcs_misc_tbl_thrd;
	unsigned int pcs_misc_tbl_num_thrd;
	const struct qcom_qmp_phy_init_tbl *lane0_pcie_pcs_tbl;
	unsigned int lane0_pcie_pcs_tbl_num;
	const struct qcom_qmp_phy_init_tbl *lane1_pcie_pcs_tbl;
	unsigned int lane1_pcie_pcs_tbl_num;
#if CONFIG(QMP_PHY_2X2_1X4)
	const struct qcom_qmp_phy_init_tbl *ln_shrd_tbl;
	unsigned int ln_shrd_tbl_num;
#endif
} pcie_qmp_phy_cfg_t;

struct qcom_pcie_cntlr_t {
	pcie_cntlr_cfg_t	*cntlr_cfg;
	pcie_qmp_phy_cfg_t	*qmp_phy_cfg;
};

int qcom_dw_pcie_enable_clock(void);
int qcom_dw_pcie_enable_pipe_clock(void);
void gcom_pcie_power_on_ep(void);
void gcom_pcie_get_config(struct qcom_pcie_cntlr_t *host_cfg);
void qcom_pci_domain_read_resources(struct device *dev);
void qcom_setup_pcie_host(struct device *dev);
bool mainboard_needs_pcie_init(void);

#endif
