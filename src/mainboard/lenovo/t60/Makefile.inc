##
## This file is part of the coreboot project.
##
## Copyright (C) 2007-2008 coresystems GmbH
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

smm-y += dock.c
bootblock-y += dock.c
romstage-y += dock.c
bootblock-y += gpio.c
romstage-y += gpio.c
bootblock-y += early_init.c
romstage-y += early_init.c
