/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/cnvi.h>
#include <soc/cnvi.h>
#include <soc/pcr_ids.h>

static const char *cnvi_wifi_acpi_name(const struct device *dev)
{
	return "CNVW";
}

static void cnvw_fill_ssdt(const struct device *dev)
{
	const char *scope = acpi_device_path(dev);

	acpi_device_write_pci_dev(dev);

	acpigen_write_scope(scope);
/*
 *	OperationRegion(CWAR, SystemMemory, \_SB_.PCI0.BASE(_ADR), 0x100)
 *	Field(CWAR, WordAcc, NoLock, Preserve) {
 *		VDID, 32,	// 0x00, VID DID
 *		Offset(CNVI_DEV_CAP),
 *		    , 28,
 *		WFLR,  1,	// Function Level Reset Capable
 *		Offset(CNVI_DEV_CONTROL),
 *		    , 15,
 *		WIFR,  1,	// Init Function Level Reset
 *		Offset(CNVI_POWER_STATUS),
 *		WPMS, 32,
 *	}
 */

	/* RegionOffset stored in Local0 */
	/* Local0 = \_SB_.PCI0.BASE(_ADR) */
	acpigen_write_store();
	acpigen_emit_namestring("\\_SB_.PCI0.BASE");
	acpigen_emit_namestring("_ADR");
	acpigen_emit_byte(LOCAL0_OP);

	/* OperationRegion */
	acpigen_emit_ext_op(OPREGION_OP);
	/* NameString 4 chars only */
	acpigen_emit_namestring("CWAR");
	/* RegionSpace */
	acpigen_emit_byte(SYSTEMMEMORY);
	/* RegionOffset */
	acpigen_emit_byte(LOCAL0_OP);
	/* RegionLen */
	acpigen_write_integer(0x100);

	struct fieldlist fields[] = {
		FIELDLIST_OFFSET(0),
		FIELDLIST_NAMESTR("VDID", 32),
		FIELDLIST_OFFSET(CNVI_DEV_CAP),
		FIELDLIST_RESERVED(28),
		FIELDLIST_NAMESTR("WFLR", 1),
		FIELDLIST_OFFSET(CNVI_DEV_CONTROL),
		FIELDLIST_RESERVED(15),
		FIELDLIST_NAMESTR("WIFR", 1),
		FIELDLIST_OFFSET(CNVI_POWER_STATUS),
		FIELDLIST_NAMESTR("WPMS", 32),
	};
	acpigen_write_field("CWAR", fields, ARRAY_SIZE(fields),
		FIELD_WORDACC | FIELD_NOLOCK | FIELD_PRESERVE);

/*
 *	Field (CWAR, ByteAcc, NoLock, Preserve)
 *	{
 *		Offset (0xcd),
 *		PMEE,    1,
 *		    ,    6,
 *		PMES,    1
 *	}
 */
	struct fieldlist fields2[] = {
		FIELDLIST_OFFSET(0xcd),
		FIELDLIST_NAMESTR("PMEE", 1),
		FIELDLIST_RESERVED(6),
		FIELDLIST_NAMESTR("PMES", 1),
	};
	acpigen_write_field("CWAR", fields2, ARRAY_SIZE(fields2),
		FIELD_BYTEACC | FIELD_NOLOCK | FIELD_PRESERVE);

/*
 *	Name (_S0W, 3)
 */
	acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D3_HOT);

/*
 *	Name (PRRS, Zero)
 *	Last _PRR Status
 *	0: Non-Platform-Level Device Reset (PLDR) executed [Default]
 *	1: Core PLDR Completed successfully
 *	2: Product PLDR Completed successfully
 *	3: Previous PLDR Not Completed
 *	4: Product PLDR timeout
 */
	acpigen_write_name_integer("PRRS", 0);

/*
 *	Name (RSTT, Zero)
 *	Reset Type
 *	0: Core Reset
 *	1: Product Reset
 */
	acpigen_write_name_integer("RSTT", 0);

