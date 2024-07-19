## SPDX-License-Identifier: GPL-2.0-only

ppc64_flags = -I$(src)/arch/ppc64/ -mbig-endian -mcpu=power8 -mtune=power8

ppc64_asm_flags =

################################################################################
## bootblock
################################################################################
ifeq ($(CONFIG_ARCH_BOOTBLOCK_PPC64),y)

bootblock-y = bootblock_crt0.S
bootblock-y += arch_timer.c
bootblock-y += boot.c
bootblock-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

bootblock-generic-ccopts += $(ppc64_flags)

$(eval $(call link_stage,bootblock))

endif

################################################################################
## romstage
################################################################################
ifeq ($(CONFIG_ARCH_ROMSTAGE_PPC64),y)

romstage-y += arch_timer.c
romstage-y += boot.c
romstage-y += stages.c
romstage-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

# Build the romstage

$(eval $(call link_stage,romstage))

romstage-c-ccopts += $(ppc64_flags)
romstage-S-ccopts += $(ppc64_asm_flags)

endif

################################################################################
## ramstage
################################################################################
ifeq ($(CONFIG_ARCH_RAMSTAGE_PPC64),y)

ramstage-y += stages.c
ramstage-y += arch_timer.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += \
	$(top)/src/lib/memchr.c \
	$(top)/src/lib/memcmp.c \
	$(top)/src/lib/memcpy.c \
	$(top)/src/lib/memmove.c \
	$(top)/src/lib/memset.c

$(eval $(call create_class_compiler,rmodules,power8))

ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mainboard.c

# Build the ramstage

$(eval $(call link_stage,ramstage))

ramstage-c-ccopts += $(ppc64_flags)
ramstage-S-ccopts += $(ppc64_asm_flags)

endif
