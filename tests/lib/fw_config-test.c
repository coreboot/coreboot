/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>

#include "../lib/fw_config.c"

#define FW_CONFIG_FIELD_FIELD_A_NAME "FIELD_A"
#define FW_CONFIG_FIELD_FIELD_A_MASK 0x1ULL
#define FW_CONFIG_FIELD_FIELD_A_OPTION_OPT_0_NAME "OPT_0"
#define FW_CONFIG_FIELD_FIELD_A_OPTION_OPT_0_VALUE 0x0ULL
#define FW_CONFIG_FIELD_FIELD_A_OPTION_OPT_1_NAME "OPT_1"
#define FW_CONFIG_FIELD_FIELD_A_OPTION_OPT_1_VALUE 0x1ULL

#define FW_CONFIG_FIELD_FIELD_B_NAME "FIELD_B"
#define FW_CONFIG_FIELD_FIELD_B_MASK 0x6ULL
#define FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_0_NAME "OPT_0"
#define FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_0_VALUE 0x0ULL
#define FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_1_NAME "OPT_1"
#define FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_1_VALUE 0x4ULL
#define FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_2_NAME "OPT_2"
#define FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_2_VALUE 0x6ULL

#define TEST_FIELD_A	FW_CONFIG_FIELD(FIELD_A)
#define TEST_FIELD_B	FW_CONFIG_FIELD(FIELD_B)

#define TEST_VALUE_A0	FW_CONFIG_VALUE(FIELD_A, OPT_0)
#define TEST_VALUE_A1	FW_CONFIG_VALUE(FIELD_A, OPT_1)
#define TEST_VALUE_B0	FW_CONFIG_VALUE(FIELD_B, OPT_0)
#define TEST_VALUE_B1	FW_CONFIG_VALUE(FIELD_B, OPT_1)
#define TEST_VALUE_B2	FW_CONFIG_VALUE(FIELD_B, OPT_2)

#define TEST_CONFIG_A0	FW_CONFIG(FIELD_A, OPT_0)
#define TEST_CONFIG_A1	FW_CONFIG(FIELD_A, OPT_1)
#define TEST_CONFIG_B0	FW_CONFIG(FIELD_B, OPT_0)
#define TEST_CONFIG_B1	FW_CONFIG(FIELD_B, OPT_1)
#define TEST_CONFIG_B2	FW_CONFIG(FIELD_B, OPT_2)

static void (*override_cb)(void);

int google_chromeec_cbi_get_fw_config(uint64_t *fw_config)
{
	int ret = mock_type(int);

	if (ret == 0)
		*fw_config = mock_type(uint64_t);
	return ret;
}

void fw_config_mainboard_override(void)
{
	if (override_cb)
		override_cb();
}

static void override_field_a_opt_1(void)
{
	fw_config_override_field(TEST_FIELD_A, TEST_VALUE_A1);
}

static void override_field_b_opt_1(void)
{
	fw_config_override_field(TEST_FIELD_B, TEST_VALUE_B1);
}

static void override_if_unprovisioned(void)
{
	if (!fw_config_is_provisioned())
		fw_config_override_field(TEST_FIELD_B, TEST_VALUE_B1);
}

static int setup_fw_config(void **state)
{
	fw_config_state = FW_CONFIG_UNINITIALIZED;
	fw_config_value = 0;
	fw_config_override_mask = 0;
	override_cb = NULL;
	return 0;
}

static void test_fw_config_provisioned_no_override(void **state)
{
	will_return(google_chromeec_cbi_get_fw_config, 0);
	will_return(google_chromeec_cbi_get_fw_config,
		    FW_CONFIG_FIELD_FIELD_A_OPTION_OPT_1_VALUE |
		    FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_1_VALUE);

	assert_true(fw_config_is_provisioned());
	assert_int_equal(0x5, fw_config_get());
	assert_int_equal(0, fw_config_get_override_mask());
	assert_int_equal(TEST_VALUE_A1, fw_config_get_field(TEST_FIELD_A));
	assert_int_equal(TEST_VALUE_B1, fw_config_get_field(TEST_FIELD_B));
	assert_false(fw_config_probe(TEST_CONFIG_A0));
	assert_true(fw_config_probe(TEST_CONFIG_A1));
	assert_false(fw_config_probe(TEST_CONFIG_B0));
	assert_true(fw_config_probe(TEST_CONFIG_B1));
	assert_false(fw_config_probe(TEST_CONFIG_B2));
}

