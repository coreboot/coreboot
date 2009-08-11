/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#ifndef _BIOSEMU_IO_H_
#define _BIOSEMU_IO_H_
#include <x86emu/x86emu.h>
#include <types.h>

u8 my_inb(X86EMU_pioAddr addr);

u16 my_inw(X86EMU_pioAddr addr);

u32 my_inl(X86EMU_pioAddr addr);

void my_outb(X86EMU_pioAddr addr, u8 val);

void my_outw(X86EMU_pioAddr addr, u16 val);

void my_outl(X86EMU_pioAddr addr, u32 val);

#endif
