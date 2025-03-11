/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci_ids.h>

External (\_SB.PCI0.SPCO, MethodObj)


/* Board specific defines */

/* PCI Subsystem ID to restore after wake */
#define GPU_SSID 0xa7611558

/* Power control signals */
#define GPIO_GPU_PERST_L	GPP_B09
#define GPIO_GPU_PWR_EN		GPP_D03
#define GPIO_GPU_PWR_GD		GPP_C15
#define GPIO_GPU_NVVDD_EN	GPP_F16

/* GPU clock pin */
#define GPU_SRCCLK_PIN		0x6

#define GC6_DEFER_TYPE_EXIT_GC6	3

/* 250ms in "Timer" units (i.e. 100ns increments) */
#define MIN_OFF_TIME_TIMERS	2500000

#define SRCCLK_DISABLE		0
#define SRCCLK_ENABLE		1

#define GPU_POWER_STATE_OFF	0
#define GPU_POWER_STATE_ON	1

/* Optimus Power Control State */
Name (OPCS, OPTIMUS_POWER_CONTROL_DISABLE)

/* PCI configuration space Owner */
Name (PCIO, PCI_OWNER_SBIOS)

/* Deferred GPU State */
Name (OPS0, OPTIMUS_CONTROL_NO_RUN_PS0)

/* GC6 Entry/Exit state */
Name (GC6E, GC6_STATE_EXITED)

/* Power State, GCOFF, GCON */
Name (GPPS, GPU_POWER_STATE_ON)

/* Defer GC6 entry / exit until D3-cold request */
Name (DFEN, 0)
/* Deferred GC6 Enter control */
Name (DFCI, 0)
/* Deferred GC6 Exit control */
Name (DFCO, 0)
/* GCOFF Timer */
Name (GCOT, 0)

/* Copy of LTR enable bit from PEG port */
Name (SLTR, 0)

/* Control the PCIe SRCCLK# for dGPU */
Method (SRCC, 1, Serialized)
{
	/* Control clock via P2SB interface */
	\_SB.PCI0.SPCO (GPU_SRCCLK_PIN, Arg0)
}

/* "GC6 In", i.e. GC6 Entry Sequence */
Method (GC6I, 0, Serialized)
{
	GC6E = GC6_STATE_TRANSITION

	/* Save the PEG port's LTR setting */
	SLTR = LREN

	/* Put PCIe link into L2/3 */
	\_SB.PCI0.RP12.DL23 ()

	/* Wait for GPU to deassert its GPIO4, i.e. GPU_NVVDD_EN */
	GPPL (GPIO_GPU_NVVDD_EN, 0, 20)

	/* Assert GPU_PERST_L */
	CTXS (GPIO_GPU_PERST_L)

	/* Disable PCIe SRCCLK# */
	SRCC (SRCCLK_DISABLE)

	GC6E = GC6_STATE_ENTERED
}

/* "GC6 Out", i.e. GC6 Exit Sequence */
Method (GC6O, 0, Serialized)
{
	GC6E = GC6_STATE_TRANSITION

	/* Re-enable PCIe SRCCLK# */
	SRCC (SRCCLK_ENABLE)

	/* Deassert GPU_PERST_L */
	STXS (GPIO_GPU_PERST_L)

	/* Wait for GPU to assert GPU_NVVDD_EN */
	GPPL (GPIO_GPU_NVVDD_EN, 1, 20)

	/* Restore PCIe link back to L0 state */
	\_SB.PCI0.RP12.LD23 ()

	/* Wait for dGPU to reappear on the bus */
	Local0 = 50
	While (NVID != PCI_VID_NVIDIA)
	{
		Stall (100)
		Local0--
		If (Local0 == 0)
		{
			Break
		}
	}

	/* Restore the PEG LTR enable bit */
	LREN = SLTR

	/* Clear recoverable errors detected bit */
	CEDR = 1

	GC6E = GC6_STATE_EXITED
}

/* GCOFF exit sequence */
Method (PGON, 0, Serialized)
{
	Local0 = Timer - GCOT
	If (Local0 < MIN_OFF_TIME_TIMERS)
	{
		Local1 = (MIN_OFF_TIME_TIMERS - Local0) / 10000
		Printf("Sleeping %o to ensure min GCOFF time", Local1)
		Sleep (Local1)
	}

	/* Assert PERST# */
	CTXS (GPIO_GPU_PERST_L)

	/* Enable power rails */
	STXS (GPIO_GPU_PWR_EN)

	/* Wait 200ms for PWRGD */
	GPPL (GPIO_GPU_PWR_GD, 1, 200)

	/* Deassert PERST# */
	STXS (GPIO_GPU_PERST_L)

	GC6E = GC6_STATE_EXITED
	GPPS = GPU_POWER_STATE_ON
}

