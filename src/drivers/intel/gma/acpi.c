/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <string.h>
#include "i915.h"

void
drivers_intel_gma_displays_ssdt_generate(const struct i915_gpu_controller_info *conf)
{
	size_t i;
	const char *names[] = { "UNK", "VGA", "TV", "DVI", "LCD" };
	int counters[ARRAY_SIZE(names)];

	memset(counters, 0, sizeof(counters));

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
	acpigen_emit_byte(0xa4); /* ReturnOp.  */
	acpigen_write_package(conf->ndid);

	for (i = 0; i < conf->ndid; i++) {
		acpigen_write_dword (conf->did[i] | 0x80010000);
	}
	acpigen_pop_len(); /* End Package. */
	acpigen_pop_len(); /* End Method. */

	for (i = 0; i < conf->ndid; i++) {
		char name[10];
		char *ptr;
		int kind;
		kind = (conf->did[i] >> 8) & 0xf;
		if (kind >= ARRAY_SIZE(names)) {
			kind = 0;
		}
		strcpy(name, names[kind]);
		for (ptr = name; *ptr; ptr++);
		*ptr++ = counters[kind] + '0';
		*ptr++ = '\0';
		counters[kind]++;
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
			acpigen_emit_byte(0xa4); /* ReturnOp.  */
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
			acpigen_emit_byte(0x68); /* Arg0Op.  */
			acpigen_pop_len();

			/*
			  Method (_BQC, 0, NotSerialized)
			  {
				Return (^^XBQC())
			  }
			*/
			acpigen_write_method("_BQC", 0);
			acpigen_emit_byte(0xa4); /* ReturnOp.  */
			acpigen_emit_namestring("^^XBQC");
			acpigen_pop_len();
		}

		/*
		Method(_DCS, 0)
		{
			Return (^^XDCS(<device number>))
		}
		*/
		acpigen_write_method("_DCS", 0);
		acpigen_emit_byte(0xa4); /* ReturnOp.  */
		acpigen_emit_namestring("^^XDCS");
		acpigen_write_byte(i);
		acpigen_pop_len();

		/*
		Method(_DGS, 0)
		{
			Return (^^XDGS(<device number>))
		}
		*/
		acpigen_write_method("_DGS", 0);
		acpigen_emit_byte(0xa4); /* ReturnOp.  */
		acpigen_emit_namestring("^^XDGS");
		acpigen_write_byte(i);
		acpigen_pop_len();

		/*
		Method(_DSS, 1)
		{
			^^XDSS(0x5a, Arg0)
		}
		*/
		acpigen_write_method("_DSS", 1);
		acpigen_emit_namestring("^^XDSS");
		acpigen_write_byte(i);
		acpigen_emit_byte(0x68); /* Arg0Op.  */
		acpigen_pop_len();

		acpigen_pop_len();
	}

	acpigen_pop_len();
}
