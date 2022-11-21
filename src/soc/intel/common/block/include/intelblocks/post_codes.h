/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_POST_CODES_H
#define SOC_INTEL_COMMON_BLOCK_POST_CODES_H

/* common/block/cpu/car/cache_as_ram.s */
#define POST_BOOTBLOCK_PRE_C_ENTRY			0x20
#define POST_SOC_NO_RESET				0x21
#define POST_SOC_CLEAR_FIXED_MTRRS			0x22
#define POST_SOC_CLEAR_VAR_MTRRS			0x23
#define POST_SOC_SET_UP_CAR_MTRRS			0x24
#define POST_SOC_BOOTGUARD_SETUP			0x25
#define POST_SOC_CLEARING_CAR				0x26
#define POST_SOC_DISABLE_CACHE_EVICT			0x27
#define POST_SOC_CAR_NEM_ENHANCED			0x28
#define POST_SOC_CAR_INIT_DONE				0x29
#define POST_SOC_BEFORE_CARSTAGE			0x2a

/* common/block/cpu/car/cache_as_ram_fsp.S */
#define POST_BOOTBLOCK_CAR				0x21

/* common/block/cse/cse.c */
#define POST_CODE_ZERO					0x00
#endif
