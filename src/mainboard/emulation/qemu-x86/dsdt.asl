/*
 * Bochs/QEMU ACPI DSDT ASL definition
 *
 * Copyright (c) 2006 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		// DSDT revision: ACPI v2.0
	"COREv4",	// OEM id
	"COREBOOT",     // OEM table id
	0x20090419	// OEM revision
)
{

/****************************************************************
 * Debugging
 ****************************************************************/

    Scope (\)
    {
        /* Debug Output */
        OperationRegion (DBG, SystemIO, 0x0402, 0x01)
        Field (DBG, ByteAcc, NoLock, Preserve)
        {
            DBGB,   8,
        }

        /* Debug method - use this method to send output to the QEMU
         * BIOS debug port.  This method handles strings, integers,
         * and buffers.  For example: DBUG("abc") DBUG(0x123) */
        Method(DBUG, 1) {
            ToHexString(Arg0, Local0)
            ToBuffer(Local0, Local0)
            Subtract(SizeOf(Local0), 1, Local1)
            Store(Zero, Local2)
            While (LLess(Local2, Local1)) {
                Store(DerefOf(Index(Local0, Local2)), DBGB)
                Increment(Local2)
            }
            Store(0x0A, DBGB)
        }
    }

/****************************************************************
 * Processor
 ****************************************************************/

    Processor (CPAA, 0xAA, 0x0000b010, 0x06) {
        Name (ID, 0xAA)
        Name (_HID, "ACPI0007")
        Method(_MAT, 0) {
            Return(\_SB.CPMA(ID))
        }
        Method (_STA, 0) {
            Return(\_SB.CPST(ID))
        }
        Method (_EJ0, 1, NotSerialized) {
            \_SB.CPEJ(ID, Arg0)
        }
    }

