/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/acpi_asl.h>

/*
 * Refer to Intel® C620 Series Chipset Platform Controller Hub EDS section 20.11
 * CONFIG_PCR_BASE_ADDRESS 0xfd000000 0x3100
 * (0xfd000000 | ((uint8_t)(0xC4) << 16) | (uint16_t)(0x3100) = 0xFDC43100
 *
 * PIRQ routing control is in PCR ITSS region.
 */

OperationRegion (ITSS, SystemMemory, PCR_ITSS_PIRQA_ROUT +
	CONFIG_PCR_BASE_ADDRESS + (PID_ITSS << PCR_PORTID_SHIFT), 8)
Field (ITSS, ByteAcc, NoLock, Preserve)
{
	PIRA, 8,  /* PIRQA Routing Control */
	PIRB, 8,  /* PIRQB Routing Control */
	PIRC, 8,  /* PIRQC Routing Control */
	PIRD, 8,  /* PIRQD Routing Control */
	PIRE, 8,  /* PIRQE Routing Control */
	PIRF, 8,  /* PIRQF Routing Control */
	PIRG, 8,  /* PIRQG Routing Control */
	PIRH, 8,  /* PIRQH Routing Control */
}

Name (IREN, 0x80)  /* Interrupt Routing Enable */
Name (IREM, 0x0f)  /* Interrupt Routing Mask */

Name (PRSA, ResourceTemplate ()
{
	IRQ (Level, ActiveLow, Shared, )
		{3,4,5,6,7,10,11,12,14,15}
})
Alias (PRSA, PRSB)
Name (PRSC, ResourceTemplate ()
{
	IRQ (Level, ActiveLow, Shared, )
		{3,4,5,6,10,11,12,14,15}
})
Alias (PRSC, PRSD)
Alias (PRSA, PRSE)
Alias (PRSA, PRSF)
Alias (PRSA, PRSG)
Alias (PRSA, PRSH)

MAKE_LINK_DEV(A,1)
MAKE_LINK_DEV(B,2)
MAKE_LINK_DEV(C,3)
MAKE_LINK_DEV(D,4)
MAKE_LINK_DEV(E,5)
MAKE_LINK_DEV(F,6)
MAKE_LINK_DEV(G,7)
MAKE_LINK_DEV(H,8)
