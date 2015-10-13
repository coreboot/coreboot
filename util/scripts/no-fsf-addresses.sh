#!/bin/sh
# Copyright (c) 2015 Google Inc.
#
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

filelist=$(git ls-files $* | \
		grep -Ev "^($(git submodule status |cut -d' ' -f3 |tr '\012' '|' | sed "s,|$,,"))$" | \
		grep -Ev "\.patch$|_shipped$|\<LICENSE_GPL$|\<LGPL.txt$|\<COPYING$|\<DISCLAIMER$|^util/kconfig/|\<no-fsf-addresses.sh$")

sed -i \
	-e "s:Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, *MA[, ]*02110-1301[, ]*USA:Foundation, Inc.:" \
	-e "s:Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, *MA[, ]*02110-1301[, ]*\$$:Foundation, Inc.:" \
	-e "s:Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA:Foundation, Inc.:" \
	-e "s:Foundation, Inc., 59 Temple Place[-, ]*Suite 330, Boston, MA *02111-1307[, ]*USA:Foundation, Inc.:" \
	-e "s:Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.:Foundation, Inc.:" \
	-e "/Foundation, Inc./ {N;s:Foundation, Inc.* USA\.* *:Foundation, Inc. :;s:Foundation, Inc. $:Foundation, Inc.:}" \
	$filelist

sed -i \
	-e "/^[ \t\*#/-]*$/ {
		N
		/You should have received a copy of the GNU/ {
			N
			/You should have received a copy of the GNU.*along with this program\.[\., ]*$/ d
			/You should have received a copy of the GNU.*[iI]f not, write to[^[:alnum:]]*the Free.*Software[^[:alnum:]]*Foundation,[^[:alnum:]]*Inc\.[\., ]*$/ d
			/You should have received a copy of the GNU.*[iI]f not, see.*http:\/\/www.gnu.org\/licenses\/>[\. ]*$/ d
			N
			/You should have received a copy of the GNU.*[iI]f not, write to[^[:alnum:]]*the Free.*Software[^[:alnum:]]*Foundation,[^[:alnum:]]*Inc\.[\., ]*$/ d
			/You should have received a copy of the GNU.*[iI]f not, write to[^[:alnum:]]*the Free.*Software[^[:alnum:]]*Foundation,[^[:alnum:]]*Inc\.[\., ]*\*\// s:^.*\*/:\*/:
			/You should have received a copy of the GNU.*[iI]f not, write to[^[:alnum:]]*the Free.*Software[^[:alnum:]]*Foundation[\., ]*\*\// s:^.*\*/:\*/:
			/You should have received a copy of the GNU.*[iI]f not, see.*http:\/\/www.gnu.org\/licenses\/>[\. ]*$/ d
			N
			/You should have received a copy of the GNU.*[iI]f not, see.*http:\/\/www.gnu.org\/licenses\/>[\. ]*$/ d
			/You should have received a copy of the GNU.*[iI]f not, see.*http:\/\/www.gnu.org\/licenses\/>[\. ]*\*\// s:^.*\*/:\*/:
		}}" \
	$filelist
