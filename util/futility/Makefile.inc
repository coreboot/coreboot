additional-dirs += $(objutil)/futility

VBOOT_FUTILITY = $(VBOOT_HOST_BUILD)/futility/futility

HOSTPKGCONFIG ?= pkg-config

$(VBOOT_FUTILITY): | check-openssl-presence
	@printf "    MAKE       $(subst $(objutil)/,,$(@))\n"
	unset CFLAGS LDFLAGS; $(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$(VBOOT_HOST_BUILD) \
		CC="$(HOSTCC)" \
		WERROR="-Werror -Wno-deprecated-declarations" \
		PKG_CONFIG="$(HOSTPKGCONFIG)" \
		V=$(V) \
		USE_FLASHROM=0 \
		$@

.PHONY: check-openssl-presence
check-openssl-presence:
	@$(HOSTPKGCONFIG) --exists libcrypto || \
	(echo "Error: Ensure that pkg-config and openssl's libcrypto, including header files, are installed."; exit 1)

$(objutil)/futility/futility: $(VBOOT_FUTILITY)
	mkdir -p $(dir $@)
	cp $< $@.tmp
	mv $@.tmp $@
