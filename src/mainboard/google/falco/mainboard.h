/*
* This file is part of the coreboot project.
*
* Copyright 2013 Google Inc.
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

#ifndef __MAINBOARD_H_
#define __MAINBOARD_H_
void mainboard_train_link(struct intel_dp *intel_dp);
void mainboard_set_port_clk_dp(struct intel_dp *intel_dp);

#endif
