# This file is part of the coreboot project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

#
# Convert the FspUpdVpd.h header file into a format usable by coreboot
# Usage:
#     fspupdvpd_sanitize.sh <path/to/FspUpdVpd.h>
#

PWD=$(dirname "${BASH_SOURCE[0]}")

SPATCH=spatch

# Fix line terminations
dos2unix $1

# Clean up trailing whitespace
sed -e "s, $,,g" -i $1

# Now fix the actual coding style
$SPATCH -sp_file ${PWD}/fspupdvpd.spatch \
	-in_place $1
