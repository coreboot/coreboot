#ifndef VERSION_H
#define VERSION_H

/* Motherboard Information */
extern const char mainboard_vendor[];
extern const char mainboard_part_number[];

/* LinuxBIOS Version */
extern const char linuxbios_version[];
extern const char linuxbios_extra_version[];
extern const char linuxbios_build[];

/* When LinuxBIOS was compiled */
extern const char linuxbios_compile_time[];
extern const char linuxbios_compile_by[];
extern const char linuxbios_compile_host[];
extern const char linuxbios_compile_domain[];
extern const char linuxbios_compiler[];
extern const char linuxbios_linker[];
extern const char linuxbios_assembler[];

#endif /* VERSION_H */
