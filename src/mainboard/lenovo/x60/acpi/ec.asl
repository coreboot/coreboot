#include <ec/lenovo/h8/acpi/ec.asl>

Scope(\_SB.PCI0.LPCB.EC)
{
	/* Volume down */
	Method(_Q1C, 0, NotSerialized)
	{
		Trap(SMI_SAVE_CMOS)
	}

	/* Volume up */
	Method(_Q1D, 0, NotSerialized)
	{
		Trap(SMI_SAVE_CMOS)
	}

	/* Mute key pressed */
	Method(_Q1E, 0, NotSerialized)
	{
		Trap(SMI_SAVE_CMOS)
	}


}
