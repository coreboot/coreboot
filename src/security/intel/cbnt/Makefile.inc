ifeq ($(CONFIG_INTEL_CBNT_SUPPORT),y)

all-y += logging.c
ramstage-y += cmos.c

# As specified in Intel Trusted Execution Technology and Boot Guard Server BIOS
# Specification, document number # 558294
PK_HASH_ALG_SHA1:=4
PK_HASH_ALG_SHA256:=11
PK_HASH_ALG_SHA384:=12

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

CBNT_PROV:=$(obj)/cbnt-prov
CBNT_CFG:=$(obj)/cbnt.json

ifneq ($(CONFIG_INTEL_CBNT_PROV_EXTERNAL_BIN),y)
$(CBNT_PROV):
	printf "    CBNT_PROV    building tool\n"
	cd 3rdparty/intel-sec-tools; \
	GO111MODULE=on go mod download; \
	GO111MODULE=on go mod verify; \
	GO111MODULE=on go build -o $(abspath $@) cmd/cbnt-prov/*.go
else
$(CBNT_PROV): $(call strip_quotes, $(CONFIG_INTEL_CBNT_PROV_EXTERNAL_BIN_PATH))
	cp $< $@
endif

$(CBNT_CFG): $(call strip_quotes, $(CONFIG_INTEL_CBNT_CBNT_PROV_CFG_FILE))
	cp $(CONFIG_INTEL_CBNT_CBNT_PROV_CFG_FILE) $@

ifeq ($(CONFIG_INTEL_CBNT_GENERATE_BPM),y)
ifeq ($(CONFIG_INTEL_CBNT_CBNT_PROV_BPM_USE_CFG_FILE),y)
$(obj)/bpm_unsigned.bin: $(obj)/coreboot.pre $(CBNT_PROV) $(CBNT_CFG)
	printf "    CBNT_PROV    creating unsigned BPM using config file\n"
	$(CBNT_PROV) bpm-gen $@ $< --config=$(CBNT_CFG) --cut
else
$(obj)/bpm_unsigned.bin: $(obj)/coreboot.pre $(CBNT_PROV) set_fit_ptr
	printf "    CBNT_PROV    creating unsigned BPM\n"
	$(CBNT_PROV) bpm-gen $@ $< --revision=$(CONFIG_INTEL_CBNT_BPM_REVISION) \
				 --svn=$(CONFIG_INTEL_CBNT_BPM_SVN) \
				 --acmsvn=$(CONFIG_INTEL_CBNT_ACM_SVN) \
				 --nems=$(CONFIG_INTEL_CBNT_NUM_NEM_PAGES) \
				 --pbet=$(CONFIG_INTEL_CBNT_PBET) \
				 --ibbflags=$(CONFIG_INTEL_CBNT_IBB_FLAGS) \
				 --entrypoint=$(shell printf "%d" 0xfffffff0) \
				 --ibbhash=$(PK_HASH_ALG_SHA256),$(PK_HASH_ALG_SHA1),$(PK_HASH_ALG_SHA384) \
				 --sinitmin=$(CONFIG_INTEL_CBNT_SINIT_SVN) \
				 --txtflags=0 \
				 --powerdowninterval=$(CONFIG_INTEL_CBNT_PD_INTERVAL) \
				 --acpibaseoffset=$(shell printf "%d" $(CONFIG_INTEL_ACPI_BASE_ADDRESS)) \
				 --powermbaseoffset=$(shell printf "%d" $(CONFIG_INTEL_PCH_PWRM_BASE_ADDRESS)) \
				 --cmosoff0=$(shell printf "%d" $(CONFIG_INTEL_CBNT_CMOS_OFFSET)) \
				 --cmosoff1=$(call int-add, $(CONFIG_INTEL_CBNT_CMOS_OFFSET) 1) \
				 --cut \
				 --out=$(obj)/bpm_cfg.json
endif

ifeq ($(CONFIG_INTEL_CBNT_BPM_ONLY_UNSIGNED),y)
build_complete:: $(obj)/bpm_unsigned.bin
	@printf "\n** WARNING **\n"
	@printf "Build generated an unsigned BPM image: build/bpm_unsigned.bin.\n"
	@printf "The resulting image will not work with CBnT.\n"
	@printf "After you have externally signed the image you can add it to the coreboot image:\n"
	@printf "$$ cbfstool build/coreboot.rom add -f bpm.bin -n boot_policy_manifest.bin -t raw -a 16\n"
	@printf "$$ ifittool -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s 12 -f build/coreboot.rom\n"
else
$(obj)/bpm.bin: $(obj)/bpm_unsigned.bin $(CBNT_PROV) $(call strip_quotes, $(CONFIG_INTEL_CBNT_BPM_PRIV_KEY_FILE))
	printf "    CBNT_PROV    signing real BPM\n"
	$(CBNT_PROV) bpm-sign $< $@ $(CONFIG_INTEL_CBNT_BPM_PRIV_KEY_FILE) ""

# Add BPM at the end of the build when all files have been added
$(call add_intermediate, add_bpm, $(obj)/bpm.bin)
	printf "    CBNT       Adding BPM\n"
	-$(CBFSTOOL) $< remove -n boot_policy_manifest.bin 2>/dev/null
	$(CBFSTOOL) $< add -f $(obj)/bpm.bin -n boot_policy_manifest.bin -a 0x10 -t raw

$(call add_intermediate, fit_bpm, set_fit_ptr add_bpm $(IFITTOOL))
	printf "    IFITTOOL   Adding BPM\n"
	$(IFITTOOL) -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<

endif # CONFIG_INTEL_CBNT_BPM_ONLY_UNSIGNED

else # CONFIG_INTEL_CBNT_GENERATE_BPM

ifneq ($(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY),"")
cbfs-files-y += boot_policy_manifest.bin
boot_policy_manifest.bin-file := $(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY)
boot_policy_manifest.bin-type := raw
boot_policy_manifest.bin-align := 0x10

$(call add_intermediate, add_bpm_fit, $(IFITTOOL) set_fit_ptr)
	$(IFITTOOL) -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif
endif # CONFIG_INTEL_CBNT_GENERATE_BPM

ifeq ($(CONFIG_INTEL_CBNT_GENERATE_KM),y)
ifeq ($(CONFIG_INTEL_CBNT_CBNT_PROV_KM_USE_CFG_FILE),y)
$(obj)/km_unsigned.bin: $(obj)/km_pub.pem $(CBNT_PROV) $(CBNT_CFG)
	printf "    CBNT_PROV    creating unsigned KM using config file\n"
	$(CBNT_PROV) km-gen $@ $< --config=$(CBNT_CFG)
else
$(obj)/km_unsigned.bin: $(obj)/km_pub.pem $(obj)/bpm_pub.pem $(CBNT_PROV)
	printf "    CBNT_PROV    creating unsigned KM\n"
	$(CBNT_PROV) km-gen $@ $< --revision=$(CONFIG_INTEL_CBNT_KM_REVISION) \
				--svn=$(CONFIG_INTEL_CBNT_KM_SVN) \
				--id=$(CONFIG_INTEL_CBNT_KM_ID) \
				--pkhashalg=$(PK_HASH_ALG_SHA256) \
				--bpmpubkey=$(obj)/bpm_pub.pem \
				--bpmhashalgo=$(PK_HASH_ALG_SHA256) \
				--out=$(obj)/km_cfg.json
endif

$(obj)/km.bin: $(obj)/km_unsigned.bin $(CBNT_PROV) $(call strip_quotes, $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE))
	printf "    CBNT_PROV    signing KM\n"
	$(CBNT_PROV) km-sign $< $@ $(CONFIG_INTEL_CBNT_KM_PRIV_KEY_FILE) ""

KM_FILE=$(obj)/km.bin
else
KM_FILE=$(CONFIG_INTEL_CBNT_KEY_MANIFEST_BINARY)
endif

ifneq ($(KM_FILE),"")
ifeq ($(CONFIG_INTEL_CBNT_KM_ONLY_UNSIGNED),y)
$(call add_intermediate, gen_unsigned_km, $(obj)/km_unsigned.bin)
	@printf "Generating unsgined KM\n"

build_complete::
	@printf "\n** WARNING **\n"
	@printf "Build generated an unsigned KM image: build/km_unsiged.bin.\n"
	@printf "The resulting image will not work with CBnT.\n"
	@printf "After you have externally signed the image you can add it to the coreboot image:\n"
	@printf "$$ cbfstool build/coreboot.rom add -f km.bin -n key_manifest.bin -t raw -a 16\n"
	@printf "$$ ifittool -r COREBOOT -a -n key_manifest.bin -t 11 -s 12 -f build/coreboot.rom\n"

else
cbfs-files-y += key_manifest.bin
key_manifest.bin-file := $(KM_FILE)
key_manifest.bin-type := raw
key_manifest.bin-align := 0x10

$(call add_intermediate, add_km_fit, $(IFITTOOL) set_fit_ptr)
	$(IFITTOOL) -r COREBOOT -a -n key_manifest.bin -t 11 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif

endif # CONFIG_INTEL_CBNT_KM_ONLY_UNSIGNED
endif # CONFIG_INTEL_CBNT_SUPPORT
