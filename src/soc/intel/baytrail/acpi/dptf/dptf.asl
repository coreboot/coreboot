Device (DPTF)
{
	Name (_HID, EISAID ("INT3400"))
	Name (_UID, 0)

	Name (IDSP, Package()
	{
		/* DPPM Passive Policy 1.0 */
		ToUUID("42A441D6-AE6A-462B-A84B-4A8CE79027D3"),

		/*  DPPM Critical Policy */
		ToUUID("97C68AE7-15FA-499c-B8C9-5DA81D606E0A"),

		/* DPPM Cooling Policy */
		ToUUID("16CAF1B7-DD38-40ED-B1C1-1B8A1913D531"),
	})

	Method (_STA)
	{
		If (LEqual (\DPTE, One)) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	Method (_OSC, 4, Serialized)
	{
		/* TODO: Enable/Disable EC control of thermals/charging */
		Return (Arg3)
	}

	Method (_TRT)
	{
		Return (\_SB.DTRT)
	}

	/* Thermal Threshold Event Handler */
	Method (TEVT, 0, Serialized)
	{
	}

	/* Include CPU Participant */
	#include "cpu.asl"

	/* Include Thermal Participants */
	#include "thermal.asl"

	/* Include Charger Participant */
	#include "charger.asl"
}
