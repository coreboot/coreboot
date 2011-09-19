/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Christoph Grenz <christophg+cb@grenz-bonn.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * include this file into a mainboard's DSDT and it will expose the W83627HF
 * SuperIO and its functionality in the _SB device tree.
 *
 * Devices are marked as nonexistant if they got 0x00000000 as I/O base address
 * (compatibility with legacy bios, which disables logical devices that way)
 *
 * It allows the change of IO ports, IRQs and DMA settings on most logical
 * devices, disabling and reenabling logical devices and controlling power
 * saving mode on logical devices or the whole chip.
 *
 * Notes:
 * - ECP mode of parallel port not implemented, DMA settings of parallel port
 *   will be reset on DSDT init
 * - Hardware monitor is only shown as "Motherboard Resource", shows no real
 *   functionality
 *
 * Controllable through preprocessor macros:
 * NO_W83627HF_FDC:      don't expose the floppy disk controller
 * NO_W83627HF_FDC_ENUM: don't try to enumerate the connected floppy drives
 * NO_W83627HF_PPORT:    don't expose the parallel port
 * NO_W83627HF_UARTA:    don't expose the first serial port
 * NO_W83627HF_UARTB:    don't expose the second serial port (already hidden
 *                       if UARTB is configured as IRDA port by firmware)
 * NO_W83627HF_IRDA:     don't expose the IRDA port (already hidden if UARTB is
 *                       configured as serial port by firmware)
 * NO_W83627HF_CIR:      don't expose the Consumer Infrared functionality
 * NO_W83627HF_KBC:      don't expose the keyboard controller
 * NO_W83627HF_PS2M:     don't expose the PS/2 mouse functionality of the
 *                       keyboard controller
 * NO_W83627HF_GAME:     don't expose the game port
 * NO_W83627HF_MIDI:     don't expose the MIDI port
 * NO_W83627HF_HWMON:    don't expose the hardware monitor as
 *                       PnP "Motherboard Ressource"
 *
 * Datasheet: "W83627HF/F WINBOND I/O" rev. 6.0
 * http://www.itox.com/pages/support/wdt/W83627HF.pdf
 */

