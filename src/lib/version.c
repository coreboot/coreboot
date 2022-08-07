/* SPDX-License-Identifier: GPL-2.0-only */

#include <version.h>
#include <build.h>

#ifndef CONFIG_MAINBOARD_VENDOR
#error CONFIG_MAINBOARD_VENDOR not defined
#endif
#ifndef CONFIG_MAINBOARD_PART_NUMBER
#error CONFIG_MAINBOARD_PART_NUMBER not defined
#endif

#ifndef COREBOOT_VERSION
#error COREBOOT_VERSION not defined
#endif
#ifndef COREBOOT_BUILD
#error COREBOOT_BUILD not defined
#endif

#ifndef COREBOOT_COMPILE_TIME
#error COREBOOT_COMPILE_TIME not defined
#endif

#ifndef COREBOOT_EXTRA_VERSION
#define COREBOOT_EXTRA_VERSION ""
#endif

const char mainboard_vendor[] = CONFIG_MAINBOARD_VENDOR;
const char mainboard_part_number[] = CONFIG_MAINBOARD_PART_NUMBER;

const char coreboot_version[] = COREBOOT_VERSION;
const char coreboot_extra_version[] = COREBOOT_EXTRA_VERSION;
const char coreboot_build[] = COREBOOT_BUILD;
const unsigned int coreboot_version_timestamp = COREBOOT_VERSION_TIMESTAMP;
const unsigned int coreboot_major_revision = COREBOOT_MAJOR_VERSION;
const unsigned int coreboot_minor_revision = COREBOOT_MINOR_VERSION;

const char coreboot_compile_time[] = COREBOOT_COMPILE_TIME;
const char coreboot_dmi_date[] = COREBOOT_DMI_DATE;

const struct bcd_date coreboot_build_date = {
	.century = 0x20,
	.year = COREBOOT_BUILD_YEAR_BCD,
	.month = COREBOOT_BUILD_MONTH_BCD,
	.day = COREBOOT_BUILD_DAY_BCD,
	.weekday = COREBOOT_BUILD_WEEKDAY_BCD,
};

const unsigned int asl_revision = ASL_VERSION;
