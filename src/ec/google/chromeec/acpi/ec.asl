/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The mainboard must define a PNOT method to handle power
 * state notifications and Notify CPU device objects to
 * re-evaluate their _PPC and _CST tables.
 */

// DTT Power Participant Device Notification
#define POWER_STATE_CHANGE_NOTIFICATION 0x81
// DTT OEM variables change notification
#define EC_OEM_VARIABLE_DATA_MASK	0x7
#define INT3400_ODVP_CHANGED		0x88

#define ACPI_NOTIFY_CROS_EC_PANIC	0xB0

// Mainboard specific throttle handler
#ifdef DPTF_ENABLE_CHARGER
External (\_SB.DPTF.TCHG, DeviceObj)
#endif
/* Enable DPTC interface with AMD ALIB */
#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
External(\_SB.DPTC, MethodObj)
#endif

External (\_SB.DPTF.TPWR, DeviceObj)

#ifdef DPTF_ENABLE_OEM_VARIABLES
External (\_SB.DPTF.ODVP, MethodObj)
External (\_SB.DPTF.ODGT, MethodObj)
External (\_SB.DPTF.ODUP, MethodObj)
#endif

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, EC_SCI_GPI)
	Name (TOFS, EC_TEMP_SENSOR_OFFSET)
	Name (TNCA, EC_TEMP_SENSOR_NOT_CALIBRATED)
	Name (TNOP, EC_TEMP_SENSOR_NOT_POWERED)
	Name (TBAD, EC_TEMP_SENSOR_ERROR)
	Name (TNPR, EC_TEMP_SENSOR_NOT_PRESENT)
	Name (DWRN, 15)		// Battery capacity warning at 15%
	Name (DLOW, 10)		// Battery capacity low at 10%

	OperationRegion (ERAM, EmbeddedControl, 0x00, EC_ACPI_MEM_MAPPED_BEGIN)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		RAMV, 8,	// EC RAM Version
		TSTB, 8,	// Test Byte
		TSTC, 8,	// Complement of Test Byte
		KBLV, 8,	// Keyboard Backlight value
		FAND, 8,	// Set Fan Duty Cycle
		PATI, 8,	// Programmable Auxiliary Trip Sensor ID
		PATT, 8,	// Programmable Auxiliary Trip Threshold
		PATC, 8,	// Programmable Auxiliary Trip Commit
		CHGL, 8,	// Charger Current Limit
		TBMD, 1,	// Tablet mode
		DDPN, 3,	// Device DPTF Profile Number
		STTB, 1,	// Switch thermal table by body detection status
		// DFUD must be 0 for the other 31 values to be valid
		Offset (0x0a),
		DFUD, 1,	// Device Features Undefined
		FLSH, 1,	// Flash commands present
		PFAN, 1,	// PWM Fan control present
		KBLE, 1,	// Keyboard Backlight present
		LTBR, 1,	// Lightbar present
		LEDC, 1,	// LED control
		MTNS, 1,	// Motion sensors present
		KEYB, 1,	// EC is keyboard controller
		PSTR, 1,	// Persistent storage
		P80P, 1,	// EC serves I/O Port 80h
		THRM, 1,	// EC supports thermal management
		SBKL, 1,	// Screen backlight switch present
		WIFI, 1,	// WIFI switch present
		HOST, 1,	// EC monitors host events (eg SCI, SMI)
		GPIO, 1,	// EC provides GPIO commands
		I2CB, 1,	// EC provides I2C controller access
		CHRG, 1,	// EC provides commands for charger control
		BATT, 1,	// Simply Battery support
		SBAT, 1,	// Smart Battery support
		HANG, 1,	// EC can detect host hang
		PMUI, 1,	// Power Information
		DSEC, 1,	// another EC exists downstream
		UPDC, 1,	// supports USB Power Delivery
		UMUX, 1,	// supports USB Mux
		MSFF, 1,	// Motion Sense has FIFO
		TVST, 1,	// supports temporary secure vstore
		TCMV, 1,	// USB Type C Muxing is virtual (host assisted)
		RTCD, 1,	// EC provides an RTC device
		FPRD, 1,	// EC provides a fingerprint reader device
		TPAD, 1,	// EC provides a touchpad device
		RWSG, 1,	// EC has RWSIG task enabled
		DEVE, 1,	// EC supports device events
		// make sure we're within our space envelope
		Offset (0x0e),
		Offset (0x12),
		BTID, 8,	// Battery index that host wants to read
		USPP, 8,	// USB Port Power
		RFWU, 8,	// Retimer Firmware Update
		PBOK, 8,	// Power source change count from dptf
	}