Scope (\_SB)
{

	Device(SIO) {
		Name (_HID, EisaId("PNP0A05"))
		Name (_STR, Unicode("Winbond W83627HF SuperIO"))
		Name (_UID, "w83627hf")

		/* Mutex for accesses to the configuration ports (prolog and epilog commands are used, so synchronization is useful) */
		Mutex(CRMX, 1)

		/* SuperIO configuration ports */
		OperationRegion (CREG, SystemIO, 0x2E, 0x02)
		Field (CREG, ByteAcc, NoLock, Preserve)
		{
			ADDR,   8,
			DATA,   8
		}
		IndexField (CREG, DATA, ByteAcc, NoLock, Preserve)
		{
			Offset (0x02),
			RST,    1,     /* Soft reset */
			,       7,
			Offset (0x07),
			LDN,    8,     /* Logical device selector */
			Offset (0x20),
			DID,    8,     /* Device ID */
			DREV,   8,     /* Device Revision */
			POWC,   8,
			FDPW,   1,     /* FDC Power Down */
			,       2,
			PRPW,   1,     /* PRT Power Down */
			UAPW,   1,     /* UART A Power Down */
			UBPW,   1,     /* UART B Power Down */
			HWPW,   1,     /* HWM Power Down */
			,       1,
			IPD,    1,     /* Immediate Chip Power Down */
			,       7,
			PNPS,   1,     /* PnP Address Select Register Default Value Mode */
			,       1,
			KBCR,   1,     /* KBC enabled after system reset (read-only) */
			,       3,
			CLKS,   1,     /* Clock select */
			AQ16,   1,     /* 16bit Address Qualification */
			FDCT,   1,     /* Tristate FDC (?) */
			,       2,
			PRTT,   1,     /* Tristate parallel port (?) */
			URAT,   1,     /* Tristate UART A (?) */
			URBT,   1,     /* Tristate UART B (?) */
			,       2,
			URAI,   1,     /* UART A Legacy IRQ Select Disable */
			URBI,   1,     /* UART B Legacy IRQ Select Disable */
			PRTI,   1,     /* Parallel Port Legacy IRQ/DRQ Select Disable */
			FDCI,   1,     /* FDC Legacy IRQ/DRQ Select Disable */
			,       1,
			LCKC,   1,     /* Lock Configuration Registers */
			Offset (0x29),
			IO3S,   8,     /* GPIO3 pin selection register */
			ACTR,   1,     /* Logical device activation */
			ACT1,   1,     /* Logical part activation 1 (mostly unused) */
			ACT2,   1,     /* Logical part activation 2 (mostly unused) */
			,       5,
			Offset (0x60),
			IO1H,   8,      /* First I/O port base - high byte */
			IO1L,   8,      /* First I/O port base - low byte */
			IO2H,   8,      /* Second I/O port base - high byte */
			IO2L,   8,      /* Second I/O port base - low byte */
			Offset (0x70),
			IRQ0,   8,      /* First IRQ */
			Offset (0x72),
			IRQ1,   8,      /* First IRQ */
			Offset (0x74),
			DMA0,   8,      /* DMA */
			Offset (0xE0),
			/* CRE0-CRE4: function logical device dependant, seems to be reserved for ACPI settings */
			CRE0,   8,
			CRE1,   8,
			CRE2,   8,
			CRE3,   8,
			CRE4,   8,
			Offset (0xF0),
			/* OPT1-OPTA aka CRF0-CRF9: function logical device dependant */
			OPT1,   8,
			OPT2,   8,
			OPT3,   8,
			OPT4,   8,
			OPT5,   8,
			OPT6,   8,
			OPT7,   8,
			OPT8,   8,
			OPT9,   8,
			OPTA,   8
		}

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x002E, 0x002E, 0x02, 0x01) /* Announce the used I/O ports to the OS */
			IO (Decode16, 0x004E, 0x004E, 0x01, 0x01) /* this port is used in some configurations, so announce it to be sure */
		})

		/* Enter configuration mode (and aquire mutex)
		   Method must be run before accesssing the configuration region.
		*/
		Method (ENCM)
		{
			Acquire (CRMX, 0xFFFF)
			Store (0x87, ADDR)
			Store (0x87, ADDR)
		}

		/* Exit configuration mode (and release mutex)
		   Method must be run after accessing the configuration region.
		*/
		Method (EXCM)
		{
			Release (CRMX)
			Store (0xAA, ADDR)
		}

		/* PM: indicate IPD (Immediate Power Down) bit state as D0/D2 */
		Method (_PSC) {
			ENCM ()
			Store (IPD, Local0)
			EXCM ()
			If (Local0) { Return (2) }
			Else { Return (0) }
		}

		/* PM: Switch to D0 by setting IPD low  */
		Method (_PS0) {
			ENCM ()
			Store (Zero, IPD)
			EXCM ()
		}

		/* PM: Switch to D2 by setting IPD high  */
		Method (_PS2) {
			ENCM ()
			Store (One, IPD)
			EXCM ()
		}

		#ifndef NO_W83627HF_FDC
		/* =================== Floppy Disk Controller ================ */
		Device (FDC0) {
			Name (_HID, EisaId ("PNP0700"))
			Name (_STR, Unicode ("W83627HF Floppy Disk Controller"))
			Name (_UID, "w83627hf-fdc")

			#ifndef NO_W83627HF_FDC_ENUM
			/* Initialization method: Should be run once on boot
			   If FDC is active, enumerate all connected devices */
			Method (_INI) {
				ENCM ()
				Store (0x00, LDN)
				Store (ACTR, Local0)
				Store (IO1H, Local1)
				Store (IO1L, Local2)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or(Local1, Local2, Local1)
				If (Local0) {
					/* Try probing drives and save result in _FDE */
					PROB(Local1)
				}
			}
			#endif

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x00, LDN)
				If (ACTR) {
					Store (0x0F, Local0)
				}
				ElseIf (LOr (IO1H, IO1L))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_PSC) {
				Store(^^_PSC (), Local0)
				If (Local0) { Return (Local0) }
				ENCM ()
				Store (FDPW, Local0)
				EXCM ()
				If (Local0) { Return (1) }
				Else { Return (0) }
			}
			Method (_PS0) {
				ENCM ()
				Store (Zero, FDPW)
				EXCM ()
			}
			Method (_PS1) {
				ENCM ()
				Store (One, FDPW)
				EXCM ()
			}
			Name (_PR0, Package () { ^^ })

			Method (_DIS)
			{
				ENCM ()
				Store (0x00, LDN)
				Store (Zero, ACTR)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x06, IO0)
					IO (Decode16, 0x03F7, 0x03F7, 0x01, 0x01)
					IRQNoFlags () {6}
					DMA (Compatibility, NotBusMaster, Transfer8) {2}
				})
				ENCM ()
				Store (0x00, LDN)
				Store(IO1H, Local0)
				Store(IO1L, Local1)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFnNoPri ()
				{
					IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x06)
					IO (Decode16, 0x03F7, 0x03F7, 0x01, 0x01)
					IRQNoFlags () {6}
					DMA (Compatibility, NotBusMaster, Transfer8) {2}
				}
				EndDependentFn ()
			})

			#ifndef NO_W83627HF_FDC_ENUM
			Name(_FDE, Buffer(){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
			#endif

			Method (PROB, 1) {
				#ifndef NO_W83627HF_FDC_ENUM
				/* Try probing drives and save result in _FDE
				   Probing is done through selecting and activating a drive
				   and reading 0x03F7 aka the "shared IDE and floppy register"
				   as any value there besides zero seems to indicate a
				   connected drive.
				*/
				OperationRegion (FIO1, SystemIO, Arg0, 0x06)
				Field (FIO1, ByteAcc, NoLock, Preserve)
				{
					Offset(0x02),
					SELE,  2,
					RSTL,  1,
					IDMA,  1,
					ACT1,  1,
					ACT2,  1,
					ACT3,  1,
					ACT4,  1,
					Offset(0x04),
					BSY1,  1,
					BSY2,  1,
					BSY3,  1,
					BSY4,  1,
					BUSY,  1,
					NDMA,  1,
					IODI,  1,
					RDY,   1,
					DATA,  8,
				}
				OperationRegion (FIO2, SystemIO, 0x3F7, 0x01)

				CreateByteField (_FDE, 3, FD1)
				CreateByteField (_FDE, 7, FD2)
				CreateByteField (_FDE, 11, FD3)
				CreateByteField (_FDE, 15, FD4)

				Store(One, ACT1)
				Store(0, SELE)
				Sleep(0x64)
				If (FIO2) { Store (One, FD1) }

				Store(Zero, ACT1)
				Store(One, ACT2)
				Store(1, SELE)
				Sleep(0x64)
				If (FIO2) { Store (One, FD2) }

				Store(Zero, ACT2)
				Store(One, ACT3)
				Store(2, SELE)
				Sleep(0x64)
				If (FIO2) { Store (One, FD3) }

				Store(Zero, ACT3)
				Store(One, ACT4)
				Store(3, SELE)
				Sleep(0x64)
				If (FIO2) { Store (One, FD4) }
				Store(Zero, ACT4)
				Store(Zero, SELE)
				#endif
			}


			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 1, 6, IO0)
					IO (Decode16, 0, 0, 1, 1, IO1)
					IRQNoFlags (IRQ0) {}
					DMA (Compatibility, NotBusMaster, Transfer8, DMA0) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateWordField (Arg0, IO1._MIN, IOA1)
				CreateByteField (Arg0, IRQ0._INT, IRQL)
				CreateByteField (Arg0, DMA0._DMA, DMCH)

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				ENCM ()
				Store (0, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				Store (One, ACTR)
				EXCM ()

				/* Try probing drives and save result in _FDE */
				PROB(IOA0)
			}
		}
		#endif

		#ifndef NO_W83627HF_PPORT
		/* ======================== Parallel Port ======================== */
		/* Currently no ECP support */
		Device (PAR0) {
			Name (_HID, EisaId ("PNP0400"))
			Name (_DDN, "LPT1")
			Name (_UID, "w83627hf-pport")

			Method (MODE, 1) {
				And(Arg0, 0x07, Local0)
				ENCM ()
				Store (0x01, LDN)
				And(OPT1, 0x3, Local1)
				Or(Local1, Local0, OPT1)
				EXCM()
			}

			Method (_INI)
			{
				/* Deactivate DMA, even if set by BIOS. We don't announce it
				through _CRS and it's only useful in ECP mode which we
				don't support at the moment. */
				ENCM ()
				Store (0x01, LDN)
				Store (0x04, DMA0)
				EXCM ()
			}

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x01, LDN)
				And(OPT1, 0x3, Local1)
				If (ACTR) {
					If (LNotEqual(Local1, 2)) {
						Store (0x0D, Local0)
					} Else {
						Store (0x0D, Local0)
					}
				}
				ElseIf (LOr (IO1H, IO1L))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_PSC) {
				Store(^^_PSC (), Local0)
				If (Local0) { Return (Local0) }
				ENCM ()
				Store (PRPW, Local0)
				EXCM ()
				If (Local0) { Return (1) }
				Else { Return (0) }
			}
			Method (_PS0) {
				ENCM ()
				Store (Zero, PRPW)
				EXCM ()
			}
			Method (_PS1) {
				ENCM ()
				Store (One, PRPW)
				EXCM ()
			}
			Name (_PR0, Package () { ^^ })

			Method (_DIS) {
				ENCM ()
				Store (0x01, LDN)
				Store (Zero, ACTR)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate ()
				{
					IO (Decode16, 0x0000, 0x0000, 0x04, 0x08, IO0)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (CRS, IO0._MIN, IOP0)
				CreateWordField (CRS, IO0._MAX, IOR0)
				CreateByteField (CRS, IO0._ALN, IOAL)
				CreateByteField (CRS, IO0._LEN, IOLE)
				CreateWordField (CRS, IRQX._INT, IRQW)

				ENCM ()
				Store (0x01, LDN)
				Store (IO1L, Local0)
				Store (IO1H, Local1)
				Store (OPT1, Local2)
				Store (IRQ0, Local5)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or (Local1, Local0, Local1)

				And(Local2, 0x3, Local3)
				And(Local2, 0x4, Local4)
				If (Local4) {
					Store(0x04, IOAL)
				}

				Store(Local1, IOP0)
				Store(Local1, IOR0)

				If (LEqual (Local0, 0xBC))
				{
					Store (0x04, IOLE)
				}
				Else
				{
					Store (0x08, IOLE)
				}

				Store (One, Local0)
				ShiftLeft (Local0, Local5, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,1)
				{
					IO (Decode16, 0x0378, 0x0378, 0x04, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,1)
				{
					IO (Decode16, 0x0278, 0x0278, 0x04, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,1)
				{
					IO (Decode16, 0x03BC, 0x03BC, 0x04, 0x04)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,0)
				{
					IO (Decode16, 0x0378, 0x0378, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,0)
				{
					IO (Decode16, 0x0278, 0x0278, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (2,0)
				{
					IO (Decode16, 0x0100, 0x0FFC, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (2,1)
				{
					IO (Decode16, 0x0100, 0x0FFC, 0x04, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				EndDependentFn ()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 4, 4, IO0)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateWordField (Arg0, IO0._MIN, IOA1)
				CreateByteField (Arg0, IO0._ALN, IOAL)
				CreateByteField (Arg0, IO0._LEN, IOLE)
				CreateWordField (Arg0, IRQX._INT, IRQL)

				If (LEqual(IOAL, 4)) {
					Store(0x0, Local2)
				} else  {
					Store(0x1, Local2)
				}

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				ENCM ()
				Store (0x01, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				/*Mode*/
				Store (OPT1, Local3)
				And (Local3, 0xF8, Local3)
				Or (Local2, Local3, OPT1)
				/*DMA off*/
				Store (0x04, DMA0)
				/*IRQ*/
				FindSetLeftBit (IRQL, Local3)
				Subtract (Local3, 0x01, Local3)
				Store (Local3, IRQ0)
				/*Activate*/
				Store (One, ACTR)
				EXCM ()
			}
		}
		#endif

		#ifndef NO_W83627HF_UARTA
		/* =========================== UART A ============================ */
		Device (SER0) {
			Name (_HID, EisaId ("PNP0501"))
			Name (_UID, "w83627hf-uarta")
			Name (_DDN, "COM1")

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x02, LDN)
				If (ACTR) {
					Store (0x0F, Local0)
				}
				ElseIf (LOr (IO1H, IO1L))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_PSC) {
				Store(^^_PSC (), Local0)
				If (Local0) { Return (Local0) }
				ENCM ()
				Store (UAPW, Local0)
				EXCM ()
				If (Local0) { Return (1) }
				Else { Return (0) }
			}
			Method (_PS0) {
				ENCM ()
				Store (Zero, UAPW)
				EXCM ()
			}
			Method (_PS1) {
				ENCM ()
				Store (One, UAPW)
				EXCM ()
			}
			Name (_PR0, Package () { ^^ })

			Method (_DIS)
			{
				ENCM ()
				Store (0x02, LDN)
				Store (Zero, ACTR)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
					IRQNoFlags (IRQX) {6}
				})
				ENCM ()
				Store (0x02, LDN)
				Store(IO1H, Local1)
				Store(IO1L, Local0)
				Store(IRQ0, Local2)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				CreateWordField (CRS, IRQX._INT, IRQW)
				Store (One, Local3)
				ShiftLeft (Local3, Local2, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,0) {
					IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,0) {
					IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (1,0) {
					IO (Decode16, 0x03E8, 0x03E8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (1,0) {
					IO (Decode16, 0x02E8, 0x02E8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (2,0) {
					IO (Decode16, 0x0100, 0x0FF8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				EndDependentFn()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 1, 6, IO0)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateByteField (Arg0, IRQX._INT, IRQL)

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				FindSetLeftBit (IRQL, Local3)
				Subtract (Local3, 0x01, Local3)

				ENCM ()
				Store (2, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				Store (Local3, IRQ0)
				Store (One, ACTR)
				EXCM ()
			}
		}
		#endif

		#ifndef NO_W83627HF_UARTB
		/* =========================== UART B ============================ */
		Device (SER1) {
			Name (_HID, EisaId ("PNP0501"))
			Name (_UID, "w83627hf-uartb")
			Name (_DDN, "COM2")

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x03, LDN)
				If (LNot(And(OPT2, 0x30)))
				{
					If (ACTR) {
						Store (0x0F, Local0)
					}
					ElseIf (LOr (IO1H, IO1L))
					{
						Store (0x0D, Local0)
					}
				}
				EXCM ()
				Return (Local0)
			}

			Method (_PSC) {
				Store(^^_PSC (), Local0)
				If (Local0) { Return (Local0) }
				ENCM ()
				Store (UBPW, Local0)
				EXCM ()
				If (Local0) { Return (1) }
				Else { Return (0) }
			}
			Method (_PS0) {
				ENCM ()
				Store (Zero, UBPW)
				EXCM ()
			}
			Method (_PS1) {
				ENCM ()
				Store (One, UBPW)
				EXCM ()
			}
			Name (_PR0, Package () { ^^ })

			Method (_DIS)
			{
				ENCM ()
				Store (0x03, LDN)
				Store (Zero, ACTR)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
					IRQNoFlags (IRQX) {6}
				})
				ENCM ()
				Store (0x03, LDN)
				Store(IO1H, Local1)
				Store(IO1L, Local0)
				Store(IRQ0, Local2)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				CreateWordField (CRS, IRQX._INT, IRQW)
				Store (One, Local3)
				ShiftLeft (Local3, Local2, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,0) {
					IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,0) {
					IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (1,0) {
					IO (Decode16, 0x03E8, 0x03E8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (1,0) {
					IO (Decode16, 0x02E8, 0x02E8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (2,0) {
					IO (Decode16, 0x0100, 0x0FF8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				EndDependentFn()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 1, 8, IO0)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateByteField (Arg0, IRQX._INT, IRQL)

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				FindSetLeftBit (IRQL, Local3)
				Subtract (Local3, 0x01, Local3)

				ENCM ()
				Store (3, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				Store (Local3, IRQ0)
				Store (One, ACTR)
				EXCM ()
			}
		}
		#endif

		#ifndef NO_W83627HF_IRDA
		/* ======================== UART B (IRDA) ======================== */
		Device (IRDA) {
			Name (_HID, EisaId ("PNP0510"))
			Name (_UID, "w83627hf-irda")
			Name (_STR, Unicode("IrDA Port"))

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x03, LDN)
				If (And(OPT2, 0x30))
				{
					If (ACTR) {
						Store (0x0F, Local0)
					}
					ElseIf (LOr (IO1H, IO1L))
					{
						Store (0x0D, Local0)
					}
				}
				EXCM ()
				Return (Local0)
			}

			Method (_PSC) {
				Store(^^_PSC (), Local0)
				If (Local0) { Return (Local0) }
				ENCM ()
				Store (UBPW, Local0)
				EXCM ()
				If (Local0) { Return (1) }
				Else { Return (0) }
			}
			Method (_PS0) {
				ENCM ()
				Store (Zero, UBPW)
				EXCM ()
			}
			Method (_PS1) {
				ENCM ()
				Store (One, UBPW)
				EXCM ()
			}
			Name (_PR0, Package () { ^^ })

			Method (_DIS)
			{
				ENCM ()
				Store (0x03, LDN)
				Store (Zero, ACTR)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
					IRQNoFlags (IRQX) {6}
				})
				ENCM ()
				Store (0x03, LDN)
				Store(IO1H, Local1)
				Store(IO1L, Local0)
				Store(IRQ0, Local2)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				CreateWordField (CRS, IRQX._INT, IRQW)
				Store (One, Local3)
				ShiftLeft (Local3, Local2, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,0) {
					IO (Decode16, 0x02F8, 0x02F8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (1,0) {
					IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,0) {
					IO (Decode16, 0x03E8, 0x03E8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (0,0) {
					IO (Decode16, 0x02E8, 0x02E8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				StartDependentFn (2,0) {
					IO (Decode16, 0x0100, 0x0FF8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				EndDependentFn()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 1, 8, IO0)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateByteField (Arg0, IRQX._INT, IRQL)

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				FindSetLeftBit (IRQL, Local3)
				Subtract (Local3, 0x01, Local3)

				ENCM ()
				Store (3, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				Store (Local3, IRQ0)
				Store (One, ACTR)
				EXCM ()
			}
		}
		#endif

		#ifndef NO_W83627HF_CIR
		/* ========================= Consumer IR ========================= */
		Device (CIR0) {
			Name (_HID, EisaId ("WEC1022")) /* Should be the correct one */
			Name (_UID, "w83627hf-cir")
			Name (_STR, Unicode("Winbond Consumer Infrared Transceiver"))

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x06, LDN)
				If (ACTR) {
					Store (0x0F, Local0)
				}
				ElseIf (LOr (IO1H, IO1L))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_DIS)
			{
				ENCM ()
				Store (0x06, LDN)
				Store (Zero, ACTR)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
					IRQNoFlags (IRQX) {6}
				})
				ENCM ()
				Store (0x03, LDN)
				Store(IO1H, Local1)
				Store(IO1L, Local0)
				Store(IRQ0, Local2)
				EXCM ()
				ShiftLeft(Local1, 8, Local1)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				CreateWordField (CRS, IRQX._INT, IRQW)
				Store (One, Local3)
				ShiftLeft (Local3, Local2, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,0) {
					IO (Decode16, 0x0100, 0x0FF8, 0x08, 0x08)
					IRQNoFlags () {3,4,5,7,9,10,11,12}
				}
				EndDependentFn()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 1, 8, IO0)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateByteField (Arg0, IRQX._INT, IRQL)

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				FindSetLeftBit (IRQL, Local3)
				Subtract (Local3, 0x01, Local3)

				ENCM ()
				Store (6, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				Store (Local3, IRQ0)
				Store (One, ACTR)
				EXCM ()
			}
		}
		#endif

		#ifndef NO_W83627HF_KBC
		/* ===================== Keyboard Controller ===================== */
		Device (KBD0) {
			Name (_HID, EisaId ("PNP0303"))
			Name (_CID, 0x0B03D041)
			Name (_UID, "w83627hf-kbc")

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x05, LDN)
				If (ACTR) {
					Store (0x0F, Local0)
				}
				ElseIf (Lor(LOr (IO1H, IO1L), LOr (IO2H, IO2L)))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_DIS)
			{
				ENCM ()
				Store (0x05, LDN)
				Store (Zero, ACTR)
				EXCM ()
				Notify(PS2M, 1)
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO1)
					IRQNoFlags (IRQX) {}
				})
				ENCM ()
				Store (0x05, LDN)
				Store(IO1H, Local0)
				Store(IO1L, Local1)
				Store(IO2H, Local2)
				Store(IO2L, Local3)
				Store(IRQ0, Local4)
				EXCM ()
				ShiftLeft(Local0, 8, Local0)
				Or (Local1, Local0, Local0)

				ShiftLeft(Local2, 8, Local2)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				CreateWordField (CRS, IO1._MIN, I1MI)
				Store (Local2, I1MI)
				CreateWordField (CRS, IO1._MAX, I1MA)
				Store (Local2, I1MA)

				CreateWordField (CRS, IRQX._INT, IRQW)
				Store (One, Local5)
				ShiftLeft (Local5, Local4, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,0) {
					IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
					IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
					IRQNoFlags () {1}
				}
				EndDependentFn()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IO (Decode16, 0, 0, 1, 1, IO0)
					IO (Decode16, 0, 0, 1, 1, IO1)
					IRQNoFlags (IRQX) {}
				})
				CreateWordField (Arg0, IO0._MIN, IOA0)
				CreateWordField (Arg0, IO1._MIN, IOA1)
				CreateByteField (Arg0, IRQX._INT, IRQL)

				Divide(IOA0, 256, Local0, Local1)
				ShiftRight(Local1, 8, Local1)

				Divide(IOA1, 256, Local2, Local3)
				ShiftRight(Local1, 8, Local1)

				FindSetLeftBit (IRQL, Local4)
				Subtract (Local4, 0x01, Local4)

				ENCM ()
				Store (5, LDN)
				Store (Local1, IO1H)
				Store (Local0, IO1L)
				Store (Local3, IO2H)
				Store (Local2, IO2L)
				Store (Local3, IRQ0)
				Store (One, ACTR)
				EXCM ()
			}
		}
		#ifndef NO_W83627HF_PS2M
		Device (PS2M) {
			Name (_HID, EisaId ("PNP0F13"))
			Name (_UID, "w83627hf-ps2m")

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x05, LDN)
				If (LAnd(ACTR, IRQ1) ) {
					Store (0x0F, Local0)
				}
				ElseIf (Lor(LOr (IO1H, IO1L), LOr (IO2H, IO2L)))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_DIS)
			{
				ENCM ()
				Store (0x05, LDN)
				Store (Zero, IRQ1)
				EXCM ()
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IRQNoFlags (IRQX) {}
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO1)
				})
				ENCM ()
				Store (0x05, LDN)
				Store(IO1H, Local0)
				Store(IO1L, Local1)
				Store(IO2H, Local2)
				Store(IO2L, Local3)
				Store(IRQ1, Local4)
				EXCM ()
				ShiftLeft(Local0, 8, Local0)
				Or (Local1, Local0, Local0)

				ShiftLeft(Local2, 8, Local2)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				CreateWordField (CRS, IO1._MIN, I1MI)
				Store (Local2, I1MI)
				CreateWordField (CRS, IO1._MAX, I1MA)
				Store (Local2, I1MA)

				CreateWordField (CRS, IRQX._INT, IRQW)
				Store (One, Local5)
				ShiftLeft (Local5, Local4, IRQW)

				Return (CRS)
			}

			Name (_PRS, ResourceTemplate ()
			{
				StartDependentFn (0,0) {
					IRQNoFlags () {12}
				}
				StartDependentFn (2,0) {
					IRQNoFlags () {1,3,4,5,6,7,9,10,11,12}
				}
				EndDependentFn()
			})

			Method (_SRS, 1, Serialized)
			{
				Name (TMPL, ResourceTemplate () {
					IRQNoFlags (IRQX) {}
				})
				CreateByteField (Arg0, IRQX._INT, IRQL)

				FindSetLeftBit (IRQL, Local0)
				Subtract (Local0, 0x01, Local0)

				ENCM ()
				Store (5, LDN)
				Store (Local0, IRQ1)
				/* Only activates if KBD is active */
				Store (One, ACTR)
				EXCM ()
			}
		}
		#endif
		#endif

		#ifndef NO_W83627HF_GAME
		/* ========================== Game Port ========================== */
		Device (GAME) {
			Name (_HID, EisaId ("PNPB02F"))
			Name (_STR, Unicode ("Joystick/Game Port"))
			Name (_UID, "w83627hf-game")

			Method (_STA) {
				ENCM ()
				Store(0, Local0)
				Store (0x07, LDN)
				If (LOr(IO1L, IO1H)) {
					If (LOr(ACTR, ACT1)) {
						Store (0x0F, Local0)
					}
					Else {
						Store (0x0D, Local0)
					}
				}
				EXCM ()
				Return (Local0)
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
					IRQNoFlags (IRQX) {}
				})
				ENCM ()
				Store (0x07, LDN)
				Store(IO1H, Local0)
				Store(IO1L, Local1)
				Store(IRQ0, Local2)
				EXCM ()
				ShiftLeft(Local0, 8, Local0)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				If (Local2) {
					CreateWordField (CRS, IRQX._INT, IRQW)
					Store (One, Local3)
					ShiftLeft (Local3, Local2, IRQW)
				}

				Return (CRS)
			}

		}
		#endif

		#ifndef NO_W83627HF_MIDI
		/* ========================== MIDI Port ========================== */
		Device (MIDI) {
			Name (_HID, EisaId ("PNPB006"))
			Name (_STR, Unicode ("MPU-401 Compatible MIDI Port"))
			Name (_UID, "w83627hf-midi")

			Method (_STA) {
				ENCM ()
				Store(0, Local0)
				Store (0x07, LDN)
				If (LOr(IO2L, IO2H)) {
					If (LOr(ACTR, ACT2)) {
						Store (0x0F, Local0)
					}
					Else {
						Store (0x0D, Local0)
					}
				}
				EXCM ()
				Return (Local0)
			}

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x02, 0x02, IO0)
					IRQNoFlags (IRQX) {}
				})
				ENCM ()
				Store (0x07, LDN)
				Store(IO2H, Local0)
				Store(IO2L, Local1)
				Store(IRQ1, Local2)
				EXCM ()
				ShiftLeft(Local0, 8, Local0)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				If (Local2) {
					CreateWordField (CRS, IRQX._INT, IRQW)
					Store (One, Local3)
					ShiftLeft (Local3, Local2, IRQW)
				}

				Return (CRS)
			}

		}
		#endif

		/* ==== Suspend LED control if it is connected to the SuperIO ==== */
		Method (SLED, 1) {
			ENCM ()
			Store (9, LDN)
			Store(OPT4, Local0)
			And(Local0, 63, Local0)
			Or(Local0, ShiftLeft(And(Arg0, 0x03), 6), OPT4)
			EXCM ()
		}

		/* ===== Power LED control if it is connected to the SuperIO ===== */
		Method (PLED, 1) {
			ENCM ()
			Store (8, LDN)
			Store(OPT4, Local0)
			And(Local0, 63, Local0)
			Or(Local0, ShiftLeft(And(Arg0, 0x03), 6), OPT4)
			EXCM ()
		}

		#ifndef NO_W83627HF_HWMON
		/* ====================== Hardware Monitor ======================= */
		Device (HMON) {
			Name (_HID, EisaId ("PNP0C02")) // TODO: find better matching ID
			Name (_STR, Unicode("W83627 Hardware Monitor"))
			Name (_UID, "w83627hf-hwmon")

			Method (_STA)
			{
				Store (0x00, Local0)
				ENCM ()
				Store (0x0B, LDN)
				If (ACTR) {
					Store (0x0F, Local0)
				}
				ElseIf (LOr (IO1H, IO1L))
				{
					Store (0x0D, Local0)
				}
				EXCM ()
				Return (Local0)
			}

			Method (_PSC) {
				Store(^^_PSC (), Local0)
				If (Local0) { Return (Local0) }
				ENCM ()
				Store (HWPW, Local0)
				EXCM ()
				If (Local0) { Return (1) }
				Else { Return (0) }
			}
			Method (_PS0) {
				ENCM ()
				Store (Zero, HWPW)
				EXCM ()
			}
			Method (_PS1) {
				ENCM ()
				Store (One, HWPW)
				EXCM ()
			}
			Name (_PR0, Package () { ^^ })

			Method (_CRS)
			{
				Name (CRS, ResourceTemplate () {
					IO (Decode16, 0x0000, 0x0000, 0x08, 0x02, IO0)
					IRQNoFlags (IRQX) {}
				})
				ENCM ()
				Store (0x0B, LDN)
				Store(IO1H, Local0)
				Store(IO1L, Local1)
				Store(IRQ1, Local2)
				EXCM ()
				ShiftLeft(Local0, 8, Local0)
				Or (Local1, Local0, Local0)

				CreateWordField (CRS, IO0._MIN, IMIN)
				Store (Local0, IMIN)
				CreateWordField (CRS, IO0._MAX, IMAX)
				Store (Local0, IMAX)

				If (Local2) {
					CreateWordField (CRS, IRQX._INT, IRQW)
					Store (One, Local3)
					ShiftLeft (Local3, Local2, IRQW)
				}
				Return (CRS)
			}
		}
		#endif
	}
}