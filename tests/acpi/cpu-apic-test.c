/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <tests/test.h>

struct device *all_devices;
static struct device cpus[5];
static struct device cluster = { .path.type = DEVICE_PATH_CPU_CLUSTER };
static struct bus cpu_bus = { .dev = &cluster };

static int setup(void **state)
{
	const uint32_t ids[] = { 5, 2, 4, 3, 8 };
	const unsigned int threads[] = { 1, 0, 0, 1, 0 };

	memset(cpus, 0, sizeof(cpus));
	for (size_t i = 0; i < ARRAY_SIZE(cpus); i++) {
		cpus[i].path.type = DEVICE_PATH_APIC;
		cpus[i].path.apic.apic_id = ids[i];
		cpus[i].path.apic.thread_id = threads[i];
		cpus[i].upstream = &cpu_bus;
		cpus[i].enabled = i != 4;
		cpus[i].next = i + 1 < ARRAY_SIZE(cpus) ? &cpus[i + 1] : NULL;
	}
	all_devices = cpus;
	return 0;
}

static void test_thread_order(void **state)
{
	const uint32_t expected[] = { 2, 4, 3, 5 };
	uint32_t ids[ARRAY_SIZE(cpus)];
	size_t count;

	count = acpi_get_cpu_apic_ids(ids, ARRAY_SIZE(ids));
	assert_int_equal(count, ARRAY_SIZE(expected));
	assert_memory_equal(ids, expected, sizeof(expected));
}

static void test_keep_valid_cpus(void **state)
{
	uint32_t ids[ARRAY_SIZE(cpus)];
	size_t count = 99;
	const uint32_t truncated[] = { 2, 4, 5 };
	const uint32_t unique[] = { 2, 4, 3 };

	count = acpi_get_cpu_apic_ids(ids, 3);
	assert_int_equal(count, ARRAY_SIZE(truncated));
	assert_memory_equal(ids, truncated, sizeof(truncated));
	cpus[0].path.apic.apic_id = cpus[1].path.apic.apic_id;
	count = acpi_get_cpu_apic_ids(ids, ARRAY_SIZE(ids));
	assert_int_equal(count, ARRAY_SIZE(unique));
	assert_memory_equal(ids, unique, sizeof(unique));
}

static void test_extra_threads_keep_supported_cpus(void **state)
{
	const uint32_t expected[] = { 2, 4, 3, 5 };
	uint32_t ids[ARRAY_SIZE(expected)];
	size_t count;

	cpus[4].enabled = true;
	for (unsigned int thread = 2; thread <= 3; thread++) {
		cpus[4].path.apic.thread_id = thread;
		count = acpi_get_cpu_apic_ids(ids, ARRAY_SIZE(ids));
		assert_int_equal(count, ARRAY_SIZE(expected));
		assert_memory_equal(ids, expected, sizeof(expected));
	}
}

static void test_empty_output(void **state)
{
	uint32_t ids[] = { 0xa5a5a5a5 };

	assert_int_equal(acpi_get_cpu_apic_ids(ids, 0), 0);
	assert_int_equal(ids[0], 0xa5a5a5a5);
	all_devices = NULL;
	assert_int_equal(acpi_get_cpu_apic_ids(ids, ARRAY_SIZE(ids)), 0);
	assert_int_equal(ids[0], 0xa5a5a5a5);
}

static void test_signed_apic_order(void **state)
{
	uint32_t ids[ARRAY_SIZE(cpus)];
	const uint32_t expected[] = { 0x80000000, 4, 3, 5 };

	cpus[1].path.apic.apic_id = 0x80000000;
	assert_int_equal(acpi_get_cpu_apic_ids(ids, ARRAY_SIZE(ids)), ARRAY_SIZE(expected));
	assert_memory_equal(ids, expected, sizeof(expected));
	assert_int_equal(acpi_get_cpu_apic_ids(ids, 2), 2);
	assert_memory_equal(ids, expected, 2 * sizeof(*ids));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_signed_apic_order, setup),
		cmocka_unit_test_setup(test_thread_order, setup),
		cmocka_unit_test_setup(test_keep_valid_cpus, setup),
		cmocka_unit_test_setup(test_extra_threads_keep_supported_cpus, setup),
		cmocka_unit_test_setup(test_empty_output, setup),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
