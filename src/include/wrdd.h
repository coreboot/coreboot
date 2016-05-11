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
#ifndef _WRDD_H_
#define _WRDD_H_

#include <stdint.h>

/* WRDD Spec Revision */
#define WRDD_REVISION 0x0

/* Domain type */
#define WRDD_DOMAIN_TYPE_WIFI 0x7

/* Default regulatory domain ID */
#define WRDD_DEFAULT_REGULATORY_DOMAIN 0x4150
/* INDONESIA regulatory domain ID */
#define WRDD_REGULATORY_DOMAIN_INDONESIA 0x4944

/* Retrieve the regulatory domain information */
uint16_t wifi_regulatory_domain(void);

#endif /* _WRDD_H_ */