/****************************************************************
 * PCI Bus definition
 ****************************************************************/

    Scope(\_SB) {
        Device(PCI0) {
            Name (_HID, EisaId ("PNP0A03"))
            Name (_ADR, 0x00)
            Name (_UID, 1)
            Name(_PRT, Package() {
                /* PCI IRQ routing table, example from ACPI 2.0a specification,
                   section 6.2.8.1 */
                /* Note: we provide the same info as the PCI routing
                   table of the Bochs BIOS */
#define prt_slot(nr, lnk0, lnk1, lnk2, lnk3) \
       Package() { nr##ffff, 0, lnk0, 0 }, \
       Package() { nr##ffff, 1, lnk1, 0 }, \
       Package() { nr##ffff, 2, lnk2, 0 }, \
       Package() { nr##ffff, 3, lnk3, 0 }

#define prt_slot0(nr) prt_slot(nr, LNKD, LNKA, LNKB, LNKC)
#define prt_slot1(nr) prt_slot(nr, LNKA, LNKB, LNKC, LNKD)
#define prt_slot2(nr) prt_slot(nr, LNKB, LNKC, LNKD, LNKA)
#define prt_slot3(nr) prt_slot(nr, LNKC, LNKD, LNKA, LNKB)
               prt_slot0(0x0000),
               /* Device 1 is power mgmt device, and can only use irq 9 */
               prt_slot(0x0001, LNKS, LNKB, LNKC, LNKD),
               prt_slot2(0x0002),
               prt_slot3(0x0003),
               prt_slot0(0x0004),
               prt_slot1(0x0005),
               prt_slot2(0x0006),
               prt_slot3(0x0007),
               prt_slot0(0x0008),
               prt_slot1(0x0009),
               prt_slot2(0x000a),
               prt_slot3(0x000b),
               prt_slot0(0x000c),
               prt_slot1(0x000d),
               prt_slot2(0x000e),
               prt_slot3(0x000f),
               prt_slot0(0x0010),
               prt_slot1(0x0011),
               prt_slot2(0x0012),
               prt_slot3(0x0013),
               prt_slot0(0x0014),
               prt_slot1(0x0015),
               prt_slot2(0x0016),
               prt_slot3(0x0017),
               prt_slot0(0x0018),
               prt_slot1(0x0019),
               prt_slot2(0x001a),
               prt_slot3(0x001b),
               prt_slot0(0x001c),
               prt_slot1(0x001d),
               prt_slot2(0x001e),
               prt_slot3(0x001f),
            })

            OperationRegion(PCST, SystemIO, 0xae00, 0x08)
            Field (PCST, DWordAcc, NoLock, WriteAsZeros)
            {
                PCIU, 32,
                PCID, 32,
            }

            OperationRegion(SEJ, SystemIO, 0xae08, 0x04)
            Field (SEJ, DWordAcc, NoLock, WriteAsZeros)
            {
                B0EJ, 32,
            }

            Name (CRES, ResourceTemplate ()
            {
                WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
                    0x0000,             // Address Space Granularity
                    0x0000,             // Address Range Minimum
                    0x00FF,             // Address Range Maximum
                    0x0000,             // Address Translation Offset
                    0x0100,             // Address Length
                    ,, )
                IO (Decode16,
                    0x0CF8,             // Address Range Minimum
                    0x0CF8,             // Address Range Maximum
                    0x01,               // Address Alignment
                    0x08,               // Address Length
                    )
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x0000,             // Address Space Granularity
                    0x0000,             // Address Range Minimum
                    0x0CF7,             // Address Range Maximum
                    0x0000,             // Address Translation Offset
                    0x0CF8,             // Address Length
                    ,, , TypeStatic)
                WordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
                    0x0000,             // Address Space Granularity
                    0x0D00,             // Address Range Minimum
                    0xFFFF,             // Address Range Maximum
                    0x0000,             // Address Translation Offset
                    0xF300,             // Address Length
                    ,, , TypeStatic)
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000,         // Address Space Granularity
                    0x000A0000,         // Address Range Minimum
                    0x000BFFFF,         // Address Range Maximum
                    0x00000000,         // Address Translation Offset
                    0x00020000,         // Address Length
                    ,, , AddressRangeMemory, TypeStatic)
                DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite,
                    0x00000000,         // Address Space Granularity
                    0xE0000000,         // Address Range Minimum
                    0xFEBFFFFF,         // Address Range Maximum
                    0x00000000,         // Address Translation Offset
                    0x1EC00000,         // Address Length
                    ,, PW32, AddressRangeMemory, TypeStatic)
            })
            Name (CR64, ResourceTemplate ()
            {
                QWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed, Cacheable, ReadWrite,
                    0x00000000,          // Address Space Granularity
                    0x8000000000,        // Address Range Minimum
                    0xFFFFFFFFFF,        // Address Range Maximum
                    0x00000000,          // Address Translation Offset
                    0x8000000000,        // Address Length
                    ,, PW64, AddressRangeMemory, TypeStatic)
            })
            Method (_CRS, 0)
            {
		/* see see acpi.h, struct bfld */
		External (BDAT, OpRegionObj)
		Field(BDAT, QWordAcc, NoLock, Preserve) {
                    P0S, 64,
                    P0E, 64,
                    P0L, 64,
                    P1S, 64,
                    P1E, 64,
                    P1L, 64,
		}
		Field(BDAT, DWordAcc, NoLock, Preserve) {
                    P0SL, 32,
                    P0SH, 32,
                    P0EL, 32,
                    P0EH, 32,
                    P0LL, 32,
                    P0LH, 32,
                    P1SL, 32,
                    P1SH, 32,
                    P1EL, 32,
                    P1EH, 32,
                    P1LL, 32,
                    P1LH, 32,
		}

                /* fixup 32bit pci io window */
		CreateDWordField (CRES,\_SB.PCI0.PW32._MIN, PS32)
		CreateDWordField (CRES,\_SB.PCI0.PW32._MAX, PE32)
		CreateDWordField (CRES,\_SB.PCI0.PW32._LEN, PL32)
		Store (P0SL, PS32)
		Store (P0EL, PE32)
		Store (P0LL, PL32)

		If (LAnd(LEqual(P1SL, 0x00), LEqual(P1SH, 0x00))) {
		    Return (CRES)
		} Else {
		    /* fixup 64bit pci io window */
		    CreateQWordField (CR64,\_SB.PCI0.PW64._MIN, PS64)
		    CreateQWordField (CR64,\_SB.PCI0.PW64._MAX, PE64)
		    CreateQWordField (CR64,\_SB.PCI0.PW64._LEN, PL64)
		    Store (P1S, PS64)
		    Store (P1E, PE64)
		    Store (P1L, PL64)
		    /* add window and return result */
		    ConcatenateResTemplate (CRES, CR64, Local0)
		    Return (Local0)
		}
            }
        }
    }


