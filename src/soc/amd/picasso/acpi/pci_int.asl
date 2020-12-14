/* SPDX-License-Identifier: GPL-2.0-only */

	Method(\_PIC, 0x01, NotSerialized)
	{
		printf("PIC MODE: %o", Arg0)
		PMOD = Arg0
	}

	/* PIC Possible Resource Values */
	Name(IRQP, ResourceTemplate() {
		Interrupt(ResourceConsumer, Level, ActiveLow, Exclusive, , , PIC){
			1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15
		}
	})

	/* IO-APIC Possible Resource Values */
	Name(IRQI, ResourceTemplate() {
		Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , APIC) {
			16, 17, 18, 19, 20, 21, 22, 23
		}
	})

#define PCI_LINK(DEV_NAME, PIC_REG, APIC_REG) \
	Device(DEV_NAME) { \
		Name(_HID, EISAID("PNP0C0F")) \
		Name(_UID, 1) \
\
		Method(_STA, 0) { \
			If (PMOD) { \
				local0=APIC_REG \
			} Else { \
				local0=PIC_REG \
			} \
\
			If (local0 != 0x1f) { \
				printf("PCI: \\_SB.%s._STA: %o, Enabled", #DEV_NAME, local0) \
				/* Present, Enabled, Functional */ \
				Return(0x0b) \
			} else { \
				printf("PCI: \\_SB.%s._STA: %o, Disabled", #DEV_NAME, local0) \
				/* Present, Functional */ \
				Return(0x09) \
			} \
		} \
\
		Method(_DIS ,0) { \
			If(PMOD) { \
				printf("PCI: \\_SB.%s._DIS APIC", #DEV_NAME) \
				APIC_REG=0x1f \
			} Else { \
				printf("PCI: \\_SB.%s._DIS PIC", #DEV_NAME) \
				PIC_REG=0x1f \
			} \
		} \
\
		Method(_PRS ,0) { \
			If(PMOD) { \
				printf("PCI: \\_SB.%s._PRS => APIC", #DEV_NAME) \
				Return(IRQI) \
			} Else { \
				printf("PCI: \\_SB.%s._PRS => PIC", #DEV_NAME) \
				Return(IRQP) \
			} \
		} \
\
		Method(_CRS ,0) { \
			local0=ResourceTemplate(){ \
				Interrupt ( \
					ResourceConsumer, \
					Level, \
					ActiveLow, \
					Exclusive, , , NUMB) \
				{ 0 } \
			} \
			CreateDWordField(local0, NUMB._INT, IRQN) \
			If(PMOD) { \
				printf("PCI: \\_SB.%s._CRS APIC: %o", #DEV_NAME, APIC_REG) \
				IRQN=APIC_REG \
			} Else { \
				printf("PCI: \\_SB.%s._CRS PIC: %o", #DEV_NAME, PIC_REG) \
				IRQN=PIC_REG \
			} \
			If (IRQN == 0x1f) { \
				Return(ResourceTemplate(){}) \
			} Else { \
				Return(local0) \
			} \
		} \
\
		Method(_SRS, 1) { \
			CreateWordField(ARG0, 0x5, IRQN) \
\
			If(PMOD) { \
				printf("PCI: \\_SB.%s._SRS APIC: %o", #DEV_NAME, IRQN) \
				APIC_REG=IRQN \
			} Else { \
				printf("PCI: \\_SB.%s._SRS PIC: %o", #DEV_NAME, IRQN) \
				PIC_REG=IRQN \
			} \
		} \
	}

PCI_LINK(INTA, PIRA, IORA)
PCI_LINK(INTB, PIRB, IORB)
PCI_LINK(INTC, PIRC, IORC)
PCI_LINK(INTD, PIRD, IORD)
PCI_LINK(INTE, PIRE, IORE)
PCI_LINK(INTF, PIRF, IORF)
PCI_LINK(INTG, PIRG, IORG)
PCI_LINK(INTH, PIRH, IORH)
