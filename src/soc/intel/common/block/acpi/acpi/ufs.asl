/* SPDX-License-Identifier: GPL-2.0-only */
#include <soc/iomap.h>
#include <soc/pcr_ids.h>
#include <soc/ufs.h>

#define TRUE 1
#define FALSE 0

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

		OperationRegion(PWMR, SystemMemory, PCH_PWRM_BASE_ADDRESS, PCH_PWRM_BASE_SIZE)
		Field(PWMR, DWordAcc, NoLock, Preserve)
		{
			Offset(R_PMC_PWRM_LTR_IGN),
			, 18,
			LTRU,  1,         /* Bit 18 - Ignore LTR from UFS X2 */
		}

		Method (ULTR, 1, Serialized) {
			LTRU = Arg0
		}

		Method (_PS0, 0, Serialized)
		{
			/* Disable PG */
			PGEN = 0

			/* Set BIT[1:0] = 00b - Power State D0 */
			PSTA &= 0xFFFFFFFC

#if CONFIG(SOC_INTEL_UFS_LTR_DISQUALIFY)
			/* Remove Disqualification of LTR from UFS IP */
			ULTR (FALSE)
#endif

#if CONFIG(SOC_INTEL_UFS_OCP_TIMER_DISABLE)
			/* Disable OCP Timer in SCS UFS IOSF Bridge */
			OCPD ()
#endif
		}

		Method (_PS3, 0, Serialized)
		{
#if CONFIG(SOC_INTEL_UFS_LTR_DISQUALIFY)
			/* Disqualify LTR from UFS IP */
			ULTR (TRUE)
#endif

			/* Enable PG */
			PGEN = 1
		}

		Method (_INI)
		{
#if CONFIG(SOC_INTEL_UFS_OCP_TIMER_DISABLE)
			OCPD ()
#endif
		}
	}
}