/****************************************************************
 * HPET
 ****************************************************************/

    Scope(\_SB) {
        Device(HPET) {
            Name(_HID,  EISAID("PNP0103"))
            Name(_UID, 0)
            OperationRegion(HPTM, SystemMemory , 0xFED00000, 0x400)
            Field(HPTM, DWordAcc, Lock, Preserve) {
                    VEND, 32,
                    PRD, 32,
            }
            Method (_STA, 0, NotSerialized) {
                    Store (VEND, Local0)
                    Store (PRD, Local1)
                    ShiftRight(Local0, 16, Local0)
                    If (LOr (LEqual(Local0, 0), LEqual(Local0, 0xffff))) {
                            Return (0x0)
                    }
                    If (LOr (LEqual(Local1, 0), LGreater(Local1, 100000000))) {
                            Return (0x0)
                    }
                    Return (0x0F)
            }
            Name(_CRS, ResourceTemplate() {
                DWordMemory(
                    ResourceConsumer, PosDecode, MinFixed, MaxFixed,
                    NonCacheable, ReadWrite,
                    0x00000000,
                    0xFED00000,
                    0xFED003FF,
                    0x00000000,
                    0x00000400 /* 1K memory: FED00000 - FED003FF */
                )
            })
        }
    }


/****************************************************************
 * VGA
 ****************************************************************/

    Scope(\_SB.PCI0) {
        Device (VGA) {
                 Name (_ADR, 0x00020000)
                 OperationRegion(PCIC, PCI_Config, Zero, 0x4)
                 Field(PCIC, DWordAcc, NoLock, Preserve) {
                         VEND, 32
                 }
                 Method (_S1D, 0, NotSerialized)
                 {
                         Return (0x00)
                 }
                 Method (_S2D, 0, NotSerialized)
                 {
                         Return (0x00)
                 }
                 Method (_S3D, 0, NotSerialized)
                 {
                         If (LEqual(VEND, 0x1001b36)) {
                                 Return (0x03)           // QXL
                         } Else {
                                 Return (0x00)
                         }
                 }
        }
    }


/****************************************************************
 * PIIX3 ISA bridge
 ****************************************************************/

    Scope(\_SB.PCI0) {
        Device (ISA) {
            Name (_ADR, 0x00010000)

            /* PIIX PCI to ISA irq remapping */
            OperationRegion (P40C, PCI_Config, 0x60, 0x04)
        }
    }


