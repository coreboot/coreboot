/* SPDX-License-Identifier: GPL-2.0-only */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <b64_decode.h>
#include <tests/test.h>

struct messages_t {
	const char *enc;
	const char *dec;
} messages[] = {
	{"QQ==", "A"},
	{"Q\r\nUI=", "AB"},
	{"QUJD", "ABC"},
	{"\nQUJDRA==", "ABCD"},
	{"SGVsbG8\r=", "Hello"},
	{"SGVsbG8h", "Hello!"}
};

const char *invalid[] = {
	"QQ=-=",
	"SGVsbG-8="
};

static void test_b64_decode(void **state)
{
	uint8_t *decoded;
	size_t res;

	for (int i = 0; i < ARRAY_SIZE(messages); i++) {
		decoded = malloc(strlen(messages[i].enc) * sizeof(char));

		res = b64_decode((uint8_t *)messages[i].enc, strlen(messages[i].enc), decoded);

		assert_int_equal(res, (strlen(messages[i].dec)));
		assert_string_equal((const char *)decoded, messages[i].dec);

		free(decoded);
	}

	for (int i = 0; i < ARRAY_SIZE(invalid); i++) {
		decoded = malloc(strlen(invalid[i]) * sizeof(char));

		res = b64_decode((uint8_t *)invalid[i], strlen(invalid[i]), decoded);

		assert_int_equal(res, 0);

		free(decoded);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_b64_decode),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
