// IOAPIC addresses determined by coreboot enumeration.
// Someday add functions to get APIC IDs and versions from the chips themselves.

#define IOAPIC_ICH3				2
#define IOAPIC_P64H2_2_BUS_B	3	// IOAPIC 3 at 01:1c.0  MBAR = fe300000 DataAddr = fe300010
#define IOAPIC_P64H2_2_BUS_A	4	// IOAPIC 4 at 01:1e.0  MBAR = fe301000 DataAddr = fe301010
#define IOAPIC_P64H2_1_BUS_B	5	// IOAPIC 5 at 04:1c.0  MBAR = fe500000 DataAddr = fe500010
#define IOAPIC_P64H2_1_BUS_A	8	// IOAPIC 8 at 04:1e.0  MBAR = fe501000 DataAddr = fe501010

#define P64H2_IOAPIC_VERSION	0x20
#define INTEL_IOAPIC_NUM_INTERRUPTS 24		// Both ICH-3 and P64-H2
