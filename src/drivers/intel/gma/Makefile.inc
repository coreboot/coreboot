## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_INTEL_DDI) += intel_ddi.c
ramstage-$(CONFIG_INTEL_EDID) += edid.c vbt.c
ifeq ($(CONFIG_VGA_ROM_RUN),y)
ramstage-$(CONFIG_INTEL_INT15) += int15.c
endif
ramstage-$(CONFIG_INTEL_GMA_ACPI) += acpi.c
ramstage-$(CONFIG_INTEL_GMA_ACPI) += opregion.c

ifeq ($(CONFIG_INTEL_GMA_ADD_VBT),y)
# add_vbt_to_cbfs, first argument is the filename in cbfs, the second one
# is the filename in the coreboot tree.
add_vbt_to_cbfs= \
	$(eval cbfs-files-y += $1) \
	$(eval $1-file := $2) \
	$(eval $1-type := raw) \
	$(eval $1-compression := lzma)
endif

$(call add_vbt_to_cbfs, vbt.bin, $(call strip_quotes,$(CONFIG_INTEL_GMA_VBT_FILE)))

ifneq (,$(filter y, $(CONFIG_GFX_GMA) $(CONFIG_EARLY_GFX_GMA)))

gfx-gma-stages = \
	$(if $(filter y,$(CONFIG_EARLY_GFX_GMA)),romstage) \
	$(if $(filter y,$(CONFIG_GFX_GMA)),ramstage) \

$(call add-special-class,gfxinit)
gfxinit-handler +=$(foreach stage, $(gfx-gma-stages), \
	$(eval $(stage)-srcs += $$(addprefix $(1),$(2))))

$(call add-special-class,gfxinit-gen)
gfxinit-gen-handler += \
	$(eval additional-dirs += $(dir $(2))) \
	$(foreach stage, $(gfx-gma-stages), \
		$(eval $(stage)-srcs += $(2)) \
		$(eval $(stage)-ads-deps += $(2)) \
		$(eval $(stage)-adb-deps += $(2))) \
	$(eval $(2): $(obj)/config.h)

ifeq ($(CONFIG_GFX_GMA_DEFAULT_MMIO),)
CONFIG_GFX_GMA_DEFAULT_MMIO := 0 # dummy, will be overwritten at runtime
else
$(call to-ada-hex,CONFIG_GFX_GMA_DEFAULT_MMIO)
endif

subdirs-y += ../../../../3rdparty/libgfxinit

$(foreach stage,$(gfx-gma-stages), \
	$(eval $(stage)-y += gma.ads gma.adb))

endif # CONFIG_GFX_GMA || CONFIG_EARLY_GFX_GMA

ifeq ($(CONFIG_GFX_GMA),y)

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-gfx_init.ads
ifeq ($(CONFIG_LINEAR_FRAMEBUFFER),y)
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += hires_fb/gma-gfx_init.adb
else
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += text_fb/gma-gfx_init.adb
endif

endif # CONFIG_GFX_GMA

ifeq ($(CONFIG_EARLY_GFX_GMA),y)

romstage-$(CONFIG_MAINBOARD_USE_EARLY_LIBGFXINIT) += \
	gma-gfx_init.ads \
	text_fb/gma-gfx_init.adb

endif # CONFIG_EARLY_GFX_GMA
