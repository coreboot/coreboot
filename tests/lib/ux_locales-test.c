/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <stdbool.h>
#include <string.h>
#include <tests/test.h>
#include <ux_locales.h>
#include <vb2_api.h>

#define DATA_DEFAULT                                                                           \
	(                                                                                      \
		"\x01"  /* Version. */                                                         \
		"name_1\x00"  /* name_1, langs = [0, 2, 30]. */                                \
		"0\x00translation_1_0\x00"                                                     \
		"2\x00translation_1_2\x00"                                                     \
		"30\x00translation_1_30\x00"                                                   \
		"\x01"                                                                         \
		"name_15\x00"  /* name_15, langs = [4, 25, 60]. */                             \
		"4\x00translation_15_4\x00"                                                    \
		"25\x00translation_15_25\x00"                                                  \
		"60\x00translation_15_60\x00"                                                  \
		"\x01"                                                                         \
		"name_20\x00"  /* name_20, langs = [8]. */                                     \
		"8\x00translation_20_8\x00"                                                    \
		"\x01"                                                                         \
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
	const char *name;
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

	will_return(_cbfs_alloc, true);
	will_return(vb2api_get_locale_id, s->lang_id);
	ret = ux_locales_get_text(s->name);
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
	assert_null(ux_locales_get_text("name_1"));
}

static void test_ux_locales_bad_version(void **state)
{
	will_return(_cbfs_alloc, true);
	will_return(vb2api_get_locale_id, 0);
	assert_null(ux_locales_get_text("name_1"));
}

static void test_ux_locales_two_calls(void **state)
{
	const char *ret;

	/* We do not need to ensure that we cached the cbfs region. */
	will_return_always(_cbfs_alloc, true);

	/* Call #1: read (15, 60). */
	will_return(vb2api_get_locale_id, 60);
	ret = ux_locales_get_text("name_15");
	assert_non_null(ret);
	assert_string_equal(ret, "translation_15_60");

	/* Call #2: read (1, 0). */
	will_return(vb2api_get_locale_id, 0);
	ret = ux_locales_get_text("name_1");
	assert_non_null(ret);
	assert_string_equal(ret, "translation_1_0");
}

static void test_ux_locales_null_terminated(void **state)
{
	will_return_always(_cbfs_alloc, true);
	will_return_always(vb2api_get_locale_id, 8);

	/* Verify the access to the very last text. */
	assert_non_null(ux_locales_get_text("name_20"));

	/* Modify the last 2 bytes from "\x00\x01" to "XX" and unmap, */
	data.raw[data.size - 1] = 'X';
	data.raw[data.size - 2] = 'X';
	ux_locales_unmap();

	/* The last few characters are now changed so that the data is not NULL terminated.
	   This will prevent us from accessing the last text. */
	assert_null(ux_locales_get_text("name_20"));
}

/*
 * This macro helps test ux_locales_get_text with `_name` and `_lang_id`.
 * If `_expect` is NULL, then the function should not find anything.
 * Otherwise, the function should find the corresponding expect value.
 */
#define UX_LOCALES_GET_TEXT_TEST(_name, _lang_id, _expect)                                     \
	((struct CMUnitTest) {                                                                 \
		.name = "test_ux_locales_get_text(name=" _name ", lang_id=" #_lang_id          \
			", expect=" #_expect ")",                                              \
		.test_func = test_ux_locales_get_text,                                         \
		.setup_func = setup_default,                                                   \
		.teardown_func = teardown_unmap,                                               \
		.initial_state = &(struct ux_locales_test_state) {                             \
			.name = _name,                                                         \
			.lang_id = _lang_id,                                                   \
			.expect = _expect,                                                     \
		},                                                                             \
	})

int main(void)
{
	const struct CMUnitTest tests[] = {
		/* Get text successfully. */
		UX_LOCALES_GET_TEXT_TEST("name_1", 0, "translation_1_0"),
		/* Get text with name and id both in the middle. */
		UX_LOCALES_GET_TEXT_TEST("name_15", 25, "translation_15_25"),
		/* Ensure we check the whole string of 'name'.
		   ('name_2' is the prefix of 'name_20') */
		UX_LOCALES_GET_TEXT_TEST("name_2", 3, NULL),
		/* Ensure we check the whole string of 'lang_id'.
		   (id:'2' is the prefix of id:'25' in 'name_15') */
		UX_LOCALES_GET_TEXT_TEST("name_15", 2, NULL),
		/* Ensure we will fallback to 0. */
		UX_LOCALES_GET_TEXT_TEST("name_1", 7, "translation_1_0"),
		/* Do not search for locale id with unmatched name. */
		UX_LOCALES_GET_TEXT_TEST("name_15", 8, NULL),
		/* Validity check of lang_id > 100. We will fallback to 0. */
		UX_LOCALES_GET_TEXT_TEST("name_1", 100, "translation_1_0"),
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
