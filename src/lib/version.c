#include <version.h>
#include <build.h>

#ifndef CONFIG_MAINBOARD_VENDOR
#error CONFIG_MAINBOARD_VENDOR not defined
#endif
#ifndef CONFIG_MAINBOARD_PART_NUMBER
#error  CONFIG_MAINBOARD_PART_NUMBER not defined
#endif

#ifndef COREBOOT_VERSION
#error  COREBOOT_VERSION not defined
#endif
#ifndef COREBOOT_BUILD
#error  COREBOOT_BUILD not defined
#endif

#ifndef COREBOOT_COMPILE_TIME
#error  COREBOOT_COMPILE_TIME not defined
#endif
#ifndef COREBOOT_COMPILE_BY
#error  COREBOOT_COMPILE_BY not defined
#endif
#ifndef COREBOOT_COMPILE_HOST
#error  COREBOOT_COMPILE_HOST not defined
#endif

#ifndef COREBOOT_COMPILER
#error  COREBOOT_COMPILER not defined
#endif
#ifndef COREBOOT_LINKER
#error  COREBOOT_LINKER not defined
#endif
#ifndef COREBOOT_ASSEMBLER
#error  COREBOOT_ASSEMBLER not defined
#endif

#ifndef  COREBOOT_EXTRA_VERSION
#define COREBOOT_EXTRA_VERSION ""
#endif

const char mainboard_vendor[] = CONFIG_MAINBOARD_VENDOR;
const char mainboard_part_number[] = CONFIG_MAINBOARD_PART_NUMBER;

const char coreboot_version[] = COREBOOT_VERSION;
const char coreboot_extra_version[] = COREBOOT_EXTRA_VERSION;
const char coreboot_build[] = COREBOOT_BUILD;

const char coreboot_compile_time[]   = COREBOOT_COMPILE_TIME;
const char coreboot_compile_by[]     = COREBOOT_COMPILE_BY;
const char coreboot_compile_host[]   = COREBOOT_COMPILE_HOST;
const char coreboot_compile_domain[] = COREBOOT_COMPILE_DOMAIN;
const char coreboot_compiler[]       = COREBOOT_COMPILER;
const char coreboot_linker[]         = COREBOOT_LINKER;
const char coreboot_assembler[]      = COREBOOT_ASSEMBLER;

