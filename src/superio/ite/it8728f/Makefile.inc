##
## This file is part of the coreboot project.
##
## Copyright (C) 2013 Damien Zammit <damien@zamaudio.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ramstage-$(CONFIG_SUPERIO_ITE_IT8728F) += it8728f_hwm.c
ramstage-$(CONFIG_SUPERIO_ITE_IT8728F) += superio.c
