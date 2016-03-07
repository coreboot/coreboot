##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Google Inc.
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

cbfs-files-$(CONFIG_COREINFO_SECONDARY_PAYLOAD) += img/coreinfo
img/coreinfo-file := payloads/coreinfo/build/coreinfo.elf
img/coreinfo-type := payload

payloads/coreinfo/build/coreinfo.elf coreinfo:
	$(MAKE) -C payloads/coreinfo defaultbuild

clean-payloads:
	$(MAKE) -C payloads/coreinfo clean
	$(MAKE) -C payloads/libpayload clean
	$(MAKE) -C payloads/external/SeaBIOS -f Makefile.inc clean
	$(MAKE) -C payloads/external/U-Boot -f Makefile.inc clean
	$(MAKE) -C payloads/external/Memtest86Plus -f Makefile.inc clean


distclean-payloads:
	$(MAKE) -C payloads/coreinfo distclean
	$(MAKE) -C payloads/libpayload distclean
	$(MAKE) -C payloads/external/SeaBIOS -f Makefile.inc distclean
	$(MAKE) -C payloads/external/U-Boot -f Makefile.inc distclean
	$(MAKE) -C payloads/external/Memtest86Plus -f Makefile.inc distclean

.phony: clean-payloads distclean-payloads
