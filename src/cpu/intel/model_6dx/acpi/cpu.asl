/* These come from the dynamically created CPU SSDT */
External(PDC0)
External(PDC1)

// Power notification

External (\_PR_.CPU0, DeviceObj)
External (\_PR_.CPU1, DeviceObj)
External (\_PR_.CPU0._PPC)
External (\_PR_.CPU1._PPC)

Method (PNOT)
{
	If (MPEN) {
		If(And(PDC0, 0x08)) {
			Notify (\_PR_.CPU0, 0x80)	 // _PPC

			If (And(PDC0, 0x10)) {
				Sleep(100)
				Notify(\_PR_.CPU0, 0x81) // _CST
			}
		}

		If(And(PDC1, 0x08)) {
			Notify (\_PR_.CPU1, 0x80)	 // _PPC
			If (And(PDC1, 0x10)) {
				Sleep(100)
				Notify(\_PR_.CPU1, 0x81) // _CST
			}
		}

	} Else { // UP
		Notify (\_PR_.CPU0, 0x80)
		Sleep(0x64)
		Notify(\_PR_.CPU0, 0x81)
	}
}
