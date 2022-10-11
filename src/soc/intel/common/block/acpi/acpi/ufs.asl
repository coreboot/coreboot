/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/pcr_ids.h>
#include <soc/ufs.h>

Scope (\_SB.PCI0)
{
	Device (UFS)
	{
		/* _ADR: Address */
		Name (_ADR, UFS_ACPI_DEVICE)
		/* _DDN: DOS Device Name */
		Name (_DDN, "UFS Controller")
		/* _DSD: Device-Specific Data */
		Name (_DSD, Package (0x02)
		{
			/* Device Properties for _DSD */
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package (0x01)
			{
				Package (0x02)
				{
					"ref-clk-freq",
					CONFIG_SOC_INTEL_UFS_CLK_FREQ_HZ
				}
			}
		})

		/*
		 * OCP Timer need to be disabled in SCS UFS IOSF Bridge to work around
		 * the Silicon Issue due to which LTR mechanism doesn't work.
		 * Registers from the UFS OCP Fabric register space that need to be
		 * programmed for the timeout are:
		 * Upstream Initiator Port  -- offset 0x4020
		 * Downstream Target Port  -- offset 0x4820
		 * Downstream Target port at Controller -- offset 0x5c20
		 * Upstream Initiator port at Controller -- offset 0x5820
		 * Control over interconnect-wide functions -- offset 0x1078
		 */
		Method (OCPD, 0, Serialized)
		{
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
			/*	0x84, PMCSR - Power Management Control and Status*/
			Offset(R_SCS_CFG_PCS),
			PSTA,32,
			/* 0xA2, Device PG config */
			Offset(R_SCS_CFG_PG_CONFIG),
			, 2,
			/* [BIT2] PGE - PG Enable */
			PGEN, 1
		}

		Method (_PS0, 0, Serialized)
		{
			/* Disable PG */
			PGEN = 0

			/* Set BIT[1:0] = 00b - Power State D0 */
			PSTA &= 0xFFFFFFFC

			/* Disable OCP Timer in SCS UFS IOSF Bridge */
			OCPD ()
		}

		Method (_PS3, 0, Serialized)
		{
			/* Enable PG */
			PGEN = 1
		}

		Method (_INI)
		{
			OCPD ()
		}
	}
}
