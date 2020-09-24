/* SPDX-License-Identifier: GPL-2.0-only */

/* Trick the linker into supporting x86_64 relocations in 32bit code */
#if ENV_X86_64
#define uintptr_t quad
#else
#define uintptr_t long
#endif

rom_mtrr_mask:
.uintptr_t _rom_mtrr_mask

rom_mtrr_base:
.uintptr_t _rom_mtrr_base

car_mtrr_mask:
.uintptr_t _car_mtrr_mask

car_mtrr_start:
.uintptr_t _car_mtrr_start
