/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
 * Foundation, Inc.
 */

void arm64_cpu_early_setup(void);
void cortex_a57_cpu_power_down(int l2_flush);

void __attribute__((weak)) arm64_cpu_early_setup(void)
{
	/* Default empty implementation */
}

void __attribute__((weak)) cortex_a57_cpu_power_down(int l2_flush)
{
	/* Default empty implementation */
}
