/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SB800_LPC_H_
#define _SB800_LPC_H_

#define SPIROM_BASE_ADDRESS_REGISTER  0xA0
#define SPI_ROM_ENABLE                0x02
#define SPI_BASE_ADDRESS              0xFEC10000

void lpc_read_resources(struct device *dev);
void lpc_set_resources(struct device *dev);
void lpc_enable_childrens_resources(struct device *dev);

#endif
