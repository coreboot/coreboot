/*
 * This file is part of the coreboot project.
 *
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

#ifndef STOUT_EC_H
#define STOUT_EC_H

#define EC_SCI_GPI  6   /* GPIO6 is EC_SCI# */
#define EC_SMI_GPI  1   /* GPIO1 is EC_SMI# */

#define EC_SMI_LID_CLOSED	0x2B

#ifndef __ACPI__
void stout_ec_init(void);
void stout_ec_finalize_smm(void);
#endif

#endif // STOUT_EC_H