/****************************************************************
 * SuperIO devices (kbd, mouse, etc.)
 ****************************************************************/

    Scope(\_SB.PCI0.ISA) {
            /* Real-time clock */
            Device (RTC)
            {
                Name (_HID, EisaId ("PNP0B00"))
                Name (_CRS, ResourceTemplate ()
                {
                    IO (Decode16, 0x0070, 0x0070, 0x10, 0x02)
                    IRQNoFlags () {8}
                    IO (Decode16, 0x0072, 0x0072, 0x02, 0x06)
                })
            }

            /* Keyboard seems to be important for WinXP install */
            Device (KBD)
            {
                Name (_HID, EisaId ("PNP0303"))
                Method (_STA, 0, NotSerialized)
                {
                    Return (0x0f)
                }

                Method (_CRS, 0, NotSerialized)
                {
                     Name (TMP, ResourceTemplate ()
                     {
                    IO (Decode16,
                        0x0060,             // Address Range Minimum
                        0x0060,             // Address Range Maximum
                        0x01,               // Address Alignment
                        0x01,               // Address Length
                        )
                    IO (Decode16,
                        0x0064,             // Address Range Minimum
                        0x0064,             // Address Range Maximum
                        0x01,               // Address Alignment
                        0x01,               // Address Length
                        )
                    IRQNoFlags ()
                        {1}
                    })
                    Return (TMP)
                }
            }

	    /* PS/2 mouse */
            Device (MOU)
            {
                Name (_HID, EisaId ("PNP0F13"))
                Method (_STA, 0, NotSerialized)
                {
                    Return (0x0f)
                }

                Method (_CRS, 0, NotSerialized)
                {
                    Name (TMP, ResourceTemplate ()
                    {
                         IRQNoFlags () {12}
                    })
                    Return (TMP)
                }
            }

	    /* PS/2 floppy controller */
	    Device (FDC0)
	    {
	        Name (_HID, EisaId ("PNP0700"))
		Method (_STA, 0, NotSerialized)
		{
		    Return (0x0F)
		}
		Method (_CRS, 0, NotSerialized)
		{
		    Name (BUF0, ResourceTemplate ()
                    {
                        IO (Decode16, 0x03F2, 0x03F2, 0x00, 0x04)
                        IO (Decode16, 0x03F7, 0x03F7, 0x00, 0x01)
                        IRQNoFlags () {6}
                        DMA (Compatibility, NotBusMaster, Transfer8) {2}
                    })
		    Return (BUF0)
		}
	    }

	    /* Parallel port */
	    Device (LPT)
	    {
	        Name (_HID, EisaId ("PNP0400"))
		Method (_STA, 0, NotSerialized)
		{
		    Store (\_SB.PCI0.PX13.DRSA, Local0)
		    And (Local0, 0x80000000, Local0)
		    If (LEqual (Local0, 0))
		    {
			Return (0x00)
		    }
		    Else
		    {
			Return (0x0F)
		    }
		}
		Method (_CRS, 0, NotSerialized)
		{
		    Name (BUF0, ResourceTemplate ()
                    {
			IO (Decode16, 0x0378, 0x0378, 0x08, 0x08)
			IRQNoFlags () {7}
		    })
		    Return (BUF0)
		}
	    }

	    /* Serial Ports */
	    Device (COM1)
	    {
	        Name (_HID, EisaId ("PNP0501"))
		Name (_UID, 0x01)
		Method (_STA, 0, NotSerialized)
		{
		    Store (\_SB.PCI0.PX13.DRSC, Local0)
		    And (Local0, 0x08000000, Local0)
		    If (LEqual (Local0, 0))
		    {
			Return (0x00)
		    }
		    Else
		    {
			Return (0x0F)
		    }
		}
		Method (_CRS, 0, NotSerialized)
		{
		    Name (BUF0, ResourceTemplate ()
                    {
			IO (Decode16, 0x03F8, 0x03F8, 0x00, 0x08)
			IRQNoFlags () {4}
		    })
		    Return (BUF0)
		}
	    }

	    Device (COM2)
	    {
	        Name (_HID, EisaId ("PNP0501"))
		Name (_UID, 0x02)
		Method (_STA, 0, NotSerialized)
		{
		    Store (\_SB.PCI0.PX13.DRSC, Local0)
		    And (Local0, 0x80000000, Local0)
		    If (LEqual (Local0, 0))
		    {
			Return (0x00)
		    }
		    Else
		    {
			Return (0x0F)
		    }
		}
		Method (_CRS, 0, NotSerialized)
		{
		    Name (BUF0, ResourceTemplate ()
                    {
			IO (Decode16, 0x02F8, 0x02F8, 0x00, 0x08)
			IRQNoFlags () {3}
		    })
		    Return (BUF0)
		}
	    }
    }


/****************************************************************
 * PIIX4 PM
 ****************************************************************/

    Scope(\_SB.PCI0) {
        Device (PX13) {
	    Name (_ADR, 0x00010003)

	    OperationRegion (P13C, PCI_Config, 0x5c, 0x24)
	    Field (P13C, DWordAcc, NoLock, Preserve)
	    {
		DRSA, 32,
		DRSB, 32,
		DRSC, 32,
		DRSE, 32,
		DRSF, 32,
		DRSG, 32,
		DRSH, 32,
		DRSI, 32,
		DRSJ, 32
	    }
	}
    }


/****************************************************************
 * PCI hotplug
 ****************************************************************/

    Scope(\_SB.PCI0) {
        /* Methods called by bulk generated PCI devices below */

        /* Methods called by hotplug devices */
        Method (PCEJ, 1, NotSerialized) {
            // _EJ0 method - eject callback
            Store(ShiftLeft(1, Arg0), B0EJ)
            Return (0x0)
        }

	/* Hotplug notification method supplied by SSDT */
	External (\_SB.PCI0.PCNT, MethodObj)

        /* PCI hotplug notify method */
        Method(PCNF, 0) {
            // Local0 = iterator
            Store (Zero, Local0)
            While (LLess(Local0, 31)) {
                Increment(Local0)
                If (And(PCIU, ShiftLeft(1, Local0))) {
                    PCNT(Local0, 1)
                }
                If (And(PCID, ShiftLeft(1, Local0))) {
                    PCNT(Local0, 3)
                }
            }
            Return(One)
        }

    }


