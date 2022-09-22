/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pcr_ids.h>

#define R_SCS_CFG_PCS               0x84
#define R_SCS_CFG_PG_CONFIG         0xA2

#define PID_UFSX2                   0x50

#define R_SCS_PCR_1C20              0x1C20
#define R_SCS_PCR_4820              0x4820
#define R_SCS_PCR_4020              0x4020
#define R_SCS_PCR_5820              0x5820
#define R_SCS_PCR_5C20              0x5C20
#define R_SCS_PCR_1078              0x1078
#define R_PMC_PWRM_LTR_IGN          0x1B0C

External(PCRA, MethodObj)

Scope (\_SB.PCI0)
{
	Device (UFS)
	{
		Name (_ADR, 0x0000000000120007) //  _ADR: Address
		Name (_DDN, "UFS Controller")  // _DDN: DOS Device Name
		Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301") /* Device Properties for _DSD */,
			Package (0x01)
			{
				Package (0x02)
				{
					"ref-clk-freq",
					CONFIG_SOC_INTEL_UFS_CLK_FREQ_HZ
				}
			}
		})

		Method (OCPD, 0, Serialized)
		{

			/*
			*  OCP Timer need to be disabled in SCS UFS IOSF Bridge to work around
			*  the Silicon Issue due to which LTR mechanism doest work Registers
			*  from the UFS OCP Fabric Register space that need to be programmed
			*  for the timeout are
			*  Upstream Initiator Port  -- offset 0x4020
			*  Downstream Target Port  -- offset 0x4820
			*  Downstream Target port at Controller -- offset 0x5c20
			*  Upstream Initiator port at Controller -- offset 0x5820
			*  Control over interconnect-wide functions -- offset 0x1078
			*/
				PCRA (PID_UFSX2, R_SCS_PCR_4020, 0x0)
				PCRA (PID_UFSX2, R_SCS_PCR_4820, 0x0)
				PCRA (PID_UFSX2, R_SCS_PCR_5C20, 0x0)
				PCRA (PID_UFSX2, R_SCS_PCR_5820, 0x0)
				PCRA (PID_UFSX2, R_SCS_PCR_1078, 0x0)
		}

		/* Memory Region to access to the UFS PCI Configuration Space */

		OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
		Field (SCSR, ByteAcc, NoLock, Preserve)
		{
			Offset(R_SCS_CFG_PCS),	/*	0x84, PMCSR - Power Management Control and Status*/
			PSTA,32,
			Offset(R_SCS_CFG_PG_CONFIG),/* 0xA2, Device PG config */
			, 2,
			PGEN, 1	/* [BIT2] PGE - PG Enable */
		}

		Method (_PS0, 0, Serialized)
		{
			Store(0, PGEN) /* Disable PG */

			And(PSTA, 0xFFFFFFFC, PSTA) /* Set BIT[1:0] = 00b - Power State D0 */

			/* Disable OCP Timer in SCS UFS IOSF Bridge */
			OCPD ()
		}

		Method (_PS3, 0, Serialized)
		{

			Store(1, PGEN)	/* Enable PG */
		}
		Method (_INI)
		{
			OCPD ()
		}
	}
}
