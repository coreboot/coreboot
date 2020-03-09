/* SPDX-License-Identifier: GPL-2.0-only */

	/* PCIe Configuration Space for CONFIG_MMCONF_BUS_NUMBER busses */
	OperationRegion(PCFG, SystemMemory, PCBA, PCLN) /* Each bus consumes 1MB */
		Field(PCFG, ByteAcc, NoLock, Preserve) {
		/* Byte offsets are computed using the following technique:
		 * ((bus number + 1) * ((device number * 8) * 4096)) + register offset
		 * The 8 comes from 8 functions per device, and 4096 bytes per function config space
		*/
		Offset(0x00088024),	/* Byte offset to SATA register 24h - Bus 0, Device 17, Function 0 */
		STB5, 32,
		Offset(0x00098042),	/* Byte offset to OHCI0 register 42h - Bus 0, Device 19, Function 0 */
		PT0D, 1,
		PT1D, 1,
		PT2D, 1,
		PT3D, 1,
		PT4D, 1,
		PT5D, 1,
		PT6D, 1,
		PT7D, 1,
		PT8D, 1,
		PT9D, 1,
		Offset(0x000a0004),	/* Byte offset to SMBUS register 4h - Bus 0, Device 20, Function 0 */
		SBIE, 1,
		SBME, 1,
		Offset(0x000a0008),	/* Byte offset to SMBUS register 8h - Bus 0, Device 20, Function 0 */
		SBRI, 8,
		Offset(0x000a0014),	/* Byte offset to SMBUS register 14h - Bus 0, Device 20, Function 0 */
		SBB1, 32,
		Offset(0x000a0078),	/* Byte offset to SMBUS register 78h - Bus 0, Device 20, Function 0 */
		,14,
		P92E, 1,		/* Port92 decode enable */
	}

	OperationRegion(SB5, SystemMemory, STB5, 0x1000)
		Field(SB5, AnyAcc, NoLock, Preserve){
		/* Port 0 */
		Offset(0x120),		/* Port 0 Task file status */
		P0ER, 1,
		, 2,
		P0DQ, 1,
		, 3,
		P0BY, 1,
		Offset(0x128),		/* Port 0 Serial ATA status */
		P0DD, 4,
		, 4,
		P0IS, 4,
		Offset(0x12c),		/* Port 0 Serial ATA control */
		P0DI, 4,
		Offset(0x130),		/* Port 0 Serial ATA error */
		, 16,
		P0PR, 1,

		/* Port 1 */
		offset(0x1a0),		/* Port 1 Task file status */
		P1ER, 1,
		, 2,
		P1DQ, 1,
		, 3,
		P1BY, 1,
		Offset(0x1a8),		/* Port 1 Serial ATA status */
		P1DD, 4,
		, 4,
		P1IS, 4,
		Offset(0x1ac),		/* Port 1 Serial ATA control */
		P1DI, 4,
		Offset(0x1b0),		/* Port 1 Serial ATA error */
		, 16,
		P1PR, 1,

		/* Port 2 */
		Offset(0x220),		/* Port 2 Task file status */
		P2ER, 1,
		, 2,
		P2DQ, 1,
		, 3,
		P2BY, 1,
		Offset(0x228),		/* Port 2 Serial ATA status */
		P2DD, 4,
		, 4,
		P2IS, 4,
		Offset(0x22c),		/* Port 2 Serial ATA control */
		P2DI, 4,
		Offset(0x230),		/* Port 2 Serial ATA error */
		, 16,
		P2PR, 1,

		/* Port 3 */
		Offset(0x2a0),		/* Port 3 Task file status */
		P3ER, 1,
		, 2,
		P3DQ, 1,
		, 3,
		P3BY, 1,
		Offset(0x2a8),		/* Port 3 Serial ATA status */
		P3DD, 4,
		, 4,
		P3IS, 4,
		Offset(0x2aC),		/* Port 3 Serial ATA control */
		P3DI, 4,
		Offset(0x2b0),		/* Port 3 Serial ATA error */
		, 16,
		P3PR, 1,
	}

	Method(\_PIC, 0x01, NotSerialized)
	{
		If (Arg0)
		{
			\_SB.CIRQ()
		}
		printf("PIC MODE: %o", Arg0)
		Store(Arg0, PMOD)
	}

	Method(CIRQ, 0x00, NotSerialized){
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
