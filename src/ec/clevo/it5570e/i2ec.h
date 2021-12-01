/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_CLEVO_IT5570E_I2EC_H
#define EC_CLEVO_IT5570E_I2EC_H

/*
 * Read/write I2EC registers through SIO "depth 2" address space
 */

uint8_t ec_d2i2ec_read(uint16_t addr);
void ec_d2i2ec_write(uint16_t addr, uint8_t val);

#endif /* EC_CLEVO_IT5570E_I2EC_H */
