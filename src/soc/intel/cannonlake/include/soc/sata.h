/*
 * This file is part of the coreboot project.
 *
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


#ifndef _SOC_SATA_H_
#define _SOC_SATA_H_

#include <stdint.h>

/* SATA Gen3 strength */
struct sata_port_config {
	uint8_t RxGen3EqBoostMagEnable;
	uint8_t RxGen3EqBoostMag;
	uint8_t TxGen3DownscaleAmpEnable;
	uint8_t TxGen3DownscaleAmp;
	uint8_t TxGen3DeEmphEnable;
	uint8_t TxGen3DeEmph;
};

#endif