#if CONFIG(EC_GOOGLE_CHROMEEC_ACPI_MEMMAP)
	OperationRegion (EMEM, EmbeddedControl,
			 EC_ACPI_MEM_MAPPED_BEGIN, EC_ACPI_MEM_MAPPED_SIZE)
	Field (EMEM, ByteAcc, Lock, Preserve)
#else
	OperationRegion (EMEM, SystemIO, EC_LPC_ADDR_MEMMAP, EC_MEMMAP_SIZE)
	Field (EMEM, ByteAcc, NoLock, Preserve)
#endif
	{
		#include "emem.asl"
	}

#ifdef EC_ENABLE_LID_SWITCH
	/* LID Switch */
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (^^LIDS)
		}

#ifdef EC_ENABLE_WAKE_PIN
		Name (_PRW, Package () { EC_ENABLE_WAKE_PIN, 0x5 })
#endif
	}
#endif

	Method (TINS, 1, Serialized)
	{
		Switch (ToInteger (Arg0))
		{
			Case (0) { Return (TIN0) }
			Case (1) { Return (TIN1) }
			Case (2) { Return (TIN2) }
			Case (3) { Return (TIN3) }
			Case (4) { Return (TIN4) }
			Case (5) { Return (TIN5) }
			Case (6) { Return (TIN6) }
			Case (7) { Return (TIN7) }
			Case (8) { Return (TIN8) }
			Case (9) { Return (TIN9) }
			Default  { Return (TIN0) }
		}
	}

	Method (_CRS, 0, Serialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16,
			    EC_LPC_ADDR_ACPI_DATA,
			    EC_LPC_ADDR_ACPI_DATA,
			    0, 1)
			IO (Decode16,
			    EC_LPC_ADDR_ACPI_CMD,
			    EC_LPC_ADDR_ACPI_CMD,
			    0, 1)
		})
		Return (ECMD)
	}

	Method (_REG, 2, NotSerialized)
	{
		// Initialize AC power state
		\PWRS = ACEX
		/*
		 * Call PNOT (Platform Notify) to inform platform code
		 * about the current AC/battery state. This handles all cases,
		 * the battery transitioning into and out of having critically
		 * low charge.
		 * PWRS isn't valid before this point.
		 */
		\PNOT ()

		// Initialize LID switch state
		\LIDS = LIDS

#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
		/*
		 * Per the device mode (clamshell or tablet) to initialize
		 * the thermal setting on OS startup.
		 */
		If (CondRefOf (\_SB.DPTC)) {
			\_SB.DPTC()
		}
#endif
	}

	/* Read requested temperature and check against EC error values */
	Method (TSRD, 1, Serialized)
	{
		Local0 = \_SB.PCI0.LPCB.EC0.TINS (Arg0)

		/* Check for sensor not calibrated */
		If (Local0 == \_SB.PCI0.LPCB.EC0.TNCA) {
			Return (Zero)
		}

		/* Check for sensor not present */
		If (Local0 == \_SB.PCI0.LPCB.EC0.TNPR) {
			Return (Zero)
		}

		/* Check for sensor not powered */
		If (Local0 == \_SB.PCI0.LPCB.EC0.TNOP) {
			Return (Zero)
		}

		/* Check for sensor bad reading */
		If (Local0 == \_SB.PCI0.LPCB.EC0.TBAD) {
			Return (Zero)
		}

		/* Adjust by offset to get Kelvin */
		Local0 += \_SB.PCI0.LPCB.EC0.TOFS

		/* Convert to 1/10 Kelvin */
		Local0 *= 10

		Return (Local0)
	}

	// Lid Closed Event
	Method (_Q01, 0, NotSerialized)
	{
		Printf ("EC: LID CLOSE")
#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
		If (CondRefOf (\_SB.DPTC)) {
			\_SB.DPTC()
		}
#endif
		\LIDS = LIDS
#ifdef EC_ENABLE_LID_SWITCH
		Notify (LID0, 0x80)
#endif
	}

	// Lid Open Event
	Method (_Q02, 0, NotSerialized)
	{
		Printf ("EC: LID OPEN")
#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
		If (CondRefOf (\_SB.DPTC)) {
			\_SB.DPTC()
		}
#endif
		\LIDS = LIDS
		Notify (CREC, 0x2)
#ifdef EC_ENABLE_LID_SWITCH
		Notify (LID0, 0x80)
#endif
	}

	// Power Button
	Method (_Q03, 0, NotSerialized)
	{
		Printf ("EC: POWER BUTTON")
	}

	// AC Connected
	Method (_Q04, 0, NotSerialized)
	{
		Printf ("EC: AC CONNECTED")
		\PWRS = ACEX
		Notify (AC, 0x80)
#ifdef DPTF_ENABLE_CHARGER
		If (CondRefOf (\_SB.DPTF.TCHG)) {
			Notify (\_SB.DPTF.TCHG, 0x80)
		}
#endif
		/*
		 * Call PNOT (Platform Notify) to inform platform code
		 * about the current battery state. This handles all cases,
		 * the battery transitioning into and out of having critically
		 * low charge.
		 */
		\PNOT ()
	}

	// AC Disconnected
	Method (_Q05, 0, NotSerialized)
	{
		Printf ("EC: AC DISCONNECTED")
		\PWRS = ACEX
		Notify (AC, 0x80)
#ifdef DPTF_ENABLE_CHARGER
		If (CondRefOf (\_SB.DPTF.TCHG)) {
			Notify (\_SB.DPTF.TCHG, 0x80)
		}
#endif
		/*
		 * Call PNOT (Platform Notify) to inform platform code
		 * about the current battery state. This handles all cases,
		 * the battery transitioning into and out of having critically
		 * low charge.
		 */
		\PNOT ()
	}

	// Battery Low Event
	Method (_Q06, 0, NotSerialized)
	{
		Printf ("EC: BATTERY LOW")
		Notify (BAT0, 0x80)
	}

	// Battery Critical Event
	Method (_Q07, 0, NotSerialized)
	{
		Printf ("EC: BATTERY CRITICAL")
		Notify (BAT0, 0x80)
	}

	// Battery Info Event
	Method (_Q08, 0, NotSerialized)
	{
		Printf ("EC: BATTERY INFO")
		Notify (BAT0, 0x81)
#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
		If (CondRefOf (BAT1)) {
			Notify (BAT1, 0x81)
		}
#endif
	}

	// Thermal Overload Event
	Method (_Q0A, 0, NotSerialized)
	{
		Printf ("EC: THERMAL OVERLOAD")
		Notify (\_TZ, 0x80)
	}

	// Thermal Event
	Method (_Q0B, 0, NotSerialized)
	{
		Printf ("EC: THERMAL")
		Notify (\_TZ, 0x80)
	}

	// Key Pressed
	Method (_Q0D, 0, NotSerialized)
	{
		Printf ("EC: KEY PRESSED")
		Notify (CREC, 0x2)
	}

	// Thermal Shutdown Imminent
	Method (_Q10, 0, NotSerialized)
	{
		Printf ("EC: THERMAL SHUTDOWN")
		Notify (\_TZ, 0x80)
	}

	// Battery Shutdown Imminent
	Method (_Q11, 0, NotSerialized)
	{
		Printf ("EC: BATTERY SHUTDOWN")
		Notify (BAT0, 0x80)
	}

	// Throttle Start
	Method (_Q12, 0, NotSerialized)
	{
#ifdef EC_ENABLE_THROTTLING_HANDLER
		Printf ("EC: THROTTLE START")
		\_TZ.THRT (1)
#endif

#ifdef DPTF_ENABLE_OEM_VARIABLES
		Local0 = ToInteger(EOVD) & EC_OEM_VARIABLE_DATA_MASK
		\_SB.DPTF.ODUP(0, Local0)
		Local0 = \_SB.DPTF.ODGT(0)
		\_SB.DPTF.ODVP()
		Notify (\_SB.DPTF, INT3400_ODVP_CHANGED)
#endif
	}

	// Throttle Stop
	Method (_Q13, 0, NotSerialized)
	{
#ifdef EC_ENABLE_THROTTLING_HANDLER
		Printf ("EC: THROTTLE STOP")
		\_TZ.THRT (0)
#endif
	}

