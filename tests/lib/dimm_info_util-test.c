/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/dram/ddr2.h>
#include <device/dram/ddr3.h>
#include <dimm_info_util.h>
#include <spd.h>
#include <tests/test.h>

#define MAX_ALLOWED_MODULE_TYPE 3

static void test_smbios_bus_width_to_spd_width_parametrized(smbios_memory_type ddr_type)
{
	/* Non-ECC variants */
	assert_int_equal(MEMORY_BUS_WIDTH_64, smbios_bus_width_to_spd_width(ddr_type, 64, 64));
	assert_int_equal(MEMORY_BUS_WIDTH_32, smbios_bus_width_to_spd_width(ddr_type, 32, 32));
	assert_int_equal(MEMORY_BUS_WIDTH_16, smbios_bus_width_to_spd_width(ddr_type, 16, 16));
	assert_int_equal(MEMORY_BUS_WIDTH_8, smbios_bus_width_to_spd_width(ddr_type, 8, 8));
	/* Incorrect data width. Fallback to 8-bit */
	assert_int_equal(MEMORY_BUS_WIDTH_8, smbios_bus_width_to_spd_width(ddr_type, 15, 15));

	/* ECC variants */
	uint8_t extension_8bits = SPD_ECC_8BIT;
	if (ddr_type == MEMORY_TYPE_DDR5 || ddr_type == MEMORY_TYPE_LPDDR5)
		extension_8bits = SPD_ECC_8BIT_LP5_DDR5;

	assert_int_equal(MEMORY_BUS_WIDTH_64 | extension_8bits,
			 smbios_bus_width_to_spd_width(ddr_type, 64 + 8, 64));
	assert_int_equal(MEMORY_BUS_WIDTH_32 | extension_8bits,
			 smbios_bus_width_to_spd_width(ddr_type, 32 + 8, 32));
	assert_int_equal(MEMORY_BUS_WIDTH_16 | extension_8bits,
			 smbios_bus_width_to_spd_width(ddr_type, 16 + 8, 16));
	assert_int_equal(MEMORY_BUS_WIDTH_8 | extension_8bits,
			 smbios_bus_width_to_spd_width(ddr_type, 8 + 8, 8));
	/* Incorrect data width. Fallback to 8-bit */
	assert_int_equal(MEMORY_BUS_WIDTH_8 | extension_8bits,
			 smbios_bus_width_to_spd_width(ddr_type, 15 + 8, 15));
}

