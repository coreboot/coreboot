/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio_defs.h>
#include <soc/intel/common/acpi/gpio.asl>
#include <soc/intel/common/block/acpi/acpi/gpio_op.asl>
#include <soc/irq.h>
#include <soc/pcr_ids.h>

#define GPIO_SW_REVISION 0x00010000

/*
 * Get GPIO DW0 Address
 * Arg0 - GPIO Number
 */
Method (GADD, 1, NotSerialized)
{
	/* GPIO Community 0 */
	If (Arg0 >= COM0_GRP_PAD_START && Arg0 <= COM0_GRP_PAD_END)
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - COM0_GRP_PAD_START
	}
	/* GPIO Community 1 */
	If (Arg0 >= COM1_GRP_PAD_START && Arg0 <= COM1_GRP_PAD_END)
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - COM1_GRP_PAD_START
	}
	/* GPIO Community 3 */
	If (Arg0 >= COM3_GRP_PAD_START && Arg0 <= COM3_GRP_PAD_END)
	{
		Local0 = PID_GPIOCOM3
		Local1 = Arg0 - COM3_GRP_PAD_START
	}
	/* GPIO Community 4 */
	If (Arg0 >= COM4_GRP_PAD_START && Arg0 <= COM4_GRP_PAD_END)
	{
		Local0 = PID_GPIOCOM4
		Local1 = Arg0 - COM4_GRP_PAD_START
	}
	/* GPIO Community 5*/
	If (Arg0 >= COM5_GRP_PAD_START && Arg0 <= COM5_GRP_PAD_END)
	{
		Local0 = PID_GPIOCOM5
		Local1 = Arg0 - COM5_GRP_PAD_START
	}

	Local2 = PCRB(Local0) + PAD_CFG_BASE + (Local1 * 16)
	Return (Local2)
}

/*
 * Return PCR Port ID of GPIO Communities
 *
 * Arg0: GPIO Community (0-5)
 */
Method (GPID, 1, Serialized)
{
	Switch (ToInteger (Arg0))
	{
		Case (COMM_0) {
			Local0 = PID_GPIOCOM0
		}
		Case (COMM_1) {
			Local0 = PID_GPIOCOM1
		}
		Case (COMM_3) {
			Local0 = PID_GPIOCOM3
		}
		Case (COMM_4) {
			Local0 = PID_GPIOCOM4
		}
		Case (COMM_5) {
			Local0 = PID_GPIOCOM5
		}
		Default {
			Return (0)
		}
	}

	Return (Local0)
}

/* GPIO Power Management bits */
Name(GPMB, Package(TOTAL_GPIO_COMM) {0, 0, 0, 0, 0})

/*
 * Save GPIO Power Management bits
 */
Method (SGPM, 0, Serialized)
{
	For (Local0 = 0, Local0 < TOTAL_GPIO_COMM, Local0++)
	{
		Local1 = GPID (Local0)
		GPMB[Local0] = PCRR (Local1, GPIO_MISCCFG)
	}
}

/*
 * Restore GPIO Power Management bits
 */
Method (RGPM, 0, Serialized)
{
	For (Local0 = 0, Local0 < TOTAL_GPIO_COMM, Local0++)
	{
		CGPM (Local0, DerefOf(GPMB[Local0]))
	}
}

/*
 * Save current setting of GPIO Power Management bits and
 * enable all Power Management bits for all communities
 */
Method (EGPM, 0, Serialized)
{
	/* Save current setting and will restore it when resuming */
	SGPM ()
	/* Enable PM bits */
	For (Local0 = 0, Local0 < TOTAL_GPIO_COMM, Local0++)
	{
		CGPM (Local0, MISCCFG_GPIO_PM_CONFIG_BITS)
	}
}

/*
 * GPIO _CRS
 * Returns Interrupt and memory resources required for GPIO controller
 *
 * Arg0 - GPIO Port ID Offset for given community
 */
Method (GCRS, 0x1, Serialized) {
	Name (RBFL,ResourceTemplate() {
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ) {
			GPIO_IRQ14
		}
		QWordMemory (
			ResourceConsumer, ,
			MinFixed,
			MaxFixed,
			NonCacheable,
			ReadWrite,
			0x0,                /* AddressGranularity */
			0x0000000000000000, /* AddressMinimum _MIN */
			0x000000000000FFFF, /* AddressMaximum _MAX */
			0x0,                /* AddressTranslation */
			GPIO_BASE_SIZE,     /* RangeLength _LEN */
			, ,
			RBL0,
			AddressRangeMemory,
		)
	})
	CreateQWordField (RBFL, RBL0._MIN, CML0)
	CreateQWordField (RBFL, RBL0._MAX, CMX0)
	CML0 = PCRB (Arg0)
	CMX0 = CML0 + GPIO_BASE_SIZE - 1
	Return (RBFL)
}

