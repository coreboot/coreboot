/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/lapic.h>
#include <console/console.h>

void lapic_virtual_wire_mode_init(void)
{
	/* this is so interrupts work. This is very limited scope --
	 * linux will do better later, we hope ...
	 */
	/* this is the first way we learned to do it. It fails on real SMP
	 * stuff. So we have to do things differently ...
	 * see the Intel mp1.4 spec, page A-3
	 */

	printk(BIOS_INFO, "Setting up local APIC...\n");

	/* Enable the local APIC */
	enable_lapic();

	/*
	 * Set Task Priority to 'accept all'.
	 */
	lapic_write_around(LAPIC_TASKPRI,
		lapic_read_around(LAPIC_TASKPRI) & ~LAPIC_TPRI_MASK);

	/* Put the local APIC in virtual wire mode */
	lapic_write_around(LAPIC_SPIV,
		(lapic_read_around(LAPIC_SPIV) & ~(LAPIC_VECTOR_MASK))
		| LAPIC_SPIV_ENABLE);
	lapic_write_around(LAPIC_LVT0,
		(lapic_read_around(LAPIC_LVT0) &
			~(LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER |
				LAPIC_LVT_REMOTE_IRR | LAPIC_INPUT_POLARITY |
				LAPIC_SEND_PENDING | LAPIC_LVT_RESERVED_1 |
				LAPIC_DELIVERY_MODE_MASK))
		| (LAPIC_LVT_REMOTE_IRR | LAPIC_SEND_PENDING |
			LAPIC_DELIVERY_MODE_EXTINT)
		);
	lapic_write_around(LAPIC_LVT1,
		(lapic_read_around(LAPIC_LVT1) &
			~(LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER |
				LAPIC_LVT_REMOTE_IRR | LAPIC_INPUT_POLARITY |
				LAPIC_SEND_PENDING | LAPIC_LVT_RESERVED_1 |
				LAPIC_DELIVERY_MODE_MASK))
		| (LAPIC_LVT_REMOTE_IRR | LAPIC_SEND_PENDING |
			LAPIC_DELIVERY_MODE_NMI)
		);

	printk(BIOS_DEBUG, " apic_id: 0x%x ", lapicid());
	printk(BIOS_INFO, "done.\n");
}
