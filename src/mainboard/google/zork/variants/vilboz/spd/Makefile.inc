# SPDX-License-Identifier: GPL-2.0-or-later

# Ordered List of APCB entries, up to 16.
# Entries should match this pattern {NAME}_x{1,2}
# There should be a matching SPD hex file in SPD_SOURCES_DIR
# matching the pattern {NAME}.spd.hex
# The _x{1,2} suffix denotes single or dual channel
# TODO: Remove channel suffix when b:141434940 is fixed
# Alternatively, generated APCBs stored at
# CONFIG_APCB_BLOB_DIR/APCB_{NAME}.bin can be included.
APCB_SOURCES = empty
