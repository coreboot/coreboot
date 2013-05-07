/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#ifndef SUPERIO_WINBOND_W83627EHG_W83627EHG_H
#define SUPERIO_WINBOND_W83627EHG_W83627EHG_H

#define W83627EHG_FDC              0   /* Floppy */
#define W83627EHG_PP               1   /* Parallel port */
#define W83627EHG_SP1              2   /* Com1 */
#define W83627EHG_SP2              3   /* Com2 */
#define W83627EHG_KBC              5   /* PS/2 keyboard & mouse */
#define W83627EHG_WDTO_PLED        8   /* Watchdog timer timeout, power LED */
#define W83627EHG_ACPI            10   /* ACPI */
#define W83627EHG_HWM             11   /* Hardware monitor */

/* The following are handled using "virtual LDNs" (hence the _V suffix). */
#define W83627EHG_SFI_V            6   /* Serial flash interface (SFI) */
#define W83627EHG_GPIO_GAME_MIDI_V 7   /* GPIO1, GPIO6, game port, MIDI */
#define W83627EHG_GPIO_SUSLED_V    9   /* GPIO2, GPIO3, GPIO4, GPIO5, SUSLED */

/*
 * Virtual devices sharing the enables are encoded as follows:
 *   VLDN = baseLDN[7:0] | [10:8] bitpos of enable in 0x30 of baseLDN
 */

/* SFI has bit 1 as enable (instead of bit 0 as usual). */
#define W83627EHG_SFI	((1 << 8) | W83627EHG_SFI_V)

#define W83627EHG_GPIO1	((0 << 8) | W83627EHG_GPIO_GAME_MIDI_V)
#define W83627EHG_GAME	((1 << 8) | W83627EHG_GPIO_GAME_MIDI_V)
#define W83627EHG_MIDI	((2 << 8) | W83627EHG_GPIO_GAME_MIDI_V)
#define W83627EHG_GPIO6	((3 << 8) | W83627EHG_GPIO_GAME_MIDI_V)

#define W83627EHG_GPIO2	((0 << 8) | W83627EHG_GPIO_SUSLED_V)
#define W83627EHG_GPIO3	((1 << 8) | W83627EHG_GPIO_SUSLED_V)
#define W83627EHG_GPIO4	((2 << 8) | W83627EHG_GPIO_SUSLED_V)
#define W83627EHG_GPIO5	((3 << 8) | W83627EHG_GPIO_SUSLED_V)

#if defined(__PRE_RAM__)
void w83627ehg_enable_dev(device_t dev, u16 iobase);
void w83627ehg_disable_dev(device_t dev);
void w83627ehg_enable_serial(device_t dev, u16 iobase);
#endif

#endif
