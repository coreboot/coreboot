/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CRATER_EC_H
#define CRATER_EC_H

#define CRATER_REVB  0x42

void crater_ec_init(void);
uint8_t crater_ec_get_board_revision(void);
void crater_ec_get_mac_addresses(uint64_t *xgbe_port0_mac, uint64_t *xgbe_port1_mac);

#endif /* CRATER_EC_H */
