additional-dirs += $(objutil)/futility

$(objutil)/futility/build/futility/futility:
	@printf "    MAKE       $(subst $(objutil)/,,$(@))\n"
	$(MAKE) -C $(VB_SOURCE) \
		BUILD=$(abspath $@/../..) \
		CC="$(HOSTCC)" \
		V=$(V) \
		$(abspath $@)

$(objutil)/futility/futility: $(objutil)/futility/build/futility/futility
	cp $< $@.tmp
	mv $@.tmp $@
