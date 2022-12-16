/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * include this file into a mainboard's DSDT _SB device tree and it will expose the
 * W83627HF SuperIO and its functionality.
 *
 * Devices are marked as nonexistent if they got 0x00000000 as I/O base address
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
 *                       PnP "Motherboard Resource"
 * W83627HF_KBC_COMPAT:  show the keyboard controller and the PS/2 mouse as
 *                       enabled if it is disabled but an address is assigned
 *                       to it. This may be necessary in some cases.
 *
 * Datasheet: "W83627HF/F WINBOND I/O" rev. 6.0
 * http://www.itox.com/pages/support/wdt/W83627HF.pdf
 */

#include <superio/acpi/pnp.asl>

Device(SIO) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("Winbond W83627HF SuperIO"))
	Name (_UID, "w83627hf")

	/* Mutex for accesses to the configuration ports (prolog and epilog commands are used, so synchronization is useful) */
	Mutex(CRMX, 1)

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8
	}
	IndexField (PNP_ADDR_REG, PNP_DATA_REG, ByteAcc, NoLock, Preserve)
	{
		Offset (0x02),
		RST,	1,	/* Soft reset */
		,	7,
		Offset (0x07),
		LDN,	8,	/* Logical device selector */
		Offset (0x20),
		DID,	8,	/* Device ID */
		DREV,	8,	/* Device Revision */
		FDPW,	1,	/* FDC Power Down */
		,	2,
		PRPW,	1,	/* PRT Power Down */
		UAPW,	1,	/* UART A Power Down */
		UBPW,	1,	/* UART B Power Down */
		HWPW,	1,	/* HWM Power Down */
		,	1,
		IPD,	1,	/* Immediate Chip Power Down */
		,	7,
		PNPS,	1,	/* PnP Address Select Register Default Value Mode */
		,	1,
		KBCR,	1,	/* KBC enabled after system reset (read-only) */
		,	3,
		CLKS,	1,	/* Clock select */
		AQ16,	1,	/* 16bit Address Qualification */
		FDCT,	1,	/* Tristate FDC (?) */
		,	2,
		PRTT,	1,	/* Tristate parallel port (?) */
		URAT,	1,	/* Tristate UART A (?) */
		URBT,	1,	/* Tristate UART B (?) */
		,	2,
		URAI,	1,	/* UART A Legacy IRQ Select Disable */
		URBI,	1,	/* UART B Legacy IRQ Select Disable */
		PRTI,	1,	/* Parallel Port Legacy IRQ/DRQ Select Disable */
		FDCI,	1,	/* FDC Legacy IRQ/DRQ Select Disable */
		,	1,
		LCKC,	1,	/* Lock Configuration Registers */
		Offset (0x29),
		IO3S,	8,	/* GPIO3 pin selection register */
		Offset (0x30),
		ACTR,	1,	/* Logical device activation */
		ACT1,	1,	/* Logical part activation 1 (mostly unused) */
		ACT2,	1,	/* Logical part activation 2 (mostly unused) */
		,	5,
		Offset (0x60),
		IO1H,	8,	/* First I/O port base - high byte */
		IO1L,	8,	/* First I/O port base - low byte */
		IO2H,	8,	/* Second I/O port base - high byte */
		IO2L,	8,	/* Second I/O port base - low byte */
		Offset (0x70),
		IRQ0,	8,	/* First IRQ */
		Offset (0x72),
		IRQ1,	8,	/* First IRQ */
		Offset (0x74),
		DMA0,	8,	/* DMA */
		Offset (0xE0),
		/* CRE0-CRE4: function logical device dependent, seems to be reserved for ACPI settings */
		CRE0,	8,
		CRE1,	8,
		CRE2,	8,
		CRE3,	8,
		CRE4,	8,
		Offset (0xF0),
		/* OPT1-OPTA aka CRF0-CRF9: function logical device dependent */
		OPT1,	8,
		OPT2,	8,
		OPT3,	8,
		OPT4,	8,
		OPT5,	8,
		OPT6,	8,
		OPT7,	8,
		OPT8,	8,
		OPT9,	8,
		OPTA,	8
	}

	Method (_CRS)
	{
		Return (ResourceTemplate () {
		/* Announce the used I/O ports to the OS */
		IO (Decode16, SUPERIO_PNP_BASE, SUPERIO_PNP_BASE, 0x02, 0x01)
		})
	}

	/* Enter configuration mode (and acquire mutex)
	   Method must be run before accessing the configuration region.
	   Parameter is the LDN which should be accessed. Values >= 0xFF mean
	   no LDN switch should be done.
	*/
	Method (ENTER_CONFIG_MODE, 1)
	{
		Acquire (CRMX, 0xFFFF)
		ADDR = 0x87
		ADDR = 0x87
		If (Arg0 < 0xFF) {
			LDN = Arg0
		}
	}

	/* Exit configuration mode (and release mutex)
	   Method must be run after accessing the configuration region.
	*/
	Method (EXIT_CONFIG_MODE)
	{
		ADDR = 0xAA
		Release (CRMX)
	}

	/* PM: indicate IPD (Immediate Power Down) bit state as D0/D3 */
	Method (_PSC) {
		ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
		Local0 = IPD
		EXIT_CONFIG_MODE ()
		If (Local0) { Return (3) }
		Else { Return (0) }
	}

	/* PM: Switch to D0 by setting IPD low */
	Method (_PS0) {
		ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
		IPD = Zero
		EXIT_CONFIG_MODE ()
	}

	/* PM: Switch to D3 by setting IPD high */
	Method (_PS3) {
		ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
		IPD = One
		EXIT_CONFIG_MODE ()
	}

	#ifndef NO_W83627HF_FDC
	/* =================== Floppy Disk Controller ================ */
	Device (FDC0) {
		Name (_HID, EisaId ("PNP0700"))
		Name (_STR, Unicode ("W83627HF Floppy Disk Controller"))
		Name (_UID, "w83627hf-fdc")

		Method (_STA)
		{
			Local0 = 0x00
			ENTER_CONFIG_MODE (0)
			If (ACTR) {
				Local0 = 0x0F
			}
			ElseIf (IO1H || IO1L)
			{
				Local0 = 0x0D
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		/* Current power state (Returns 1 if LDN in power saving mode,
		 * 2 if whole chip is powered down), 0 else
		 */
		Method (_PSC) {
			Local0 = ^^_PSC ()
			If (Local0) { Return (Local0) }
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			Local0 = FDPW
			EXIT_CONFIG_MODE ()
			If (Local0) { Return (3) }
			Else { Return (0) }
		}
		/* Disable power saving mode */
		Method (_PS0) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			FDPW = One
			EXIT_CONFIG_MODE ()
		}
		/* Enable power saving mode */
		Method (_PS3) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			FDPW = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (0)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x07, IO0)
				IRQNoFlags () {6}
				DMA (Compatibility, NotBusMaster, Transfer8) {2}
			})

			/* Get IO port info */
			ENTER_CONFIG_MODE (0)
			Local0 = IO1L
			Local1 = IO1H
			EXIT_CONFIG_MODE ()

			/* Calculate full IO port address */
			Or(Local1 << 8, Local0, Local0)

			/* Modify the resource template and return it */
			CreateWordField (CRS, IO0._MIN, IMIN)
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMIN = Local0
			IMAX = Local0
			Return (CRS)
		}

		Method (_PRS)
		{
			Return (ResourceTemplate ()
			{
				StartDependentFnNoPri ()
				{
					IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x07)
					IRQNoFlags () {6}
					DMA (Compatibility, NotBusMaster, Transfer8) {2}
				}
				EndDependentFn ()
			})
		}

		#ifndef NO_W83627HF_FDC_ENUM
		Method (_FDE, 0) {
			/* Try probing drives.
			   Probing is done through selecting and activating a drive
			   and reading 0x03F7 aka the "shared IDE and floppy register"
			   as any value there besides zero seems to indicate a
			   connected drive.
			*/
			// Create template
			Name(FDE, Buffer(){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
			CreateByteField (FDE, 1, FD1)
			CreateByteField (FDE, 4, FD2)
			CreateByteField (FDE, 8, FD3)
			CreateByteField (FDE, 12, FD4)

			// Get resources from logical device
			ENTER_CONFIG_MODE (0)
			Local0 = ACTR
			Local1 = IO1H
			Local2 = IO1L
			EXIT_CONFIG_MODE ()
			Local1 <<= 8
			Or(Local1, Local2, Local1)
			If (!Local0) {
				Return (FDE)
			}

			OperationRegion (FIO1, SystemIO, Local1, 0x06)
			Field (FIO1, ByteAcc, NoLock, Preserve)
			{
				Offset(0x02),
				SELE,	2,
				RSTL,	1,
				IDMA,	1,
				ACT1,	1,
				ACT2,	1,
				ACT3,	1,
				ACT4,	1,
				Offset(0x04),
				BSY1,	1,
				BSY2,	1,
				BSY3,	1,
				BSY4,	1,
				BUSY,	1,
				NDMA,	1,
				IODI,	1,
				RDY,	1,
				DATA,	8,
			}
			OperationRegion (FIO2, SystemIO, 0x3F7, 0x01)
			Field (FIO2, ByteAcc, NoLock, Preserve)
			{
				SIFR, 8
			}

			ACT1 = One
			SELE = 0
			Sleep(0x64)
			If (SIFR) { FD1 = One }

			ACT1 = Zero
			ACT2 = One
			SELE = 1
			Sleep(0x64)
			If (SIFR) { FD2 = One }

			ACT2 = Zero
			ACT3 = One
			SELE = 2
			Sleep(0x64)
			If (SIFR) { FD3 = One }

			ACT3 = Zero
			ACT4 = One
			SELE = 3
			Sleep(0x64)
			If (SIFR) { FD4 = One }
			ACT4 = Zero
			SELE = Zero

			Return (FDE)
		}
		#endif


		Method (_SRS, 1, Serialized)
		{
			Name (TMPL, ResourceTemplate () {
				IO (Decode16, 0, 0, 1, 7, IO0)
				IRQNoFlags (IRQ0) {}
				DMA (Compatibility, NotBusMaster, Transfer8, DMA0) {}
			})
			CreateWordField (Arg0, IO0._MIN, IOA0)
			CreateByteField (Arg0, IRQ0._INT, IRQL)
			CreateByteField (Arg0, DMA0._DMA, DMCH)

			Divide(IOA0, 256, Local0, Local1)

			ENTER_CONFIG_MODE (0)
			IO1L = Local0
			IO1H = Local1
			ACTR = One
			EXIT_CONFIG_MODE ()
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
			ENTER_CONFIG_MODE (1)
			And(OPT1, 0x3, Local1)
			Or(Local1, Local0, OPT1)
			EXIT_CONFIG_MODE()
		}

		Method (_INI)
		{
			/* Deactivate DMA, even if set by BIOS. We don't announce it
			   through _CRS and it's only useful in ECP mode which we
			   don't support at the moment. */
			ENTER_CONFIG_MODE (1)
			DMA0 = 0x04
			EXIT_CONFIG_MODE ()
		}

		Method (_STA)
		{
			Local0 = 0x00
			ENTER_CONFIG_MODE (1)
			And(OPT1, 0x3, Local1)
			If (ACTR) {
				If (Local1 != 2) {
					Local0 = 0x0D
				} Else {
					Local0 = 0x0D
				}
			}
			ElseIf (IO1H || IO1L)
			{
				Local0 = 0x0D
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_PSC) {
			Local0 = ^^_PSC ()
			If (Local0) { Return (Local0) }
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			Local0 = PRPW
			EXIT_CONFIG_MODE ()
			If (Local0) { Return (3) }
			Else { Return (0) }
		}
		Method (_PS0) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			PRPW = One
			EXIT_CONFIG_MODE ()
		}
		Method (_PS3) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			PRPW = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_DIS) {
			ENTER_CONFIG_MODE (1)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
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

			/* Get device settings */
			ENTER_CONFIG_MODE (1)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = OPT1
			Local5 = IRQ0
			EXIT_CONFIG_MODE ()
			/* Calculate IO port and modify template */
			Or(Local1 << 8, Local0, Local0)
			IOP0 = Local1
			IOR0 = Local1

			/* Set align and length based on active parallel port mode */
			And(Local2, 0x3, Local3)
			And(Local2, 0x4, Local4)
			If (Local4) {
				IOAL = 0x04
			}
			If (Local0 == 0xBC)
			{
				IOLE = 0x04
			}
			Else
			{
				IOLE = 0x08
			}
			/* Calculate IRQ bitmap */
			Local0 = One
			IRQW = Local0 << Local5
			/* Return resource template */
			Return (CRS)
		}

		Name (_PRS, ResourceTemplate ()
		{
			/* Traditional configurations (SPP mode) */
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
			/* Traditional configurations (EPP mode) */
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
			/* Any configurable address (EPP mode) */
			StartDependentFn (2,0)
			{
				IO (Decode16, 0x0100, 0x0FF8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			/* Any configurable address (No EPP mode) */
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

			If (IOAL == 4) {
				Local2 = 0x0
			} else {
				Local2 = 0x1
			}

			Divide(IOA0, 256, Local0, Local1)

			ENTER_CONFIG_MODE (1)
			/* IO port */
			IO1L = Local0
			IO1H = Local1
			/* Mode */
			Local3 = OPT1
			And (Local3, 0xF8, Local3)
			Or (Local2, Local3, OPT1)
			/* DMA off */
			DMA0 = 0x04
			/* IRQ */
			IRQ0 = FindSetLeftBit (IRQL) - 1
			/* Activate */
			ACTR = One
			EXIT_CONFIG_MODE ()
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
			Local0 = 0x00
			ENTER_CONFIG_MODE (2)
			If (ACTR) {
				Local0 = 0x0F
			}
			ElseIf (IO1H || IO1L)
			{
				Local0 = 0x0D
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_PSC) {
			Local0 = ^^_PSC ()
			If (Local0) { Return (Local0) }
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			Local0 = UAPW
			EXIT_CONFIG_MODE ()
			If (Local0) { Return (3) }
			Else { Return (0) }
		}
		Method (_PS0) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			UAPW = One
			EXIT_CONFIG_MODE ()
		}
		Method (_PS3) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			UAPW = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (2)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS, 0, Serialized)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
				IRQNoFlags (IRQX) {6}
			})
			ENTER_CONFIG_MODE (2)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = IRQ0
			EXIT_CONFIG_MODE ()
			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			IMIN = Local0
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMAX = Local0

			CreateWordField (CRS, IRQX._INT, IRQW)
			Local3 = One
			IRQW = Local3 << Local2

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
			CreateWordField (Arg0, IRQX._INT, IRQL)

			Divide(IOA0, 256, Local0, Local1)

			Local3 = FindSetLeftBit (IRQL) - 1

			ENTER_CONFIG_MODE (2)
			IO1L = Local0
			IO1H = Local1
			IRQ0 = Local3
			ACTR = One
			EXIT_CONFIG_MODE ()
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
			Local0 = 0x00
			ENTER_CONFIG_MODE (3)
			If (!And(OPT2, 0x30))
			{
				If (ACTR) {
					Local0 = 0x0F
				}
				ElseIf (IO1H || IO1L)
				{
					Local0 = 0x0D
				}
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_PSC) {
			Local0 = ^^_PSC ()
			If (Local0) { Return (Local0) }
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			Local0 = UBPW
			EXIT_CONFIG_MODE ()
			If (Local0) { Return (3) }
			Else { Return (0) }
		}
		Method (_PS0) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			UBPW = One
			EXIT_CONFIG_MODE ()
		}
		Method (_PS3) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			UBPW = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (3)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
				IRQNoFlags (IRQX) {6}
			})
			ENTER_CONFIG_MODE (3)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = IRQ0
			EXIT_CONFIG_MODE ()
			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			IMIN = Local0
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMAX = Local0

			CreateWordField (CRS, IRQX._INT, IRQW)
			Local3 = One
			IRQW = Local3 << Local2

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

			Local3 = FindSetLeftBit (IRQL) - 1

			ENTER_CONFIG_MODE (3)
			IO1L = Local0
			IO1H = Local1
			IRQ0 = Local3
			ACTR = One
			EXIT_CONFIG_MODE ()
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
			Local0 = 0x00
			ENTER_CONFIG_MODE (3)
			If (And(OPT2, 0x30))
			{
				If (ACTR) {
					Local0 = 0x0F
				}
				ElseIf (IO1H || IO1L)
				{
					Local0 = 0x0D
				}
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_PSC) {
			Local0 = ^^_PSC ()
			If (Local0) { Return (Local0) }
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			Local0 = UBPW
			EXIT_CONFIG_MODE ()
			If (Local0) { Return (3) }
			Else { Return (0) }
		}
		Method (_PS0) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			UBPW = One
			EXIT_CONFIG_MODE ()
		}
		Method (_PS3) {
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			UBPW = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (3)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
				IRQNoFlags (IRQX) {6}
			})
			ENTER_CONFIG_MODE (3)
			Local1 = IO1H
			Local0 = IO1L
			Local2 = IRQ0
			EXIT_CONFIG_MODE ()
			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			IMIN = Local0
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMAX = Local0

			CreateWordField (CRS, IRQX._INT, IRQW)
			Local3 = One
			IRQW = Local3 << Local2

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

			Local3 = FindSetLeftBit (IRQL) - 1

			ENTER_CONFIG_MODE (3)
			IO1L = Local0
			IO1H = Local1
			IRQ0 = Local3
			ACTR = One
			EXIT_CONFIG_MODE ()
		}
	}
	#endif

	#ifndef NO_W83627HF_CIR
	/* ========================= Consumer IR ========================= */
	Device (CIR0)
	{
		Name (_HID, EisaId ("WEC1022")) /* Should be the correct one */
		Name (_UID, "w83627hf-cir")
		Name (_STR, Unicode("Winbond Consumer Infrared Transceiver"))

		Method (_STA)
		{
			Local0 = 0x00
			ENTER_CONFIG_MODE (6)
			If (ACTR) {
				Local0 = 0x0F
			}
			ElseIf (IO1H || IO1L)
			{
				Local0 = 0x0D
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (6)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x08, IO0)
				IRQNoFlags (IRQX) {6}
			})
			ENTER_CONFIG_MODE (6)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = IRQ0
			EXIT_CONFIG_MODE ()
			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			IMIN = Local0
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMAX = Local0

			CreateWordField (CRS, IRQX._INT, IRQW)
			Local3 = One
			IRQW = Local3 << Local2

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

			Local3 = FindSetLeftBit (IRQL) - 1

			ENTER_CONFIG_MODE (6)
			IO1L = Local0
			IO1H = Local1
			IRQ0 = Local3
			ACTR = One
			EXIT_CONFIG_MODE ()
		}
	}
	#endif

	#ifndef NO_W83627HF_KBC
	/* ===================== Keyboard Controller ===================== */
	Device (KBD0)
	{
		Name (_HID, EisaId ("PNP0303"))
		Name (_UID, "w83627hf-kbc")

		Method (_STA)
		{
			Local0 = 0x00
			ENTER_CONFIG_MODE (5)
			If (ACTR) {
				Local0 = 0x0F
			}
			ElseIf (IO1H || IO1L || IO2H || IO2L)
			{
				#ifdef W83627HF_KBC_COMPAT
				Local0 = 0x0F
				#else
				Local0 = 0x0D
				#endif
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (5)
			ACTR = Zero
			EXIT_CONFIG_MODE ()
			Notify(PS2M, 1)
		}

		Method (_CRS, 0, Serialized)
		{
			Name (CRS, ResourceTemplate () {
				IRQNoFlags (IRQX) {}
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO1)
			})
			ENTER_CONFIG_MODE (5)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = IO2L
			Local3 = IO2H
			Local4 = IRQ0
			EXIT_CONFIG_MODE ()

			Or(Local1 << 8, Local0, Local0)
			Or(Local3 << 8, Local2, Local2)

			CreateWordField (CRS, IO0._MIN, IMIN)
			IMIN = Local0
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMAX = Local0

			CreateWordField (CRS, IO1._MIN, I1MI)
			I1MI = Local2
			CreateWordField (CRS, IO1._MAX, I1MA)
			I1MA = Local2

			CreateWordField (CRS, IRQX._INT, IRQW)
			Local5 = One
			IRQW = Local5 << Local4

			Return (CRS)
		}

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0,0) {
				IRQNoFlags () {1}
				IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
				IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
			}
			EndDependentFn()
		})

		Method (_SRS, 1, Serialized)
		{
			Name (TMPL, ResourceTemplate () {
				IRQNoFlags (IRQX) {}
				IO (Decode16, 0, 0, 1, 1, IO0)
				IO (Decode16, 0, 0, 1, 1, IO1)
			})
			CreateWordField (Arg0, IO0._MIN, IOA0)
			CreateWordField (Arg0, IO1._MIN, IOA1)
			CreateWordField (Arg0, IRQX._INT, IRQL)

			Divide(IOA0, 256, Local0, Local1)
			Divide(IOA1, 256, Local2, Local3)

			Local4 = FindSetLeftBit (IRQL) - 1

			ENTER_CONFIG_MODE (5)
			IO1L = Local0
			IO1H = Local1
			IO2L = Local2
			IO2H = Local3
			IRQ0 = Local4
			ACTR = One
			EXIT_CONFIG_MODE ()
			Notify(PS2M, 1)
		}
	}
	#ifndef NO_W83627HF_PS2M
	Device (PS2M)
	{
		Name (_HID, EisaId ("PNP0F13"))
		Name (_UID, "w83627hf-ps2m")

		Method (_STA)
		{
			Local0 = 0x00
			ENTER_CONFIG_MODE (5)
			If (ACTR && IRQ1) {
				Local0 = 0x0F
			}
			ElseIf (IO1H || IO1L || IO2H || IO2L)
			{
				#ifdef W83627HF_KBC_COMPAT
				Local0 = 0x0F
				#else
				Local0 = 0x0D
				#endif
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_DIS)
		{
			ENTER_CONFIG_MODE (5)
			IRQ1 = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS, 0, Serialized)
		{
			Name (CRS, ResourceTemplate () {
				IRQNoFlags (IRQX) {}
			})
			ENTER_CONFIG_MODE (5)
			Local4 = IRQ1
			EXIT_CONFIG_MODE ()

			CreateWordField (CRS, IRQX._INT, IRQW)
			Local5 = One
			IRQW = Local5 << Local4

			Return (CRS)
		}

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0,0) {
				IRQNoFlags () {12}
			}
			StartDependentFn (2,0) {
				IRQNoFlags () {1,3,4,5,6,7,9,10,11}
			}
			EndDependentFn()
		})

		Method (_SRS, 1, Serialized)
		{
			Name (TMPL, ResourceTemplate () {
				IRQNoFlags (IRQX) {}
			})
			CreateWordField (Arg0, IRQX._INT, IRQL)

			Local0 = FindSetLeftBit (IRQL) - 1

			ENTER_CONFIG_MODE (5)
			IRQ1 = Local0
			/* Only activates if KBD is active */
			EXIT_CONFIG_MODE ()
		}
	}
	#endif
	#endif

	#ifndef NO_W83627HF_GAME
	/* ========================== Game Port ========================== */
	Device (GAME)
	{
		Name (_HID, EisaId ("PNPB02F"))
		Name (_STR, Unicode ("Joystick/Game Port"))
		Name (_UID, "w83627hf-game")

		Method (_STA) {
			Local0 = 0
			ENTER_CONFIG_MODE (7)
			If (IO1L || IO1H) {
				If (ACTR || ACT1) {
					Local0 = 0x0F
				}
				Else {
					Local0 = 0x0D
				}
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_CRS)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
				IRQNoFlags (IRQX) {}
			})
			ENTER_CONFIG_MODE (7)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = IRQ0
			EXIT_CONFIG_MODE ()

			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMIN = Local0
			IMAX = Local0

			If (Local2) {
				CreateWordField (CRS, IRQX._INT, IRQW)
				Local3 = One
				IRQW = Local3 << Local2
			}

			Return (CRS)
		}

	}
	#endif

	#ifndef NO_W83627HF_MIDI
	/* ========================== MIDI Port ========================== */
	Device (MIDI)
	{
		Name (_HID, EisaId ("PNPB006"))
		Name (_STR, Unicode ("MPU-401 Compatible MIDI Port"))
		Name (_UID, "w83627hf-midi")

		Method (_STA)
		{
			Local0 = 0
			ENTER_CONFIG_MODE (7)
			If (IO2L || IO2H) {
				If (ACTR || ACT2) {
					Local0 = 0x0F
				}
				Else {
					Local0 = 0x0D
				}
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_CRS)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x02, 0x02, IO0)
				IRQNoFlags (IRQX) {}
			})
			ENTER_CONFIG_MODE (7)
			Local0 = IO2L
			Local1 = IO2H
			Local2 = IRQ1
			EXIT_CONFIG_MODE ()

			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMIN = Local0
			IMAX = Local0

			If (Local2) {
				CreateWordField (CRS, IRQX._INT, IRQW)
				Local3 = One
				IRQW = Local3 << Local2
			}

			Return (CRS)
		}

	}
	#endif

	/* ==== Suspend LED control if it is connected to the SuperIO ==== */
	Method (SLED, 1)
	{
		ENTER_CONFIG_MODE (9)
		Local0 = OPT4
		And(Local0, 63, Local0)
		Or(Local0, And(Arg0, 0x03) << 6, OPT4)
		EXIT_CONFIG_MODE ()
	}

	/* ===== Power LED control if it is connected to the SuperIO ===== */
	Method (PLED, 1)
	{
		ENTER_CONFIG_MODE (8)
		Local0 = OPT4
		And(Local0, 63, Local0)
		Or(Local0, And(Arg0, 0x03) << 6, OPT4)
		EXIT_CONFIG_MODE ()
	}

	#ifndef NO_W83627HF_HWMON
	/* ====================== Hardware Monitor ======================= */
	Device (HMON)
	{
		Name (_HID, EisaId ("PNP0C02")) // TODO: find better matching ID
		Name (_STR, Unicode("W83627 Hardware Monitor"))
		Name (_UID, "w83627hf-hwmon")

		Method (_STA)
		{
			Local0 = 0x00
			ENTER_CONFIG_MODE (11)
			If (ACTR) {
				Local0 = 0x0F
			}
			ElseIf (IO1H || IO1L)
			{
				Local0 = 0x0D
			}
			EXIT_CONFIG_MODE ()
			Return (Local0)
		}

		Method (_PSC)
		{
			Local0 = ^^_PSC ()
			If (Local0) { Return (Local0) }
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			Local0 = HWPW
			EXIT_CONFIG_MODE ()
			If (Local0) { Return (3) }
			Else { Return (0) }
		}

		Method (_PS0)
		{
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			HWPW = One
			EXIT_CONFIG_MODE ()
		}

		Method (_PS3)
		{
			ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
			HWPW = Zero
			EXIT_CONFIG_MODE ()
		}

		Method (_CRS, 0, Serialized)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x08, 0x02, IO0)
				IRQNoFlags (IRQX) {}
			})
			ENTER_CONFIG_MODE (11)
			Local0 = IO1L
			Local1 = IO1H
			Local2 = IRQ1
			EXIT_CONFIG_MODE ()

			Or(Local1 << 8, Local0, Local0)

			CreateWordField (CRS, IO0._MIN, IMIN)
			CreateWordField (CRS, IO0._MAX, IMAX)
			IMIN = Local0
			IMAX = Local0

			If (Local2) {
				CreateWordField (CRS, IRQX._INT, IRQW)
				Local3 = One
				IRQW = Local3 << Local2
			}
			Return (CRS)
		}
	}
	#endif

	/* Returns the wake source register.
	   It is cleared after reading.
	   Bit 0: Keyboard wake-up event
	   Bit 1: Mouse wake-up event
	   Bit 2: Power button event
	   Bit 3: CIR wake-up event
	   Bit 4: Power loss event
	   Bit 5: VSB power loss status
	*/
	Method (WAKS)
	{
		ENTER_CONFIG_MODE (10)
		Local0 = CRE3
		EXIT_CONFIG_MODE ()
		Return (Local0)
	}
}
