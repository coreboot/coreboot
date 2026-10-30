/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <types.h>
#include <tests/test.h>
#include <acpi/acpigen.h>

#define ACPIGEN_TEST_BUFFER_SZ (512 * KiB)

/* Returns AML package length. Works with normal and extended packages.
   This implementation is independent from acpigen.c implementation of package length. */
static u32 decode_package_length(const char *ptr)
{
	const u8 *aml = (u8 *)ptr;
	const u32 offset = (aml[0] == EXT_OP_PREFIX ? 2 : 1);
	u32 byte_zero_mask = 0x3F; /* Bits [0:5] */
	u32 byte_count = aml[offset] >> 6;
	u32 package_length = 0;

	while (byte_count) {
		package_length |= aml[offset + byte_count] << ((byte_count << 3) - 4);
		byte_zero_mask = 0x0F; /* Use bits [0:3] of byte 0 */
		byte_count--;
	}

	package_length |= (aml[offset] & byte_zero_mask);

	return package_length;
}

static u32 get_current_block_length(const char *base)
{
	const u32 offset = (base[0] == EXT_OP_PREFIX ? 2 : 1);

	return ((uintptr_t)acpigen_get_current() - ((uintptr_t)base + offset));
}

static int setup_acpigen(void **state)
{
	void *buffer = malloc(ACPIGEN_TEST_BUFFER_SZ);

	if (buffer == NULL)
		return -1;

	memset(buffer, 0, ACPIGEN_TEST_BUFFER_SZ);

	*state = buffer;
	return 0;
}

static int teardown_acpigen(void **state)
{
	free(*state);
	return 0;
}

static void test_acpigen_single_if(void **state)
{
	char *acpigen_buf = *state;
	u32 if_package_length = 0;
	u32 block_length = 0;

	acpigen_set_current(acpigen_buf);

	/* Create dummy AML */
	acpigen_write_if_lequal_op_int(LOCAL0_OP, 64);

	for (int i = 0; i < 20; ++i)
		acpigen_write_store_ops(ZERO_OP, LOCAL1_OP);

	/* Close if */
	acpigen_pop_len();

	if_package_length = decode_package_length(acpigen_buf);
	block_length = get_current_block_length(acpigen_buf);
	assert_int_equal(if_package_length, block_length);
}

static void create_nested_ifs_recursive(size_t stack_len[], u32 i, u32 n)
{
	if (i >= n)
		return;

	char *const start = acpigen_get_current();
	acpigen_write_if_and(LOCAL0_OP, ZERO_OP);

	for (int k = 0; k < 3; ++k)
		acpigen_write_store_ops(ZERO_OP, LOCAL1_OP);

	create_nested_ifs_recursive(stack_len, i + 1, n);

	acpigen_pop_len();
	stack_len[i] = acpigen_get_current() - start;
}

static void test_acpigen_nested_ifs(void **state)
{
	char *acpigen_buf = *state;
	const size_t nesting_level = 8;
	size_t block_len[8] = {0};

	acpigen_set_current(acpigen_buf);

	create_nested_ifs_recursive(block_len, 0, nesting_level);

	for (int i = 0, j = 0; i < nesting_level; ++i, ++j) {
		/* Find next if op */
		for (; j < ACPIGEN_TEST_BUFFER_SZ; ++j) {
			if ((u8)acpigen_buf[j] == IF_OP)
				break;
		}
		assert_int_equal(decode_package_length(acpigen_buf + j), block_len[i] - 1);
	}
}

static void test_acpigen_write_package(void **state)
{
	char *acpigen_buf = *state;
	u32 package_length;
	u32 block_length;

	acpigen_set_current(acpigen_buf);
	acpigen_write_package(3);

	acpigen_write_return_singleton_buffer(0xA);
	acpigen_write_return_singleton_buffer(0x7);
	acpigen_write_return_singleton_buffer(0xF);

	acpigen_pop_len();

	package_length = decode_package_length(acpigen_buf);
	block_length = get_current_block_length(acpigen_buf);
	assert_int_equal(package_length, block_length);
}