/*
 *	PowerResource(CNVP, 0, 0)
 *	{
 *		Method(_STA)
 *		{
 *			If (CondRefOf (\_SB.PCI0.CNVS)) {
 *				Local0 = \_SB.PCI0.CNVS()
 *				Return (Local0)
 *			}
 *			Else
 *			{
 *				Return (0x01)
 *			}
 *		}
 *		Method(_ON, 0)
 *		{
 *			If (CondRefOf (\_SB.PCI0.CNVC)) {
 *				If ((\_SB.PCI0.CNVS() == 1)) {
 *					Return (1)
 *				}
 *				\_SB.PCI0.CNVS(1)
 *			}
 *		}
 *		Method(_OFF, 0)
 *		{
 *			If (CondRefOf (\_SB.PCI0.CNVC)) {
 *				\_SB.PCI0.CNVS(0)
 *			}
 *		}
 *		Method(_RST, 0, NotSerialized)
 *		{
 *			Local0 = Acquire (\_SB.PCI0.CNMT, 1000)
 *			If ((Local0 == Zero))
 *			{
 *				CFLR ()
 *				PRRS = One
 *				If ((RSTT == One))
 *				{
 *					If (((PCRR (CNVI_SIDEBAND_ID, CNVI_ABORT_PLDR) & CNVI_ABORT_REQUEST) == Zero))
 *					{
 *						Local2 = Zero
 *						If (CondRefOf (\_SB.PCI0.GBTR)) {
 *							If ((\_SB.PCI0.GBTR() == One))
 *							{
 *								\_SB.PCI0.BTRK (Zero)
 *								Sleep (160)
 *								Local2 = One
 *							}
 *						}
 *						PCRO (CNVI_SIDEBAND_ID, CNVI_ABORT_PLDR, CNVI_ABORT_REQUEST | CNVI_ABORT_ENABLE)
 *						Sleep (10)
 *						Local1 = PCRR (CNVI_SIDEBAND_ID, CNVI_ABORT_PLDR)
 *						If ((((Local1 & CNVI_ABORT_REQUEST) == Zero) && (Local1 & CNVI_READY)))
 *						{
 *							PRRS = CNVI_PLDR_COMPLETE
 *							If ((Local2 == One))
 *							{
 *								\_SB.PCI0.BTRK (One)
 *								Sleep (160)
 *							}
 *						}
 *						Else
 *						{
 *							PRRS = CNVI_PLDR_NOT_COMPLETE
 *						}
 *					}
 *					Else
 *					{
 *						PRRS = CNVI_PLDR_TIMEOUT
 *					}
 *				}
 *				Release (\_SB.PCI0.CNMT)
 *			}
 *		}
 *	}
 *
 */
	acpigen_write_power_res("CNVP", 0, 0, NULL, 0);
	{
		acpigen_write_method("_STA", 0);
		{
			acpigen_write_if_cond_ref_of("\\_SB.PCI0.CNVS");
			{
				acpigen_write_store();
				acpigen_emit_namestring("\\_SB.PCI0.CNVS");
				acpigen_emit_byte(LOCAL0_OP);

				acpigen_write_return_op(LOCAL0_OP);
			}
			acpigen_write_else();
			{
				acpigen_write_return_integer(1);
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();

		acpigen_write_method("_ON", 0);
		{
			acpigen_write_if_cond_ref_of("\\_SB.PCI0.CNVC");
			{

				acpigen_write_store();
				acpigen_emit_namestring("\\_SB.PCI0.CNVS");
				acpigen_emit_byte(LOCAL0_OP);

				acpigen_write_if_lequal_op_int(LOCAL0_OP, 1);
				{
					acpigen_write_return_integer(1);
				}
				acpigen_pop_len();

				acpigen_emit_namestring("\\_SB.PCI0.CNVC");
				acpigen_emit_byte(1);
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();

		acpigen_write_method("_OFF", 0);
		{
			acpigen_write_if_cond_ref_of("\\_SB.PCI0.CNVC");
			{
				acpigen_emit_namestring("\\_SB.PCI0.CNVC");
				acpigen_emit_byte(0);
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();

		acpigen_write_method("_RST", 0);
		{
			acpigen_write_store();
			acpigen_write_acquire("\\_SB.PCI0.CNMT", 1000);
			acpigen_emit_byte(LOCAL0_OP);

			acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
			{
				acpigen_emit_namestring("CFLR");

				acpigen_write_store_int_to_namestr(1, "PRRS");

				acpigen_write_if_lequal_namestr_int("RSTT", 1);
				{
					acpigen_write_store();
					acpigen_emit_namestring("\\_SB.PCI0.PCRR");
					acpigen_write_integer(PID_CNVI);
					acpigen_write_integer(CNVI_ABORT_PLDR);
					acpigen_emit_byte(LOCAL0_OP);

					acpigen_emit_byte(AND_OP);
					acpigen_emit_byte(LOCAL0_OP);
					acpigen_write_integer(CNVI_ABORT_REQUEST);
					acpigen_emit_byte(LOCAL0_OP);

					acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
					{
						acpigen_write_store_int_to_op(0, LOCAL2_OP);
						acpigen_write_if_cond_ref_of("\\_SB.PCI0.GBTR");
						{
							acpigen_write_if_lequal_namestr_int("\\_SB.PCI0.GBTR", 1);
							{
								acpigen_emit_namestring("\\_SB.PCI0.BTRK");
								acpigen_emit_byte(0);

								acpigen_write_sleep(160);

								acpigen_write_store_ops(1, LOCAL2_OP);
							}
							acpigen_pop_len();
						}
						acpigen_pop_len();

						acpigen_emit_namestring("\\_SB.PCI0.PCRO");
						acpigen_write_integer(PID_CNVI);
						acpigen_write_integer(CNVI_ABORT_PLDR);
						acpigen_write_integer(CNVI_ABORT_REQUEST | CNVI_ABORT_ENABLE);

						acpigen_write_sleep(10);

						acpigen_write_store();
						acpigen_emit_namestring("\\_SB.PCI0.PCRR");
						acpigen_write_integer(PID_CNVI);
						acpigen_write_integer(CNVI_ABORT_PLDR);
						acpigen_emit_byte(LOCAL0_OP);

						acpigen_emit_byte(AND_OP);
						acpigen_emit_byte(LOCAL0_OP);
						acpigen_write_integer(CNVI_ABORT_REQUEST);
						acpigen_emit_byte(LOCAL1_OP);

						acpigen_emit_byte(AND_OP);
						acpigen_emit_byte(LOCAL0_OP);
						acpigen_write_integer(CNVI_READY);
						acpigen_emit_byte(LOCAL3_OP);

						acpigen_write_if_lequal_op_int(LOCAL1_OP, 0);
						{
							acpigen_write_if_lequal_op_int(LOCAL3_OP, CNVI_READY);
							{
								acpigen_write_store_int_to_namestr(CNVI_PLDR_COMPLETE, "PRRS");

								acpigen_write_if_lequal_op_int(LOCAL2_OP, 1);
								{
									acpigen_emit_namestring("\\_SB.PCI0.BTRK");
									acpigen_emit_byte(1);
									acpigen_write_sleep(160);
								}
								acpigen_pop_len();
							}
							acpigen_pop_len();
						}
						acpigen_write_else();
						{
							acpigen_write_store_int_to_namestr(CNVI_PLDR_NOT_COMPLETE, "PRRS");
						}
						acpigen_pop_len();
					}
					acpigen_write_else();
					{
						acpigen_write_store_int_to_namestr(CNVI_PLDR_TIMEOUT, "PRRS");
					}
					acpigen_pop_len();
				}
				acpigen_pop_len();
				acpigen_write_release("\\_SB.PCI0.CNMT");
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();
	}
	acpigen_write_power_res_end();

/*
 *	Name (_PRR, Package (0x01)
 *	{
 *		CNVP
 *	})
 */
	acpigen_write_name("_PRR");
	{
		acpigen_write_package(1);
		acpigen_emit_namestring("CNVP");
	}
	acpigen_pop_len();

/*
 *	Name (_PR0, Package (0x01)
 *	{
 *		CNVP
 *	})
 */
	acpigen_write_name("_PR0");
	{
		acpigen_write_package(1);
		acpigen_emit_namestring("CNVP");
	}
	acpigen_pop_len();

/*
 *	Method (GPEH, 0, NotSerialized)
 *	{
 *		If ((VDID == 0xFFFFFFFF))
 *		{
 *			Return (Zero)
 *		}
 *		If ((PMES == One))
 *		{
 *			Notify (CNVW, 0x02) // Device Wake
 *		}
 *	}
 */
	acpigen_write_method("GPEH", 0);
	{
		acpigen_write_if_lequal_namestr_int("VDID", 0xffffffff);
		{
			acpigen_write_return_integer(0);
		}
		acpigen_pop_len();

		acpigen_write_if_lequal_namestr_int("PMES", 1);
		{
			acpigen_notify("CNVW", 2);
		}
		acpigen_pop_len();
	}
	acpigen_pop_len();

/*
 *	Method (CFLR, 0, NotSerialized)
 *	{
 *		If (WFLR == One)
 *		{
 *			WIFR = One
 *		}
 *	}
 */
	acpigen_write_method("CFLR", 0);
	{
		acpigen_write_if_lequal_namestr_int("WFLR", 1);
		{
			acpigen_write_store_int_to_namestr(1, "WIFR");
		}
		acpigen_pop_len();
	}
	acpigen_pop_len();

	acpigen_write_scope_end();
}

static struct device_operations cnvi_wifi_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
	.acpi_name		= cnvi_wifi_acpi_name,
	.acpi_fill_ssdt		= cnvw_fill_ssdt,
};

static const unsigned short wifi_pci_device_ids[] = {
	PCI_DID_INTEL_WCL_CNVI_WIFI_0,
	PCI_DID_INTEL_WCL_CNVI_WIFI_1,
	PCI_DID_INTEL_WCL_CNVI_WIFI_2,
	PCI_DID_INTEL_WCL_CNVI_WIFI_3,
	PCI_DID_INTEL_PTL_H_CNVI_WIFI_0,
	PCI_DID_INTEL_PTL_H_CNVI_WIFI_1,
	PCI_DID_INTEL_PTL_H_CNVI_WIFI_2,
	PCI_DID_INTEL_PTL_H_CNVI_WIFI_3,
	PCI_DID_INTEL_PTL_U_H_CNVI_WIFI_0,
	PCI_DID_INTEL_PTL_U_H_CNVI_WIFI_1,
	PCI_DID_INTEL_PTL_U_H_CNVI_WIFI_2,
	PCI_DID_INTEL_PTL_U_H_CNVI_WIFI_3,
	PCI_DID_INTEL_LNL_CNVI_WIFI_0,
	PCI_DID_INTEL_LNL_CNVI_WIFI_1,
	PCI_DID_INTEL_LNL_CNVI_WIFI_2,
	PCI_DID_INTEL_LNL_CNVI_WIFI_3,
	PCI_DID_INTEL_MTL_CNVI_WIFI_0,
	PCI_DID_INTEL_MTL_CNVI_WIFI_1,
	PCI_DID_INTEL_MTL_CNVI_WIFI_2,
	PCI_DID_INTEL_MTL_CNVI_WIFI_3,
	PCI_DID_INTEL_ARL_CNVI_WIFI,
	PCI_DID_INTEL_CML_LP_CNVI_WIFI,
	PCI_DID_INTEL_CML_H_CNVI_WIFI,
	PCI_DID_INTEL_CNL_LP_CNVI_WIFI,
	PCI_DID_INTEL_CNL_H_CNVI_WIFI,
	PCI_DID_INTEL_GLK_CNVI_WIFI,
	PCI_DID_INTEL_JSL_CNVI_WIFI_0,
	PCI_DID_INTEL_JSL_CNVI_WIFI_1,
	PCI_DID_INTEL_JSL_CNVI_WIFI_2,
	PCI_DID_INTEL_JSL_CNVI_WIFI_3,
	PCI_DID_INTEL_TGL_CNVI_WIFI_0,
	PCI_DID_INTEL_TGL_CNVI_WIFI_1,
	PCI_DID_INTEL_TGL_CNVI_WIFI_2,
	PCI_DID_INTEL_TGL_CNVI_WIFI_3,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_0,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_1,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_2,
	PCI_DID_INTEL_TGL_H_CNVI_WIFI_3,
	PCI_DID_INTEL_ADL_P_CNVI_WIFI_0,
	PCI_DID_INTEL_ADL_P_CNVI_WIFI_1,
	PCI_DID_INTEL_ADL_P_CNVI_WIFI_2,
	PCI_DID_INTEL_ADL_P_CNVI_WIFI_3,
	PCI_DID_INTEL_ADL_S_CNVI_WIFI_0,
	PCI_DID_INTEL_ADL_S_CNVI_WIFI_1,
	PCI_DID_INTEL_ADL_S_CNVI_WIFI_2,
	PCI_DID_INTEL_ADL_S_CNVI_WIFI_3,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_0,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_1,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_2,
	PCI_DID_INTEL_ADL_N_CNVI_WIFI_3,
	PCI_DID_INTEL_RPL_S_CNVI_WIFI_0,
	PCI_DID_INTEL_RPL_S_CNVI_WIFI_1,
	PCI_DID_INTEL_RPL_S_CNVI_WIFI_2,
	PCI_DID_INTEL_RPL_S_CNVI_WIFI_3,
	0
};

static const struct pci_driver pch_cnvi_wifi __pci_driver = {
	.ops			= &cnvi_wifi_ops,
	.vendor			= PCI_VID_INTEL,
	.devices		= wifi_pci_device_ids,
};

static const char *cnvi_bt_acpi_name(const struct device *dev)
{
	return "CNVB";
}

static struct device_operations cnvi_bt_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus,
	.acpi_name		= cnvi_bt_acpi_name,
	.acpi_fill_ssdt		= acpi_device_write_pci_dev,
};

static const unsigned short bt_pci_device_ids[] = {
	PCI_DID_INTEL_WCL_CNVI_BT,
	PCI_DID_INTEL_PTL_H_CNVI_BT,
	PCI_DID_INTEL_PTL_U_H_CNVI_BT,
	PCI_DID_INTEL_TGL_CNVI_BT_0,
	PCI_DID_INTEL_TGL_CNVI_BT_1,
	PCI_DID_INTEL_TGL_CNVI_BT_2,
	PCI_DID_INTEL_TGL_CNVI_BT_3,
	PCI_DID_INTEL_TGL_H_CNVI_BT_0,
	PCI_DID_INTEL_TGL_H_CNVI_BT_1,
	PCI_DID_INTEL_TGL_H_CNVI_BT_2,
	0
};

static const struct pci_driver pch_cnvi_bt __pci_driver = {
	.ops			= &cnvi_bt_ops,
	.vendor			= PCI_VID_INTEL,
	.devices		= bt_pci_device_ids,
};
