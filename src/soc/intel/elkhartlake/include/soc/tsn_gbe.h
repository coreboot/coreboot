/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ELKHARTLAKE_TSN_GBE_H_
#define _SOC_ELKHARTLAKE_TSN_GBE_H_

#define MAC_ADDR_LEN		6

#define TSN_MAC_ADD0_HIGH	0x300	/* MAC Address0 High register */
#define TSN_MAC_ADD0_LOW	0x304	/* MAC Address0 Low register */

/* We need one function we can call to get a MAC address to use. */
/* This function can be coded somewhere else but must exist. */
enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[MAC_ADDR_LEN]);

#endif /* _SOC_ELKHARTLAKE_TSN_GBE_H_ */
