## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_DRIVER_LENOVO_SERIALS) += at24rf08c.c
$(call src-to-obj,ramstage,$(dir)/lenovo_serials.c) : $(obj)/build.h
ramstage-$(CONFIG_DRIVER_LENOVO_SERIALS) += lenovo_serials.c
