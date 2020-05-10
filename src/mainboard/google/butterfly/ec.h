/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BUTTERFLY_EC_H
#define BUTTERFLY_EC_H

#define EC_SCI_GPI  13   /* GPIO13 is EC_SCI# */

/* EC SMI sources TODO: MLR- make defines */

#ifndef __ACPI__
extern void butterfly_ec_init(void);
#endif

#endif // BUTTERFLY_EC_H