/* GPIO Community 0: GPP_V, GPP_C */
Device (GPI0)
{
	Name (_HID, ACPI_GPIO_HID)
	Name (_CID, ACPI_GPIO_CID)
	Name (_UID, GPP_COMM0_ID)
	Name (_DDN, "GPIO Controller 0")

	Method (_CRS, 0, NotSerialized)
	{
		Return (^^GCRS (PID_GPIOCOM0))
	}

	Name (_DSD, Package (0x04)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x08)
		{
			Package (0x02)
			{
				"intc-gpio-sw-revision",
				GPIO_SW_REVISION
			},

			Package (0x02)
			{
				"intc-gpio-community-name",
				GPP_COMM0_NAME
			},

			Package (0x02)
			{
				"intc-gpio-group-count",
				NUM_COM0_GROUPS
			},

			Package (0x02)
			{
				"intc-gpio-pad-ownership-offset",
				PAD_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-pad-configuration-lock-offset",
				PAD_CFG_LOCK_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-host-software-pad-ownership-offset",
				HOSTSW_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-status-offset",
				GPI_INT_STS_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-enable-offset",
				GPI_INT_EN_0
			}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package (0x02)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				GPPV
			},

			Package (0x02)
			{
				"intc-gpio-group-1-subproperties",
				GPPC
			}
		}
	})

	/* first bank/group in community 0: GPP_V */
	Name (GPPV, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_V_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_V_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_V_START_OFFSET
			}
		}
	})

	/* 2nd bank/group in community 0: GPP_C */
	Name (GPPC, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_C_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_C_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_C_START_OFFSET
			}
		}
	})

	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}
}

/* GPIO Community 1: GPP_F, GPP_E */
Device (GPI1)
{
	Name (_HID, ACPI_GPIO_HID)
	Name (_CID, ACPI_GPIO_CID)
	Name (_UID, GPP_COMM1_ID)
	Name (_DDN, "GPIO Controller 1")

	Method (_CRS, 0, NotSerialized)
	{
		Return (^^GCRS (PID_GPIOCOM1))
	}

	Name (_DSD, Package (0x04)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x08)
		{
			Package (0x02)
			{
				"intc-gpio-sw-revision",
				GPIO_SW_REVISION
			},

			Package (0x02)
			{
				"intc-gpio-community-name",
				GPP_COMM1_NAME
			},

			Package (0x02)
			{
				"intc-gpio-group-count",
				NUM_COM1_GROUPS
			},

			Package (0x02)
			{
				"intc-gpio-pad-ownership-offset",
				PAD_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-pad-configuration-lock-offset",
				PAD_CFG_LOCK_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-host-software-pad-ownership-offset",
				HOSTSW_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-status-offset",
				GPI_INT_STS_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-enable-offset",
				GPI_INT_EN_0
			}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package (0x02)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				GPPF
			},

			Package (0x02)
			{
				"intc-gpio-group-1-subproperties",
				GPPE
			},

		}
	})
	/* first bank/group in community 1: GPP_F */
	Name (GPPF, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_F_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_F_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_F_START_OFFSET
			}
		}
	})
	/* 2nd bank/group in community 1: GPP_E */
	Name (GPPE, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_E_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_E_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_E_START_OFFSET
			}
		}
	})
	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}
}

/* GPIO Community 3: CPUJTAG (reserved), GPP_H, GPP_A, VGPIO3 */
Device (GPI3)
{
	Name (_HID, ACPI_GPIO_HID)
	Name (_CID, ACPI_GPIO_CID)
	Name (_UID, GPP_COMM3_ID)
	Name (_DDN, "GPIO Controller 2")

	Method (_CRS, 0, NotSerialized)
	{
		Return (^^GCRS (PID_GPIOCOM3))
	}

	Name (_DSD, Package (0x04)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x08)
		{
			Package (0x02)
			{
				"intc-gpio-sw-revision",
				GPIO_SW_REVISION
			},

			Package (0x02)
			{
				"intc-gpio-community-name",
				GPP_COMM3_NAME
			},

			Package (0x02)
			{
				"intc-gpio-group-count",
				NUM_COM3_GROUPS
			},

			Package (0x02)
			{
				"intc-gpio-pad-ownership-offset",
				PAD_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-pad-configuration-lock-offset",
				PAD_CFG_LOCK_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-host-software-pad-ownership-offset",
				HOSTSW_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-status-offset",
				GPI_INT_STS_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-enable-offset",
				GPI_INT_EN_0
			}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
#if CONFIG(SOC_INTEL_WILDCATLAKE)
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				GPPH
			},

			Package (0x02)
			{
				"intc-gpio-group-1-subproperties",
				GPPA
			},

			Package (0x02)
			{
				"intc-gpio-group-2-subproperties",
				VGP3
			}
		}
#else
		Package (0x04)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				RSVD
			},

			Package (0x02)
			{
				"intc-gpio-group-1-subproperties",
				GPPH
			},

			Package (0x02)
			{
				"intc-gpio-group-2-subproperties",
				GPPA
			},

			Package (0x02)
			{
				"intc-gpio-group-3-subproperties",
				VGP3
			}
		}
#endif
	})
