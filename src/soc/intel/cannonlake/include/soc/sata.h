/* SPDX-License-Identifier: GPL-2.0-only */

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
