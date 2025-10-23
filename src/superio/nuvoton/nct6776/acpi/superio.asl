/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME NCT6776
#endif

#ifndef SUPERIO_FULL_CHIP_NAME
#define SUPERIO_FULL_CHIP_NAME "Nuvoton NCT6776 Super I/O"
#endif

#ifdef NCT6776_SHOW_PP
#define NCT677X_SHOW_PP
#endif
#ifdef NCT6776_SHOW_SP1
#define NCT677X_SHOW_SP1
#endif
#ifdef NCT6776_SHOW_SP2
#define NCT677X_SHOW_SP2
#endif
#ifdef NCT6776_SHOW_KBC
#define NCT677X_SHOW_KBC
#endif
#ifdef NCT6776_SHOW_HWM
#define NCT677X_SHOW_HWM
#endif
#ifdef NCT6776_SHOW_GPIO
#define NCT677X_SHOW_GPIO
#endif

#include <superio/nuvoton/common/acpi/superio.asl>
