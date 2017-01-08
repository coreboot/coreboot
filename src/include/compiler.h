/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#ifndef __COMPILER_H__
#define __COMPILER_H__

#if defined(__WIN32) || defined(__WIN64)
#define __packed __attribute__((gcc_struct, packed))
#else
#define __packed __attribute__((packed))
#endif

#define __aligned(x) __attribute__((aligned(x)))
#define __always_unused __attribute__((unused))
#define __must_check __attribute__((warn_unused_result))

#if IS_ENABLED(CONFIG_COMPILER_LLVM_CLANG)
#define ADDR32(opcode) opcode
#else
#define ADDR32(opcode) addr32 opcode
#endif

#endif
