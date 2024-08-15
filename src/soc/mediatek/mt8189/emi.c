/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <soc/emi.h>

size_t sdram_size(void)
{
	return (size_t)4 * GiB;
}
