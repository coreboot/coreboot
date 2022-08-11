/* SPDX-License-Identifier: GPL-2.0-or-later */


/* Voltage rail control signals */
#define GPIO_1V8_PWR_EN		GPP_E18
#define GPIO_1V8_PG		GPP_E20
#define GPIO_NV33_PWR_EN	GPP_A21
#define GPIO_NV33_PG		GPP_A22
#define GPIO_NVVDD_PWR_EN	GPP_E0
#define GPIO_NVVDD_PG		GPP_E16
#define GPIO_PEXVDD_PWR_EN	GPP_E10
#define GPIO_PEXVDD_PG		GPP_E17
#define GPIO_FBVDD_PWR_EN	GPP_A19
#define GPIO_FBVDD_PG		GPP_E4

#define GPIO_GPU_PERST_L	GPP_B3
#define GPIO_GPU_ALLRAILS_PG	GPP_E5
#define GPIO_GPU_NVVDD_EN	GPP_A17

#define GC6_DEFER_TYPE_EXIT_GC6		3

/* Optimus Power Control State */
Name (OPCS, OPTIMUS_POWER_CONTROL_DISABLE)

/* PCI configuration space Owner */
Name (PCIO, PCI_OWNER_DRIVER)

/* Saved PCI configuration space memory (VGA Buffer) */
Name (VGAB, Buffer (0xfb) { 0x00 })

/* Deferred GPU State */
Name (OPS0, OPTIMUS_CONTROL_NO_RUN_PS0)

/* GC6 Entry/Exit state */
Name (GC6E, GC6_STATE_EXITED)

/* Power State, GCOFF, GCON */
#define GPU_POWER_STATE_OFF	0
#define GPU_POWER_STATE_ON	1
Name (GPPS, GPU_POWER_STATE_ON)

/* Defer GC6 entry / exit until D3-cold request */
Name (DFEN, 0)
/* Deferred GC6 Enter control */
Name (DFCI, 0)
/* Deferred GC6 Exit control */
Name (DFCO, 0)

/* "GC6 In", i.e. GC6 Entry Sequence */
Method (GC6I, 0, Serialized)
{
	GC6E = GC6_STATE_TRANSITION

	/* Put PCIe link into L2/3 */
	\_SB.PCI0.PEG0.DL23 ()

	/* Wait for GPU to deassert its GPIO4, i.e. GPU_NVVDD_EN */
	GPPL (GPIO_GPU_NVVDD_EN, 0, 20)

	/* Deassert PG_GPU_ALLRAILS */
	\_SB.PCI0.CTXS (GPIO_GPU_ALLRAILS_PG)

	/* Ramp down PEXVDD */
	\_SB.PCI0.CTXS (GPIO_PEXVDD_PWR_EN)
	GPPL (GPIO_PEXVDD_PG, 0, 20)
	Sleep (10)

	/* Deassert EN_PPVAR_GPU_NVVDD */
	\_SB.PCI0.CTXS (GPIO_NVVDD_PWR_EN)
	GPPL (GPIO_NVVDD_PG, 0, 20)
	Sleep (2)

	/* Assert GPU_PERST_L */
	\_SB.PCI0.CTXS (GPIO_GPU_PERST_L)

	Printf ("dGPU entered GC6")
	GC6E = GC6_STATE_ENTERED
}

/* "GC6 Out", i.e. GC6 Exit Sequence  */
Method (GC6O, 0, Serialized)
{
	GC6E = GC6_STATE_TRANSITION

	/* Deassert GPU_PERST_L */
	\_SB.PCI0.STXS (GPIO_GPU_PERST_L)

	/* Wait for GPU to assert GPU_NVVDD_EN */
	GPPL (GPIO_GPU_NVVDD_EN, 1, 20)

	/* Ramp up NVVDD */
	\_SB.PCI0.STXS (GPIO_NVVDD_PWR_EN)
	GPPL (GPIO_NVVDD_PG, 1, 4)

	/* Ramp up PEXVDD */
	 \_SB.PCI0.STXS (GPIO_PEXVDD_PWR_EN)
	 GPPL (GPIO_PEXVDD_PG, 1, 4)

	/* Assert PG_GPU_ALLRAILS */
	\_SB.PCI0.STXS (GPIO_GPU_ALLRAILS_PG)

	/* Put PCIe link into L0 state */
	\_SB.PCI0.PEG0.LD23 ()

	Printf ("dGPU exited GC6")
	GC6E = GC6_STATE_EXITED
}

