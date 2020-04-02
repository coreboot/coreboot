/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Values should match those defined in devicetree.cb */

#define SIO_ENABLE_SPM1          // pnp 2e.1: Enable ACPI PM1 Block
#define SIO_SPM1_IO0      0xb00  // pnp 2e.1: io 0x60

#undef SIO_ENABLE_SEC1           // pnp 2e.2: Disable EC 1

#undef SIO_ENABLE_SEC2           // pnp 2e.3: Disable EC 2

#undef SIO_ENABLE_SSP1           // pnp 2e.4: Disable UART

#define SIO_ENABLE_SKBC          // pnp 2e.7: Enable Keyboard

#undef SIO_ENABLE_SEC0           // pnp 2e.8: Already exported as EC

#define SIO_ENABLE_SMBX          // pnp 2e.9: Enable Mailbox
#define SIO_SMBX_IO0      0xa00  // pnp 2e.9: io 0xa00

#include <superio/smsc/mec1308/acpi/superio.asl>
