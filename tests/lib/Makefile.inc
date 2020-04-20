##
## This file is part of the coreboot project.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

# object filest should be under build/tests/ build/test/src/ build/test/run/
# two examples - first should be simply string.c, second should use -wrap

tests-y += string-test

string-test-srcs += tests/lib/string-test.c
string-test-srcs += src/lib/string.c
