/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <bootstate.h>
#include <tests/test.h>
#include "psp_def.h"

extern struct boot_state_init_entry *test_bootstate_begin[];
extern struct boot_state_init_entry *test_bootstate_end[];

static unsigned int requests;
static int delivery_status;
static uint32_t response_status;
static uint32_t rejected_command;

static const uint32_t command_order[] = {
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK)
	0x1b,
#endif
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_SMM_LOCK)
	0x6c,
#endif
	0x06,
};

static void run_boot_done(void)
{
	assert_int_equal(test_bootstate_end - test_bootstate_begin, 1);
	struct boot_state_init_entry *entry = test_bootstate_begin[0];

	assert_int_equal(entry->state, BS_PAYLOAD_BOOT);
	assert_int_equal(entry->when, BS_ON_ENTRY);
	assert_non_null(entry->bscb.callback);
	entry->bscb.callback(entry->bscb.arg);
}

void __noreturn die(const char *message, ...)
{
	mock_assert(false, message, __FILE__, __LINE__);
	__builtin_unreachable();
}

int send_psp_command(uint32_t command, void *buffer)
{
	struct mbox_buffer_header *header = buffer;
	assert_true(requests < ARRAY_SIZE(command_order));
	assert_int_equal(command, command_order[requests]);
	assert_int_equal((uintptr_t)buffer % 32, 0);
	assert_int_equal(header->status, 0);
	assert_int_equal(header->size, command != 0x06 ? 8 : 32);
	requests++;
	if (command != rejected_command)
		return 0;
	header->status = response_status;
	return delivery_status;
}

static void reset_requests(uint32_t command, int delivery, uint32_t response)
{
	requests = 0;
	rejected_command = command;
	delivery_status = delivery;
	response_status = response;
}

static void boot_done_request(void **state)
{
	reset_requests(0, 0, 0);
	run_boot_done();
	assert_int_equal(requests, ARRAY_SIZE(command_order));
}

static void rejected_request_is_not_retried(void **state)
{
	reset_requests(0x06, -PSPSTS_SEND_ERROR, 0);
	run_boot_done();
	assert_int_equal(requests, ARRAY_SIZE(command_order));
}

#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_SMM_LOCK)
static void rejected_smm_lock_stops_before_boot_done(void **state)
{
	reset_requests(0x6c, -PSPSTS_SEND_ERROR, 0);
	run_boot_done();
	assert_int_equal(requests, CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK) ? 2 : 1);
}

static void timed_out_smm_lock_keeps_request_live(void **state)
{
	reset_requests(0x6c, -PSPSTS_CMD_TIMEOUT, 0);
	expect_assert_failure(run_boot_done());
	assert_int_equal(requests, CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK) ? 2 : 1);
}

static void failed_smm_lock_response_stops_before_boot_done(void **state)
{
	reset_requests(0x6c, 0, 1);
	run_boot_done();
	assert_int_equal(requests, CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK) ? 2 : 1);
}

static void unsubmitted_smm_lock_skips_boot_done(void **state)
{
	const int failures[] = {-PSPSTS_NOBASE, -PSPSTS_RECOVERY, -PSPSTS_INIT_TIMEOUT};

	for (size_t i = 0; i < ARRAY_SIZE(failures); i++) {
		reset_requests(0x6c, failures[i], 0);
		run_boot_done();
		assert_int_equal(requests, CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK) ? 2 : 1);
	}
}
#endif

#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK)
static void failed_df_lock_stops_before_later_commands(void **state)
{
	const struct {
		int delivery;
		uint32_t response;
	} failures[] = {
		{-PSPSTS_SEND_ERROR, 0},
		{-PSPSTS_NOBASE, 0},
		{-PSPSTS_RECOVERY, 0},
		{-PSPSTS_INIT_TIMEOUT, 0},
		{0, 1},
	};

	for (size_t i = 0; i < ARRAY_SIZE(failures); i++) {
		reset_requests(0x1b, failures[i].delivery, failures[i].response);
		run_boot_done();
		assert_int_equal(requests, 1);
	}
}

static void timed_out_df_lock_keeps_request_live(void **state)
{
	reset_requests(0x1b, -PSPSTS_CMD_TIMEOUT, 0);
	expect_assert_failure(run_boot_done());
	assert_int_equal(requests, 1);
}
#endif

int psp_boot_done_test_main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(boot_done_request),
		cmocka_unit_test(rejected_request_is_not_retried),
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_SMM_LOCK)
		cmocka_unit_test(rejected_smm_lock_stops_before_boot_done),
		cmocka_unit_test(timed_out_smm_lock_keeps_request_live),
		cmocka_unit_test(failed_smm_lock_response_stops_before_boot_done),
		cmocka_unit_test(unsubmitted_smm_lock_skips_boot_done),
#endif
#if CONFIG(SOC_AMD_COMMON_BLOCK_PSP_DF_LOCK)
		cmocka_unit_test(failed_df_lock_stops_before_later_commands),
		cmocka_unit_test(timed_out_df_lock_keeps_request_live),
#endif
	};
	return cb_run_group_tests(tests, NULL, NULL);
}
