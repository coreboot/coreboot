/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

Scope(\)
{
	// Private Chipset Register(PCR). Memory Mapped through ILB
	OperationRegion(PCRR, SystemMemory, P2SB_BAR, 0x01000000)
	Field(PCRR, DWordAcc, Lock, Preserve)
	{
		Offset (0xD03100),  // Interrupt Routing Registers
		PRTA, 8,
		PRTB, 8,
		PRTC, 8,
		PRTD, 8,
		PRTE, 8,
		PRTF, 8,
		PRTG, 8,
		PRTH, 8,
	}
}

Scope (\_SB)
{
	#include "pci_irqs.asl"
	#include "pch_irq.asl"		/* TODO:  Move to PCH asl. */
	#include "uncore_irq.asl"
	#include "iiostack.asl"
}
