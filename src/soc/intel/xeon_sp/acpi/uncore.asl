/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


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

Scope (_SB)
{
	#include "pci_irq.asl"
	#include "uncore_irq.asl"
	#include "iiostack.asl"
}
