/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_CPU_ID_H
#define SOC_MEDIATEK_COMMON_CPU_ID_H

#include <stdint.h>

#define MTK_CPU_ID_MT8186G	0x81861001
#define MTK_CPU_ID_MT8186T	0x81862001
#define MTK_CPU_ID_MT8189	0x81890000
#define MTK_CPU_SEG_ID_MT8189G	0x20
#define MTK_CPU_SEG_ID_MT8189H	0x21

u32 get_cpu_id(void);
u32 get_cpu_segment_id(void);

#endif /* SOC_MEDIATEK_COMMON_CPU_ID_H */
