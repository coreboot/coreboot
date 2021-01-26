/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef TAS5825M_H
#define TAS5825M_H

#include <device/device.h>

int tas5825m_write_at(struct device *dev, uint8_t addr, uint8_t value);
int tas5825m_write_block_at(struct device *dev, uint8_t addr,
	const uint8_t *values, uint8_t length);
int tas5825m_set_page(struct device *dev, uint8_t page);
int tas5825m_set_book(struct device *dev, uint8_t book);
int tas5825m_setup(struct device *dev, int id);

#endif // TAS5825M_H
