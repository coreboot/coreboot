/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <stdbool.h>
#include <string.h>
#include <tests/test.h>
#include <ux_locales.h>
#include <vb2_api.h>

#define DATA_DEFAULT \
	( \
		"\x01" /* Version. */                                                       \
		"memory_training_desc\x00" /* memory_training_desc, langs = [0, 2, 30]. */  \
		"0\x00memory_training_desc_0\x00"                                           \
		"2\x00memory_training_desc_2\x00"                                           \
		"30\x00memory_training_desc_30\x00"                                         \
		"\x01"                                                                      \
		"low_battery_desc\x00" /* low_battery_desc, langs = [8]. */                 \
		"8\x00low_battery_desc_8\x00"                                               \
		"\x01"                                                                      \
	)
const unsigned char data_default[] = DATA_DEFAULT;

/*
 * The data must be set in the setup function and might be used in cbfs related functions.
 * The size of the data must be recorded because the data contains the null character \x00.
 *
 * Note that sizeof(DATA_DEFAULT) will count the '\0' at the end, so DATA_DEFAULT_SIZE, i.e.
 * the real data size must be minus one.
 */
#define DATA_DEFAULT_SIZE (sizeof(DATA_DEFAULT) - 1)

#define MAX_DATA_SIZE (DATA_DEFAULT_SIZE + 1)
struct {
	unsigned char raw[MAX_DATA_SIZE];
	size_t size;
} data;

/* Mock functions. */
void cbfs_unmap(void *mapping)
{
	/* No-op. */
}

/* We can't mock cbfs_ro_map() directly as it's a static inline function. */
void *_cbfs_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
		  size_t *size_out, bool force_ro, enum cbfs_type *type)
{
	/* Mock a successful CBFS mapping. */
	if (mock_type(bool)) {
		*size_out = data.size;
		return data.raw;
	}
	*size_out = 0;
	return NULL;
}

uint32_t vb2api_get_locale_id(struct vb2_context *ctx)
{
	return mock_type(uint32_t);
}

struct vb2_context *vboot_get_context(void)
{
	static struct vb2_context vboot2_ctx;
	return &vboot2_ctx;
}

/* Test states for test_ux_locales_get_text with valid CBFS data. */
struct ux_locales_test_state {
	enum ux_locale_msg msg_id;
	uint32_t lang_id;
	const char *expect;
};

/* Setup and teardown functions. */
static int setup_default(void **state)
{
	void *ret;
	/* Setup the mocked cbfs region data. */
	data.size = DATA_DEFAULT_SIZE;
	ret = memset(data.raw, 0xff, MAX_DATA_SIZE);
	if (!ret)
		return 1;
	ret = memcpy(data.raw, data_default, data.size);
	if (!ret)
		return 1;
	return 0;
}

static int setup_bad_version(void **state)
{
	int ret = setup_default(state);
	if (ret)
		return ret;
	/* Modify the version byte. */
	data.raw[0] = ~data.raw[0];
	return 0;
}

static int teardown_unmap(void **state)
{
	/* We need to reset the cached_state in src/lib/ux_locales.c. */
	ux_locales_unmap();
	return 0;
}

/* Test items. */
static void test_ux_locales_get_text(void **state)
{
	struct ux_locales_test_state *s = *state;
	const char *ret;

	if (s->msg_id < UX_LOCALE_MSG_NUM) {
		will_return(_cbfs_alloc, true);
		will_return(vb2api_get_locale_id, s->lang_id);
	}

	ret = ux_locales_get_text(s->msg_id);
	if (s->expect) {
		assert_non_null(ret);
		assert_string_equal(ret, s->expect);
	} else {
		assert_null(ret);
	}
}

static void test_ux_locales_bad_cbfs(void **state)
{
	will_return(_cbfs_alloc, false);
	will_return_maybe(vb2api_get_locale_id, 0);
	assert_string_equal(ux_locales_get_text(UX_LOCALE_MSG_MEMORY_TRAINING),
		 "Your device is finishing an update. This may take 1-2 minutes.\n"
		 "Please do not turn off your device.");
}

static void test_ux_locales_bad_version(void **state)
{
	will_return(_cbfs_alloc, true);
	will_return(vb2api_get_locale_id, 0);
	assert_string_equal(ux_locales_get_text(UX_LOCALE_MSG_MEMORY_TRAINING),
		 "Your device is finishing an update. This may take 1-2 minutes.\n"
		 "Please do not turn off your device.");
}

