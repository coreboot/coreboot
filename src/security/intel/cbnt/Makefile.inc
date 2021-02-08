ifeq ($(CONFIG_INTEL_CBNT_SUPPORT),y)

ramstage-y += cmos.c

# The private key also contains the public key, so use that if a private key is provided.
ifeq ($(CONFIG_INTEL_CBNT_NEED_KM_PRIV_KEY),y)
$(obj)/km_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE))
	openssl pkey -in $< -pubout > $@
else ifeq ($(CONFIG_INTEL_CBNT_NEED_KM_PUB_KEY),y)
$(obj)/km_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PUB_KEY_FILE))
	cp $< $@
endif

# The private key also contains the public key, so use that if a private key is provided.
ifeq ($(CONFIG_INTEL_CBNT_NEED_BPM_PRIV_KEY),y)
$(obj)/bpm_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_BPM_PRIV_KEY_FILE))
	openssl pkey -in $< -pubout > $@
else ifeq ($(CONFIG_INTEL_CBNT_NEED_BPM_PUB_KEY),y)
$(obj)/bpm_pub.pem: $(call strip_quotes, $(CONFIG_INTEL_CBNT_BPM_PUB_KEY_FILE))
	cp $< $@
endif

BG_PROV:=$(obj)/bg-prov
CBNT_CFG:=$(obj)/cbnt.json

$(BG_PROV):
	printf "    BG_PROV    building tool\n"
	cd 3rdparty/intel-sec-tools; \
	GO111MODULE=on go mod download; \
	GO111MODULE=on go mod verify; \
	GO111MODULE=on go build -o $(top)/$@ cmd/bg-prov/*.go

$(CBNT_CFG): $(call strip_quotes, $(CONFIG_INTEL_CBNT_BG_PROV_CFG_FILE))
	cp $(CONFIG_INTEL_CBNT_BG_PROV_CFG_FILE) $@

ifneq ($(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY),"")
cbfs-files-y += boot_policy_manifest.bin
boot_policy_manifest.bin-file := $(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY)
boot_policy_manifest.bin-type := raw
boot_policy_manifest.bin-align := 0x10

$(call add_intermediate, add_bpm_fit, $(IFITTOOL) set_fit_ptr)
	$(IFITTOOL) -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif

ifeq ($(CONFIG_INTEL_CBNT_GENERATE_KM),y)
$(obj)/km_unsigned.bin: $(obj)/km_pub.pem $(BG_PROV) $(CBNT_CFG)
	printf "    BG_PROV    creating unsigned KM using config file\n"
	$(BG_PROV) km-gen $@ $< --config=$(CBNT_CFG)

$(obj)/km.bin: $(obj)/km_unsigned.bin $(BG_PROV) $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE))
	printf "    BG_PROV    signing KM\n"
	$(BG_PROV) km-sign $< $@ $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE) ""

KM_FILE=$(obj)/km.bin
else
KM_FILE=$(CONFIG_INTEL_CBNT_KEY_MANIFEST_BINARY)
endif

ifneq ($(KM_FILE),"")
cbfs-files-y += key_manifest.bin
key_manifest.bin-file := $(KM_FILE)
key_manifest.bin-type := raw
key_manifest.bin-align := 0x10

$(call add_intermediate, add_km_fit, $(IFITTOOL) set_fit_ptr)
	$(IFITTOOL) -r COREBOOT -a -n key_manifest.bin -t 11 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif

endif # CONFIG_INTEL_CBNT_SUPPORT
