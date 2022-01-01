/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <string.h>
#include "i915.h"

void
drivers_intel_gma_displays_ssdt_generate(const struct i915_gpu_controller_info *conf)
{
	size_t i;
	const char *names[] = { "UNK", "VGA", "TV", "DVI", "LCD" };
	int counters[ARRAY_SIZE(names)] = { 0 };

	if (!conf->ndid)
		return;

	acpigen_write_scope("\\_SB.PCI0.GFX0");

	/*
	  Method (_DOD, 0)
	  {
		Return (Package() {
				0x5a5a5a5a,
				0x5a5a5a5a,
				0x5a5a5a5a
			})
	  }
	*/
	acpigen_write_method("_DOD", 0);

	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(conf->ndid);
	for (i = 0; i < conf->ndid; i++) {
		acpigen_write_dword (conf->did[i] | 0x80010000);
	}
	acpigen_pop_len(); /* End Package. */

	acpigen_pop_len(); /* End Method. */

	for (i = 0; i < conf->ndid; i++) {
		char name[5];
		int kind;

		kind = (conf->did[i] >> 8) & 0xf;
		if (kind >= ARRAY_SIZE(names)) {
			kind = 0;
		}

		snprintf(name, sizeof(name), "%s%d", names[kind], counters[kind]);
		counters[kind]++;

		/* Device (LCD0) */
		acpigen_write_device(name);

		/* Name (_ADR, 0x0410) */
		acpigen_write_name_dword("_ADR", conf->did[i] & 0xffff);

		/* ACPI brightness for LCD.  */
		if (kind == 4) {
			/*
			  Method (_BCL, 0, NotSerialized)
			  {
				Return (^^XBCL())
			  }
			*/
			acpigen_write_method("_BCL", 0);
			acpigen_emit_byte(RETURN_OP);
			acpigen_emit_namestring("^^XBCL");
			acpigen_pop_len();

			/*
			  Method (_BCM, 1, NotSerialized)
			  {
				^^XBCM(Arg0)
			  }
			*/
			acpigen_write_method("_BCM", 1);
			acpigen_emit_namestring("^^XBCM");
			acpigen_emit_byte(ARG0_OP);
			acpigen_pop_len();

			/*
			  Method (_BQC, 0, NotSerialized)
			  {
				Return (^^XBQC())
			  }
			*/
			acpigen_write_method("_BQC", 0);
			acpigen_emit_byte(RETURN_OP);
			acpigen_emit_namestring("^^XBQC");
			acpigen_pop_len();
		}

		/*
		 * _DCS, _DGS and _DSS are required by specification. However,
		 * we never implemented them properly, and no OS driver com-
		 * plained yet. So we stub them out and keep the traditional
		 * behavior in case an OS driver checks for their existence.
		 */

		/*
		  Method(_DCS, 0)
		  {
			Return (0x1d)
		  }
		*/
		acpigen_write_method("_DCS", 0);
		acpigen_write_return_integer(0x1d);
		acpigen_pop_len();

		/*
		  Method(_DGS, 0)
		  {
			Return (0)
		  }
		*/
		acpigen_write_method("_DGS", 0);
		acpigen_write_return_integer(0);
		acpigen_pop_len();

		/*
		  Method(_DSS, 1)
		  {
		  }
		*/
		acpigen_write_method("_DSS", 1);
		acpigen_pop_len();

		acpigen_pop_len(); /* End Device. */
	}

	acpigen_pop_len(); /* End Scope. */
}
