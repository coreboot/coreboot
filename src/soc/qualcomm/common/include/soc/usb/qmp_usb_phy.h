/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>

/* USB3PHY_PCIE_USB3_PCS_PCS_STATUS bit */
#define USB3_PCS_PHYSTATUS		BIT(6)

struct qmp_phy_init_tbl {
	u32 *address;
	u32 val;
};

struct ss_usb_phy_reg {
	/* Init sequence for QMP PHY blocks - serdes, tx, rx, pcs */
	const struct qmp_phy_init_tbl *serdes_tbl;
	int serdes_tbl_num;

	const struct qmp_phy_init_tbl *tx_tbl;
	int tx_tbl_num;

	const struct qmp_phy_init_tbl *rx_tbl;
	int rx_tbl_num;

	const struct qmp_phy_init_tbl *pcs_tbl;
	int pcs_tbl_num;

	struct usb3_phy_pcs_reg_layout *qmp_pcs_reg;
};

void ss_qmp_phy_init(void);
