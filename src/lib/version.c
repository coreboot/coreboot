#include <version.h>

#define __STR(X) #X
#define STR(X) __STR(X)

#ifndef MAINBOARD_VENDOR
#error MAINBOARD_VENDOR not defined
#endif
#ifndef MAINBOARD_PART_NUMBER
#error  MAINBOARD_PART_NUMBER not defined
#endif

#ifndef LINUXBIOS_VERSION
#error  LINUXBIOS_VERSION not defined
#endif
#ifndef LINUXBIOS_BUILD
#error  LINUXBIOS_BUILD not defined
#endif

#ifndef LINUXBIOS_COMPILE_TIME
#error  LINUXBIOS_COMPILE_TIME not defined
#endif
#ifndef LINUXBIOS_COMPILE_BY
#error  LINUXBIOS_COMPILE_BY not defined
#endif
#ifndef LINUXBIOS_COMPILE_HOST
#error  LINUXBIOS_COMPILE_HOST not defined
#endif

#ifndef LINUXBIOS_COMPILER
#error  LINUXBIOS_COMPILER not defined
#endif
#ifndef LINUXBIOS_LINKER
#error  LINUXBIOS_LINKER not defined
#endif
#ifndef LINUXBIOS_ASSEMBLER
#error  LINUXBIOS_ASSEMBLER not defined
#endif


#ifndef  LINUXBIOS_EXTRA_VERSION
#define LINUXBIOS_EXTRA_VERSION
#endif

const char mainboard_vendor[] = STR(MAINBOARD_VENDOR);
const char mainboard_part_number[] = STR(MAINBOARD_PART_NUMBER);

const char linuxbios_version[] = STR(LINUXBIOS_VERSION);
const char linuxbios_extra_version[] = STR(LINUXBIOS_EXTRA_VERSION);
const char linuxbios_build[] = STR(LINUXBIOS_BUILD);

const char linuxbios_compile_time[]   = STR(LINUXBIOS_COMPILE_TIME);
const char linuxbios_compile_by[]     = STR(LINUXBIOS_COMPILE_BY);
const char linuxbios_compile_host[]   = STR(LINUXBIOS_COMPILE_HOST);
const char linuxbios_compile_domain[] = STR(LINUXBIOS_COMPILE_DOMAIN);
const char linuxbios_compiler[]       = STR(LINUXBIOS_COMPILER);
const char linuxbios_linker[]         = STR(LINUXBIOS_LINKER);
const char linuxbios_assembler[]      = STR(LINUXBIOS_ASSEMBLER);




