/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include "chip.h"

/*
 * Intel Bluetooth DSM
 *
 * Check/Set Reset Delay (aa10f4e0-81ac-4233-abf6-3b2ac50e28d9)
 * Arg2 == 0:	Return a package with the following bit set
 * BIT(0)	Indicates whether the device supports other functions
 * BIT(1)	Check Bluetooth reset timing
 *
 * Arg2 == 1:	Set the reset delay based on Arg3
 *
 * Arg2 == 3:	Set the reset method based on Arg3 (Not supported by this driver)
 * WDISABLE2 (BT_RF_KILL_N)
 * VSEC (PCI Config Space)
 */

static void check_reset_delay(void *arg)
{
	acpigen_write_if_lequal_op_int(ARG1_OP, 0);
	{
		acpigen_write_return_singleton_buffer(0x03);
	}
	acpigen_write_else();
	{
		acpigen_write_return_singleton_buffer(0x00);
	}
	acpigen_pop_len();
}

static void set_reset_delay(void *arg)
{
	acpigen_write_store_op_to_namestr(ARG3_OP, "RDLY");
}

static void not_supported(void *arg)
{
	acpigen_write_return_singleton_buffer(0x00);
}

void (*reset_supported[])(void *) = { check_reset_delay, set_reset_delay };
void (*reset_unsupported[])(void *) = { not_supported };

void acpi_device_intel_bt(unsigned int reset_gpio, unsigned int enable_gpio, bool audio_offload)
{
/*
 *	Name (RDLY, 0x69)
 */
	acpigen_write_name_integer("RDLY", 0x69);

/*
 *	Method (_DSM, 4, Serialized)
 *	{
 *		If ((Arg0 == ToUUID ("aa10f4e0-81ac-4233-abf6-3b2ac50e28d9")))
 *		{
 *			If ((Arg2 == Zero))
 *			{
 *				If ((Arg1 == Zero))
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x03
 *					})
 *				}
 *				Else
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x00
 *					})
 *				}
 *			}
 *			If ((Arg2 == One))
 *			{
 *				RDLY = Arg3
 *			}
 *			Return (Zero)
 *		}
 *		Else
 *		{
 *			Return (Buffer (One)
 *			{
 *				0x00
 *			})
 *		}
 *	}
 */

	struct dsm_uuid uuid_callbacks[] = {
		DSM_UUID("aa10f4e0-81ac-4233-abf6-3b2ac50e28d9",
			reset_gpio ? reset_supported : reset_unsupported,
			reset_gpio ? ARRAY_SIZE(reset_supported) : ARRAY_SIZE(reset_unsupported),
			NULL),
	};

	acpigen_write_dsm_uuid_arr(uuid_callbacks, ARRAY_SIZE(uuid_callbacks));
/*
 *	PowerResource (BTRT, 0, 0)
 *	{
 *		Method (_STA, 0, NotSerialized)
 *		{
 *			Return (\_SB.PCI0.GBTE())
 *		}
 *		Method (_ON, 0, NotSerialized)
 *		{
 *			\_SB.PCI0.SBTE(1)
 *		}
 *		Method (_OFF, 0, NotSerialized)
 *		{
 *			\_SB.PCI0.SBTE(0)
 *		}
 *		Method (_RST, 0, NotSerialized)
 *		{
 *			Local0 = Acquire (\_SB.PCI0.CNMT, 1000)
 *			If ((Local0 == Zero))
 *			{
 *				\_SB.PCI0.BTRK (Zero)
 *				Sleep (RDLY)
 *				\_SB.PCI0.BTRK (One)
 *				Sleep (RDLY)
 *				Release (\_SB.PCI0.CNMT)
			}
 *		}
 *	}
 */
	acpigen_write_power_res("BTRT", 0, 0, NULL, 0);
	{
		acpigen_write_method("_STA", 0);
		{
			if (enable_gpio) {
				acpigen_write_store();
				acpigen_emit_namestring("\\_SB.PCI0.GBTE");
				acpigen_emit_byte(LOCAL0_OP);

				acpigen_write_return_op(LOCAL0_OP);
			} else {
				acpigen_write_return_integer(1);
			}
		}
		acpigen_pop_len();

		acpigen_write_method("_ON", 0);
		{
			if (enable_gpio) {
				acpigen_emit_namestring("\\_SB.PCI0.SBTE");
				acpigen_emit_byte(1);
			}
		}
		acpigen_pop_len();

		acpigen_write_method("_OFF", 0);
		{
			if (enable_gpio) {
				acpigen_emit_namestring("\\_SB.PCI0.SBTE");
				acpigen_emit_byte(0);
			}
		}
		acpigen_pop_len();

		acpigen_write_method("_RST", 0);
		{
			if (reset_gpio) {
				acpigen_write_store();
				acpigen_write_acquire("\\_SB.PCI0.CNMT", 1000);
				acpigen_emit_byte(LOCAL0_OP);

				acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
				{
					acpigen_emit_namestring("\\_SB.PCI0.BTRK");
					acpigen_emit_byte(0);

					acpigen_emit_ext_op(SLEEP_OP);
					acpigen_emit_namestring("RDLY");

					acpigen_emit_namestring("\\_SB.PCI0.BTRK");
					acpigen_emit_byte(1);

					acpigen_emit_ext_op(SLEEP_OP);
					acpigen_emit_namestring("RDLY");

					acpigen_write_release("\\_SB.PCI0.CNMT");
				}
				acpigen_pop_len();
			}
		}
		acpigen_pop_len();
	}
	acpigen_write_power_res_end();

/*
 *	Name (_PRR, Package (0x01)
 *	{
 *		BTRT
 *	})
 */
	acpigen_write_name("_PRR");
	{
		acpigen_write_package(1);
		acpigen_emit_namestring("BTRT");
	}
	acpigen_pop_len();

/*
 *	Name (_PR0, Package (0x01)
 *	{
 *		BTRT
 *	})
 */
	acpigen_write_name("_PR0");
	{
		acpigen_write_package(1);
		acpigen_emit_namestring("BTRT");
	}
	acpigen_pop_len();

/*
 *	Name (_PR3, Package (0x01)
 *	{
 *		BTRT
 *	})
 */
	acpigen_write_name("_PR3");
	{
		acpigen_write_package(1);
		acpigen_emit_namestring("BTRT");
	}
	acpigen_pop_len();

/*
 *	Method (AOLD, 0, Serialized)
 *	{
 *		Name (AODS, Package (0x03)
 *		{
 *			Zero,
 *			0x12,
 *			Zero	// Audio Offload - 0: Disabled
 *						   1: Enabled
 *		})
 *		Return (AODS)
 *	}
 */
	acpigen_write_method_serialized("AOLD", 0);
	{
		acpigen_write_name("AODS");
		acpigen_write_package(3);
		{
			acpigen_write_integer(0);
			acpigen_write_integer(0x12);
			acpigen_write_integer(audio_offload);
		}
		acpigen_pop_len();

		acpigen_write_return_namestr("AODS");
	}
	acpigen_pop_len();
}

