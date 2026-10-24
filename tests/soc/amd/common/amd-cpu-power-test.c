/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/cpu.h>
#include <cpu/amd/msr.h>
#include <device/device.h>
#include <soc/msr.h>
#include <stdio.h>
#include <string.h>
#include <tests/test.h>

static uint8_t aml[2 * MiB];
static unsigned int pstate_count;
static int cpu_count = CONFIG_MAX_CPUS;
static unsigned int threads_per_core = 2;
struct device *all_devices;
static struct device cpus[CONFIG_MAX_CPUS];
static struct device cluster = { .path.type = DEVICE_PATH_CPU_CLUSTER };
static struct bus cpu_bus = { .dev = &cluster };

static int setup(void **state)
{
	cpu_count = CONFIG_MAX_CPUS;
	threads_per_core = 2;
	memset(cpus, 0, sizeof(cpus));
	for (size_t i = 0; i < ARRAY_SIZE(cpus); i++) {
		cpus[i].path.type = DEVICE_PATH_APIC;
		cpus[i].path.apic.apic_id = i;
		cpus[i].path.apic.thread_id = i % 2;
		cpus[i].upstream = &cpu_bus;
		cpus[i].enabled = true;
		cpus[i].next = i + 1 < ARRAY_SIZE(cpus) ? &cpus[i + 1] : NULL;
	}
	all_devices = cpus;
	return 0;
}

void generate_cpu_entries(const struct device *device);

msr_t cpu_power_test_rdmsr(unsigned int index)
{
	if (index == MSR_CSTATE_ADDRESS)
		return (msr_t){ .lo = 0x841 };
	if (index == PS_LIM_REG)
		return (msr_t){ .lo = (pstate_count - 1) << PS_MAX_VAL_SHFT };
	assert_true(index >= PSTATE_0_MSR && index < PSTATE_0_MSR + pstate_count);
	const union pstate_msr pstate = { .pstate_en = 1, .idd_value = 32, .idd_div = 1 };
	return (msr_t){ .raw = pstate.raw };
}

int get_cpu_count(void)
{
	return cpu_count;
}

unsigned int get_threads_per_core(void)
{
	return threads_per_core;
}

uint32_t get_pstate_0_reg(void)
{
	return 0;
}

uint32_t get_pstate_latency(void)
{
	return 10;
}

uint32_t get_pstate_core_freq(union pstate_msr pstate)
{
	return 2400;
}

uint32_t get_pstate_core_uvolts(union pstate_msr pstate)
{
	return 1000000;
}

const acpi_cstate_t *get_cstate_config_data(size_t *count)
{
	/* The common generator fills the register resources. */
	static const acpi_cstate_t states[] = {
		{ .ctype = 1, .latency = 1, .power = 0 },
		{ .ctype = 2, .latency = 0x64, .power = 0 },
	};
	*count = ARRAY_SIZE(states);
	return states;
}

enum cb_err get_ccx_cppc_min_frequency(uint32_t *frequency)
{
	*frequency = 900;
	return CB_SUCCESS;
}

enum cb_err get_ccx_cppc_nom_frequency(uint32_t *frequency)
{
	*frequency = 2400;
	return CB_SUCCESS;
}

static size_t package_length(const uint8_t *field, const uint8_t **body)
{
	const unsigned int bytes = 1 + (field[0] >> 6);
	size_t length = field[0] & (bytes == 1 ? 0x3f : 0x0f);
	for (unsigned int i = 1; i < bytes; i++)
		length |= (size_t)field[i] << (4 + 8 * (i - 1));
	*body = field + bytes;
	return length;
}

