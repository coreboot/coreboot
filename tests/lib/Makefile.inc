# SPDX-License-Identifier: GPL-2.0-only

# object filest should be under build/tests/ build/test/src/ build/test/run/
# two examples - first should be simply string.c, second should use -wrap

tests-y += string-test

string-test-srcs += tests/lib/string-test.c
string-test-srcs += src/lib/string.c
