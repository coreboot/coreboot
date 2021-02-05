/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <lib.h>
#include <symbols.h>


#if CONFIG_STACK_SIZE == 0
# define STACK_SIZE 0x1000
#else
# define STACK_SIZE CONFIG_STACK_SIZE
#endif

/* Value used for stack initialization. Change if implementation changes. */
#define STACK_GUARD_VALUE 0xDEADBEEF

__weak extern u8 _stack[];
__weak extern u8 _estack[];
TEST_REGION(stack, STACK_SIZE);


static void fill_stack(u32 value)
{
	u32 *stack = (u32 *)_stack;
	for (size_t i = 0; i < STACK_SIZE / sizeof(u32); ++i)
		stack[i] = value;
}

/* Fill stack region with guard value.  */
static void clean_stack(void)
{
	fill_stack(STACK_GUARD_VALUE);
}

static int setup_stack_test(void **state)
{
	void *top_of_stack = _stack + sizeof(_stack);

	clean_stack();
	*state = top_of_stack;

	return 0;
}

static void test_empty_stack(void **state)
{
	void *top_of_stack = *state;

	/* Expect success when checking empty stack. */
	assert_int_equal(0, checkstack(top_of_stack, 0));
}

static void test_almost_full_stack(void **state)
{
	void *top_of_stack = *state;
	u32 *stack = (u32 *)_stack;

	/* Fill stack with random value except last word */
	for (size_t i = 1; i < STACK_SIZE / sizeof(u32); ++i)
		stack[i] = 0xAA11FF44;

	/* Expect success when checking almost-full stack,
	   because last guard value is present */
	assert_int_equal(0, checkstack(top_of_stack, 0));
}

static void test_full_stack(void **state)
{
	void *top_of_stack = *state;

	/* Fill stack with value different than stack guard */
	fill_stack(0x600DB015);

	/* Expect failure when checking full stack as absence of guard value at the end of
	   the stack indicates stack overrun. */
	assert_int_equal(-1, checkstack(top_of_stack, 0));
}

static void test_partialy_filled_stack(void **state)
{
	void *top_of_stack = *state;
	u32 *stack = (u32 *)_stack;
	size_t i = STACK_SIZE / sizeof(u32) / 2;

	for (; i < STACK_SIZE / sizeof(u32); ++i)
		stack[i] = 0x4321ABDC + i;

	/* Expect success when checking partially-filled stack,
	   because only part of stack is filled with non-guard value. */
	assert_int_equal(0, checkstack(top_of_stack, 0));
}

static void test_alternately_filled_stack(void **state)
{
	void *top_of_stack = *state;
	u32 *stack = (u32 *)_stack;
	size_t i;

	for (i = 0; i < STACK_SIZE / sizeof(u32); i += 2)
		stack[i] = STACK_GUARD_VALUE;

	for (i = 1; i < STACK_SIZE / sizeof(u32); i += 2)
		stack[i] = 0x42420707;

	assert_int_equal(0, checkstack(top_of_stack, 0));
}

static void test_incorrectly_initialized_stack(void **state)
{
	void *top_of_stack = *state;
	u32 *stack = (u32 *)_stack;

	/* Remove last stack guard value */
	stack[0] = 0xFF00FF11;

	/* Expect failure when there is no last stack guard value even if no other value was
	   changed. */
	assert_int_equal(-1, checkstack(top_of_stack, 0));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_empty_stack, setup_stack_test),
		cmocka_unit_test_setup(test_almost_full_stack, setup_stack_test),
		cmocka_unit_test_setup(test_full_stack, setup_stack_test),
		cmocka_unit_test_setup(test_partialy_filled_stack, setup_stack_test),
		cmocka_unit_test_setup(test_alternately_filled_stack, setup_stack_test),
		cmocka_unit_test_setup(test_incorrectly_initialized_stack, setup_stack_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
