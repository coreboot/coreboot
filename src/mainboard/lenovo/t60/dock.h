/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
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

#ifndef THINKPAD_X60_DOCK_H
#define THINKPAD_X60_DOCK_H

int dock_connect(void);
void dock_disconnect(void);
int dock_present(void);
int dlpc_init(void);

int legacy_io_present(void);
void legacy_io_init(void);
#endif