#ifdef EC_ENABLE_PD_MCU_DEVICE
	// PD event
	Method (_Q16, 0, NotSerialized)
	{
		Printf ("EC: GOT PD EVENT")
		Notify (\_SB.PCI0.LPCB.EC0.CREC.ECPD, 0x80)
		If (CondRefOf (\_SB.DPTF.TPWR)) {
			Notify (\_SB.DPTF.TPWR, POWER_STATE_CHANGE_NOTIFICATION)
		}
	}
#endif

	// Battery Status
	Method (_Q17, 0, NotSerialized)
	{
		Printf ("EC: BATTERY STATUS")
		Notify (BAT0, 0x80)
#ifdef EC_ENABLE_SECOND_BATTERY_DEVICE
		If (CondRefOf (BAT1)) {
			Notify (BAT1, 0x80)
		}
#endif

		/*
		 * Call PNOT (Platform Notify) to inform platform code
		 * about the current battery state. This handles all cases,
		 * the battery transitioning into and out of having critically
		 * low charge.
		 */
		\PNOT ()
	}

	// EC Panic
	Method (_Q18, 0, NotSerialized)
	{
		Printf ("EC: PANIC")
		Notify (CREC, ACPI_NOTIFY_CROS_EC_PANIC)
	}

	// MKBP interrupt.
	Method (_Q1B, 0, NotSerialized)
	{
		Printf ("EC: MKBP")
		Notify (CREC, 0x80)
	}

