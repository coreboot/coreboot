additional-dirs += $(objutil)/futility

$(objutil)/futility/build/futility/futility:
	@printf "    MAKE       $(subst $(objutil)/,,$(@))\n"
	unset CFLAGS LDFLAGS; $(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$(abspath $@/../..) \
		CC="$(HOSTCC)" \
		$(if $(HOSTPKGCONFIG), PKG_CONFIG="$(HOSTPKGCONFIG)") \
		V=$(V) \
		$(abspath $@)

$(objutil)/futility/futility: $(objutil)/futility/build/futility/futility
	cp $< $@.tmp
	mv $@.tmp $@
