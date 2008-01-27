/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

/* TODO: This structure should contain typematic settings, but coreboot
 * does not care yet.
 */
struct pc_keyboard {
};

/* This function is called in the Super-IO code. */
void init_pc_keyboard(unsigned int port0, unsigned int port1,
		      struct pc_keyboard *kbd);

#endif /* KEYBOARD_H */
