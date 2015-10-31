##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 DMP Electronics Inc.
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

ifeq ($(CONFIG_NORTHBRIDGE_DMP_VORTEX86EX),y)

ramstage-y += northbridge.c
ramstage-y += xgi_oprom.c

endif
