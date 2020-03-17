/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef DCP847SKE_USB_H
#define DPC847SKE_USB_H

USB_CONFIG(1, 1, 0), /* back, towards HDMI plugs */
USB_CONFIG(1, 1, 0), /* back, towards power plug */
USB_CONFIG(1, 1, 1), /* half-width miniPCIe */
USB_CONFIG(1, 1, 1), /* full-width miniPCIe */
USB_CONFIG(1, 1, 2), /* front-panel header */
USB_CONFIG(1, 1, 2), /* front-panel header */
USB_CONFIG(1, 1, 3), /* front connector */
USB_CONFIG(0, 1, 3), /* not available */
USB_CONFIG(0, 1, 4), /* not available */
USB_CONFIG(0, 1, 4), /* not available */
USB_CONFIG(0, 1, 5), /* not available */
USB_CONFIG(0, 1, 5), /* not available */
USB_CONFIG(0, 1, 6), /* not available */
USB_CONFIG(0, 1, 6), /* not available */

#endif