/****************************************************************
 * PCI IRQs
 ****************************************************************/

    Scope(\_SB) {
        Field (PCI0.ISA.P40C, ByteAcc, NoLock, Preserve)
        {
            PRQ0,   8,
            PRQ1,   8,
            PRQ2,   8,
            PRQ3,   8
        }

        Method (IQST, 1, NotSerialized) {
            // _STA method - get status
            If (And (0x80, Arg0)) {
                Return (0x09)
            }
            Return (0x0B)
        }
        Method (IQCR, 1, NotSerialized) {
            // _CRS method - get current settings
            Name (PRR0, ResourceTemplate ()
            {
                Interrupt (, Level, ActiveHigh, Shared)
                    { 0 }
            })
            CreateDWordField (PRR0, 0x05, PRRI)
            If (LLess (Arg0, 0x80)) {
                Store (Arg0, PRRI)
            }
            Return (PRR0)
        }
        // _DIS method - disable interrupt
#define DISIRQ(PRQVAR)                          \
            Or(PRQVAR, 0x80, PRQVAR)            \
        // _SRS method - set interrupt
#define SETIRQ(PRQVAR, IRQINFO)                         \
            CreateDWordField (IRQINFO, 0x05, PRRI)      \
            Store (PRRI, PRQVAR)

        Device(LNKA) {
            Name(_HID, EISAID("PNP0C0F"))     // PCI interrupt link
            Name(_UID, 1)
            Name(_PRS, ResourceTemplate(){
                Interrupt (, Level, ActiveHigh, Shared)
                    { 5, 10, 11 }
            })
            Method (_STA, 0, NotSerialized) { Return (IQST(PRQ0)) }
            Method (_DIS, 0, NotSerialized) { DISIRQ(PRQ0) }
            Method (_CRS, 0, NotSerialized) { Return (IQCR(PRQ0)) }
            Method (_SRS, 1, NotSerialized) { SETIRQ(PRQ0, Arg0) }
        }
        Device(LNKB) {
            Name(_HID, EISAID("PNP0C0F"))     // PCI interrupt link
            Name(_UID, 2)
            Name(_PRS, ResourceTemplate(){
                Interrupt (, Level, ActiveHigh, Shared)
                    { 5, 10, 11 }
            })
            Method (_STA, 0, NotSerialized) { Return (IQST(PRQ1)) }
            Method (_DIS, 0, NotSerialized) { DISIRQ(PRQ1) }
            Method (_CRS, 0, NotSerialized) { Return (IQCR(PRQ1)) }
            Method (_SRS, 1, NotSerialized) { SETIRQ(PRQ1, Arg0) }
        }
        Device(LNKC) {
            Name(_HID, EISAID("PNP0C0F"))     // PCI interrupt link
            Name(_UID, 3)
            Name(_PRS, ResourceTemplate() {
                Interrupt (, Level, ActiveHigh, Shared)
                    { 5, 10, 11 }
            })
            Method (_STA, 0, NotSerialized) { Return (IQST(PRQ2)) }
            Method (_DIS, 0, NotSerialized) { DISIRQ(PRQ2) }
            Method (_CRS, 0, NotSerialized) { Return (IQCR(PRQ2)) }
            Method (_SRS, 1, NotSerialized) { SETIRQ(PRQ2, Arg0) }
        }
        Device(LNKD) {
            Name(_HID, EISAID("PNP0C0F"))     // PCI interrupt link
            Name(_UID, 4)
            Name(_PRS, ResourceTemplate() {
                Interrupt (, Level, ActiveHigh, Shared)
                    { 5, 10, 11 }
            })
            Method (_STA, 0, NotSerialized) { Return (IQST(PRQ3)) }
            Method (_DIS, 0, NotSerialized) { DISIRQ(PRQ3) }
            Method (_CRS, 0, NotSerialized) { Return (IQCR(PRQ3)) }
            Method (_SRS, 1, NotSerialized) { SETIRQ(PRQ3, Arg0) }
        }
        Device(LNKS) {
            Name(_HID, EISAID("PNP0C0F"))     // PCI interrupt link
            Name(_UID, 5)
            Name(_PRS, ResourceTemplate() {
                Interrupt (, Level, ActiveHigh, Shared)
                    { 9 }
            })
            Method (_STA, 0, NotSerialized) { Return (IQST(PRQ0)) }
            Method (_DIS, 0, NotSerialized) { DISIRQ(PRQ0) }
            Method (_CRS, 0, NotSerialized) { Return (IQCR(PRQ0)) }
        }
    }

