## SPDX-License-Identifier: GPL-2.0-only

# Add files spd_0.bin to spd_20.bin to the cbfs image
SPD_BINS := $(shell seq 0 20)
define SPD_template
cbfs-files-y += spd_$(1).bin
spd_$(1).bin-file := spd/spd_$(1).bin
spd_$(1).bin-type := raw
endef
$(foreach n,$(SPD_BINS),$(eval $(call SPD_template,$(n))))
