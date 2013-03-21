/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <arch/io.h>

static void enable_spi_prefetch(void)
{
        u8 reg8;
        device_t dev;

        dev = PCI_DEV(0, 0x1f, 0);

        reg8 = pci_read_config8(dev, 0xdc);
        reg8 &= ~(3 << 2);
        reg8 |= (2 << 2); /* Prefetching and Caching Enabled */
        pci_write_config8(dev, 0xdc, reg8);
}

static void bootblock_southbridge_init(void)
{
        enable_spi_prefetch();
}

