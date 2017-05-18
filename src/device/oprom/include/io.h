/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#if IS_ENABLED(CONFIG_ARCH_X86)
#include <arch/io.h>
#else
void outb(u8 val, u16 port);
void outw(u16 val, u16 port);
void outl(u32 val, u16 port);

u8 inb(u16 port);
u16 inw(u16 port);
u32 inl(u16 port);
#endif
