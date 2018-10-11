/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux.
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

#ifndef _RISCV_SMP_H
#define _RISCV_SMP_H

/*
 * This function is used to pause smp. Only the hart with hartid equal
 * to working_hartid can be returned from smp_pause, other harts will
 * enter halt and wait for wakeup
 */
void smp_pause(int working_hartid);

/*
 * This function is used to wake up the harts that are halted by the
 * smp_pause function. And this function will not return, all hart will
 * jump to fn execution, and arg is the argument of the function fn.
 */
void smp_resume(void (*fn)(void *), void *arg);

#endif
