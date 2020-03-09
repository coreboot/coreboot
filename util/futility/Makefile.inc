additional-dirs += $(objutil)/futility

VBOOT_FUTILITY = $(VBOOT_HOST_BUILD)/futility/futility

$(VBOOT_FUTILITY):
	@printf "    MAKE       $(subst $(objutil)/,,$(@))\n"
	unset CFLAGS LDFLAGS; $(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$(VBOOT_HOST_BUILD) \
		CC="$(HOSTCC)" \
		$(if $(HOSTPKGCONFIG), PKG_CONFIG="$(HOSTPKGCONFIG)") \
		V=$(V) \
		$@

$(objutil)/futility/futility: $(VBOOT_FUTILITY)
	mkdir -p $(dir $@)
	cp $< $@.tmp
	mv $@.tmp $@
