# SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_VENDOR_STARLABS) += cfr
subdirs-$(CONFIG_VENDOR_STARLABS) += pin_mux
subdirs-$(CONFIG_VENDOR_STARLABS) += smbios

CPPFLAGS_common += -I$(src)/mainboard/starlabs/common/include
