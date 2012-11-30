/****************************************************************************
 * YABEL BIOS Emulator
 *
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Copyright (c) 2008 Pattrick Hueper <phueper@hueper.net>
 ****************************************************************************/

#ifndef _BIOSEMU_COMPAT_TIME_H
#define _BIOSEMU_COMPAT_TIME_H

/* TODO: check how this works in x86 */
extern unsigned long tb_freq;
u64 get_time(void);
#endif
