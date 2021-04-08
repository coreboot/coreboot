/* SPDX-License-Identifier: GPL-2.0-only */

#include <dimm_info_util.h>
#include <spd.h>
#include <tests/test.h>

static void test_smbios_bus_width_to_spd_width(void **state)
{
	/* Non-ECC variants */
	assert_int_equal(MEMORY_BUS_WIDTH_64, smbios_bus_width_to_spd_width(64, 64));
	assert_int_equal(MEMORY_BUS_WIDTH_32, smbios_bus_width_to_spd_width(32, 32));
	assert_int_equal(MEMORY_BUS_WIDTH_16, smbios_bus_width_to_spd_width(16, 16));
	assert_int_equal(MEMORY_BUS_WIDTH_8, smbios_bus_width_to_spd_width(8, 8));
	/* Incorrect data width. Fallback to 8-bit */
	assert_int_equal(MEMORY_BUS_WIDTH_8, smbios_bus_width_to_spd_width(15, 15));

	/* ECC variants */
	assert_int_equal(MEMORY_BUS_WIDTH_64 | SPD_ECC_8BIT,
			smbios_bus_width_to_spd_width(64 + 8, 64));
	assert_int_equal(MEMORY_BUS_WIDTH_32 | SPD_ECC_8BIT,
			smbios_bus_width_to_spd_width(32 + 8, 32));
	assert_int_equal(MEMORY_BUS_WIDTH_16 | SPD_ECC_8BIT,
			smbios_bus_width_to_spd_width(16 + 8, 16));
	assert_int_equal(MEMORY_BUS_WIDTH_8 | SPD_ECC_8BIT,
			smbios_bus_width_to_spd_width(8 + 8, 8));
	/* Incorrect data width. Fallback to 8-bit */
	assert_int_equal(MEMORY_BUS_WIDTH_8 | SPD_ECC_8BIT,
			smbios_bus_width_to_spd_width(15 + 8, 15));
}

static void test_smbios_memory_size_to_mib(void **state)
{
	uint32_t extended_size;
	uint16_t memory_size;

	/* Unknown memory size */
	assert_int_equal(0, smbios_memory_size_to_mib(0xFFFF, 0));
	assert_int_equal(0, smbios_memory_size_to_mib(0xFFFF, 0xFFFF));
	assert_int_equal(0, smbios_memory_size_to_mib(0xFFFF, 87642));

	/* 32GiB - 1MiB */
	extended_size = 0;
	assert_int_equal(extended_size, smbios_memory_size_to_mib(0x7FFF, extended_size));
	extended_size = 0xFFFFFFFF;
	assert_int_equal(extended_size, smbios_memory_size_to_mib(0x7FFF, extended_size));
	extended_size = 0xDEDE6666;
	assert_int_equal(extended_size, smbios_memory_size_to_mib(0x7FFF, extended_size));

	/* Memory size in KiB when MSB is flipped */
	memory_size = 0x0 & 0x8000; /* Zero bytes */
	assert_int_equal(0, smbios_memory_size_to_mib(memory_size, 0));
	assert_int_equal(0, smbios_memory_size_to_mib(memory_size, 0xFFFFFFFF));
	assert_int_equal(0, smbios_memory_size_to_mib(memory_size, 2345568));
	memory_size = (31 * KiB) | 0x8000;
	assert_int_equal(31, smbios_memory_size_to_mib(memory_size, 0));
	assert_int_equal(31, smbios_memory_size_to_mib(memory_size, 0xFFFFFFFF));
	assert_int_equal(31, smbios_memory_size_to_mib(memory_size, 72594344));

	/* Value in MiB Only when memory size is not 0xFFFF and 0x7FFF and MSB is not set */
	memory_size = 32766; /* value in MiB */
	assert_int_equal(memory_size, smbios_memory_size_to_mib(memory_size, 0));
	assert_int_equal(memory_size, smbios_memory_size_to_mib(memory_size, 0xFFFFFFFF));
	assert_int_equal(memory_size, smbios_memory_size_to_mib(memory_size, 694735));
}

static void test_smbios_form_factor_to_spd_mod_type(void **state)
{
	/* Form factors defined in coreboot */
	const LargestIntegralType udimm_allowed[] = {
		SPD_UDIMM, SPD_MICRO_DIMM, SPD_MINI_UDIMM,
	};
	assert_in_set(smbios_form_factor_to_spd_mod_type(MEMORY_FORMFACTOR_DIMM),
			udimm_allowed, ARRAY_SIZE(udimm_allowed));

	const LargestIntegralType rdimm_allowed[] = { SPD_RDIMM, SPD_MINI_RDIMM };
	assert_in_set(smbios_form_factor_to_spd_mod_type(MEMORY_FORMFACTOR_RIMM),
			rdimm_allowed, ARRAY_SIZE(rdimm_allowed));

	assert_int_equal(SPD_SODIMM,
			smbios_form_factor_to_spd_mod_type(MEMORY_FORMFACTOR_SODIMM));

	const smbios_memory_form_factor undefined_factors[] = {
		MEMORY_FORMFACTOR_OTHER,
		MEMORY_FORMFACTOR_UNKNOWN,
		MEMORY_FORMFACTOR_SIMM,
		MEMORY_FORMFACTOR_SIP,
		MEMORY_FORMFACTOR_CHIP,
		MEMORY_FORMFACTOR_DIP,
		MEMORY_FORMFACTOR_ZIP,
		MEMORY_FORMFACTOR_PROPRIETARY_CARD,
		MEMORY_FORMFACTOR_TSOP,
		MEMORY_FORMFACTOR_ROC,
		MEMORY_FORMFACTOR_SRIMM,
		MEMORY_FORMFACTOR_FBDIMM,
		MEMORY_FORMFACTOR_DIE,
	};
	for (int i = 0; i < ARRAY_SIZE(undefined_factors); ++i) {
		assert_int_equal(SPD_UNDEFINED,
				smbios_form_factor_to_spd_mod_type(undefined_factors[i]));
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_smbios_bus_width_to_spd_width),
		cmocka_unit_test(test_smbios_memory_size_to_mib),
		cmocka_unit_test(test_smbios_form_factor_to_spd_mod_type),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
