/*
 *****************************************************************************
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * ***************************************************************************
 *
 */

#include <stdint.h>
#include <delay.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>

/* NOTE: We use the APIC TIMER register is to hold flags for AP init during
 * pre-memory init (__PRE_RAM__). Don't use init_timer() and  udelay is
 * redirected to udelay_tsc().
 */


void init_timer(void)
{
  /* Set the apic timer to no interrupts and periodic mode */
  lapic_write(LAPIC_LVTT, (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0));

  /* Set the divider to 1, no divider */
  lapic_write(LAPIC_TDCR, LAPIC_TDR_DIV_1);

  /* Set the initial counter to 0xffffffff */
  lapic_write(LAPIC_TMICT, 0xffffffff);
}


void udelay(u32 usecs)
{
  u32 start, value, ticks;
  /* Calculate the number of ticks to run, our FSB runs a 200Mhz */
  ticks = usecs * 200;
  start = lapic_read(LAPIC_TMCCT);
  do {
    value = lapic_read(LAPIC_TMCCT);
  } while((start - value) < ticks);

}
