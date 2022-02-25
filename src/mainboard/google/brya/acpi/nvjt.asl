/* SPDX-License-Identifier: GPL-2.0-or-later */

#define JT_FUNC_SUPPORT		0
#define JT_FUNC_CAPS		1
#define JT_FUNC_POWERCONTROL	2
#define JT_FUNC_PLATPOLICY	3

Method (NVJT, 2, Serialized)
{
	Switch (ToInteger(Arg0))
	{
		Case (JT_FUNC_SUPPORT)
		{
			Return (ITOB(
				(1 << JT_FUNC_SUPPORT) |
				(1 << JT_FUNC_CAPS) |
				(1 << JT_FUNC_POWERCONTROL) |
				(1 << JT_FUNC_PLATPOLICY)))
		}
		Case (JT_FUNC_CAPS)
		{
			Return (ITOB(
				(0 << 0) |		/* JTE: G-Sync NVSR Power Features Enabled */
				(1 << 0) |		/* NVSE: NVSR Disabled */
				(0 << 3) |		/* PPR: Panel Power Rail */
				(0 << 5) |		/* SRPR: Self-Refresh Controller Power Rail */
				(0 << 6) |		/* FBPR: FB Power Rail */
				(0 << 8) |		/* GPR: GPU Power Rail */
				(0 << 10) |		/* GCR: GC6 ROM */
				(1 << 11) |		/* PTH: No SMI Handler */
				(0 << 12) |		/* NOT: Supports Notify on GC6 State done */
				(1 << 13) |		/* MHYB: MS Hybrid Support (deferred GC6) */
				(0 << 14) |		/* RPC: Root Port Control */
				(0 << 15) |		/* GC6 Version (GC6-E) */
				(0 << 17) |		/* GEI: GC6 Exit ISR Support */
				(0 << 18) |		/* GSW: GC6 Self Wakeup */
				(0x200 << 20)))		/* MXRV: Highest Revision */
		}
		Case (JT_FUNC_POWERCONTROL)
		{
			CreateField (Arg1, 0, 3, GPC)	/* GPU Power Control */
			CreateField (Arg1, 4, 1, PPC)	/* Panel Power Control */
			CreateField (Arg1, 14, 2, DFGC)	/* Defer GC6 enter/exit */
			CreateField (Arg1, 16, 3, GPCX) /* Deferred GC6 exit */

			/* Deferred GC6 entry/exit is requested */
			If (ToInteger(GPC) != 0 || ToInteger(DFGC) != 0)
			{
				DFEN = ToInteger(DFGC)
				DFCI = ToInteger(GPC)
				DFCO = ToInteger(GPCX)
			}

			Local0 = Buffer (4) { 0x0 }
			CreateField (Local0, 0, 3, CGCS)	/* Current GC State */
			CreateField (Local0, 3, 1, CGPS)	/* Current GPU power status */
			CreateField (Local0, 7, 1, CPSS)	/* Current panel and SRC state */

			/* Leave early if deferred GC6 is requested */
			If (DFEN != 0)
			{
				CGCS = 1
				CGPS = 1
				Return (Local0)
			}

			Switch (ToInteger(GPC))
			{
				/* Get GCU GCx Sleep Status */
				Case (NVJT_GPC_GSS)
				{
					If (^_STA () != 0)
					{
						CGPS = 1
						CGCS = 1
					}
					Else
					{
						CGPS = 0
						CGCS = 3
					}
				}
				Case (NVJT_GPC_EGNS)
				{
					/* Enter GC6; no self-refresh */
					GC6I ()
					CPSS = 1
					CGCS = 0
				}
				Case (NVJT_GPC_EGIS)
				{
					/* Enter GC6; enable self-refresh */
					GC6I ()
					If (ToInteger (PPC) == 0)
					{
						CPSS = 0
					}
					CGCS = 0
				}
				Case (NVJT_GPS_XGXS)
				{
					/* Exit GC6; stop self-refresh */
					GC6O ()
					CGCS = 1
					CGPS = 1
					If (ToInteger (PPC) != 0)
					{
						CPSS = 0
					}
				}
				Case (NVJT_GPS_XGIS)
				{
					/* Exit GC6 for self-refresh */
					GC6O ()
					CGCS = 1
					CGPS = 1
					If (ToInteger (PPC) != 0)
					{
						CPSS = 0
					}
				}
			}

			Return (Local0)
		}
	}

	Return (NV_ERROR_UNSUPPORTED)
}
