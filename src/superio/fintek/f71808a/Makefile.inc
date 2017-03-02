##
## This file is part of the coreboot project.
##
## Copyright (C) 2017 Nicola Corna <nicola@corna.info>
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

ramstage-$(CONFIG_SUPERIO_FINTEK_F71808A) += f71808a_multifunc.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F71808A) += f71808a_hwm.c
ramstage-$(CONFIG_SUPERIO_FINTEK_F71808A) += superio.c
