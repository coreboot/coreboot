/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 4.8
 */

#include <soc/emi.h>

size_t sdram_size(void)
{
	return (size_t)4 * GiB;
}
