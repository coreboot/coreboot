/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Address of the MRC status byte in CMOS. Should be reserved
 * in mainboards' cmos.layout and not covered by checksum.
 */

#if CONFIG(USE_OPTION_TABLE)
#include "option_table.h"
#if CMOS_VSTART_cbnt_cmos != CONFIG_INTEL_CBNT_CMOS_OFFSET * 8
#error "CMOS start for CBNT CMOS is not correct, check your cmos.layout"
#endif
#if CMOS_VLEN_cbnt_cmos != 16
#error "CMOS length for CBNT CMOS bytes are not correct, check your cmos.layout"
#endif
#endif
