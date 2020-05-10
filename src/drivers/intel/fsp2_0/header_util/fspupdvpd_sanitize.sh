# SPDX-License-Identifier: GPL-2.0-or-later

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
