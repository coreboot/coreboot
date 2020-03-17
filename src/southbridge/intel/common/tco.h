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

#ifndef SOUTHBRIDGE_INTEL_COMMON_TCO_H
#define SOUTHBRIDGE_INTEL_COMMON_TCO_H

#define PMBASE_TCO_OFFSET	0x60
#define TCO1_STS		0x04
#define  TCO1_TIMEOUT		(1 << 3)
#define TCO2_STS		0x06
#define  SECOND_TO_STS		(1 << 1)
#define TCO1_CNT		0x08
#define  TCO_TMR_HLT		(1 << 11)

#endif /* SOUTHBRIDGE_INTEL_COMMON_TCO_H */
