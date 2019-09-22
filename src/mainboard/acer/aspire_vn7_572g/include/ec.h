/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_EC_H
#define MAINBOARD_EC_H

uint8_t ec_cmd_90_read(uint8_t addr);
void ec_cmd_91_write(uint8_t addr, uint8_t data);
uint8_t ec_cmd_94_query(void);
uint8_t ec_idx_read(uint16_t addr);
void ec_idx_write(uint16_t addr, uint8_t data);
/* TODO: Check if ADC is valid. */
uint16_t read_ec_adc_converter(uint8_t adc);

#endif
