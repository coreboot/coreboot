/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#include <string.h>
#include <stdint.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include "southbridge/via/vt8237r/vt8237r.h"
#include "southbridge/via/k8t890/k8x8xx.h"

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	mptable_write_buses(mc, NULL, &bus_isa);

	/* I/O APICs:	APIC ID	Version	State		Address */
	smp_write_ioapic(mc, VT8237R_APIC_ID, 0x20, IO_APIC_ADDR);
	smp_write_ioapic(mc, K8T890_APIC_ID, 0x20, K8T890_APIC_BASE);

	mptable_add_isa_interrupts(mc, bus_isa, VT8237R_APIC_ID, 0);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xb << 2) | 0, VT8237R_APIC_ID, 0x10); //IRQ16
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xb << 2) | 1, VT8237R_APIC_ID, 0x11); //IRQ17
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xb << 2) | 2, VT8237R_APIC_ID, 0x12); //IRQ18
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xb << 2) | 3, VT8237R_APIC_ID, 0x13); //IRQ19

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xc << 2) | 0, VT8237R_APIC_ID, 0x11); //IRQ17
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xc << 2) | 1, VT8237R_APIC_ID, 0x12); //IRQ18
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xc << 2) | 2, VT8237R_APIC_ID, 0x13); //IRQ19
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xc << 2) | 3, VT8237R_APIC_ID, 0x10); //IRQ16

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xd << 2) | 0, VT8237R_APIC_ID, 0x12); //IRQ18
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xd << 2) | 1, VT8237R_APIC_ID, 0x13); //IRQ19
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xd << 2) | 2, VT8237R_APIC_ID, 0x10); //IRQ16
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xd << 2) | 3, VT8237R_APIC_ID, 0x11); //IRQ17

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xf << 2) | 0, VT8237R_APIC_ID, 0x14);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0xf << 2) | 1, VT8237R_APIC_ID, 0x14);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x10 << 2) | 0, VT8237R_APIC_ID, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x10 << 2) | 1, VT8237R_APIC_ID, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x10 << 2) | 2, VT8237R_APIC_ID, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x11 << 2) | 2, VT8237R_APIC_ID, 0x16);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x2 << 2) | 0, K8T890_APIC_ID, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x2 << 2) | 1, K8T890_APIC_ID, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x2 << 2) | 2, K8T890_APIC_ID, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x2 << 2) | 3, K8T890_APIC_ID, 0x3);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x3 << 2) | 0, K8T890_APIC_ID, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x3 << 2) | 1, K8T890_APIC_ID, 0xb);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x3 << 2) | 2, K8T890_APIC_ID, 0xf);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0,  (0x3 << 2) | 3, K8T890_APIC_ID, 0x13);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2,  (0x00 << 2) | 0, K8T890_APIC_ID, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2,  (0x00 << 2) | 1, K8T890_APIC_ID, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2,  (0x00 << 2) | 2, K8T890_APIC_ID, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2,  (0x00 << 2) | 3, K8T890_APIC_ID, 0x3);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3,  (0x00 << 2) | 0, K8T890_APIC_ID, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3,  (0x00 << 2) | 1, K8T890_APIC_ID, 0x5);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3,  (0x00 << 2) | 2, K8T890_APIC_ID, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3,  (0x00 << 2) | 3, K8T890_APIC_ID, 0x7);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4,  (0x00 << 2) | 0, K8T890_APIC_ID, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4,  (0x00 << 2) | 1, K8T890_APIC_ID, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4,  (0x00 << 2) | 2, K8T890_APIC_ID, 0xa);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4,  (0x00 << 2) | 3, K8T890_APIC_ID, 0xb);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x5,  (0x00 << 2) | 0, K8T890_APIC_ID, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x5,  (0x00 << 2) | 1, K8T890_APIC_ID, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x5,  (0x00 << 2) | 2, K8T890_APIC_ID, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x5,  (0x00 << 2) | 3, K8T890_APIC_ID, 0xf);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x6,  (0x00 << 2) | 0, K8T890_APIC_ID, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x6,  (0x00 << 2) | 1, K8T890_APIC_ID, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x6,  (0x00 << 2) | 2, K8T890_APIC_ID, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x6,  (0x00 << 2) | 3, K8T890_APIC_ID, 0x13);

	/* Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	mptable_lintsrc(mc, bus_isa);
	/* There is no extension information... */

	/* Compute the checksums. */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