#ifdef EC_ENABLE_PD_MCU_DEVICE
	// USB MUX Interrupt
	Method (_Q1C, 0, NotSerialized)
	{
		Printf ("EC: USB MUX")
		Notify (\_SB.PCI0.LPCB.EC0.CREC.ECPD, 0x80)
	}
#endif

	// TABLET mode switch Event
	Method (_Q1D, 0, NotSerialized)
	{
		Printf ("EC: TABLET mode switch Event")
		Notify (CREC, 0x2)
#ifdef EC_ENABLE_MULTIPLE_DPTF_PROFILES
		\_SB.DPTF.TPET()
#endif
#ifdef EC_ENABLE_TBMC_DEVICE
		Notify (TBMC, 0x80)
#endif
#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
		If (CondRefOf (\_SB.DPTC)) {
			\_SB.DPTC()
		}
#endif
	}

	/*
	 * Dynamic Platform Thermal Framework support
	 */

	/* Mutex for EC PAT interface */
	Mutex (PATM, 1)

	/*
	 * Set Aux Trip Point 0
	 *   Arg0 = Temp Sensor ID
	 *   Arg1 = Value to set
	 */
	Method (PAT0, 2, Serialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return (0)
		}

		/* Set sensor ID */
		^PATI = ToInteger (Arg0)

		/* Temperature is passed in 1/10 Kelvin */
		Local1 = ToInteger (Arg1) / 10

		/* Adjust by EC temperature offset */
		^PATT = Local1 - ^TOFS

		/* Set commit value with SELECT=0 and ENABLE=1 */
		^PATC = 0x02

		Release (^PATM)
		Return (1)
	}

	/*
	 * Set Aux Trip Point 1
	 *   Arg0 = Temp Sensor ID
	 *   Arg1 = Value to set
	 */
	Method (PAT1, 2, Serialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return (0)
		}

		/* Set sensor ID */
		^PATI = ToInteger (Arg0)

		/* Temperature is passed in 1/10 Kelvin */
		Local1 = ToInteger (Arg1) / 10

		/* Adjust by EC temperature offset */
		^PATT = Local1 - ^TOFS

		/* Set commit value with SELECT=1 and ENABLE=1 */
		^PATC = 0x03

		Release (^PATM)
		Return (1)
	}

	/* Disable Aux Trip Points
	 *   Arg0 = Temp Sensor ID
	 */
	Method (PATD, 1, Serialized)
	{
		If (Acquire (^PATM, 1000)) {
			Return (0)
		}

		^PATI = ToInteger (Arg0)
		^PATT = 0x00

		/* Disable PAT0 */
		^PATC = 0x00

		/* Disable PAT1 */
		^PATC = 0x01

		Release (^PATM)
		Return (1)
	}

	/*
	 * Thermal Threshold Event
	 */
	Method (_Q09, 0, NotSerialized)
	{

#if CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_DPTC)
		If (CondRefOf (\_SB.DPTC)) {
			\_SB.DPTC()
		}
#endif
		If (!Acquire (^PATM, 1000)) {
			/* Read sensor ID for event */
			Local0 = ^PATI

			/* When sensor ID returns 0xFF then no more events */
			While (Local0 != EC_TEMP_SENSOR_NOT_PRESENT)
			{
#ifdef HAVE_THERM_EVENT_HANDLER
				\_SB.DPTF.TEVT (Local0)
#endif

				/* Keep reaading sensor ID for event */
				Local0 = ^PATI
			}

			Release (^PATM)
		}
	}

	/*
	 * Set Charger Current Limit
	 *   Arg0 = Current Limit in 64mA steps
	 */
	Method (CHGS, 1, Serialized)
	{
		^CHGL = ToInteger (Arg0)
	}

	/*
	 * Disable Charger Current Limit
	 */
	Method (CHGD, 0, Serialized)
	{
		^CHGL = 0xFF
	}

	/* Read current Tablet mode */
	Method (RCTM, 0, NotSerialized)
	{
		Return (^TBMD)
	}

