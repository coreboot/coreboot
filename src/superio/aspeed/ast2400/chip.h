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

#ifndef __SUPERIO_ASPEED__AST2400_CHIP_H__
#define __SUPERIO_ASPEED__AST2400_CHIP_H__

struct superio_aspeed_ast2400_config {
	/* On AST2500 only 1: ESPI, 0: LPC */
	bool use_espi;
};

#endif /* __SUPERIO_ASPEED__AST2400_CHIP_H__ */
