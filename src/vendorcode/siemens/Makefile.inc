## SPDX-License-Identifier: GPL-2.0-only

subdirs-$(CONFIG_USE_SIEMENS_HWILIB) += hwilib

CPPFLAGS_common += -I$(src)/vendorcode/siemens/hwilib
