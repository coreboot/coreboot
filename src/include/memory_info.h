/* Memory information */
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MEMORY_INFO_H_
#define _MEMORY_INFO_H_

#include <commonlib/memory_info.h>

/*
 * mainboard_get_dram_part_num returns a DRAM part number override string
 *  return NULL = no part number override provided by mainboard
 *  return non-NULL = pointer to a string terminating in '\0'
 */
const char *mainboard_get_dram_part_num(void);
#endif