/****************************************************************
 * CPU hotplug
 ****************************************************************/

    Scope(\_SB) {
        /* Objects filled in by run-time generated SSDT */
        External(NTFY, MethodObj)
        External(CPON, PkgObj)

        /* Methods called by run-time generated SSDT Processor objects */
        Method (CPMA, 1, NotSerialized) {
            // _MAT method - create an madt apic buffer
            // Arg0 = Processor ID = Local APIC ID
            // Local0 = CPON flag for this cpu
            Store(DerefOf(Index(CPON, Arg0)), Local0)
            // Local1 = Buffer (in madt apic form) to return
            Store(Buffer(8) {0x00, 0x08, 0x00, 0x00, 0x00, 0, 0, 0}, Local1)
            // Update the processor id, lapic id, and enable/disable status
            Store(Arg0, Index(Local1, 2))
            Store(Arg0, Index(Local1, 3))
            Store(Local0, Index(Local1, 4))
            Return (Local1)
        }
        Method (CPST, 1, NotSerialized) {
            // _STA method - return ON status of cpu
            // Arg0 = Processor ID = Local APIC ID
            // Local0 = CPON flag for this cpu
            Store(DerefOf(Index(CPON, Arg0)), Local0)
            If (Local0) { Return(0xF) } Else { Return(0x0) }
        }
        Method (CPEJ, 2, NotSerialized) {
            // _EJ0 method - eject callback
            Sleep(200)
        }

        /* CPU hotplug notify method */
        OperationRegion(PRST, SystemIO, 0xaf00, 32)
        Field (PRST, ByteAcc, NoLock, Preserve)
        {
            PRS, 256
        }
        Method(PRSC, 0) {
            // Local5 = active cpu bitmap
            Store (PRS, Local5)
            // Local2 = last read byte from bitmap
            Store (Zero, Local2)
            // Local0 = Processor ID / APIC ID iterator
            Store (Zero, Local0)
            While (LLess(Local0, SizeOf(CPON))) {
                // Local1 = CPON flag for this cpu
                Store(DerefOf(Index(CPON, Local0)), Local1)
                If (And(Local0, 0x07)) {
                    // Shift down previously read bitmap byte
                    ShiftRight(Local2, 1, Local2)
                } Else {
                    // Read next byte from cpu bitmap
                    Store(DerefOf(Index(Local5, ShiftRight(Local0, 3))), Local2)
                }
                // Local3 = active state for this cpu
                Store(And(Local2, 1), Local3)

                If (LNotEqual(Local1, Local3)) {
                    // State change - update CPON with new state
                    Store(Local3, Index(CPON, Local0))
                    // Do CPU notify
                    If (LEqual(Local3, 1)) {
                        NTFY(Local0, 1)
                    } Else {
                        NTFY(Local0, 3)
                    }
                }
                Increment(Local0)
            }
            Return(One)
        }
    }


