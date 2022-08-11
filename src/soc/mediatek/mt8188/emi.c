/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 3.7
 */

#include <soc/emi.h>

size_t sdram_size(void)
{
	return (size_t)4 * GiB;
}
