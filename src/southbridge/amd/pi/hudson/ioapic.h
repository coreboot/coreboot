/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_IOAPIC_H
#define AMD_BLOCK_IOAPIC_H

/* Since the old APIC bus isn't used any more, the IOAPIC IDs could be < CONFIG_MAX_CPUS */
#define FCH_IOAPIC_ID		(CONFIG_MAX_CPUS)
#define GNB_IOAPIC_ID		(CONFIG_MAX_CPUS + 1)

#endif /* AMD_BLOCK_IOAPIC_H */
