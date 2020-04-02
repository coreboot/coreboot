/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <wrdd.h>

uint16_t __weak wifi_regulatory_domain(void)
{
	return WRDD_DEFAULT_REGULATORY_DOMAIN;
}
