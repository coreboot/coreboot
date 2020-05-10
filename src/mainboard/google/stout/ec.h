/* SPDX-License-Identifier: GPL-2.0-only */

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
