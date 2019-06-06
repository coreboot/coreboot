##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
## Copyright (C) 2018 Eltan B.V.
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

ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += superio.c
ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += ../../common/ssdt.c
ramstage-$(CONFIG_SUPERIO_ASPEED_AST2400) += ../../common/generic.c