static void test_ux_locales_two_calls(void **state)
{
	const char *ret;

	/* We do not need to ensure that we cached the cbfs region. */
	will_return_always(_cbfs_alloc, true);

	/* Call #1: read (1, 30). */
	will_return(vb2api_get_locale_id, 30);
	ret = ux_locales_get_text(UX_LOCALE_MSG_MEMORY_TRAINING);
	assert_non_null(ret);
	assert_string_equal(ret, "memory_training_desc_30");

	/* Call #2: read (1, 0). */
	will_return(vb2api_get_locale_id, 0);
	ret = ux_locales_get_text(UX_LOCALE_MSG_MEMORY_TRAINING);
	assert_non_null(ret);
	assert_string_equal(ret, "memory_training_desc_0");
}

static void test_ux_locales_null_terminated(void **state)
{
	will_return_always(_cbfs_alloc, true);
	will_return_always(vb2api_get_locale_id, 8);

	/* Verify the access to the very last text. */
	assert_non_null(ux_locales_get_text(UX_LOCALE_MSG_LOW_BATTERY));

	/* Modify the last 2 bytes from "\x00\x01" to "XX" and unmap, */
	data.raw[data.size - 1] = 'X';
	data.raw[data.size - 2] = 'X';
	ux_locales_unmap();

	/* The last few characters are now changed so that the data is not NULL terminated.
	   This will prevent us from accessing the last text therefore, make use of fallback
	   text message */
	assert_string_equal(ux_locales_get_text(UX_LOCALE_MSG_LOW_BATTERY),
			 "Battery low. Shutting down.");
}

/*
 * This macro helps test ux_locales_get_text with `_msg_id` and `_lang_id`.
 * If `_expect` is NULL, then the function should not find anything.
 * Otherwise, the function should find the corresponding expect value.
 */
#define UX_LOCALES_GET_TEXT_TEST(_msg_id, _lang_id, _expect)                               \
	((struct CMUnitTest) {                                                                 \
		.name = "test_ux_locales_get_text(msg_id=" #_msg_id ", lang_id=" #_lang_id          \
			", expect=" #_expect ")",                                              \
		.test_func = test_ux_locales_get_text,                                         \
		.setup_func = setup_default,                                                   \
		.teardown_func = teardown_unmap,                                               \
		.initial_state = &(struct ux_locales_test_state) {                             \
			.msg_id = _msg_id,                                                         \
			.lang_id = _lang_id,                                                   \
			.expect = _expect,                                                     \
		},                                                                             \
	})

int main(void)
{
	const struct CMUnitTest tests[] = {
		/* Get text successfully. */
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_MEMORY_TRAINING, 0, "memory_training_desc_0"),
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_MEMORY_TRAINING, 2, "memory_training_desc_2"),
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_LOW_BATTERY, 8, "low_battery_desc_8"),
		/* Check the whole string of lang_id. */
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_MEMORY_TRAINING, 3, "memory_training_desc_0"),
		/* Validity check of lang_id > 100. We will fallback to 0. */
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_MEMORY_TRAINING, 100, "memory_training_desc_0"),
		/* Ensure we show fallback message if `msg_id >= UX_LOCALE_MSG_NUM` */
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_NUM, 0, "Trying to display an unknown message?"),
		/* Do not search for locale id with unmatched name. */
		UX_LOCALES_GET_TEXT_TEST(UX_LOCALE_MSG_LOW_BATTERY, 4,
			 "Battery low. Shutting down."),
		/* cbfs not found. */
		cmocka_unit_test_setup_teardown(test_ux_locales_bad_cbfs, setup_default,
						teardown_unmap),
		/* Bad version. */
		cmocka_unit_test_setup_teardown(test_ux_locales_bad_version, setup_bad_version,
						teardown_unmap),
		/* Read multiple times. */
		cmocka_unit_test_setup_teardown(test_ux_locales_two_calls, setup_default,
						teardown_unmap),
		/* Validity check of NULL terminated. */
		cmocka_unit_test_setup_teardown(test_ux_locales_null_terminated,
						setup_default, teardown_unmap),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
