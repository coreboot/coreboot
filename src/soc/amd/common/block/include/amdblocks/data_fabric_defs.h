/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_DATA_FABRIC_DEFS_H
#define AMD_BLOCK_DATA_FABRIC_DEFS_H

#define DF_REG_ID(fn, reg)	(((fn) & 0x7) << 12 | ((reg) & 0xfff))
#define DF_REG_FN(id)		((id) >> 12)
#define DF_REG_OFFSET(id)	((id) & 0xfff)

#endif /* AMD_BLOCK_DATA_FABRIC_DEFS_H */
