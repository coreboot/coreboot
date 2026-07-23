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

/*
 * Structure to encapsulate base addresses and configuration tables for a
 * QMP PHY instance. Shared by both Type-A (USB3 MP UniPHY) and Type-C
 * (USB4 SS PHY) controllers:
 *
 *  Type-A (USB3_UNI_PHY_MP0/1):
 *    com_base      = QSERDES_COM  (0xFA3000)
 *    tx_base       = QSERDES_TX   (0xFA3E00)
 *    rx_base       = QSERDES_RX   (0xFA4000)
 *    pcs_base      = PCS          (0xFA3400)
 *    pcs_usb3_base = PCS_USB3     (0xFA4200)
 *
 *  Type-C (QUSB4PHY_SS_0/1):
 *    com_base        = USB43DP_COM       (0xFD5000)
 *    qserdes_pll_base= USB3_QSERDES_PLL  (0xFD6000)
 *    lanea_base      = USB43DP_QSERDES_LA(0xFD9000)
 *    laneb_base      = USB43DP_QSERDES_LB(0xFDA000)
 *    pcs_misc_base   = USB3_PCS_MISC     (0xFD6400)
 *    pcs_base        = USB3_PCS          (0xFD6600)
 *    pcs_usb3_base   = USB3_PCS_USB3     (0xFD6900)
 */
struct ss_usb_phy_reg {
	void *com_base;
	/* Type-A: QSERDES_TX; unused for Type-C (use lanea_base instead) */
	void *tx_base;
	void *rx_base;
	/* Type-C only: USB3_QSERDES_PLL, Lane A/B, PCS_MISC */
	void *qserdes_pll_base;
	void *lanea_base;
	void *laneb_base;
	void *rxb_base;
	void *pcs_misc_base;
	void *pcs_base;
	void *pcs_usb3_base;
	const char *name;
	/* Init tables: Type-A uses tx_tbl; Type-C uses lane_tbl */
	const qmp_phy_init_tbl_t *serdes_tbl;
	int serdes_tbl_num;
	const qmp_phy_init_tbl_t *tx_tbl;       /* Type-A TX lane */
	int tx_tbl_num;
	const qmp_phy_init_tbl_t *lane_tbl;     /* Type-C Lane A */
	int lane_tbl_num;
	const qmp_phy_init_tbl_t *rx_tbl;
	int rx_tbl_num;
	const qmp_phy_init_tbl_t *pcs_tbl;
	int pcs_tbl_num;
	const qmp_phy_init_tbl_t *pcs_misc_tbl;
	int pcs_misc_tbl_num;
	const qmp_phy_init_tbl_t *pcs_usb3_tbl;
	int pcs_usb3_tbl_num;
};
/* Initialize a specific QMP USB3 PHY instance */
bool ss_qmp_phy_init(u32 phy_idx);
