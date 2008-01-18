#ifndef VERSION_H
#define VERSION_H

/* Motherboard Information */
extern const char mainboard_vendor[];
extern const char mainboard_part_number[];

/* coreboot Version */
extern const char coreboot_version[];
extern const char coreboot_extra_version[];
extern const char coreboot_build[];

/* When coreboot was compiled */
extern const char coreboot_compile_time[];
extern const char coreboot_compile_by[];
extern const char coreboot_compile_host[];
extern const char coreboot_compile_domain[];
extern const char coreboot_compiler[];
extern const char coreboot_linker[];
extern const char coreboot_assembler[];

#endif /* VERSION_H */
