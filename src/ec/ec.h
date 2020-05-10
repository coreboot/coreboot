/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_EC_H
#define EC_EC_H

#ifndef __ACPI__
/* Entry point doing any mainboard specific EC initialization. */
void mainboard_ec_init(void);
#endif

#endif