static void measure_cpu_aml(unsigned int states)
{
	pstate_count = states;
	memset(aml, 0, sizeof(aml));
	acpigen_set_current((char *)aml);
	generate_cpu_entries(NULL);
	const uint8_t *end = (const uint8_t *)acpigen_get_current();
	assert_true(end > aml && end < aml + sizeof(aml));
	const uint8_t *next = aml;
	for (unsigned int i = 0; i < CONFIG_MAX_CPUS; i++) {
		assert_int_equal(next[0], 0x5b);
		assert_int_equal(next[1], 0x82);
		const uint8_t *body;
		size_t length = package_length(next + 2, &body);
		assert_true(next + 2 + length <= end);
		static const char hex[] = "0123456789ABCDEF";
		const uint8_t name[] = {
			0x5c, 0x2e, '_', 'S', 'B', '_', 'C',
			hex[(i >> 8) & 15], hex[(i >> 4) & 15], hex[i & 15],
		};
		assert_memory_equal(body, name, sizeof(name));
		next += 2 + length;
	}
	assert_memory_equal(next, "\x08PPKG", 5);
	next += 5;
	assert_int_equal(*next++, 0x13);
	const uint8_t *body;
	size_t length = package_length(next, &body);
	assert_ptr_equal(next + length, end);
	assert_int_equal(body[0], 0x0b);
	assert_int_equal(body[1] | (unsigned int)body[2] << 8, CONFIG_MAX_CPUS);
	assert_ptr_equal(body + 3 + 10 * CONFIG_MAX_CPUS, end);
	print_message("CPU AML: %u CPUs, %u P-states, 2 C-states, CPPCv3: %zu bytes\n",
		      CONFIG_MAX_CPUS, states, (size_t)(end - aml));
}

static void test_one_pstate(void **state)
{
	measure_cpu_aml(1);
}

static void test_three_pstates(void **state)
{
	measure_cpu_aml(3);
}

static void test_eight_pstates(void **state)
{
	measure_cpu_aml(8);
}

static void test_invalid_cpu_count(void **state)
{
	pstate_count = 1;
	for (cpu_count = -1; cpu_count <= 0; cpu_count++) {
		acpigen_set_current((char *)aml);
		generate_cpu_entries(NULL);
		assert_ptr_equal(acpigen_get_current(), aml);
	}
	cpu_count = CONFIG_MAX_CPUS;
}

static const uint8_t *aml_package(const uint8_t **data, const uint8_t *limit)
{
	const uint8_t *field = *data;
	assert_true(field < limit);
	const unsigned int bytes = 1 + (field[0] >> 6);
	assert_true(field + bytes <= limit);
	size_t length = field[0] & (bytes == 1 ? 0x3f : 0x0f);
	for (unsigned int i = 1; i < bytes; i++)
		length |= (size_t)field[i] << (4 + 8 * (i - 1));
	assert_true(length >= bytes && length <= (size_t)(limit - field));
	*data += bytes;
	return field + length;
}

static uint64_t aml_integer(const uint8_t **data, const uint8_t *limit)
{
	assert_true(*data < limit);
	const uint8_t opcode = *(*data)++;
	if (opcode == ZERO_OP || opcode == ONE_OP)
		return opcode;
	unsigned int bytes;
	switch (opcode) {
	case BYTE_PREFIX:
		bytes = 1;
		break;
	case WORD_PREFIX:
		bytes = 2;
		break;
	case DWORD_PREFIX:
		bytes = 4;
		break;
	case QWORD_PREFIX:
		bytes = 8;
		break;
	default:
		fail_msg("Unexpected AML integer opcode %#x", opcode);
		return 0;
	}
	assert_true(*data + bytes <= limit);
	uint64_t value = 0;
	for (unsigned int i = 0; i < bytes; i++)
		value |= (uint64_t)*(*data)++ << (i * 8);
	return value;
}

static const uint8_t *aml_name(const uint8_t *data, const uint8_t *end, const char *name)
{
	for (; end - data >= 5; data++) {
		if (data[0] == NAME_OP && !memcmp(data + 1, name, 4))
			return data + 5;
	}
	fail_msg("Missing AML object %.4s", name);
	return NULL;
}

static void check_power_domain(const uint8_t *data, const uint8_t *limit,
			       uint32_t domain, uint32_t siblings, bool cstate)
{
	assert_int_equal(*data++, PACKAGE_OP);
	const uint8_t *outer_end = aml_package(&data, limit);
	assert_int_equal(*data++, 1);
	assert_int_equal(*data++, PACKAGE_OP);
	const uint8_t *inner_end = aml_package(&data, outer_end);
	assert_int_equal(*data++, cstate ? 6 : 5);
	assert_int_equal(aml_integer(&data, inner_end), cstate ? 6 : 5);
	assert_int_equal(aml_integer(&data, inner_end), 0);
	const bool global = !cstate && !CONFIG(ACPI_SSDT_PSD_INDEPENDENT);
	assert_int_equal(aml_integer(&data, inner_end), global ? 0 : domain);
	assert_int_equal(aml_integer(&data, inner_end), cstate ? CSD_HW_ALL : global ? SW_ALL : HW_ALL);
	assert_int_equal(aml_integer(&data, inner_end), global ? cpu_count : siblings);
	if (cstate)
		assert_int_equal(aml_integer(&data, inner_end), 0);
	assert_ptr_equal(data, inner_end);
	assert_ptr_equal(inner_end, outer_end);
}