void acpi_device_intel_bt_common(unsigned int enable_gpio, unsigned int reset_gpio)
{
	acpigen_write_scope("\\_SB.PCI0");
/*
 *	Mutex (CNMT, 0)
 */
	acpigen_write_mutex("CNMT", 0);

/*
 *	Method (SBTE, 1, Serialized)
 *	{
 *		If (Arg0 == 1)
 *		{
 *			STXS(enable_gpio)
 *		} Else {
 *			CTXS(enable_gpio)
 *		}
 *	}
 */
	acpigen_write_method_serialized("SBTE", 1);
	{
		if (enable_gpio) {
			acpigen_write_if_lequal_op_int(ARG0_OP, 1);
			{
				acpigen_soc_set_tx_gpio(enable_gpio);
			}
			acpigen_write_else();
			{
				acpigen_soc_clear_tx_gpio(enable_gpio);
			}
			acpigen_pop_len();
		}
	}
	acpigen_pop_len();

/*
 *	Method (GBTE, 0, NotSerialized)
 *	{
 *		Return (GTXS (enable_gpio))
 *	}
 */
	acpigen_write_method("GBTE", 0);
	{
		if (enable_gpio) {
			acpigen_soc_get_tx_gpio(enable_gpio);
			acpigen_write_return_op(LOCAL0_OP);
		} else {
			acpigen_write_return_integer(0);
		}
	}
	acpigen_pop_len();

/*
 *	Method (BTRK, 1, Serialized)
 *	{
 *		If (Arg0 == 1)
 *		{
 *			STXS (reset_gpio)
 *		} Else {
 *			CTXS (reset_gpio)
 *		}
 *	}
 */
	acpigen_write_method_serialized("BTRK", 1);
	{
		acpigen_write_if_lequal_op_int(ARG0_OP, 1);
		{
			acpigen_soc_set_tx_gpio(reset_gpio);
		}
		acpigen_write_else();
		{
			acpigen_soc_clear_tx_gpio(reset_gpio);
		}
		acpigen_pop_len();
	}
	acpigen_pop_len();

/*
 *	Method (GBTR, 0, NotSerialized)
 *	{
 *		 Return (GTXS (reset_gpio))
 *	}
 */
	acpigen_write_method("GBTR", 0);
	{
		if (reset_gpio) {
			acpigen_soc_get_tx_gpio(reset_gpio);
			acpigen_write_return_op(LOCAL0_OP);
		} else {
			acpigen_write_return_op(0);
		}
	}
	acpigen_pop_len();

	acpigen_pop_len();
}
