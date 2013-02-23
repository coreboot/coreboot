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

#ifndef EC_LENOVO_PMH7_H
#define EC_LENOVO_PMH7_H

#define EC_LENOVO_PMH7_INDEX 0x77

#define EC_LENOVO_PMH7_BASE 0x15e0
#define EC_LENOVO_PMH7_ADDR (EC_LENOVO_PMH7_BASE + 0x0c)
#define EC_LENOVO_PMH7_DATA (EC_LENOVO_PMH7_BASE + 0x0e)

void pmh7_register_set_bit(int reg, int bit);
void pmh7_register_clear_bit(int reg, int bit);
char pmh7_register_read(int reg);
void pmh7_register_write(int reg, int val);

void pmh7_backlight_enable(int onoff);
void pmh7_dock_event_enable(int onoff);
void pmh7_touchpad_enable(int onoff);
void pmh7_ultrabay_power_enable(int onoff);
#endif