static void test_hardware_siblings(void **state)
{
	const uint32_t ids[] = {0, 1, 0x102, 0x1000, 0x1001, 0x10202, 0x10203};
	const uint32_t domains[] = {0, 0x102, 0x1000, 0x10202, 0, 0x1000, 0x10202};
	const uint32_t siblings[] = {2, 1, 2, 2, 2, 2, 2};

	cpu_count = ARRAY_SIZE(ids);
	cpus[cpu_count - 1].next = NULL;
	for (size_t i = 0; i < ARRAY_SIZE(ids); i++) {
		cpus[i].path.apic.apic_id = ids[i];
		cpus[i].path.apic.thread_id = ids[i] & 1;
	}
	pstate_count = 1;
	acpigen_set_current((char *)aml);
	generate_cpu_entries(NULL);
	const uint8_t *data = aml, *end = (const uint8_t *)acpigen_get_current();
	for (size_t uid = 0; uid < ARRAY_SIZE(ids); uid++) {
		assert_int_equal(*data++, EXT_OP_PREFIX);
		assert_int_equal(*data++, DEVICE_OP);
		const uint8_t *device_end = aml_package(&data, end);
		check_power_domain(aml_name(data, device_end, "_PSD"), device_end,
				   domains[uid], siblings[uid], false);
		check_power_domain(aml_name(data, device_end, "_CSD"), device_end,
				   domains[uid], siblings[uid], true);
		data = device_end;
	}
}

static void test_smt_disabled(void **state)
{
	threads_per_core = 1;
	cpu_count = 3;
	cpus[2].next = NULL;
	for (size_t i = 0; i < 3; i++) {
		cpus[i].path.apic.apic_id = 2 * i;
		cpus[i].path.apic.thread_id = 0;
	}
	pstate_count = 1;
	acpigen_set_current((char *)aml);
	generate_cpu_entries(NULL);
	const uint8_t *data = aml, *end = (const uint8_t *)acpigen_get_current();
	for (size_t uid = 0; uid < 3; uid++) {
		assert_int_equal(*data++, EXT_OP_PREFIX);
		assert_int_equal(*data++, DEVICE_OP);
		const uint8_t *device_end = aml_package(&data, end);
		check_power_domain(aml_name(data, device_end, "_PSD"), device_end,
				   2 * uid, 1, false);
		check_power_domain(aml_name(data, device_end, "_CSD"), device_end,
				   2 * uid, 1, true);
		data = device_end;
	}
}

static void test_missing_power_topology(void **state)
{
	pstate_count = 1;
	for (unsigned int missing = 0; missing < 2; missing++) {
		threads_per_core = missing ? 0 : 2;
		all_devices = missing ? cpus : NULL;
		acpigen_set_current((char *)aml);
		generate_cpu_entries(NULL);
		const uint8_t *end = (const uint8_t *)acpigen_get_current();
		assert_true(end > aml);
		for (const uint8_t *data = aml; end - data >= 5; data++) {
			if (data[0] == NAME_OP) {
				if (CONFIG(ACPI_SSDT_PSD_INDEPENDENT))
					assert_true(memcmp(data + 1, "_PSD", 4));
				else if (!memcmp(data + 1, "_PSD", 4))
					check_power_domain(data + 5, end, 0, cpu_count, false);
				assert_true(memcmp(data + 1, "_CSD", 4));
			}
		}
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_hardware_siblings, setup),
		cmocka_unit_test_setup(test_smt_disabled, setup),
		cmocka_unit_test_setup(test_missing_power_topology, setup),
		cmocka_unit_test_setup(test_invalid_cpu_count, setup),
		cmocka_unit_test_setup(test_one_pstate, setup),
		cmocka_unit_test_setup(test_three_pstates, setup),
		cmocka_unit_test_setup(test_eight_pstates, setup),
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
