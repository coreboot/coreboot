##
## This file is part of the coreboot project.
##
## Copyright (C) 2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

$(obj)/util/nrv2b/nrv2b: nrv2bdir $(src)/util/nrv2b/nrv2b.c
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -DENCODE -DDECODE -DMAIN -DVERBOSE -DNDEBUG -DBITSIZE=32 -DENDIAN=0 -o $@ $(src)/util/nrv2b/nrv2b.c

$(obj)/util/nrv2b/nrv2b-compress.o: nrv2bdir $(src)/util/nrv2b/nrv2b.c
	$(Q)printf "  HOSTCC  $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -DENCODE -DDECODE -DVERBOSE -DCOMPACT -DNDEBUG -DBITSIZE=32 -DENDIAN=0 -c -o $@ $(src)/util/nrv2b/nrv2b.c

nrv2bdir:
	$(Q)printf "  BUILD   NRV2B\n"
	$(Q)mkdir -p $(obj)/util/nrv2b/

