/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BYTEORDER_H
#define _BYTEORDER_H

#define __BIG_ENDIAN 4321

#define PPC_BIT(bit)		(0x8000000000000000UL >> (bit))
#define PPC_BITMASK(bs, be)	((PPC_BIT(bs) - PPC_BIT(be)) | PPC_BIT(bs))

#ifndef __ASSEMBLER__

#include <types.h>
#define PPC_SHIFT(val, lsb)	(((uint64_t)(val)) << (63 - (lsb)))

#else
#define PPC_SHIFT(val, lsb)	((val) << (63 - (lsb)))
#endif

#endif /* _BYTEORDER_H */