static void test_fw_config_unprovisioned_no_override(void **state)
{
	will_return(google_chromeec_cbi_get_fw_config, -1);

	assert_false(fw_config_is_provisioned());
	assert_int_equal(UNDEFINED_FW_CONFIG, fw_config_get());
	assert_int_equal(0, fw_config_get_override_mask());
	assert_int_equal(UNDEFINED_FW_CONFIG, fw_config_get_field(TEST_FIELD_A));
	assert_int_equal(UNDEFINED_FW_CONFIG, fw_config_get_field(TEST_FIELD_B));
	assert_false(fw_config_probe(TEST_CONFIG_A0));
	assert_false(fw_config_probe(TEST_CONFIG_A1));
	assert_false(fw_config_probe(TEST_CONFIG_B0));
	assert_false(fw_config_probe(TEST_CONFIG_B1));
	assert_false(fw_config_probe(TEST_CONFIG_B2));
}

static void test_fw_config_unprovisioned_with_override(void **state)
{
	will_return(google_chromeec_cbi_get_fw_config, -1);
	override_cb = override_field_b_opt_1;

	assert_false(fw_config_is_provisioned());
	assert_int_not_equal(UNDEFINED_FW_CONFIG, fw_config_get());
	assert_int_equal(FW_CONFIG_FIELD_FIELD_B_MASK,
			 fw_config_get_override_mask());
	assert_int_equal(TEST_VALUE_B1, fw_config_get_field(TEST_FIELD_B));
	assert_false(fw_config_probe(TEST_CONFIG_B0));
	assert_true(fw_config_probe(TEST_CONFIG_B1));
	assert_false(fw_config_probe(TEST_CONFIG_B2));

	/* Non-overridden field must still be treated as unprovisioned. */
	assert_int_equal(UNDEFINED_FW_CONFIG, fw_config_get_field(TEST_FIELD_A));
	assert_false(fw_config_probe(TEST_CONFIG_A0));
	assert_false(fw_config_probe(TEST_CONFIG_A1));
}

static void test_fw_config_unprovisioned_override_all_ones(void **state)
{
	will_return(google_chromeec_cbi_get_fw_config, -1);
	override_cb = override_field_a_opt_1;

	/*
	 * Overriding a 1-bit field to 1 when unprovisioned leaves
	 * fw_config_get() equal to UNDEFINED_FW_CONFIG (~0ULL), but
	 * fw_config_get_override_mask() tracks that FIELD_A was overridden.
	 */
	assert_false(fw_config_is_provisioned());
	assert_int_equal(UNDEFINED_FW_CONFIG, fw_config_get());
	assert_int_equal(FW_CONFIG_FIELD_FIELD_A_MASK,
			 fw_config_get_override_mask());
	assert_int_equal(TEST_VALUE_A1, fw_config_get_field(TEST_FIELD_A));
	assert_false(fw_config_probe(TEST_CONFIG_A0));
	assert_true(fw_config_probe(TEST_CONFIG_A1));

	/* Non-overridden FIELD_B must still fail. */
	assert_int_equal(UNDEFINED_FW_CONFIG, fw_config_get_field(TEST_FIELD_B));
	assert_false(fw_config_probe(TEST_CONFIG_B2));
}

static void test_fw_config_provisioned_with_override(void **state)
{
	will_return(google_chromeec_cbi_get_fw_config, 0);
	will_return(google_chromeec_cbi_get_fw_config,
		    FW_CONFIG_FIELD_FIELD_A_OPTION_OPT_0_VALUE |
		    FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_1_VALUE);
	override_cb = override_field_a_opt_1;

	assert_true(fw_config_is_provisioned());
	assert_int_equal(0x5, fw_config_get());
	assert_int_equal(FW_CONFIG_FIELD_FIELD_A_MASK,
			 fw_config_get_override_mask());
	assert_int_equal(TEST_VALUE_A1, fw_config_get_field(TEST_FIELD_A));
	assert_int_equal(TEST_VALUE_B1, fw_config_get_field(TEST_FIELD_B));
	assert_true(fw_config_probe(TEST_CONFIG_A1));
	assert_true(fw_config_probe(TEST_CONFIG_B1));
}

