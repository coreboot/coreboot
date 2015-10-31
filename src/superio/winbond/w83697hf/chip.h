/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Sean Nelson <snelson@nmt.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SUPERIO_WINBOND_W83697HF_CHIP_H
#define SUPERIO_WINBOND_W83697HF_CHIP_H


struct superio_winbond_w83697hf_config {
	unsigned int hwmon_fan1_divisor;
	unsigned int hwmon_fan2_divisor;
};

#endif /* SUPERIO_WINBOND_W83697HF_CHIP_H */
