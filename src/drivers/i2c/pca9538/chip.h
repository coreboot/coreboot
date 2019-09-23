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


struct drivers_i2c_pca9538_config {
	unsigned char in_out;	/* Use bit as input(1) or output (0). */
	unsigned char invert;	/* If a bit is 1, the input will be inverted. */
	unsigned char out_val;	/* Initial output value to drive. */
};
