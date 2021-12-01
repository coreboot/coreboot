/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>

#include "chip.h"
#include "ec.h"
#include "commands.h"

static bool is_curve_valid(struct ec_clevo_it5570e_fan_curve curve)
{
	/*
	 * Fan curve speeds have to be non-decreasing.
	 * Fan curve temperatures have to be increasing (to avoid division by 0).
	 * This also covers the case when the curve is all zeroes (i.e. not configured).
	 */

	for (int i = 1; i < IT5570E_FAN_CURVE_LEN; i++) {
		if (curve.speed[i]       <  curve.speed[i - 1] ||
		    curve.temperature[i] <= curve.temperature[i - 1])
			return false;
	}

	return true;
}

static void write_fan_curve(struct ec_clevo_it5570e_fan_curve curve, int fan)
{
	uint16_t ramp;
	char fieldname[5];

	/* Curve points */
	for (int i = 0; i < IT5570E_FAN_CURVE_LEN; i++) {
		snprintf(fieldname, 5, "F%dT%d", fan + 1, i + 1);
		acpigen_write_store_int_to_namestr(curve.temperature[i], fieldname);
		snprintf(fieldname, 5, "F%dD%d", fan + 1, i + 1);
		acpigen_write_store_int_to_namestr(curve.speed[i] * 255 / 100, fieldname);
	}

	/* Ramps */
	for (int i = 0; i < (IT5570E_FAN_CURVE_LEN - 1); i++) {
		ramp = 255 * 16 *
		       (curve.speed[i + 1] - curve.speed[i]) /
		       (curve.temperature[i + 1] - curve.temperature[i]) /
		       100;

		snprintf(fieldname, 5, "F%dR%d", fan + 1, i + 1);
		acpigen_write_store_int_to_namestr(ramp, fieldname);
	}
}

static void write_fan_opregion(int fan_cnt)
{
	char fieldname[5];
	uint8_t flags = FIELD_ANYACC | FIELD_LOCK | FIELD_PRESERVE;
	struct opregion opreg = {
		.name         = "FNCV",
		.regionspace  = SYSTEMMEMORY,
		.regionoffset = CONFIG_EC_CLEVO_IT5570E_MEM_BASE + 0x38c,
		.regionlen    = fan_cnt * 14,
	};

	acpigen_write_opregion(&opreg);
	acpigen_emit_ext_op(FIELD_OP);
	acpigen_write_len_f();
	acpigen_emit_namestring(opreg.name);
	acpigen_emit_byte(flags);

	for (int fan = 1; fan <= fan_cnt; fan++) {
		/* temps */
		for (int i = 1; i <= IT5570E_FAN_CURVE_LEN; i++) {
			snprintf(fieldname, 5, "F%dT%d", fan, i);
			acpigen_write_field_name(fieldname, 8);
		}

		/* duties */
		for (int i = 1; i <= IT5570E_FAN_CURVE_LEN; i++) {
			snprintf(fieldname, 5, "F%dD%d", fan, i);
			acpigen_write_field_name(fieldname, 8);
		}

		/* ramps */
		for (int i = 1; i < IT5570E_FAN_CURVE_LEN; i++) {
			snprintf(fieldname, 5, "F%dR%d", fan, i);
			acpigen_write_field_name(fieldname, 16);
		}
	}

	acpigen_pop_len();	/* Field */
}

/*
 * Set Fan curve
 * The function must exist even if the fan curve isn't enabled in devicetree.
 */
void ec_fan_curve_fill_ssdt(const struct device *dev)
{
	const ec_config_t *config = config_of(dev);
	const int fan_cnt = read8p(ECRAM + FANC);

	acpigen_write_scope(acpi_device_path(dev));
	write_fan_opregion(fan_cnt);
	acpigen_write_method("SFCV", 0);

	if (config->fan_mode == FAN_MODE_CUSTOM) {
		int curve_cnt = 0;

		/* Check curve count against fan count from EC */
		for (int i = 0; i < IT5570E_MAX_FAN_CNT; i++)
			if (*config->fan_curves[i].speed && *config->fan_curves[i].temperature)
				curve_cnt++;

		if (curve_cnt != fan_cnt) {
			printk(BIOS_WARNING,
			       "EC: Fan curve count (%d) does not match fan count (%d). "
			       "Check your devicetree!\n", curve_cnt, fan_cnt);
			goto pop;
		}

		/*
		 * Check all curves.
		 * Custom mode can only be enabled for all fans or none. Thus, all
		 * custom curves must be valid before custom mode can be enabled.
		 */
		bool error = false;
		for (int i = 0; i < fan_cnt; i++) {
			if (!is_curve_valid(config->fan_curves[i])) {
				printk(BIOS_ERR,
				       "EC: Fan %d curve invalid. Check your devicetree!\n", i);
				error = true;
			}
		}
		if (error)
			goto pop;

		acpigen_write_debug_string("EC: Apply custom fan curve");

		for (int i = 0; i < fan_cnt; i++)
			write_fan_curve(config->fan_curves[i], i);

		/* Enable custom fan mode */
		acpigen_write_store_int_to_namestr(0x04, "FDAT");
		acpigen_emit_namestring("SFCC");
		acpigen_write_integer(0xd7);
	}

pop:
	acpigen_pop_len(); /* Method */
	acpigen_pop_len(); /* Scope */
}
