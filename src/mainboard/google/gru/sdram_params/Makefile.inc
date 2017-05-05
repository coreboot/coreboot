##
## This file is part of the coreboot project.
##
## Copyright 2017 Rockchip Inc.
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

sdram-params :=
sdram-params += sdram-lpddr3-hynix-4GB-928

sdram-params += sdram-lpddr3-micron-2GB-800
sdram-params += sdram-lpddr3-micron-2GB-928
sdram-params += sdram-lpddr3-micron-4GB-800
sdram-params += sdram-lpddr3-micron-4GB-928

sdram-params += sdram-lpddr3-samsung-2GB-24EB-800
sdram-params += sdram-lpddr3-samsung-2GB-24EB-928
sdram-params += sdram-lpddr3-samsung-4GB-04EB-800
sdram-params += sdram-lpddr3-samsung-4GB-04EB-928

$(foreach params,$(sdram-params), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(params).c:struct) \
	$(eval $(params)-type := struct) \
	$(eval $(params)-compression := $(CBFS_COMPRESS_FLAG)) \
)
