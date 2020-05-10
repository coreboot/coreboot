/* SPDX-License-Identifier: GPL-2.0-only */
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
