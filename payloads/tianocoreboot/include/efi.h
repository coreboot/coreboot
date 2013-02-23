/*
 * This file is part of the TianoCoreBoot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stdint.h>

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1UL)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

typedef long intn_t;
typedef unsigned long uintn_t;

#include <EfiTypes.h>
#include <EfiFirmwareVolumeHeader.h>
#include <PiFirmwareFile.h>
#include <PeiHob.h>
#include <MemoryAllocationHob.h>

#define EFI_DXE_FILE_GUID \
  { 0xb1644c1a, 0xc16a, 0x4c5b, { 0x88, 0xde, 0xea, 0xfb, 0xa9, 0x7e, 0x74, 0xd8 } }

