/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 The ChromiumOS Authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __HALT_H__
#define __HALT_H__

#ifdef __ROMCC__
#include <lib/halt.c>
#else
/**
 * halt the system reliably
 */
void __attribute__((noreturn)) halt(void);
#endif /* __ROMCC__ */
#endif /* __HALT_H__ */
