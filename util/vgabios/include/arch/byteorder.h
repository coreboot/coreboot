/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BYTEORDER_H
#define _BYTEORDER_H

#define __LITTLE_ENDIAN 1234

#define cpu_to_le16(x) ((uint16_t)(x))
#define cpu_to_le32(x) ((uint32_t)(x))

#endif /* _BYTEORDER_H */