/* GCOFF entry sequence */
Method (PGOF, 0, Serialized)
{
	/* Assert PERST# */
	CTXS (GPIO_GPU_PERST_L)

	/* Disable power rails */
	CTXS (GPIO_GPU_PWR_EN)

	GCOT = Timer

	GPPS = GPU_POWER_STATE_OFF
}

/* GCOFF Out, i.e. full power-on sequence */
Method (GCOO, 0, Serialized)
{
	If (GPPS == GPU_POWER_STATE_ON)
	{
		Printf ("PGON: GPU already on")
		Return
	}

	SRCC (SRCCLK_ENABLE)
	PGON ()
	\_SB.PCI0.RP12.LD23 ()

	/* Wait for dGPU to reappear on the bus */
	Local0 = 50
	While (NVID != PCI_VID_NVIDIA)
	{
		Stall (100)
		Local0--
		If (Local0 == 0)
		{
			Break
		}
	}

	/* Restore the PEG LTR enable bit */
	LREN = SLTR

	/* Clear recoverable errors detected bit */
	CEDR = 1

	/* Restore the PEG LTR enable bit */
	LREN = SLTR

	/* Clear recoverable errors detected bit */
	CEDR = 1
}

/* GCOFF In, i.e. full power-off sequence */
Method (GCOI, 0, Serialized)
{
	If (GPPS == GPU_POWER_STATE_OFF)
	{
		Printf ("GPU already off")
		Return
	}

	/* Save the PEG port's LTR setting */
	SLTR = LREN
	\_SB.PCI0.RP12.DL23 ()
	PGOF ()
	SRCC (SRCCLK_DISABLE)
}

/* Handle deferred GC6 vs. poweron request */
Method (NPON, 0, Serialized)
{
	If (DFEN == GC6_DEFER_ENABLE)
	{
		If (DFCO == GC6_DEFER_TYPE_EXIT_GC6)
		{
			GC6O ()
		}

		DFEN = GC6_DEFER_DISABLE
	}
	Else
	{
		GCOO ()
	}
	NVSS = GPU_SSID
}

/* Handle deferred GC6 vs. poweroff request */
Method (NPOF, 0, Serialized)
{
	/* Don't touch the `DFEN` flag until the GC6 exit. */
	If (DFEN == GC6_DEFER_ENABLE)
	{
		/* Deferred GC6 entry */
		If (DFCI == NVJT_GPC_EGNS || DFCI == NVJT_GPC_EGIS)
		{
			GC6I ()
		}
	}
	Else
	{
		GCOI ()
	}
}

Method (_ON, 0, Serialized)
{
	PGON ()
	NVSS = GPU_SSID
}

Method (_OFF, 0, Serialized)
{
	PGOF ()
}

/* Put device into D0 */
Method (_PS0, 0, NotSerialized)
{
	If (OPS0 == OPTIMUS_CONTROL_RUN_PS0)
	{
		/* Poweron or deferred GC6 exit */
		NPON ()

		OPS0 = OPTIMUS_CONTROL_NO_RUN_PS0
	}
}

/* Put device into D3 */
Method (_PS3, 0,  NotSerialized)
{
	If (OPCS == OPTIMUS_POWER_CONTROL_ENABLE)
	{
		/* Poweroff or deferred GC6 entry */
		NPOF ()

		/* Because _PS3 ran NPOF, _PS0 must run NPON */
		OPS0 = OPTIMUS_CONTROL_RUN_PS0

		/* OPCS is one-shot, so reset it */
		OPCS = OPTIMUS_POWER_CONTROL_DISABLE
	}
}

Method (PSTA, 0, Serialized)
{
	If (GC6E == GC6_STATE_EXITED &&
	    \_SB.PCI0.GTXS(GPIO_GPU_PWR_GD) == 1)
	{
		Return (1)
	}
	Else
	{
		Return (0)
	}
}

Method (_STA, 0, Serialized)
{
	Return (0xF)
}
