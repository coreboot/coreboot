/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ELKHARTLAKE_TSN_GBE_H_
#define _SOC_ELKHARTLAKE_TSN_GBE_H_

#define MAC_ADDR_LEN			6

#define TSN_MAC_ADD0_HIGH		0x300		/* MAC Address0 High register */
#define TSN_MAC_ADD0_LOW		0x304		/* MAC Address0 Low register */

#define TSN_GMII_TIMEOUT_MS		20

#define TSN_MAC_MDIO_ADR		0x200		/* MAC MDIO Address register */
#define TSN_MAC_MDIO_ADR_MASK		0x03FF7F0E
#define  TSN_MAC_PHYAD(pa)		(pa << 21)	/* Physical Layer Address */
#define  TSN_MAC_REGAD(rda)		(rda << 16)	/* Register/Device Address */
#define  TSN_MAC_CLK_TRAIL_4		(4 << 12)	/* 4 Trailing Clocks */
#define  TSN_MAC_CSR_CLK_DIV_62		(1 << 8)	/* 0001: CSR=100-150 MHz; CSR/62 */
#define  TSN_MAC_OP_CMD_WRITE		(1 << 2)	/* GMII Operation Command Write */
#define  TSN_MAC_OP_CMD_READ		(3 << 2)	/* GMII Operation Command Read */
#define  TSN_MAC_GMII_BUSY		(1 << 0)	/* GMII Busy bit */

/* MDIO - Adhoc PHY Sublayer Register */
#define TSN_MAC_MDIO_ADHOC_ADR		0x15
/* Global Configuration Register */
#define TSN_MAC_MDIO_GCR		0x0
/* PHY to MAC Interrupt Polarity bit */
#define  TSN_MAC_PHY2MAC_INTR_POL	(1 << 6)

#define TSN_MAC_MDIO_DATA		0x204		/* MAC MDIO Data register */

/* We need one function we can call to get a MAC address to use. */
/* This function can be coded somewhere else but must exist. */
enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[MAC_ADDR_LEN]);

enum cb_err phy_gmii_ready(void *base);
uint16_t tsn_mdio_read(void *base, uint8_t phy_adr, uint8_t reg_adr);
void tsn_mdio_write(void *base, uint8_t phy_adr, uint8_t reg_adr, uint16_t data);

#endif /* _SOC_ELKHARTLAKE_TSN_GBE_H_ */
