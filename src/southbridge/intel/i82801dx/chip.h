/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef I82801DX_CHIP_H
#define I82801DX_CHIP_H

#include <stdint.h>

struct southbridge_intel_i82801dx_config {
	int enable_usb;
	int enable_native_ide;
	/**
	 * Interrupt Routing configuration
	 * If bit7 is 1, the interrupt is disabled.
	 */
	uint8_t pirqa_routing;
	uint8_t pirqb_routing;
	uint8_t pirqc_routing;
	uint8_t pirqd_routing;
	uint8_t pirqe_routing;
	uint8_t pirqf_routing;
	uint8_t pirqg_routing;
	uint8_t pirqh_routing;

	uint8_t ide0_enable;
	uint8_t ide1_enable;
};

#endif /* I82801DX_CHIP_H */
