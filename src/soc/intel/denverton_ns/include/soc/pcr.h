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
 *
 */

#ifndef _DENVERTON_NS_PCR_H_
#define _DENVERTON_NS_PCR_H_

/* PCR BASE */
#include <soc/iomap.h>

/* PCR address */
#define PCH_PCR_ADDRESS(Pid, Offset) \
	(DEFAULT_PCR_BASE | ((uint8_t)(Pid) << 16) | (uint16_t)(Offset))

/* PID for PCR and SBI */
typedef enum {
	PID_SMB = 0xCF,
	PID_ITSS = 0xD0,
	PID_GPIOCOM0 = 0xC2,
	PID_GPIOCOM1 = 0xC5,
} PCH_SBI_PID;

#endif /* _DENVERTON_NS_PCR_H_ */
