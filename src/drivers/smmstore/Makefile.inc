ramstage-$(CONFIG_SMMSTORE) += store.c

ifeq ($(CONFIG_SMMSTORE),y)
$(call src-to-obj,ramstage,$(dir)/store.c) : $(obj)/fmap_config.h
$(call src-to-obj,smm,$(dir)/store.c) : $(obj)/fmap_config.h
endif
ramstage-$(CONFIG_SMMSTORE_V2) += ramstage.c

smm-$(CONFIG_SMMSTORE) += store.c smi.c