/* GCOFF exit sequence */
Method (PGON, 0, Serialized)
{
	If (GPPS == GPU_POWER_STATE_ON)
	{
		Printf ("PGON: GPU already on")
		Return
	}

	/* Assert PERST# */
	\_SB.PCI0.CTXS (GPIO_GPU_PERST_L)

	/* Ramp up 1.8V rail */
	\_SB.PCI0.STXS (GPIO_1V8_PWR_EN)
	GPPL (GPIO_1V8_PG, 1, 20)

	/* Ramp up NV33 rail */
	\_SB.PCI0.STXS (GPIO_NV33_PWR_EN)
	GPPL (GPIO_NV33_PG, 1, 20)

	/* Ramp up NVVDD rail */
	\_SB.PCI0.STXS (GPIO_NVVDD_PWR_EN)
	GPPL (GPIO_NVVDD_PG, 1, 5)

	/* Ramp up PEXVDD rail */
	\_SB.PCI0.STXS (GPIO_PEXVDD_PWR_EN)
	GPPL (GPIO_PEXVDD_PG, 1, 5)

	/* Ramp up FBVDD rail (active low) */
	\_SB.PCI0.CTXS (GPIO_FBVDD_PWR_EN)
	GPPL (GPIO_FBVDD_PG, 1, 5)

	/* All rails are good */
	\_SB.PCI0.STXS (GPIO_GPU_ALLRAILS_PG)
	Sleep (1)

	/* Deassert PERST# */
	\_SB.PCI0.STXS (GPIO_GPU_PERST_L)

	GC6E = GC6_STATE_EXITED
	GPPS = GPU_POWER_STATE_ON
	Printf ("GPU Sequenced on")
}

/* GCOFF entry sequence */
Method (PGOF, 0, Serialized)
{
	If (GPPS == GPU_POWER_STATE_OFF)
	{
		Printf ("GPU already off")
		Return
	}

	/* Assert PERST# */
	\_SB.PCI0.CTXS (GPIO_GPU_PERST_L)

	/* All rails are about to go down */
	\_SB.PCI0.CTXS (GPIO_GPU_ALLRAILS_PG)
	Sleep (1)

	/* Ramp down FBVDD (active-low) and let rail discharge to <10% */
	\_SB.PCI0.STXS (GPIO_FBVDD_PWR_EN)
	GPPL (GPIO_FBVDD_PG, 0, 20)

	/* Ramp down PEXVDD and let rail discharge to <10% */
	\_SB.PCI0.CTXS (GPIO_PEXVDD_PWR_EN)
	GPPL (GPIO_PEXVDD_PG, 0, 20)
	Sleep (10)

	/* Ramp down NVVDD and let rail discharge to <10% */
	\_SB.PCI0.CTXS (GPIO_NVVDD_PWR_EN)
	GPPL (GPIO_NVVDD_PG, 0, 20)
	Sleep (2)

	/* Ramp down NV33 and let rail discharge to <10% */
	\_SB.PCI0.CTXS (GPIO_NV33_PWR_EN)
	GPPL (GPIO_NV33_PG, 0, 20)
	Sleep (4)

	/* Ramp down 1.8V */
	\_SB.PCI0.CTXS (GPIO_1V8_PWR_EN)
	GPPL (GPIO_1V8_PG, 0, 20)

	GPPS = GPU_POWER_STATE_OFF
	Printf ("GPU sequenced off")
}

/* Handle deferred GC6 vs. poweron request */
Method (NPON, 0, Serialized)
{
	If (DFEN == GC6_DEFER_ENABLE) /* 1 */
	{
		If (DFCO == GC6_DEFER_TYPE_EXIT_GC6) /* 3 */
		{
			GC6O ()
		}

		DFEN = GC6_DEFER_DISABLE
	}
	Else
	{
		PGON ()
		\_SB.PCI0.PEG0.LD23 ()
	}
}

/* Handle deferred GC6 vs. poweroff request */
Method (NPOF, 0, Serialized)
{
	If (DFEN == GC6_DEFER_ENABLE)
	{
		/* Deferred GC6 entry */
		If (DFCI == NVJT_GPC_EGNS || DFCI == NVJT_GPC_EGIS)
		{
			GC6I ()
		}

		DFEN = GC6_DEFER_DISABLE
	}
	Else
	{
		\_SB.PCI0.PEG0.DL23 ()
		PGOF ()
	}
}

Method (_ON, 0, Serialized)
{
	PGON ()
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
		/* Restore PCI config space */
		If (PCIO == PCI_OWNER_SBIOS)
		{
			VGAR = VGAB
		}

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
		/* Save PCI config space to ACPI buffer */
		If (PCIO == PCI_OWNER_SBIOS)
		{
			VGAB = VGAR
		}

		/* Poweroff or deferred GC6 entry */
		NPOF ()

		/* Because _PS3 ran _OFF, _PS0 must run _ON */
		OPS0 = OPTIMUS_CONTROL_RUN_PS0

		/* OPCS is one-shot, so reset it */
		OPCS = OPTIMUS_POWER_CONTROL_DISABLE
	}
}

/*
 * Normally, _ON and _OFF of the power resources listed in _PRx will be
 * evaluated before entering D0/D3. However, for Optimus, the package
 * should refer to the PCIe controller itself, not a dependent device.
 */
Name (_PR0, Package() { \_SB.PCI0.PEG0 })
Name (_PR3, Package() { \_SB.PCI0.PEG0 })

Method (_STA, 0, Serialized)
{
	If (GC6E == GC6_STATE_EXITED &&
	    \_SB.PCI0.GTXS(GPIO_GPU_ALLRAILS_PG) == 1)
	{
		Return (0xF)
	}
	Else
	{
		Return (0)
	}
}
