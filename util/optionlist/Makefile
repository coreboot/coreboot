#
# Copyright (C) 2010 coresystems GmbH
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#


SVNVERSION := $(shell LC_ALL=C svnversion -cn ../.. | sed -e "s/.*://" -e "s/\([0-9]*\).*/\1/" | grep "[0-9]" || LC_ALL=C svn info . | grep ^Revision | sed "s/.*[[:blank:]]\+\([0-9]*\)[^0-9]*/\1/" | grep "[0-9]" || echo unknown)

all:
	cd ../..; util/optionlist/kconfig2wiki src/Kconfig $(SVNVERSION) > util/optionlist/Options.wiki

clean:
	rm -rf Options.wiki

