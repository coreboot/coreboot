/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

unsigned char microcode[] __attribute__ ((aligned(16))) = {
#include "mc_patch_01000086.h"
#include "mc_patch_01000095.h"
#include "mc_patch_01000096.h"
#include "mc_patch_0100009f.h"
#include "mc_patch_010000b6.h"
#include "mc_patch_010000bf.h"
#include "mc_patch_010000c4.h"
};
