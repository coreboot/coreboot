#ifndef _BYTEORDER_H
#define _BYTEORDER_H

#define __LITTLE_ENDIAN 1234

#include <stdint.h>
#include <swab.h>

#define cpu_to_le64(x) ((uint64_t)(x))
#define le64_to_cpu(x) ((uint64_t)(x))
#define cpu_to_le32(x) ((uint32_t)(x))
#define le32_to_cpu(x) ((uint32_t)(x))
#define cpu_to_le16(x) ((uint16_t)(x))
#define le16_to_cpu(x) ((uint16_t)(x))
#define cpu_to_be64(x) swab64(x)
#define be64_to_cpu(x) swab64(x)
#define cpu_to_be32(x) swab32((x))
#define be32_to_cpu(x) swab32((x))
#define cpu_to_be16(x) swab16((x))
#define be16_to_cpu(x) swab16((x))

#define ntohll(x) be64_to_cpu(x)
#define htonll(x) cpu_to_be64(x)
#define ntohl(x)  be32_to_cpu(x)
#define htonl(x)  cpu_to_be32(x)

#endif /* _BYTEORDER_H */
