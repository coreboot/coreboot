/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PCI_INT_TYPES_H
#define AMD_PCI_INT_TYPES_H

const char *intr_types[] = {
	[0x00] = "INTA#\t", "INTB#\t", "INTC#\t", "INTD#\t", "INTE#\t", "INTF#\t", "INTG#\t", "INTH#\t",
	[0x08] = "Misc\t", "Misc0\t", "Misc1\t", "Misc2\t", "Ser IRQ INTA", "Ser IRQ INTB", "Ser IRQ INTC", "Ser IRQ INTD",
	[0x10] = "SCI\t", "SMBUS0\t", "ASF\t", "HDA\t", "FC\t\t", "GEC\t", "PerMon\t", "SD\t\t",
	[0x20] = "IMC INT0\t", "IMC INT1\t", "IMC INT2\t", "IMC INT3\t", "IMC INT4\t", "IMC INT5\t",
	[0x30] = "Dev18.0 INTA", "Dev18.2 INTB", "Dev19.0 INTA", "Dev19.2 INTB", "Dev22.0 INTA", "Dev22.2 INTB", "Dev20.5 INTC",
	[0x7F] = "RSVD\t",
#if CONFIG(SOUTHBRIDGE_AMD_PI_AVALON)
	[0x40] = "RSVD\t", "SATA\t",
	[0x60] = "RSVD\t", "RSVD\t", "GPIO\t",
#elif CONFIG(SOUTHBRIDGE_AMD_PI_KERN)
	[0x40] = "IDE\t", "SATA\t",
	[0x50] = "GPPInt0\t", "GPPInt1\t", "GPPInt2\t", "GPPInt3\t",
	[0x62] = "GPIO\t",
	[0x70] = "I2C0\t", "I2C1\t", "I2C2\t", "I2C3\t", "UART0\t", "UART1\t",
#endif
};

#endif /* AMD_PCI_INT_TYPES_H */
