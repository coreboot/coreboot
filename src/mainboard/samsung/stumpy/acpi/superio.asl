/* SPDX-License-Identifier: GPL-2.0-only */

/* Values should match those defined in devicetree.cb */

#undef SIO_ENABLE_FDC0           // pnp 2e.0: Disable Floppy Controller
#undef SIO_ENABLE_INFR           // pnp 2e.a: Disable Consumer IR

#define SIO_ENABLE_PS2K          // pnp 2e.5: Enable PS/2 Keyboard
#define SIO_ENABLE_PS2M          // pnp 2e.6: Enable PS/2 Mouse
#define SIO_ENABLE_COM1          // pnp 2e.1: Enable Serial Port 1
#define SIO_ENABLE_ENVC          // pnp 2e.4: Enable Environmental Controller
#define SIO_ENVC_IO0      0x700  // pnp 2e.4: io 0x60
#define SIO_ENVC_IO1      0x710  // pnp 2e.4: io 0x62
#define SIO_ENABLE_GPIO		 // pnp 2e.7: Enable GPIO
#define SIO_GPIO_IO0      0x720  // pnp 2e.7: io 0x60
#define SIO_GPIO_IO1      0x730  // pnp 2e.7: io 0x60

#include <superio/ite/it8772f/acpi/superio.asl>
