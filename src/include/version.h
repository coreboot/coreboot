/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VERSION_H
#define VERSION_H

/* Motherboard Information */
extern const char mainboard_vendor[];
extern const char mainboard_part_number[];

/* coreboot Version */
extern const char coreboot_version[];
extern const char coreboot_extra_version[];
extern const char coreboot_build[];
extern const unsigned int coreboot_version_timestamp;
extern const unsigned int coreboot_major_revision;
extern const unsigned int coreboot_minor_revision;

/* When coreboot was compiled */
extern const char coreboot_compile_time[];
extern const char coreboot_dmi_date[];

struct bcd_date {
	unsigned char century;
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char weekday;
};

extern const struct bcd_date coreboot_build_date;

/* IASL version */
extern const unsigned int asl_revision;

#endif /* VERSION_H */
