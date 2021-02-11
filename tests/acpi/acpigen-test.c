/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <types.h>
#include <tests/test.h>
#include <acpi/acpigen.h>

#define ACPIGEN_TEST_BUFFER_SZ (16 * KiB)

/* Returns AML package length. Works with normal and extended packages.
   This implementation is independent from acpigen.c implementation of package length. */
static u32 decode_package_length(const char *ptr)
{
	const u8 *aml = (u8 *)ptr;
	const u32 offset = (aml[0] == EXT_OP_PREFIX ? 2 : 1);
	u32 byte_zero_mask = 0x3F;  /* Bits [0:5] */
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

static void create_nested_ifs_recursive(char *stack_start[], char *stack_end[], u32 i, u32 n)
{
	if (i >= n)
		return;

	stack_start[i] = acpigen_get_current();
	acpigen_write_if_and(LOCAL0_OP, ZERO_OP);

	for (int k = 0; k < 3; ++k)
		acpigen_write_store_ops(ZERO_OP, LOCAL1_OP);

	create_nested_ifs_recursive(stack_start, stack_end, i + 1, n);

	acpigen_pop_len();
	stack_end[i] = acpigen_get_current();
}

static void test_acpigen_nested_ifs(void **state)
{
	char *acpigen_buf = *state;
	const size_t nesting_level = 8;
	char *block_start[8] = {0};
	char *block_end[8] = {0};

	acpigen_set_current(acpigen_buf);

	create_nested_ifs_recursive(block_start, block_end, 0, nesting_level);

	for (int i = 0; i < nesting_level; ++i)
		assert_int_equal(decode_package_length(block_start[i]),
				block_end[i] - block_start[i] - 1);
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

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_acpigen_single_if,
						setup_acpigen, teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_nested_ifs,
						setup_acpigen, teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_write_package,
						setup_acpigen, teardown_acpigen),
		cmocka_unit_test_setup_teardown(test_acpigen_scope_with_contents,
						setup_acpigen, teardown_acpigen),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
