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

void acpi_device_intel_bt(void)
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
 *			Local0 = Acquire (CNMT, 0x03E8)
 *			If ((Local0 == Zero))
 *			{
 *				BTRK (Zero)
 *				Sleep (RDLY)
 *				BTRK (One)
 *				Sleep (RDLY)
 *			}
 *			Release (CNMT)
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
			acpigen_write_acquire("CNMT", 0x03e8);
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
			}
			acpigen_pop_len();
			acpigen_write_release("CNMT");
		}
		acpigen_pop_len();
	}
	acpigen_write_power_res_end();
}
