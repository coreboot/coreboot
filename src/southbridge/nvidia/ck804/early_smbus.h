/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

int ck804_smbus_read_byte(unsigned int, unsigned int, unsigned int);
int ck804_smbus_write_byte(unsigned int, unsigned int, unsigned int, unsigned char);
void enable_smbus(void);
int smbus_read_byte(unsigned int, unsigned int);
int smbus_write_byte(unsigned int, unsigned int, unsigned char);
