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

#endif /* VERSION_H */