#ifdef EC_ENABLE_MULTIPLE_DPTF_PROFILES
	/* Read current Device DPTF Profile Number */
	Method (RCDP, 0, NotSerialized)
	{
		/*
		 * DDPN = 0 is reserved for backwards compatibility.
		 * If DDPN == 0 use TBMD to load appropriate DPTF table.
		 */
		If (^DDPN == 0) {
			Return (^TBMD)
		} Else {
			Local0 = ^DDPN - 1
			Return (Local0)
		}
	}
#endif
#if CONFIG(EC_GOOGLE_CHROMEEC_ACPI_USB_PORT_POWER)
	/*
	 * Enable USB Port Power
	 *   Arg0 = USB port ID
	 */
	Method (UPPS, 1, Serialized)
	{
		USPP |= 1 << Arg0
	}

	/*
	 * Disable USB Port Power
	 *   Arg0 = USB port ID
	 */
	Method (UPPC, 1, Serialized)
	{
		And (USPP, Not (1 << Arg0), USPP)
	}
#endif

	#include "ac.asl"
	#include "battery.asl"
	#include "cros_ec.asl"

#ifdef EC_ENABLE_ALS_DEVICE
	#include "als.asl"
#endif

#ifdef EC_ENABLE_KEYBOARD_BACKLIGHT
	#include "keyboard_backlight.asl"
#endif

#ifdef EC_ENABLE_TBMC_DEVICE
	#include "tbmc.asl"
#endif
}
