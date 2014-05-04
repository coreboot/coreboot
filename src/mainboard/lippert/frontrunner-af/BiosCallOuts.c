/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "agesawrapper.h"
#include "amdlib.h"
#include "BiosCallOuts.h"
#include "heapManager.h"
#include <northbridge/amd/agesa/family14/dimmSpd.h>

STATIC BIOS_CALLOUT_STRUCT BiosCallouts[] =
{
	{AGESA_ALLOCATE_BUFFER,
	 BiosAllocateBuffer
	},

	{AGESA_DEALLOCATE_BUFFER,
	 BiosDeallocateBuffer
	},

	{AGESA_DO_RESET,
	 BiosReset
	},

	{AGESA_LOCATE_BUFFER,
	 BiosLocateBuffer
	},

	{AGESA_READ_SPD,
	 BiosReadSpd
	},

	{AGESA_READ_SPD_RECOVERY,
	 BiosDefaultRet
	},

	{AGESA_RUNFUNC_ONAP,
	 BiosRunFuncOnAp
	},

	{AGESA_GNB_PCIE_SLOT_RESET,
	 BiosGnbPcieSlotReset
	},

	{AGESA_HOOKBEFORE_DRAM_INIT,
	 BiosHookBeforeDramInit
	},

	{AGESA_HOOKBEFORE_DRAM_INIT_RECOVERY,
	 BiosHookBeforeDramInitRecovery
	},

	{AGESA_HOOKBEFORE_DQS_TRAINING,
	 BiosHookBeforeDQSTraining
	},

	{AGESA_HOOKBEFORE_EXIT_SELF_REF,
	 BiosHookBeforeExitSelfRefresh
	},
};

AGESA_STATUS GetBiosCallout (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	UINTN i;
	AGESA_STATUS CalloutStatus;
	UINTN CallOutCount = sizeof (BiosCallouts) / sizeof (BiosCallouts [0]);

	/*
	 * printk(BIOS_SPEW,"%s function: %x\n", __func__, (u32) Func);
	 */

	CalloutStatus = AGESA_UNSUPPORTED;

	for (i = 0; i < CallOutCount; i++) {
		if (BiosCallouts[i].CalloutName == Func) {
			CalloutStatus = BiosCallouts[i].CalloutPtr (Func, Data, ConfigPtr);
			return CalloutStatus;
		}
	}

	return CalloutStatus;
}

/*	Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDQSTraining (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}
/*	Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInit (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	MEM_DATA_STRUCT *MemData = ConfigPtr;

	printk(BIOS_INFO, "Setting DDR3 voltage: ");
	FCH_IOMUX(184) = 2; // GPIO184: VMEM_LV_EN# lowers VMEM from 1.5 to 1.35V
	switch (MemData->ParameterListPtr->DDR3Voltage) {
	case VOLT1_25: // board is not able to provide this
		MemData->ParameterListPtr->DDR3Voltage = VOLT1_35; // sorry
		printk(BIOS_INFO, "can't provide 1.25 V, using ");
		// fall through
	default: // AGESA.h says in mixed case 1.5V DIMMs get excluded
	case VOLT1_35:
		FCH_GPIO(184) = 0x08; // = output, disable PU, set to 0
		printk(BIOS_INFO, "1.35 V\n");
		break;
	case VOLT1_5:
		FCH_GPIO(184) = 0xC8; // = output, disable PU, set to 1
		printk(BIOS_INFO, "1.5 V\n");
	}

	return AGESA_SUCCESS;
}

/*	Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeDramInitRecovery (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}

/*	Call the host environment interface to provide a user hook opportunity. */
AGESA_STATUS BiosHookBeforeExitSelfRefresh (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	return AGESA_SUCCESS;
}
/* PCIE slot reset control */
AGESA_STATUS BiosGnbPcieSlotReset (UINT32 Func, UINT32 Data, VOID *ConfigPtr)
{
	// Dedicated reset not needed for the on-board Intel I210 GbE controller.
	return AGESA_UNSUPPORTED;
}
