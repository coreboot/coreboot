/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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


#ifdef __PRE_RAM__

#include <arch/cpu.h>
#include "southbridge/amd/sb600/sb600.h"

#else

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "southbridge/amd/sb600/sb600.h"
#include <delay.h>

#endif /* __PRE_RAM__ */

#include "speaker.h"

void speaker_init(uint8_t time) {
   /* SB600 RRG.
    * Options_0 - RW - 8 bits - [PM_Reg: 60h].
    * SpkrEn, bit[5]=1b, Setting this bit will configure GPIO2 to be speaker output.
    */
#ifdef __PRE_RAM__
   pmio_write(0x60, (pmio_read(0x60) | (1<<5)));
#else
   pm_iowrite(0x60, (pm_ioread(0x60) | (1<<5)));
#endif /* __PRE_RAM__ */

   /* SB600 RRG.
    * Tmr1CntrlWord - RW - 8 bits - [IO_Reg: 43h].
    * ModeSelect, bit[3:1]=011b, Square wave output.
    * CmmandSelect, bit[5:4]=11b, Read/write least, and then most significant byte.
    * CounterSelect, bit[7:6]=10b, Select counter 2.
    */
   outb(0xb6, 0x43);


   /* SB600 RRG.
    * TimerCh2- RW - 8 bits - [IO_Reg: 42h].
    */
   outb(time, 0x42);
}

void speaker_on_nodelay(void) {
   /* SB600 RRG.
    * Nmi_Status - RW - 8 bits - [IO_Reg: 61h].
    * SpkrEnable, bit[0]=1b, Enable counter 2
    * SpkrTmrEnable, bit[1]=1b, Speaker timer on
    */
   outb(inb(0x61) | 0x03, 0x61);
}

void speaker_on_delay(void) {
   speaker_on_nodelay();
   mdelay(100);
}

void speaker_off_nodelay(void) {
   /* SB600 RRG.
    * Nmi_Status - RW - 8 bits - [IO_Reg: 61h].
    * SpkrEnable, bit[0]=0b, Disable counter 2
    * SpkrTmrEnable, bit[1]=0b, Speaker timer off
    */
   outb(inb(0x61) & ~0x03, 0x61);
}

void speaker_off_delay(void) {
   speaker_off_nodelay();
   mdelay(100);
}
