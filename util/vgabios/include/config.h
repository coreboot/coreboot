/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define CONFIG_PCI_OPTION_ROM_RUN_YABEL 1
#define CONFIG_YABEL_PCI_ACCESS_OTHER_DEVICES 0
#define CONFIG_YABEL_DIRECTHW 1

#define CONFIG_X86EMU_DEBUG 1
#define CONFIG_X86EMU_DEBUG_TIMINGS 0
#define CONFIG_X86EMU_DEBUG_JMP 0
#define CONFIG_X86EMU_DEBUG_TRACE 0
#define CONFIG_X86EMU_DEBUG_PNP 0
#define CONFIG_X86EMU_DEBUG_DISK 0
#define CONFIG_X86EMU_DEBUG_PMM 0
#define CONFIG_X86EMU_DEBUG_VBE 0
#define CONFIG_X86EMU_DEBUG_INT10 0
#define CONFIG_X86EMU_DEBUG_INTERRUPTS 0
#define CONFIG_X86EMU_DEBUG_CHECK_VMEM_ACCESS 0
#define CONFIG_X86EMU_DEBUG_MEM 0
#define CONFIG_X86EMU_DEBUG_IO 0

#define CONFIG_ARCH_X86 1
#define CONFIG_ARCH_ARM 0
