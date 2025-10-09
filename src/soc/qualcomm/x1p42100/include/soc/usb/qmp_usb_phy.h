/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>

/* USB3PHY_PCIE_USB3_PCS_PCS_STATUS bit */
#define USB3_PCS_PHYSTATUS		BIT(6)

/* PCS Control Register Values */
#define PCS_SW_RESET_DEASSERT		0x00
#define QPHY_PCS_START			BIT(0)
#define QPHY_SERDES_START			BIT(1)

/* USB43DP_COM_TYPEC_CTRL register bit definitions */
#define SW_PORTSELECT_SHIFT     0
#define SW_PORTSELECT_MUX_SHIFT 1
#define SW_PORTSELECT_MASK      BIT(SW_PORTSELECT_SHIFT)
#define SW_PORTSELECT_MUX_MASK  BIT(SW_PORTSELECT_MUX_SHIFT)

/* Define qmp_phy_init_tbl to use offsets instead of absolute addresses.
This makes the tables generic and reusable */
typedef struct qmp_phy_init_tbl {
	u32 offset; // Offset from the base address of the register block
	u32 val;
} qmp_phy_init_tbl_t;

/* Structure to encapsulate base addresses and configuration tables for a specific PHY instance */
struct ss_usb_phy_reg {
	void *com_base;
	void *qserdes_pll_base;
	void *tx_base;
	void *rx_base;
	void *txb_base;
	void *rxb_base;
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

/* Initialize SS0/SS1 QMP PHY - pass 0 for SS0, 1 for SS1 */
enum cb_err qmp_usb4_dp_phy_ss_init(int phy_instance, bool polarity_inverse);
