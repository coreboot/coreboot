#!/usr/bin/env sh
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# FIXME/NOTE: some false positives in dmp/vortex86ex
KCONFIG_FILE_LIST=$(git ls-files |grep "/Kconfig$")
for i in $(git grep -h "^config " $KCONFIG_FILE_LIST |cut -d' ' -f2 |sort -u); do
	git grep -q "CONFIG_$i" || \
	git grep -Eq "(depends on |select |default ).*$i" $KCONFIG_FILE_LIST || \
	echo $i not found
done | cat
