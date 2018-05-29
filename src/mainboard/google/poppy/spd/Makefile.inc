
SPD_BIN = $(obj)/spd.bin
SEC_SPD_BIN = $(obj)/sec-spd.bin

define gen_spd_bin
	for f in $2; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\%o' 0x$$c); \
	  done; \
	done > $1
endef

add_spd_to_cbfs= \
	$(eval cbfs-files-y += $1) \
	$(eval $1-file := $2) \
	$(eval $1-type := spd)

ifeq ($(SPD_SOURCES),)
	SPD_DEPS := $(error SPD_SOURCES is not set. Variant must provide this)
else
	SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)
endif

# Include SPD ROM data
$(SPD_BIN): $(SPD_DEPS)
	$(call gen_spd_bin, $@, $+)

$(call add_spd_to_cbfs, spd.bin, $(SPD_BIN))

# Add optional secondary SPD ROM data if present
ifneq ($(SEC_SPD_SOURCES),)

SEC_SPD_DEPS := $(foreach f, $(SEC_SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

$(SEC_SPD_BIN): $(SEC_SPD_DEPS)
		$(call gen_spd_bin, $@, $+)

$(call add_spd_to_cbfs, sec-spd.bin, $(SEC_SPD_BIN))

endif
