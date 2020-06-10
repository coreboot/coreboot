/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <smp/node.h>

void do_lapic_init(void)
{
	uint32_t lvt0_val;

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

	lvt0_val = (lapic_read_around(LAPIC_LVT0) &
				~(LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER |
				LAPIC_LVT_REMOTE_IRR | LAPIC_INPUT_POLARITY |
				LAPIC_SEND_PENDING | LAPIC_LVT_RESERVED_1)) |
				(LAPIC_LVT_REMOTE_IRR | LAPIC_SEND_PENDING);
	if (boot_cpu())
		lvt0_val = SET_LAPIC_DELIVERY_MODE(lvt0_val, LAPIC_MODE_EXINT);
	lapic_write_around(LAPIC_LVT0, lvt0_val);

	lapic_write_around(LAPIC_LVT1,
		(lapic_read_around(LAPIC_LVT1) &
			~(LAPIC_LVT_MASKED | LAPIC_LVT_LEVEL_TRIGGER |
				LAPIC_LVT_REMOTE_IRR | LAPIC_INPUT_POLARITY |
				LAPIC_SEND_PENDING | LAPIC_LVT_RESERVED_1 |
				LAPIC_DELIVERY_MODE_MASK))
		| (LAPIC_LVT_REMOTE_IRR | LAPIC_SEND_PENDING |
			LAPIC_DELIVERY_MODE_NMI)
		);

	printk(BIOS_DEBUG, " apic_id: 0x%02lx ", lapicid());
	printk(BIOS_INFO, "done.\n");
}
