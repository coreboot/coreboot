/* SPDX-License-Identifier: GPL-2.0-only */
#define SUPERIO_DEV		SIO0
#define SUPERIO_PNP_BASE	CONFIG_SUPERIO_NUVOTON_PNP_BASE

/* Defines to have ASLs below make visible some devices */
#if !CONFIG(NO_UART_ON_SUPERIO)
#define NCT677X_SHOW_SP1
#endif

/* TODO: Exclude next lines for variants without PS/2 port */
#define NCT677X_SHOW_KBC
#define SUPERIO_KBC_PS2KID	PS2K
#define SUPERIO_KBC_PS2MID	PS2M

/*
 * This series uses one of four SIO chips:
 * NCT6776 (p8c_ws)
 * NCT6779D
 * NCT5535D
 * IT8771E (P8H77-I and P8H77-V LE, both not yet supported)
 *
 * Include the correct ASL file.
 */

#if CONFIG(SUPERIO_NUVOTON_NCT6779D)
#include <superio/nuvoton/nct6779d/acpi/superio.asl>
#elif CONFIG(SUPERIO_NUVOTON_NCT5535D)
#include <superio/nuvoton/nct5535d/acpi/superio.asl>
#elif CONFIG(SUPERIO_NUVOTON_NCT6776)
#include <superio/nuvoton/nct6776/acpi/superio.asl>
#endif
