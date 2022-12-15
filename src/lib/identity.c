/* SPDX-License-Identifier: GPL-2.0-only */

#include <identity.h>

#ifndef CONFIG_MAINBOARD_VENDOR
#error CONFIG_MAINBOARD_VENDOR not defined
#endif
#ifndef CONFIG_MAINBOARD_PART_NUMBER
#error CONFIG_MAINBOARD_PART_NUMBER not defined
#endif

const char mainboard_vendor[] = CONFIG_MAINBOARD_VENDOR;
const char mainboard_part_number[] = CONFIG_MAINBOARD_PART_NUMBER;
const char mainboard_name[] = CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER;
