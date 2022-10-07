/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

void intel_gmbus_read_edid(u8 *gmbus_mmio, u8 bus, u8 slave, u8 *edid, u32 edid_size);
void intel_gmbus_stop(u8 *gmbus_mmio);