/****************************************************************
 * General purpose events
 ****************************************************************/

    Scope (\_GPE)
    {
        Name(_HID, "ACPI0006")

        Method(_L00) {
        }
        Method(_E01) {
            // PCI hotplug event
            \_SB.PCI0.PCNF()
        }
        Method(_E02) {
            // CPU hotplug event
            \_SB.PRSC()
        }
        Method(_L03) {
        }
        Method(_L04) {
        }
        Method(_L05) {
        }
        Method(_L06) {
        }
        Method(_L07) {
        }
        Method(_L08) {
        }
        Method(_L09) {
        }
        Method(_L0A) {
        }
        Method(_L0B) {
        }
        Method(_L0C) {
        }
        Method(_L0D) {
        }
        Method(_L0E) {
        }
        Method(_L0F) {
        }
    }


    Scope(\_SB.PCI0) {
        Device (S01) {
 Name (_ADR, 0x010000)
 Method (_EJ0, 1) { PCEJ(0x01) } Name (_SUN, 0x01) }
        Device (S02) {
 Name (_ADR, 0x020000)
 Method (_EJ0, 1) { PCEJ(0x02) } Name (_SUN, 0x02) }
        Device (S03) {
 Name (_ADR, 0x030000)
 Method (_EJ0, 1) { PCEJ(0x03) } Name (_SUN, 0x03) }
        Device (S04) {
 Name (_ADR, 0x040000)
 Method (_EJ0, 1) { PCEJ(0x04) } Name (_SUN, 0x04) }
        Device (S05) {
 Name (_ADR, 0x050000)
 Method (_EJ0, 1) { PCEJ(0x05) } Name (_SUN, 0x05) }
        Device (S06) {
 Name (_ADR, 0x060000)
 Method (_EJ0, 1) { PCEJ(0x06) } Name (_SUN, 0x06) }
        Device (S07) {
 Name (_ADR, 0x070000)
 Method (_EJ0, 1) { PCEJ(0x07) } Name (_SUN, 0x07) }
        Device (S08) {
 Name (_ADR, 0x080000)
 Method (_EJ0, 1) { PCEJ(0x08) } Name (_SUN, 0x08) }
        Device (S09) {
 Name (_ADR, 0x090000)
 Method (_EJ0, 1) { PCEJ(0x09) } Name (_SUN, 0x09) }
        Device (S0a) {
 Name (_ADR, 0x0a0000)
 Method (_EJ0, 1) { PCEJ(0x0a) } Name (_SUN, 0x0a) }
        Device (S0b) {
 Name (_ADR, 0x0b0000)
 Method (_EJ0, 1) { PCEJ(0x0b) } Name (_SUN, 0x0b) }
        Device (S0c) {
 Name (_ADR, 0x0c0000)
 Method (_EJ0, 1) { PCEJ(0x0c) } Name (_SUN, 0x0c) }
        Device (S0d) {
 Name (_ADR, 0x0d0000)
 Method (_EJ0, 1) { PCEJ(0x0d) } Name (_SUN, 0x0d) }
        Device (S0e) {
 Name (_ADR, 0x0e0000)
 Method (_EJ0, 1) { PCEJ(0x0e) } Name (_SUN, 0x0e) }
        Device (S0f) {
 Name (_ADR, 0x0f0000)
 Method (_EJ0, 1) { PCEJ(0x0f) } Name (_SUN, 0x0f) }
        Device (S10) {
 Name (_ADR, 0x100000)
 Method (_EJ0, 1) { PCEJ(0x10) } Name (_SUN, 0x10) }
        Device (S11) {
 Name (_ADR, 0x110000)
 Method (_EJ0, 1) { PCEJ(0x11) } Name (_SUN, 0x11) }
        Device (S12) {
 Name (_ADR, 0x120000)
 Method (_EJ0, 1) { PCEJ(0x12) } Name (_SUN, 0x12) }
        Device (S13) {
 Name (_ADR, 0x130000)
 Method (_EJ0, 1) { PCEJ(0x13) } Name (_SUN, 0x13) }
        Device (S14) {
 Name (_ADR, 0x140000)
 Method (_EJ0, 1) { PCEJ(0x14) } Name (_SUN, 0x14) }
        Device (S15) {
 Name (_ADR, 0x150000)
 Method (_EJ0, 1) { PCEJ(0x15) } Name (_SUN, 0x15) }
        Device (S16) {
 Name (_ADR, 0x160000)
 Method (_EJ0, 1) { PCEJ(0x16) } Name (_SUN, 0x16) }
        Device (S17) {
 Name (_ADR, 0x170000)
 Method (_EJ0, 1) { PCEJ(0x17) } Name (_SUN, 0x17) }
        Device (S18) {
 Name (_ADR, 0x180000)
 Method (_EJ0, 1) { PCEJ(0x18) } Name (_SUN, 0x18) }
        Device (S19) {
 Name (_ADR, 0x190000)
 Method (_EJ0, 1) { PCEJ(0x19) } Name (_SUN, 0x19) }
        Device (S1a) {
 Name (_ADR, 0x1a0000)
 Method (_EJ0, 1) { PCEJ(0x1a) } Name (_SUN, 0x1a) }
        Device (S1b) {
 Name (_ADR, 0x1b0000)
 Method (_EJ0, 1) { PCEJ(0x1b) } Name (_SUN, 0x1b) }
        Device (S1c) {
 Name (_ADR, 0x1c0000)
 Method (_EJ0, 1) { PCEJ(0x1c) } Name (_SUN, 0x1c) }
        Device (S1d) {
 Name (_ADR, 0x1d0000)
 Method (_EJ0, 1) { PCEJ(0x1d) } Name (_SUN, 0x1d) }
        Device (S1e) {
 Name (_ADR, 0x1e0000)
 Method (_EJ0, 1) { PCEJ(0x1e) } Name (_SUN, 0x1e) }
        Device (S1f) {
 Name (_ADR, 0x1f0000)
 Method (_EJ0, 1) { PCEJ(0x1f) } Name (_SUN, 0x1f) }
        Method(PCNT, 2) {
            If (LEqual(Arg0, 0x01)) { Notify(S01, Arg1) }
            If (LEqual(Arg0, 0x02)) { Notify(S02, Arg1) }
            If (LEqual(Arg0, 0x03)) { Notify(S03, Arg1) }
            If (LEqual(Arg0, 0x04)) { Notify(S04, Arg1) }
            If (LEqual(Arg0, 0x05)) { Notify(S05, Arg1) }
            If (LEqual(Arg0, 0x06)) { Notify(S06, Arg1) }
            If (LEqual(Arg0, 0x07)) { Notify(S07, Arg1) }
            If (LEqual(Arg0, 0x08)) { Notify(S08, Arg1) }
            If (LEqual(Arg0, 0x09)) { Notify(S09, Arg1) }
            If (LEqual(Arg0, 0x0a)) { Notify(S0a, Arg1) }
            If (LEqual(Arg0, 0x0b)) { Notify(S0b, Arg1) }
            If (LEqual(Arg0, 0x0c)) { Notify(S0c, Arg1) }
            If (LEqual(Arg0, 0x0d)) { Notify(S0d, Arg1) }
            If (LEqual(Arg0, 0x0e)) { Notify(S0e, Arg1) }
            If (LEqual(Arg0, 0x0f)) { Notify(S0f, Arg1) }
            If (LEqual(Arg0, 0x10)) { Notify(S10, Arg1) }
            If (LEqual(Arg0, 0x11)) { Notify(S11, Arg1) }
            If (LEqual(Arg0, 0x12)) { Notify(S12, Arg1) }
            If (LEqual(Arg0, 0x13)) { Notify(S13, Arg1) }
            If (LEqual(Arg0, 0x14)) { Notify(S14, Arg1) }
            If (LEqual(Arg0, 0x15)) { Notify(S15, Arg1) }
            If (LEqual(Arg0, 0x16)) { Notify(S16, Arg1) }
            If (LEqual(Arg0, 0x17)) { Notify(S17, Arg1) }
            If (LEqual(Arg0, 0x18)) { Notify(S18, Arg1) }
            If (LEqual(Arg0, 0x19)) { Notify(S19, Arg1) }
            If (LEqual(Arg0, 0x1a)) { Notify(S1a, Arg1) }
            If (LEqual(Arg0, 0x1b)) { Notify(S1b, Arg1) }
            If (LEqual(Arg0, 0x1c)) { Notify(S1c, Arg1) }
            If (LEqual(Arg0, 0x1d)) { Notify(S1d, Arg1) }
            If (LEqual(Arg0, 0x1e)) { Notify(S1e, Arg1) }
            If (LEqual(Arg0, 0x1f)) { Notify(S1f, Arg1) }
        }
    }

    Scope(\) {
        Name (_S3, Package (0x04)
        {
            One,
            One,
            Zero,
            Zero
        })
        Name (_S4, Package (0x04)
        {
            0x2,
            0x2,
            Zero,
            Zero
        })
        Name (_S5, Package (0x04)
        {
            Zero,
            Zero,
            Zero,
            Zero
        })
    }
}
