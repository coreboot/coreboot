/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 10.2
 */

#include <soc/emi.h>

size_t sdram_size(void)
{
	return (size_t)4 * GiB;
}
