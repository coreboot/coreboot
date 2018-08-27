/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux
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

#ifndef __SOC_SIFIVE_FU540_CLINT_H
#define __SOC_SIFIVE_FU540_CLINT_H

/* This function is used to set MSIP.
 * It can be used to send an IPI (inter-processor interrupt) to
 * another hart*/
void set_msip(int hartid, int val);

#endif
