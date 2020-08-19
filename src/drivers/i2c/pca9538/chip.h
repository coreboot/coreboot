/* SPDX-License-Identifier: GPL-2.0-only */

struct drivers_i2c_pca9538_config {
	unsigned char in_out;	/* Use bit as input(1) or output (0). */
	unsigned char invert;	/* If a bit is 1, the input will be inverted. */
	unsigned char out_val;	/* Initial output value to drive. */
};
