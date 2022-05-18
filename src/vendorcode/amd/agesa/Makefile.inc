subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY14) += f14
subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY15_TN) += f15tn
subdirs-$(CONFIG_CPU_AMD_AGESA_FAMILY16_KB) += f16kb

ifeq ($(CONFIG_CPU_AMD_AGESA),y)

subdirs-y += common

classes-y += libagesa
libagesa-y =
libagesa-generic-ccopts += -D__LIBAGESA__
libagesa-generic-ccopts += $(AGESA_INC) $(AGESA_AUTOINCLUDES)
libagesa-generic-ccopts += -Wno-pragma-pack -Wno-unused-but-set-variable
libagesa-generic-ccopts += -Wno-implicit-fallthrough

romstage-generic-ccopts += -Wno-pragma-pack
ramstage-generic-ccopts += -Wno-pragma-pack

AGESA_CFLAGS := -march=k8-sse3 -mtune=k8-sse3 -fno-strict-aliasing

CFLAGS_x86_32 += $(AGESA_CFLAGS)
CFLAGS_x86_64 += $(AGESA_CFLAGS)

ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
$(eval $(call create_class_compiler,libagesa,x86_32))
else
$(eval $(call create_class_compiler,libagesa,x86_64))
endif

$(obj)/libagesa.a: $$(libagesa-objs)
	@printf "    AGESA        $(subst $(obj)/,,$(@))\n"
	$(AR_libagesa) rcsDT $@ $+

romstage-libs  += $(obj)/libagesa.a
ramstage-libs  += $(obj)/libagesa.a

# buildOpts should be in libagesa
$(obj)/romstage/mainboard/$(MAINBOARDDIR)/buildOpts.o: CPPFLAGS_x86_32 += $(BUILDOPTS_INCLUDES)
$(obj)/ramstage/mainboard/$(MAINBOARDDIR)/buildOpts.o: CPPFLAGS_x86_32 += $(BUILDOPTS_INCLUDES)

$(obj)/romstage/vendorcode/amd/agesa/common/agesa-entry.o: CPPFLAGS_x86_32 += $(BUILDOPTS_INCLUDES)
$(obj)/ramstage/vendorcode/amd/agesa/common/agesa-entry.o: CPPFLAGS_x86_32 += $(BUILDOPTS_INCLUDES)

endif