#if CONFIG(SOC_INTEL_PANTHERLAKE)
	/* first bank/group in community 3: RSVD */
	Name (RSVD, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				"RSVD"
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_RSVD_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_RSVD_START_OFFSET
			}
		}
	})
#endif
	/* 2nd bank/group in community 3: GPP_H */
	Name (GPPH, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_H_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_H_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
	 			GPP_H_START_OFFSET
			}
		}
	})

	/* 3rd bank/group in community 3: GPP_A */
	Name (GPPA, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_A_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_A_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
	 			GPP_A_START_OFFSET
			}
		}
	})

	/* 4th bank/group in community 3: VGPIO3 */
	Name (VGP3, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_VGPIO3_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_VGPIO3_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_VGPIO3_START_OFFSET
			}
		}
	})

	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}
}

/* GPIO Community 4: GPP_S */
Device (GPI4)
{
	Name (_HID, ACPI_GPIO_HID)
	Name (_CID, ACPI_GPIO_CID)
	Name (_UID, GPP_COMM4_ID)
	Name (_DDN, "GPIO Controller 3")

	Method (_CRS, 0, NotSerialized)
	{
		Return (^^GCRS (PID_GPIOCOM4))
	}

	Name (_DSD, Package (0x04)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x08)
		{
			Package (0x02)
			{
				"intc-gpio-sw-revision",
				GPIO_SW_REVISION
			},

			Package (0x02)
			{
				"intc-gpio-community-name",
				GPP_COMM4_NAME
			},

			Package (0x02)
			{
				"intc-gpio-group-count",
				NUM_COM4_GROUPS
			},

			Package (0x02)
			{
				"intc-gpio-pad-ownership-offset",
				PAD_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-pad-configuration-lock-offset",
				PAD_CFG_LOCK_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-host-software-pad-ownership-offset",
				HOSTSW_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-status-offset",
				GPI_INT_STS_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-enable-offset",
				GPI_INT_EN_0
			}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
#if CONFIG(SOC_INTEL_WILDCATLAKE)
		Package (0x02)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				GPPS
			},

			Package (0x02)
			{
				"intc-gpio-group-1-subproperties",
				RSVD
			}
		}
#else
		Package (0x01)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				GPPS
			}
		}
#endif
	})
	/* only bank/group in community 4: GPP_S */
	Name (GPPS, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_S_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_S_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_S_START_OFFSET
			}
		}
	})
#if CONFIG(SOC_INTEL_WILDCATLAKE)
	/* second bank/group in community 4: RSVD */
	Name (RSVD, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				"RSVD"
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_RSVD_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_RSVD_START_OFFSET
			}
		}
	})
#endif
	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}
}

/* GPIO Community 5: GPP_B, GPP_D, VGPIO */
Device (GPI5)
{
	Name (_HID, ACPI_GPIO_HID)
	Name (_CID, ACPI_GPIO_CID)
	Name (_UID, GPP_COMM5_ID)
	Name (_DDN, "GPIO Controller 4")

	Method (_CRS, 0, NotSerialized)
	{
		Return (^^GCRS (PID_GPIOCOM5))
	}

	Name (_DSD, Package (0x04)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x08)
		{
			Package (0x02)
			{
				"intc-gpio-sw-revision",
				GPIO_SW_REVISION
			},

			Package (0x02)
			{
				"intc-gpio-community-name",
				GPP_COMM5_NAME
			},

			Package (0x02)
			{
				"intc-gpio-group-count",
				NUM_COM5_GROUPS
			},

			Package (0x02)
			{
				"intc-gpio-pad-ownership-offset",
				// 0xD0
				PAD_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-pad-configuration-lock-offset",
				PAD_CFG_LOCK_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-host-software-pad-ownership-offset",
				HOSTSW_OWN_REG_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-status-offset",
				GPI_INT_STS_0
			},

			Package (0x02)
			{
				"intc-gpio-gpi-interrupt-enable-offset",
				GPI_INT_EN_0
			}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-0-subproperties",
				GPPB
			},

			Package (0x02)
			{
				"intc-gpio-group-1-subproperties",
				GPPD
			},

			Package (0x02)
			{
				"intc-gpio-group-2-subproperties",
				VGP0
			}
		}
	})
	/* first bank/group in community 5: GPP_B */
	Name (GPPB, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_B_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_B_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_B_START_OFFSET
			}
		}
	})
	/* 2nd bank/group in community 5: GPP_D */
	Name (GPPD, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_D_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_D_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_D_START_OFFSET
			}
		}
	})
	/* 3rd bank/group in community 5: VGPIO */
	Name (VGP0, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x03)
		{
			Package (0x02)
			{
				"intc-gpio-group-name",
				GPP_VGPIO_NAME
			},

			Package (0x02)
			{
				"intc-gpio-pad-count",
				NUM_GRP_VGPIO_PADS
			},

			Package (0x02)
			{
				"intc-gpio-group-offset",
				GPP_VGPIO_START_OFFSET
			}
		}
	})

	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}
}
