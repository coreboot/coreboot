/* SPDX-License-Identifier: GPL-2.0-only */

/* Values should match those defined in devicetree.cb */

#undef SIO_ENABLE_FDC0           // pnp 2e.0: Disable Floppy Controller
#undef SIO_ENABLE_COM1           // pnp 2e.1: Disable Serial Port 1
#undef SIO_ENABLE_PS2K           // pnp 2e.5: Disable PS/2 Keyboard
#undef SIO_ENABLE_PS2M           // pnp 2e.6: Disable PS/2 Mouse
#undef SIO_ENABLE_INFR           // pnp 2e.a: Disable Consumer IR

#define SIO_ENABLE_ENVC          // pnp 2e.4: Enable Environmental Controller
#define SIO_ENVC_IO0      0xa30  // pnp 2e.4: io 0x60
#define SIO_ENVC_IO1      0xa20  // pnp 2e.4: io 0x62
#define SIO_ENABLE_GPIO          // pnp 2e.7: Enable GPIO
#define SIO_GPIO_IO0      0x0    // pnp 2e.7: io 0x60
#define SIO_GPIO_IO1      0xa00  // pnp 2e.7: io 0x62

#include <superio/ite/it8772f/acpi/superio.asl>