static void test_smbios_bus_width_to_spd_width(void **state)
{
	smbios_memory_type memory_type[] = {
		MEMORY_TYPE_DDR2,   MEMORY_TYPE_DDR3,	MEMORY_TYPE_DDR4,   MEMORY_TYPE_DDR5,
		MEMORY_TYPE_LPDDR3, MEMORY_TYPE_LPDDR4, MEMORY_TYPE_LPDDR5,
	};

	for (int i = 0; i < ARRAY_SIZE(memory_type); i++) {
		print_message("test_smbios_bus_width_to_spd_width_parametrized(%d)\n",
			      memory_type[i]);
		test_smbios_bus_width_to_spd_width_parametrized(memory_type[i]);
	}
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

static void test_smbios_form_factor_to_spd_mod_type_ddr(smbios_memory_type memory_type)
{
	const smbios_memory_form_factor undefined_factors[] = {
		MEMORY_FORMFACTOR_OTHER, MEMORY_FORMFACTOR_UNKNOWN,
		MEMORY_FORMFACTOR_SIMM,	 MEMORY_FORMFACTOR_SIP,
		MEMORY_FORMFACTOR_CHIP,	 MEMORY_FORMFACTOR_DIP,
		MEMORY_FORMFACTOR_ZIP,	 MEMORY_FORMFACTOR_PROPRIETARY_CARD,
		MEMORY_FORMFACTOR_TSOP,	 MEMORY_FORMFACTOR_ROC,
		MEMORY_FORMFACTOR_SRIMM, MEMORY_FORMFACTOR_FBDIMM,
		MEMORY_FORMFACTOR_DIE,
	};
	for (int i = 0; i < ARRAY_SIZE(undefined_factors); ++i) {
		assert_int_equal(SPD_UNDEFINED, smbios_form_factor_to_spd_mod_type(
							memory_type, undefined_factors[i]));
	}
}

static void test_smbios_form_factor_to_spd_mod_type_ddrx_parametrized(
	smbios_memory_type memory_type, const LargestIntegralType udimm_allowed[],
	const LargestIntegralType rdimm_allowed[], LargestIntegralType expected_module_type)
{
	print_message("%s(%d)\n", __func__, memory_type);

	assert_in_set(smbios_form_factor_to_spd_mod_type(memory_type, MEMORY_FORMFACTOR_DIMM),
		      udimm_allowed, MAX_ALLOWED_MODULE_TYPE);

	assert_in_set(smbios_form_factor_to_spd_mod_type(memory_type, MEMORY_FORMFACTOR_RIMM),
		      rdimm_allowed, MAX_ALLOWED_MODULE_TYPE);

	assert_int_equal(expected_module_type, smbios_form_factor_to_spd_mod_type(
						       memory_type, MEMORY_FORMFACTOR_SODIMM));

	test_smbios_form_factor_to_spd_mod_type_ddr(memory_type);
}

static void test_smbios_form_factor_to_spd_mod_type_lpddrx(smbios_memory_type memory_type)
{
	print_message("%s(%d)\n", __func__, memory_type);
	/* Form factors defined in coreboot */
	assert_int_equal(LPX_SPD_NONDIMM, smbios_form_factor_to_spd_mod_type(
						  memory_type, MEMORY_FORMFACTOR_ROC));
}

static void test_smbios_form_factor_to_spd_mod_type(void **state)
{
	const struct smbios_form_factor_test_info_ddrx {
		smbios_memory_type memory_type;
		const LargestIntegralType udimm_allowed[MAX_ALLOWED_MODULE_TYPE];
		const LargestIntegralType rdimm_allowed[MAX_ALLOWED_MODULE_TYPE];
		LargestIntegralType expected_module_type;
	} ddrx_info[] = {
		{
			.memory_type = MEMORY_TYPE_DDR2,
			.udimm_allowed = {SPD_DDR2_DIMM_TYPE_UDIMM,
					  SPD_DDR2_DIMM_TYPE_MICRO_DIMM,
					  SPD_DDR2_DIMM_TYPE_MINI_UDIMM},
			.rdimm_allowed = {SPD_DDR2_DIMM_TYPE_RDIMM,
					  SPD_DDR2_DIMM_TYPE_MINI_RDIMM},
			.expected_module_type = SPD_DDR2_DIMM_TYPE_SO_DIMM,
		},
		{
			.memory_type = MEMORY_TYPE_DDR3,
			.udimm_allowed = {SPD_DDR3_DIMM_TYPE_UDIMM,
					  SPD_DDR3_DIMM_TYPE_MICRO_DIMM,
					  SPD_DDR3_DIMM_TYPE_MINI_UDIMM},
			.rdimm_allowed = {SPD_DDR3_DIMM_TYPE_RDIMM,
					  SPD_DDR3_DIMM_TYPE_MINI_RDIMM},
			.expected_module_type = SPD_DDR3_DIMM_TYPE_SO_DIMM,
		},
		{
			.memory_type = MEMORY_TYPE_DDR4,
			.udimm_allowed = {DDR4_SPD_UDIMM, DDR4_SPD_MINI_UDIMM},
			.rdimm_allowed = {DDR4_SPD_RDIMM, DDR4_SPD_MINI_RDIMM},
			.expected_module_type = DDR4_SPD_SODIMM,
		},
		{.memory_type = MEMORY_TYPE_DDR5,
		 .udimm_allowed = {DDR5_SPD_UDIMM, DDR5_SPD_MINI_UDIMM},
		 .rdimm_allowed = {DDR5_SPD_RDIMM, DDR5_SPD_MINI_RDIMM},
		 .expected_module_type = DDR5_SPD_SODIMM},
	};

	/* Test for DDRx DIMM Modules */
	for (int i = 0; i < ARRAY_SIZE(ddrx_info); i++)
		test_smbios_form_factor_to_spd_mod_type_ddrx_parametrized(
			ddrx_info[i].memory_type, ddrx_info[i].udimm_allowed,
			ddrx_info[i].rdimm_allowed, ddrx_info[i].expected_module_type);

	smbios_memory_type lpddrx_memory_type[] = {
		MEMORY_TYPE_LPDDR3,
		MEMORY_TYPE_LPDDR4,
		MEMORY_TYPE_LPDDR5,
	};

	/* Test for Lpddrx DIMM Modules */
	for (int i = 0; i < ARRAY_SIZE(lpddrx_memory_type); i++)
		test_smbios_form_factor_to_spd_mod_type_lpddrx(lpddrx_memory_type[i]);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_smbios_bus_width_to_spd_width),
		cmocka_unit_test(test_smbios_memory_size_to_mib),
		cmocka_unit_test(test_smbios_form_factor_to_spd_mod_type),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
