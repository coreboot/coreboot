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

#ifndef _BIOSEMU_MEM_H_
#define _BIOSEMU_MEM_H_
#include <x86emu/x86emu.h>
#include <types.h>

// read byte from memory
u8 my_rdb(u32 addr);

//read word from memory
u16 my_rdw(u32 addr);

//read long from memory
u32 my_rdl(u32 addr);

//write byte to memory
void my_wrb(u32 addr, u8 val);

//write word to memory
void my_wrw(u32 addr, u16 val);

//write long to memory
void my_wrl(u32 addr, u32 val);

#endif
