/* SPDX-License-Identifier: GPL-2.0-only */

#include <wrdd.h>

uint16_t __weak wifi_regulatory_domain(void)
{
	return WRDD_DEFAULT_REGULATORY_DOMAIN;
}