static void test_acpigen_scope_with_contents(void **state)
{
	char *acpigen_buf = *state;
	char *block_start[8] = {0};
	u32 block_counter = 0;
	u32 package_length;
	u32 block_length;

	acpigen_set_current(acpigen_buf);

	/* Scope("\_SB") { */
	block_start[block_counter++] = acpigen_get_current();
	acpigen_write_scope("\\_SB");

	/* Device("PCI0") { */
	block_start[block_counter++] = acpigen_get_current();
	acpigen_write_device("PCI0");

	/* Name(INT1, 0x1234) */
	acpigen_write_name_integer("INT1", 0x1234);

	/* Name (_HID, EisaId ("PNP0A08")) // PCI Express Bus */
	acpigen_write_name("_HID");
	acpigen_emit_eisaid("PNP0A08");

	/* Method(^BN00, 0, NotSerialized) { */
	block_start[block_counter++] = acpigen_get_current();
	acpigen_write_method("^BN00", 0);

	/* Return( 0x12 + ^PCI0.INT1 ) */
	acpigen_write_return_op(AND_OP);
	acpigen_write_byte(0x12);
	acpigen_emit_namestring("^PCI0.INT1");

	/* } */
	acpigen_pop_len();
	block_counter--;
	package_length = decode_package_length(block_start[block_counter]);
	block_length = get_current_block_length(block_start[block_counter]);
	assert_int_equal(package_length, block_length);

	/* Method (_BBN, 0, NotSerialized) { */
	block_start[block_counter++] = acpigen_get_current();
	acpigen_write_method("_BBN", 0);

	/* Return (BN00 ()) */
	acpigen_write_return_namestr("BN00");
	acpigen_emit_byte(0x0A);

	/* } */
	acpigen_pop_len();
	block_counter--;
	package_length = decode_package_length(block_start[block_counter]);
	block_length = get_current_block_length(block_start[block_counter]);
	assert_int_equal(package_length, block_length);

	/* } */
	acpigen_pop_len();
	block_counter--;
	package_length = decode_package_length(block_start[block_counter]);
	block_length = get_current_block_length(block_start[block_counter]);
	assert_int_equal(package_length, block_length);

	/* } */
	acpigen_pop_len();
	block_counter--;
	package_length = decode_package_length(block_start[block_counter]);
	block_length = get_current_block_length(block_start[block_counter]);
	assert_int_equal(package_length, block_length);
}

static void verify_processor_package(void *buffer, unsigned int first, unsigned int count)
{
	uint8_t *aml = buffer;
	acpigen_set_current(buffer);
	acpigen_write_processor_package("PPKG", first, count);
	const uint8_t *end = (const uint8_t *)acpigen_get_current();
	const uint8_t name[] = { NAME_OP, 'P', 'P', 'K', 'G' };
	assert_memory_equal(aml, name, sizeof(name));
	const uint8_t *package = aml + sizeof(name);
	assert_int_equal(*package, count <= UINT8_MAX ? PACKAGE_OP : VARIABLE_PACKAGE_OP);
	const uint8_t *length_field = package + 1;
	const unsigned int length_bytes = 1 + (*length_field >> 6);
	assert_int_equal(decode_package_length((const char *)package), end - length_field);
	const uint8_t *element = length_field + length_bytes;
	if (count <= UINT8_MAX) {
		assert_int_equal(*element++, count);
	} else {
		assert_int_equal(*element++, WORD_PREFIX);
		assert_int_equal(element[0] | (uint32_t)element[1] << 8, count);
		element += 2;
	}
	for (unsigned int i = first; i < first + count; i++) {
		static const char hex[] = "0123456789ABCDEF";
		const uint8_t reference[] = {
			ROOT_PREFIX, DUAL_NAME_PREFIX, '_', 'S', 'B', '_', 'C',
			hex[(i >> 8) & 15], hex[(i >> 4) & 15], hex[i & 15],
		};
		assert_true(end - element >= sizeof(reference));
		assert_memory_equal(element, reference, sizeof(reference));
		element += sizeof(reference);
	}
	assert_ptr_equal(element, end);
}

static void test_processor_packages(void **state)
{
	/* Check empty, single-CPU and byte-count limits, plus a two-socket CPU set. */
	const unsigned int counts[] = { 0, 1, 255, 256, 768 };
	for (size_t i = 0; i < ARRAY_SIZE(counts); i++)
		verify_processor_package(*state, 0, counts[i]);
	verify_processor_package(*state, 16, 256);
}

static void test_large_packages(void **state)
{
	const unsigned int counts[] = { 255, 256, 65536 };
	uint8_t *aml = *state;

	for (size_t i = 0; i < ARRAY_SIZE(counts); i++) {
		unsigned int count = counts[i];
		acpigen_set_current((char *)aml);
		acpigen_write_package(count);
		for (unsigned int j = 0; j < count; j++)
			acpigen_write_zero();
		acpigen_pop_len();
		const uint8_t *end = (const uint8_t *)acpigen_get_current();
		assert_int_equal(aml[0], count <= UINT8_MAX ? PACKAGE_OP : VARIABLE_PACKAGE_OP);
		assert_int_equal(decode_package_length((const char *)aml), end - aml - 1);
		const uint8_t *element = aml + 2 + (aml[1] >> 6);
		if (count <= UINT8_MAX) {
			assert_int_equal(*element++, count);
		} else {
			unsigned int bytes = count <= UINT16_MAX ? 2 : 4;
			assert_int_equal(*element++, bytes == 2 ? WORD_PREFIX : DWORD_PREFIX);
			uint32_t decoded = 0;
			for (unsigned int j = 0; j < bytes; j++)
				decoded |= (uint32_t)*element++ << (8 * j);
			assert_int_equal(decoded, count);
		}
		assert_int_equal(end - element, count);
		while (element < end)
			assert_int_equal(*element++, ZERO_OP);
	}
}

