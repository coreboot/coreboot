/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _RISCV_SMP_H
#define _RISCV_SMP_H

unsigned int smp_get_hart_count(void);

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
