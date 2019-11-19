##
## This file is part of the coreboot project.
##
## Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
## Copyright (C) 2018 Intel Corp.
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

ifeq ($(CONFIG_UEFI_2_4_BINDING),y)
# ProccessorBind.h provided in Ia32 directory. Types are derived from ia32.
# It's possible to provide our own ProcessorBind.h using posix types. However,
# ProcessorBind.h isn't just about types. There's compiler definitions as well
# as ABI enforcement. Luckily long is not used in Ia32/ProcessorBind.h for
# a fixed width type.
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/uefi_2.4/MdePkg/Include/Ia32
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/uefi_2.4/MdePkg/Include
else ifeq ($(CONFIG_UDK_2015_BINDING),y)
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2015/MdePkg/Include/Ia32
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2015/MdePkg/Include
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2015/IntelFsp2Pkg/Include
else ifeq ($(CONFIG_UDK_2017_BINDING),y)
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2017/MdePkg/Include/Ia32
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2017/MdePkg/Include
CPPFLAGS_common += -I$(src)/vendorcode/intel/edk2/UDK2017/IntelFsp2Pkg/Include
endif
