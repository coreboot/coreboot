/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>

/* USB3PHY_PCIE_USB3_PCS_PCS_STATUS bit */
#define USB3_PCS_PHYSTATUS		BIT(6)

/* Define qmp_phy_init_tbl to use offsets instead of absolute addresses.
This makes the tables generic and reusable */
typedef struct qmp_phy_init_tbl {
	u32 offset; // Offset from the base address of the register block
	u32 val;
} qmp_phy_init_tbl_t;

/* Structure to encapsulate base addresses and configuration tables for a specific PHY instance */
struct ss_usb_phy_reg {
	void *com_base;
	void *tx_base;
	void *rx_base;
	void *pcs_base;
	void *pcs_usb3_base;
	const char *name;
	/* Init sequence for QMP PHY blocks - serdes, tx, rx, pcs */
	const qmp_phy_init_tbl_t *serdes_tbl;
	int serdes_tbl_num;
	const qmp_phy_init_tbl_t *tx_tbl;
	int tx_tbl_num;
	const qmp_phy_init_tbl_t *rx_tbl;
	int rx_tbl_num;
	const qmp_phy_init_tbl_t *pcs_tbl;
	int pcs_tbl_num;
	const qmp_phy_init_tbl_t *pcs_usb3_tbl;
	int pcs_usb3_tbl_num;
};
/* Initialize a specific QMP USB3 PHY instance */
bool ss_qmp_phy_init(u32 phy_idx);
