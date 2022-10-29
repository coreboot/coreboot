## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_STITCH_ME_BIN),y)

objcse := $(obj)/cse
additional-dirs += $(objcse)

define cse_input_path
$(call strip_quotes,$(CONFIG_CSE_COMPONENTS_PATH))/$(call strip_quotes,$(1))
endef

define cse_add_dummy
$(eval cse_$(1)_ingredients+=$(2))
endef

define cse_add_dummy_to_bp1_bp2
$(call cse_add_dummy,bp1,$(1))
$(call cse_add_dummy,bp2,$(1))
endef

define cse_add_file
$(eval cse_$(3)_ingredients+=$(4))
$(eval file=$(2))
$(eval $(4)-file=$(file))
$(eval $(1)+=$(if $(filter $(file),$($(1))),,$(file)))
endef

define cse_add_decomp
$(call cse_add_file,cse_decomp_files,$(objcse)/$(2),$(1),$(2))
endef

define cse_add_decomp_to_bp1_bp2
$(call cse_add_decomp,bp1,$(1))
$(call cse_add_decomp,bp2,$(1))
endef

define cse_add_input
$(call cse_add_file,cse_input_files,$(call cse_input_path,$(CONFIG_CSE_$(2)_FILE)),$(1),$(2))
endef

define cse_add_input_to_bp1_bp2
$(call cse_add_input,bp1,$(1))
$(call cse_add_input,bp2,$(1))
endef

endif
