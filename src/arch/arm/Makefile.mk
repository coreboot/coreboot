## SPDX-License-Identifier: GPL-2.0-only

###############################################################################
# ARM specific options
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)
check-ramstage-overlap-regions += postram_cbfs_cache stack ttb
endif

ifeq ($(CONFIG_ARCH_ARM),y)
subdirs-y += libgcc/
subdirs-y += armv4/ armv7/

stages_c = $(src)/arch/arm/stages.c
stages_o = $(obj)/arch/arm/stages.o

$(stages_o): $(stages_c) $(obj)/config.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_arm) -I. $(CPPFLAGS_arm) -c -o $@ $< -marm

endif # CONFIG_ARCH_ARM

###############################################################################
# bootblock
###############################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM),y)

decompressor-y += boot.c
bootblock-y += boot.c
decompressor-y += div0.c
bootblock-y += div0.c
decompressor-y += eabi_compat.c
bootblock-y += eabi_compat.c
decompressor-y += memset.S
bootblock-y += memset.S
decompressor-y += memcpy.S
bootblock-y += memcpy.S
decompressor-y += memmove.S
bootblock-y += memmove.S

bootblock-y += clock.c
bootblock-y += stages.c

$(eval $(call link_stage,bootblock))
$(eval $(call link_stage,decompressor))

endif # CONFIG_ARCH_BOOTBLOCK_ARM

###############################################################################
# verification stage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM),y)

$(eval $(call link_stage,verstage))

verstage-y += boot.c
verstage-y += div0.c
verstage-y += eabi_compat.c
verstage-y += memset.S
verstage-y += memcpy.S
verstage-y += memmove.S
verstage-y += stages.c

endif # CONFIG_ARCH_VERSTAGE_ARM

###############################################################################
# romstage
###############################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)

romstage-y += boot.c
romstage-y += stages.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S
romstage-y += clock.c
rmodules_arm-y += memset.S
rmodules_arm-y += memcpy.S
rmodules_arm-y += memmove.S
rmodules_arm-y += eabi_compat.c

$(eval $(call link_stage,romstage))

endif # CONFIG_ARCH_ROMSTAGE_ARM

###############################################################################
# ramstage
###############################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM),y)

ramstage-y += stages.c
ramstage-y += div0.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S
ramstage-y += clock.c
ramstage-y += boot_linux.S
ramstage-$(CONFIG_PAYLOAD_FIT_SUPPORT) += fit_payload.c

rmodules_arm-y += memset.S
rmodules_arm-y += memcpy.S
rmodules_arm-y += memmove.S
rmodules_arm-y += eabi_compat.c
ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

$(eval $(call link_stage,ramstage))

endif # CONFIG_ARCH_RAMSTAGE_ARM
