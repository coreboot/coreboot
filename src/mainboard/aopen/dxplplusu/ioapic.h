// IOAPIC addresses determined by coreboot enumeration. 
// Someday add functions to get APIC IDs and versions from the chips themselves.
	
#define IOAPIC_ICH4		2
#define IOAPIC_P64H2_BUS_B	3	// IOAPIC 3 at 02:1c.0  MBAR = fe300000 DataAddr = fe300010
#define IOAPIC_P64H2_BUS_A	4	// IOAPIC 4 at 02:1e.0  MBAR = fe301000 DataAddr = fe301010

#define P64H2_IOAPIC_VERSION		0x20
#define INTEL_IOAPIC_NUM_INTERRUPTS 	24	// Both ICH-4 and P64-H2
