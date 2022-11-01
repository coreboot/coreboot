# SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_NORTHBRIDGE_AMD_AGESA),y)

subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY15_TN) += family15tn
subdirs-$(CONFIG_NORTHBRIDGE_AMD_AGESA_FAMILY16_KB) += family16kb

endif
