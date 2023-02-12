all-$(CONFIG_SMMSTORE) += store.c

ifeq ($(CONFIG_SMMSTORE),y)
$(call src-to-obj,bootblock,$(dir)/store.c) : $(obj)/fmap_config.h
$(call src-to-obj,verstage,$(dir)/store.c) : $(obj)/fmap_config.h
$(call src-to-obj,romstage,$(dir)/store.c) : $(obj)/fmap_config.h
$(call src-to-obj,postcar,$(dir)/store.c) : $(obj)/fmap_config.h
$(call src-to-obj,ramstage,$(dir)/store.c) : $(obj)/fmap_config.h
$(call src-to-obj,smm,$(dir)/store.c) : $(obj)/fmap_config.h
endif
ramstage-$(CONFIG_SMMSTORE_V2) += ramstage.c

smm-$(CONFIG_SMMSTORE) += store.c smi.c