static void test_fw_config_is_provisioned_in_mainboard_override(void **state)
{
	/* Unprovisioned: fallback override in hook should be applied. */
	will_return(google_chromeec_cbi_get_fw_config, -1);
	override_cb = override_if_unprovisioned;

	assert_false(fw_config_is_provisioned());
	assert_int_equal(FW_CONFIG_FIELD_FIELD_B_MASK,
			 fw_config_get_override_mask());
	assert_int_equal(TEST_VALUE_B1, fw_config_get_field(TEST_FIELD_B));

	/* Provisioned: fallback override in hook should be skipped. */
	setup_fw_config(state);
	will_return(google_chromeec_cbi_get_fw_config, 0);
	will_return(google_chromeec_cbi_get_fw_config,
		    FW_CONFIG_FIELD_FIELD_B_OPTION_OPT_2_VALUE);
	override_cb = override_if_unprovisioned;

	assert_true(fw_config_is_provisioned());
	assert_int_equal(0, fw_config_get_override_mask());
	assert_int_equal(TEST_VALUE_B2, fw_config_get_field(TEST_FIELD_B));
}

static void test_fw_config_override_outside_mainboard_override(void **state)
{
	/* Calling fw_config_override_field before setup must assert. */
	expect_assert_failure(fw_config_override_field(TEST_FIELD_A, TEST_VALUE_A1));

	will_return(google_chromeec_cbi_get_fw_config, 0);
	will_return(google_chromeec_cbi_get_fw_config, 0);
	assert_true(fw_config_is_provisioned());

	/* Calling fw_config_override_field after setup must assert. */
	expect_assert_failure(fw_config_override_field(TEST_FIELD_A, TEST_VALUE_A1));
}

static void test_fw_config_probe_dev(void **state)
{
	const struct fw_config *match = NULL;
	struct fw_config probe_field_a_opt_1[] = {
		*TEST_CONFIG_A1,
		{ 0 },
	};
	struct fw_config probe_field_b_opt_1[] = {
		*TEST_CONFIG_B1,
		{ 0 },
	};
	struct fw_config probe_field_b_opt_2[] = {
		*TEST_CONFIG_B2,
		{ 0 },
	};
	const struct device dev_unprovisioned_en = {
		.probe_list = probe_field_b_opt_1,
		.enable_on_unprovisioned_fw_config = 1,
	};
	const struct device dev_field_a = {
		.probe_list = probe_field_a_opt_1,
	};
	const struct device dev_field_b = {
		.probe_list = probe_field_b_opt_2,
	};

	will_return(google_chromeec_cbi_get_fw_config, -1);
	override_cb = override_field_a_opt_1;

	/*
	 * Device with enable_on_unprovisioned_fw_config must be enabled when
	 * CBI is unprovisioned even if another field was overridden.
	 */
	assert_true(fw_config_probe_dev(&dev_unprovisioned_en, &match));
	assert_null(match);

	/* Device probing overridden FIELD_A = OPT_1 should match. */
	assert_true(fw_config_probe_dev(&dev_field_a, &match));
	assert_ptr_equal(&probe_field_a_opt_1[0], match);

	/* Device probing non-overridden FIELD_B = OPT_2 should not match. */
	assert_false(fw_config_probe_dev(&dev_field_b, &match));
	assert_null(match);
}

#define TEST(func) cmocka_unit_test_setup(func, setup_fw_config)

int main(void)
{
	const struct CMUnitTest tests[] = {
		TEST(test_fw_config_provisioned_no_override),
		TEST(test_fw_config_unprovisioned_no_override),
		TEST(test_fw_config_unprovisioned_with_override),
		TEST(test_fw_config_unprovisioned_override_all_ones),
		TEST(test_fw_config_provisioned_with_override),
		TEST(test_fw_config_is_provisioned_in_mainboard_override),
		TEST(test_fw_config_override_outside_mainboard_override),
		TEST(test_fw_config_probe_dev),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
