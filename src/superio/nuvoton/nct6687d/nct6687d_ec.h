/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT6687D_EC_H
#define SUPERIO_NUVOTON_NCT6687D_EC_H

#include <types.h>

/* Offsets in the EC IO base. Port0 for firmware use, Port1 for software use */
#define EC_PORT0_PAGE 0
#define EC_PORT0_INDEX 1
#define EC_PORT0_DATA 2
#define EC_PORT0_HOST_IF_EVENT 3

#define EC_PORT1_PAGE 4
#define EC_PORT1_INDEX 5
#define EC_PORT1_DATA 6
#define EC_PORT1_HOST_IF_EVENT 7

void nct6687d_ec_write_page(uint16_t iobase, uint8_t page, uint8_t index, uint8_t value);
void nct6687d_ec_write_page_ff(uint16_t iobase, uint8_t page, uint8_t index, uint8_t value);
void nct6687d_ec_and_or_page(uint16_t iobase, uint8_t page, uint8_t index,
			     uint8_t and_mask, uint8_t or_mask);
void nct6687d_ec_and_or_page_ff(uint16_t iobase, uint8_t page, uint8_t index,
			     uint8_t and_mask, uint8_t or_mask);
uint8_t nct6687d_ec_read_page(uint16_t iobase, uint8_t page, uint8_t index);

#endif /* SUPERIO_NUVOTON_NCT6687D_EC_H */
