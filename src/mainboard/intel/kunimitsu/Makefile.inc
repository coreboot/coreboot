##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Google Inc.
## Copyright (C) 2015 Intel Corporation.
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

subdirs-y += spd

romstage-y += boardid.c
romstage-y += pei_data.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c

ramstage-$(CONFIG_EC_GOOGLE_CHROMEEC) += ec.c

ramstage-y += boardid.c
ramstage-y += mainboard.c
ramstage-y += pei_data.c
ramstage-y += ramstage.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
