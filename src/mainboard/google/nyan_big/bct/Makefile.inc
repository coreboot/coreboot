##
## This file is part of the coreboot project.
##
## Copyright 2014 Google Inc.
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

bct-cfg-$(CONFIG_NYAN_BIG_BCT_CFG_EMMC) += emmc.cfg
bct-cfg-$(CONFIG_NYAN_BIG_BCT_CFG_SPI) += spi.cfg
bct-cfg-y += odmdata.cfg

# Note when SDRAM config (sdram-*.cfg) files are changed, we have to regenerate
# the include files (sdram-*.inc). See ../../nyan/bct/Makefile.inc for more
# information.
