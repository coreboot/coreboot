/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include "chip.h"

/*
 * Intel Bluetooth DSM
 *
 * Check Tile Activation (2d19d3e1-5708-4696-bd5b-2c3dbae2d6a9)
 *
 * Arg2 == 0: Return a package with the following bits set
 * BIT(0)	Indicates whether the device supports other functions
 * BIT(1)	Check Tile Activation
 *
 * Check/Set Reset Delay (aa10f4e0-81ac-4233-abf6-3b2ac50e28d9)
 * Arg2 == 0: Return a package with the following bit set
 * BIT(0)	Indicates whether the device supports other functions
 * BIT(1)	Check Bluetooth reset timing
 * Arg2 == 1: Set the reset delay based on Arg3
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

static void get_feature_flag(void *arg)
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

void (*uuid_callbacks1[])(void *) = { check_reset_delay, set_reset_delay };
void (*uuid_callbacks2[])(void *) = { get_feature_flag };

void acpi_device_intel_bt(unsigned int reset_gpio, bool audio_offload)
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
 *		ElseIf ((Arg0 == ToUUID ("2d19d3e1-5708-4696-bd5b-2c3dbae2d6a9")))
 *		{
 *			If ((Arg2 == Zero))
 *			{
 *				If ((Arg1 == Zero))
 *				{
 *					Return (Buffer (One)
 *					{
 *						0x00
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
		DSM_UUID("aa10f4e0-81ac-4233-abf6-3b2ac50e28d9", uuid_callbacks1, 2, NULL),
		DSM_UUID("2d19d3e1-5708-4696-bd5b-2c3dbae2d6a9", uuid_callbacks2, 1, NULL),
	};

	acpigen_write_dsm_uuid_arr(uuid_callbacks, ARRAY_SIZE(uuid_callbacks));
/*
 *	PowerResource (BTRT, 0x05, 0x0000)
 *	{
 *		Method (_STA, 0, NotSerialized)
 *		{
 *			Return (One)
 *		}
 *		Method (_ON, 0, NotSerialized)
 *		{
 *		}
 *		Method (_OFF, 0, NotSerialized)
 *		{
 *		}
 *		Method (_RST, 0, NotSerialized)
 *		{
 *			Local0 = Acquire (\_SB.PCI0.CNMT, 1000)
 *			If ((Local0 == Zero))
 *			{
 *				BTRK (Zero)
 *				Sleep (RDLY)
 *				BTRK (One)
 *				Sleep (RDLY)
 *				Release (\_SB.PCI0.CNMT)
			}
 *		}
 *	}
 */
	acpigen_write_power_res("BTRT", 5, 0, NULL, 0);
	{
		acpigen_write_method("_STA", 0);
		{
			acpigen_write_return_integer(1);
		}
		acpigen_pop_len();

		acpigen_write_method("_ON", 0);
		acpigen_pop_len();

		acpigen_write_method("_OFF", 0);
		acpigen_pop_len();

		acpigen_write_method("_RST", 0);
		{
			acpigen_write_store();
			acpigen_write_acquire("\\_SB.PCI0.CNMT", 1000);
			acpigen_emit_byte(LOCAL0_OP);

			acpigen_write_if_lequal_op_int(LOCAL0_OP, 0);
			{
				acpigen_emit_namestring("BTRK");
				acpigen_emit_byte(0);

				acpigen_emit_ext_op(SLEEP_OP);
				acpigen_emit_namestring("RDLY");

				acpigen_emit_namestring("BTRK");
				acpigen_emit_byte(1);

				acpigen_emit_ext_op(SLEEP_OP);
				acpigen_emit_namestring("RDLY");

				acpigen_write_release("\\_SB.PCI0.CNMT");
			}
			acpigen_pop_len();
		}
		acpigen_pop_len();
	}
	acpigen_write_power_res_end();

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

void acpi_device_intel_bt_common(void)
{
	acpigen_write_scope("\\_SB.PCI0");
/*
 *	Mutex (CNMT, 0)
 */
	acpigen_write_mutex("CNMT", 0);

	acpigen_pop_len();
}
