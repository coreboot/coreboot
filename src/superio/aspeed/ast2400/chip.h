/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SUPERIO_ASPEED__AST2400_CHIP_H__
#define __SUPERIO_ASPEED__AST2400_CHIP_H__

#include <stdbool.h>

struct superio_aspeed_ast2400_config {
	/* On AST2500 only 1: ESPI, 0: LPC */
	bool use_espi;
};

#endif /* __SUPERIO_ASPEED__AST2400_CHIP_H__ */
