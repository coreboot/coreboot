#ifndef _BYTEORDER_H
#define _BYTEORDER_H

#define __BIG_ENDIAN 4321

#include <swab.h>

#define cpu_to_le32(x) swab32((x))
#define le32_to_cpu(x) swab32((x))
#define cpu_to_le16(x) swab16((x))
#define le16_to_cpu(x) swab16((x))
#define cpu_to_be32(x) ((unsigned int)(x))
#define be32_to_cpu(x) ((unsigned int)(x))
#define cpu_to_be16(x) ((unsigned short)(x))
#define be16_to_cpu(x) ((unsigned short)(x))

#endif /* _BYTEORDER_H */