static void test_patch_package_count(void **state)
{
	const uint32_t counts[] = { 0, 1, 255, 256, 65535, 65536 };
	uint8_t *aml = *state;

	for (size_t i = 0; i < ARRAY_SIZE(counts); i++) {
		/* Exercise both repeated increments and a single update after the payload. */
		for (unsigned int incremental = 0; incremental < 2; incremental++) {
			acpigen_set_current((char *)aml);
			struct acpigen_package package = acpigen_write_package(0);
			for (uint32_t j = 0; j < counts[i]; j++) {
				acpigen_write_zero();
				if (incremental)
					acpigen_increment_package_count(&package);
			}
			if (!incremental)
				acpigen_package_add_count(&package, counts[i]);
			acpigen_pop_len();
			size_t length = acpigen_get_current() - (char *)aml;
			assert_int_equal(aml[0], counts[i] <= UINT8_MAX ?
					 PACKAGE_OP : VARIABLE_PACKAGE_OP);
			assert_int_equal(decode_package_length((char *)aml), length - 1);
			const uint8_t *count = aml + 2 + (aml[1] >> 6);
			if (counts[i] <= UINT8_MAX) {
				assert_int_equal(*count++, counts[i]);
			} else {
				unsigned int width = counts[i] <= UINT16_MAX ? 2 : 4;
				assert_int_equal(*count++, width == 2 ? WORD_PREFIX : DWORD_PREFIX);
				uint32_t decoded = 0;
				for (unsigned int j = 0; j < width; j++)
					decoded |= (uint32_t)*count++ << (8 * j);
				assert_int_equal(decoded, counts[i]);
			}
			assert_int_equal(aml + length - count, counts[i]);

			/* A deferred count must emit exactly the same AML as a known count. */
			acpigen_write_package(counts[i]);
			for (uint32_t j = 0; j < counts[i]; j++)
				acpigen_write_zero();
			acpigen_pop_len();
			assert_int_equal(acpigen_get_current() - (char *)aml, 2 * length);
			assert_memory_equal(aml, aml + length, length);
		}
	}
}

static void test_nested_deferred_packages(void **state)
{
	uint8_t *aml = *state;

	/* Name (PROP, Package () { Package () { "enabled", One }, Zero }) */
	const uint8_t expected[] = {
		NAME_OP, 'P', 'R', 'O', 'P', PACKAGE_OP, 16, 2,
		PACKAGE_OP, 12, 2, STRING_PREFIX,
		'e', 'n', 'a', 'b', 'l', 'e', 'd', 0, ONE_OP, ZERO_OP,
	};

	acpigen_set_current((char *)aml);
	acpigen_write_name("PROP");
	struct acpigen_package parent = acpigen_write_package(0);
	acpigen_package_add_count(&parent, 0);
	struct acpigen_package child = acpigen_write_package(0);
	acpigen_write_string("enabled");
	acpigen_write_one();
	char *end = acpigen_get_current();
	acpigen_package_add_count(&child, 2);
	assert_ptr_equal(acpigen_get_current(), end);
	acpigen_pop_len();
	/* Closing a child must leave the parent's count descriptor valid. */
	acpigen_increment_package_count(&parent);
	acpigen_write_zero();
	acpigen_increment_package_count(&parent);
	acpigen_pop_len();
	assert_int_equal(acpigen_get_current() - (char *)aml, sizeof(expected));
	assert_memory_equal(aml, expected, sizeof(expected));
}

static void test_growing_package_length_boundaries(void **state)
{
	uint8_t *aml = *state;
	/* Count growth also crosses one- and two-byte PkgLength boundaries. */
	const unsigned int counts[] = { 60, 61, 62, 4089, 4090, 4091 };

	for (size_t i = 0; i < ARRAY_SIZE(counts); i++) {
		acpigen_set_current((char *)aml);
		acpigen_write_scope("TEST");
		struct acpigen_package package = acpigen_write_package(0);
		for (unsigned int j = 0; j < counts[i]; j++)
			acpigen_write_zero();
		acpigen_package_add_count(&package, counts[i]);
		acpigen_pop_len();
		acpigen_pop_len();
		size_t length = acpigen_get_current() - (char *)aml;

		/* Compare every byte with the same package whose count is known up front. */
		acpigen_write_scope("TEST");
		acpigen_write_package(counts[i]);
		for (unsigned int j = 0; j < counts[i]; j++)
			acpigen_write_zero();
		acpigen_pop_len();
		acpigen_pop_len();
		assert_int_equal(acpigen_get_current() - (char *)aml, 2 * length);
		assert_memory_equal(aml, aml + length, length);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_nested_deferred_packages, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_growing_package_length_boundaries,
						setup_acpigen, teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_patch_package_count, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_large_packages, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_processor_packages, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_single_if, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_nested_ifs, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_write_package, setup_acpigen,
						teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_scope_with_contents, setup_acpigen,
						teardown_acpigen),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
