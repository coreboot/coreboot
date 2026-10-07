/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5525D_CHIP_H
#define SUPERIO_NUVOTON_NCT5525D_CHIP_H

#include <stdbool.h>
#include <stdint.h>


/* Function of pins 46/48 (CR1B[1:0], CR2A[1]) */
enum nct5525d_pins_46_48 {
	NCT5525D_PINS_46_48_KEEP = 0,
	NCT5525D_PINS_46_48_GPIO,		/* GP63/GP64 */
};

struct superio_nuvoton_nct5525d_config {
	enum nct5525d_pins_46_48 pins_46_48;

	/* Route pins 39/40 to TSIC/TSID for AMD SB-TSI */
	bool tsi_pins;
};

#endif /* SUPERIO_NUVOTON_NCT5525D_CHIP_H */
