/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SUPERIO_WINBOND_W83627DHG_W83627DHG_H
#define SUPERIO_WINBOND_W83627DHG_W83627DHG_H

#define W83627DHG_FDC              0   /* Floppy */
#define W83627DHG_PP               1   /* Parallel port */
#define W83627DHG_SP1              2   /* Com1 */
#define W83627DHG_SP2              3   /* Com2 */
#define W83627DHG_KBC              5   /* PS/2 keyboard & mouse */
#define W83627DHG_SPI              6   /* Serial peripheral interface */
#define W83627DHG_WDTO_PLED        8   /* WDTO#, PLED */
#define W83627DHG_ACPI            10   /* ACPI */
#define W83627DHG_HWM             11   /* Hardware monitor */
#define W83627DHG_PECI_SST        12   /* PECI, SST */

/* The following are handled using "virtual LDNs" (hence the _V suffix). */
#define W83627DHG_GPIO6_V          7   /* GPIO6 */
#define W83627DHG_GPIO2345_V       9   /* GPIO2, GPIO3, GPIO4, GPIO5 */

/*
 * Virtual devices sharing the enables are encoded as follows:
 *   VLDN = baseLDN[7:0] | [10:8] bitpos of enable in 0x30 of baseLDN
 */

/* GPIO6 has bit 3 as enable (instead of bit 0 as usual). */
#define W83627DHG_GPIO6	((3 << 8) | W83627DHG_GPIO6_V)

#define W83627DHG_GPIO2	((0 << 8) | W83627DHG_GPIO2345_V)
#define W83627DHG_GPIO3	((1 << 8) | W83627DHG_GPIO2345_V)
#define W83627DHG_GPIO4	((2 << 8) | W83627DHG_GPIO2345_V)
#define W83627DHG_GPIO5	((3 << 8) | W83627DHG_GPIO2345_V)

/* Note: There is no GPIO1 on the W83627DHG as per datasheet. */

void pnp_enter_ext_func_mode(device_t dev);
void pnp_exit_ext_func_mode(device_t dev);
void w83627dhg_enable_serial(device_t dev, u16 iobase);
void w83627dhg_enable_i2c(device_t dev);
void w83627dhg_set_clksel_48(device_t dev);

#endif
