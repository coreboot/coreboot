/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_AHCI_H_
#define _SOC_APOLLOLAKE_AHCI_H_

#include <soc/soc_chip.h>

#define AHCI_TMP_BASE_ADDR		0x9872c000

#define AHCI_CAP			0x0
#define AHCI_CAP_ISS_MASK		0x00f00000
#define  AHCI_SPEED(speed)		(speed << 20)

/* Set SATA controller speed. */
void ahci_set_speed(enum sata_speed_limit speed);

#endif /* _SOC_APOLLOLAKE_AHCI_H_ */
