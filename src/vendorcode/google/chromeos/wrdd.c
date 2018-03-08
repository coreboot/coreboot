/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
#include <console/console.h>
#include <cpu/cpu.h>
#include <types.h>
#include <string.h>
#include <wrdd.h>
#include <drivers/vpd/vpd.h>

#define CROS_VPD_REGION_NAME	"region"

/*
 * wrdd_domain_value is ISO 3166-2
 * ISO 3166-2 code consists of two parts, separated by a hyphen
 * The first part is the ISO 3166-1 alpha-2 code of the country;
 * The second part is a string of up to three alphanumeric characters
 */
#define VARIANT_SEPARATOR '.'
struct wrdd_code_value_pair {
	const char *code;
	u16 value;
};

/* Retrieve the regulatory domain information from VPD and
 * return it as an uint16.
 * WARNING: if domain information is not found in the VPD,
 * this function will fall back to the default value
 */
uint16_t wifi_regulatory_domain(void)
{
	static struct wrdd_code_value_pair wrdd_table[] = {
		{
			/* Indonesia
			 * Alpha-2 code         'ID'
			 * Full name            'the Republic of Indonesia'
			 * Alpha-3 code         'IDN'
			 * Numeric code         '360'
			 */
			.code = "id",
			.value = WRDD_REGULATORY_DOMAIN_INDONESIA
		}
	};
	const char *wrdd_domain_key = CROS_VPD_REGION_NAME;
	int i;
	struct wrdd_code_value_pair *p;
	/* wrdd_domain_value is ISO 3166-2 */
	char wrdd_domain_code[7];
	char *separator;

	/* If not found for any reason fall backto the default value */
	if (!vpd_gets(wrdd_domain_key, wrdd_domain_code,
		ARRAY_SIZE(wrdd_domain_code), VPD_ANY)) {
		printk(BIOS_DEBUG,
		"Error: Could not locate '%s' in VPD\n", wrdd_domain_key);
		return WRDD_DEFAULT_REGULATORY_DOMAIN;
	}
	printk(BIOS_DEBUG, "Found '%s'='%s' in VPD\n",
		wrdd_domain_key, wrdd_domain_code);
	separator = memchr(wrdd_domain_code, VARIANT_SEPARATOR,
				ARRAY_SIZE(wrdd_domain_code));
	if (separator) {
		*separator = '\0';
	}

	for (i = 0; i < ARRAY_SIZE(wrdd_table); i++) {
		p = &wrdd_table[i];
		if (strncmp(p->code, wrdd_domain_code,
			ARRAY_SIZE(wrdd_domain_code)) == 0)
			return p->value;
	}
	return WRDD_DEFAULT_REGULATORY_DOMAIN;
}
