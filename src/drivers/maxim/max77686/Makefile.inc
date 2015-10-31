##
## This file is part of the coreboot project.
##
## Copyright (C) 2012 The ChromiumOS Authors
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

romstage-$(CONFIG_DRIVER_MAXIM_MAX77686) += max77686.c
ramstage-$(CONFIG_DRIVER_MAXIM_MAX77686) += max77686.c
