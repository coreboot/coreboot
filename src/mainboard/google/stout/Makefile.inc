##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

ramstage-y += ec.c

romstage-y += chromeos.c
ramstage-y += chromeos.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += mainboard_smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += ec.c

SRC_ROOT = $(src)/mainboard/google/stout
